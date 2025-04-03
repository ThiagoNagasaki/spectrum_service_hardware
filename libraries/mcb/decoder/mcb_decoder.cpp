#include "mcb_decoder.h"
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include "../../../utils/logger.h"
#include "../../../libraries/mcb/config/mcb_constants.h"

namespace mcb::decoder {

//=============================================================================
// Implementação interna (PImpl) da classe MCBDecoder
//=============================================================================
class MCBDecoder::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    // Função auxiliar para converter uma string para vetor de bytes com tamanho fixo (15 bytes)
    std::vector<uint8_t> stringToFixedBytes(const std::string& s, size_t fixedSize = 15) const {
        std::vector<uint8_t> v(s.begin(), s.end());
        if (v.size() < fixedSize)
            v.resize(fixedSize, 0);
        else if (v.size() > fixedSize)
            v.resize(fixedSize);
        return v;
    }

    // Converte um frame em uma string legível ignorando os bytes de controle.
    std::string decodificar_resposta(const std::vector<uint8_t>& frame) const {
        std::string mensagem;
        if (frame.size() < 4)
            return "[ERRO] Resposta muito curta para decodificação!";
        // Ignora STX (índice 0), Tamanho (índice 1), Checksum (penúltimo) e ETX (último)
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

    std::string decodeFirmwarePayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 2)
            return "[Erro] Payload de firmware insuficiente";
        std::ostringstream oss;
        oss << "Firmware da MCB: Versão " << static_cast<int>(payload[0])
            << ", Revisão " << static_cast<int>(payload[1]);
        return oss.str();
    }

    std::string decodeStatusPayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 2)
            return "[ERRO] Payload de status insuficiente";
        uint8_t b0 = payload[0];
        uint8_t b1 = payload[1];
        std::ostringstream oss;
        oss << "Status da MCB:\n";
        oss << "  Interlock/Emergência 0: " << ((b0 & 0x01) ? "Aberto" : "OK") << "\n";
        oss << "  Interlock/Emergência 1: " << ((b0 & 0x02) ? "Aberto" : "OK") << "\n";
        oss << "  Raio-X: " << ((b0 & 0x04) ? "Ligado" : "Desligado") << "\n";
        oss << "  Sensor túnel 1: " << ((b0 & 0x08) ? "Ativado" : "Desativado") << "\n";
        oss << "  Sensor túnel 2: " << ((b0 & 0x10) ? "Ativado" : "Desativado") << "\n";
        oss << "  Sensor túnel 3: " << ((b0 & 0x20) ? "Ativado" : "Desativado") << "\n";
        oss << "  Sensor túnel 4: " << ((b0 & 0x40) ? "Ativado" : "Desativado") << "\n";
        oss << "  EXP2_IN: " << ((b0 & 0x80) ? "Ativado" : "Desligado") << "\n";
        oss << "  Expansões (bits 0-4): ";
        for (int i = 0; i < 5; i++) {
            oss << (((b1 >> i) & 0x01) ? "1" : "0");
        }
        oss << "\n";
        oss << "  Esteira: " << ((b1 & 0x20) ? "Ligada" : "Desligada") << "\n";
        oss << "  Expansões (bits 6-7): " << ((b1 >> 6) & 0x03);
        if (payload.size() >= 3)
            oss << "\n  Extra: 0x" << std::hex << static_cast<int>(payload[2]);
        return oss.str();
    }

    std::string decodeInputDigitalPayload(const std::vector<uint8_t>& payload) const {
        if (payload.empty())
            return "[Erro] Payload de entrada digital vazio";
        // Usa o primeiro byte
        uint8_t state = payload[0];
        std::ostringstream oss;
        oss << "Entrada digital: " << (state ? "Ligada" : "Desligada");
        return oss.str();
    }

    std::string decodeAnalogInputPayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 2)
            return "[Erro] Payload de entrada analógica insuficiente";
        uint16_t value = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
        std::ostringstream oss;
        oss << "Entrada analógica: " << value;
        return oss.str();
    }

    std::string decodeSensorDistancePayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 2)
            return "[Erro] Payload de sensor de distância insuficiente";
        uint16_t distance = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
        std::ostringstream oss;
        oss << "Sensor de distância: " << distance;
        return oss.str();
    }

    std::string decodeStatusSpecialPayload(const std::vector<uint8_t>& payload) const {
        if (payload.empty())
            return "[Erro] Payload de status especial vazio";
        uint8_t special = payload[0];
        std::ostringstream oss;
        oss << "Status Especial: 0x" << std::hex << static_cast<int>(special);
        return oss.str();
    }

    std::string decodeChaveLigaPayload(const std::vector<uint8_t>& payload) const {
        if (payload.empty())
            return "[Erro] Payload de chave liga vazio";
        std::ostringstream oss;
        oss << "Chave liga: " << (payload[0] ? "Ligada" : "Desligada");
        return oss.str();
    }

    std::string decodeDataHoraPayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 8)
            return "[Erro] Payload de data/hora insuficiente";
        std::ostringstream oss;
        oss << "Data/Hora: "
            << "Hora: " << static_cast<int>(payload[3]) << "h, "
            << "Minuto: " << static_cast<int>(payload[2]) << "m, "
            << "Segundo: " << static_cast<int>(payload[1]) << "s, "
            << "Dia: " << static_cast<int>(payload[5]) << ", "
            << "Mês: " << static_cast<int>(payload[6]) << ", "
            << "Ano: " << static_cast<int>(payload[7]);
        return oss.str();
    }

    std::string decodeMacPcPayload(const std::vector<uint8_t>& payload) const {
        if (payload.size() < 6)
            return "[Erro] Payload de MAC PC insuficiente";
        std::ostringstream oss;
        oss << "MAC PC: ";
        for (size_t i = 0; i < 6; i++) {
            if (i > 0)
                oss << ":";
            oss << std::hex << (payload[i] < 16 ? "0" : "") << static_cast<int>(payload[i]);
        }
        return oss.str();
    }

    std::string decodeInfoPayload(const std::vector<uint8_t>& payload, const std::string& infoName) const {
        if (payload.size() != 15)
            return "[Erro] Payload de " + infoName + " deve conter 15 bytes, mas contém " + std::to_string(payload.size());
        bool valid = false;
        std::ostringstream oss;
        for (auto byte : payload) {
            if (byte != 0)
                valid = true;
        }
        if (!valid)
            oss << infoName << ": [inválido]";
        else {
            oss << infoName << ": ";
            for (auto byte : payload)
                oss << std::hex << (byte < 16 ? "0" : "") << static_cast<int>(byte) << " ";
        }
        return oss.str();
    }

    std::string decodeWriteAckPayload(const std::vector<uint8_t>& payload, const std::string& commandName) const {
        std::ostringstream oss;
        oss << commandName << " executado com sucesso";
        if (!payload.empty()) {
            oss << " | Resposta: ";
            for (auto byte : payload)
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        return oss.str();
    }

    /**
     * @brief Decodifica um frame completo recebido da MCB.
     *
     * Verifica a presença do STX, ETX e o tamanho do frame e, com base no comando,
     * chama a função de decodificação apropriada.
     *
     * @param frame Vetor de bytes que compõe o frame.
     * @return String com a decodificação do frame.
     */
    std::string decodeFrame(const std::vector<uint8_t>& frame) const {
        if (frame.size() < mcb::config::MCB_MIN_FRAME_SIZE)
            return "[Frame inválido: muito curto]";
        if (frame.front() != mcb::config::STX)
            return "[Frame inválido: STX incorreto]";
        if (frame.back() != mcb::config::ETX)
            return "[Frame inválido: ETX incorreto]";

        uint8_t length = frame[1];
        if (frame.size() != static_cast<size_t>(length + 4)) {
            std::ostringstream oss;
            oss << "[Frame inválido: esperado " << static_cast<int>(length + 4)
                << " bytes, mas recebido " << frame.size() << " bytes]";
            return oss.str();
        }

        uint8_t cmd = frame[2];
        // Extrai o payload (dos índices 3 até penúltimo byte)
        std::vector<uint8_t> payload(frame.begin() + 3, frame.end() - 2);
        std::string descricao;
        switch (cmd) {
            case 0x51: descricao = decodeFirmwarePayload(payload); break;
            case 0x52: descricao = decodeStatusPayload(payload); break;
            case 0x53: descricao = decodeInputDigitalPayload(payload); break;
            case 0x54: descricao = decodeAnalogInputPayload(payload); break;
            case 0x55: descricao = decodeSensorDistancePayload(payload); break;
            case 0x56: descricao = decodeStatusSpecialPayload(payload); break;
            case 0x5A: descricao = decodeChaveLigaPayload(payload); break;
            case 0x5B: descricao = decodeDataHoraPayload(payload); break;
            case 0x5C: descricao = decodeMacPcPayload(payload); break;
            case 0x5D: descricao = decodeInfoPayload(payload, "Info1"); break;
            case 0x5E: descricao = decodeInfoPayload(payload, "Info2"); break;
            case 0x5F: descricao = decodeInfoPayload(payload, "Info3"); break;
            // Comandos de escrita – ACK genérico:
            case 0x61: descricao = decodeWriteAckPayload(payload, "WRITE_SPECIAL"); break;
            case 0x62: descricao = decodeWriteAckPayload(payload, "WRITE_CONFIG"); break;
            case 0x63: descricao = decodeWriteAckPayload(payload, "WRITE_DIGITAL_OUT"); break;
            case 0x64: descricao = decodeWriteAckPayload(payload, "WRITE_ANALOG_OUT"); break;
            case 0x65: descricao = decodeWriteAckPayload(payload, "WRITE_NETWORK_CONFIG"); break;
            case 0x66: descricao = decodeWriteAckPayload(payload, "WRITE_MAC"); break;
            case 0x67: descricao = decodeWriteAckPayload(payload, "WRITE_BAUD_UART1"); break;
            case 0x68: descricao = decodeWriteAckPayload(payload, "WRITE_BAUD_UART2"); break;
            case 0x69: descricao = decodeWriteAckPayload(payload, "WRITE_DIGITAL_TIMER"); break;
            case 0x6A: descricao = decodeWriteAckPayload(payload, "DESLIGA_MCB"); break;
            case 0x6B: descricao = decodeWriteAckPayload(payload, "WRITE_DATA_HORA"); break;
            case 0x6C: descricao = decodeWriteAckPayload(payload, "WRITE_MAC_PC"); break;
            case 0x6D: descricao = decodeWriteAckPayload(payload, "WRITE_INFO1"); break;
            case 0x6E: descricao = decodeWriteAckPayload(payload, "WRITE_INFO2"); break;
            case 0x6F: descricao = decodeWriteAckPayload(payload, "WRITE_INFO3"); break;
            default:
                descricao = "Comando MCB desconhecido (0x" + intToHex(cmd) + ")";
                break;
        }
        return descricao + " | Payload decodificado: " + decodificar_resposta(frame);
    }

private:
    std::string intToHex(uint8_t value) const {
        std::ostringstream oss;
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
        return oss.str();
    }
};

//==============================================================================
// Métodos públicos da classe MCBDecoder
//==============================================================================
MCBDecoder::MCBDecoder()
    : pImpl_(std::make_unique<Impl>())
{
}

MCBDecoder::~MCBDecoder() = default;

std::string MCBDecoder::decodeFirmwarePayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeFirmwarePayload(payload);
}

std::string MCBDecoder::decodeStatusPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeStatusPayload(payload);
}

std::string MCBDecoder::decodeInputDigitalPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeInputDigitalPayload(payload);
}

std::string MCBDecoder::decodeAnalogInputPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeAnalogInputPayload(payload);
}

std::string MCBDecoder::decodeSensorDistancePayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeSensorDistancePayload(payload);
}

std::string MCBDecoder::decodeStatusSpecialPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeStatusSpecialPayload(payload);
}

std::string MCBDecoder::decodeChaveLigaPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeChaveLigaPayload(payload);
}

std::string MCBDecoder::decodeDataHoraPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeDataHoraPayload(payload);
}

std::string MCBDecoder::decodeMacPcPayload(const std::vector<uint8_t>& payload) const {
    return pImpl_->decodeMacPcPayload(payload);
}

std::string MCBDecoder::decodeInfoPayload(const std::vector<uint8_t>& payload, const std::string& infoName) const {
    return pImpl_->decodeInfoPayload(payload, infoName);
}

std::string MCBDecoder::decodeWriteAckPayload(const std::vector<uint8_t>& payload, const std::string& commandName) const {
    return pImpl_->decodeWriteAckPayload(payload, commandName);
}

std::string MCBDecoder::decodeFrame(const std::vector<uint8_t>& frame) const {
    return pImpl_->decodeFrame(frame);
}

} // namespace decoder
