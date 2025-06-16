// commands/read_chave_liga_command.cpp
#include "read_chave_liga_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadChaveLigaCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
        : protocol(std::move(p))
        , logger(spdlog::default_logger())
    {}

    ChaveLigaInfo run() {
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_CHAVE_LIGA),
            {}
        );

        if (data.size() < 1) {
            throw std::runtime_error(
                "READ_CHAVE_LIGA: payload inesperado, esperado >=1 byte"
            );
        }
        return ChaveLigaInfo{
            /*ligado=*/ static_cast<bool>(data[0])
        };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadChaveLigaCommand::ReadChaveLigaCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadChaveLigaCommand::~ReadChaveLigaCommand() = default;

ChaveLigaInfo ReadChaveLigaCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_CHAVE_LIGA executado: ligado={}", info.ligado
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_CHAVE_LIGA falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
