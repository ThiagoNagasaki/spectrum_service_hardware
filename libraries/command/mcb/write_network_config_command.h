// commands/write_network_config_command.h
#ifndef WRITE_NETWORK_CONFIG_COMMAND_H
#define WRITE_NETWORK_CONFIG_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <array>
#include <vector>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Configuração de rede para WRITE_NETWORK_CONFIG (0x65).
 *
 * Payload de 16 bytes:
 *  - Bytes 0–3:     Endereço IP (IP[0], IP[1], IP[2], IP[3])
 *  - Bytes 4–7:     Máscara de sub-rede (mask[0],…)
 *  - Bytes 8–11:    Gateway padrão (gw[0],…)
 *  - Bytes 12–15:   DNS (dns[0],…) 
 */
struct NetworkConfig {
    std::array<uint8_t,4> ip;
    std::array<uint8_t,4> mask;
    std::array<uint8_t,4> gateway;
    std::array<uint8_t,4> dns;
};

class WriteNetworkConfigCommand : public ICommand<void> {
public:
    WriteNetworkConfigCommand(std::shared_ptr<protocol::IProtocol> protocol,
                              NetworkConfig cfg);
    ~WriteNetworkConfigCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_NETWORK_CONFIG_COMMAND_H
