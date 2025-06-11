// commands/keyboard/read_key_released_command.cpp
#include "read_key_released_command.h"
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

struct ReadKeyReleasedCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
        : protocol(std::move(p)), logger(spdlog::default_logger())
    {}

    KeyReleasedInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(MCBCommand::CMD_KEY_RELEASED),
            {}
        );
        if (data.empty()) {
            throw std::runtime_error("CMD_KEY_RELEASED: payload inesperado, esperado 1 byte");
        }
        return KeyReleasedInfo{ static_cast<KeyCode>(data[0] & 0x7F) };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadKeyReleasedCommand::ReadKeyReleasedCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadKeyReleasedCommand::~ReadKeyReleasedCommand() = default;

KeyReleasedInfo ReadKeyReleasedCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "CMD_KEY_RELEASED executado: code=0x{:02X}",
            static_cast<uint8_t>(info.code)
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("CMD_KEY_RELEASED falhou: {}", e.what());
        throw;
    }
}

} // namespace command::keyboard
