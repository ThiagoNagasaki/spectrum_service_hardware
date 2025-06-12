// protocols/mcb_keyboard/mcb_keyboard_protocol.h
#ifndef PROTOCOLS_MCB_KEYBOARD_MCB_KEYBOARD_PROTOCOL_H
#define PROTOCOLS_MCB_KEYBOARD_MCB_KEYBOARD_PROTOCOL_H
#include "../../../utils/enum_/mcb_port_addresses.h"
#include "../../../utils/logger/logger.h"  
#include <vector>
#include <optional>
#include <cstdint>
#include <memory>

           

namespace protocols::mcb_keyboard {
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;
using utils::enum_::MCBCommand;
using utils::enum_::STX;
using utils::enum_::ETX;
using utils::enum_::MCB_MIN_FRAME_SIZE;
/**
 * \brief Frame MCB decodificado (após parseFrame).
 */
struct MCBFrame {
    MCBCommand command;  ///< comando
    std::vector<uint8_t>    data;     ///< payload (bytes de dados)
};

/**
 * \class MCBProtocol
 * \brief Monta e desmonta frames MCB com PImpl e logs em spdlog.
 */
class MCBProtocol {
public:
    MCBProtocol();
    ~MCBProtocol();

    /// Gera frame completo: STX | length | cmd | data.. | checksum | ETX
    std::vector<uint8_t> buildFrame(
        MCBCommand cmd,
        const std::vector<uint8_t>& payload
    ) const;

    /// Tenta parsear um buffer como frame MCB e retorna MCBFrame.
    std::optional<MCBFrame> parseFrame(
        const std::vector<uint8_t>& buffer
    ) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace protocols::mcb_keyboard

#endif // PROTOCOLS_MCB_KEYBOARD_MCB_KEYBOARD_PROTOCOL_H
