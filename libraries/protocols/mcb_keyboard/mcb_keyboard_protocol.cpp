#include "mcb_keyboard_protocol.h"

#include "../../utils/enum_/enum_commandcontext.h"
#include "../../utils/enum_/enum_errorcode.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fmt/core.h>
#include <numeric>
#include <optional>
#include <vector>
#include <iostream>

namespace protocols::mcb_keyboard {

using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;
using utils::enum_::ETX;
using utils::enum_::MCB_MIN_FRAME_SIZE;
using utils::enum_::MCBCommand;
using utils::enum_::STX;

class MCBProtocol::Impl {
public:
    Impl(std::shared_ptr<transport::interface::ITransport> t,
         std::shared_ptr<receiver::IReceiver<MCBFrame>> r)
        : transp(std::move(t)), recv(std::move(r)) {}

    std::shared_ptr<transport::interface::ITransport> transp;
    std::shared_ptr<receiver::IReceiver<MCBFrame>> recv;

    std::vector<uint8_t> buildFrame(MCBCommand cmd, const std::vector<uint8_t>& payload) const {
        Logger::instance().debug(CommandContext::HARDWARE,
            "MCBProtocol: montando frame cmd=0x" + toHex(static_cast<uint8_t>(cmd)));

        std::vector<uint8_t> frame;
        frame.reserve(5 + payload.size());

        frame.push_back(STX);
        uint8_t length = static_cast<uint8_t>(1 + payload.size());
        frame.push_back(length);
        frame.push_back(static_cast<uint8_t>(cmd));
        frame.insert(frame.end(), payload.begin(), payload.end());

        uint8_t chksum = calculateChecksum(cmd, payload);
        frame.push_back(chksum);
        frame.push_back(ETX);

        return frame;
    }

    std::optional<MCBFrame> parseFrame(const std::vector<uint8_t>& buffer) const {

         std::cout << "chegou EM parseFrame \n";
        if (buffer.size() < MCB_MIN_FRAME_SIZE || buffer.front() != STX || buffer.back() != ETX) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::ProtocolError,
                "MCBProtocol: frame inválido");
            return std::nullopt;
        }
        std::cout << "PROTOCOLO MCB_MIN_FRAME_SIZE STX ETX OK \n";
        uint8_t length = buffer[1];
        size_t expected = static_cast<size_t>(length) + 4;
        if (buffer.size() != expected) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::ProtocolError,
                fmt::format("MCBProtocol: tamanho inconsistente, esperado={}, real={}", expected, buffer.size()));
            return std::nullopt;
        }
        std::cout << "Tamanho ok \n";
        uint8_t cmdByte = buffer[2];
        auto cmd = toMCBCommand(cmdByte);

        size_t dataSize = length - 1;
        std::vector<uint8_t> data;
        if (dataSize) {
            data.insert(data.end(), buffer.begin() + 3, buffer.begin() + 3 + dataSize);
        }

        uint8_t recvChk = buffer[3 + dataSize];
        uint8_t calcChk = calculateChecksum(cmd, data);
        if (recvChk != calcChk) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::ProtocolError,
                fmt::format("Checksum inválido: calc=0x{:02X}, recv=0x{:02X}", calcChk, recvChk));
            return std::nullopt;
        }
    std::cout << "CalculateChecksum ok \n";
        return MCBFrame{cmd, std::move(data)};
    }

    void subscribe(std::function<void(const std::vector<uint8_t>&)> cb) {
        transp->subscribe(std::move(cb));
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
        case 0x54: return C::READ_ANALOG_INPUT;
        case 0x5A: return C::READ_CHAVE_LIGA;
        case 0x5B: return C::READ_DATA_HORA;
        case 0x63: return C::WRITE_DIGITAL_OUT;
        default:   return C::UNKNOWN;
        }
    }

    static uint8_t calculateChecksum(MCBCommand cmd, const std::vector<uint8_t>& payload) {
        uint32_t sum = static_cast<uint8_t>(cmd);
        for (auto b : payload) sum += b;
        return static_cast<uint8_t>(sum & 0xFF);
    }
};

// ===================== Interface pública =====================

MCBProtocol::MCBProtocol(
    std::shared_ptr<transport::interface::ITransport> transport,
    std::shared_ptr<receiver::IReceiver<MCBFrame>> receiver
) : pImpl_(std::make_unique<Impl>(std::move(transport), std::move(receiver))) {}

MCBProtocol::~MCBProtocol() = default;

std::vector<uint8_t> MCBProtocol::buildFrame(uint8_t cmd, const std::vector<uint8_t>& payload) {
    return pImpl_->buildFrame(static_cast<MCBCommand>(cmd), payload);
}

std::optional<std::vector<uint8_t>> MCBProtocol::parseFrame(const std::vector<uint8_t>& buffer) {
    auto parsed = pImpl_->parseFrame(buffer);
    if (!parsed) return std::nullopt;
    return parsed->data;
}

void MCBProtocol::subscribe(std::function<void(const std::vector<uint8_t>&)> cb) {
    pImpl_->subscribe(std::move(cb));
}

} // namespace protocols::mcb_keyboard
