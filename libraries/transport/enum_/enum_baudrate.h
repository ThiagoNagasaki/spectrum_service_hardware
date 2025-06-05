#ifndef BAUD_RATE_H
#define BAUD_RATE_H

namespace transport::enum_ {

/**
 * \enum BaudRate
 * \brief Enum que representa as velocidades de comunicação suportadas.
 */
enum class BaudRate {
    BR_4800,
    BR_9600,
    BR_19200,
    BR_38400,
    BR_57600,
    BR_115200
};

} // namespace transport::enum_

#endif // BAUD_RATE_H
 