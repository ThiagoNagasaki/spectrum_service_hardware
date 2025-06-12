// libraries/receiver/mcb/mcb_receiver.h
#ifndef MCB_RECEIVER_H
#define MCB_RECEIVER_H

#include "../../libraries/receiver/i_receiver.h"
#include "../../libraries/command/mcb/read_status_command.h"
#include "../../libraries/command/mcb/read_analog_input_command.h"
#include "../../libraries/command/mcb/read_data_hora_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include <memory>
#include <vector>

namespace receiver::mcb {

/**
 * \brief “Listener” de mensagens vindas da MCB.
 *
 * Despacha READ_STATUS, READ_ANALOG_INPUT e READ_DATA_HORA via IReceiver<T>.
 */
class McbReceiver {
public:
    using Frame = std::vector<uint8_t>;

    explicit McbReceiver(std::shared_ptr<protocol::IProtocol> proto);
    ~McbReceiver();

    void setStatusHandler(std::shared_ptr<IReceiver<command::mcb::StatusInfo>>     handler);
    void setAnalogHandler(std::shared_ptr<IReceiver<command::mcb::AnalogInputInfo>> handler);
    void setDateTimeHandler(std::shared_ptr<IReceiver<command::mcb::DataHoraInfo>>  handler);

private:
    void dispatch(const Frame& frame);

    std::shared_ptr<protocol::IProtocol>                             protocol_;
    std::shared_ptr<IReceiver<command::mcb::StatusInfo>>             statusHandler_;
    std::shared_ptr<IReceiver<command::mcb::AnalogInputInfo>>        analogHandler_;
    std::shared_ptr<IReceiver<command::mcb::DataHoraInfo>>           dateTimeHandler_;
};

} // namespace command::mcb::receiver

#endif // MCB_RECEIVER_H
