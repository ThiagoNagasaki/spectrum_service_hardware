#include "serial_transport.h"
#include "../enum_/enum_transportstatus.h"
#include "../enum_/enum_baudrate.h"
#include "../../../utils/logger.h"                  
#include "../../../utils/enum_/enum_commandcontext.h"
#include "../../../utils/enum_/enum_errorcode.h"

#include <fmt/core.h>
#include <fcntl.h>      // open()
#include <unistd.h>     // close()
#include <termios.h>    // termios, tcgetattr, tcsetattr, B9600 etc.
#include <cstring>
#include <iostream>

namespace transport::serial {

using transport::enum_::TransportStatus;
using transport::enum_::BaudRate;
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

/**
 * \brief Mapeia enum BaudRate para speed_t do POSIX.
 */
static speed_t to_speed_t(BaudRate rate) {
    switch (rate) {
        case BaudRate::BR_4800:   return B4800;
        case BaudRate::BR_9600:   return B9600;
        case BaudRate::BR_19200:  return B19200;
        case BaudRate::BR_38400:  return B38400;
        case BaudRate::BR_57600:  return B57600;
        case BaudRate::BR_115200: return B115200;
        default:
            return B9600;  
    }
}

/**
 * \class SerialTransport::Impl
 * \brief Implementação interna (PImpl) do transporte serial.
 */
class SerialTransport::Impl {
public:
    Impl(const std::string& device, BaudRate baud_rate, SerialType type)
        : device_(device)
        , baud_rate_(baud_rate)
        , type_(type)
        , fd_(-1)
        , status_(TransportStatus::Disconnected)
    {

        Logger::instance().debug(CommandContext::HARDWARE,
            fmt::format("SerialTransport Impl criado para {} (tipo={}, baud={})",
                        device_, toString(type_), static_cast<int>(baud_rate_)));
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        if (status_ == TransportStatus::Connected) {
            Logger::instance().debug(CommandContext::HARDWARE,
                fmt::format("SerialTransport ({}): já conectado em {}", toString(type_), device_));
            return true;
        }

        fd_ = ::open(device_.c_str(), O_RDWR | O_NOCTTY);
        if (fd_ < 0) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                fmt::format("Erro ao abrir porta {} (tipo: {})", device_, toString(type_)));
            status_ = TransportStatus::Error;
            return false;
        }

        struct termios tty{};
        if (::tcgetattr(fd_, &tty) != 0) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                fmt::format("Erro ao obter atributos da porta {} (tipo: {})", device_, toString(type_)));
            status_ = TransportStatus::Error;
            return false;
        }

        // Ajusta baud rate
        speed_t speed = to_speed_t(baud_rate_);
        ::cfsetospeed(&tty, speed);
        ::cfsetispeed(&tty, speed);

        // 8N1
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;

        if (::tcsetattr(fd_, TCSANOW, &tty) != 0) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                fmt::format("Erro ao configurar porta {} (tipo: {})", device_, toString(type_)));
            status_ = TransportStatus::Error;
            return false;
        }

        status_ = TransportStatus::Connected;
        Logger::instance().info(CommandContext::HARDWARE,
            fmt::format("Conectado à porta {} (tipo: {}, baud: {})",
                        device_, toString(type_), baudRateToString(baud_rate_)));
        return true;
    }

    bool disconnect() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
        status_ = TransportStatus::Disconnected;
        Logger::instance().info(CommandContext::HARDWARE,
            fmt::format("Porta {} (tipo: {}) desconectada", device_, toString(type_)));
        return true;
    }

    bool send(const std::vector<uint8_t>& data) {
        if (status_ != TransportStatus::Connected || fd_ < 0) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                fmt::format("Tentativa de envio sem conexão na porta {} (tipo: {})",
                            device_, toString(type_)));
            return false;
        }

        ssize_t bytes_written = ::write(fd_, data.data(), data.size());
        if (bytes_written < 0 || static_cast<size_t>(bytes_written) != data.size()) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::GeneratorConnectionError,
                fmt::format("Falha ao enviar dados pela porta {} (tipo: {})",
                            device_, toString(type_)));
            return false;
        }

        Logger::instance().debug(CommandContext::HARDWARE,
            fmt::format("Enviados {} bytes pela porta {} (tipo: {})",
                        bytes_written, device_, toString(type_)));
        return true;
    }

    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
        receive_callback_ = std::move(callback);
    }

    TransportStatus get_status() const {
        return status_;
    }

private:
    /**
     * \brief Converte SerialType em string para logs.
     */
    static std::string toString(SerialType t) {
        switch (t) {
            case SerialType::RS232: return "RS232";
            case SerialType::RS485: return "RS485";
            default:                return "Desconhecido";
        }
    }

    /**
     * \brief Converte BaudRate em string para logs.
     */
    static std::string baudRateToString(BaudRate br) {
        switch (br) {
            case BaudRate::BR_4800:   return "4800";
            case BaudRate::BR_9600:   return "9600";
            case BaudRate::BR_19200:  return "19200";
            case BaudRate::BR_38400:  return "38400";
            case BaudRate::BR_57600:  return "57600";
            case BaudRate::BR_115200: return "115200";
            default:                  return "Desconhecido";
        }
    }

    std::string device_;
    BaudRate baud_rate_;
    SerialType type_;
    int fd_;
    TransportStatus status_;

    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

// ============ SerialTransport (métodos públicos) ============

SerialTransport::SerialTransport(const std::string& device, BaudRate baud_rate, SerialType type)
    : pImpl_(std::make_unique<Impl>(device, baud_rate, type))
{
}

SerialTransport::~SerialTransport() = default;

bool SerialTransport::connect() {
    return pImpl_->connect();
}

bool SerialTransport::disconnect() {
    return pImpl_->disconnect();
}

bool SerialTransport::send(const std::vector<uint8_t>& data) {
    return pImpl_->send(data);
}

void SerialTransport::subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->subscribe(std::move(callback));
}

TransportStatus SerialTransport::get_status() const {
    return pImpl_->get_status();
}

} // namespace transport::serial
