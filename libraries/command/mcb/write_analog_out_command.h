// commands/write_analog_out_command.h
#ifndef WRITE_ANALOG_OUT_COMMAND_H
#define WRITE_ANALOG_OUT_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_ANALOG_OUT (0x64): escreve valor na saída analógica.
 *
 * Payload:
 *  - Byte 0: endereço da porta analógica (AnalogInputPort)
 *  - Byte 1: valor a ser escrito no DAC (0x00–0xFF)
 */
class WriteAnalogOutCommand : public ICommand<void> {
public:
    WriteAnalogOutCommand(std::shared_ptr<protocol::IProtocol> protocol,
                          utils::enum_::AnalogInputPort address,
                          uint8_t dacValue);
    ~WriteAnalogOutCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_ANALOG_OUT_COMMAND_H
