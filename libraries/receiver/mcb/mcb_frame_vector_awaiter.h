// receiver/mcb/mcb_frame_vector_awaiter.h
#pragma once
#include "../i_receiver.h"
#include "mcb_frame_awaiter.h"
#include "../../protocols/mcb_keyboard/mcb_keyboard_protocol.h"

namespace receiver::mcb {

class MCBFrameVectorAwaiter : public IFrameAwaiter<std::vector<uint8_t>> {
public:
    explicit MCBFrameVectorAwaiter(std::shared_ptr<transport::interface::ITransport> transport)
        : inner_(transport, nullptr) {}

    std::vector<uint8_t> waitForFrame() override {
        auto frame = inner_.waitForFrame();
        std::vector<uint8_t> raw;
        raw.push_back(static_cast<uint8_t>(frame.command));
        raw.insert(raw.end(), frame.data.begin(), frame.data.end());
        return raw;
    }

private:
    MCBFrameAwaiter inner_;
};

} // namespace receiver::mcb
