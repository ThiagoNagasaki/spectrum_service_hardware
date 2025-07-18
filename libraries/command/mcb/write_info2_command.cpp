// commands/write_info2_command.cpp
#include "write_info2_command.h"
#include <spdlog/spdlog.h>
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace command::mcb {

struct WriteInfo2Command::Impl {
    Impl(std::shared_ptr<protocols::IProtocol> p,
         const std::array<uint8_t, 15>& d)
      : protocols(std::move(p))
      , data(d)
      , logger(spdlog::default_logger())
    {
        if (std::all_of(data.begin(), data.end(), [](auto b){ return b == 0; })) {
            throw std::invalid_argument("WRITE_INFO2: dado inválido, não pode ser todos zeros");
        }
    }

    void run() {
        std::vector<uint8_t> payload(data.begin(), data.end());
        protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_INFO2),
            payload
        );
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    std::array<uint8_t, 15>               data;
    std::shared_ptr<spdlog::logger>       logger;
};

WriteInfo2Command::WriteInfo2Command(
    std::shared_ptr<protocols::IProtocol> proto,
    const std::array<uint8_t, 15>& data
) : impl_(std::make_unique<Impl>(proto, data))
{}

WriteInfo2Command::~WriteInfo2Command() = default;

void WriteInfo2Command::execute() {
    try {
        impl_->run();
        impl_->logger->info("WRITE_INFO2 executado: 15 bytes gravados em INFO2");
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_INFO2 falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
