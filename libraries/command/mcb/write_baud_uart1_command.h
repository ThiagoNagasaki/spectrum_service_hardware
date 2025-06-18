// commands/write_baud_uart2_command.h
#ifndef WRITE_BAUD_UART1_COMMAND_H
#define WRITE_BAUD_UART1_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Configura baud rate e paridade da UART2 (Raio-X 2) via WRITE_BAUD_UART2 (0x68).
 *
 * Payload:
 *  - Byte 0: código de velocidade (mesmos valores de 0x00 a 0x08)  
 *  - Byte 1: código de paridade (mesmos valores de 0x00 a 0x03) :contentReference[oaicite:1]{index=1}
 */
class WriteBaudUart1Command : public ICommand<void> {
public:
    WriteBaudUart1Command(std::shared_ptr<protocols::IProtocol> protocols,
                          uint8_t speedCode,
                          uint8_t parityCode);
    ~WriteBaudUart1Command() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_BAUD_UART2_COMMAND_H
