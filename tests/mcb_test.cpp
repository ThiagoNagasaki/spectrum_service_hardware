// tests/mcb_demo.cpp

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "libraries/transport/interface/i_transport.h"
#include "libraries/receiver/i_receiver.h"
#include "libraries/command/i_command.h"
#include "libraries/protocols/i_protocol.h"
#include "utils/enum_/mcb_port_addresses.h"
#include "libraries/transport/network/tcp_transport.h"
#include "libraries/receiver/mcb/mcb_receiver.h"

// #include "libraries/command/mcb/turn_on_conveyor_command.h"
// #include "libraries/command/mcb/turn_off_conveyor_command.h"

int main() {

    std::shared_ptr<transport::network::TCPTransport> transport = std::make_shared<transport::network::TCPTransport>(
        /* host */ "192.168.1.123",
        /* port */ 5000
    );

    auto protocol = std::make_shared<protocols::mcb_keyboard::MCBProtocol>();

    auto receiver = std::make_shared<receiver::mcb::McbReceiver>(protocol);

    receiver->setAnalogHandler(command::mcb::AnalogInputInfo, [](const std::vector<uint8_t>& payload) {
        std::cout << "[AppClient] Autonomous Message (0x5D) Received: ";
        for (auto b : payload) std::cout << std::hex << int(b) << ' ';
        std::cout << std::dec << "\n";
    });

    if (!transport->connect()) {
        std::cerr << "[AppClient] Failed to connect transport\n";
        return 1;
    }

    receiver->start(transport);

    std::vector<std::shared_ptr<command::ICommand>> commands;
    commands.push_back(std::make_shared<command::mcb::TurnOnConveyorCommand>(protocol));
    commands.push_back(std::make_shared<command::mcb::TurnOffConveyorCommand>(protocol));

    for (auto& cmd : commands) {
        cmd->execute(transport);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[AppClient] Commands sent. Waiting for autonomous messages for 5s...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    receiver->stop();
    transport->disconnect();

    std::cout << "--- Demo Finished ---\n";
    return 0;
}
