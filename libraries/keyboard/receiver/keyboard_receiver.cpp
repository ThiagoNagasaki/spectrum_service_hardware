#include "keyboard_receiver.h"
#include "../config/keyboard_constants.h"       // STX, ETX, KEYBOARD_MIN_FRAME_SIZE, etc.
#include "../decoder/keyboard_decoder.h"        // KeyboardDecoder
#include "../../../utils/logger.h"              // Logger::instance()
#include "../../../utils/enum_/enum_commandcontext.h"
#include "../../../utils/enum_/enum_errorcode.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <fmt/core.h>

namespace keyboard {
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

namespace receiver {

class KeyboardReceiver::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    using Callback = KeyboardReceiver::Callback;

    void setCallback(Callback cb) {
        callback = cb;
    }

    void feed(const std::vector<uint8_t>& data) {
        buffer.insert(buffer.end(), data.begin(), data.end());
        processBuffer();
    }

private:
    std::vector<uint8_t> buffer;
    Callback callback;

    void processBuffer() {
        keyboard::decoder::KeyboardDecoder decoder;
        while (buffer.size() >= keyboard::config::KEYBOARD_MIN_FRAME_SIZE) {
            if (buffer.front() != keyboard::config::STX) {
                auto it = std::find(buffer.begin() + 1, buffer.end(), keyboard::config::STX);
                if (it == buffer.end()) {
                    buffer.clear();
                    break;
                } else {
                    buffer.erase(buffer.begin(), it);
                    if (buffer.size() < keyboard::config::KEYBOARD_MIN_FRAME_SIZE)
                        break;
                }
            }
            uint8_t length = buffer[1];
            size_t totalSize = static_cast<size_t>(length) + 4;  // STX + Length + (Cmd+Payload) + Checksum + ETX
            if (buffer.size() < totalSize)
                break;
            
            if (buffer[totalSize - 1] != keyboard::config::ETX) {
                // Chama warning com CommandContext e ErrorCode corretos
                Logger::instance().warning(
                    CommandContext::HARDWARE,
                    ErrorCode::TCPDataSendFailure,
                    fmt::format("ETX inválido detectado, descartando byte. Primeiro byte: {}", buffer.front())
                );
                buffer.erase(buffer.begin());
                continue;
            }

            // Frame válido, decodifica e notifica
            std::vector<uint8_t> frame(buffer.begin(), buffer.begin() + totalSize);
            buffer.erase(buffer.begin(), buffer.begin() + totalSize);
            std::string decoded = decoder.decodeFrame(frame);

            Logger::instance().debug(
                CommandContext::HARDWARE,
                fmt::format("Frame decodificado: {}", decoded)
            );

            if (callback) {
                callback(decoded);
            }
        }
    }
};

KeyboardReceiver::KeyboardReceiver()
    : pImpl_(std::make_unique<Impl>()) {}

KeyboardReceiver::~KeyboardReceiver() = default;

void KeyboardReceiver::setCallback(Callback cb) {
    pImpl_->setCallback(cb);
}

void KeyboardReceiver::feed(const std::vector<uint8_t>& data) {
    pImpl_->feed(data);
}

} // namespace receiver
} // namespace keyboard
