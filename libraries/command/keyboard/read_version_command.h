// commands/keyboard/read_version_command.h
#ifndef READ_VERSION_COMMAND_H
#define READ_VERSION_COMMAND_H

#include "../../utils/translator/response_translator.h"
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/keyboard_keycodes.h"
#include <memory>
#include <cstdint>
#include <string>  // para std::string

namespace command::keyboard {

/**
 * \brief Informações de versão de firmware do teclado (CMD_VERSION – 0x71).
 *
 * Byte 0: versão  
 * Byte 1: revisão :contentReference[oaicite:1]{index=1}
 */
struct VersionInfo {
    uint8_t version;  ///< versão principal
    uint8_t revision; ///< número de revisão

    /// Retorna algo como "v1 rev3"
    std::string toString() const {
        return "v" + std::to_string(version)
             + " rev" + std::to_string(revision);
    }
};

/**
 * \brief Comando CMD_VERSION (0x71): lê versão do firmware do teclado.
 */
class ReadVersionCommand : public ICommand<VersionInfo> {
public:
    explicit ReadVersionCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadVersionCommand() override;

    VersionInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::keyboard

#endif // READ_VERSION_COMMAND_H
