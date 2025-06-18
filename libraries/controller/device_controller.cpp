#include "device_controller.h"
#include "../command/mcb/read_firmware_command.h"
#include <fmt/core.h>
#include <iostream>
namespace controller {

DeviceController::DeviceController(std::shared_ptr<transport::interface::ITransport> transport,
                                   std::shared_ptr<ProtocolController> controller)
    : transport_(std::move(transport)), controller_(std::move(controller)) {}

bool DeviceController::connect() {
    return transport_->connect();
}

void DeviceController::disconnect() {
    transport_->disconnect();
}

std::string DeviceController::versaoFirmware() {
      std::cout << "Chegou em versaoFirmware !\n";
    command::mcb::ReadFirmwareCommand cmd(controller_);
    auto info = cmd.execute();
    return fmt::format("v{}.{}.{}", info.version, info.revision, info.build);
}

} // namespace controller
