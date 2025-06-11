// commands/read_analog_input_command.h
#ifndef READ_ANALOG_INPUT_COMMAND_H
#define READ_ANALOG_INPUT_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>
#include <string>  // adiciona suporte a std::string

namespace command::mcb {

/**
 * \brief Valor de uma porta analógica lida pelo comando 0x54.
 * 16-bit MSB|LSB
 */
struct AnalogInputInfo {
    uint16_t value;

    /// Retorna representação “humana” do valor analógico, ex: "1023"
    std::string toString() const {
        return std::to_string(value);
    }
};

/**
 * \brief Comando READ_ANALOG_INPUT (0x54) da MCB.
 * Request: [endereço da porta: 0x00–0xFF]  
 * Response: [MSB, LSB] → uint16_t
 */
class ReadAnalogInputCommand 
  : public ICommand<AnalogInputInfo>
{
public:
    ReadAnalogInputCommand(std::shared_ptr<protocol::IProtocol> proto,
                           utils::enum_::AnalogInputPort analogAddress);
    ~ReadAnalogInputCommand() override;

    /// Executa o comando para a porta configurada e retorna seu valor.
    AnalogInputInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_ANALOG_INPUT_COMMAND_H
