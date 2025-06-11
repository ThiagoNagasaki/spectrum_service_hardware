// command/mcb/read_sensor_distance_command.cpp
#include "read_sensor_distance_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadSensorDistanceCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p,
         utils::enum_::SensorDistanceMode m)
      : protocol(std::move(p)), mode(m),
        logger(spdlog::default_logger())
    {}

    SensorDistanceInfo run() {
        // envia [0x55, mode]
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_SENSOR_DISTANCE),
            { static_cast<uint8_t>(mode) }
        );
        if (data.size() < 2) {
            throw std::runtime_error(
                "READ_SENSOR_DISTANCE: payload inesperado, esperado 2 bytes"
            );
        }
        uint16_t v = (uint16_t(data[0]) << 8) | uint16_t(data[1]);
        return { v };
    }

    std::shared_ptr<protocol::IProtocol>       protocol;
    utils::enum_::SensorDistanceMode           mode;
    std::shared_ptr<spdlog::logger>           logger;
};

ReadSensorDistanceCommand::ReadSensorDistanceCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    utils::enum_::SensorDistanceMode mode
) : impl_(std::make_unique<Impl>(proto, mode))
{}

ReadSensorDistanceCommand::~ReadSensorDistanceCommand() = default;

SensorDistanceInfo ReadSensorDistanceCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_SENSOR_DISTANCE[0x{:02X}] → {}",
            static_cast<uint8_t>(impl_->mode), info.value
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_SENSOR_DISTANCE falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
