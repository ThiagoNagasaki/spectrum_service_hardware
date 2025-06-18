// commands/write_data_hora_command.h
#ifndef WRITE_DATA_HORA_COMMAND_H
#define WRITE_DATA_HORA_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include "read_data_hora_command.h"  // para DataHoraInfo
#include <memory>

namespace command::mcb {

/**
 * \brief Comando WRITE_DATA_HORA (0x6B): escreve data e hora na MCB.
 *
 * Bytes do payload (9 bytes) :contentReference[oaicite:0]{index=0}:
 *  - Byte 0: reservado (0)  
 *  - Byte 1: segundo (0–59)  
 *  - Byte 2: minuto  (0–59)  
 *  - Byte 3: hora    (bit6 e bit5 conforme manual)  
 *  - Byte 4: dia da semana (1–7)  
 *  - Byte 5: dia do mês    (1–31)  
 *  - Byte 6: mês           (1–12)  
 *  - Byte 7: ano (00–99)  
 *  - Byte 8: controle (0)  
 */
class WriteDataHoraCommand : public ICommand<void> {
public:
    explicit WriteDataHoraCommand(std::shared_ptr<protocols::IProtocol> protocols,
                                  const DataHoraInfo& dataHora);
    ~WriteDataHoraCommand() override;

    void execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // WRITE_DATA_HORA_COMMAND_H
