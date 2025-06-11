// utils/response_translator.h
#ifndef RESPONSE_TRANSLATOR_H
#define RESPONSE_TRANSLATOR_H

#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include "../../utils/enum_/keyboard_keycodes.h"

namespace utils {

/**
 * \brief Transforma respostas brutas em bytes ASCII em texto legível.
 */
class ResponseTranslator {
public:
    /// Converte vetor de bytes ASCII (até '\0') em std::string.
    static std::string asciiToString(const std::vector<uint8_t>& data);

    /// Sobrecarga para array fixo de 15 bytes (Info1/Info2/Info3).
    static std::string asciiToString(const std::array<uint8_t,15>& data);

    /**
     * \brief Traduz um KeyCode em nome de tecla.
     * Exemplo: KeyCode::S1 → "S1", KeyCode::Expansion3 → "Expansion3", etc.
     */
    static std::string keyCodeToString(utils::enum_::KeyCode code);
};

} // namespace utils

#endif // RESPONSE_TRANSLATOR_H
