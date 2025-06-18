// commands/write_baud_uart1_command.cpp
#include "write_baud_uart1_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>


namespace command::mcb {

struct WriteBaudUart1Command::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p,
         uint8_t speed,
         uint8_t parity)
      : protocols(std::move(p))
      , speedCode(speed)
      , parityCode(parity)
      , logger(spdlog::default_logger())
    {
        if (speedCode > 0x08) {
            throw std::invalid_argument("WRITE_BAUD_UART1: speedCode inválido");
        }
        if (parityCode > 0x03) {
            throw std::invalid_argument("WRITE_BAUD_UART1: parityCode inválido");
        }
    }

    void run() {
        protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_BAUD_UART1),
            { speedCode, parityCode }
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    uint8_t speedCode;
    uint8_t parityCode;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteBaudUart1Command::WriteBaudUart1Command(
    std::shared_ptr<protocols::IProtocol> proto,
    uint8_t speedCode,
    uint8_t parityCode
) : impl_(std::make_unique<Impl>(proto, speedCode, parityCode))
{}

WriteBaudUart1Command::~WriteBaudUart1Command() = default;

void WriteBaudUart1Command::execute() {
    try {
        impl_->run();
        impl_->logger->info(
            "WRITE_BAUD_UART1 executado: speedCode=0x{:02X}, parityCode=0x{:02X}",
            impl_->speedCode, impl_->parityCode
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_BAUD_UART1 falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
