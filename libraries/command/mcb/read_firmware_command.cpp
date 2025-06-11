#include "read_firmware_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadFirmwareCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
      : protocol(std::move(p))
      , logger(spdlog::default_logger())
    {}

    FirmwareInfo run() {
              auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_FIRMWARE),
            {}
        );
        if (data.size() < 3) {
            throw std::runtime_error(
                "READ_FIRMWARE: payload inesperado, esperado >=3 bytes"
            );
        }
        return FirmwareInfo{
            /*version*/  data[0],
            /*revision*/ data[1],
            /*build*/    data[2]
        };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>           logger;
};

ReadFirmwareCommand::ReadFirmwareCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadFirmwareCommand::~ReadFirmwareCommand() = default;

FirmwareInfo ReadFirmwareCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_FIRMWARE executado: version={}, revision={}, build={}",
            info.version, info.revision, info.build
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_FIRMWARE falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
