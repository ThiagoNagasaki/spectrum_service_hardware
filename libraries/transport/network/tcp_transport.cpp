#include "tcp_transport.h"
#include "../enum_/enum_transportstatus.h"
#include "../../utils/logger/logger.h"
#include "../../utils/enum_/mcb_port_addresses.h"
#include "../../utils/enum_/enum_commandcontext.h"
#include "../../utils/enum_/enum_errorcode.h"

#include <fmt/core.h>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>

namespace transport::network {

using transport::enum_::TransportStatus;
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

class TCPTransport::Impl {
public:
    explicit Impl(const TCPConfig& config)
        : config_(config), sockfd_(-1),
          status_(TransportStatus::Disconnected), running_(false)
    {
        Logger::instance().debug(CommandContext::NETWORK,
            fmt::format("TCPTransport Impl criado para {}:{}", config_.ip, config_.port));
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        if (status_ == TransportStatus::Connected) return true;
    std::cout << "Conectando MCB em " << config_.ip << ":" << config_.port << "...\n";
        std::cout << "chegou em tcp transporte esta tentando conectart \n";
        sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPConnectionFailure,
                fmt::format("Erro ao criar socket {}", config_.ip));
            status_ = TransportStatus::Error;
                    std::cout << "ERRO LV 1 \n";
            return false;
        }

        sockaddr_in srv{};
        srv.sin_family = AF_INET;
        srv.sin_port   = htons(config_.port);
        if (::inet_pton(AF_INET, config_.ip.c_str(), &srv.sin_addr) <= 0) {
            Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPInvalidAddress,
                fmt::format("IP inválido {}", config_.ip));
            ::close(sockfd_); sockfd_ = -1;
            status_ = TransportStatus::Error;
                    std::cout << "ERRO LV 2 \n";
            return false;
        }

        status_ = TransportStatus::Connecting;
        if (::connect(sockfd_, (sockaddr*)&srv, sizeof(srv)) < 0) {
            Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPConnectionFailure,
                fmt::format("Falha conectar {}:{}", config_.ip, config_.port));
            ::close(sockfd_); sockfd_ = -1;
            status_ = TransportStatus::Error;
                    std::cout << "ERRO LV 3 \n";
            return false;
        }

        Logger::instance().info(CommandContext::NETWORK,
            fmt::format("Conectado {}:{}", config_.ip, config_.port));
        status_ = TransportStatus::Connected;
        running_ = true;
        read_thread_ = std::thread(&Impl::read_loop, this);
        return true;
    }

    bool disconnect() {
        if (status_ == TransportStatus::Disconnected || status_ == TransportStatus::Error) {
            return true;
        }

        running_ = false;
        if (read_thread_.joinable()) read_thread_.join();

        if (sockfd_ != -1) {
            ::close(sockfd_);
            sockfd_ = -1;
        }

        Logger::instance().info(CommandContext::NETWORK,
            fmt::format("Desconectado de {}:{}", config_.ip, config_.port));
        status_ = TransportStatus::Disconnected;
        return true;
    }

    bool send(const std::vector<uint8_t>& data) {
        if (status_ != TransportStatus::Connected || sockfd_ < 0) {
            Logger::instance().warning(CommandContext::NETWORK, ErrorCode::TCPConnectionFailure,
                fmt::format("Tentativa de envio sem conexão ativa em {}", config_.ip));
            return false;
        }

        ssize_t total_sent = 0;
        while (total_sent < static_cast<ssize_t>(data.size())) {
            ssize_t sent = ::send(sockfd_, data.data() + total_sent, data.size() - total_sent, 0);
            if (sent <= 0) {
                logError(ErrorCode::TCPDataSendFailure, "Falha ao enviar dados");
                return false;
            }
            total_sent += sent;
        }

        Logger::instance().debug(CommandContext::NETWORK,
            fmt::format("Enviados {} bytes para {}", total_sent, config_.ip));
        return true;
    }

    void subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
        receive_callback_ = std::move(callback);
    }

    TransportStatus get_status() const {
        return status_;
    }
    std::vector<uint8_t> receive() {
    constexpr uint8_t ETX = static_cast<uint8_t>(utils::enum_::ETX);
    std::vector<uint8_t> buffer;
    buffer.reserve(256); // tamanho médio de frame

    uint8_t byte = 0;
    while (true) {
        ssize_t bytesRead = ::recv(sockfd_, &byte, 1, 0);
        if (bytesRead <= 0) {
            throw std::runtime_error("TCPTransport::receive: erro ao receber byte ou conexão fechada");
        }

        buffer.push_back(byte);
        if (byte == ETX) {
            break; // fim de frame MCB
        }
    }

    return buffer;
}
    

private:
    void read_loop() {
        constexpr size_t BUFFER_SIZE = 1024;
        std::vector<uint8_t> buffer(BUFFER_SIZE);

        while (running_) {
            ssize_t received = ::recv(sockfd_, buffer.data(), buffer.size(), 0);
            if (received > 0) {
                if (receive_callback_) {
                    std::vector<uint8_t> data(buffer.begin(), buffer.begin() + received);
                    receive_callback_(data);
                }
            } else if (received == 0) {
                logWarning(ErrorCode::TCPReceiveError, "Conexão encerrada remotamente");
                status_ = TransportStatus::Disconnected;
                break;
            } else {
                logError(ErrorCode::TCPReceiveError, "Erro ao receber dados");
                status_ = TransportStatus::Error;
                break;
            }
        }

        running_ = false;
        if (sockfd_ != -1) {
            ::close(sockfd_);
            sockfd_ = -1;
        }

        if (status_ != TransportStatus::Error)
            status_ = TransportStatus::Disconnected;
    }

    void logError(ErrorCode code, const std::string& message) {
        Logger::instance().error(CommandContext::NETWORK, code,
            fmt::format("{} ({}:{})", message, config_.ip, config_.port));
    }

    void logWarning(ErrorCode code, const std::string& message) {
        Logger::instance().warning(CommandContext::NETWORK, code,
            fmt::format("{} ({}:{})", message, config_.ip, config_.port));
    }

    TCPConfig config_;
    int sockfd_;
    TransportStatus status_;
    bool running_;
    std::thread read_thread_;
    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

// ==================== Interface pública ====================

TCPTransport::TCPTransport(const TCPConfig& config)
    : pImpl_(std::make_unique<Impl>(config)) {}

TCPTransport::~TCPTransport() = default;

bool TCPTransport::connect() {
    return pImpl_->connect();
}

bool TCPTransport::disconnect() {
    return pImpl_->disconnect();
}

bool TCPTransport::send(const std::vector<uint8_t>& data) {
    return pImpl_->send(data);
}

void TCPTransport::subscribe(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->subscribe(std::move(callback));
}

TransportStatus TCPTransport::get_status() const {
    return pImpl_->get_status();
    
}

std::vector<uint8_t> TCPTransport::receive() {
    return pImpl_->receive();  
}

} // namespace transport::network
