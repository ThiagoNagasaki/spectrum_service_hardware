// commands/read_status_command.cpp
#include "read_status_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadStatusCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
        : protocol(std::move(p))
        , logger(spdlog::default_logger())
    {}

    StatusInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_STATUS),
            {}
        );

        if (data.size() < 2) {
            throw std::runtime_error(
                "READ_STATUS: payload inesperado, esperado >=2 bytes"
            );
        }
        uint8_t b0 = data[0];
        uint8_t b1 = data[1];

        return StatusInfo{
            /*interlockEmergency0=*/ static_cast<bool>(b0 & 0x01),
            /*interlockEmergency1=*/ static_cast<bool>(b0 & 0x02),
            /*xrayOn=*/             static_cast<bool>(b0 & 0x04),
            /*tunnelSensor1=*/      static_cast<bool>(b0 & 0x08),
            /*tunnelSensor2=*/      static_cast<bool>(b0 & 0x10),
            /*tunnelSensor3=*/      static_cast<bool>(b0 & 0x20),
            /*tunnelSensor4=*/      static_cast<bool>(b0 & 0x40),
            /*exp2In=*/             static_cast<bool>(b0 & 0x80),

            /*exp0=*/               static_cast<bool>(b1 & 0x01),
            /*exp1=*/               static_cast<bool>(b1 & 0x02),
            /*exp2=*/               static_cast<bool>(b1 & 0x04),
            /*exp3=*/               static_cast<bool>(b1 & 0x08),
            /*exp4=*/               static_cast<bool>(b1 & 0x10),
            /*conveyorOn=*/         static_cast<bool>(b1 & 0x20),
            /*exp6=*/               static_cast<bool>(b1 & 0x40),
            /*exp7=*/               static_cast<bool>(b1 & 0x80)
        };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadStatusCommand::ReadStatusCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadStatusCommand::~ReadStatusCommand() = default;

StatusInfo ReadStatusCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_STATUS executado: interlock={}, emerg={}, xray={}, "
            "t1={} t2={} t3={} t4={}, exp2In={}, convOn={}, exp0={},exp1={},"
            "exp2={}, exp3={}, exp4={},   exp6={}, exp7={}",
            info.interlockEmergency0, info.interlockEmergency1,
            info.xrayOn, info.tunnelSensor1, info.tunnelSensor2,
            info.tunnelSensor3, info.tunnelSensor4,info.exp2In, info.conveyorOn,info.exp0,info.exp1,
            info.exp2,info.exp3,info.exp4,info.exp6,info.exp7
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_STATUS falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
 