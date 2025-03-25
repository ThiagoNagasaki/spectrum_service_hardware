#ifndef KEYBOARD_CONSTANTS_H
#define KEYBOARD_CONSTANTS_H

#include <cstdint>

namespace keyboard::config {

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
 * ... etc ...
 */
static constexpr uint8_t CMD_KEY_PRESSED  = 0x70; 
static constexpr uint8_t CMD_VERSION      = 0x71; 
static constexpr uint8_t CMD_KEY_RELEASED = 0x72; 
static constexpr uint8_t CMD_LED_TECLA    = 0x78; 
static constexpr uint8_t CMD_BUZZER       = 0x79; 
static constexpr uint8_t CMD_BEEP_TECLA   = 0x7A; 
static constexpr uint8_t CMD_PISCA_TECLA  = 0x7B; 

} // namespace keyboard::config

#endif // KEYBOARD_CONSTANTS_H
