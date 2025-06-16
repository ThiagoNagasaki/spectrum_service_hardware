// commands/keyboard/read_key_pressed_command.h
#ifndef READ_KEY_PRESSED_COMMAND_H
#define READ_KEY_PRESSED_COMMAND_H

#include "../../utils/translator/response_translator.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/keyboard_keycodes.h"
#include <memory>
 
namespace command::keyboard {
using command::ICommand; 
using utils::enum_::KeyCode;
struct KeyPressedInfo {
    KeyCode code; 
    bool    shift;
     std::string toString() const {
        using namespace command::keyboard;
        using utils::ResponseTranslator;
        auto name = ResponseTranslator::keyCodeToString(static_cast<KeyCode>(code));
        return name + (shift ? " + Shift" : "");
    }
};

class ReadKeyPressedCommand : public command::ICommand<KeyPressedInfo> {
public:
    explicit ReadKeyPressedCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadKeyPressedCommand() override;

    KeyPressedInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
 

} // namespace command::keyboard

#endif // READ_KEY_PRESSED_COMMAND_H
