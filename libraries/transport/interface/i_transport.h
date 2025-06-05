#ifndef I_TRANSPORT_H
#define I_TRANSPORT_H

#include <string>
#include "../enum_/enum_transportstatus.h"
#include <functional>
#include <vector>
#include <cstdint>

namespace transport::interface {
 using transport::enum_::TransportStatus;
/**
 * @brief Interface base para todos os transportes de comunicação.
 * 
 * Os métodos permitem conectar, desconectar, enviar dados e assinar callbacks
 * para dados recebidos.
 */
class ITransport {
public:
    virtual ~ITransport() = default;

    /**
     * @brief Estabelece a conexão com o dispositivo.
     */
    virtual bool connect() = 0;

    /**
     * @brief Finaliza a conexão com o dispositivo.
     */
    virtual bool disconnect() = 0;

    /**
     * @brief Envia dados para o dispositivo conectado.
     * 
     * @param data Vetor de bytes.
     */
    virtual bool send(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Inscreve um callback para lidar com dados recebidos.
     * 
     * @param callback Função a ser chamada quando dados forem recebidos.
     */
    virtual void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) = 0;

    /**
     * @brief Retorna o status atual da conexão.
     */
    virtual TransportStatus get_status() const = 0;
};

} // namespace transport::interface

#endif // I_TRANSPORT_H
