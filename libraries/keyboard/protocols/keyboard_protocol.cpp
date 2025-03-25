#include "keyboard_protocol.h"
#include "../config/keyboard_constants.h"
#include "../../../utils/logger.h"
#include "../../../utils/enum_/enum_commandcontext.h"
#include "../../../utils/enum_/enum_errorcode.h"
#include <fmt/core.h>
#include <iostream>

namespace keyboard::protocols {

using transport::interface::ITransport;
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

KeyboardProtocol::KeyboardProtocol(std::shared_ptr<ITransport> transport)
    : transport_(std::move(transport))
{
    // Inscreve callback no transporte para receber bytes
    if (transport_) {
        transport_->subscribe([this](const std::vector<uint8_t>& data){
            this->onDataReceived(data);
        });
    }
}

KeyboardProtocol::~KeyboardProtocol() {
    disconnect();
}

bool KeyboardProtocol::connect() {
    if (!transport_) return false;
    return transport_->connect();
}

bool KeyboardProtocol::disconnect() {
    if (!transport_) return false;
    return transport_->disconnect();
}

bool KeyboardProtocol::setLedTecla(uint8_t keyCode, bool on) {
    // Monta payload: [keyCode, (on?1:0)]
    std::vector<uint8_t> payload = { keyCode, static_cast<uint8_t>(on ? 1 : 0) };
    return sendFrame(keyboard::config::CMD_LED_TECLA, payload);
}

bool KeyboardProtocol::setBuzzer(uint16_t ms) {
    // Monta payload: 2 bytes (LSB, MSB) do tempo em ms
    std::vector<uint8_t> payload = {
        static_cast<uint8_t>(ms & 0xFF),
        static_cast<uint8_t>((ms >> 8) & 0xFF)
    };
    return sendFrame(keyboard::config::CMD_BUZZER, payload);
}

void KeyboardProtocol::subscribeKeyPress(std::function<void(const KeyEvent&)> callback) {
    keyPressCallback_ = std::move(callback);
}

void KeyboardProtocol::onDataReceived(const std::vector<uint8_t>& data) {
    parseFrame(data);
}

void KeyboardProtocol::parseFrame(const std::vector<uint8_t>& frame) {
    if (frame.size() < 5) {
        Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
            "KeyboardProtocol: Frame muito curto.");
        return;
    }

    uint8_t cmd = frame[2];
    // Exemplo: se cmd == 0x70 (tecla pressionada), parse
    if (cmd == keyboard::config::CMD_KEY_PRESSED) {
        // Payload começa em frame[3]
        if (frame.size() >= 6) { 
            uint8_t key = frame[3];
            bool shift = false; // se tiver algo indicando SHIFT
            // Dispara callback
            if (keyPressCallback_) {
                KeyEvent ev{ key, shift };
                keyPressCallback_(ev);
            }
            Logger::instance().debug(CommandContext::HARDWARE,
                fmt::format("Tecla pressionada: 0x{:02X}", key));
        }
    }
    // tratar 0x71, 0x72, etc.
    // ...
}

bool KeyboardProtocol::sendFrame(uint8_t cmd, const std::vector<uint8_t>& payload) {
    if (!transport_) return false;

    // Monta [STX, length, cmd, payload..., ETX]
    uint8_t length = 1 + payload.size(); // 1 para o cmd
    std::vector<uint8_t> frame;
    frame.reserve(3 + payload.size());
    frame.push_back(0x02); // STX
    frame.push_back(length);
    frame.push_back(cmd);
    frame.insert(frame.end(), payload.begin(), payload.end());
    frame.push_back(0x03); // ETX

    return transport_->send(frame);
}

} // namespace keyboard::protocols
