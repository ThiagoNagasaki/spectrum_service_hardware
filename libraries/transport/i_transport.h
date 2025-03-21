#ifndef I_TRANSPORT_H
#define I_TRANSPORT_H

#include <string>
#include <functional>
#include <vector>
#include <iostream> 
#include <cstdint>

namespace transport {

/**
 * @brief Status do transporte.
 */
enum class TransportStatus {
    Disconnected,
    Connecting,
    Connected,
    Error
};

/**
 * @brief Interface base para todos os transportes de comunicação.
 */
class ITransport {
public:
    virtual ~ITransport() = default;

    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool send(const std::vector<uint8_t>& data) = 0;
    virtual void set_receive_callback(std::function<void(const std::vector<uint8_t>&)> callback) = 0;
    virtual TransportStatus get_status() const = 0;
};

} // namespace transport

#endif // I_TRANSPORT_H
