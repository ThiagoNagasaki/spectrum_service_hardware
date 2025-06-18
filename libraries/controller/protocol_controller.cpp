#include "protocol_controller.h"
#include <stdexcept>
#include <iostream>
namespace controller {
controller::ProtocolController::ProtocolController(
    std::shared_ptr<protocols::IProtocol> protocol,
    std::shared_ptr<transport::interface::ITransport> transport,
    std::shared_ptr<receiver::IFrameAwaiter<std::vector<uint8_t>>> awaiter)
    : protocol_(std::move(protocol)), transport_(std::move(transport)), awaiter_(std::move(awaiter)) {}

std::vector<uint8_t> ProtocolController::sendCommand(uint8_t cmd, const std::vector<uint8_t>& payload) {
    std::cout << "Chegou em sendCommand !\n";
    auto frame = protocol_->buildFrame(cmd, payload);
 std::cout << "Montou o frame !\n";
    if (!transport_->send(frame)) {
        throw std::runtime_error("ProtocolController::sendCommand: envio falhou");
    }
std::cout << "Enviou o frame !\n";
    auto responseRaw = awaiter_->waitForFrame();
    auto parsed = protocol_->parseFrame(responseRaw);
auto& vec = parsed.value();
std::cout << "Conectando MCB responseRaw: ";
for (auto b : vec) {
    std::cout << std::hex << static_cast<int>(b) << " ";
}

    if (!parsed.has_value()) {
        throw std::runtime_error("ProtocolController::sendCommand: parsing falhou");
    }

    return parsed.value();
}
}