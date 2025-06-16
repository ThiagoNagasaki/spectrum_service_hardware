// commands/write_data_hora_command.cpp
#include "write_data_hora_command.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vector>

namespace command::mcb {

struct WriteDataHoraCommand::Impl {
    Impl(std::shared_ptr<protocol::IProtocol> p,
         const DataHoraInfo& dh)
      : protocol(std::move(p))
      , data(dh)
      , logger(spdlog::default_logger())
    {}

    void run() {
        std::vector<uint8_t> payload(9);
        payload[0] = 0;                              
        payload[1] = data.second;
        payload[2] = data.minute;
        payload[3] = data.hour;
        payload[4] = data.weekday;
        payload[5] = data.day;
        payload[6] = data.month;
        payload[7] = static_cast<uint8_t>(data.year);
        payload[8] = data.control;

        protocol->sendCommand(
            static_cast<uint8_t>(utils::enum_::MCBCommand::WRITE_DATA_HORA),
            payload
        );
    }

    std::shared_ptr<protocol::IProtocol> protocol;
    DataHoraInfo                        data;
    std::shared_ptr<spdlog::logger>     logger;
};

WriteDataHoraCommand::WriteDataHoraCommand(
    std::shared_ptr<protocol::IProtocol> proto,
    const DataHoraInfo& dataHora
) : impl_(std::make_unique<Impl>(proto, dataHora))
{}

WriteDataHoraCommand::~WriteDataHoraCommand() = default;

void WriteDataHoraCommand::execute() {
    try {
        impl_->run();
        // loga no formato dd/mm/yy hh:mm:ss ctrl=x
        impl_->logger->info(
            "WRITE_DATA_HORA executado: {}/{}/{} {:02}:{:02}:{:02} ctrl={}",
            impl_->data.day,   impl_->data.month,
            impl_->data.year,  impl_->data.hour,
            impl_->data.minute,impl_->data.second,
            impl_->data.control
        );
    } catch (const std::exception& e) {
        impl_->logger->error("WRITE_DATA_HORA falhou: {}", e.what());
        throw;
    }
}

} // namespace command::mcb
