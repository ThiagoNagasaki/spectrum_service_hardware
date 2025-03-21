#include "logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <vector>

namespace mcb_transport {
namespace utils {

/**
 * \brief Array para mapear CommandContext em string.
 *
 * Índices:
 *  0 -> GENERAL
 *  1 -> NETWORK
 *  2 -> SYSTEM
 *  3 -> SECURITY
 *  4 -> DEVICE_CONTROL
 *  5 -> UNKNOWN
 */
static const std::string ContextString[] = {
    "GENERAL",
    "NETWORK",
    "SYSTEM",
    "SECURITY",
    "DEVICE_CONTROL",
    "UNKNOWN"
};

/**
 * \class Logger::Impl
 * \brief Implementação interna (PImpl) do Logger usando spdlog.
 */
class Logger::Impl {
public:
    Impl() = default;
    ~Impl() = default;

    /**
     * \brief Inicializa sinks e configura spdlog (se ainda não foi inicializado).
     */
    void init(const std::string& logFileName) {
        if (initialized_) {
            return;
        }

        try {
            // Sink diário: cria novo arquivo todo dia à 00:00
            auto dailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logFileName, 0, 0);

            // Sink para console colorido
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            // Cria logger com múltiplos sinks
            std::vector<spdlog::sink_ptr> sinks{ dailySink, consoleSink };
            logger_ = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

            // Registra e define como logger padrão
            spdlog::register_logger(logger_);
            spdlog::set_default_logger(logger_);

            // Formato e nível de log
            spdlog::set_pattern("[%T.%e] [%^%l%$] [%n] %v");
            spdlog::set_level(spdlog::level::trace);

            initialized_ = true;
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Falha ao inicializar o logger: " << ex.what() << std::endl;
        }
    }

    /**
     * \brief Converte CommandContext em string, garantindo que índices inválidos virem "UNKNOWN".
     */
    std::string contextToString(mcb_transport::models::enum_::CommandContext context) {
        int index = static_cast<int>(context);
        constexpr int maxIndex = static_cast<int>(sizeof(ContextString) / sizeof(std::string));
        if (index < 0 || index >= maxIndex) {
            return ContextString[5]; // "UNKNOWN"
        }
        return ContextString[index];
    }

    void trace(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
        if (initialized_) {
            spdlog::trace("[{}] {}", contextToString(context), message);
        }
    }

    void debug(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
        if (initialized_) {
            spdlog::debug("[{}] {}", contextToString(context), message);
        }
    }

    void info(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
        if (initialized_) {
            spdlog::info("[{}] {}", contextToString(context), message);
        }
    }

    void warning(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
        if (initialized_) {
            spdlog::warn("[{}] {}", contextToString(context), message);
        }
    }

    void error(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
        if (initialized_) {
            spdlog::error("[{}] {}", contextToString(context), message);
        }
    }

    void fatal(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
        if (initialized_) {
            spdlog::critical("[{}] {}", contextToString(context), message);
        }
    }

private:
    bool initialized_ = false;
    std::shared_ptr<spdlog::logger> logger_;
};


Logger::Logger() : pImpl_(std::make_unique<Impl>()) {
}

Logger::~Logger() = default;

void Logger::init(const std::string& logFileName) {
    pImpl_->init(logFileName);
}

void Logger::trace(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
    pImpl_->trace(context, message);
}

void Logger::debug(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
    pImpl_->debug(context, message);
}

void Logger::info(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
    pImpl_->info(context, message);
}

void Logger::warning(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
    pImpl_->warning(context, message);
}

void Logger::error(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
    pImpl_->error(context, message);
}

void Logger::fatal(mcb_transport::models::enum_::CommandContext context, const std::string& message) {
    pImpl_->fatal(context, message);
}

} // namespace utils
} // namespace mcb_transport
