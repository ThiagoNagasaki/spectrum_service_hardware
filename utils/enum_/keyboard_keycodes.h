// commands/keyboard/keyboard_keycodes.h
#ifndef KEYBOARD_KEYCODES_H
#define KEYBOARD_KEYCODES_H

#include <cstdint>

namespace utils::enum_ {

/**
 * \brief Códigos de tecla do teclado matricial (KeyCode).
 *
 * Baseado na matriz de teclas do manual (códigos de 0x00 a 0x27). :contentReference[oaicite:0]{index=0}
 */
enum class KeyCode : uint8_t {
    None = 0x00,  ///< Nenhuma tecla
    S1   = 0x01,  S2  = 0x02,  S3  = 0x03,  S4  = 0x04,
    S5   = 0x05,  S6  = 0x06,  S7  = 0x07,  S8  = 0x08,
    S9   = 0x09,  S10 = 0x0A,  S11 = 0x0B,  S12 = 0x0C,
    S13  = 0x0D,  S14 = 0x0E,  S15 = 0x0F,  S16 = 0x10,
    S17  = 0x11,  S18 = 0x12,  S19 = 0x13,  S20 = 0x14,
    S21  = 0x15,  S22 = 0x16,  S23 = 0x17,  S24 = 0x18,
    S25  = 0x19,  S26 = 0x1A,  S27 = 0x1B,  S28 = 0x1C,
    Shift= 0x1D,  ///< Tecla SHIFT :contentReference[oaicite:1]{index=1}
    Ctrl = 0x1E,  ///< Tecla CTRL  :contentReference[oaicite:2]{index=2}
    S31  = 0x1F,  S32 = 0x20,  S33 = 0x21,  S34 = 0x22,
    S35  = 0x23,  S36 = 0x24,  S37 = 0x25,  S38 = 0x26,
    S39  = 0x27
};

} // namespace command::keyboard

#endif // KEYBOARD_KEYCODES_H
