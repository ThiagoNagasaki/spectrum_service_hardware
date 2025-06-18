// commands/write_info2_command.h
#ifndef WRITE_INFO2_COMMAND_H
#define WRITE_INFO2_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_INFO2 (0x6E): escreve 15 bytes de identificação na posição INFO2 da EEPROM.
 *
 * OBS.: sequência de 15 bytes todos zero é inválida. :contentReference[oaicite:2]{index=2}
 */
class WriteInfo2Command : public ICommand<void> {
public:
    explicit WriteInfo2Command(std::shared_ptr<protocols::IProtocol> protocols,
                               const std::array<uint8_t, 15>& data);
    ~WriteInfo2Command() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_INFO2_COMMAND_H
