#pragma once

#include <memory>

namespace controller {

/**
 * \brief Interface abstrata para controladores que executam comandos genéricos.
 *
 * @tparam T Tipo de retorno do comando
 */
template<typename T>
class IController {
public:
    virtual ~IController() = default;

    /// Executa o comando e retorna resultado
    virtual T execute() = 0;
};

} // namespace controller
