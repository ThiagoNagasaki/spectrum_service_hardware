// commands/write_info3_command.h
#ifndef WRITE_INFO3_COMMAND_H
#define WRITE_INFO3_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_INFO3 (0x6F): escreve 15 bytes de identificação na posição INFO3 da EEPROM.
 *
 * OBS.: sequência de 15 bytes todos zero é inválida. :contentReference[oaicite:3]{index=3}
 */
class WriteInfo3Command : public ICommand<void> {
public:
    explicit WriteInfo3Command(std::shared_ptr<protocol::IProtocol> protocol,
                               const std::array<uint8_t, 15>& data);
    ~WriteInfo3Command() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_INFO3_COMMAND_H
