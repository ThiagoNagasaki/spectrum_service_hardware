#ifndef WRITE_DIGITAL_OUT_COMMAND_H
#define WRITE_DIGITAL_OUT_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_DIGITAL_OUT (0x63): altera valor de uma saída digital.
 *
 * Payload:
 *  - Byte 0: endereço da porta digital (DigitalOutputPort)
 *  - Byte 1: valor (0 = Off, 1 = On)
 */
class WriteDigitalOutCommand : public ICommand<void> {
public:
    WriteDigitalOutCommand(std::shared_ptr<protocols::IProtocol> protocols,
                           utils::enum_::DigitalOutputPort port,
                           uint8_t value);
    ~WriteDigitalOutCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_DIGITAL_OUT_COMMAND_H
