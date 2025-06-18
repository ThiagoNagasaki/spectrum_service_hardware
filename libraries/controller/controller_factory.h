#pragma once

#include <memory>
#include <string>
#include "../controller/device_controller.h"
#include "protocol_controller.h"
#include "../transport/network/tcp_transport.h"
#include "../protocols/mcb_keyboard/mcb_keyboard_protocol.h"
namespace controller {
class ControllerFactory {
public:
    static ControllerFactory buildMCB();

    ControllerFactory& withTCP(const std::string& ip, uint16_t port);

    controller::DeviceController build();

private:
    transport::network::TCPConfig tcp_config_;
    bool tcp_config_set_ = false;
};
}