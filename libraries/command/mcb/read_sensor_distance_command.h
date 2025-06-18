// command/mcb/read_sensor_distance_command.h
#ifndef COMMAND_MCB_READ_SENSOR_DISTANCE_COMMAND_H
#define COMMAND_MCB_READ_SENSOR_DISTANCE_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Leitura de distância ou velocidade via READ_SENSOR_DISTANCE (0x55).
 * Mode = 0x00 → distância; 0x01 → velocidade.
 * Response: 2 bytes (MSB, LSB) com valor 0–65535. 
 */
struct SensorDistanceInfo {
    uint16_t value;
};

class ReadSensorDistanceCommand
  : public command::ICommand<SensorDistanceInfo>
{
public:
    ReadSensorDistanceCommand(
        std::shared_ptr<protocols::IProtocol> proto,
        utils::enum_::SensorDistanceMode mode
    );
    ~ReadSensorDistanceCommand() override;

    SensorDistanceInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // COMMAND_MCB_READ_SENSOR_DISTANCE_COMMAND_H
