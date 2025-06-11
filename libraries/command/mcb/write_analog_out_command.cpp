// commands/write_analog_out_command.cpp
#include "write_analog_out_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <stdexcept>

namespace command::mcb {
using utils::enum_::MCBCommand;
struct WriteAnalogOutCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p,
         utils::enum_::AnalogInputPort addr,
         uint8_t val)
      : protocol(std::move(p))
      , address(addr)
      , dacValue(val)
      , logger(spdlog::default_logger())
    {}

    void run() {
        // converte enum para byte ao montar o payload
        std::vector<uint8_t> payload{
            static_cast<uint8_t>(address),
            dacValue
        };
        protocol->sendCommand(
            static_cast<uint8_t>(MCBCommand::WRITE_ANALOG_OUT),
            payload
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    utils::enum_::AnalogInputPort       address;
    uint8_t                              dacValue;
    std::shared_ptr<spdlog::logger>      logger;
};

WriteAnalogOutCommand::WriteAnalogOutCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    utils::enum_::AnalogInputPort address,
    uint8_t dacValue
) : impl_(std::make_unique<Impl>(proto, address, dacValue))
{}

WriteAnalogOutCommand::~WriteAnalogOutCommand() = default;

void WriteAnalogOutCommand::execute() {
    try {
        impl_->run();
        // converte enum para byte ao logar
        impl_->logger->info(
            "WRITE_ANALOG_OUT executado: address=0x{:02X}, dacValue=0x{:02X}",
            static_cast<uint8_t>(impl_->address),
            impl_->dacValue
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_ANALOG_OUT falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
