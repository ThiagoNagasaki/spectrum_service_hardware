// libraries/receiver/keyboard/keyboard_receiver.cpp
#include "keyboard_receiver.h"
#include "../../utils/enum_/keyboard_keycodes.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include <spdlog/spdlog.h>
#include "../../../libraries/receiver/i_receiver.h"
namespace command::keyboard {
using receiver::IReceiver;
using utils::enum_::MCBCommand;
KeyboardReceiver::KeyboardReceiver(std::shared_ptr<protocols::IProtocol> proto)
  : protocol_(std::move(proto))
{
    protocol_->subscribe([this](const Frame& frame) {
        try {
            dispatch(frame);
        } catch (const std::exception& e) {
            spdlog::default_logger()->error("KeyboardReceiver dispatch erro: {}", e.what());
        }
    });
}

KeyboardReceiver::~KeyboardReceiver() = default;

void KeyboardReceiver::setPressedHandler(std::shared_ptr<IReceiver<KeyPressedInfo>> h) {
    pressedHandler_ = std::move(h);
}

void KeyboardReceiver::setReleasedHandler(std::shared_ptr<IReceiver<KeyReleasedInfo>> h) {
    releasedHandler_ = std::move(h);
}

void KeyboardReceiver::dispatch(const Frame& frame) {
    if (frame.empty()) return;
    uint8_t cmd = frame[0];
    switch (static_cast<MCBCommand>(cmd)) {
        case MCBCommand::CMD_KEY_PRESSED:
            if (pressedHandler_) {
                ReadKeyPressedCommand c(protocol_);
                auto info = c.execute();
                pressedHandler_->onReceive(info);
            }
            break;
        case MCBCommand::CMD_KEY_RELEASED:
            if (releasedHandler_) {
                ReadKeyReleasedCommand c(protocol_);
                auto info = c.execute();
                releasedHandler_->onReceive(info);
            }
            break;
        default:
            break;
    }
}

} // namespace command::keyboard
