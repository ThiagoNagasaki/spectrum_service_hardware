#pragma once

#include "../enum_/enum_transportstatus.h"
#include "../interface/i_transport.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace transport::network {

/**
 * \struct TCPConfig
 * \brief Estrutura para configurar parâmetros de conexão TCP.
 */
struct TCPConfig {
    std::string ip;
    uint16_t port;
};

/**
 * \class TCPTransport
 * \brief Implementa ITransport para comunicação via TCP/IP usando PImpl.
 *
 * - Construtor recebe uma TCPConfig com IP e porta.
 * - connect() cria o socket e conecta ao servidor.
 * - disconnect() encerra a conexão e a thread de leitura.
 * - send() envia dados de forma síncrona.
 * - subscribe() registra callback para dados recebidos.
 * - get_status() retorna o estado atual da conexão.
 */
class TCPTransport : public interface::ITransport {
public:
    explicit TCPTransport(const TCPConfig& config);
    ~TCPTransport() override;

    bool connect() override;
    bool disconnect() override;
    bool send(const std::vector<uint8_t>& data) override;
    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) override;
    enum_::TransportStatus get_status() const override;
    std::vector<uint8_t> receive();  

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport::network
