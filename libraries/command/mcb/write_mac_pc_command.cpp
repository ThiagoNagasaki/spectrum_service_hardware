// commands/write_mac_pc_command.cpp
#include "write_mac_pc_command.h"
#include <spdlog/spdlog.h>
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <stdexcept>
#include <vector>

namespace command::mcb {

struct WriteMacPcCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p,
         const MacPcInfo& m)
      : protocol(std::move(p))
      , info(m)
      , logger(spdlog::default_logger())
    {}

    void run() {
        protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_MAC_PC),
            { info.mac[0], info.mac[1], info.mac[2],
              info.mac[3], info.mac[4], info.mac[5] }
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    MacPcInfo                           info;
    std::shared_ptr<spdlog::logger>     logger;
};

WriteMacPcCommand::WriteMacPcCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    const MacPcInfo& mac
) : impl_(std::make_unique<Impl>(proto, mac))
{}

WriteMacPcCommand::~WriteMacPcCommand() = default;

void WriteMacPcCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "WRITE_MAC_PC executado: {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
            impl_->info.mac[0], impl_->info.mac[1], impl_->info.mac[2],
            impl_->info.mac[3], impl_->info.mac[4], impl_->info.mac[5]
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_MAC_PC falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
