#ifndef KEYBOARD_PROTOCOL_H
#define KEYBOARD_PROTOCOL_H

#include "../../transport/interface/i_transport.h" // Para ITransport
#include "../config/keyboard_constants.h"          // Define KBCommand, STX, ETX, KEYBOARD_MIN_FRAME_SIZE, etc.
#include <functional>
#include <vector>
#include <memory>
#include <cstdint>

namespace keyboard::protocols {

/**
 * \struct KeyEvent
 * \brief Representa um evento de tecla (ex.: código da tecla, SHIFT, etc.).
 */
struct KeyEvent {
    uint8_t code;   ///< Código da tecla pressionada
    bool shift;     ///< Indica se SHIFT estava ativo
};

/**
 * \class KeyboardProtocol
 * \brief Implementa a lógica de comunicação com o teclado via TCP.
 *
 * Internamente, este protocolo cria sua própria conexão TCP (por exemplo, "192.168.100.1:3001")
 * e monta os frames conforme o padrão definido no manual do teclado. Não expõe métodos
 * públicos para conexão/disconexão, pois isso é gerenciado internamente.
 */
class KeyboardProtocol {
public:
    /**
     * \brief Construtor padrão: cria internamente a conexão TCP para o teclado.
     */
    KeyboardProtocol();

    /**
     * \brief Destrutor.
     */
    ~KeyboardProtocol();

    /**
     * \brief Envia comando para acender/apagar o LED de uma tecla (CMD_LED_TECLA – 0x78).
     * \param keyCode Código da tecla.
     * \param on True para ligar, false para desligar.
     * \return True se o envio foi bem-sucedido.
     */
    bool setLedTecla(uint8_t keyCode, bool on);

    /**
     * \brief Envia comando para acionar o buzzer (CMD_BUZZER – 0x79).
     * \param ms Tempo (em milissegundos) para tocar.
     * \return True se o envio foi bem-sucedido.
     */
    bool setBuzzer(uint16_t ms);

    /**
     * \brief Assina callback para quando uma tecla for pressionada (CMD_KEY_PRESSED – 0x70).
     * \param callback Função a ser chamada com o KeyEvent.
     */
    void subscribeKeyPress(std::function<void(const KeyEvent&)> callback);

    /**
     * \brief Constrói um frame de comunicação para o teclado.
     *
     * \param cmd Comando do teclado (KBCommand).
     * \param payload Vetor de bytes contendo os dados do comando.
     * \return Um vetor de bytes representando o frame completo.
     */
    std::vector<uint8_t> buildFrame(keyboard::config::KBCommand cmd,
                                    const std::vector<uint8_t>& payload = {});

private:
    // Funções internas para envio e processamento de frames
    void onDataReceived(const std::vector<uint8_t>& data);
    bool sendFrame(keyboard::config::KBCommand cmd, const std::vector<uint8_t>& payload);
    void parseFrame(const std::vector<uint8_t>& frame);

    // PImpl para esconder a implementação
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace keyboard::protocols

#endif // KEYBOARD_PROTOCOL_H
