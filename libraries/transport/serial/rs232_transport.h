#ifndef RS232_TRANSPORT_H
#define RS232_TRANSPORT_H

#include "../interface/i_transport.h"
#include "../../../utils/logger.h"
#include "../../transport/enum_/enum_baudrate.h" // Enum BaudRate (BR_9600, BR_115200, etc.)
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace transport::serial {

/**
 * \class RS232Transport
 * \brief Implementa ITransport para comunicação RS232 usando PImpl.
 *
 * - Construtor recebe dispositivo (ex.: "/dev/ttyS0") e baud rate (ex.: BaudRate::BR_115200).
 * - \c connect() abre a porta e configura 8N1 (sem paridade, 1 stop bit).
 * - \c disconnect() fecha a porta.
 * - \c send() envia dados.
 * - \c subscribe() registra callback para dados recebidos (não implementado em loop).
 * - \c get_status() retorna Disconnected, Connected, Error etc.
 */
class RS232Transport : public interface::ITransport {
public:
    /**
     * \brief Construtor que define dispositivo e velocidade.
     * \param device Caminho do dispositivo serial (ex.: "/dev/ttyS0").
     * \param baud_rate Enum que representa a velocidade (ex.: BaudRate::BR_115200).
     */
    RS232Transport(const std::string& device, enum_::BaudRate baud_rate);

    /**
     * \brief Destrutor. Garante que a porta seja fechada.
     */
    ~RS232Transport() override;

    bool connect() override;
    bool disconnect() override;
    bool send(const std::vector<uint8_t>& data) override;
    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) override;
    enum_::TransportStatus get_status() const override;

private:
    /**
     * \brief Classe interna (PImpl) com a lógica de RS232.
     */
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport::serial

#endif // RS232_TRANSPORT_H
