#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include "../enum_/enum_transportstatus.h"         // TransportStatus
#include "../interface/i_transport.h"              // ITransport
#include "../../../libraries/protocols/mcb_keyboard/mcb_keyboard_protocol.h"
#include "../../../libraries/protocols/i_protocol.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace transport::network {
using protocols::mcb_keyboard::MCBProtocol;
using protocols::mcb_keyboard::MCBFrame;
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
class TCPTransport : public interface::ITransport,public protocol::IProtocol{
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
    // IProtocol:
    std::vector<uint8_t> sendCommand(
       uint8_t cmd,
       const std::vector<uint8_t>& payload ) override;
    /// bloqueia até ler um frame completo (termina em ETX)
    std::vector<uint8_t> receive();

private:
    /**
     * \brief Classe interna (PImpl) que contém toda a lógica.
     */
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport::network

#endif // TCP_TRANSPORT_H
