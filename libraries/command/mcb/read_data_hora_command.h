// commands/read_data_hora_command.h
#ifndef READ_DATA_HORA_COMMAND_H
#define READ_DATA_HORA_COMMAND_H

#include "../../libraries/command/i_command.h"
#include "../../libraries/protocols/i_protocol.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/logger/logger.h"
#include <memory>
#include <cstdint>

namespace command::mcb {

/**
 * \brief Representa a data e hora lidas pelo comando 0x5B (READ_DATA_HORA).
 *
 * Bytes de resposta:
 *  - Byte 0: reservado (interno)  
 *  - Byte 1: segundo (0–59)  
 *  - Byte 2: minuto  (0–59)  
 *  - Byte 3: hora  
 *      - bit6=0 → 0–23  
 *      - bit6=1 → 1–12 AM/PM (bit5 indica PM)  
 *      - bit0–3 = hora  
 *  - Byte 4: dia da semana (1–7)  
 *  - Byte 5: dia do mês    (1–31)  
 *  - Byte 6: mês           (1–12)  
 *  - Byte 7: ano (00–99)  
 *  - Byte 8: controle (0)  :contentReference[oaicite:1]{index=1}
 */
struct DataHoraInfo {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t weekday;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t control;
};

/**
 * \brief Comando READ_DATA_HORA (0x5B) da MCB.
 */
class ReadDataHoraCommand
  : public ICommand<DataHoraInfo>
{
public:
    explicit ReadDataHoraCommand(std::shared_ptr<protocol::IProtocol> protocol);
    ~ReadDataHoraCommand() override;

    /// Executa o comando e retorna um DataHoraInfo parseado.
    DataHoraInfo execute() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace command::mcb

#endif // READ_DATA_HORA_COMMAND_H
