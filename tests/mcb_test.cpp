#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

// IP e porta padrão da MCB
#define MCB_IP   "127.0.0.1"
#define MCB_PORT 3000

// ——— Comandos SANDT ———
constexpr uint8_t CMD_SET_KV_MA   = 0xAE;
constexpr uint8_t CMD_XRAY_ON     = 0xA1;
constexpr uint8_t CMD_XRAY_OFF    = 0xA2;
constexpr uint8_t CMD_CLEAR_ALARM = 0xC8;

// ——— I/O Digital MCB ———
constexpr uint8_t MCB_IO_WRITE = 0x63;
constexpr uint8_t MCB_IO_READ  = 0x53;

// ——— Opcodes Sandt → MCB ———
constexpr uint8_t OPCODE_GEN1 = 0x80;
constexpr uint8_t OPCODE_GEN2 = 0x81;

static int mcb_sock = -1;

// Abre conexão TCP persistente
static bool open_connection(const std::string &host) {
    if (mcb_sock >= 0) return true;
    mcb_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (mcb_sock < 0) { perror("socket"); return false; }
    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(MCB_PORT);
    if (inet_pton(AF_INET, host.c_str(), &srv.sin_addr) <= 0) {
        perror("inet_pton"); close(mcb_sock); mcb_sock = -1; return false;
    }
    if (connect(mcb_sock, (sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect"); close(mcb_sock); mcb_sock = -1; return false;
    }
    return true;
}

// Envia e recebe no socket persistente
static ssize_t persistent_send(const uint8_t *data, size_t len) {
    ssize_t n = send(mcb_sock, data, len, 0);
    if (n < 0) perror("[ERRO] send");
    return n;
}
static ssize_t persistent_recv(uint8_t *buf, size_t len) {
    ssize_t n = recv(mcb_sock, buf, len, 0);
    if (n < 0) perror("[ERRO] recv");
    return n;
}

// ——— Checksum helpers ———
static uint8_t xor_checksum(const std::vector<uint8_t> &buf, size_t b, size_t e) {
    uint8_t cs = 0;
    for (size_t i = b; i < e; ++i) cs ^= buf[i];
    return cs;
}
static uint8_t sum_checksum(const std::vector<uint8_t> &buf, size_t b, size_t e) {
    uint16_t s = 0;
    for (size_t i = b; i < e; ++i) s += buf[i];
    return static_cast<uint8_t>(s & 0xFF);
}

// ——— Frame builders ———
static std::vector<uint8_t> build_generator_frame(uint8_t cmd,
    const std::vector<uint8_t> &p = {}) {
    std::vector<uint8_t> f(15, 0);
    f[0] = 0x02; f[1] = cmd;
    for (size_t i = 0; i < p.size() && i < 12; ++i) f[2+i] = p[i];
    f[13] = xor_checksum(f,1,14);
    f[14] = 0x03;
    return f;
}
static std::vector<uint8_t> build_mcb_frame(uint8_t cmd,
    const std::vector<uint8_t> &data) {
    std::vector<uint8_t> m;
    m.push_back(0x02);
    m.push_back(static_cast<uint8_t>(1 + data.size()));
    m.push_back(cmd);
    m.insert(m.end(), data.begin(), data.end());
    m.push_back(sum_checksum(m,2,m.size()));
    m.push_back(0x03);
    return m;
}

// Forward para usar no monitor
static std::vector<uint8_t> clear_locked_alarms(const std::string &host,
                                                uint8_t opc,
                                                uint8_t mask);

// ——— Parsers ———
static void parse_io_response(const std::vector<uint8_t> &r) {
    if (r.size() >= 5 && r[3] == 0xFF)
        std::cout << "[RESP I/O] NACK\n";
    else
        std::cout << "[RESP I/O] ACK\n";
}
static void parse_generator_response(const std::vector<uint8_t> &r) {
    if (r.size() < 15) {
        std::cout << "[WARN] resp gen muito curta (" << r.size() << " bytes)\n";
        return;
    }

    std::cout << "[DEBUG r] ";
    for (size_t i = 0; i < r.size(); ++i) {
        std::printf("%02X ", r[i]);
    }
    std::cout << "\n";

 
    uint8_t year_bcd   = r[2];
    uint8_t month_bcd  = r[3];
    uint8_t day_bcd    = r[4];
    uint8_t hour_bcd   = r[5];
    uint8_t minute_bcd = r[6];
    uint8_t second_bcd = r[7];

    int    kv_out      = r[8];            // 0–255 kV
    double ma_out      = r[9] / 10.0;     // 0.0–25.5 mA
    uint8_t io_status  = r[10];
    uint8_t alarm_lock = r[11];
    uint8_t tmp        = r[12];

    // verifica que é mesmo um C8H (r[1] == CMD_CLEAR_ALARM)
    if (r[1] != CMD_CLEAR_ALARM) return;

    auto bcd2dec = [&](uint8_t v){ return (v>>4)*10 + (v&0x0F); };
    int year   = 2000 + bcd2dec(year_bcd);
    int month  = bcd2dec(month_bcd);
    int day    = bcd2dec(day_bcd);
    int hour   = bcd2dec(hour_bcd);
    int minute = bcd2dec(minute_bcd);
    int second = bcd2dec(second_bcd);

    // temperatura: two's complement, 0xFF = N/A
    bool hasTemp = (tmp != 0xFF);
    int  temp    = hasTemp ? static_cast<int>(int8_t(tmp)) : 0;

    std::cout << "[RESP gen C8] "
              << year << "/" << std::setw(2)<<month << "/" << std::setw(2)<<day
              << "  " << std::setw(2)<<hour << ":" << std::setw(2)<<minute
              << ":" << std::setw(2)<<second << "\n"
              << "  kV out=" << kv_out
              << "   mA out=" << std::fixed << std::setprecision(1) << ma_out << "\n"
              << "  I/O status=0x" << std::hex << int(io_status) << std::dec << "\n"
              << "  Alarm lock=0x" << std::hex << int(alarm_lock) << std::dec << "\n"
              << "  Temperatura= " 
              << (hasTemp ? std::to_string(temp) + " °C" : "N/A")
              << "\n";
}


// ——— clear_locked_alarms com máscara dinâmica ———
static std::vector<uint8_t> clear_locked_alarms(const std::string &host,
                                                uint8_t opc,
                                                uint8_t mask)
{
    std::vector<uint8_t> empty;
    if (!open_connection(host)) return empty;

    auto g = build_generator_frame(CMD_CLEAR_ALARM, {mask});
    auto m = build_mcb_frame(opc, g);
    persistent_send(m.data(), m.size());

    uint8_t buf[128];
    ssize_t n = persistent_recv(buf, sizeof(buf));
    if (n <= 0) return empty;
    return std::vector<uint8_t>(buf, buf + n);
}

// ——— Monitor contínuo só do gerador ———
static void monitor_generator(const std::string &host, uint8_t opc) {
    uint8_t clear_mask = 0xFF;
    while (true) {
        auto resp = clear_locked_alarms(host, opc, clear_mask);
        if (!resp.empty()) {
            const uint8_t* f = resp.data() + 3;
            clear_mask = f[2];
            std::cout << "[MONITOR] Gerador (opc=0x"
                      << std::hex << int(opc) << std::dec << "):\n";
            parse_generator_response(resp);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// ——— Envio ON/OFF sem fechar ———
static void send_only(const std::string &host,
                      const std::vector<uint8_t> &f) {
    if (!open_connection(host)) return;
    persistent_send(f.data(), f.size());
}

int main(){
    std::string host;
    std::cout << "IP da MCB (enter para " << MCB_IP << "): ";
    std::getline(std::cin, host);
    if (host.empty()) host = MCB_IP;

    if (!open_connection(host)) {
        std::cerr << "[ERRO] falha ao conectar em " << host << ":" << MCB_PORT << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "[LOG] Conexao persistente estabelecida\n";

    while (true) {
        std::cout << "\n--- MENU ---\n"
                  << "1) Ajustar kV/mA Gen1\n"
                  << "2) Ligar Gen1\n"
                  << "3) Desligar Gen1\n"
                  << "4) Status Gen1\n"
                  << "5) Ajustar kV/mA Gen2\n"
                  << "6) Ligar Gen2\n"
                  << "7) Desligar Gen2\n"
                  << "8) Status Gen2\n"
                  << "9) Saída Digital\n"
                  << "10) Entrada Digital\n"
                  << "11) Monitorar Gerador Contínuo\n"
                  << "0) Sair\n"
                  << "Escolha: ";
        int ch; std::cin >> ch;
        if (!std::cin || ch == 0) break;

        uint8_t opc = 0;
        switch (ch) {
          case 1: case 5: {
            opc = (ch == 1 ? OPCODE_GEN1 : OPCODE_GEN2);
            int kv, ma;
            std::cout<<"kV: "; std::cin>>kv;
            std::cout<<"mA: "; std::cin>>ma;
            clear_locked_alarms(host, opc, 0xFF);
            auto g = build_generator_frame(CMD_SET_KV_MA, {uint8_t(kv), uint8_t(ma)});
            auto m = build_mcb_frame(opc, g);
            persistent_send(m.data(), m.size());
            std::cout<<"[LOG] Set kV/mA enviado\n";
            break;
          }
          case 2: case 6: {
            opc = (ch == 2 ? OPCODE_GEN1 : OPCODE_GEN2);
            clear_locked_alarms(host, opc, 0xFF);
            send_only(host, build_mcb_frame(opc, build_generator_frame(CMD_XRAY_ON)));
            std::cout<<"[LOG] X-ray On enviado\n";
            break;
          }
          case 3: case 7: {
            opc = (ch == 3 ? OPCODE_GEN1 : OPCODE_GEN2);
            send_only(host, build_mcb_frame(opc, build_generator_frame(CMD_XRAY_OFF)));
            std::cout<<"[LOG] X-ray Off enviado\n";
            break;
          }
          case 4: case 8: {
            opc = (ch == 4 ? OPCODE_GEN1 : OPCODE_GEN2);
            auto resp = clear_locked_alarms(host, opc, 0xFF);
            parse_generator_response(resp);
            break;
          }
          case 9: {
            int addr, val;
            std::cout<<"End. I/O (hex): 0x"; std::cin>>std::hex>>addr>>std::dec;
            auto f = build_mcb_frame(MCB_IO_WRITE, {uint8_t(addr), uint8_t(val)});
            persistent_send(f.data(), f.size());
            std::cout<<"[LOG] Saída Digital enviada\n";
            break;
          }
          case 10: {
            int addr;
            std::cout<<"End. I/O (hex): 0x"; std::cin>>std::hex>>addr>>std::dec;
            auto f = build_mcb_frame(MCB_IO_READ, {uint8_t(addr)});
            persistent_send(f.data(), f.size());
            uint8_t buf[8]; if (persistent_recv(buf,sizeof(buf))>3)
                std::cout<<"[LOG] Entrada Digital: "<<(buf[3]?"ON\n":"OFF\n");
            break;
          }
          case 11: {
            int gen;
            std::cout<<"Gerador (1/2): "; std::cin>>gen;
            std::thread(monitor_generator, host,
                        gen==2? OPCODE_GEN2 : OPCODE_GEN1).detach();
            std::cout<<"[LOG] Monitor iniciado\n";
            break;
          }
          default:
            std::cout<<"Opcao invalida\n";
        }
    }

    close(mcb_sock);
    std::cout<<"Conexao encerrada.\n";
    return 0;
}
