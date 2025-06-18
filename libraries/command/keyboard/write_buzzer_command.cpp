// commands/keyboard/write_buzzer_command.cpp
#include "write_buzzer_command.h"
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
struct WriteBuzzerCommand::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p, uint16_t dur)
      : protocols(std::move(p)), duration(dur),
        logger(spdlog::default_logger())
    {
        if (duration > 16000) {
            throw std::invalid_argument(
                "CMD_BUZZER: durationMs inválido, deve ser 0-16000"
            );
        }
    }

    void run() {
        uint8_t high = static_cast<uint8_t>((duration >> 8) & 0xFF);
        uint8_t low  = static_cast<uint8_t>(duration & 0xFF);
        protocols->sendCommand(
            static_cast<uint8_t>(MCBCommand::CMD_BUZZER),
            { high, low }
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    uint16_t                             duration;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteBuzzerCommand::WriteBuzzerCommand(
    std::shared_ptr<protocols::IProtocol> proto,
    uint16_t durationMs
) : impl_(std::make_unique<Impl>(proto, durationMs))
{}

WriteBuzzerCommand::~WriteBuzzerCommand() = default;

void WriteBuzzerCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "CMD_BUZZER executado: duration={} ms", impl_->duration
        );
    } catch (const std::exception& e) {
        impl_->logger->error("CMD_BUZZER falhou: {}", e.what());
        throw;
    }
}

} // namespace command::keyboard
