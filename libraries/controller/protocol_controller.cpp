#include "protocol_controller.h"
#include <iostream>
#include <stdexcept>
#include <fmt/core.h>
namespace controller {
controller::ProtocolController::ProtocolController(
    std::shared_ptr<protocols::IProtocol> protocol,
    std::shared_ptr<transport::interface::ITransport> transport,
    std::shared_ptr<receiver::IFrameAwaiter<std::vector<uint8_t>>> awaiter)
    : protocol_(std::move(protocol)), transport_(std::move(transport)),
      awaiter_(std::move(awaiter)) {}

std::vector<uint8_t>
ProtocolController::sendCommand(uint8_t cmd,
                                const std::vector<uint8_t> &payload) {
  std::cout << "Chegou em sendCommand !\n";
  auto frame = protocol_->buildFrame(cmd, payload);
  std::cout << "[sendCommand] CMD: 0x" << std::hex << static_cast<int>(cmd)
            << "\n";

  std::cout << "[sendCommand] Payload (" << payload.size() << " bytes): ";
  for (auto b : payload) {
    std::cout << "0x" << std::hex << static_cast<int>(b) << " ";
  }
  std::cout << "\n";

  std::cout << "[sendCommand] Frame montado (" << frame.size() << " bytes): ";
  for (auto b : frame) {
    std::cout << "0x" << std::hex << static_cast<int>(b) << " ";
  }
  std::cout << "\n";
  if (!transport_->send(frame)) {
    throw std::runtime_error("ProtocolController::sendCommand: envio falhou");
  }
  std::cout << "Enviou o frame !\n";
  auto responseRaw = awaiter_->waitForFrame();
auto parsed = protocol_->parseFrame(responseRaw);
if (!parsed.has_value()) {
    throw std::runtime_error("ProtocolController::sendCommand: parsing falhou");
}

const auto& vec = parsed.value();
if (vec.empty()) {
    throw std::runtime_error("ProtocolController::sendCommand: payload vazio");
}

uint8_t receivedCmd = vec[0]; 
if (receivedCmd != cmd) {
    throw std::runtime_error(
        fmt::format("ProtocolController::sendCommand: comando recebido (0x{:02X}) não corresponde ao enviado (0x{:02X})", receivedCmd, cmd)
    );
}

  std::cout << "Conectando MCB responseRaw: ";
  for (auto b : vec) {
    std::cout << std::hex << static_cast<int>(b) << " ";
  }

  if (!parsed.has_value()) {
    throw std::runtime_error("ProtocolController::sendCommand: parsing falhou");
  }

  return parsed.value();
}
} // namespace controller