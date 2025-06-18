// command/mcb/read_status_special_command.h
#ifndef COMMAND_MCB_READ_STATUS_SPECIAL_COMMAND_H
#define COMMAND_MCB_READ_STATUS_SPECIAL_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Palavra de status especial via READ_STATUS_SPECIAL (0x56):
 * BIT0 = ST_DESLIGA_RAIOX, BIT1 = ST_INVERSOR_0. 
 */
struct StatusSpecialInfo {
    bool radioxOff;
    bool inverter0;
};

class ReadStatusSpecialCommand
  : public command::ICommand<StatusSpecialInfo>
{
public:
    explicit ReadStatusSpecialCommand(std::shared_ptr<protocols::IProtocol> proto);
    ~ReadStatusSpecialCommand() override;

    StatusSpecialInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // COMMAND_MCB_READ_STATUS_SPECIAL_COMMAND_H
