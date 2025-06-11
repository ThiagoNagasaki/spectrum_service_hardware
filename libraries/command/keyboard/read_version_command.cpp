// commands/keyboard/read_version_command.cpp
#include "read_version_command.h"
#include "../../utils/translator/response_translator.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/keyboard_keycodes.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::keyboard {
using utils::enum_::MCBCommand;
struct ReadVersionCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
        : protocol(std::move(p))
        , logger(spdlog::default_logger())
    {}

    VersionInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(MCBCommand::CMD_VERSION),
            {}
        );

        if (data.size() < 2) {
            throw std::runtime_error("CMD_VERSION: payload inesperado, esperado >=2 bytes");
        }
        return VersionInfo{
            /*version=*/  data[0],
            /*revision=*/ data[1]
        };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadVersionCommand::ReadVersionCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadVersionCommand::~ReadVersionCommand() = default;

VersionInfo ReadVersionCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "CMD_VERSION executado: version={} revision={}",
            info.version, info.revision
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("CMD_VERSION falhou: {}", e.what());
        throw;
    }
}

} // namespace command::keyboard
