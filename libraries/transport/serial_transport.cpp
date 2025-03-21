#include "serial_transport.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <iostream>

namespace transport {

/**
 * \brief Função auxiliar para mapear nosso enum BaudRate para os valores speed_t.
 */static speed_t to_speed_t(BaudRate rate)
{
    switch (rate) {
        case BaudRate::BR_4800:   return B4800;
        case BaudRate::BR_9600:   return B9600;
        case BaudRate::BR_19200:  return B19200;
        case BaudRate::BR_38400:  return B38400;
        case BaudRate::BR_57600:  return B57600;
        case BaudRate::BR_115200: return B115200;
        default:
            std::cerr << "[SerialTransport] Baud rate não mapeado. Usando 9600.\n";
            return B9600;
    }
}


/**
 * \class SerialTransport::Impl
 * \brief Implementação interna (oculta) do transporte serial.
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
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        fd_ = open(device_.c_str(), O_RDWR | O_NOCTTY);
        if (fd_ < 0) {
            std::cerr << "[SerialTransport] Erro ao abrir porta " << device_ << "\n";
            status_ = TransportStatus::Error;
            return false;
        }

        struct termios tty{};
        if (tcgetattr(fd_, &tty) != 0) {
            std::cerr << "[SerialTransport] Erro ao obter atributos da porta\n";
            status_ = TransportStatus::Error;
            return false;
        }

        speed_t speed = to_speed_t(baud_rate_);
        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        // Configura 8 bits, sem paridade, 1 stop bit (8N1).
        tty.c_cflag |= (CLOCAL | CREAD); // Habilita recepção e ignora modem lines
        tty.c_cflag &= ~PARENB;         // Sem paridade
        tty.c_cflag &= ~CSTOPB;         // 1 stop bit
        tty.c_cflag &= ~CSIZE;          // Limpa o tamanho
        tty.c_cflag |= CS8;             // 8 bits

        // pode ser preciso habilitar controle de direção com TIOCGRS485/TIOCSRS485 (ioctl) ou GPIOs externos. Fica a critério do hardware ?

        if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
            std::cerr << "[SerialTransport] Erro ao definir atributos da porta\n";
            status_ = TransportStatus::Error;
            return false;
        }

        status_ = TransportStatus::Connected;
        return true;
    }

    bool disconnect() {
        if (fd_ != -1) {
            close(fd_);
            fd_ = -1;
        }
        status_ = TransportStatus::Disconnected;
        return true;
    }

    bool send(const std::vector<uint8_t>& data) {
        if (status_ != TransportStatus::Connected || fd_ < 0) {
            std::cerr << "[SerialTransport] Porta não conectada.\n";
            return false;
        }
        ssize_t bytes_written = write(fd_, data.data(), data.size());
        return (bytes_written == static_cast<ssize_t>(data.size()));
    }

    void set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) {
        receive_callback_ = std::move(callback);
        // Para leitura assíncrona, seria preciso uma thread ou uso de select/poll/epoll.Agora apenas armazeno o callback.
    }

    TransportStatus get_status() const {
        return status_;
    }

private:
    std::string device_;
    BaudRate baud_rate_;
    SerialType type_;
    int fd_;
    TransportStatus status_;
    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

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

void SerialTransport::set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->set_receive_callback(std::move(callback));
}

TransportStatus SerialTransport::get_status() const {
    return pImpl_->get_status();
}

} // namespace transport
