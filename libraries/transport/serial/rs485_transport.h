#ifndef RS485_TRANSPORT_H
#define RS485_TRANSPORT_H

#include "../interface/i_transport.h"
#include "../enum_/enum_bauntrate.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace transport::serial {

/**
 * \class RS485Transport
 * \brief Implementa ITransport para comunicação RS485 usando PImpl.
 *
 * - Construtor recebe dispositivo e baud rate (ex.: "/dev/ttyS1", BaudRate::B9600).
 * - \c connect() abre a porta e configura 8N1.
 * - \c disconnect() fecha a porta.
 * - \c send() envia dados.
 * - \c subscribe() registra callback.
 * - \c get_status() retorna Disconnected, Connected, Error etc.
 */
class RS485Transport : public interface::ITransport {
public:
    /**
     * \brief Construtor que define dispositivo e velocidade.
     */
    RS485Transport(const std::string& device, enum_::BaudRate baud_rate);

    ~RS485Transport() override;

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

#endif // RS485_TRANSPORT_H
