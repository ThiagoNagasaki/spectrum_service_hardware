#pragma once

#include "../protocols/i_protocol.h"
#include "../transport/interface/i_transport.h"
#include "../receiver/i_receiver.h"

#include <vector>
#include <cstdint>
#include <memory>
#include <optional>
namespace controller {
class ProtocolController {
public:
    ProtocolController(std::shared_ptr<protocols::IProtocol> protocol,
                       std::shared_ptr<transport::interface::ITransport> transport,
                       std::shared_ptr<receiver::IFrameAwaiter<std::vector<uint8_t>>> awaiter);

    std::vector<uint8_t> sendCommand(uint8_t cmd, const std::vector<uint8_t>& payload);

private:
    std::shared_ptr<protocols::IProtocol> protocol_;
    std::shared_ptr<transport::interface::ITransport> transport_;
    std::shared_ptr<receiver::IFrameAwaiter<std::vector<uint8_t>>> awaiter_;
};
}