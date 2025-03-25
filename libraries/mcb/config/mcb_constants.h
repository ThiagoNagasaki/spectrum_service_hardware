#ifndef MCB_CONSTANTS_H
#define MCB_CONSTANTS_H

#include <cstdint>
#include <cstddef>

namespace mcb::config {

/**
 * \brief Byte de início de pacote (STX) segundo o manual.
 */
static constexpr uint8_t STX = 0x02;

/**
 * \brief Byte de fim de pacote (ETX) segundo o manual.
 */
static constexpr uint8_t ETX = 0x03;

/**
 * \brief Tamanho mínimo de um frame MCB, conforme o manual:
 *        STX(1) + Tamanho(1) + Comando(1) + Checksum(1) + ETX(1) = 5.
 */
static constexpr std::size_t MCB_MIN_FRAME_SIZE = 5;

/**
 * \brief Tamanho máximo de payload (exemplo). Ajuste se o manual definir outro valor.
 */
static constexpr std::size_t MCB_MAX_DATA_SIZE = 230;

/**
 * \enum MCBCommand
 * \brief Enumeração dos comandos do protocolo MCB, conforme o manual.
 *
 */
enum class MCBCommand : uint8_t {
    // Leituras
    READ_FIRMWARE        = 0x51,
    READ_STATUS          = 0x52,
    READ_INPUT_DIGITAL   = 0x53,
    READ_ANALOG_INPUT    = 0x54,
    READ_SENSOR_DISTANCE = 0x55,
    READ_STATUS_SPECIAL  = 0x56,
    READ_CHAVE_LIGA      = 0x5A,
    READ_DATA_HORA       = 0x5B,
    READ_MAC_PC          = 0x5C,
    READ_INFO1           = 0x5D,
    READ_INFO2           = 0x5E,
    READ_INFO3           = 0x5F,

    // Escritas
    WRITE_SPECIAL        = 0x61,
    WRITE_CONFIG         = 0x62,
    WRITE_DIGITAL_OUT    = 0x63,
    WRITE_ANALOG_OUT     = 0x64,
    WRITE_NETWORK_CONFIG = 0x65,
    WRITE_MAC            = 0x66,
    WRITE_BAUD_UART1     = 0x67,
    WRITE_BAUD_UART2     = 0x68,
    WRITE_DIGITAL_TIMER  = 0x69,
    DESLIGA_MCB          = 0x6A,
    WRITE_DATA_HORA      = 0x6B,
    WRITE_MAC_PC         = 0x6C,
    WRITE_INFO1          = 0x6D,
    WRITE_INFO2          = 0x6E,
    WRITE_INFO3          = 0x6F,

    UNKNOWN              = 0xFF
};

} // namespace mcb::config

#endif // MCB_CONSTANTS_H
