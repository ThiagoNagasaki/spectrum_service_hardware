#include "rs232_transport.h"
#include "../enum_/enum_transportstatus.h"
#include "../../../utils/logger.h"
#include "../../../utils/enum_/enum_commandcontext.h"
#include "../../../utils/enum_/enum_errorcode.h"
#include "../../transport/enum_/enum_bauntrate.h"

#include <fmt/core.h>
#include <fcntl.h>      // open()
#include <termios.h>    // termios, tcgetattr, tcsetattr
#include <unistd.h>     // close()
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>

/**
 * \brief Função auxiliar para mapear enum BaudRate para as constantes speed_t do POSIX.
 */
static speed_t to_speed_t(transport::enum_::BaudRate rate) {
    using transport::enum_::BaudRate;
    switch (rate) {
    case BaudRate::B4800:   return B4800;
    case BaudRate::B9600:   return B9600;
    case BaudRate::B19200:  return B19200;
    case BaudRate::B38400:  return B38400;
    case BaudRate::B57600:  return B57600;
    case BaudRate::B115200: return B115200;
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
 * \class RS232Transport::Impl
 * \brief Implementação interna (PImpl) do RS232Transport.
 */
class RS232Transport::Impl {
public:
    Impl(const std::string& device, enum_::BaudRate baud_rate)
        : device_(device)
        , baud_rate_(baud_rate)
        , fd_(-1)
        , status_(TransportStatus::Disconnected)
    {
        logger_.init(); // Se quiser log em arquivo, ex.: logger_.init("logs/rs232.log");
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        if (status_ == TransportStatus::Connected) {
            logger_.debug(CommandContext::HARDWARE,
                          fmt::format("RS232 já conectado em {}", device_));
            return true;
        }

        fd_ = ::open(device_.c_str(), O_RDWR | O_NOCTTY);
        if (fd_ < 0) {
            logger_.error(CommandContext::HARDWARE, ErrorCode::RS232ConnectionFailure,
                          fmt::format("Erro ao abrir porta RS232: {}", device_));
            status_ = TransportStatus::Error;
            return false;
        }

        struct termios tty{};
        if (::tcgetattr(fd_, &tty) != 0) {
            logger_.error(CommandContext::HARDWARE, ErrorCode::RS232ConfigurationFailure,
                          fmt::format("Erro ao obter atributos da porta RS232: {}", device_));
            status_ = TransportStatus::Error;
            return false;
        }

        // Converte enum para speed_t
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
            logger_.error(CommandContext::HARDWARE, ErrorCode::RS232ConfigurationFailure,
                          fmt::format("Erro ao configurar porta RS232: {}", device_));
            status_ = TransportStatus::Error;
            return false;
        }

        status_ = TransportStatus::Connected;
        logger_.info(CommandContext::HARDWARE,
                     fmt::format("Conectado à porta RS232: {} (baud {})", device_, baud_rate_to_string(baud_rate_)));
        return true;
    }

    bool disconnect() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
        status_ = TransportStatus::Disconnected;
        logger_.info(CommandContext::HARDWARE,
                     fmt::format("Porta RS232 desconectada: {}", device_));
        return true;
    }

    bool send(const std::vector<uint8_t>& data) {
        if (status_ != TransportStatus::Connected || fd_ < 0) {
            logger_.warning(CommandContext::HARDWARE,
                            fmt::format("Tentativa de envio sem conexão na porta RS232: {}", device_));
            return false;
        }

        ssize_t bytes_written = ::write(fd_, data.data(), data.size());
        if (bytes_written < 0 || static_cast<size_t>(bytes_written) != data.size()) {
            logger_.error(CommandContext::HARDWARE, ErrorCode::RS232DataSendFailure,
                          fmt::format("Falha ao enviar dados pela porta RS232: {}", device_));
            return false;
        }

        logger_.debug(CommandContext::HARDWARE,
                      fmt::format("Enviados {} bytes pela porta RS232: {}", bytes_written, device_));
        return true;
    }

    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
        receive_callback_ = std::move(callback);
        // OBS: sem leitura assíncrona, esse callback não é acionado sozinho.
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
        case BaudRate::B4800:   return "4800";
        case BaudRate::B9600:   return "9600";
        case BaudRate::B19200:  return "19200";
        case BaudRate::B38400:  return "38400";
        case BaudRate::B57600:  return "57600";
        case BaudRate::B115200: return "115200";
        default:                return "desconhecido";
        }
    }

    std::string device_;
    enum_::BaudRate baud_rate_;
    int fd_;
    TransportStatus status_;

    Logger logger_;
    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

// =========== RS232Transport (métodos públicos) ===========

RS232Transport::RS232Transport(const std::string& device, enum_::BaudRate baud_rate)
    : pImpl_(std::make_unique<Impl>(device, baud_rate))
{
}

RS232Transport::~RS232Transport() = default;

bool RS232Transport::connect() {
    return pImpl_->connect();
}

bool RS232Transport::disconnect() {
    return pImpl_->disconnect();
}

bool RS232Transport::send(const std::vector<uint8_t>& data) {
    return pImpl_->send(data);
}

void RS232Transport::subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->subscribe(std::move(callback));
}

TransportStatus RS232Transport::get_status() const {
    return pImpl_->get_status();
}

} // namespace transport::serial
