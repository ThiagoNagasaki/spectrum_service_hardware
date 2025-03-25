#include <iostream>
#include <vector>
#include <string>
#include <cstdint> 

#include "../utils/logger.h"                         // Logger::instance()
#include "../utils/enum_/enum_commandcontext.h"      // CommandContext
#include "../utils/enum_/enum_errorcode.h"           // ErrorCode
#include "../libraries/transport/network/tcp_transport.h"    // TCPTransport, TCPConfig
#include "../libraries/mcb/protocols/mcb_protocol.h"         // MCBProtocol
#include "../libraries/mcb/config/mcb_constants.h"           // MCBCommand, etc.

namespace {

void printHex(const std::vector<uint8_t>& data) {
    for (auto b : data) {
        std::printf("%02X ", b);
    }
    std::printf("\n");
}

} // namespace

int main() {
    using utils::Logger;
    using utils::enum_::CommandContext;

    // 1) Inicializa o logger (Singleton)
    Logger::instance().init("logs/test_tcp.log");

    // 2) Config TCP
    transport::network::TCPConfig config;
    config.ip = "192.168.100.1";
    config.port = 3000;

    // 3) Instancia TCPTransport
    transport::network::TCPTransport tcp(config);

    std::cout << "Tentando conectar em " << config.ip << ":" << config.port << "...\n";
    if (!tcp.connect()) {
        std::cerr << "Falha ao conectar em " << config.ip << ":" << config.port << "\n";
        return 1;
    }
    std::cout << "Conectado com sucesso!\n";

    // 4) Assina callback para exibir dados recebidos em hex
    tcp.subscribe([](const std::vector<uint8_t>& incoming){
        std::cout << "[Recebido HEX] ";
        printHex(incoming);
    });

    // 5) Cria um MCBProtocol para montar frames
    mcb::protocols::MCBProtocol protocol;

    // 6) Pequeno loop de menu para enviar comandos MCB
    char opcao;
    do {
        std::cout << "\n--- MENU COMANDOS MCB ---\n";
        std::cout << "1) READ_FIRMWARE (0x51)\n";
        std::cout << "2) READ_STATUS   (0x52)\n";
        std::cout << "q) Sair\n";
        std::cout << "Escolha: ";
        std::cin >> opcao;

        if (opcao == '1') {
            auto frame = protocol.buildFrame(mcb::config::MCBCommand::READ_FIRMWARE, {});
            if (!tcp.send(frame)) {
                std::cerr << "Falha ao enviar comando READ_FIRMWARE.\n";
            } else {
                std::cout << "Comando READ_FIRMWARE enviado.\n";
            }
        }
        else if (opcao == '2') {
            auto frame = protocol.buildFrame(mcb::config::MCBCommand::READ_STATUS, {});
            if (!tcp.send(frame)) {
                std::cerr << "Falha ao enviar comando READ_STATUS.\n";
            } else {
                std::cout << "Comando READ_STATUS enviado.\n";
            }
        }
        else if (opcao == 'q') {
            std::cout << "Encerrando...\n";
        }
        else {
            std::cout << "Opção inválida.\n";
        }
    } while (opcao != 'q');

    // 7) Desconecta
    tcp.disconnect();
    std::cout << "Desconectado.\n";

    return 0;
}
