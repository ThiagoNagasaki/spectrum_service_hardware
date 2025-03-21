#include "tcp_transport.h"
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>       // close()
#include <arpa/inet.h>    // inet_pton(), sockaddr_in
#include <sys/socket.h>   // socket(), connect()
#include <fcntl.h>        // fcntl()

namespace transport::network {

/**
 * \class TCPTransport::Impl
 * \brief Implementação interna do transporte TCP.
 *
 * Aqui :
 * - Socket
 * - Thread de leitura
 * - Callback de recebimento
 * - Status de conexão
 */
class TCPTransport::Impl {
public:
    Impl(const std::string& ip, uint16_t port)
        : ip_(ip)
        , port_(port)
        , sockfd_(-1)
        , status_(TransportStatus::Disconnected)
        , running_(false)
    {
    }

    ~Impl() {
        disconnect();
    }

    bool connect() {
        if (status_ == TransportStatus::Connected) {
            return true; // Já conectado
        }

        // Cria socket TCP (IPv4)
        sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            std::cerr << "[TCPTransport] Erro ao criar socket.\n";
            status_ = TransportStatus::Error;
            return false;
        }

        // Preenche struct de endereço
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_);

        if (::inet_pton(AF_INET, ip_.c_str(), &server_addr.sin_addr) <= 0) {
            std::cerr << "[TCPTransport] IP inválido ou não suportado: " << ip_ << "\n";
            ::close(sockfd_);
            sockfd_ = -1;
            status_ = TransportStatus::Error;
            return false;
        }

        // Tenta conectar
        status_ = TransportStatus::Connecting;
        if (::connect(sockfd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            std::cerr << "[TCPTransport] Falha ao conectar em " << ip_ << ":" << port_ << "\n";
            ::close(sockfd_);
            sockfd_ = -1;
            status_ = TransportStatus::Error;
            return false;
        }

        std::cout << "[TCPTransport] Conectado em " << ip_ << ":" << port_ << "\n";
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
        if (read_thread_.joinable()) {
            read_thread_.join();
        }

        if (sockfd_ != -1) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
        status_ = TransportStatus::Disconnected;
        return true;
    }

    bool send(const std::vector<uint8_t>& data) {
        if (status_ != TransportStatus::Connected || sockfd_ < 0) {
            std::cerr << "[TCPTransport] Socket não está conectado.\n";
            return false;
        }
        // Envia todos os bytes
        ssize_t total_sent = 0;
        while (total_sent < static_cast<ssize_t>(data.size())) {
            ssize_t sent = ::send(sockfd_, data.data() + total_sent, data.size() - total_sent, 0);
            if (sent <= 0) {
                std::cerr << "[TCPTransport] Falha no envio.\n";
                return false;
            }
            total_sent += sent;
        }
        return true;
    }

    void set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) {
        receive_callback_ = std::move(callback);
    }

    TransportStatus get_status() const {
        return status_;
    }

private:
    /**
     * \brief Loop de leitura: aguarda dados e chama callback.
     */
    void read_loop() {
        constexpr size_t BUFFER_SIZE = 1024;
        std::vector<uint8_t> buffer(BUFFER_SIZE);

        while (running_) {
            ssize_t received = ::recv(sockfd_, buffer.data(), buffer.size(), 0);
            if (received > 0) {
                // Chama callback se existir
                if (receive_callback_) {
                    // Redimensiona para a quantidade exata recebida
                    std::vector<uint8_t> data(buffer.begin(), buffer.begin() + received);
                    receive_callback_(data);
                }
            } else if (received == 0) {
                std::cerr << "[TCPTransport] Conexão fechada remotamente.\n";
                status_ = TransportStatus::Disconnected;
                break;
            } else {
                std::cerr << "[TCPTransport] Erro na recepção de dados.\n";
                status_ = TransportStatus::Error;
                break;
            }
        }

        // Se sair do loop, força desconexão
        running_ = false;
        if (sockfd_ != -1) {
            ::close(sockfd_);
            sockfd_ = -1;
        }
        if (status_ != TransportStatus::Error) {
            status_ = TransportStatus::Disconnected;
        }
    }

    std::string ip_;
    uint16_t port_;
    int sockfd_;
    TransportStatus status_;

    std::function<void(const std::vector<uint8_t>&)> receive_callback_;

    bool running_;
    std::thread read_thread_;
};


TCPTransport::TCPTransport(const std::string& ip, uint16_t port)
    : pImpl_(std::make_unique<Impl>(ip, port))
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

void TCPTransport::set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) {
    pImpl_->set_receive_callback(std::move(callback));
}

TransportStatus TCPTransport::get_status() const {
    return pImpl_->get_status();
}

} // namespace transport::network
