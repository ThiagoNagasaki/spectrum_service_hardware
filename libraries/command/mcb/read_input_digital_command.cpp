// commands/read_input_digital_command.cpp
#include "read_input_digital_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadInputDigitalCommand::Impl {
  Impl(std::shared_ptr<protocol::IProtocol> p,
       utils::enum_::DigitalInputPort addr)
      : protocol(std::move(p)), address(addr),
        logger(spdlog::default_logger()) {}

  DigitalInputInfo run() {
    // envia [0x53, address]
    std::vector<uint8_t> payload{static_cast<uint8_t>(address)};
    auto data = protocol->sendCommand(
        static_cast<uint8_t>(utils::enum_::MCBCommand::READ_INPUT_DIGITAL),
        payload);

    if (data.empty()) {
      throw std::runtime_error(
          "READ_INPUT_DIGITAL: payload vazio (esperado 1 byte)");
    }
    // 0→Off, !=0→On
    return DigitalInputInfo{data[0] != 0};
  }

  std::shared_ptr<protocol::IProtocol> protocol;
  utils::enum_::DigitalInputPort address;
  std::shared_ptr<spdlog::logger> logger;
};

ReadInputDigitalCommand::ReadInputDigitalCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    utils::enum_::DigitalInputPort inputAddress)
    : impl_(std::make_unique<Impl>(proto, inputAddress)) {}

ReadInputDigitalCommand::~ReadInputDigitalCommand() = default;

DigitalInputInfo ReadInputDigitalCommand::execute() {
  try {
    auto info = impl_->run();
    impl_->logger->info("READ_INPUT_DIGITAL[0x{:02X}] → {}",
                        static_cast<uint8_t>(impl_->address), info.toString());

    return info;
  } catch (const std::exception &e) {
    impl_->logger->error("READ_INPUT_DIGITAL falhou: {}", e.what());
    throw;
  }
}

} // namespace command::mcb
