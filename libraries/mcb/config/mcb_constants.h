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
 * \brief Tamanho máximo de payload (exemplo). Ajuste conforme o manual
 *        se houver limite, como 230 bytes de dados.
 */
static constexpr std::size_t MCB_MAX_DATA_SIZE = 230;

/**
 * \enum MCBCommand
 * \brief Enumeração dos comandos do protocolo MCB, conforme o manual.
 *
 */
enum class MCBCommand : uint8_t {
    // Leituras
    READ_FIRMWARE        = 0x51, ///< 0x51 - Leitura da versão do firmware
    READ_STATUS          = 0x52, ///< 0x52 - Leitura de status geral
    READ_INPUT_DIGITAL   = 0x53, ///< 0x53 - Leitura de entrada digital específica
    READ_ANALOG_INPUT    = 0x54, ///< 0x54 - Leitura de entrada analógica
    READ_SENSOR_DISTANCE = 0x55, ///< 0x55 - Leitura de sensor de distância (Cargo Compact)
    READ_STATUS_SPECIAL  = 0x56, ///< 0x56 - Leitura de estado especial
    READ_CHAVE_LIGA      = 0x5A, ///< 0x5A - Leitura da chave liga/desliga
    READ_DATA_HORA       = 0x5B, ///< 0x5B - Leitura da data/hora
    READ_MAC_PC          = 0x5C, ///< 0x5C - Leitura do MAC do PC
    READ_INFO1           = 0x5D, ///< 0x5D - Leitura de info 1
    READ_INFO2           = 0x5E, ///< 0x5E - Leitura de info 2
    READ_INFO3           = 0x5F, ///< 0x5F - Leitura de info 3

    // Escritas
    WRITE_SPECIAL        = 0x61, ///< 0x61 - Altera comandos especiais
    WRITE_CONFIG         = 0x62, ///< 0x62 - Altera configurações gerais
    WRITE_DIGITAL_OUT    = 0x63, ///< 0x63 - Altera saídas digitais
    WRITE_ANALOG_OUT     = 0x64, ///< 0x64 - Altera saídas analógicas
    WRITE_NETWORK_CONFIG = 0x65, ///< 0x65 - Altera configurações de rede
    WRITE_MAC            = 0x66, ///< 0x66 - Altera MAC Address do servidor
    WRITE_BAUD_UART1     = 0x67, ///< 0x67 - Altera baud rate da UART1
    WRITE_BAUD_UART2     = 0x68, ///< 0x68 - Altera baud rate da UART2
    WRITE_DIGITAL_TIMER  = 0x69, ///< 0x69 - Acionamento temporizado de saída digital
    DESLIGA_MCB          = 0x6A, ///< 0x6A - Comando para desligar a MCB
    WRITE_DATA_HORA      = 0x6B, ///< 0x6B - Escreve data/hora
    WRITE_MAC_PC         = 0x6C, ///< 0x6C - Armazena MAC do PC
    WRITE_INFO1          = 0x6D, ///< 0x6D - Escreve info 1
    WRITE_INFO2          = 0x6E, ///< 0x6E - Escreve info 2
    WRITE_INFO3          = 0x6F, ///< 0x6F - Escreve info 3

    UNKNOWN              = 0xFF  ///< Comando não reconhecido
};

} // namespace mcb

#endif // MCB_CONSTANTS_H
