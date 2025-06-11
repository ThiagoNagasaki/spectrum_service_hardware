// commands/write_mac_pc_command.h
#ifndef WRITE_MAC_PC_COMMAND_H
#define WRITE_MAC_PC_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "read_mac_pc_command.h"  // para MacPcInfo
#include <memory>

namespace command::mcb {

/**
 * \brief Comando WRITE_MAC_PC (0x6C): grava o endereço MAC do PC na MCB.
 *
 * Payload: 6 bytes (MAC[0]…MAC[5]) &#10;Relação conforme manual :contentReference[oaicite:2]{index=2}
 */
class WriteMacPcCommand : public ICommand<void> {
public:
    WriteMacPcCommand(std::shared_ptr<protocol::IProtocol> protocol,
                      const MacPcInfo& mac);
    ~WriteMacPcCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_MAC_PC_COMMAND_H
