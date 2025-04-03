#include "keyboard_decoder.h"
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include "../../../utils/logger.h"
#include "../../../libraries/keyboard/config/keyboard_constants.h" // Define STX, ETX, KEYBOARD_MIN_FRAME_SIZE, CMD_KEY_PRESSED, etc.

namespace keyboard::decoder {

//=============================================================================
// Implementação interna (PImpl) da classe KeyboardDecoder
//=============================================================================
class KeyboardDecoder::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    /**
     * @brief Converte um frame em uma string legível ignorando os bytes de controle.
     * @param frame Vetor de bytes que compõe o frame.
     * @return String com os dados (payload) em formato legível.
     */
    std::string decodificarResposta(const std::vector<uint8_t>& frame) const {
        std::string mensagem;
        if (frame.size() < 4)
            return "[ERRO] Resposta muito curta para decodificação!";
        // Ignora STX (índice 0), Tamanho (1), Checksum (penúltimo) e ETX (último)
        for (size_t i = 2; i < frame.size() - 2; i++) {
            if (frame[i] >= 32 && frame[i] <= 126)
                mensagem += static_cast<char>(frame[i]);
            else {
                char buffer[10];
                std::snprintf(buffer, sizeof(buffer), "\\x%02X", frame[i]);
                mensagem += buffer;
            }
        }
        return mensagem;
    }

    std::string decodeKeyPressedPayload(const std::vector<uint8_t>& payload) const {
        if (payload.empty())
            return "[ERRO] Payload vazio do teclado (pressionada)";
        uint8_t raw = payload[0];
        bool shift = (raw & 0x80) != 0;
        bool alt   = (raw & 0x40) != 0;
        uint8_t keyCode = raw & 0x3F;
        std::ostringstream oss;
        oss << "Tecla pressionada, código: " << static_cast<int>(keyCode);
        if (shift) oss << " [SHIFT]";
        if (alt)   oss << " [ALT]";
        return oss.str();
    }

    std::string decodeKeyReleasedPayload(const std::vector<uint8_t>& payload) const {
        if (payload.empty())
            return "[ERRO] Payload vazio do teclado (liberada)";
        uint8_t raw = payload[0];
        bool shift = (raw & 0x80) != 0;
        bool alt   = (raw & 0x40) != 0;
        uint8_t keyCode = raw & 0x3F;
        std::ostringstream oss;
        oss << "Tecla liberada, código: " << static_cast<int>(keyCode);
        if (shift) oss << " [SHIFT]";
        if (alt)   oss << " [ALT]";
        return oss.str();
    }

    std::string decodeKeyboardVersionPayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 2)
            return "[ERRO] Payload de versão do teclado insuficiente";
        std::ostringstream oss;
        oss << "Versão do Teclado: v" << static_cast<int>(payload[0])
            << ", Revisão: " << static_cast<int>(payload[1]);
        return oss.str();
    }

    std::string decodeKeyboardWriteAckPayload(const std::vector<uint8_t>& payload, const std::string& commandName) const {
        std::ostringstream oss;
        oss << commandName << " executado com sucesso";
        if (!payload.empty()) {
            oss << " | Resposta: ";
            for (auto byte : payload)
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        return oss.str();
    }

    std::string decodeFrame(const std::vector<uint8_t>& frame) const {
        if (frame.size() < keyboard::config::KEYBOARD_MIN_FRAME_SIZE)
            return "[Frame inválido: muito curto]";
        if (frame.front() != keyboard::config::STX)
            return "[Frame inválido: STX incorreto]";
        if (frame.back() != keyboard::config::ETX)
            return "[Frame inválido: ETX incorreto]";

        uint8_t length = frame[1];
        if (frame.size() != static_cast<size_t>(length + 4)) {
            std::ostringstream oss;
            oss << "[Frame inválido: esperado " << static_cast<int>(length + 4)
                << " bytes, mas recebido " << frame.size() << " bytes]";
            return oss.str();
        }

        uint8_t cmd = frame[2];
        std::vector<uint8_t> payload(frame.begin() + 3, frame.end() - 2);
        std::string descricao;
        switch (cmd) {
            case 0x70:
                descricao = decodeKeyPressedPayload(payload);
                break;
            case 0x72:
                descricao = decodeKeyReleasedPayload(payload);
                break;
            case 0x71:
                descricao = decodeKeyboardVersionPayload(payload);
                break;
            case 0x78:
                descricao = decodeKeyboardWriteAckPayload(payload, "LED_TECLA");
                break;
            case 0x79:
                descricao = decodeKeyboardWriteAckPayload(payload, "BUZZER");
                break;
            case 0x7A:
                descricao = decodeKeyboardWriteAckPayload(payload, "BEEP_TECLA");
                break;
            case 0x7B:
                descricao = decodeKeyboardWriteAckPayload(payload, "PISCA_TECLA");
                break;
            default:
                descricao = "Comando TECLADO desconhecido (0x" + intToHex(cmd) + ")";
                break;
        }
        return descricao + " | Payload decodificado: " + decodificarResposta(frame);
    }

private:
    std::string intToHex(uint8_t value) const {
        std::ostringstream oss;
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
        return oss.str();
    }
};

//==============================================================================
// Métodos públicos da classe KeyboardDecoder
//==============================================================================
KeyboardDecoder::KeyboardDecoder()
    : pImpl_(std::make_unique<Impl>())
{
}

KeyboardDecoder::~KeyboardDecoder() = default;

std::string KeyboardDecoder::decodeKeyPressedPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeKeyPressedPayload(payload);
}

std::string KeyboardDecoder::decodeKeyReleasedPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeKeyReleasedPayload(payload);
}

std::string KeyboardDecoder::decodeKeyboardVersionPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeKeyboardVersionPayload(payload);
}

std::string KeyboardDecoder::decodeKeyboardWriteAckPayload(const std::vector<uint8_t>& payload, const std::string& commandName) const {
    return pImpl_->decodeKeyboardWriteAckPayload(payload, commandName);
}

std::string KeyboardDecoder::decodeFrame(const std::vector<uint8_t>& frame) const {
    return pImpl_->decodeFrame(frame);
}

} // namespace keyboard
