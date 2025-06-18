// commands/desliga_mcb_command.h
#ifndef DESLIGA_MCB_COMMAND_H
#define DESLIGA_MCB_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>

namespace command::mcb {

/**
 * \brief Comando DESLIGA_MCB (0x6A): desliga a MCB.
 *
 * ACK deste comando não possui payload; se a chave liga/desliga estiver ligada,
 * a MCB não desliga e retorna o estado da chave pelo comando READ_CHAVE_LIGA :contentReference[oaicite:1]{index=1}.
 */
class DesligaMcbCommand : public ICommand<void> {
public:
    explicit DesligaMcbCommand(std::shared_ptr<protocols::IProtocol> protocols);
    ~DesligaMcbCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // DESLIGA_MCB_COMMAND_H
