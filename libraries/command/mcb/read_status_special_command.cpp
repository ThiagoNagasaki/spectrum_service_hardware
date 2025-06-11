// command/mcb/read_status_special_command.cpp
#include "read_status_special_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadStatusSpecialCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
      : protocol(std::move(p)),
        logger(spdlog::default_logger())
    {}

    StatusSpecialInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_STATUS_SPECIAL),
            {}
        );
        if (data.empty()) {
            throw std::runtime_error(
                "READ_STATUS_SPECIAL: payload vazio (esperado 1 byte)"
            );
        }
        uint8_t b0 = data[0];
        return {
            /*radioxOff=*/ static_cast<bool>(b0 & 0x01),
            /*inverter0=*/ static_cast<bool>(b0 & 0x02)
        };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadStatusSpecialCommand::ReadStatusSpecialCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadStatusSpecialCommand::~ReadStatusSpecialCommand() = default;

StatusSpecialInfo ReadStatusSpecialCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_STATUS_SPECIAL → radioxOff={}, inverter0={}",
            info.radioxOff, info.inverter0
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_STATUS_SPECIAL falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
