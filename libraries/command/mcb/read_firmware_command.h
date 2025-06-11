#ifndef READ_FIRMWARE_COMMAND_H
#define READ_FIRMWARE_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Estrutura com os três bytes de firmware retornados pelo comando 0x51:
 * Byte 0 = versão, Byte 1 = revisão, Byte 2 = build. :contentReference[oaicite:1]{index=1}
 */
struct FirmwareInfo {
    uint8_t version;
    uint8_t revision;
    uint8_t build;
};

/**
 * \brief Implementa o comando READ_FIRMWARE (0x51) da MCB.
 * Retorna um FirmwareInfo já parseado.
 */
class ReadFirmwareCommand 
  : public ICommand<FirmwareInfo>
{
public:
    explicit ReadFirmwareCommand(std::shared_ptr<protocol::IProtocol> proto);
    ~ReadFirmwareCommand() override;

    /// Executa o comando e retorna os três bytes de firmware.
    FirmwareInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_FIRMWARE_COMMAND_H
