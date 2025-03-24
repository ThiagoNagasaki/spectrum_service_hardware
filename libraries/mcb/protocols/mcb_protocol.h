#ifndef MCB_PROTOCOL_H
#define MCB_PROTOCOL_H

#include "../config/mcb_constants.h"         // Para STX, ETX, MCBCommand
#include "../../../utils/logger.h"           // Para utils::Logger
#include <vector>
#include <optional>
#include <memory>

namespace mcb::protocols {

/**
 * \brief Estrutura representando um frame MCB decodificado.
 */
struct MCBFrame {
    MCBCommand command;          ///< Comando (ex.: 0x51, 0x52 etc.)
    std::vector<uint8_t> data;   ///< Dados do frame (payload)
};

/**
 * \class MCBProtocol
 * \brief Classe para manipular frames do protocolo MCB, usando PImpl.
 *
 * Métodos principais:
 *  - buildFrame(): gera o frame [STX, length, command, data..., checksum, ETX].
 *  - parseFrame(): valida buffer, checa STX, ETX, tamanho e checksum.
 *  - Usa \c utils::Logger para registrar mensagens (debug, warning, etc.).
 */
class MCBProtocol {
public:
    /**
     * \brief Construtor que recebe referência a um \c Logger.
     * \param logger Instância de logger (PImpl) para registrar logs.
     */
    explicit MCBProtocol(utils::Logger& logger);

    /**
     * \brief Destrutor padrão.
     */
    ~MCBProtocol();

    /**
     * \brief Monta um frame MCB a partir de um comando e de um payload.
     * \param cmd Comando MCB (ex.: MCBCommand::READ_FIRMWARE).
     * \param payload Bytes de dados do comando.
     * \return Vetor de bytes representando o frame completo.
     */
    std::vector<uint8_t> buildFrame(MCBCommand cmd, const std::vector<uint8_t>& payload) const;

    /**
     * \brief Tenta analisar um buffer como um frame MCB.
     * \param buffer Buffer de bytes a analisar.
     * \return \c std::optional<MCBFrame> contendo o frame decodificado,
     *         ou \c std::nullopt se falhar (ex.: checksum inválido).
     */
    std::optional<MCBFrame> parseFrame(const std::vector<uint8_t>& buffer) const;

private:
    /**
     * \brief Classe interna que contém toda a implementação (PImpl).
     */
    class Impl;

    /**
     * \brief Ponteiro para a implementação oculta.
     */
    std::unique_ptr<Impl> pImpl_;
};

} // namespace mcb::protocols

#endif // MCB_PROTOCOL_H
