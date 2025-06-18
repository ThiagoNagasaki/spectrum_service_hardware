#pragma once
#include "../../../utils/enum_/mcb_port_addresses.h"
#include "../../../utils/logger/logger.h"  
#include "../../../libraries/protocols/i_protocol.h"  
#include "../../../libraries/transport/interface/i_transport.h" 
#include "../../../libraries/receiver/i_receiver.h" 

#include <vector>
#include <optional>
#include <cstdint>
#include <memory>
#include <functional>

namespace protocols::mcb_keyboard {

using utils::enum_::MCBCommand;

/**
 * \brief Frame MCB decodificado (após parseFrame).
 */
struct MCBFrame {
    MCBCommand command; 
    std::vector<uint8_t> data;
};

class MCBProtocol : public protocols::IProtocol {
public:
    MCBProtocol(std::shared_ptr<transport::interface::ITransport> transport,
                std::shared_ptr<receiver::IReceiver<MCBFrame>> receiver);
    ~MCBProtocol();

    std::vector<uint8_t> buildFrame(uint8_t cmd, const std::vector<uint8_t>& payload) override;
    std::optional<std::vector<uint8_t>> parseFrame(const std::vector<uint8_t>& buffer) override;
    void subscribe(std::function<void(const std::vector<uint8_t>&)> cb) override;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace protocols::mcb_keyboard
