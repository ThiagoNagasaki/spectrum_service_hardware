#ifndef MCB_PROTOCOL_H
#define MCB_PROTOCOL_H

#include "../config/mcb_constants.h"  // STX, ETX, MCBCommand, MCB_MIN_FRAME_SIZE
#include <vector>
#include <optional>
#include <memory>

namespace mcb::protocols {

/**
 * \brief Estrutura que representa um frame MCB decodificado.
 */
struct MCBFrame {
    mcb::config::MCBCommand command; ///< Comando (ex.: 0x51, 0x52 etc.)
    std::vector<uint8_t> data;       ///< Dados do frame (payload)
};

/**
 * \class MCBProtocol
 * \brief Classe para manipular frames do protocolo MCB, usando PImpl e Singleton logger.
 *
 * Métodos principais:
 *  - buildFrame(): Gera o frame [STX, length, command, data..., checksum, ETX].
 *  - parseFrame(): Valida buffer, checa STX, ETX, tamanho e checksum.
 *  - Faz logs via \c Logger::instance().
 */
class MCBProtocol {
public:
    /**
     * \brief Construtor padrão.
     *        Logger Singleton será acessado internamente.
     */
    MCBProtocol();

    /**
     * \brief Destrutor.
     */
    ~MCBProtocol();

    /**
     * \brief Monta um frame MCB a partir de um comando e de um payload.
     * \param cmd Comando MCB (ex.: MCBCommand::READ_FIRMWARE).
     * \param payload Bytes de dados do comando.
     * \return Vetor de bytes representando o frame completo.
     */
    std::vector<uint8_t> buildFrame(mcb::config::MCBCommand cmd, 
                                    const std::vector<uint8_t>& payload) const;

    /**
     * \brief Tenta analisar um buffer como um frame MCB.
     * \param buffer Buffer de bytes a analisar.
     * \return std::optional<MCBFrame> contendo o frame decodificado,
     *         ou std::nullopt se falhar (ex.: checksum inválido).
     */
    std::optional<MCBFrame> parseFrame(const std::vector<uint8_t>& buffer) const;

private:
    /**
     * \brief Classe interna (PImpl) com toda a lógica do protocolo.
     */
    class Impl;

    /**
     * \brief Ponteiro único para a implementação oculta.
     */
    std::unique_ptr<Impl> pImpl_;
};

} // namespace mcb::protocols

#endif // MCB_PROTOCOL_H
