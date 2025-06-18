// commands/keyboard/write_led_tecla_command.h
#ifndef WRITE_LED_TECLA_COMMAND_H
#define WRITE_LED_TECLA_COMMAND_H

#include "../../utils/translator/response_translator.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/keyboard_keycodes.h"
#include <memory>

namespace command::keyboard {
using utils::enum_::KeyCode;
/**
 * \brief Comando CMD_LED_TECLA (0x78): liga/desliga LED da tecla.
 */
class WriteLedTeclaCommand : public ICommand<void> {
public:
    WriteLedTeclaCommand(std::shared_ptr<protocols::IProtocol> protocols,
                         KeyCode key,
                         bool on);
    ~WriteLedTeclaCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::keyboard

#endif // WRITE_LED_TECLA_COMMAND_H
