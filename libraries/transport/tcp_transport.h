#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include "i_transport.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

/**
 * \file tcp_transport.h
 * \brief Classe para comunicação via TCP/IP, seguindo PImpl e a interface ITransport.
 *
 * De acordo com o manual, a MCB disponibiliza canais TCP (p. ex. IP: 192.168.0.1, portas 3000 e 3001).
 * Aqui criamos uma classe genérica para conexão a qualquer IP/porta, podendo enviar e receber dados
 * de acordo com os protocolos definidos no firmware.
 */

namespace transport::network {

/**
 * \class TCPTransport
 * \brief Implementa a interface ITransport para prover comunicação via TCP/IP.
 *
 * - Construtor recebe IP e porta.
 * - connect() inicia a conexão como cliente.
 * - send() envia dados.
 * - Uma thread interna faz leitura contínua e chama o callback definido em set_receive_callback().
 * - disconnect() encerra a conexão e a thread de leitura.
 */
class TCPTransport : public ITransport {
public:
    /**
     * \brief Construtor que define o IP e a porta do servidor (ex.: MCB).
     * \param ip Endereço IP (ex.: "192.168.0.1")
     * \param port Porta (ex.: 3000)
     */
    TCPTransport(const std::string& ip, uint16_t port);

    /**
     * \brief Destrutor. Garante que a conexão seja fechada.
     */
    ~TCPTransport() override;

    /**
     * \brief Conecta ao servidor TCP (MCB).
     * \return true se conectou com sucesso, false se houve falha.
     */
    bool connect() override;

    /**
     * \brief Desconecta caso esteja conectado.
     * \return true se desconectou com sucesso, false se houve falha.
     */
    bool disconnect() override;

    /**
     * \brief Envia dados pela conexão TCP.
     * \param data Vetor de bytes a enviar.
     * \return true se todos os bytes foram enviados, false caso contrário.
     */
    bool send(const std::vector<uint8_t>& data) override;

    /**
     * \brief Define callback para lidar com dados recebidos.
     * \param callback Função a ser chamada sempre que dados chegarem.
     */
    void set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) override;

    /**
     * \brief Retorna o status atual (Disconnected, Connecting, Connected, Error).
     */
    TransportStatus get_status() const override;

private:
 
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport::network

#endif // TCP_TRANSPORT_H
