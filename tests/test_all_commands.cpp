#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <exception>
#include <sstream>
#include <algorithm>

// Ajuste os caminhos conforme sua estrutura:
#include "../utils/logger.h"                           // Logger Singleton
#include "../utils/enum_/enum_commandcontext.h"        // CommandContext
#include "../utils/enum_/enum_errorcode.h"             // ErrorCode
#include "../libraries/transport/network/tcp_transport.h"  // TCPTransport, TCPConfig
#include "../libraries/mcb/protocols/mcb_protocol.h"         // MCBProtocol
#include "../libraries/mcb/config/mcb_constants.h"           // MCBCommand, STX, ETX, MCB_MIN_FRAME_SIZE, etc.
#include "../libraries/mcb/decoder/mcb_decoder.h"            // MCBDecoder
#include "../libraries/keyboard/protocols/keyboard_protocol.h" // KeyboardProtocol
#include "../libraries/keyboard/config/keyboard_constants.h" // Constantes do teclado (0x70...0x7B) e KBCommand
#include "../libraries/keyboard/decoder/keyboard_decoder.h"  // KeyboardDecoder

// Utilitários para conversão de strings em vetores de bytes
std::vector<uint8_t> stringToFixedBytes(const std::string& s, size_t fixedSize = 15) {
    std::vector<uint8_t> v(s.begin(), s.end());
    if (v.size() < fixedSize)
        v.resize(fixedSize, 0);
    else if (v.size() > fixedSize)
        v.resize(fixedSize);
    return v;
}

std::vector<uint8_t> stringToBytes(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}

//-------------------------------------------------------------------------------------------------
// Função para montar um frame genérico para comandos do teclado (caso não se use o protocolo)
//-------------------------------------------------------------------------------------------------
std::vector<uint8_t> buildGenericFrame(uint8_t cmd, const std::vector<uint8_t>& payload) {
    uint8_t length = static_cast<uint8_t>(1 + payload.size());
    std::vector<uint8_t> frame;
    frame.reserve(3 + payload.size());
    // Usamos o mesmo STX e ETX definidos para a MCB (são comuns)
    frame.push_back(mcb::config::STX);
    frame.push_back(length);
    frame.push_back(cmd);
    frame.insert(frame.end(), payload.begin(), payload.end());
    frame.push_back(mcb::config::ETX);
    return frame;
}

//-------------------------------------------------------------------------------------------------
// Callbacks para recepção dos frames dos módulos MCB e Teclado
//-------------------------------------------------------------------------------------------------
void onMCBDataReceived(const std::vector<uint8_t>& incoming, mcb::decoder::MCBDecoder& decoder) {
    static std::vector<uint8_t> bufferMCB;
    bufferMCB.insert(bufferMCB.end(), incoming.begin(), incoming.end());

    while (bufferMCB.size() >= mcb::config::MCB_MIN_FRAME_SIZE) {
        if (bufferMCB.front() != mcb::config::STX) {
            auto it = std::find(bufferMCB.begin() + 1, bufferMCB.end(), mcb::config::STX);
            if (it == bufferMCB.end()) {
                bufferMCB.clear();
                break;
            } else {
                bufferMCB.erase(bufferMCB.begin(), it);
                if (bufferMCB.size() < mcb::config::MCB_MIN_FRAME_SIZE)
                    break;
            }
        }
        uint8_t length = bufferMCB[1];
        size_t totalSize = length + 4; // STX + Tamanho + (Comando+Payload) + ETX
        if (bufferMCB.size() < totalSize)
            break;
        if (bufferMCB[totalSize - 1] != mcb::config::ETX) {
            bufferMCB.erase(bufferMCB.begin());
            continue;
        }
        std::vector<uint8_t> frame(bufferMCB.begin(), bufferMCB.begin() + totalSize);
        bufferMCB.erase(bufferMCB.begin(), bufferMCB.begin() + totalSize);

        std::string info = decoder.decodeFrame(frame);
        std::cout << "[MCB FRAME DECODIFICADO] " << info << std::endl;
    }
}

void onKeyboardDataReceived(const std::vector<uint8_t>& incoming, keyboard::decoder::KeyboardDecoder& decoder) {
    static std::vector<uint8_t> bufferKBD;
    bufferKBD.insert(bufferKBD.end(), incoming.begin(), incoming.end());

    // Usamos o mesmo tamanho mínimo de frame definido para a MCB
    while (bufferKBD.size() >= mcb::config::MCB_MIN_FRAME_SIZE) {
        if (bufferKBD.front() != mcb::config::STX) {
            auto it = std::find(bufferKBD.begin() + 1, bufferKBD.end(), mcb::config::STX);
            if (it == bufferKBD.end()) {
                bufferKBD.clear();
                break;
            } else {
                bufferKBD.erase(bufferKBD.begin(), it);
                if (bufferKBD.size() < mcb::config::MCB_MIN_FRAME_SIZE)
                    break;
            }
        }
        uint8_t length = bufferKBD[1];
        size_t totalSize = length + 4;
        if (bufferKBD.size() < totalSize)
            break;
        if (bufferKBD[totalSize - 1] != mcb::config::ETX) {
            bufferKBD.erase(bufferKBD.begin());
            continue;
        }
        std::vector<uint8_t> frame(bufferKBD.begin(), bufferKBD.begin() + totalSize);
        bufferKBD.erase(bufferKBD.begin(), bufferKBD.begin() + totalSize);

        std::string info = decoder.decodeFrame(frame);
        std::cout << "[TECLADO FRAME DECODIFICADO] " << info << std::endl;
    }
}

int main() {
    using utils::Logger;
    using transport::network::TCPConfig;
    using transport::network::TCPTransport;
    using mcb::config::MCBCommand;
    using keyboard::protocols::KeyboardProtocol;
    using keyboard::config::KBCommand; // Agora usamos o KBCommand para teclado

    // Inicializa o Logger (Singleton)
    Logger::instance().init("logs/test_all_commands.log");

    // Cria os objetos de protocolo e decodificação para MCB e Teclado
    mcb::protocols::MCBProtocol mcbProt;
    keyboard::protocols::KeyboardProtocol kbProt;
    mcb::decoder::MCBDecoder mcbDecoder;
    keyboard::decoder::KeyboardDecoder keyboardDecoder;

    // Configura as conexões TCP:
    // - MCB na porta 3000
    // - Teclado na porta 3001 
    TCPConfig configMcb{"127.0.0.1", 3000};
    TCPConfig configKbd{"127.0.0.1", 3001};

    TCPTransport tcpMcb(configMcb);
    TCPTransport tcpKbd(configKbd);

    std::cout << "Conectando MCB em " << configMcb.ip << ":" << configMcb.port << "...\n";
    if (!tcpMcb.connect()) {
        std::cerr << "Falha ao conectar MCB.\n";
        return 1;
    }
    std::cout << "MCB conectado!\n";

    std::cout << "Conectando Teclado em " << configKbd.ip << ":" << configKbd.port << "...\n";
    if (!tcpKbd.connect()) {
        std::cerr << "Falha ao conectar Teclado.\n";
        return 1;
    }
    std::cout << "Teclado conectado!\n";
    // Registra callbacks para processamento dos frames recebidos
    tcpMcb.subscribe([&](const std::vector<uint8_t>& incoming) {
        onMCBDataReceived(incoming, mcbDecoder);
    });
    tcpKbd.subscribe([&](const std::vector<uint8_t>& incoming) {
        onKeyboardDataReceived(incoming, keyboardDecoder);
    });

    // Lambdas para envio dos comandos
    auto sendMcb = [&](uint8_t cmd, const std::vector<uint8_t>& payload = {}) {
        auto frame = mcbProt.buildFrame(static_cast<MCBCommand>(cmd), payload);
        if (!tcpMcb.send(frame))
            std::cerr << "Falha ao enviar MCB cmd=0x" << std::hex << static_cast<int>(cmd) << "\n";
        else
            std::cout << "Enviado MCB cmd=0x" << std::hex << static_cast<int>(cmd) << "\n";
    };

    // Agora usamos o protocolo de teclado (KeyboardProtocol) para montar os frames
    auto sendKbd = [&](uint8_t cmd, const std::vector<uint8_t>& payload = {}) {
        auto frame = kbProt.buildFrame(static_cast<KBCommand>(cmd), payload);
        if (!tcpKbd.send(frame))
            std::cerr << "Falha ao enviar Teclado cmd=0x" << std::hex << static_cast<int>(cmd) << "\n";
        else
            std::cout << "Enviado Teclado cmd=0x" << std::hex << static_cast<int>(cmd) << "\n";
    };

    // Menu interativo de testes
    std::string opcao;
    do {
        std::cout << "\n===== MENU DE COMANDOS =====\n";
        std::cout << "MCB Commands:\n";
        std::cout << "  1)  READ_FIRMWARE        (0x51)\n";
        std::cout << "  2)  READ_STATUS          (0x52)\n";
        std::cout << "  3)  READ_INPUT_DIGITAL   (0x53)\n";
        std::cout << "  4)  READ_ANALOG_INPUT    (0x54)\n";
        std::cout << "  5)  READ_SENSOR_DISTANCE (0x55)\n";
        std::cout << "  6)  READ_STATUS_SPECIAL  (0x56)\n";
        std::cout << "  7)  READ_CHAVE_LIGA      (0x5A)\n";
        std::cout << "  8)  READ_DATA_HORA       (0x5B)\n";
        std::cout << "  9)  READ_MAC_PC          (0x5C)\n";
        std::cout << " 10)  READ_INFO1           (0x5D)\n";
        std::cout << " 11)  READ_INFO2           (0x5E)\n";
        std::cout << " 12)  READ_INFO3           (0x5F)\n";
        std::cout << " 13)  WRITE_SPECIAL        (0x61)\n";
        std::cout << " 14)  WRITE_CONFIG         (0x62)\n";
        std::cout << " 15)  WRITE_DIGITAL_OUT    (0x63)\n";
        std::cout << " 16)  WRITE_ANALOG_OUT     (0x64)\n";
        std::cout << " 17)  WRITE_NETWORK_CONFIG (0x65)\n";
        std::cout << " 18)  WRITE_MAC            (0x66)\n";
        std::cout << " 19)  WRITE_BAUD_UART1     (0x67)\n";
        std::cout << " 20)  WRITE_BAUD_UART2     (0x68)\n";
        std::cout << " 21)  WRITE_DIGITAL_TIMER  (0x69)\n";
        std::cout << " 22)  DESLIGA_MCB          (0x6A)\n";
        std::cout << " 23)  WRITE_DATA_HORA      (0x6B)\n";
        std::cout << " 24)  WRITE_MAC_PC         (0x6C)\n";
        std::cout << " 25)  WRITE_INFO1          (0x6D)\n";
        std::cout << " 26)  WRITE_INFO2          (0x6E)\n";
        std::cout << " 27)  WRITE_INFO3          (0x6F)\n";
        std::cout << "\nKeyboard Commands:\n";
        std::cout << " 28) TECLA PRESSIONADA    (0x70)\n";
        std::cout << " 29) VERSÃO DO TECLADO    (0x71)\n";
        std::cout << " 30) TECLA LIBERADA       (0x72)\n";
        std::cout << " 31) LED TECLA            (0x78)\n";
        std::cout << " 32) BUZZER               (0x79)\n";
        std::cout << " 33) BEEP TECLA           (0x7A)\n";
        std::cout << " 34) PISCA TECLA          (0x7B)\n";
        std::cout << " 35) HABILITAR STATUS     (0x70 payload 0x01)\n";
        std::cout << "\nq) Sair\n";
        std::cout << "Escolha: ";
        std::cin >> opcao;
        if (opcao == "q" || opcao == "Q") {
            std::cout << "Encerrando...\n";
            break;
        }
        int op = 0;
        try {
            op = std::stoi(opcao);
        } catch (const std::exception&) {
            std::cout << "Opção inválida. Tente novamente.\n";
            continue;
        }
        switch (op) {
            // Comandos MCB
            case 1:  sendMcb(0x51); break;
            case 2:  sendMcb(0x52); break;
            case 3:  sendMcb(0x53); break;
            case 4:  sendMcb(0x54); break;
            case 5:  sendMcb(0x55); break;
            case 6:  sendMcb(0x56); break;
            case 7:  sendMcb(0x5A); break;
            case 8:  sendMcb(0x5B); break;
            case 9:  sendMcb(0x5C); break;
            case 10: sendMcb(0x5D, stringToFixedBytes("TestInfo1")); break;
            case 11: sendMcb(0x5E, stringToFixedBytes("TestInfo2")); break;
            case 12: sendMcb(0x5F, stringToFixedBytes("TestInfo3")); break;
            case 13: sendMcb(0x61, stringToBytes("SpecialCmd")); break;
            case 14: sendMcb(0x62, stringToBytes("ConfigData")); break;
            case 15: sendMcb(0x63, stringToBytes("DigitalOut")); break;
            case 16: sendMcb(0x64, stringToBytes("AnalogOut")); break;
            case 17: sendMcb(0x65, stringToBytes("NetworkConfig")); break;
            case 18: sendMcb(0x66, stringToBytes("MACData")); break;
            case 19: sendMcb(0x67, stringToBytes("BaudUART1")); break;
            case 20: sendMcb(0x68, stringToBytes("BaudUART2")); break;
            case 21: sendMcb(0x69, stringToBytes("DigitalTimer")); break;
            case 22: sendMcb(0x6A); break;
            case 23: sendMcb(0x6B, stringToBytes("DataHora")); break;
            case 24: sendMcb(0x6C, stringToBytes("MAC_PC")); break;
            case 25: sendMcb(0x6D, stringToBytes("Info1Data")); break;
            case 26: sendMcb(0x6E, stringToBytes("Info2Data")); break;
            case 27: sendMcb(0x6F, stringToBytes("Info3Data")); break;
            // Comandos de teclado
            case 28: {
                std::vector<uint8_t> payload = { 0x05 };
                sendKbd(0x70, payload);
            } break;
            case 29: {
                sendKbd(0x71);
            } break;
            case 30: {
                std::vector<uint8_t> payload = { 0x05 };
                sendKbd(0x72, payload);
            } break;
            case 31: {
                uint8_t key = 0;
                std::cout << "Digite código da tecla: ";
                std::cin >> key;
                char onOff;
                std::cout << "Ligar (l) ou Desligar (d)? ";
                std::cin >> onOff;
                bool on = (onOff == 'l' || onOff == 'L');
                std::vector<uint8_t> payload = { key, static_cast<uint8_t>(on ? 1 : 0) };
                sendKbd(0x78, payload);
            } break;
            case 32: {
                uint16_t ms = 0;
                std::cout << "Digite tempo em ms para BUZZER: ";
                std::cin >> ms;
                std::vector<uint8_t> payload = {
                    static_cast<uint8_t>(ms & 0xFF),
                    static_cast<uint8_t>((ms >> 8) & 0xFF)
                };
                sendKbd(0x79, payload);
            } break;
            case 33: {
                char resp;
                std::cout << "Ativar beep? (s/n): ";
                std::cin >> resp;
                bool on = (resp == 's' || resp == 'S');
                std::vector<uint8_t> payload = { static_cast<uint8_t>(on ? 1 : 0) };
                sendKbd(0x7A, payload);
            } break;
            case 34: {
                uint8_t key = 0;
                std::cout << "Digite o código da tecla para piscar: ";
                std::cin >> key;
                std::vector<uint8_t> payload = { key, 0x01 };
                sendKbd(0x7B, payload);
            } break;
            case 35: {
                auto frameStatus = mcbProt.buildFrame(static_cast<MCBCommand>(0x70), { 0x01 });
                if (!tcpMcb.send(frameStatus))
                    std::cerr << "Falha ao enviar comando de habilitar status (0x70).\n";
                else
                    std::cout << "Comando de habilitar status (0x70) enviado.\n";
            } break;
            default:
                std::cout << "Opção inválida.\n";
                break;
        }
    } while (true);

    tcpMcb.disconnect();
    tcpKbd.disconnect();
    std::cout << "Desconectado de ambas as conexões.\n";
    return 0;
}
