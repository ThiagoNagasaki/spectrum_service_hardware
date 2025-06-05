#ifndef KEYBOARD_CONSTANTS_H
#define KEYBOARD_CONSTANTS_H

#include <cstdint>

namespace keyboard::config {

/**
 * \brief Delimitadores de frame.
 *
 * STX: Byte de início do frame (Start of Text)
 * ETX: Byte de fim do frame (End of Text)
 */
static constexpr uint8_t STX = 0x02;
static constexpr uint8_t ETX = 0x03;

/**
 * \brief Tamanho mínimo do frame do teclado.
 *
 * O frame deve conter pelo menos: STX, Tamanho, Comando, Checksum e ETX.
 */
static constexpr size_t KEYBOARD_MIN_FRAME_SIZE = 5;

/**
 * \brief Comandos conforme o manual do teclado PED12-TECLADO.
 * 
 * 0x70 -> TECLA PRESSIONADA
 * 0x71 -> VERSÃO DO TECLADO
 * 0x72 -> TECLA LIBERADA (se houver)
 * 0x78 -> LED TECLA (ligar/desligar)
 * 0x79 -> BUZZER
 * 0x7A -> BEEP TECLA
 * 0x7B -> PISCA TECLA
 */
enum class KBCommand : uint8_t {
 
    CMD_KEY_PRESSED         = 0x70,
    CMD_VERSION             = 0x71,
    CMD_KEY_RELEASED        = 0x72,
    CMD_LED_TECLA           = 0x78,
    CMD_BUZZER              = 0x79,
    CMD_BEEP_TECLA          = 0x7A,
    CMD_PISCA_TECLA         = 0x7B, 
    UNKNOWN                 = 0xFF
};

}
#endif // KEYBOARD_CONSTANTS_H
