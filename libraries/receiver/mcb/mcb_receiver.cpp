// libraries/receiver/mcb/mcb_receiver.cpp
#include "mcb_receiver.h"
#include <spdlog/spdlog.h>
#include "../../libraries/command/mcb/read_status_command.h"
#include "../../libraries/command/mcb/read_analog_input_command.h"
#include "../../libraries/command/mcb/read_data_hora_command.h"
#include "../../utils/enum_/mcb_port_addresses.h"

namespace command::mcb::receiver {
using utils::enum_::MCBCommand;
using utils::enum_::AnalogInputPort;
McbReceiver::McbReceiver(std::shared_ptr<protocol::IProtocol> proto)
  : protocol_(std::move(proto))
{
    protocol_->subscribe([this](const Frame& frame) {
        try {
            dispatch(frame);
        } catch (const std::exception& e) {
            spdlog::default_logger()->error("MCBReceiver dispatch erro: {}", e.what());
        }
    });
}

McbReceiver::~McbReceiver() = default;

void McbReceiver::setStatusHandler(std::shared_ptr<IReceiver<StatusInfo>> h) {
    statusHandler_ = std::move(h);
}

void McbReceiver::setAnalogHandler(std::shared_ptr<IReceiver<AnalogInputInfo>> h) {
    analogHandler_ = std::move(h);
}

void McbReceiver::setDateTimeHandler(std::shared_ptr<IReceiver<DataHoraInfo>> h) {
    dateTimeHandler_ = std::move(h);
}

void McbReceiver::dispatch(const Frame& frame) {
    if (frame.size() < 2) return;
    uint8_t cmd = frame[0];
    switch (static_cast<MCBCommand>(cmd)) {
        case MCBCommand::READ_STATUS:
            if (statusHandler_) {
                ReadStatusCommand c(protocol_);
                auto info = c.execute();
                statusHandler_->onReceive(info);
            }
            break;
        case MCBCommand::READ_ANALOG_INPUT:
            if (analogHandler_ && frame.size() >= 3) {
                auto port = static_cast<AnalogInputPort>(frame[1]);
                ReadAnalogInputCommand c(protocol_, port);
                auto info = c.execute();
                analogHandler_->onReceive(info);
            }
            break;
        case MCBCommand::READ_DATA_HORA:
            if (dateTimeHandler_) {
                ReadDataHoraCommand c(protocol_);
                auto info = c.execute();
                dateTimeHandler_->onReceive(info);
            }
            break;
        default:
            break;
    }
}

} // namespace command::mcb::receiver
