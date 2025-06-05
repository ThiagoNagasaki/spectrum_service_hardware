#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <exception>
#include <algorithm>
#include <thread>
#include <chrono>

// Ajuste os caminhos conforme sua estrutura de pastas:
#include "../utils/logger.h"                                    // Logger Singleton
#include "../libraries/transport/network/tcp_transport.h"       // TCPTransport, TCPConfig
#include "../libraries/mcb/protocols/mcb_protocol.h"            // MCBProtocol
#include "../libraries/mcb/config/mcb_constants.h"              // MCBCommand, STX, ETX, MCB_MIN_FRAME_SIZE
#include "../libraries/mcb/decoder/mcb_decoder.h"               // MCBDecoder
#include "../libraries/keyboard/protocols/keyboard_protocol.h"  // KeyboardProtocol
#include "../libraries/keyboard/config/keyboard_constants.h"    // KBCommand
#include "../libraries/keyboard/decoder/keyboard_decoder.h"     // KeyboardDecoder

// Converte string para vetor de bytes de tamanho fixo (preencher com 0 se precisar)
std::vector<uint8_t> stringToFixedBytes(const std::string& s, size_t fixedSize = 15) {
    std::vector<uint8_t> v(s.begin(), s.end());
    if (v.size() < fixedSize)
        v.resize(fixedSize, 0);
    else if (v.size() > fixedSize)
        v.resize(fixedSize);
    return v;
}

// Converte string para vetor de bytes (tamanho variável)
std::vector<uint8_t> stringToBytes(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}

// Callback para dados vindos da MCB
void onMCBDataReceived(const std::vector<uint8_t>& incoming, mcb::decoder::MCBDecoder& decoder) {
    static std::vector<uint8_t> buffer;
    buffer.insert(buffer.end(), incoming.begin(), incoming.end());

    while (buffer.size() >= mcb::config::MCB_MIN_FRAME_SIZE) {
        // procura STX
        if (buffer.front() != mcb::config::STX) {
            auto it = std::find(buffer.begin()+1, buffer.end(), mcb::config::STX);
            if (it == buffer.end()) { buffer.clear(); break; }
            buffer.erase(buffer.begin(), it);
            if (buffer.size() < mcb::config::MCB_MIN_FRAME_SIZE) break;
        }
        uint8_t length = buffer[1];
        size_t totalSize = size_t(length) + 4; // STX + len + payload+cmd + ETX
        if (buffer.size() < totalSize) break;
        if (buffer[totalSize-1] != mcb::config::ETX) {
            buffer.erase(buffer.begin());
            continue;
        }
        std::vector<uint8_t> frame(buffer.begin(), buffer.begin()+totalSize);
        buffer.erase(buffer.begin(), buffer.begin()+totalSize);

        std::string info = decoder.decodeFrame(frame);
        std::cout << "[MCB] " << info << std::endl;
    }
}

// Callback para dados vindos do Teclado
void onKeyboardDataReceived(const std::vector<uint8_t>& incoming, keyboard::decoder::KeyboardDecoder& decoder) {
    static std::vector<uint8_t> buffer;
    buffer.insert(buffer.end(), incoming.begin(), incoming.end());

    while (buffer.size() >= mcb::config::MCB_MIN_FRAME_SIZE) {
        if (buffer.front() != mcb::config::STX) {
            auto it = std::find(buffer.begin()+1, buffer.end(), mcb::config::STX);
            if (it == buffer.end()) { buffer.clear(); break; }
            buffer.erase(buffer.begin(), it);
            if (buffer.size() < mcb::config::MCB_MIN_FRAME_SIZE) break;
        }
        uint8_t length = buffer[1];
        size_t totalSize = size_t(length) + 4;
        if (buffer.size() < totalSize) break;
        if (buffer[totalSize-1] != mcb::config::ETX) {
            buffer.erase(buffer.begin());
            continue;
        }
        std::vector<uint8_t> frame(buffer.begin(), buffer.begin()+totalSize);
        buffer.erase(buffer.begin(), buffer.begin()+totalSize);

        std::string info = decoder.decodeFrame(frame);
        std::cout << "[KBD] " << info << std::endl;
    }
}

int main() {
    using utils::Logger;
    using transport::network::TCPConfig;
    using transport::network::TCPTransport;
    using mcb::config::MCBCommand;
    using mcb::protocols::MCBProtocol;
    using mcb::decoder::MCBDecoder;
    using keyboard::protocols::KeyboardProtocol;
    using keyboard::decoder::KeyboardDecoder;

    // 1) Inicializa o Logger
    Logger::instance().init("logs/mcb_status.log");

    // 2) Instancia protocolos e decodificadores
    MCBProtocol      mcbProt;
    KeyboardProtocol kbProt;
    MCBDecoder       mcbDecoder;
    KeyboardDecoder  keyboardDecoder;

    // 3) Configura conexões TCP
    TCPConfig configMcb{"192.168.100.1", 3000};
    TCPConfig configKbd{"192.168.100.1", 3001};
    TCPTransport tcpMcb(configMcb);
    TCPTransport tcpKbd(configKbd);

    // 4) Conecta
    std::cout << "Conectando MCB em " << configMcb.ip << ":" << configMcb.port << "...\n";
    if (!tcpMcb.connect()) { std::cerr << "Erro ao conectar MCB\n"; return 1; }
    std::cout << "MCB conectado!\n";

    std::cout << "Conectando Teclado em " << configKbd.ip << ":" << configKbd.port << "...\n";
    if (!tcpKbd.connect()) { std::cerr << "Erro ao conectar Teclado\n"; return 1; }
    std::cout << "Teclado conectado!\n";

    // 5) Registra callbacks
    tcpMcb.subscribe([&](auto&& in){ onMCBDataReceived(in, mcbDecoder); });
    tcpKbd.subscribe([&](auto&& in){ onKeyboardDataReceived(in, keyboardDecoder); });

    auto sendMcb = [&](uint8_t cmd, const std::vector<uint8_t>& payload = {}) {
        auto frame = mcbProt.buildFrame(static_cast<MCBCommand>(cmd), payload);
        if (!tcpMcb.send(frame))
            std::cerr << "Falha ao enviar MCB cmd=0x"
                      << std::hex << int(cmd) << std::dec << "\n";
    };
    std::thread([&](){
        while (true) {
            sendMcb(static_cast<uint8_t>(MCBCommand::READ_STATUS));
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }).detach();

    std::string opcao;
    do {
        std::cout << "\n=== MENU ===\n"
                  << "1) READ_STATUS\n"
                  << "q) Sair\n"
                  << "Escolha: ";
        std::cin >> opcao;
        if (opcao=="q"||opcao=="Q") break;
        if (opcao=="1") {
            sendMcb(static_cast<uint8_t>(MCBCommand::READ_STATUS));
        } else {
            std::cout << "Opção inválida\n";
        }
    } while (true);

    // 8) Desconecta e sai
    tcpMcb.disconnect();
    tcpKbd.disconnect();
    std::cout << "Desconectado. Encerrando.\n";
    return 0;
}
