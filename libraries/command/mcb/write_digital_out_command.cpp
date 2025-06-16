// commands/write_digital_out_command.cpp
#include "write_digital_out_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <stdexcept>

namespace command::mcb {

struct WriteDigitalOutCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p,
         utils::enum_::DigitalOutputPort port,
         uint8_t val)
      : protocol(std::move(p))
      , port(port)
      , value(val)
      , logger(spdlog::default_logger())
    {
        if (value > 1) {
            throw std::invalid_argument(
                "WRITE_DIGITAL_OUT: valor inválido, deve ser 0 ou 1"
            );
        }
    }

    void run() {
        std::vector<uint8_t> payload{
            static_cast<uint8_t>(port),
            value
        };
        protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_DIGITAL_OUT),
            payload
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    utils::enum_::DigitalOutputPort      port;
    uint8_t                              value;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteDigitalOutCommand::WriteDigitalOutCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    utils::enum_::DigitalOutputPort port,
    uint8_t value
) : impl_(std::make_unique<Impl>(proto, port, value))
{}

WriteDigitalOutCommand::~WriteDigitalOutCommand() = default;

void WriteDigitalOutCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "WRITE_DIGITAL_OUT executado: address=0x{:02X}, value={}",
            static_cast<uint8_t>(impl_->port),
            impl_->value
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_DIGITAL_OUT falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
