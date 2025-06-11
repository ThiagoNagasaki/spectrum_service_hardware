// commands/read_info1_command.h
#ifndef READ_INFO1_COMMAND_H
#define READ_INFO1_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Representa os 15 bytes de identificação lidos pelo comando 0x5D (READ_INFO1).
 * Se inválido (tudo zero), nenhum byte é retornado. :contentReference[oaicite:1]{index=1}
 */
struct Info1Info {
    std::array<uint8_t,15> info;
};

/**
 * \brief Comando READ_INFO1 (0x5D) da MCB.
 */
class ReadInfo1Command
  : public ICommand<Info1Info>
{
public:
    explicit ReadInfo1Command(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadInfo1Command() override;

    /// Executa o comando e retorna 15 bytes de identificação.
    Info1Info execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_INFO1_COMMAND_H
