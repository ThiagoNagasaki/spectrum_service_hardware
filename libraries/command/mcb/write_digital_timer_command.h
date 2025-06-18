// commands/write_digital_timer_command.h
#ifndef WRITE_DIGITAL_TIMER_COMMAND_H
#define WRITE_DIGITAL_TIMER_COMMAND_H


#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_DIGITAL_TIMER (0x69): aciona uma saída digital com temporização.
 *
 * Payload:
 *  - Byte 0: endereço da porta digital (DigitalOutputPort)
 *  - Byte 1: parte alta do tempo de acionamento (timeout >> 8)
 *  - Byte 2: parte baixa do tempo de acionamento (timeout & 0xFF)
 */
class WriteDigitalTimerCommand : public ICommand<void> {
public:
    WriteDigitalTimerCommand(std::shared_ptr<protocols::IProtocol> protocols,
                             utils::enum_::DigitalOutputPort port,
                             uint16_t timeoutSeconds);
    ~WriteDigitalTimerCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_DIGITAL_TIMER_COMMAND_H
