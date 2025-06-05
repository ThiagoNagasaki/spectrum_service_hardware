#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../libraries/mcb/protocols/mcb_protocol.h" 
#include "../libraries/mcb/config/mcb_constants.h" 
#include "../libraries/transport/network/tcp_transport.h" 
#include "../libraries/mcb/decoder/mcb_decoder.h" 
using namespace std;

// Converte um vetor de bytes em uma string hexadecimal (para debug)
string bytesToHex(const vector<uint8_t>& data) {
    ostringstream oss;
    for (auto byte : data) {
        oss << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
    }
    return oss.str();
}

// Função para enviar qualquer comando READ e exibir o frame enviado
bool enviarComandoRead(transport::network::TCPTransport &tcpMcb,
                        mcb::protocols::MCBProtocol &mcbProt,
                        mcb::config::MCBCommand cmd,
                        const vector<uint8_t>& payload = {}) {
    auto frame = mcbProt.buildFrame(cmd, payload);
    cout << "[ENVIAR] Comando 0x" 
         << hex << static_cast<int>(static_cast<uint8_t>(cmd))
         << " - Frame: " << bytesToHex(frame) << dec << endl;
    return tcpMcb.send(frame);
}

// Callback para recepção de dados da MCB que utiliza o decoder
void onMCBDataReceived(const vector<uint8_t>& incoming, mcb::decoder::MCBDecoder &decoder) {
    cout << "[RAW MCB DATA RECEBIDO] " << bytesToHex(incoming) << endl;

    static vector<uint8_t> buffer;
    // Acumula os dados recebidos (no caso de frames fragmentados)
    buffer.insert(buffer.end(), incoming.begin(), incoming.end());
    
    // Processa o buffer enquanto houver dados suficientes para um frame completo
    while (buffer.size() >= mcb::config::MCB_MIN_FRAME_SIZE) {
        if (buffer.front() != mcb::config::STX) {
            auto it = find(buffer.begin() + 1, buffer.end(), mcb::config::STX);
            if (it == buffer.end()) {
                buffer.clear();
                break;
            } else {
                buffer.erase(buffer.begin(), it);
                if (buffer.size() < mcb::config::MCB_MIN_FRAME_SIZE)
                    break;
            }
        }
        uint8_t length = buffer[1];
        size_t totalSize = length + 4;  // STX + Length + (Cmd + Payload) + Checksum + ETX
        if (buffer.size() < totalSize)
            break;
        if (buffer[totalSize - 1] != mcb::config::ETX) {
            cout << "[DEBUG] ETX inválido detectado, descartando byte." << endl;
            buffer.erase(buffer.begin());
            continue;
        }
        vector<uint8_t> frame(buffer.begin(), buffer.begin() + totalSize);
        buffer.erase(buffer.begin(), buffer.begin() + totalSize);
        
        string decoded = decoder.decodeFrame(frame);
        cout << "[MCB RESPONSE DECODIFICADO] " << decoded << endl;
    }
}

int main() {
    using transport::network::TCPConfig;
    using transport::network::TCPTransport;
    using mcb::config::MCBCommand;
    
    // Configuração da conexão TCP com a MCB – ajuste IP e porta conforme o ambiente real
    TCPConfig config{"127.0.0.1", 3000};
    TCPTransport tcpMcb(config);
    
    cout << "Conectando à MCB em " << config.ip << ":" << config.port << "..." << endl;
    if (!tcpMcb.connect()) {
        cerr << "Falha ao conectar na MCB." << endl;
        return 1;
    }
    cout << "Conectado à MCB." << endl;
    
    mcb::protocols::MCBProtocol mcbProt;
    mcb::decoder::MCBDecoder mcbDecoder;
    
    // Registra o callback para processar as respostas recebidas da MCB
    tcpMcb.subscribe([&](const vector<uint8_t>& incoming) {
        onMCBDataReceived(incoming, mcbDecoder);
    });
    
    // Mapeia as opções do menu para cada comando READ
    unordered_map<char, function<bool()>> comandoMap = {
        // 0x51 - READ_FIRMWARE (sem payload)
        {'1', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_FIRMWARE); }},
        // 0x52 - READ_STATUS (sem payload)
        {'2', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_STATUS); }},
        // 0x53 - READ_INPUT_DIGITAL (exemplo: endereço 0x10)
        {'3', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_INPUT_DIGITAL, {0x10}); }},
        // 0x54 - READ_ANALOG_INPUT (exemplo: endereço 0x01)
        {'4', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_ANALOG_INPUT, {0x01}); }},
        // 0x55 - READ_SENSOR_DISTANCE (exemplo: 0x00 para distância)
        {'5', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_SENSOR_DISTANCE, {0x00}); }},
        // 0x56 - READ_STATUS_SPECIAL (sem payload)
        {'6', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_STATUS_SPECIAL); }},
        // 0x5A - READ_CHAVE_LIGA (sem payload)
        {'7', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_CHAVE_LIGA); }},
        // 0x5B - READ_DATA_HORA (sem payload)
        {'8', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_DATA_HORA); }},
        // 0x5C - READ_MAC_PC (sem payload)
        {'9', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_MAC_PC); }},
        // 0x5D - READ_INFO1 (sem payload)
        {'a', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_INFO1); }},
        // 0x5E - READ_INFO2 (sem payload)
        {'b', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_INFO2); }},
        // 0x5F - READ_INFO3 (sem payload)
        {'c', [&]() { return enviarComandoRead(tcpMcb, mcbProt, MCBCommand::READ_INFO3); }}
    };

    char opcao;
    do {
        cout << "\n===== MENU DE COMANDOS READ =====" << endl;
        cout << "1: READ_FIRMWARE (0x51)" << endl;
        cout << "2: READ_STATUS   (0x52)" << endl;
        cout << "3: READ_INPUT_DIGITAL (0x53, exemplo endereço 0x10)" << endl;
        cout << "4: READ_ANALOG_INPUT  (0x54, exemplo endereço 0x01)" << endl;
        cout << "5: READ_SENSOR_DISTANCE (0x55, exemplo 0x00)" << endl;
        cout << "6: READ_STATUS_SPECIAL (0x56)" << endl;
        cout << "7: READ_CHAVE_LIGA     (0x5A)" << endl;
        cout << "8: READ_DATA_HORA      (0x5B)" << endl;
        cout << "9: READ_MAC_PC         (0x5C)" << endl;
        cout << "a: READ_INFO1          (0x5D)" << endl;
        cout << "b: READ_INFO2          (0x5E)" << endl;
        cout << "c: READ_INFO3          (0x5F)" << endl;
        cout << "q: Sair" << endl;
        cout << "Opção: ";
        cin >> opcao;
        if (opcao == 'q' || opcao == 'Q') {
            cout << "Encerrando..." << endl;
            break;
        }
        auto it = comandoMap.find(opcao);
        if (it != comandoMap.end()) {
            bool res = it->second();
            cout << (res ? "Comando enviado com sucesso." : "Erro no envio do comando.") << endl;
        } else {
            cout << "Opção inválida." << endl;
        }
        // Aguarda alguns segundos para que a resposta seja recebida e processada
        this_thread::sleep_for(chrono::seconds(5));
    } while (true);
    
    tcpMcb.disconnect();
    return 0;
}
