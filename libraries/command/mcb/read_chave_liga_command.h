// commands/read_chave_liga_command.h
#ifndef READ_CHAVE_LIGA_COMMAND_H
#define READ_CHAVE_LIGA_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
 
#include <memory>

namespace command::mcb {

/**
 * \brief Representa o estado lido pelo comando 0x5A (READ_CHAVE_LIGA).
 *
 * Retorna 1 se a chave liga/desliga está ligada, 0 caso contrário. :contentReference[oaicite:0]{index=0}
 */
struct ChaveLigaInfo {
    bool ligado;
};

/**
 * \brief Comando READ_CHAVE_LIGA (0x5A) da MCB.
 */
class ReadChaveLigaCommand
  : public ICommand<ChaveLigaInfo>
{
public:
    explicit ReadChaveLigaCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadChaveLigaCommand() override;

    /// Executa o comando e retorna o estado da chave liga/desliga.
    ChaveLigaInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_CHAVE_LIGA_COMMAND_H
