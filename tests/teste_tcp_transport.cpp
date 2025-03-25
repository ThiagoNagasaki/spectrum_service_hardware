#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdio> // printf

// Ajuste os caminhos conforme sua organização:
#include "../utils/logger.h"
#include "../utils/enum_/enum_commandcontext.h"
#include "../utils/enum_/enum_errorcode.h"
#include "../libraries/transport/network/tcp_transport.h"
#include "../libraries/mcb/protocols/mcb_protocol.h"
#include "../libraries/mcb/config/mcb_constants.h"    // MCBCommand
// Se quiser macros de Keyboard (0x70, 0x78 etc.):
#include "../libraries/keyboard/config/keyboard_constants.h" // CMD_LED_TECLA, etc. (se tiver)

namespace {

/**
 * @brief Imprime dados em hex (ex.: 0x1A 0xFF ...)
 */
void printHex(const std::vector<uint8_t>& data) {
    for (auto b : data) {
        std::printf("%02X ", b);
    }
    std::printf("\n");
}

/**
 * @brief Monta frame estilo MCB/Keyboard: [STX, length, cmd, payload..., ETX].
 * 
 * @param cmd byte de comando (ex.: 0x78 para LED TECLA)
 * @param payload bytes adicionais
 * @return std::vector<uint8_t> com o frame
 */
std::vector<uint8_t> buildGenericFrame(uint8_t cmd, const std::vector<uint8_t>& payload) {
    uint8_t length = static_cast<uint8_t>(1 + payload.size()); // 1 para o cmd
    std::vector<uint8_t> frame;
    frame.reserve(3 + payload.size());
    frame.push_back(0x02); // STX
    frame.push_back(length);
    frame.push_back(cmd);
    frame.insert(frame.end(), payload.begin(), payload.end());
    frame.push_back(0x03); // ETX
    return frame;
}

} // namespace

int main() {
    using utils::Logger;
    using utils::enum_::CommandContext;
    using mcb::config::MCBCommand;

    // 1) Inicializa logger (Singleton)
    Logger::instance().init("logs/test_all_commands.log");

    // 2) Configura TCP
    transport::network::TCPConfig config;
    config.ip   = "192.168.100.1";
    config.port = 3001;

    // 3) Cria transporte
    transport::network::TCPTransport tcp(config);

    std::cout << "Tentando conectar em " << config.ip << ":" << config.port << "...\n";
    if (!tcp.connect()) {
        std::cerr << "Falha ao conectar.\n";
        return 1;
    }
    std::cout << "Conectado!\n";

    // 4) Recebe dados -> imprime em hex
    tcp.subscribe([](const std::vector<uint8_t>& incoming){
        std::cout << "[Recebido HEX] ";
        printHex(incoming);
    });

    // 5) Cria MCBProtocol p/ montar frames MCB
    mcb::protocols::MCBProtocol mcbProt;

    // Menu
    char opcao;
    do {
        std::cout << "\n--- TESTE DE COMANDOS ---\n";
        // Exemplos de MCB
        std::cout << "1) MCB - READ_FIRMWARE (0x51)\n";
        std::cout << "2) MCB - READ_STATUS   (0x52)\n";
        // Exemplos de Keyboard
        std::cout << "3) KEY - LED TECLA (0x78)\n";
        std::cout << "4) KEY - BUZZER    (0x79)\n";
        // ...
        std::cout << "q) Sair\n";
        std::cout << "Escolha: ";
        std::cin >> opcao;

        if (opcao == '1') {
            // 0x51 => READ_FIRMWARE
            auto frame = mcbProt.buildFrame(MCBCommand::READ_FIRMWARE, {});
            if (!tcp.send(frame)) {
                std::cerr << "Falha ao enviar READ_FIRMWARE\n";
            } else {
                std::cout << "Comando READ_FIRMWARE enviado.\n";
            }
        }
        else if (opcao == '2') {
            // 0x52 => READ_STATUS
            auto frame = mcbProt.buildFrame(MCBCommand::READ_STATUS, {});
            if (!tcp.send(frame)) {
                std::cerr << "Falha ao enviar READ_STATUS\n";
            } else {
                std::cout << "Comando READ_STATUS enviado.\n";
            }
        }
        else if (opcao == '3') {
            // Keyboard LED TECLA (0x78)
            // Exemplo: pergunta qual tecla e se liga ou desliga
            uint8_t keyCode;
            char onOff;
            std::cout << "Digite o codigo da tecla (ex.: 5 em decimal): ";
            std::cin >> keyCode;
            std::cout << "Ligar (l) ou desligar (d)? ";
            std::cin >> onOff;
            bool on = (onOff == 'l');

            // Monta payload: [keyCode, (on?1:0)]
            std::vector<uint8_t> payload = { keyCode, static_cast<uint8_t>(on ? 1 : 0) };
            // Monta frame
            auto frame = buildGenericFrame(0x78, payload); 
            if (!tcp.send(frame)) {
                std::cerr << "Falha ao enviar LED TECLA.\n";
            } else {
                std::cout << "Comando LED TECLA enviado.\n";
            }
        }
        else if (opcao == '4') {
            // Keyboard BUZZER (0x79)
            // Exemplo: tempo em ms
            uint16_t ms;
            std::cout << "Digite o tempo em ms: ";
            std::cin >> ms;

            // Payload = 2 bytes (LSB, MSB)
            std::vector<uint8_t> payload = {
                static_cast<uint8_t>(ms & 0xFF),
                static_cast<uint8_t>((ms >> 8) & 0xFF)
            };
            auto frame = buildGenericFrame(0x79, payload);
            if (!tcp.send(frame)) {
                std::cerr << "Falha ao enviar BUZZER.\n";
            } else {
                std::cout << "Comando BUZZER enviado.\n";
            }
        }
        else if (opcao == 'q') {
            std::cout << "Encerrando...\n";
        }
        else {
            std::cout << "Opcao invalida.\n";
        }
    } while (opcao != 'q');

    tcp.disconnect();
    std::cout << "Desconectado.\n";

    return 0;
}

