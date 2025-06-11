// commands/read_info2_command.h
#ifndef READ_INFO2_COMMAND_H
#define READ_INFO2_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Representa os 15 bytes de identificação lidos pelo comando 0x5E (READ_INFO2).
 * Se inválido (tudo zero), nenhum byte é retornado. :contentReference[oaicite:2]{index=2}
 */
struct Info2Info {
    std::array<uint8_t,15> info;
};

/**
 * \brief Comando READ_INFO2 (0x5E) da MCB.
 */
class ReadInfo2Command
  : public ICommand<Info2Info>
{
public:
    explicit ReadInfo2Command(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadInfo2Command() override;

    /// Executa o comando e retorna 15 bytes de identificação.
    Info2Info execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_INFO2_COMMAND_H
