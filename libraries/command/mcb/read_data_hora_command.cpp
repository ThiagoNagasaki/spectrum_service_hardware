// commands/read_data_hora_command.cpp
#include "read_data_hora_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace command::mcb {

struct ReadDataHoraCommand::Impl {
    explicit Impl(std::shared_ptr<protocol::IProtocol> p)
        : protocol(std::move(p))
        , logger(spdlog::default_logger())
    {}

    DataHoraInfo run() {
        // envia comando 0x5B sem payload
        auto data = protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::READ_DATA_HORA),
            {}
        );

        // valida resposta de 9 bytes
        if (data.size() < 9) {
            throw std::runtime_error(
                "READ_DATA_HORA: payload inesperado, esperado >=9 bytes"
            );
        }
        return DataHoraInfo{
            /*second=*/   data[1],
            /*minute=*/   data[2],
            /*hour=*/     data[3],
            /*weekday=*/  data[4],
            /*day=*/      data[5],
            /*month=*/    data[6],
            /*year=*/     static_cast<uint16_t>(data[7]),
            /*control=*/  data[8]
        };
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    std::shared_ptr<spdlog::logger>      logger;
};

ReadDataHoraCommand::ReadDataHoraCommand(
    std::shared_ptr<protocol::IProtocol> proto
) : impl_(std::make_unique<Impl>(proto))
{}

ReadDataHoraCommand::~ReadDataHoraCommand() = default;

DataHoraInfo ReadDataHoraCommand::execute() {
    try {
        auto info = impl_->run();
        impl_->logger->info(
            "READ_DATA_HORA executado: {}/{}/{} {}:{:02}:{:02} ctrl={}",
            info.day, info.month, info.year,
            (info.hour & 0x40) ? ((info.hour & 0x1F) % 12) : info.hour,
            info.minute, info.second,
            info.control
        );
        return info;
    } catch (const std::exception& e) {
        impl_->logger->error("READ_DATA_HORA falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
