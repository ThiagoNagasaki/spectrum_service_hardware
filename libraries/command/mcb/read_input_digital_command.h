#ifndef READ_INPUT_DIGITAL_COMMAND_H
#define READ_INPUT_DIGITAL_COMMAND_H

 
#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Estado de uma porta digital lida pelo comando 0x53.
 * isOn = true  → nível alto (1)
 * isOn = false → nível baixo (0) :contentReference[oaicite:0]{index=0}
 */
struct DigitalInputInfo {
    bool isOn;
        std::string toString() const {
        return isOn ? "On" : "Off";
    }
};

/**
 * \brief Comando READ_INPUT_DIGITAL (0x53) da MCB.
 * Request: [endereço da porta: 0x00–0xFF]  
 * Response: [0x00=Off | 0x01=On] :contentReference[oaicite:1]{index=1}
 */
class ReadInputDigitalCommand 
  : public ICommand<DigitalInputInfo>
{
public:
    ReadInputDigitalCommand(std::shared_ptr<protocol::IProtocol> proto,
                            utils::enum_::DigitalInputPort inputAddress);
    ~ReadInputDigitalCommand() override;

    /// Executa o comando para a porta configurada e retorna seu estado.
    DigitalInputInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_INPUT_DIGITAL_COMMAND_H
