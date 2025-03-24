#include "logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <vector>

namespace utils {
using enum_::CommandContext;
using enum_::ErrorCode;

static const std::string ContextString[] = {
    "HARDWARE", "USER", "UNKNOWN"
};

static const std::string ErrorCodeString[] = {
    "GeneratorConnectionError"
};

class Logger::Impl {
public:
    void init(const std::string& logFileName) {
        if (initialized_) return;

        try {
            auto dailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logFileName, 0, 0);
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            std::vector<spdlog::sink_ptr> sinks{ dailySink, consoleSink };
            logger_ = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

            spdlog::register_logger(logger_);
            spdlog::set_default_logger(logger_);
            spdlog::set_pattern("[%T.%e] [%^%l%$] [%n] %v");
            spdlog::set_level(spdlog::level::trace);

            initialized_ = true;
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Falha ao inicializar o logger: " << ex.what() << std::endl;
        }
    }

    std::string contextToString(CommandContext context) {
        int index = static_cast<int>(context);
        constexpr int maxIndex = sizeof(ContextString) / sizeof(std::string);
        return (index < 0 || index >= maxIndex) ? ContextString[2] : ContextString[index];
    }

    std::string errorCodeToString(ErrorCode code) {
        int index = static_cast<int>(code);
        constexpr int maxIndex = sizeof(ErrorCodeString) / sizeof(std::string);
        return (index < 0 || index >= maxIndex) ? "UnknownError" : ErrorCodeString[index];
    }

    void trace(CommandContext context, const std::string& message) {
        if (initialized_) spdlog::trace("[{}] {}", contextToString(context), message);
    }

    void debug(CommandContext context, const std::string& message) {
        if (initialized_) spdlog::debug("[{}] {}", contextToString(context), message);
    }

    void info(CommandContext context, const std::string& message) {
        if (initialized_) spdlog::info("[{}] {}", contextToString(context), message);
    }

    void warning(CommandContext context, ErrorCode code, const std::string& message) {
        if (initialized_) spdlog::warn("[{}][{}] {}", contextToString(context), errorCodeToString(code), message);
    }

    void error(CommandContext context, ErrorCode code, const std::string& message) {
        if (initialized_) spdlog::error("[{}][{}] {}", contextToString(context), errorCodeToString(code), message);
    }

    void fatal(CommandContext context, ErrorCode code, const std::string& message) {
        if (initialized_) spdlog::critical("[{}][{}] {}", contextToString(context), errorCodeToString(code), message);
    }

private:
    bool initialized_ = false;
    std::shared_ptr<spdlog::logger> logger_;
};

// Encapsulamento público
Logger::Logger() : pImpl_(std::make_unique<Impl>()) {}
Logger::~Logger() = default;

void Logger::init(const std::string& logFileName) { pImpl_->init(logFileName); }
void Logger::trace(CommandContext context, const std::string& message) { pImpl_->trace(context, message); }
void Logger::debug(CommandContext context, const std::string& message) { pImpl_->debug(context, message); }
void Logger::info(CommandContext context, const std::string& message) { pImpl_->info(context, message); }
void Logger::warning(CommandContext context, ErrorCode code, const std::string& message) { pImpl_->warning(context, code, message); }
void Logger::error(CommandContext context, ErrorCode code, const std::string& message) { pImpl_->error(context, code, message); }
void Logger::fatal(CommandContext context, ErrorCode code, const std::string& message) { pImpl_->fatal(context, code, message); }

} // namespace utils
