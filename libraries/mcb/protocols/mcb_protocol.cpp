#include "mcb_protocol.h"
#include "../config/mcb_constants.h"                 // STX, ETX, MCBCommand, etc.
#include "../../../utils/logger.h"                   // Logger::instance()
#include "../../../utils/enum_/enum_commandcontext.h"// CommandContext
#include "../../../utils/enum_/enum_errorcode.h"     // ErrorCode
#include <numeric>   // std::accumulate
#include <cstdint>
#include <cstdio>    // std::snprintf
#include <optional>
#include <vector>

namespace mcb::protocols {

using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

/**
 * \class MCBProtocol::Impl
 * \brief Implementação interna (PImpl) do MCBProtocol.
 */
class MCBProtocol::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    std::vector<uint8_t> buildFrame(mcb::config::MCBCommand cmd, 
                                    const std::vector<uint8_t>& payload) const
    {
        Logger::instance().debug(CommandContext::HARDWARE,
            "Montando frame para comando 0x" + toHex(static_cast<uint8_t>(cmd)));

        // Frame = [STX, length, command, data..., checksum, ETX]
        std::vector<uint8_t> frame;
        frame.reserve(5 + payload.size());

        // 1) STX
        frame.push_back(mcb::config::STX);

        // 2) length = 1 (comando) + payload.size()
        uint8_t length = static_cast<uint8_t>(1 + payload.size());
        frame.push_back(length);

        // 3) comando
        frame.push_back(static_cast<uint8_t>(cmd));

        // 4) data
        frame.insert(frame.end(), payload.begin(), payload.end());

        // 5) checksum (Comando + Data)
        uint8_t chksum = calculateChecksum(cmd, payload);
        frame.push_back(chksum);

        // 6) ETX
        frame.push_back(mcb::config::ETX);

        Logger::instance().debug(CommandContext::HARDWARE,
            "Frame montado com " + std::to_string(frame.size()) + " bytes.");
        return frame;
    }

    std::optional<MCBFrame> parseFrame(const std::vector<uint8_t>& buffer) const {
        if (buffer.size() < mcb::config::MCB_MIN_FRAME_SIZE) {
            Logger::instance().warning(CommandContext::HARDWARE, 
                                       ErrorCode::ProtocolError,
                                       "Buffer muito curto para ser um frame MCB.");
            return std::nullopt;
        }

        if (buffer.front() != mcb::config::STX) {
            Logger::instance().warning(CommandContext::HARDWARE, 
                                       ErrorCode::ProtocolError,
                                       "STX inválido: 0x" + toHex(buffer.front()));
            return std::nullopt;
        }
        if (buffer.back() != mcb::config::ETX) {
            Logger::instance().warning(CommandContext::HARDWARE, 
                                       ErrorCode::ProtocolError,
                                       "ETX inválido: 0x" + toHex(buffer.back()));
            return std::nullopt;
        }

        uint8_t length = buffer[1];
        // Tamanho total esperado = STX(1) + Tamanho(1) + length + Checksum(1) + ETX(1)
        if (buffer.size() != static_cast<size_t>(length + 4)) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "Tamanho inconsistente. Esperado=" 
                                       + std::to_string(length + 4)
                                       + ", real=" + std::to_string(buffer.size()));
            return std::nullopt;
        }

        uint8_t cmdByte = buffer[2];
        mcb::config::MCBCommand cmd = toMCBCommand(cmdByte);

        size_t dataSize = length - 1;
        std::vector<uint8_t> data;
        data.reserve(dataSize);
        if (dataSize > 0) {
            data.insert(data.end(), buffer.begin() + 3, buffer.begin() + 3 + dataSize);
        }

        uint8_t chksum = buffer[3 + dataSize];
        uint8_t computed = calculateChecksum(cmd, data);
        if (computed != chksum) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "Checksum inválido. Calc=0x" + toHex(computed) 
                                       + ", recebido=0x" + toHex(chksum));
            return std::nullopt;
        }

        // Monta o frame final
        MCBFrame frame;
        frame.command = cmd;
        frame.data    = std::move(data);

        Logger::instance().debug(CommandContext::HARDWARE,
            "Frame parseado com sucesso. Comando=0x" + toHex(cmdByte) 
            + ", dataSize=" + std::to_string(frame.data.size()));
        return frame;
    }

private:
    static std::string toHex(uint8_t b) {
        char buf[4];
        std::snprintf(buf, sizeof(buf), "%02X", b);
        return std::string(buf);
    }

    static mcb::config::MCBCommand toMCBCommand(uint8_t cmdByte) {
        using mcb::config::MCBCommand;
        switch (cmdByte) {
            case 0x51: return MCBCommand::READ_FIRMWARE;
            case 0x52: return MCBCommand::READ_STATUS;
            default:   return MCBCommand::UNKNOWN;
        }
    }

    static uint8_t calculateChecksum(mcb::config::MCBCommand cmd,
                                     const std::vector<uint8_t>& payload)
    {
        uint32_t sum = static_cast<uint8_t>(cmd);
        for (auto b : payload) {
            sum += b;
        }
        return static_cast<uint8_t>(sum & 0xFF);
    }
};

// =========== MCBProtocol ===========

MCBProtocol::MCBProtocol()
    : pImpl_(std::make_unique<Impl>())
{
}

MCBProtocol::~MCBProtocol() = default;

std::vector<uint8_t> MCBProtocol::buildFrame(mcb::config::MCBCommand cmd,
                                             const std::vector<uint8_t>& payload) const
{
    return pImpl_->buildFrame(cmd, payload);
}

std::optional<MCBFrame> MCBProtocol::parseFrame(const std::vector<uint8_t>& buffer) const {
    return pImpl_->parseFrame(buffer);
}

} // namespace mcb::protocols
