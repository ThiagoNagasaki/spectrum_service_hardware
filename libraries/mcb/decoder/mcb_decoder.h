#ifndef MCB_DECODER_H
#define MCB_DECODER_H

#include <vector>
#include <string>
#include <cstdint>
#include <memory>

namespace mcb {
namespace decoder {

/**
 * @brief Classe responsável por decodificar os frames dos comandos da MCB.
 *
 * Esta classe interpreta os payloads dos frames recebidos (via Ethernet, por exemplo),
 * de acordo com as especificações do manual técnico. Ela decodifica os comandos de leitura
 * (como firmware, status, entradas digitais/analógicas, sensores, data/hora, MAC, e informações)
 * e também interpreta as confirmações (ACK) dos comandos de escrita.
 */
class MCBDecoder {
public:
    MCBDecoder();
    ~MCBDecoder();

    /**
     * @brief Decodifica o payload do comando READ_FIRMWARE (0x51).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a versão e revisão do firmware.
     */
    std::string decodeFirmwarePayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_STATUS (0x52).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a interpretação do status da MCB.
     */
    std::string decodeStatusPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_INPUT_DIGITAL (0x53).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a leitura da entrada digital.
     */
    std::string decodeInputDigitalPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_ANALOG_INPUT (0x54).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com o valor lido da entrada analógica.
     */
    std::string decodeAnalogInputPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_SENSOR_DISTANCE (0x55).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a distância (ou velocidade) medida.
     */
    std::string decodeSensorDistancePayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_STATUS_SPECIAL (0x56).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com o status especial.
     */
    std::string decodeStatusSpecialPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_CHAVE_LIGA (0x5A).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com o estado da chave liga.
     */
    std::string decodeChaveLigaPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_DATA_HORA (0x5B).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com a data e hora.
     */
    std::string decodeDataHoraPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload do comando READ_MAC_PC (0x5C).
     * @param payload Vetor de bytes contendo o payload.
     * @return String com o endereço MAC.
     */
    std::string decodeMacPcPayload(const std::vector<uint8_t>& payload) const;

    /**
     * @brief Decodifica o payload dos comandos READ_INFO1/2/3 (0x5D, 0x5E, 0x5F).
     *        Cada comando espera um payload de 15 bytes.
     * @param payload Vetor de bytes contendo o payload.
     * @param infoName Nome do comando ("Info1", "Info2" ou "Info3").
     * @return String com a informação decodificada.
     */
    std::string decodeInfoPayload(const std::vector<uint8_t>& payload, const std::string& infoName) const;

    /**
     * @brief Decodifica o ACK genérico para os comandos de escrita (0x61 a 0x6F).
     * @param payload Vetor de bytes com a resposta (pode estar vazio).
     * @param commandName Nome do comando.
     * @return String indicando o sucesso da operação e, se houver, a resposta em hexadecimal.
     */
    std::string decodeWriteAckPayload(const std::vector<uint8_t>& payload, const std::string& commandName) const;

    /**
     * @brief Decodifica um frame completo recebido da MCB.
     *
     * O frame tem a estrutura: [STX, Tamanho, Comando, Payload..., Checksum, ETX].
     * Esta função valida o frame e, com base no comando, delega a decodificação do payload.
     *
     * @param frame Vetor de bytes que compõe o frame.
     * @return String com a decodificação completa do frame.
     */
    std::string decodeFrame(const std::vector<uint8_t>& frame) const;

private:
    /**
     * @brief Classe interna (PImpl) com toda a lógica de decodificação.
     */
    class Impl;
    /**
     * @brief Ponteiro único para a implementação oculta.
     */
    std::unique_ptr<Impl> pImpl_;
};

} // namespace decoder
} // namespace mcb

#endif // MCB_DECODER_H
