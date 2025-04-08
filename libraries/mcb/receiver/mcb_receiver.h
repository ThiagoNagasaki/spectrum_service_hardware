#ifndef MCB_RECEIVER_H
#define MCB_RECEIVER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mcb {
namespace receiver {

/**
 * @brief Classe MCBReceiver.
 *
 * Responsável por acumular os dados brutos recebidos, reassemble dos frames completos (baseado em STX, tamanho, checksum e ETX)
 * e decodificação dos frames utilizando a classe MCBDecoder. 
 *
 */
class MCBReceiver {
public:
    using Callback = std::function<void(const std::string&)>;

    MCBReceiver();
    ~MCBReceiver();

    void setCallback(Callback cb);
    void feed(const std::vector<uint8_t>& data);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace receiver
} // namespace mcb

#endif // MCB_RECEIVER_H
