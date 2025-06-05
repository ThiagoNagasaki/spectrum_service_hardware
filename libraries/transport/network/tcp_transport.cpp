#include "tcp_transport.h"
#include "../enum_/enum_transportstatus.h"
#include "../../../utils/logger.h"                    // Singleton Logger
#include "../../../utils/enum_/enum_commandcontext.h" // CommandContext
#include "../../../utils/enum_/enum_errorcode.h"      // ErrorCode

#include <fmt/core.h>
#include <thread>
#include <unistd.h>     // close()
#include <arpa/inet.h>  // inet_pton, sockaddr_in
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <fcntl.h>      // fcntl()

namespace transport::network {

using transport::enum_::TransportStatus;
using utils::Logger;
using utils::enum_::CommandContext;
using utils::enum_::ErrorCode;

/**
 * \class TCPTransport::Impl
 * \brief Implementação interna do transporte TCP usando PImpl.
 */
class TCPTransport::Impl {
public:
    explicit Impl(const TCPConfig& config)
        : config_(config)
        , sockfd_(-1)
        , status_(TransportStatus::Disconnected)
        , running_(false)
    {

        Logger::instance().debug(CommandContext::NETWORK,
                                 fmt::format("TCPTransport Impl criado para {}:{}", config_.ip, config_.port));
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        if (status_ == TransportStatus::Connected) {
            // Já está conectado
            return true;
        }

        sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPConnectionFailure,
                                     fmt::format("Erro ao criar socket para {}", config_.ip));
            status_ = TransportStatus::Error;
            return false;
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(config_.port);

        if (::inet_pton(AF_INET, config_.ip.c_str(), &server_addr.sin_addr) <= 0) {
            Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPInvalidAddress,
                                     fmt::format("Endereço IP inválido: {}", config_.ip));
            ::close(sockfd_);
            sockfd_ = -1;
            status_ = TransportStatus::Error;
            return false;
        }

        status_ = TransportStatus::Connecting;
        if (::connect(sockfd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPConnectionFailure,
                                     fmt::format("Falha ao conectar em {}:{}", config_.ip, config_.port));
            ::close(sockfd_);
            sockfd_ = -1;
            status_ = TransportStatus::Error;
            return false;
        }

        Logger::instance().info(CommandContext::NETWORK,
                                fmt::format("Conectado em {}:{}", config_.ip, config_.port));
        status_ = TransportStatus::Connected;

        running_ = true;
        read_thread_ = std::thread(&Impl::read_loop, this);
        return true;
    }

    bool disconnect() {
        if (status_ == TransportStatus::Disconnected || status_ == TransportStatus::Error) {
            // Já desconectado ou em erro
            return true;
        }

        running_ = false;
        if (read_thread_.joinable()) {
            read_thread_.join();
        }

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
            ssize_t sent = ::send(sockfd_, data.data() + total_sent,
                                  data.size() - total_sent, 0);
            if (sent <= 0) {
                Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPDataSendFailure,
                                         fmt::format("Falha ao enviar dados para {}", config_.ip));
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

private:
    void read_loop() {
        constexpr size_t BUFFER_SIZE = 1024;
        std::vector<uint8_t> buffer(BUFFER_SIZE);

        while (running_) {
            ssize_t received = ::recv(sockfd_, buffer.data(), buffer.size(), 0);
            if (received > 0) {
                // Temos dados
                if (receive_callback_) {
                    std::vector<uint8_t> data(buffer.begin(), buffer.begin() + received);
                    receive_callback_(data);
                }
            } else if (received == 0) {
                // Conexão fechada pelo servidor
                Logger::instance().warning(CommandContext::NETWORK, ErrorCode::TCPReceiveError,
                                           fmt::format("Conexão encerrada remotamente por {}:{}", config_.ip, config_.port));
                status_ = TransportStatus::Disconnected;
                break;
            } else {
                // Erro ou interrupção
                Logger::instance().error(CommandContext::NETWORK, ErrorCode::TCPReceiveError,
                                         fmt::format("Erro ao receber dados de {}:{}", config_.ip, config_.port));
                status_ = TransportStatus::Error;
                break;
            }
        }

        running_ = false;
        if (sockfd_ != -1) {
            ::close(sockfd_);
            sockfd_ = -1;
        }

        if (status_ != TransportStatus::Error) {
            status_ = TransportStatus::Disconnected;
        }
    }

    TCPConfig config_;
    int sockfd_;
    TransportStatus status_;
    bool running_;
    std::thread read_thread_;

    // Callback para dados recebidos
    std::function<void(const std::vector<uint8_t>&)> receive_callback_;
};

// ==================== Métodos públicos ====================

TCPTransport::TCPTransport(const TCPConfig& config)
    : pImpl_(std::make_unique<Impl>(config))
{
}

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

} // namespace transport::network
 