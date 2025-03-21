#include "rs485_transport.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <iostream>

namespace transport {

// Classe interna que contém os detalhes da implementação
class RS485Transport::Impl {
public:
    Impl(const std::string& device, int baud_rate)
        : device_(device), baud_rate_(baud_rate), fd_(-1),
          status_(TransportStatus::Disconnected) {}

    ~Impl() {
        disconnect();
    }

    bool connect() {
        fd_ = open(device_.c_str(), O_RDWR | O_NOCTTY);
        if (fd_ < 0) {
            std::cerr << "[RS485] Erro ao abrir porta " << device_ << std::endl;
            status_ = TransportStatus::Error;
            return false;
        }

        struct termios tty{};
        if (tcgetattr(fd_, &tty) != 0) {
            std::cerr << "[RS485] Erro ao obter atributos da porta" << std::endl;
            status_ = TransportStatus::Error;
            return false;
        }

        cfsetospeed(&tty, B9600);
        cfsetispeed(&tty, B9600);
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;

        if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
            std::cerr << "[RS485] Erro ao definir atributos da porta" << std::endl;
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
            std::cerr << "[RS485] Porta não conectada." << std::endl;
            return false;
        }
        ssize_t bytes_written = write(fd_, data.data(), data.size());
        return bytes_written == static_cast<ssize_t>(data.size());
    }

    void set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) {
        receive_callback_ = std::move(callback);
    }

    TransportStatus get_status() const {
        return status_;
    }

private:
    std::string device_;
    int baud_rate_;
    int fd_;
    TransportStatus status_;
    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

// Métodos públicos de RS485Transport delegam para a implementação oculta (Impl)
RS485Transport::RS485Transport(const std::string& device, int baud_rate)
    : pImpl_(std::make_unique<Impl>(device, baud_rate)) {}

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

void RS485Transport::set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->set_receive_callback(std::move(callback));
}

TransportStatus RS485Transport::get_status() const {
    return pImpl_->get_status();
}

} // namespace transport
