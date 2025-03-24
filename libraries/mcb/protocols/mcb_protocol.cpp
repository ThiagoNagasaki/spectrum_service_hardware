#include "mcb_protocol.h"
#include "../config/mcb_constants.h"            // STX, ETX, enum MCBCommand
#include "../../../utils/logger.h"
#include "../../../utils/enum_/enum_commandcontext.h" // Para CommandContext::GENERAL
#include <numeric>    // std::accumulate
#include <iostream>   // Se quiser logs extras
#include <cstdint>
#include <cstdio>     // std::snprintf

namespace mcb::protocols {

// Tamanho mínimo de um frame: STX(1) + Tamanho(1) + Comando(1) + Checksum(1) + ETX(1) = 5
static constexpr size_t MIN_FRAME_SIZE = 5;

/**
 * \class MCBProtocol::Impl
 * \brief Implementação interna (PImpl).
 */
class MCBProtocol::Impl {
public:
    // Construtor: recebe referência para logger
    explicit Impl(utils::Logger& loggerRef)
        : logger_(loggerRef) {}

    // Monta frame
    std::vector<uint8_t> buildFrame(MCBCommand cmd, const std::vector<uint8_t>& payload) const {
        // Log de debug
        logger_.debug(utils::enum_::CommandContext::HARDWARE,
                      "Montando frame para comando 0x" + toHex(static_cast<uint8_t>(cmd)));

        std::vector<uint8_t> frame;
        frame.reserve(5 + payload.size());

        // 1) STX
        frame.push_back(STX);

        // 2) length = 1 (comando) + payload.size()
        uint8_t length = static_cast<uint8_t>(1 + payload.size());
        frame.push_back(length);

        // 3) comando (FALTAVA no seu snippet original)
        frame.push_back(static_cast<uint8_t>(cmd));

        // 4) data
        frame.insert(frame.end(), payload.begin(), payload.end());

        // 5) checksum (Comando + Data)
        uint8_t chksum = calculateChecksum(cmd, payload);
        frame.push_back(chksum);

        // 6) ETX
        frame.push_back(ETX);

        logger_.debug(utils::enum_::CommandContext::HARDWARE,
                      "Frame montado com " + std::to_string(frame.size()) + " bytes.");

        return frame;
    }

    // Parseia frame
    std::optional<MCBFrame> parseFrame(const std::vector<uint8_t>& buffer) const {
        if (buffer.size() < MIN_FRAME_SIZE) {
            logger_.warning(utils::enum_::CommandContext::HARDWARE,
                            "Buffer muito curto para ser um frame MCB.");
            return std::nullopt;
        }
        if (buffer.front() != STX) {
            logger_.warning(utils::enum_::CommandContext::HARDWARE,
                            "STX inválido: 0x" + toHex(buffer.front()));
            return std::nullopt;
        }
        if (buffer.back() != ETX) {
            logger_.warning(utils::enum_::CommandContext::HARDWARE,
                            "ETX inválido: 0x" + toHex(buffer.back()));
            return std::nullopt;
        }

        // Lê tamanho
        uint8_t length = buffer[1];
        // Tamanho total esperado = STX(1) + Tamanho(1) + length + Checksum(1) + ETX(1) = length + 4
        if (buffer.size() != static_cast<size_t>(length + 4)) {
            logger_.warning(utils::enum_::CommandContext::HARDWARE,
                            "Tamanho inconsistente. Esperado=" + std::to_string(length + 4) +
                            ", real=" + std::to_string(buffer.size()));
            return std::nullopt;
        }

        uint8_t cmdByte = buffer[2];
        MCBCommand cmd = toMCBCommand(cmdByte);

        // Data = bytes [3..(3 + dataSize - 1)]
        size_t dataSize = length - 1; // retira 1 para o comando
        std::vector<uint8_t> data;
        data.reserve(dataSize);
        if (dataSize > 0) {
            data.insert(data.end(), buffer.begin() + 3, buffer.begin() + 3 + dataSize);
        }

        // Checksum = byte [3 + dataSize]
        uint8_t chksum = buffer[3 + dataSize];

        // Recalcula
        uint8_t computed = calculateChecksum(cmd, data);
        if (computed != chksum) {
            logger_.warning(utils::enum_::CommandContext::HARDWARE,
                            "Checksum inválido. Calc=0x" + toHex(computed) +
                            ", recebido=0x" + toHex(chksum));
            return std::nullopt;
        }

        // Monta frame
        MCBFrame frame;
        frame.command = cmd;
        frame.data    = std::move(data);

        logger_.debug(utils::enum_::CommandContext::HARDWARE,
                      "Frame parseado com sucesso. Comando=0x" + toHex(cmdByte) +
                      ", dataSize=" + std::to_string(frame.data.size()));
        return frame;
    }

private:
    utils::Logger& logger_;

    // Converte byte para string hexa
    static std::string toHex(uint8_t b) {
        char buf[4];
        std::snprintf(buf, sizeof(buf), "%02X", b);
        return std::string(buf);
    }

    // Mapeia cmdByte em MCBCommand
    static MCBCommand toMCBCommand(uint8_t cmdByte) {
        switch (cmdByte) {
            case 0x51: return MCBCommand::READ_FIRMWARE;
            case 0x52: return MCBCommand::READ_STATUS;
            // ... e assim por diante, conforme seu manual ...
            default:   return MCBCommand::UNKNOWN;
        }
    }

    // Calcula checksum = soma(Comando + Data) mod 256
    static uint8_t calculateChecksum(MCBCommand cmd, const std::vector<uint8_t>& payload) {
        uint32_t sum = static_cast<uint8_t>(cmd);
        for (auto b : payload) {
            sum += b;
        }
        return static_cast<uint8_t>(sum & 0xFF);
    }
};

// ================== Métodos públicos de MCBProtocol ==================

MCBProtocol::MCBProtocol(utils::Logger& logger)
    : pImpl_(std::make_unique<Impl>(logger)) {}

MCBProtocol::~MCBProtocol() = default;

std::vector<uint8_t> MCBProtocol::buildFrame(MCBCommand cmd, const std::vector<uint8_t>& payload) const {
    return pImpl_->buildFrame(cmd, payload);
}

std::optional<MCBFrame> MCBProtocol::parseFrame(const std::vector<uint8_t>& buffer) const {
    return pImpl_->parseFrame(buffer);
}

} // namespace mcb::protocols
