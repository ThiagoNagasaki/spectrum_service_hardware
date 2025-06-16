// commands/keyboard/read_key_released_command.h
#ifndef READ_KEY_RELEASED_COMMAND_H
#define READ_KEY_RELEASED_COMMAND_H

#include "../../utils/translator/response_translator.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/keyboard_keycodes.h"
 
#include <memory>
#include <string>

namespace command::keyboard {
using utils::enum_::KeyCode;
struct KeyReleasedInfo {
    KeyCode code; 

    std::string toString() const {
        return utils::ResponseTranslator::keyCodeToString(code);
    }
};

class ReadKeyReleasedCommand : public ICommand<KeyReleasedInfo> {
public:
    explicit ReadKeyReleasedCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadKeyReleasedCommand() override;

    KeyReleasedInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::keyboard

#endif // READ_KEY_RELEASED_COMMAND_H
