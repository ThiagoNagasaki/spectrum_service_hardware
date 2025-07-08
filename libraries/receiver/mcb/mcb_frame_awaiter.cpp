// receiver/mcb_frame_awaiter.cpp
#include "mcb_frame_awaiter.h"
#include <stdexcept>
#include <iostream>

namespace receiver::mcb {

MCBFrameAwaiter::MCBFrameAwaiter(
    std::shared_ptr<transport::interface::ITransport> transport,
    std::shared_ptr<receiver::IReceiver<protocols::mcb_keyboard::MCBFrame>> receiver)
    : transport_(std::move(transport)),
      protocol_(transport_, std::move(receiver)) 
{

}

protocols::mcb_keyboard::MCBFrame MCBFrameAwaiter::waitForFrame() {
    std::cout << "[MCBFrameAwaiter] Aguardando frame...\n";

    auto raw = transport_->receive(); 
    std::cout << "[MCBFrameAwaiter] Frame recebido: ";
    for (auto b : raw) std::cout << std::hex << static_cast<int>(b) << " ";
    std::cout << "\n";

    auto parsed = protocol_.parseFrame(raw);
    if (!parsed || parsed->empty()) {
        throw std::runtime_error("MCBFrameAwaiter: falha ao parsear frame");
    }
    std::cout << "PROTOCOLO MCB_MIN_FRAME_SIZE STX ETX OK \n";
    const auto& data = *parsed;
    if (data.size() < 1) {
        throw std::runtime_error("MCBFrameAwaiter: frame inválido, sem comando");
    }

    protocols::mcb_keyboard::MCBFrame frame;
    frame.command = static_cast<utils::enum_::MCBCommand>(data[0]);
    frame.data.assign(data.begin() + 1, data.end());

    std::cout << "[MCBFrameAwaiter] Frame OK. Comando: " << static_cast<int>(frame.command)
              << ", tamanho do payload: " << frame.data.size() << "\n";

    return frame;
}
} // namespace receiver
