#ifndef SERIAL_TRANSPORT_H
#define SERIAL_TRANSPORT_H

#include "../interface/i_transport.h"
#include "../../transport/enum_/enum_baudrate.h" 
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
using transport::enum_::BaudRate;

/**
 * \brief Enum para diferenciar o tipo de comunicação serial.
 */
enum class SerialType {
    RS232,
    RS485
};

/**
 * \class SerialTransport
 * \brief Implementa a interface ITransport para prover comunicação serial
 *        (RS232 ou RS485) de forma genérica, usando PImpl.
 *
 */
class SerialTransport : public interface::ITransport {
public:
    SerialTransport(const std::string& device, BaudRate baud_rate, SerialType type);
    ~SerialTransport() override;

    bool connect() override;
    bool disconnect() override;
    bool send(const std::vector<uint8_t>& data) override;
    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) override;
    enum_::TransportStatus get_status() const override;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport::serial

#endif // SERIAL_TRANSPORT_H
