#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <string>

namespace mcb_transport {
namespace models {
namespace enum_ {

/**
 * \enum CommandContext
 * \brief Enum para categorizar logs (ex.: GENERAL, NETWORK, etc.).
 */
enum class CommandContext {
    GENERAL = 0,
    NETWORK,
    SYSTEM,
    SECURITY,
    DEVICE_CONTROL,
    UNKNOWN
};

} // namespace enum_
} // namespace models

namespace utils {

/**
 * \class Logger
 * \brief Classe de log usando PImpl e spdlog.
 *
 * Você cria uma instância, chama \c init() para configurar os sinks
 * (arquivo diário e console colorido) e depois usa \c info(), \c error() etc.
 *
 * Exemplo de uso:
 * \code
 *   Logger logger;
 *   logger.init("logs/app.log");
 *   logger.info(CommandContext::NETWORK, "Conectado com sucesso!");
 * \endcode
 */
class Logger {
public:
    /**
     * \brief Construtor que inicializa a estrutura PImpl.
     */
    Logger();

    /**
     * \brief Destrutor padrão. Fecha sinks se necessário.
     */
    ~Logger();

    /**
     * \brief Inicializa o spdlog com um arquivo diário e console colorido.
     * \param logFileName Nome do arquivo de log (p. ex. "logs/app.log").
     *
     * Chame este método apenas uma vez por instância. Caso já esteja inicializado,
     * ele não faz nada.
     */
    void init(const std::string& logFileName = "logs/app.log");

    /**
     * \brief Loga uma mensagem em nível TRACE, com um contexto.
     */
    void trace(mcb_transport::models::enum_::CommandContext context, const std::string& message);

    /**
     * \brief Loga uma mensagem em nível DEBUG, com um contexto.
     */
    void debug(mcb_transport::models::enum_::CommandContext context, const std::string& message);

    /**
     * \brief Loga uma mensagem em nível INFO, com um contexto.
     */
    void info(mcb_transport::models::enum_::CommandContext context, const std::string& message);

    /**
     * \brief Loga uma mensagem em nível WARNING, com um contexto.
     */
    void warning(mcb_transport::models::enum_::CommandContext context, const std::string& message);

    /**
     * \brief Loga uma mensagem em nível ERROR, com um contexto.
     */
    void error(mcb_transport::models::enum_::CommandContext context, const std::string& message);

    /**
     * \brief Loga uma mensagem em nível CRITICAL, com um contexto (equivalente a 'fatal').
     */
    void fatal(mcb_transport::models::enum_::CommandContext context, const std::string& message);

private:

    class Impl;
   
    std::unique_ptr<Impl> pImpl_;
};

} // namespace utils
} // namespace mcb_transport

#endif // LOGGER_H
