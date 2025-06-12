// commands/write_digital_timer_command.cpp
#include "write_digital_timer_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <stdexcept>

namespace command::mcb {

struct WriteDigitalTimerCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p,
         utils::enum_::DigitalOutputPort port,
         uint16_t t)
      : protocol(std::move(p))
      , port(port)
      , timeout(t)
      , logger(spdlog::default_logger())
    {}

    void run() {
        uint8_t high = static_cast<uint8_t>((timeout >> 8) & 0xFF);
        uint8_t low  = static_cast<uint8_t>(timeout & 0xFF);
    
        std::vector<uint8_t> payload = {
            static_cast<uint8_t>(port),
            high,
            low
        };
        protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_DIGITAL_TIMER),
            payload
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    utils::enum_::DigitalOutputPort      port;
    uint16_t                             timeout;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteDigitalTimerCommand::WriteDigitalTimerCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    utils::enum_::DigitalOutputPort port,
    uint16_t timeoutSeconds
) : impl_(std::make_unique<Impl>(proto, port, timeoutSeconds))
{}

WriteDigitalTimerCommand::~WriteDigitalTimerCommand() = default;

void WriteDigitalTimerCommand::execute() {
    try {
        impl_->run();
        // converte enum para byte ao logar
        impl_->logger->info(
            "WRITE_DIGITAL_TIMER executado: address=0x{:02X}, timeout={}s",
            static_cast<uint8_t>(impl_->port),
            impl_->timeout
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_DIGITAL_TIMER falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
