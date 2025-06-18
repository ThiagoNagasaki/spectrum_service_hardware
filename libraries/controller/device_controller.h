#pragma once

#include <memory>
#include <string>
#include "protocol_controller.h"
#include "../transport/interface/i_transport.h"

namespace controller {

class DeviceController {
public:
    DeviceController(std::shared_ptr<transport::interface::ITransport> transport,
                     std::shared_ptr<ProtocolController> controller);

    bool connect();
    void disconnect();

    std::string versaoFirmware();  // Comando 0x51

private:
    std::shared_ptr<transport::interface::ITransport> transport_;
    std::shared_ptr<ProtocolController> controller_;
};

} // namespace controller
