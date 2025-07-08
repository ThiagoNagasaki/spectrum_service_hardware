#include "controller_factory.h"
#include "../receiver/dummy_mcb_frame_receiver.h"
#include "../receiver/mcb/mcb_frame_awaiter.h"
#include "../receiver/mcb/mcb_frame_vector_awaiter.h"
#include <stdexcept>

namespace controller {
using transport::network::TCPTransport;
using transport::network::TCPConfig;
using protocols::mcb_keyboard::MCBProtocol;

ControllerFactory ControllerFactory::buildMCB() {
    return ControllerFactory();
}

ControllerFactory& ControllerFactory::withTCP(const std::string& ip, uint16_t port) {
    tcp_config_ = TCPConfig{ip, port};
    tcp_config_set_ = true;
    return *this;
}

controller::DeviceController ControllerFactory::build() {
    if (!tcp_config_set_) {
        throw std::runtime_error("ControllerFactory::build: TCP config não definido");
    }

    auto transport       =  std::make_shared<TCPTransport>(tcp_config_);
    auto receiver        =  std::make_shared<receiver::DummyMCBFrameReceiver>();
    auto protocol        =  std::make_shared<MCBProtocol>(transport, receiver);
    auto awaiter         =  std::make_shared<receiver::mcb::MCBFrameVectorAwaiter>(transport);
    auto protoController =  std::make_shared<controller::ProtocolController>(protocol, transport, awaiter);
    return controller::DeviceController(transport, protoController);
}
}