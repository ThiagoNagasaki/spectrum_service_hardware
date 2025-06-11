// protocols/mcb_keyboard/mcb_keyboard_protocol.cpp
#include "mcb_keyboard_protocol.h"

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"            // Logger::instance()
#include "../../utils/enum_/enum_commandcontext.h"// CommandContext
#include "../../utils/enum_/enum_errorcode.h"// ErrorCode

#include <algorithm>   // std::find
#include <numeric>     // std::accumulate
#include <optional>
#include <vector>
#include <cstdint>
#include <cstdio>      // std::snprintf
#include <fmt/core.h>

namespace protocols::mcb_keyboard {

using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

struct MCBProtocol::Impl {
    Impl() = default;
    ~Impl() = default;

    //======================================================================
    // Montagem de frame: STX | length | cmd | payload... | checksum | ETX
    //======================================================================
    std::vector<uint8_t> buildFrame(utils::enum_::MCBCommand cmd,
                                    const std::vector<uint8_t>& payload) const
    {
        Logger::instance().debug(CommandContext::HARDWARE,
            "Montando frame para comando 0x" + toHex(static_cast<uint8_t>(cmd)));

        std::vector<uint8_t> frame;
        frame.reserve(5 + payload.size());

        frame.push_back(utils::enum_::STX);

        uint8_t length = static_cast<uint8_t>(1 + payload.size());
        frame.push_back(length);

        frame.push_back(static_cast<uint8_t>(cmd));

        frame.insert(frame.end(), payload.begin(), payload.end());

        uint8_t chksum = calculateChecksum(cmd, payload);
        frame.push_back(chksum);

        frame.push_back(utils::enum_::ETX);

        Logger::instance().debug(CommandContext::HARDWARE,
            "Frame montado com " + std::to_string(frame.size()) + " bytes.");
        return frame;
    }

    //======================================================================
    // Desmontagem de frame, valida STX/ETX/length/checksum e retorna MCBFrame
    //======================================================================
    std::optional<MCBFrame> parseFrame(const std::vector<uint8_t>& buffer) const {
        if (buffer.size() < utils::enum_::MCB_MIN_FRAME_SIZE) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "Buffer muito curto para ser um frame MCB.");
            return std::nullopt;
        }
        if (buffer.front() != utils::enum_::STX) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "STX inválido: 0x" + toHex(buffer.front()));
            return std::nullopt;
        }
        if (buffer.back() != utils::enum_::ETX) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "ETX inválido: 0x" + toHex(buffer.back()));
            return std::nullopt;
        }

        uint8_t length = buffer[1];
        size_t expected = static_cast<size_t>(length) + 4;
        if (buffer.size() != expected) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "Tamanho inconsistente. Esperado="
                                       + std::to_string(expected)
                                       + ", real=" + std::to_string(buffer.size()));
            return std::nullopt;
        }

        uint8_t cmdByte = buffer[2];
        auto cmd = toMCBCommand(cmdByte);

        size_t dataSize = length - 1;
        std::vector<uint8_t> data;
        if (dataSize > 0) {
            data.insert(data.end(),
                        buffer.begin() + 3,
                        buffer.begin() + 3 + dataSize);
        }

        uint8_t receivedChksum = buffer[3 + dataSize];
        uint8_t computed = calculateChecksum(cmd, data);
        if (receivedChksum != computed) {
            Logger::instance().warning(CommandContext::HARDWARE,
                                       ErrorCode::ProtocolError,
                                       "Checksum inválido. Calc=0x" + toHex(computed)
                                       + ", recebido=0x" + toHex(receivedChksum));
            return std::nullopt;
        }

        MCBFrame frame;
        frame.command = cmd;
        frame.data = std::move(data);

        Logger::instance().debug(CommandContext::HARDWARE,
            "Frame parseado com sucesso. Comando=0x" + toHex(cmdByte)
            + ", dataSize=" + std::to_string(frame.data.size()));
        return frame;
    }

private:
    // converte byte em hex string “XX”
    static std::string toHex(uint8_t b) {
        char buf[4];
        std::snprintf(buf, sizeof(buf), "%02X", b);
        return buf;
    }

    // mapeia todo o range de comandos do manual :contentReference[oaicite:0]{index=0}
    static utils::enum_::MCBCommand toMCBCommand(uint8_t b) {
        using C = utils::enum_::MCBCommand;
        switch (b) {
            case 0x51: return C::READ_FIRMWARE;
            case 0x52: return C::READ_STATUS;
            case 0x53: return C::READ_INPUT_DIGITAL;
            case 0x54: return C::READ_ANALOG_INPUT;
            case 0x55: return C::READ_SENSOR_DISTANCE;
            case 0x56: return C::READ_STATUS_SPECIAL;
            case 0x5A: return C::READ_CHAVE_LIGA;
            case 0x5B: return C::READ_DATA_HORA;
            case 0x5C: return C::READ_MAC_PC;
            case 0x5D: return C::READ_INFO1;
            case 0x5E: return C::READ_INFO2;
            case 0x5F: return C::READ_INFO3;
            case 0x61: return C::WRITE_SPECIAL;
            case 0x62: return C::WRITE_CONFIG;
            case 0x63: return C::WRITE_DIGITAL_OUT;
            case 0x64: return C::WRITE_ANALOG_OUT;
            case 0x65: return C::WRITE_NETWORK_CONFIG;
            case 0x66: return C::WRITE_MAC;
            case 0x67: return C::WRITE_BAUD_UART1;
            case 0x68: return C::WRITE_BAUD_UART2;
            case 0x69: return C::WRITE_DIGITAL_TIMER;
            case 0x6A: return C::DESLIGA_MCB;
            case 0x6B: return C::WRITE_DATA_HORA;
            case 0x6C: return C::WRITE_MAC_PC;
            case 0x6D: return C::WRITE_INFO1;
            case 0x6E: return C::WRITE_INFO2;
            case 0x6F: return C::WRITE_INFO3;
            default:   return C::UNKNOWN;
        }
    }

    // soma cmd + payload, modulo 256
    static uint8_t calculateChecksum(utils::enum_::MCBCommand cmd,
                                     const std::vector<uint8_t>& payload)
    {
        uint32_t sum = static_cast<uint8_t>(cmd);
        for (auto b : payload) sum += b;
        return static_cast<uint8_t>(sum & 0xFF);
    }
};

// ——— MCBProtocol público ——— 

MCBProtocol::MCBProtocol()
  : pImpl_(std::make_unique<Impl>())
{}

MCBProtocol::~MCBProtocol() = default;

std::vector<uint8_t> MCBProtocol::buildFrame(utils::enum_::MCBCommand cmd,
                                             const std::vector<uint8_t>& payload) const
{
    return pImpl_->buildFrame(cmd, payload);
}

std::optional<MCBFrame> MCBProtocol::parseFrame(const std::vector<uint8_t>& buffer) const {
    return pImpl_->parseFrame(buffer);
}

} // namespace protocols::mcb_keyboard
