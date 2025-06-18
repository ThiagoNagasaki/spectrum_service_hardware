// commands/write_network_config_command.cpp
#include "write_network_config_command.h"
#include <spdlog/spdlog.h>
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <stdexcept>

namespace command::mcb {

struct WriteNetworkConfigCommand::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p, NetworkConfig c)
      : protocols(std::move(p)), cfg(std::move(c)), logger(spdlog::default_logger())
    {}

    void run() {
        std::vector<uint8_t> payload;
        payload.reserve(16);
        payload.insert(payload.end(), cfg.ip.begin(), cfg.ip.end());
        payload.insert(payload.end(), cfg.mask.begin(), cfg.mask.end());
        payload.insert(payload.end(), cfg.gateway.begin(), cfg.gateway.end());
        payload.insert(payload.end(), cfg.dns.begin(), cfg.dns.end());

        protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_NETWORK_CONFIG),
            payload
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    NetworkConfig cfg;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteNetworkConfigCommand::WriteNetworkConfigCommand(
    std::shared_ptr<protocols::IProtocol> proto,
    NetworkConfig cfg
) : impl_(std::make_unique<Impl>(proto, std::move(cfg)))
{}

WriteNetworkConfigCommand::~WriteNetworkConfigCommand() = default;

void WriteNetworkConfigCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "WRITE_NETWORK_CONFIG executado: IP={:d}.{:d}.{:d}.{:d}, "
            "MASK={:d}.{:d}.{:d}.{:d}, GW={:d}.{:d}.{:d}.{:d}, "
            "DNS={:d}.{:d}.{:d}.{:d}",
            impl_->cfg.ip[0],   impl_->cfg.ip[1],
            impl_->cfg.ip[2],   impl_->cfg.ip[3],
            impl_->cfg.mask[0], impl_->cfg.mask[1],
            impl_->cfg.mask[2], impl_->cfg.mask[3],
            impl_->cfg.gateway[0], impl_->cfg.gateway[1],
            impl_->cfg.gateway[2], impl_->cfg.gateway[3],
            impl_->cfg.dns[0], impl_->cfg.dns[1],
            impl_->cfg.dns[2], impl_->cfg.dns[3]
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_NETWORK_CONFIG falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
