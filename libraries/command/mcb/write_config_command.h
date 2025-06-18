// commands/write_config_command.h
#ifndef WRITE_CONFIG_COMMAND_H
#define WRITE_CONFIG_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <vector>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Comando WRITE_CONFIG (0x62): grava configurações gerais na EEPROM.
 *
 * Payload (varia conforme o endereço) :contentReference[oaicite:1]{index=1}:
 *  - Byte 0: endereço da configuração
 *      • 0x00 – Desliga scanner em emergência
 *      • 0x01 – Debounce sensores de túnel 1/2
 *      • 0x02 – Debounce sensores de túnel 3/4
 *      • 0x03 – Correção de ângulo do sensor de distância
 *      • 0x04 – Tempo de desligamento da MCB (2 bytes: parte alta, parte baixa)
 *  - Bytes seguintes: valor ou parâmetros (1 ou 2 bytes)
 */
class WriteConfigCommand : public ICommand<void> {
public:
    WriteConfigCommand(std::shared_ptr<protocols::IProtocol> protocols,
                       utils::enum_::ConfigAddress address,
                       std::vector<uint8_t> values);
    ~WriteConfigCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_CONFIG_COMMAND_H
