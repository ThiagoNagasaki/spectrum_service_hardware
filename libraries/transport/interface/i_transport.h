#pragma once

#include <string>
#include <functional>
#include <vector>
#include <cstdint>
#include "../enum_/enum_transportstatus.h"

namespace transport::interface {

using transport::enum_::TransportStatus;

/**
 * \brief Interface para transporte de dados brutos (TCP, Serial, RS485, etc.)
 */
class ITransport {
public:
    virtual ~ITransport() = default;

    /// Estabelece a conexão com o dispositivo remoto
    virtual bool connect() = 0;

    /// Finaliza a conexão
    virtual bool disconnect() = 0;

    /// Envia dados brutos
    virtual bool send(const std::vector<uint8_t>& data) = 0;

    /// Inscreve um callback para dados recebidos
    virtual void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) = 0;

    /// Obtém o status da conexão
    virtual TransportStatus get_status() const = 0;

    /// Realiza leitura síncrona (bloqueante) até o fim do frame
    virtual std::vector<uint8_t> receive() = 0;

};

} // namespace transport::interface
