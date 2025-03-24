#ifndef LOGGER_H
#define LOGGER_H

#include "enum_/enum_commandcontext.h"
#include <memory>
#include <string>

namespace utils {
using enum_::CommandContext;
using enum_::ErrorCode;

/**
 * \class Logger
 * \brief Classe de log usando PImpl e spdlog.
 */
class Logger {
public:
    Logger();
    ~Logger();

    void init(const std::string& logFileName = "logs/app.log");

    void trace(CommandContext context, const std::string& message);
    void debug(CommandContext context, const std::string& message);
    void info(CommandContext context, const std::string& message);

    void warning(CommandContext context, ErrorCode code, const std::string& message);
    void error(CommandContext context, ErrorCode code, const std::string& message);
    void fatal(CommandContext context, ErrorCode code, const std::string& message);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace utils

#endif // LOGGER_H
