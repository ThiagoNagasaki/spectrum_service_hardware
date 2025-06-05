#ifndef LOGGER_H
#define LOGGER_H

#include "enum_/enum_commandcontext.h"
#include "enum_/enum_errorcode.h"
#include <memory>
#include <string>

namespace utils {
using enum_::CommandContext;
using enum_::ErrorCode;

/**
 * \class Logger
 * \brief Logger singleton usando PImpl e spdlog.
 */
class Logger {
public:
    /**
     * \brief Acesso estático à única instância de Logger (singleton).
     */
    static Logger& instance();

    /**
     * \brief Inicializa o logger (arquivo, console, etc.).
     * \param logFileName Caminho do arquivo de log.
     */
    void init(const std::string& logFileName = "logs/app.log");

    // Métodos de log
    void trace(CommandContext context, const std::string& message);
    void debug(CommandContext context, const std::string& message);
    void info(CommandContext context, const std::string& message);
    void warning(CommandContext context, ErrorCode code, const std::string& message);
    void error(CommandContext context, ErrorCode code, const std::string& message);
    void fatal(CommandContext context, ErrorCode code, const std::string& message);

private:
    // Construtor e destrutor privados (Singleton)
    Logger();
    ~Logger();

    // Impedir cópia e movimento
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * \brief Classe interna (PImpl) que faz a lógica real com spdlog.
     */
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace utils

#endif // LOGGER_H
