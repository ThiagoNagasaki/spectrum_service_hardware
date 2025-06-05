#include "keyboard_protocol.h"
#include "../config/keyboard_constants.h"
#include "../../../utils/logger.h"
#include "../../../utils/enum_/enum_commandcontext.h"
#include "../../../utils/enum_/enum_errorcode.h"
#include <fmt/core.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <algorithm>
// Inclua as classes do transporte TCP (ajuste os caminhos conforme sua estrutura)
#include "../../transport/network/tcp_transport.h"

namespace keyboard::protocols {

using transport::network::TCPTransport;
using transport::network::TCPConfig;
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

//=============================================================================
// Implementação interna (PImpl) da classe KeyboardProtocol
//=============================================================================
class KeyboardProtocol::Impl {
public:
Impl() = default;
~Impl() = default;

    bool send(const std::vector<uint8_t>& frame) {
        return transport ? transport->send(frame) : false;
    }

    std::vector<uint8_t> buildFrame(keyboard::config::KBCommand cmd, const std::vector<uint8_t>& payload) {
        uint8_t length = static_cast<uint8_t>(1 + payload.size());
        std::vector<uint8_t> frame;
        frame.reserve(3 + payload.size());
        frame.push_back(keyboard::config::STX);
        frame.push_back(length);
        frame.push_back(static_cast<uint8_t>(cmd));
        frame.insert(frame.end(), payload.begin(), payload.end());
        frame.push_back(keyboard::config::ETX);
        return frame;
    }

    void onDataReceived(const std::vector<uint8_t>& data) {
        parseFrame(data);
    }

    void parseFrame(const std::vector<uint8_t>& frame) {
        if (frame.size() < keyboard::config::KEYBOARD_MIN_FRAME_SIZE) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                "KeyboardProtocol: Frame muito curto.");
            return;
        }
        if (frame.front() != keyboard::config::STX || frame.back() != keyboard::config::ETX) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                "KeyboardProtocol: STX ou ETX inválido.");
            return;
        }
        uint8_t cmd = frame[2];
        // Exemplo: se o comando é CMD_KEY_PRESSED (0x70)
        if (cmd == static_cast<uint8_t>(keyboard::config::KBCommand::CMD_KEY_PRESSED)) {
            if (frame.size() >= 6) {
                uint8_t key = frame[3];
                bool shift = false; // Aqui você pode implementar a leitura do indicador SHIFT, se necessário
                if (keyPressCallback) {
                    KeyEvent ev{ key, shift };
                    keyPressCallback(ev);
                }
                Logger::instance().debug(CommandContext::HARDWARE,
                    fmt::format("Tecla pressionada: 0x{:02X}", key));
            }
        }
        // Você pode adicionar o tratamento de outros comandos, como CMD_KEY_RELEASED, CMD_VERSION, etc.
    }

    bool sendFrame(keyboard::config::KBCommand cmd, const std::vector<uint8_t>& payload) {
        std::vector<uint8_t> frame = buildFrame(cmd, payload);
        return send(frame);
    }

    // Membro de transporte (TCP) usado para comunicação
    std::shared_ptr<transport::interface::ITransport> transport;
    // Callback para notificar eventos de tecla pressionada
    std::function<void(const KeyEvent&)> keyPressCallback;
};

//=============================================================================
// Métodos públicos da classe KeyboardProtocol
//=============================================================================
KeyboardProtocol::KeyboardProtocol()
    : pImpl_(std::make_unique<Impl>())
{
}

KeyboardProtocol::~KeyboardProtocol() = default;

bool KeyboardProtocol::setLedTecla(uint8_t keyCode, bool on) {
    std::vector<uint8_t> payload = { keyCode, static_cast<uint8_t>(on ? 1 : 0) };
    return pImpl_->sendFrame(keyboard::config::KBCommand::CMD_LED_TECLA, payload);
}

bool KeyboardProtocol::setBuzzer(uint16_t ms) {
    std::vector<uint8_t> payload = {
        static_cast<uint8_t>(ms & 0xFF),
        static_cast<uint8_t>((ms >> 8) & 0xFF)
    };
    return pImpl_->sendFrame(keyboard::config::KBCommand::CMD_BUZZER, payload);
}

void KeyboardProtocol::subscribeKeyPress(std::function<void(const KeyEvent&)> callback) {
    pImpl_->keyPressCallback = std::move(callback);
}

std::vector<uint8_t> KeyboardProtocol::buildFrame(keyboard::config::KBCommand cmd,const std::vector<uint8_t>& payload) {
    return pImpl_->buildFrame(cmd, payload);
}

// Métodos privados – delegam para o PImpl
void KeyboardProtocol::onDataReceived(const std::vector<uint8_t>& data) {
    pImpl_->onDataReceived(data);
}

bool KeyboardProtocol::sendFrame(keyboard::config::KBCommand cmd, const std::vector<uint8_t>& payload) {
    return pImpl_->sendFrame(cmd, payload);
}

void KeyboardProtocol::parseFrame(const std::vector<uint8_t>& frame) {
    pImpl_->parseFrame(frame);
}

} // namespace keyboard::protocols
