// commands/read_status_command.h
#ifndef READ_STATUS_COMMAND_H
#define READ_STATUS_COMMAND_H

#include "../../../libraries/command/i_command.h"
#include "../../../libraries/protocols/i_protocol.h"
#include "../../../utils/enum_/mcb_port_addresses.h"
#include "../../../utils/logger/logger.h"
#include <memory>

namespace command::mcb {

/**
 * \brief Representa o estado lido pelo comando 0x52 (READ_STATUS).
 *
 * Byte 0:
 *  - bit0: Interlock/Emergência 0  
 *  - bit1: Interlock/Emergência 1  
 *  - bit2: Estado Raio-X  
 *  - bit3: Sensor de túnel 1  
 *  - bit4: Sensor de túnel 2  
 *  - bit5: Sensor de túnel 3  
 *  - bit6: Sensor de túnel 4  
 *  - bit7: EXP2_IN (CN16)  
 *
 * Byte 1:
 *  - bit0: Expansão 0 (CN10)  
 *  - bit1: Expansão 1 (CN10)  
 *  - bit2: Expansão 2 (CN10)  
 *  - bit3: Expansão 3 (CN10)  
 *  - bit4: Expansão 4 (CN10)  
 *  - bit5: Status da esteira (ON/OFF)  
 *  - bit6: Expansão 6 (CN10)  
 *  - bit7: Expansão 7 (CN10)  
 *
 * :contentReference[oaicite:1]{index=1}
 */
struct StatusInfo {
    // byte 0
    bool interlockEmergency0;
    bool interlockEmergency1;
    bool xrayOn;
    bool tunnelSensor1;
    bool tunnelSensor2;
    bool tunnelSensor3;
    bool tunnelSensor4;
    bool exp2In;

    // byte 1
    bool exp0;
    bool exp1;
    bool exp2;
    bool exp3;
    bool exp4;
    bool conveyorOn;
    bool exp6;
    bool exp7;
};

/**
 * \brief Comando READ_STATUS (0x52) da MCB.
 * Retorna um StatusInfo já parseado.
 */
class ReadStatusCommand
  : public ICommand<StatusInfo>
{
public:
    explicit ReadStatusCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadStatusCommand() override;

    StatusInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_STATUS_COMMAND_H
