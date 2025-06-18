#pragma once

#include "../receiver/i_receiver.h"
#include "../protocols/mcb_keyboard/mcb_keyboard_protocol.h"

namespace receiver {

/**
 * \brief Implementação vazia de IReceiver para MCBFrame.
 * 
 * Pode ser usada quando não há interesse em receber dados "push",
 * mas é necessário passar um receiver válido para o protocolo.
 */
class DummyMCBFrameReceiver : public receiver::IReceiver<protocols::mcb_keyboard::MCBFrame> {
public:
    void onReceive(const protocols::mcb_keyboard::MCBFrame& /*frame*/) override {
        // Nada a fazer — apenas evita nullptr
    }
};

} // namespace receiver
