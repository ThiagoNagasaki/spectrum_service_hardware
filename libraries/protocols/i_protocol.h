#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include <optional> 
#include <memory>  
namespace protocols {

/**
 * \brief Interface genérica de protocolo: responsável por framing e parsing.
 */
class IProtocol {
public:
    virtual ~IProtocol() = default;

    /// Monta frame completo a partir de comando e payload
    virtual std::vector<uint8_t> buildFrame(uint8_t cmd, const std::vector<uint8_t>& payload) = 0;

    /// Realiza o parsing de um frame recebido e retorna payload útil
    virtual std::optional<std::vector<uint8_t>> parseFrame(const std::vector<uint8_t>& raw) = 0;

    /// (Opcional) Callback para protocolos que fazem push
    virtual void subscribe(std::function<void(const std::vector<uint8_t>&)> cb) = 0;
};

using IProtocolPtr = std::shared_ptr<IProtocol>;

} // namespace protocols
