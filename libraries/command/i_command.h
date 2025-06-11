// libraries/command/i_command.h
#ifndef I_COMMAND_H
#define I_COMMAND_H

#include <memory>

namespace command {

/**
 * \brief Interface genérica para todo comando que retorna um T.
 *
 * @tparam T Tipo de retorno do execute().
 */
template<typename T>
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual T execute() = 0;
};

/**
 * @brief Alias para ponteiro de comando.
 */
template<typename T>
using ICommandPtr = std::shared_ptr<ICommand<T>>;

} // namespace command

#endif // I_COMMAND_H
