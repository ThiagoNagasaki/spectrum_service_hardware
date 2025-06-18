// commands/keyboard/read_key_pressed_command.cpp
#include "read_key_pressed_command.h"
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
struct ReadKeyPressedCommand::Impl {
    explicit Impl(std::shared_ptr<protocols::IProtocol> p)
        : protocols(std::move(p)), logger(spdlog::default_logger())
    {}

    KeyPressedInfo run() {
        auto data = protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::CMD_KEY_PRESSED),
            {}
        );

        if (data.empty()) {
            throw std::runtime_error("CMD_KEY_PRESSED: payload inesperado, esperado 1 byte");
        }
        uint8_t b = data[0];
        return KeyPressedInfo{
            static_cast<KeyCode>(b & 0x7F),
            static_cast<bool>(b & 0x80)
        };
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadKeyPressedCommand::ReadKeyPressedCommand(
    std::shared_ptr<protocols::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadKeyPressedCommand::~ReadKeyPressedCommand() = default;

KeyPressedInfo ReadKeyPressedCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "CMD_KEY_PRESSED executado: code=0x{:02X} shift={}",
            static_cast<uint8_t>(info.code), info.shift
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("CMD_KEY_PRESSED falhou: {}", e.what());
        throw;
    }
}

} // namespace command::keyboard
