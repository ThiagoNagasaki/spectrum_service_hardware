#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include "../libraries/mcb/protocols/mcb_protocol.h"     // MCBProtocol
#include "../libraries/mcb/config/mcb_constants.h"         // MCBCommand, STX, ETX, etc.
#include "../libraries/transport/network/tcp_transport.h"  // TCPTransport, TCPConfig

// Função para controlar a esteira
// Parâmetro "direcao":
//    0 = Parar a esteira
//    1 = Mover a esteira para frente
//    2 = Mover a esteira para trás
bool controlarEsteira(transport::network::TCPTransport& tcpMcb,
                      mcb::protocols::MCBProtocol& mcbProt,
                      uint8_t direcao) {
    uint8_t inverterAddress = 0x10;   // Endereço do inversor da esteira
    std::vector<uint8_t> payload = { inverterAddress, direcao };
    auto frame = mcbProt.buildFrame(mcb::config::MCBCommand::WRITE_DIGITAL_OUT, payload);
    return tcpMcb.send(frame);
}

int main() {
    using transport::network::TCPConfig;
    using transport::network::TCPTransport;
    using mcb::config::MCBCommand;
    
    // Configuração da conexão TCP com a MCB
    TCPConfig configMcb{"192.168.100.1", 3000};
    TCPTransport tcpMcb(configMcb);
    
    if (!tcpMcb.connect()) {
        std::cerr << "Falha ao conectar na MCB." << std::endl;
        return 1;
    }
    std::cout << "Conectado à MCB." << std::endl;
    
    mcb::protocols::MCBProtocol mcbProt;
    
    std::unordered_map<char, std::function<void()>> comandoMap = {
        // Mover para frente: valor 1
        {'f', [&]() {
            bool res = controlarEsteira(tcpMcb, mcbProt, 1);
            std::cout << (res ? "Comando enviado: Esteira para frente" : "Erro ao enviar comando") << std::endl;
        }},
        {'F', [&]() {
            bool res = controlarEsteira(tcpMcb, mcbProt, 1);
            std::cout << (res ? "Comando enviado: Esteira para frente" : "Erro ao enviar comando") << std::endl;
        }},
        // Mover para trás: valor 2
        {'b', [&]() {
            bool res = controlarEsteira(tcpMcb, mcbProt, 2);
            std::cout << (res ? "Comando enviado: Esteira para trás" : "Erro ao enviar comando") << std::endl;
        }},
        {'B', [&]() {
            bool res = controlarEsteira(tcpMcb, mcbProt, 2);
            std::cout << (res ? "Comando enviado: Esteira para trás" : "Erro ao enviar comando") << std::endl;
        }},
        // Parar a esteira: valor 0
        {'s', [&]() {
            bool res = controlarEsteira(tcpMcb, mcbProt, 0);
            std::cout << (res ? "Comando enviado: Esteira parada" : "Erro ao enviar comando") << std::endl;
        }},
        {'S', [&]() {
            bool res = controlarEsteira(tcpMcb, mcbProt, 0);
            std::cout << (res ? "Comando enviado: Esteira parada" : "Erro ao enviar comando") << std::endl;
        }}
    };
    
    char opcao;
    do {
        std::cout << "\nDigite 'f' para mover para frente, 'b' para mover para trás, 's' para parar a esteira ou 'q' para sair: ";
        std::cin >> opcao;
        if (opcao == 'q' || opcao == 'Q') {
            std::cout << "Encerrando..." << std::endl;
            break;
        }
        
        auto it = comandoMap.find(opcao);
        if (it != comandoMap.end()) {
            it->second();
        } else {
            std::cout << "Opção inválida." << std::endl;
        }
        
    } while (true);
    
    tcpMcb.disconnect();
    return 0;
}
