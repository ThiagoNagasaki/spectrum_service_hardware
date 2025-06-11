// commands/keyboard/write_buzzer_command.h
#ifndef WRITE_BUZZER_COMMAND_H
#define WRITE_BUZZER_COMMAND_H

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
 * \brief Comando CMD_BUZZER (0x79): liga o buzzer por um tempo especificado.
 *
 * Payload (2 bytes):
 *  - Byte 0: parte alta do tempo em ms (0–16000 ms)  
 *  - Byte 1: parte baixa do tempo em ms :contentReference[oaicite:0]{index=0}
 */
class WriteBuzzerCommand : public ICommand<void> {
public:
    /// @param durationMs duração em milissegundos (0–16000)
    WriteBuzzerCommand(std::shared_ptr<protocol::IProtocol> protocol,
                       uint16_t durationMs);
    ~WriteBuzzerCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::keyboard

#endif // WRITE_BUZZER_COMMAND_H
