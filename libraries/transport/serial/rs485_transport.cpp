#include "rs485_transport.h"
#include "../enum_/enum_transportstatus.h"
#include "../../../utils/logger/logger.h"                    // Singleton Logger
#include "../../../utils/enum_/enum_commandcontext.h" // CommandContext
#include "../../../utils/enum_/enum_errorcode.h"      // ErrorCode
#include "../../transport/enum_/enum_baudrate.h"      // enum_::BaudRate

#include <fmt/core.h>
#include <fcntl.h>      // open()
#include <unistd.h>     // close()
#include <termios.h>    // termios, tcgetattr, tcsetattr, B9600 etc.
#include <cstring>
#include <iostream>

/**
 * \brief Mapeia enum BaudRate -> speed_t do POSIX (ex.: B9600, B115200).
 */
static speed_t to_speed_t(transport::enum_::BaudRate rate) {
    using transport::enum_::BaudRate;
    switch (rate) {
    case BaudRate::BR_4800:   return B4800;
    case BaudRate::BR_9600:   return B9600;
    case BaudRate::BR_19200:  return B19200;
    case BaudRate::BR_38400:  return B38400;
    case BaudRate::BR_57600:  return B57600;
    case BaudRate::BR_115200: return B115200;
    default:
        return B9600; // fallback
    }
}

namespace transport::serial {

using transport::enum_::TransportStatus;
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

/**
 * \class RS485Transport::Impl
 * \brief Implementação interna do RS485Transport (PImpl).
 */
class RS485Transport::Impl {
public:
    Impl(const std::string& device, enum_::BaudRate baud_rate)
        : device_(device)
        , baud_rate_(baud_rate)
        , fd_(-1)
        , status_(TransportStatus::Disconnected)
    {
        Logger::instance().debug(CommandContext::HARDWARE,
                                 fmt::format("RS485Transport Impl criado para dispositivo {} (baud enum={})",
                                             device_, static_cast<int>(baud_rate_)));
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        if (status_ == TransportStatus::Connected) {
            Logger::instance().debug(CommandContext::HARDWARE,
                                     fmt::format("RS485 já conectado em {}", device_));
            return true;
        }

        fd_ = ::open(device_.c_str(), O_RDWR | O_NOCTTY);
        if (fd_ < 0) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::RS485ConfigurationFailure,
                                     fmt::format("Erro ao abrir porta RS485: {}", device_));
            status_ = TransportStatus::Error;
            return false;
        }

        struct termios tty{};
        if (::tcgetattr(fd_, &tty) != 0) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::RS485ConnectionFailure,
                                     fmt::format("Erro ao obter atributos da porta RS485: {}", device_));
            status_ = TransportStatus::Error;
            return false;
        }

        // Mapeia baud rate
        speed_t speed = to_speed_t(baud_rate_);
        ::cfsetospeed(&tty, speed);
        ::cfsetispeed(&tty, speed);

        // Configura 8N1
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;

        if (::tcsetattr(fd_, TCSANOW, &tty) != 0) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::RS485ConfigurationFailure,
                                     fmt::format("Erro ao configurar atributos da porta RS485: {}", device_));
            status_ = TransportStatus::Error;
            return false;
        }

        status_ = TransportStatus::Connected;
        Logger::instance().info(CommandContext::HARDWARE,
                                fmt::format("Conectado à porta RS485: {} (baud {})",
                                            device_, baud_rate_to_string(baud_rate_)));
        return true;
    }

    bool disconnect() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
        status_ = TransportStatus::Disconnected;
        Logger::instance().info(CommandContext::HARDWARE,
                                fmt::format("Porta RS485 desconectada: {}", device_));
        return true;
    }

    bool send(const std::vector<uint8_t>& data) {
        if (status_ != TransportStatus::Connected || fd_ < 0) {
            Logger::instance().warning(CommandContext::HARDWARE, ErrorCode::RS485ConnectionFailure,
                                       fmt::format("Tentativa de envio sem conexão ativa na porta {}", device_));
            return false;
        }

        ssize_t bytes_written = ::write(fd_, data.data(), data.size());
        if (bytes_written < 0 || static_cast<size_t>(bytes_written) != data.size()) {
            Logger::instance().error(CommandContext::HARDWARE, ErrorCode::RS485DataSendFailure,
                                     fmt::format("Erro ao enviar dados pela porta RS485: {}", device_));
            return false;
        }

        Logger::instance().debug(CommandContext::HARDWARE,
                                 fmt::format("Enviados {} bytes pela RS485: {}", bytes_written, device_));
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
     * \brief Converte BaudRate em string para logs.
     */
    static std::string baud_rate_to_string(enum_::BaudRate br) {
        using transport::enum_::BaudRate;
        switch (br) {
            case BaudRate::BR_4800:   return "4800";
            case BaudRate::BR_9600:   return "9600";
            case BaudRate::BR_19200:  return "19200";
            case BaudRate::BR_38400:  return "38400";
            case BaudRate::BR_57600:  return "57600";
            case BaudRate::BR_115200: return "115200";
            default:                  return "desconhecido";
        }
    }

    std::string device_;
    enum_::BaudRate baud_rate_;
    int fd_;
    TransportStatus status_;
    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

// ==================== RS485Transport ====================

RS485Transport::RS485Transport(const std::string& device, enum_::BaudRate baud_rate)
    : pImpl_(std::make_unique<Impl>(device, baud_rate))
{
}

RS485Transport::~RS485Transport() = default;

bool RS485Transport::connect() {
    return pImpl_->connect();
}

bool RS485Transport::disconnect() {
    return pImpl_->disconnect();
}

bool RS485Transport::send(const std::vector<uint8_t>& data) {
    return pImpl_->send(data);
}

void RS485Transport::subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->subscribe(std::move(callback));
}

TransportStatus RS485Transport::get_status() const {
    return pImpl_->get_status();
}

} // namespace transport::serial
