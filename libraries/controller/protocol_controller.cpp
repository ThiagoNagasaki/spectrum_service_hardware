#include "protocol_controller.h"
#include <iostream>
#include <stdexcept>
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
  auto frame = protocol_->buildFrame(cmd, payload);
  std::cout << "Montou o frame !\n";
  if (!transport_->send(frame)) {
    throw std::runtime_error("ProtocolController::sendCommand: envio falhou");
  }
  std::cout << "Enviou o frame !\n";
  auto responseRaw = awaiter_->waitForFrame();
 
  
 
  return responseRaw;
}
} // namespace controller