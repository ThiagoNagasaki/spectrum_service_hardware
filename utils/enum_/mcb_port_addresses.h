// mcb_port_addresses.h
#ifndef MCB_PORT_ADDRESSES_H
#define MCB_PORT_ADDRESSES_H

#include <cstdint>

namespace utils::enum_ {



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

    CMD_KEY_PRESSED         = 0x70,
    CMD_VERSION             = 0x71,
    CMD_KEY_RELEASED        = 0x72,
    CMD_LED_TECLA           = 0x78,
    CMD_BUZZER              = 0x79,
    CMD_BEEP_TECLA          = 0x7A,
    CMD_PISCA_TECLA         = 0x7B, 
    UNKNOWN                 = 0xFF
};

// -----------------------------------------------------------------------------
// Portas de Entrada Digital (READ_INPUT_DIGITAL – 0x53)
// -----------------------------------------------------------------------------
enum class DigitalInputPort : uint8_t {
    Status0  = 0x00,
    Status1  = 0x01,
    Status2  = 0x02,
    Status3  = 0x03,
    Status4  = 0x04,
    Status5  = 0x05,
    Status6  = 0x06,
    Status7  = 0x07,
    Status8  = 0x08,
    Status9  = 0x09,
    Status10 = 0x0A,
    Status11 = 0x0B,
    Status12 = 0x0C,
    Status13 = 0x0D,
    Status14 = 0x0E,
    Status15 = 0x0F,

    // Exemplo de uso em hardware customizado
    Port3  = Status3, 
    Port5  = Status5,  
    // … adicione outras portas aqui
};

// -----------------------------------------------------------------------------
// Portas de Entrada Analógica (READ_ANALOG_INPUT – 0x54)
// -----------------------------------------------------------------------------
enum class AnalogInputPort : uint8_t {
    Port0 = 0x00,
    Port1 = 0x01,
    Port2 = 0x02,
    Port3 = 0x03,
    Port4 = 0x04,
    Port5 = 0x05,
    // … adicione outras portas aqui
};

// -----------------------------------------------------------------------------
// Modo de Leitura de Sensor de Distância (READ_SENSOR_DISTANCE – 0x55)
// -----------------------------------------------------------------------------
enum class SensorDistanceMode : uint8_t {
    Distance = 0x00,
    Speed    = 0x01,
    // … se o manual definir mais modos
};

// -----------------------------------------------------------------------------
// Comandos de Saída Digital (WRITE_DIGITAL_OUT – 0x63)
// -----------------------------------------------------------------------------
enum class DigitalOutputPort : uint8_t {
    SuspectLight    = 0x00,
    Gate1           = 0x01,
    Gate2           = 0x02,
    Inverter0       = 0x10,
    Inverter1       = 0x11,
    Inverter2       = 0x12,
    ShutterFull     = 0x15,
    ShutterThorax   = 0x16,
    Exp1Out         = 0x20,
    Expansion0      = 0x21,
    Expansion1      = 0x22,
    Expansion2      = 0x23,
    Expansion3      = 0x24,
    Expansion4      = 0x25,
    Expansion5      = 0x26,
    Expansion6      = 0x27,
    // … adicione as demais expanções
    RayOnIndicator  = 0x28,
};

// -----------------------------------------------------------------------------
// Endereços de Configuração (WRITE_CONFIG – 0x62)
// -----------------------------------------------------------------------------
enum class ConfigAddress : uint8_t {
    EmergencyOff         = 0x00,
    DebounceTunnel12     = 0x01,
    DebounceTunnel34     = 0x02,
    DistanceAngle        = 0x03,
    ShutdownTimeout      = 0x04,
    // … demais endereços de config
};

} // namespace command::config

#endif // MCB_PORT_ADDRESSES_H
