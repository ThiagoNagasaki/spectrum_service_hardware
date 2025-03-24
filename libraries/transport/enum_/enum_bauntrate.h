#ifndef BAUD_RATE_H
#define BAUD_RATE_H

namespace transport::enum_ {

/**
 * \enum BaudRate
 * \brief Enum que representa as velocidades de comunicação suportadas.
 */
enum class BaudRate {
    B4800,
    B9600,
    B19200,
    B38400,
    B57600,
    B115200
};

} // namespace transport::enum_

#endif // BAUD_RATE_H
