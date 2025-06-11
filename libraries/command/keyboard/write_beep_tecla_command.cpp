// commands/keyboard/write_beep_tecla_command.cpp
#include "write_beep_tecla_command.h"
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
struct WriteBeepTeclaCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p, bool en)
      : protocol(std::move(p)), enabled(en),
        logger(spdlog::default_logger())
    {}

    void run() {
        uint8_t val = enabled ? 1 : 0;
        protocol->sendCommand(
            static_cast<uint8_t>(MCBCommand::CMD_BEEP_TECLA),
            { val }
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    bool                                 enabled;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteBeepTeclaCommand::WriteBeepTeclaCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    bool enable
) : impl_(std::make_unique<Impl>(proto, enable))
{}

WriteBeepTeclaCommand::~WriteBeepTeclaCommand() = default;

void WriteBeepTeclaCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "CMD_BEEP_TECLA executado: {}", impl_->enabled ? "On" : "Off"
        );
    } catch (const std::exception& e) {
        impl_->logger->error("CMD_BEEP_TECLA falhou: {}", e.what());
        throw;
    }
}

} // namespace command::keyboard
