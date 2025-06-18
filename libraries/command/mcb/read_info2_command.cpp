// commands/read_info2_command.cpp
#include "read_info2_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadInfo2Command::Impl {
    explicit Impl(std::shared_ptr<protocols::IProtocol> p)
        : protocols(std::move(p))
        , logger(spdlog::default_logger())
    {}

    Info2Info run() {
        auto data = protocols->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_INFO2),
            {}
        );

        if (data.size() < 15) {
            throw std::runtime_error(
                "READ_INFO2: payload inesperado, esperado >=15 bytes"
            );
        }

        Info2Info info;
        std::copy_n(data.begin(), 15, info.info.begin());
        return info;
    }

    std::shared_ptr<protocols::IProtocol> protocols;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadInfo2Command::ReadInfo2Command(
    std::shared_ptr<protocols::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadInfo2Command::~ReadInfo2Command() = default;

Info2Info ReadInfo2Command::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_INFO2 executado: recuperados {} bytes", info.info.size()
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_INFO2 falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
