// protocols/mcb_keyboard/mcb_keyboard_protocol.cpp
#include "mcb_keyboard_protocol.h"

#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"   
#include "../../utils/enum_/enum_commandcontext.h"
#include "../../utils/enum_/enum_errorcode.h"
 

#include <algorithm>
#include <numeric>
#include <optional>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <fmt/core.h>

namespace protocols::mcb_keyboard {

using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;
using utils::enum_::MCBCommand;
using utils::enum_::STX;
using utils::enum_::ETX;
using utils::enum_::MCB_MIN_FRAME_SIZE;
struct MCBProtocol::Impl {
    Impl() = default;
    ~Impl() = default;

    //======================================================================
    // Montagem de frame MCB
    //======================================================================
    std::vector<uint8_t> buildFrame(
        MCBCommand cmd,
        const std::vector<uint8_t>& payload
    ) const {
        Logger::instance().debug(
            CommandContext::HARDWARE,
            "MCBProtocol: montando frame cmd=0x" +
            toHex(static_cast<uint8_t>(cmd))
        );

        std::vector<uint8_t> frame;
        frame.reserve(5 + payload.size());

        // STX
        frame.push_back(STX);

        // length = 1 (cmd) + payload.size()
        uint8_t length = static_cast<uint8_t>(1 + payload.size());
        frame.push_back(length);

        // comando
        frame.push_back(static_cast<uint8_t>(cmd));

        // payload
        frame.insert(frame.end(), payload.begin(), payload.end());

        // checksum
        uint8_t chksum = calculateChecksum(cmd, payload);
        frame.push_back(chksum);

        // ETX
        frame.push_back(ETX);

        Logger::instance().debug(
            CommandContext::HARDWARE,
            "MCBProtocol: frame montado, tamanho=" +
            std::to_string(frame.size())
        );
        return frame;
    }

    //======================================================================
    // Parse de frame MCB
    //======================================================================
    std::optional<MCBFrame> parseFrame(
        const std::vector<uint8_t>& buffer
    ) const {
        if (buffer.size() < MCB_MIN_FRAME_SIZE) {
            Logger::instance().warning(
                CommandContext::HARDWARE, ErrorCode::ProtocolError,
                "MCBProtocol: buffer muito curto"
            );
            return std::nullopt;
        }
        if (buffer.front() != STX) {
            Logger::instance().warning(
                CommandContext::HARDWARE, ErrorCode::ProtocolError,
                "MCBProtocol: STX inválido 0x" + toHex(buffer.front())
            );
            return std::nullopt;
        }
        if (buffer.back() != ETX) {
            Logger::instance().warning(
                CommandContext::HARDWARE, ErrorCode::ProtocolError,
                "MCBProtocol: ETX inválido 0x" + toHex(buffer.back())
            );
            return std::nullopt;
        }

        uint8_t length = buffer[1];
        size_t expected = static_cast<size_t>(length) + 4;
        if (buffer.size() != expected) {
            Logger::instance().warning(
                CommandContext::HARDWARE, ErrorCode::ProtocolError,
                "MCBProtocol: tamanho inconsistente, esperado=" +
                std::to_string(expected) +
                " real=" + std::to_string(buffer.size())
            );
            return std::nullopt;
        }

        uint8_t cmdByte = buffer[2];
        auto cmd = toMCBCommand(cmdByte);

        // extrai payload
        size_t dataSize = length - 1;
        std::vector<uint8_t> data;
        if (dataSize) {
            data.insert(
                data.end(),
                buffer.begin() + 3,
                buffer.begin() + 3 + dataSize
            );
        }

        // verifica checksum
        uint8_t recvChk = buffer[3 + dataSize];
        uint8_t calcChk = calculateChecksum(cmd, data);
        if (recvChk != calcChk) {
            Logger::instance().warning(
                CommandContext::HARDWARE, ErrorCode::ProtocolError,
                "MCBProtocol: checksum inválido calc=0x" +
                toHex(calcChk) + " recv=0x" + toHex(recvChk)
            );
            return std::nullopt;
        }

        // monta MCBFrame
        MCBFrame frame;
        frame.command = cmd;
        frame.data    = std::move(data);

        Logger::instance().debug(
            CommandContext::HARDWARE,
            "MCBProtocol: frame parseado cmd=0x" +
            toHex(cmdByte) + " dataSize=" +
            std::to_string(frame.data.size())
        );
        return frame;
    }

private:
    static std::string toHex(uint8_t b) {
        char buf[4];
        std::snprintf(buf, sizeof(buf), "%02X", b);
        return buf;
    }

    static MCBCommand toMCBCommand(uint8_t b) {
        using C = MCBCommand;
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

    static uint8_t calculateChecksum(
        MCBCommand cmd,
        const std::vector<uint8_t>& payload
    ) {
        uint32_t sum = static_cast<uint8_t>(cmd);
        for (auto b : payload) sum += b;
        return static_cast<uint8_t>(sum & 0xFF);
    }
};

// ——— PImpl público ——— 

MCBProtocol::MCBProtocol()
  : pImpl_(std::make_unique<Impl>())
{}

MCBProtocol::~MCBProtocol() = default;

std::vector<uint8_t> MCBProtocol::buildFrame(
    MCBCommand cmd,
    const std::vector<uint8_t>& payload
) const {
    return pImpl_->buildFrame(cmd, payload);
}

std::optional<MCBFrame> MCBProtocol::parseFrame(
    const std::vector<uint8_t>& buffer
) const {
    return pImpl_->parseFrame(buffer);
}

} // namespace protocols::mcb_keyboard
