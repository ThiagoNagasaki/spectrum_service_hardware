#ifndef SERIAL_TRANSPORT_H
#define SERIAL_TRANSPORT_H

#include "../interface/i_transport.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

/**
 * \file serial_transport.h
 * \brief Classe genérica para comunicação serial (RS232 ou RS485) com PImpl.
 */
namespace transport::serial {

/**
 * \brief Enum para diferenciar o tipo de comunicação serial.
 */
enum class SerialType {
    RS232,
    RS485
};

/**
 * \brief Enum para baud rates suportados.
 */
enum class BaudRate {
    BR_4800,
    BR_9600,
    BR_19200,
    BR_38400,
    BR_57600,
    BR_115200
};

/**
 * \class SerialTransport
 * \brief Implementa a interface ITransport para prover comunicação serial
 *        (RS232 ou RS485) de forma genérica, usando PImpl.
 *
 * Configuração padrão (8 bits de dados, sem paridade, 1 stop bit).
 * Caso seja RS485 half-duplex, pode ser preciso controlar DE/RE via ioctl ou GPIO.
 */
class SerialTransport : public interface::ITransport {
public:
    /**
     * \brief Construtor que define o dispositivo, baud rate e tipo de porta (RS232 ou RS485).
     * \param device Caminho do dispositivo (ex: "/dev/ttyS0")
     * \param baud_rate Enum com a velocidade (ex: BaudRate::BR_115200)
     * \param type Indica se é RS232 ou RS485
     */
    SerialTransport(const std::string& device, BaudRate baud_rate, SerialType type);

    /**
     * \brief Destrutor.
     */
    ~SerialTransport() override;

    /**
     * \brief Abre a porta serial e configura (8N1).
     * \return true se sucesso, false caso contrário.
     */
    bool connect() override;

    /**
     * \brief Fecha a porta serial se estiver aberta.
     * \return true se sucesso, false caso contrário.
     */
    bool disconnect() override;

    /**
     * \brief Envia dados pela porta serial.
     * \param data Vetor de bytes a serem enviados.
     * \return true se enviou todos os bytes, false se ocorrer erro.
     */
    bool send(const std::vector<uint8_t>& data) override;

    /**
     * \brief Assina (subscribe) para receber dados do dispositivo.
     * \param callback Função a ser chamada quando dados chegarem.
     */
    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) override;

    /**
     * \brief Retorna o status atual da conexão.
     */
    enum_::TransportStatus get_status() const override;

private:
    class Impl;                        
    std::unique_ptr<Impl> pImpl_;      
};

} // namespace transport::serial

#endif // SERIAL_TRANSPORT_H
