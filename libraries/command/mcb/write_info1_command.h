// commands/write_info1_command.h
#ifndef WRITE_INFO1_COMMAND_H
#define WRITE_INFO1_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_INFO1 (0x6D): escreve 15 bytes de identificação na posição INFO1 da EEPROM.
 *
 * OBS.: sequência de 15 bytes todos zero é inválida. :contentReference[oaicite:1]{index=1}
 */
class WriteInfo1Command : public ICommand<void> {
public:
    explicit WriteInfo1Command(std::shared_ptr<protocol::IProtocol> protocol,
                               const std::array<uint8_t, 15>& data);
    ~WriteInfo1Command() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_INFO1_COMMAND_H
