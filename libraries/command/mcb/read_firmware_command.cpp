#include "read_firmware_command.h"
#include "../../../utils/enum_/mcb_port_addresses.h"
#include "../../../utils/logger/logger.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <iostream>

namespace command::mcb {

struct ReadFirmwareCommand::Impl {
    explicit Impl(std::shared_ptr<controller::ProtocolController> c)
      : controller(std::move(c))
      , logger(spdlog::default_logger())
    {}

    FirmwareInfo run() {
        std::cout << "Chegou em run !\n";
        auto data = controller->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_FIRMWARE),
            {}
        );

        if (data.size() < 3) {
            throw std::runtime_error("READ_FIRMWARE: payload inesperado, esperado >= 3 bytes");
        }

        return FirmwareInfo{
            data[0], // version
            data[1], // revision
            data[2]  // build
        };
    }

    std::shared_ptr<controller::ProtocolController> controller;
    std::shared_ptr<spdlog::logger>     logger;
};

ReadFirmwareCommand::ReadFirmwareCommand(std::shared_ptr<controller::ProtocolController> controller)
    : impl_(std::make_unique<Impl>(std::move(controller))) {}

ReadFirmwareCommand::~ReadFirmwareCommand() = default;

FirmwareInfo ReadFirmwareCommand::execute() {
    try {
        std::cout << "Chegou em Execute !\n";
        auto info = impl_->run();
        impl_->logger->info("READ_FIRMWARE executado: version={}, revision={}, build={}",
                            info.version, info.revision, info.build);
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_FIRMWARE falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
