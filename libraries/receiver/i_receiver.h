#pragma once
#include <memory>

namespace receiver {

/**
 * \brief Interface para recepção de dados decodificados via callback.
 * 
 * @tparam T Tipo da informação recebida (ex.: StatusInfo, KeyPressedInfo)
 */
template<typename T>
class IReceiver {
public:
    virtual ~IReceiver() = default;

    /// Notificação reativa (push)
    virtual void onReceive(const T& info) = 0;

};

/**
 * \brief Interface para aguardar resposta bloqueante (pull).
 */
template<typename T>
class IFrameAwaiter {
public:
    virtual ~IFrameAwaiter() = default;

    /// Aguarda um frame completo e decodificado
    virtual T waitForFrame() = 0;
};

} // namespace receiver
