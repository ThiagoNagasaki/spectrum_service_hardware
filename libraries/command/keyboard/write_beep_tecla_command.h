// commands/keyboard/write_beep_tecla_command.h
#ifndef WRITE_BEEP_TECLA_COMMAND_H
#define WRITE_BEEP_TECLA_COMMAND_H

#include "../../utils/translator/response_translator.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/keyboard_keycodes.h"
#include <memory>
#include <cstdint>

namespace command::keyboard {

/**
 * \brief Comando CMD_BEEP_TECLA (0x7A): liga/desliga o beep do teclado.
 *
 * Payload (1 byte):
 *  - Byte 0: 0 = Off, 1 = On :contentReference[oaicite:1]{index=1}
 */
class WriteBeepTeclaCommand : public ICommand<void> {
public:
    WriteBeepTeclaCommand(std::shared_ptr<protocols::IProtocol> protocols,
                          bool enable);
    ~WriteBeepTeclaCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::keyboard

#endif // WRITE_BEEP_TECLA_COMMAND_H
