#ifndef ENUM_TRANSPORT_STATUS_H
#define ENUM_TRANSPORT_STATUS_H

namespace transport::enum_ {
/**
 * @brief Status do transporte.
 */
    enum class TransportStatus { 
        Disconnected, 
        Connecting, 
        Connected, 
        Error };
}
#endif // ENUM_TRANSPORT_STATUS_H



