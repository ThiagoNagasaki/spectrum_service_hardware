// commands/write_special_command.h
#ifndef WRITE_SPECIAL_COMMAND_H
#define WRITE_SPECIAL_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_SPECIAL (0x61): altera comandos especiais.
 *
 * Payload:
 *  - Byte 0: endereço da configuração (0x00–0x0F)
 *      • 0x00 – Luz de objeto suspeito
 *      • 0x01 – Cancela 1 (Cargo Compact)
 *      • 0x02 – Cancela 2 (Cargo Compact)
 *      …  
 *  - Byte 1: valor (0 = desabilita / fecha, 1 = habilita / abre) :contentReference[oaicite:0]{index=0}
 */
class WriteSpecialCommand : public ICommand<void> {
public:
    WriteSpecialCommand(std::shared_ptr<protocol::IProtocol> protocol,
                        uint8_t address,
                        uint8_t value);
    ~WriteSpecialCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_SPECIAL_COMMAND_H
