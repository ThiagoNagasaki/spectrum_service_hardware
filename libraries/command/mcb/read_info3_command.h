// commands/read_info3_command.h
#ifndef READ_INFO3_COMMAND_H
#define READ_INFO3_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Representa os 15 bytes de identificação lidos pelo comando 0x5F (READ_INFO3).
 * Se inválido (tudo zero), nenhum byte é retornado. :contentReference[oaicite:3]{index=3}
 */
struct Info3Info {
    std::array<uint8_t,15> info;
};

/**
 * \brief Comando READ_INFO3 (0x5F) da MCB.
 */
class ReadInfo3Command
  : public ICommand<Info3Info>
{
public:
    explicit ReadInfo3Command(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadInfo3Command() override;

    /// Executa o comando e retorna 15 bytes de identificação.
    Info3Info execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_INFO3_COMMAND_H
