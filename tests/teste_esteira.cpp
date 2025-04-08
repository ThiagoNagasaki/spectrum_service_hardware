#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Ajuste os caminhos conforme sua estrutura:
#include "../libraries/transport/network/tcp_transport.h"  // TCPTransport, TCPConfig
#include "../libraries/mcb/protocols/mcb_protocol.h"         // MCBProtocol
#include "../libraries/mcb/config/mcb_constants.h"           // MCBCommand, STX, ETX, MCB_MIN_FRAME_SIZE, etc.
#include "../libraries/mcb/decoder/mcb_decoder.h"            // MCBDecoder

using namespace std;
using namespace transport::network;
using namespace mcb::protocols;
using namespace mcb::config;
using namespace mcb::decoder;

// Função para exibir um vetor de bytes em hexadecimal (útil para debug)
void printHex(const vector<uint8_t>& data) {
    ostringstream oss;
    for (auto byte : data) {
        oss << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
    }
    cout << oss.str() << endl;
}

// Função para gravar data/hora na MCB (comando 0x6B)
// Payload: { 0x00, segundo, minuto, hora, dia da semana, dia, mês, ano, 0x00 }
bool writeDataHora(TCPTransport& tcp, MCBProtocol& mcbProt) {
    // Exemplo fixo: 15:30:45, quarta-feira (3), dia 15, mês 09, ano 25
    vector<uint8_t> payload = { 0x00, 45, 30, 15, 3, 15, 9, 25, 0x00 };
    auto frame = mcbProt.buildFrame(MCBCommand::WRITE_DATA_HORA, payload);
    cout << "Enviando frame para WRITE_DATA_HORA:" << endl;
    printHex(frame);
    return tcp.send(frame);
}

// Função para ler data/hora da MCB (comando 0x5B – READ_DATA_HORA)
// Se o comando READ_DATA_HORA não estiver definido no enum, usamos o cast para 0x5B.
bool readDataHora(TCPTransport& tcp, MCBProtocol& mcbProt) {
    auto frame = mcbProt.buildFrame(static_cast<MCBCommand>(0x5B), {});
    cout << "Enviando frame para READ_DATA_HORA:" << endl;
    printHex(frame);
    return tcp.send(frame);
}

// Callback para processar os frames recebidos da MCB e exibir a resposta
void onMCBDataReceived(const vector<uint8_t>& incoming, MCBDecoder& decoder) {
    cout << "[RAW MCB DATA RECEBIDO] ";
    printHex(incoming);
    static vector<uint8_t> buffer;
    buffer.insert(buffer.end(), incoming.begin(), incoming.end());
    while (buffer.size() >= MCB_MIN_FRAME_SIZE) {
        if (buffer.front() != STX) {
            auto it = find(buffer.begin() + 1, buffer.end(), STX);
            if (it == buffer.end()) {
                buffer.clear();
                break;
            } else {
                buffer.erase(buffer.begin(), it);
            }
        }
        uint8_t length = buffer[1];
        size_t totalSize = length + 4; // STX + Length + (Comando+Payload) + ETX
        if (buffer.size() < totalSize)
            break;
        if (buffer[totalSize - 1] != ETX) {
            buffer.erase(buffer.begin());
            continue;
        }
        vector<uint8_t> frame(buffer.begin(), buffer.begin() + totalSize);
        buffer.erase(buffer.begin(), buffer.begin() + totalSize);
        string decoded = decoder.decodeFrame(frame);
        cout << "[MCB FRAME DECODIFICADO] " << decoded << endl;
    }
}

int main() {
    // Configuração da conexão TCP com a MCB (exemplo: IP 192.168.100.1 e porta 3000)
    TCPConfig config{"127.0.0.1", 3000};
    TCPTransport tcp(config);

    cout << "Conectando à MCB em " << config.ip << ":" << config.port << "..." << endl;
    if (!tcp.connect()) {
        cerr << "Falha ao conectar na MCB." << endl;
        return 1;
    }
    cout << "Conectado à MCB." << endl;

    // Cria instância do protocolo e do decodificador
    MCBProtocol mcbProt;
    MCBDecoder decoder;

    // Registra callback para processar os frames recebidos
    tcp.subscribe([&](const vector<uint8_t>& incoming) {
        onMCBDataReceived(incoming, decoder);
    });

    // Envia o comando de gravação de data/hora
    if (!writeDataHora(tcp, mcbProt)) {
        cerr << "Erro ao enviar comando WRITE_DATA_HORA." << endl;
    } else {
        cout << "Comando WRITE_DATA_HORA enviado." << endl;
    }

    // Aguarda alguns segundos para que o dispositivo processe o comando
    this_thread::sleep_for(chrono::seconds(2));

    // Envia o comando de leitura de data/hora
    if (!readDataHora(tcp, mcbProt)) {
        cerr << "Erro ao enviar comando READ_DATA_HORA." << endl;
    } else {
        cout << "Comando READ_DATA_HORA enviado." << endl;
    }

    // Aguarda 10 segundos para receber a resposta
    cout << "Aguardando resposta por 10 segundos..." << endl;
    this_thread::sleep_for(chrono::seconds(10));

    tcp.disconnect();
    cout << "Desconectado da MCB." << endl;
    return 0;
}
