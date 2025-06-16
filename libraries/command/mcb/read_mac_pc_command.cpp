// commands/read_mac_pc_command.cpp
#include "read_mac_pc_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadMacPcCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
        : protocol(std::move(p))
        , logger(spdlog::default_logger())
    {}

    MacPcInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_MAC_PC),
            {}
        );

        if (data.size() < 6) {
            throw std::runtime_error(
                "READ_MAC_PC: payload inesperado, esperado >=6 bytes"
            );
        }

        MacPcInfo info;
        std::copy_n(data.begin(), 6, info.mac.begin());
        return info;
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadMacPcCommand::ReadMacPcCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadMacPcCommand::~ReadMacPcCommand() = default;

MacPcInfo ReadMacPcCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_MAC_PC executado: {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
            info.mac[0], info.mac[1], info.mac[2],
            info.mac[3], info.mac[4], info.mac[5]
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_MAC_PC falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
