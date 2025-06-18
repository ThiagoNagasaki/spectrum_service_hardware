#ifndef READ_FIRMWARE_COMMAND_H
#define READ_FIRMWARE_COMMAND_H

#include "../../../libraries/command/i_command.h"
#include "../../../libraries/controller/protocol_controller.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

struct FirmwareInfo {
    uint8_t version;
    uint8_t revision;
    uint8_t build;
};

class ReadFirmwareCommand 
  : public ICommand<FirmwareInfo>
{
public:
    explicit ReadFirmwareCommand(std::shared_ptr<controller::ProtocolController> controller);
    ~ReadFirmwareCommand() override;

    FirmwareInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_FIRMWARE_COMMAND_H
