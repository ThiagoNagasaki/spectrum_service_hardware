// commands/write_config_command.cpp
#include "write_config_command.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct WriteConfigCommand::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p,
         utils::enum_::ConfigAddress addr,
         std::vector<uint8_t> vals)
      : protocols(std::move(p))
      , address(addr)
      , values(std::move(vals))
      , logger(spdlog::default_logger())
    {
        if (values.empty() || values.size() > 2) {
            throw std::invalid_argument(
                "WRITE_CONFIG: valores inválidos, deve ter 1 ou 2 bytes"
            );
        }
    }

    void run() {
        std::vector<uint8_t> payload;
        payload.reserve(1 + values.size());
        payload.push_back(static_cast<uint8_t>(address));
        payload.insert(payload.end(), values.begin(), values.end());

        protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_CONFIG),
            payload
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    utils::enum_::ConfigAddress          address;
    std::vector<uint8_t>                 values;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteConfigCommand::WriteConfigCommand(
    std::shared_ptr<protocols::IProtocol> proto,
    utils::enum_::ConfigAddress address,
    std::vector<uint8_t> values
) : impl_(std::make_unique<Impl>(proto, address, std::move(values)))
{}

WriteConfigCommand::~WriteConfigCommand() = default;

void WriteConfigCommand::execute() {
    try {
        impl_->run();
        // converte o enum para byte antes de logar
        uint8_t addr = static_cast<uint8_t>(impl_->address);
        if (impl_->values.size() == 1) {
            impl_->logger->info(
                "WRITE_CONFIG executado: address=0x{:02X}, value=0x{:02X}",
                addr,
                impl_->values[0]
            );
        } else {
            impl_->logger->info(
                "WRITE_CONFIG executado: address=0x{:02X}, high=0x{:02X}, low=0x{:02X}",
                addr,
                impl_->values[0],
                impl_->values[1]
            );
        }
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_CONFIG falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
