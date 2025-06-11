// libraries/protocols/i_protocol.h
#ifndef I_PROTOCOL_H
#define I_PROTOCOL_H

#include <vector>
#include <cstdint>
#include <functional>

namespace protocol {

/**
 * \brief Interface genérica de protocolo: basta implementar sendCommand()
 *        e subscribe() para quem quiser receber frames “push”.
 */
class IProtocol {
public:
    virtual ~IProtocol() = default;

    /**
     * \param cmd     código do comando (por ex. 0x51 para READ_FIRMWARE)
     * \param payload dados a enviar (vazio em leituras)
     * \return        vetor de bytes do payload de resposta
     * \throws std::runtime_error em caso de framing, checksum ou tamanho incorreto
     */
    virtual std::vector<uint8_t> sendCommand(
        uint8_t cmd,
        const std::vector<uint8_t>& payload
    ) = 0;

    /**
     * \brief Registra um callback para “push” de frames completos.
     * @param cb função que recebe o vetor de bytes (STX…ETX)
     */
    virtual void subscribe(
        std::function<void(const std::vector<uint8_t>&)> cb
    ) = 0;
};

using IProtocolPtr = std::shared_ptr<IProtocol>;

} // namespace protocol
#endif //  