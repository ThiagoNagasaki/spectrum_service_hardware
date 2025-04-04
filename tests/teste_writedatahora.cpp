#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include "../libraries/mcb/protocols/mcb_protocol.h"     // MCBProtocol
#include "../libraries/mcb/config/mcb_constants.h"         // MCBCommand, STX, ETX, etc.
#include "../libraries/transport/network/tcp_transport.h"  // TCPTransport, TCPConfig

// Função para controlar a esteira sem temporizador
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

bool controlarEsteira(transport::network::TCPTransport& tcpMcb,
                      mcb::protocols::MCBProtocol& mcbProt,
                      uint8_t direcao,
                      uint16_t tempo) {
    uint8_t inverterAddress = 0x10;   // Endereço do inversor da esteira
    // Monta o payload com o endereço do inversor, a direção e os 2 bytes do temporizador (big-endian)
    std::vector<uint8_t> payload = { inverterAddress, direcao,
                                     static_cast<uint8_t>(tempo >> 8),
                                     static_cast<uint8_t>(tempo & 0xFF) };
    // Usa o comando 0x69 (acionamento temporizado da esteira)
    auto frame = mcbProt.buildFrame(mcb::config::MCBCommand::WRITE_DIGITAL_TIMER, payload);
    return tcpMcb.send(frame);
}

// Função para gravar data e hora
// Estrutura do payload (comando 0x6B):
//   Byte 0: Reservado (0x00)
//   Byte 1: Segundo (00-59)
//   Byte 2: Minuto (00-59)
//   Byte 3: Hora (00-23)
//   Byte 4: Semana (1-7)
//   Byte 5: Dia (01-31)
//   Byte 6: Mês (01-12)
//   Byte 7: Ano (00-99)
//   Byte 8: Controle (0x00)
bool gravarDataHora(transport::network::TCPTransport& tcpMcb,
                    mcb::protocols::MCBProtocol& mcbProt,
                    uint8_t segundo,
                    uint8_t minuto,
                    uint8_t hora,
                    uint8_t semana,
                    uint8_t dia,
                    uint8_t mes,
                    uint8_t ano) {
    std::vector<uint8_t> payload = { 0x00, segundo, minuto, hora, semana, dia, mes, ano, 0x00 };
    auto frame = mcbProt.buildFrame(mcb::config::MCBCommand::WRITE_DATA_HORA, payload);
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
        }},
        // Gravar data/hora fixa (exemplo: 15:30:45, sexta-feira, 04/04/2025)
        {'h', [&]() {
            bool res = gravarDataHora(tcpMcb, mcbProt, 45, 30, 15, 6, 4, 4, 25);
            std::cout << (res ? "Comando enviado: Data/Hora gravada" : "Erro ao enviar comando de Data/Hora") << std::endl;
        }}
    };
    
    char opcao;
    do {
        std::cout << "\nDigite 'f' para mover para frente, 'b' para mover para trás, 's' para parar a esteira, 'h' para gravar data/hora ou 'q' para sair: ";
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
