// receiver/mcb_frame_awaiter.h
#pragma once

#include "../receiver/i_receiver.h"
#include "../transport/interface/i_transport.h"
#include "../protocols/mcb_keyboard/mcb_keyboard_protocol.h"
#include <memory>

namespace receiver::mcb {

class MCBFrameAwaiter : public IFrameAwaiter<protocols::mcb_keyboard::MCBFrame> {
public:
    explicit MCBFrameAwaiter(std::shared_ptr<transport::interface::ITransport> transport,
    std::shared_ptr<receiver::IReceiver<protocols::mcb_keyboard::MCBFrame>> receiver);
    ~MCBFrameAwaiter() override = default;

    protocols::mcb_keyboard::MCBFrame waitForFrame() override;

private:
    std::shared_ptr<transport::interface::ITransport> transport_;
    protocols::mcb_keyboard::MCBProtocol protocol_;  
};

} // namespace receiver
