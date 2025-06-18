#include "../libraries/controller/controller_factory.h"
#include "../libraries/controller/device_controller.h"
#include <iostream>
#include <vector>
#include <functional>

int main() {
    try {
        auto controller = controller::ControllerFactory::buildMCB()
                            .withTCP("127.0.0.1", 3001)
                            .build();

        if (!controller.connect()) {
            std::cerr << "Falha ao conectar." << std::endl;
            return 1;
        }

        // Enfileira 5 execuções dinamicamente
        std::vector<std::function<void()>> comandos;
        for (int i = 0; i < 5; ++i) {
            comandos.emplace_back([&, i]() {
                auto v = controller.versaoFirmware();
                std::cout << "[" << i << "] Firmware: " << v << std::endl;
            });
        }
        for (auto& cmd : comandos) {
            try {
                cmd();
            } catch (const std::exception& e) {
                std::cerr << "Erro: " << e.what() << std::endl;
            }
        }

        controller.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
