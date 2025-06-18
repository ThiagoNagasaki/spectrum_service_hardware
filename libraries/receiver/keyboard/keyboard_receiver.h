// libraries/receiver/keyboard/keyboard_receiver.h
#ifndef KEYBOARD_RECEIVER_H
#define KEYBOARD_RECEIVER_H

#include "../../libraries/receiver/i_receiver.h"
#include "../../libraries/command/keyboard/read_key_pressed_command.h"
#include "../../libraries/command/keyboard/read_key_released_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include <memory>
#include <vector>
#include "../../../libraries/receiver/i_receiver.h"
namespace command::keyboard {
using receiver::IReceiver;

/**
 * \brief Listener de eventos vindos do teclado via RS-485.
 *
 * Dispara CMD_KEY_PRESSED (0x70) e CMD_KEY_RELEASED (0x72) via IReceiver<T>.
 */
class KeyboardReceiver {
public:
    using Frame = std::vector<uint8_t>;

    explicit KeyboardReceiver(std::shared_ptr<protocols::IProtocol> proto);
    ~KeyboardReceiver();

    void setPressedHandler(std::shared_ptr<IReceiver<KeyPressedInfo>>  handler);
    void setReleasedHandler(std::shared_ptr<IReceiver<KeyReleasedInfo>> handler);

private:
    void dispatch(const Frame& frame);

    std::shared_ptr<protocols::IProtocol>        protocol_;
    std::shared_ptr<IReceiver<KeyPressedInfo>>  pressedHandler_;
    std::shared_ptr<IReceiver<KeyReleasedInfo>> releasedHandler_;
};

} // namespace command::keyboard

#endif // KEYBOARD_RECEIVER_H
