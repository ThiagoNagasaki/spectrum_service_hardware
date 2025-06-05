#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include "../enum_/enum_transportstatus.h"         // TransportStatus
#include "../interface/i_transport.h"              // ITransport
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
 * - Construtor recebe uma \c TCPConfig com IP e porta.
 * - \c connect() cria o socket e conecta ao servidor.
 * - \c disconnect() encerra a conexão e a thread de leitura.
 * - \c send() envia dados de forma síncrona.
 * - \c subscribe() registra callback para dados recebidos.
 * - \c get_status() retorna o estado atual (Disconnected, Connecting, Connected, Error).
 */
class TCPTransport : public interface::ITransport {
public:
    /**
     * \brief Construtor que recebe configurações TCP (IP e porta).
     */
    explicit TCPTransport(const TCPConfig& config);

    /**
     * \brief Destrutor, garante desconexão limpa.
     */
    ~TCPTransport() override;

    bool connect() override;
    bool disconnect() override;
    bool send(const std::vector<uint8_t>& data) override;
    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) override;
    enum_::TransportStatus get_status() const override;

private:
    /**
     * \brief Classe interna (PImpl) que contém toda a lógica.
     */
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport::network

#endif // TCP_TRANSPORT_H
