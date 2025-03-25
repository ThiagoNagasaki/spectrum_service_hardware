#ifndef KEYBOARD_PROTOCOL_H
#define KEYBOARD_PROTOCOL_H

#include "../../transport/interface/i_transport.h" // Para ITransport
#include "../config/keyboard_constants.h"          // CMD_KEY_PRESSED, etc.
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
    bool shift;     ///< Indica se SHIFT estava ativo, se for o caso
    // Você pode expandir com mais informações
};

/**
 * \class KeyboardProtocol
 * \brief Implementa a lógica de comunicação com o teclado via RS485, usando \c ITransport.
 *
 * - Recebe frames do teclado (ex.: 0x70 -> Tecla Pressionada).
 * - Envia comandos para controlar LED de tecla, buzzer, beep, etc. (ex.: 0x78, 0x79...).
 * - Oferece métodos de alto nível (setLedTecla, setBuzzer, etc.).
 * - Exibe KeyEvent via callback subscribeKeyPress().
 */
class KeyboardProtocol {
public:
    /**
     * \brief Construtor que recebe um transporte RS485 (ou outro ITransport).
     */
    explicit KeyboardProtocol(std::shared_ptr<transport::interface::ITransport> transport);

    /**
     * \brief Destrutor.
     */
    ~KeyboardProtocol();

    /**
     * \brief Conecta (internamente chama transport->connect()).
     */
    bool connect();

    /**
     * \brief Desconecta (transport->disconnect()).
     */
    bool disconnect();

    /**
     * \brief Envia comando para acender/apagar LED de uma tecla (0x78).
     * \param keyCode Código da tecla.
     * \param on true para ligar, false para desligar.
     * \return true se envio foi bem-sucedido.
     */
    bool setLedTecla(uint8_t keyCode, bool on);

    /**
     * \brief Envia comando para acionar buzzer (0x79).
     * \param ms Tempo em milissegundos para tocar.
     * \return true se envio foi bem-sucedido.
     */
    bool setBuzzer(uint16_t ms);

    /**
     * \brief Assina callback para quando uma tecla for pressionada (0x70).
     * \param callback Função a ser chamada com o \c KeyEvent.
     */
    void subscribeKeyPress(std::function<void(const KeyEvent&)> callback);

private:
    /**
     * \brief Chamado pelo transporte quando dados chegam via RS485.
     */
    void onDataReceived(const std::vector<uint8_t>& data);

    /**
     * \brief Constrói e envia um frame para o teclado: [STX, length, cmd, payload..., ETX].
     */
    bool sendFrame(uint8_t cmd, const std::vector<uint8_t>& payload);

    /**
     * \brief Decodifica o frame do teclado e aciona callbacks.
     *        Exemplo simples, assumindo [STX, length, cmd, payload..., ETX].
     */
    void parseFrame(const std::vector<uint8_t>& frame);

    std::shared_ptr<transport::interface::ITransport> transport_;
    std::function<void(const KeyEvent&)> keyPressCallback_;
};

} // namespace keyboard::protocols

#endif // KEYBOARD_PROTOCOL_H
