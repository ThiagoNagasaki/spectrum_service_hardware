// commands/read_analog_input_command.cpp
#include "read_analog_input_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadAnalogInputCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p, utils::enum_::AnalogInputPort addr)
      : protocol(std::move(p))
      , address(addr)
      , logger(spdlog::default_logger())
    {}

    AnalogInputInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_ANALOG_INPUT),
            { static_cast<uint8_t>(address) }
        );

        if (data.size() < 2) {
            throw std::runtime_error(
                "READ_ANALOG_INPUT: payload inesperado, esperado >=2 bytes"
            );
        }
        uint16_t val = (static_cast<uint16_t>(data[0]) << 8)
                     | static_cast<uint16_t>(data[1]);
        return AnalogInputInfo{ val };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    utils::enum_::AnalogInputPort    address;
    std::shared_ptr<spdlog::logger>     logger;
};

ReadAnalogInputCommand::ReadAnalogInputCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    utils::enum_::AnalogInputPort analogAddress
) : impl_(std::make_unique<Impl>(proto, analogAddress))
{}

ReadAnalogInputCommand::~ReadAnalogInputCommand() = default;

AnalogInputInfo ReadAnalogInputCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_ANALOG_INPUT executado: {}", info.toString()
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_ANALOG_INPUT falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
