// commands/write_mac_command.cpp
#include "write_mac_command.h"
#include <spdlog/spdlog.h>
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <stdexcept>

namespace command::mcb {

struct WriteMacCommand::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p, MacAddr m)
      : protocols(std::move(p)), addr(std::move(m)), logger(spdlog::default_logger())
    {}

    void run() {
        protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_MAC),
            { addr.mac[0], addr.mac[1], addr.mac[2],
              addr.mac[3], addr.mac[4], addr.mac[5] }
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    MacAddr                             addr;
    std::shared_ptr<spdlog::logger>     logger;
};

WriteMacCommand::WriteMacCommand(
    std::shared_ptr<protocols::IProtocol> proto,
    MacAddr mac
) : impl_(std::make_unique<Impl>(proto, std::move(mac)))
{}

WriteMacCommand::~WriteMacCommand() = default;

void WriteMacCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "WRITE_MAC executado: {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
            impl_->addr.mac[0], impl_->addr.mac[1], impl_->addr.mac[2],
            impl_->addr.mac[3], impl_->addr.mac[4], impl_->addr.mac[5]
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_MAC falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
