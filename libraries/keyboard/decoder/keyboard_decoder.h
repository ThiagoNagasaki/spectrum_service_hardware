#ifndef KEYBOARD_DECODER_H
#define KEYBOARD_DECODER_H

#include <vector>
#include <string>
#include <cstdint>
#include <memory>

namespace keyboard::decoder {

/**
 * @brief Classe responsável por decodificar os frames dos comandos do teclado.
 *
 * Esta classe interpreta os payloads dos frames recebidos via RS485 do teclado,
 * de acordo com as especificações do manual técnico. Suas funções permitem extrair
 * informações de comandos de tecla pressionada, tecla liberada, versão, e ACKs dos comandos de escrita.
 */
class KeyboardDecoder {
public:
    KeyboardDecoder();
    ~KeyboardDecoder();

    /**
     * @brief Decodifica o payload do comando de tecla pressionada (CMD_KEY_PRESSED – 0x70).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a informação decodificada (código da tecla e indicadores de SHIFT/ALT).
     */
    std::string decodeKeyPressedPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando de tecla liberada (CMD_KEY_RELEASED – 0x72).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a informação decodificada.
     */
    std::string decodeKeyReleasedPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando de versão do teclado (CMD_VERSION – 0x71).
     * @param payload Vetor de bytes contendo 2 bytes: versão e revisão.
     * @return String com a versão e revisão do teclado.
     */
    std::string decodeKeyboardVersionPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o ACK genérico para comandos de escrita do teclado.
     *
     * Comandos como LED TECLA (0x78), BUZZER (0x79), BEEP TECLA (0x7A) e PISCA TECLA (0x7B)
     * utilizam este ACK para confirmar a execução.
     *
     * @param payload Vetor de bytes da resposta (pode ser vazio).
     * @param commandName Nome do comando.
     * @return String indicando o sucesso da operação e a resposta, se houver.
     */
    std::string decodeKeyboardWriteAckPayload(const std::vector<uint8_t>& payload, const std::string& commandName) const;

    /**
     * @brief Função genérica para decodificar um frame completo recebido do teclado.
     *
     * O frame é composto de: [STX, Tamanho, Comando, Payload..., ETX]. Esta função
     * valida o tamanho do frame, extrai o comando e delega a decodificação do payload
     * para a função adequada.
     *
     * @param frame Vetor de bytes que compõe o frame.
     * @return String com a decodificação completa do frame.
     */
    std::string decodeFrame(const std::vector<uint8_t>& frame) const;

private:
    /**
     * @brief Classe interna (PImpl) com toda a lógica de decodificação do teclado.
     */
    class Impl;

    /**
     * @brief Ponteiro único para a implementação oculta.
     */
    std::unique_ptr<Impl> pImpl_;
};

} // namespace decoder

#endif // KEYBOARD_DECODER_H
