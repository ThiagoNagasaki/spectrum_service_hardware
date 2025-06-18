#include "../libraries/controller/controller_factory.h"
#include "../libraries/controller/device_controller.h"
#include <iostream>

int main() {
    auto controller = controller::ControllerFactory::buildMCB()
                        .withTCP("127.0.0.1", 3000)
                        .build();



    std::cout << "Tentando Connectar !\n";
    if (!controller.connect()) {
        std::cerr << "Erro ao conectar. Abortando." << std::endl;
        return 1;
    }
  
    try {
        auto versao = controller.versaoFirmware();
        std::cout << "Firmware: " << versao << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erro durante execução do comando: " << e.what() << std::endl;
    }

    controller.disconnect();
    return 0;
}
