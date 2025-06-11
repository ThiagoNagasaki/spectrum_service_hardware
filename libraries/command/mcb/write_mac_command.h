// commands/write_mac_command.h
#ifndef WRITE_MAC_COMMAND_H
#define WRITE_MAC_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Alteração de MAC Address pelo comando WRITE_MAC (0x66).
 *
 * Payload de 6 bytes: MAC[0]…MAC[5] :contentReference[oaicite:1]{index=1}
 */
struct MacAddr {
    std::array<uint8_t,6> mac;
};

class WriteMacCommand : public ICommand<void> {
public:
    WriteMacCommand(std::shared_ptr<protocol::IProtocol> protocol,
                    MacAddr mac);
    ~WriteMacCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_MAC_COMMAND_H
