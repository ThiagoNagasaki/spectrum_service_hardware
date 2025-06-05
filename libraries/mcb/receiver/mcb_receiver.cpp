#include "mcb_receiver.h"
#include "../config/mcb_constants.h"       // STX, ETX, MCB_MIN_FRAME_SIZE, etc.
#include "../decoder/mcb_decoder.h"          // MCBDecoder
#include "../../../utils/logger.h"           // Logger::instance()
#include "../../../utils/enum_/enum_commandcontext.h"
#include "../../../utils/enum_/enum_errorcode.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <fmt/core.h>

namespace mcb {
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;
namespace receiver {

class MCBReceiver::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    using Callback = MCBReceiver::Callback;

    void setCallback(Callback cb) {
        callback = cb;
    }

    void feed(const std::vector<uint8_t>& data) {
        // Acumula os dados recebidos
        buffer.insert(buffer.end(), data.begin(), data.end());
        processBuffer();
    }

private:
    std::vector<uint8_t> buffer;
    Callback callback;

    // Processa o buffer para extrair e decodificar frames completos
    void processBuffer() {
        mcb::decoder::MCBDecoder decoder;
        while (buffer.size() >= mcb::config::MCB_MIN_FRAME_SIZE) {
            // Garante que o frame começa com STX
            if (buffer.front() != mcb::config::STX) {
                auto it = std::find(buffer.begin() + 1, buffer.end(), mcb::config::STX);
                if (it == buffer.end()) {
                    buffer.clear();
                    break;
                } else {
                    buffer.erase(buffer.begin(), it);
                    if (buffer.size() < mcb::config::MCB_MIN_FRAME_SIZE)
                        break;
                }
            }
            uint8_t length = buffer[1];
            size_t totalSize = length + 4;  // STX + Length + (Cmd+Payload) + Checksum + ETX
            if (buffer.size() < totalSize)
                break;  // Ainda não há um frame completo

            if (buffer[totalSize - 1] != mcb::config::ETX) {
                Logger::instance().warning(
                    utils::enum_::CommandContext::HARDWARE,
                    utils::enum_::ErrorCode::ProtocolError,
                    fmt::format("MCBReceiver::Impl: ETX inválido detectado, descartando byte. Buffer: {}", 
                        fmt::format("{}", buffer.front()))
                );
                buffer.erase(buffer.begin());
                continue;
            }
            std::vector<uint8_t> frame(buffer.begin(), buffer.begin() + totalSize);
            buffer.erase(buffer.begin(), buffer.begin() + totalSize);

            std::string decoded = decoder.decodeFrame(frame);
            Logger::instance().debug(utils::enum_::CommandContext::HARDWARE, 
                                      
                                     fmt::format("MCBReceiver::Impl Frame decodificado: {}", decoded));
            if (callback)
                callback(decoded);
        }
    }
};

MCBReceiver::MCBReceiver() : pImpl_(std::make_unique<Impl>()) {}
MCBReceiver::~MCBReceiver() = default;
void MCBReceiver::setCallback(Callback cb) {
    pImpl_->setCallback(cb);
}
void MCBReceiver::feed(const std::vector<uint8_t>& data) {
    pImpl_->feed(data);
}

} // namespace receiver
} // namespace mcb
