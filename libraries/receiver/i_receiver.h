// libraries/receiver/i_receiver.h
#ifndef I_RECEIVER_H
#define I_RECEIVER_H

#include <memory>

/**
 * \brief Interface genérica para “receber” informações decodificadas.
 * 
 * @tparam T Tipo de informação recebida (StatusInfo, KeyPressedInfo, etc).
 */
template<typename T>
class IReceiver {
public:
    virtual ~IReceiver() = default;

    /// Chamado sempre que chega um novo T para processar.
    virtual void onReceive(const T& info) = 0;
};

#endif // I_RECEIVER_H
