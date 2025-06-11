// commands/desliga_mcb_command.cpp
#include "desliga_mcb_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {
using utils::enum_::MCBCommand;
struct DesligaMcbCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
      : protocol(std::move(p))
      , logger(spdlog::default_logger())
    {}

    void run() {
        protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::DESLIGA_MCB),
            {}
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger> logger;
};

DesligaMcbCommand::DesligaMcbCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

DesligaMcbCommand::~DesligaMcbCommand() = default;

void DesligaMcbCommand::execute() {
    try {
        impl_->run();
        impl_->logger->info("DESLIGA_MCB executado");
    } catch (const std::exception& e) {
        impl_->logger->error("DESLIGA_MCB falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
