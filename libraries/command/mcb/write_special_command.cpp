// commands/write_special_command.cpp
#include "write_special_command.h"
#include <spdlog/spdlog.h>
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <stdexcept>

namespace command::mcb {

struct WriteSpecialCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p, uint8_t addr, uint8_t val)
      : protocol(std::move(p))
      , address(addr)
      , value(val)
      , logger(spdlog::default_logger())
    {}

    void run() {
        protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_SPECIAL),
            { address, value }
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    uint8_t address;
    uint8_t value;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteSpecialCommand::WriteSpecialCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    uint8_t address,
    uint8_t value
) : impl_(std::make_unique<Impl>(proto, address, value))
{}

WriteSpecialCommand::~WriteSpecialCommand() = default;

void WriteSpecialCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "WRITE_SPECIAL executado: address=0x{:02X}, value={}",
            impl_->address, impl_->value
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_SPECIAL falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
