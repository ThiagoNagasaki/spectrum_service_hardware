// commands/read_mac_pc_command.h
#ifndef READ_MAC_PC_COMMAND_H
#define READ_MAC_PC_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Representa o endereço MAC do PC lido pelo comando 0x5C (READ_MAC_PC).
 * Relação de bytes do endereço MAC do PC: Byte0:Byte1:Byte2:Byte3:Byte4:Byte5 :contentReference[oaicite:0]{index=0}
 */
struct MacPcInfo {
    std::array<uint8_t,6> mac;
};

/**
 * \brief Comando READ_MAC_PC (0x5C) da MCB.
 */
class ReadMacPcCommand
  : public ICommand<MacPcInfo>
{
public:
    explicit ReadMacPcCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadMacPcCommand() override;

    /// Executa o comando e retorna o endereço MAC do PC.
    MacPcInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_MAC_PC_COMMAND_H
