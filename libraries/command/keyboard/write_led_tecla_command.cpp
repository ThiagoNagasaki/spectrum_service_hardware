// commands/keyboard/write_led_tecla_command.cpp
#include "write_led_tecla_command.h"
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
struct WriteLedTeclaCommand::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p, KeyCode k, bool s)
      : protocols(std::move(p)), key(k), on(s), logger(spdlog::default_logger())
    {}

    void run() {
        protocols->sendCommand(
            static_cast<uint8_t>(MCBCommand::CMD_LED_TECLA),
            { static_cast<uint8_t>(key), static_cast<uint8_t>(on) }
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    KeyCode                              key;
    bool                                 on;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteLedTeclaCommand::WriteLedTeclaCommand(
    std::shared_ptr<protocols::IProtocol> proto,
    KeyCode key,
    bool on
) : impl_(std::make_unique<Impl>(proto, key, on))
{}

WriteLedTeclaCommand::~WriteLedTeclaCommand() = default;

void WriteLedTeclaCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "CMD_LED_TECLA executado: key=0x{:02X}, state={}",
            static_cast<uint8_t>(impl_->key), impl_->on
        );
    } catch (const std::exception& e) {
        impl_->logger->error("CMD_LED_TECLA falhou: {}", e.what());
        throw;
    }
}

} // namespace command::keyboard
