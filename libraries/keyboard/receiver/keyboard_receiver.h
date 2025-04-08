#ifndef KEYBOARD_RECEIVER_H
#define KEYBOARD_RECEIVER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace keyboard {
namespace receiver {

/**
 * @brief Classe KeyboardReceiver.
 *
 * Encapsula a lógica de acumulação de dados brutos, extração de frames completos
 * e decodificação dos frames utilizando a classe KeyboardDecoder. O usuário final
 * registra um callback para receber a mensagem decodificada.
 */
class KeyboardReceiver {
public:
    using Callback = std::function<void(const std::string&)>;

    KeyboardReceiver();
    ~KeyboardReceiver();

    void setCallback(Callback cb);

    void feed(const std::vector<uint8_t>& data);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace receiver
} // namespace keyboard

#endif // KEYBOARD_RECEIVER_H
