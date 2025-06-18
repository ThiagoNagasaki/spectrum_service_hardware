// commands/read_info1_command.cpp
#include "read_info1_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadInfo1Command::Impl {
    explicit Impl(std::shared_ptr<protocols::IProtocol> p)
        : protocols(std::move(p))
        , logger(spdlog::default_logger())
    {}

    Info1Info run() {
        auto data = protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_INFO1),
            {}
        );

        if (data.size() < 15) {
            throw std::runtime_error(
                "READ_INFO1: payload inesperado, esperado >=15 bytes"
            );
        }

        Info1Info info;
        std::copy_n(data.begin(), 15, info.info.begin());
        return info;
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadInfo1Command::ReadInfo1Command(
    std::shared_ptr<protocols::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadInfo1Command::~ReadInfo1Command() = default;

Info1Info ReadInfo1Command::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_INFO1 executado: recuperados {} bytes", info.info.size()
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_INFO1 falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
