#ifndef RS232_TRANSPORT_H
#define RS232_TRANSPORT_H

#include "i_transport.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace transport {

class RS232Transport : public ITransport {
public:
    explicit RS232Transport(const std::string& device, int baud_rate);
    ~RS232Transport() override;
    /**
     * @brief Estabelece conexão com o dispositivo RS232.
     * 
     * @return true se a conexão for bem-sucedida, false caso contrário.
     */
    bool connect() override;
    /**
     * @brief Encerra a conexão com o dispositivo RS232.
     * 
     * @return true se a desconexão for bem-sucedida, false caso contrário.
     */
    bool disconnect() override;
    /**
     * @brief Envia um conjunto de bytes para o dispositivo conectado via RS232.
     * 
     * @param data Vetor de bytes a serem enviados.
     * @return true se os dados forem enviados com sucesso, false caso contrário.
     */
    bool send(const std::vector<uint8_t>& data) override;
     /**
     * @brief Define uma função de callback para processar dados recebidos via RS232.
     * 
     * O callback será chamado sempre que novos dados forem recebidos do dispositivo.
     * 
     * @param callback Função a ser chamada ao receber dados.
     */
    void set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) override;
    /**
     * @brief Obtém o status atual da conexão RS232.
     * 
     * @return O status da conexão, podendo ser Disconnected, Connecting, Connected ou Error.
     */
    TransportStatus get_status() const override;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace transport

#endif // RS232_TRANSPORT_H
 
