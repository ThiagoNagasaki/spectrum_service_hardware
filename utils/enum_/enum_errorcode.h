#ifndef ENUM_ERROR_CODE_H
#define ENUM_ERROR_CODE_H

namespace utils::enum_ {

/**
 * @brief Enum que representa códigos de erro padronizados no sistema.
 */
enum class ErrorCode {
    GeneratorConnectionError,
    TCPConnectionFailure,
    TCPDataSendFailure,
    TCPInvalidAddress,
    TCPReceiveError,
    RS232ConnectionFailure,
    RS232ConfigurationFailure,
    RS232DataSendFailure,
    RS485ConnectionFailure,
    RS485ConfigurationFailure,
    RS485DataSendFailure,
    ProtocolError,
    

};

} // namespace utils::enum_

#endif // ENUM_ERROR_CODE_H
