#ifndef UART_H
#define UART_H

#include <inttypes.h>
#include "std.h"






#define Uart0Init() { uart0_init_tx(); uart0_init_rx(); }
#define Uart1Init() { uart1_init_tx(); uart1_init_rx(); }

#define Uart0CheckFreeSpace(_x) uart0_check_free_space(_x)
#define Uart1CheckFreeSpace(_x) uart1_check_free_space(_x)

#define Uart0Transmit(_x) uart0_transmit(_x)
#define Uart1Transmit(_x) uart1_transmit(_x)

#define Uart0SendMessage() {}
#define Uart1SendMessage() {}

#define Uart0TxRunning uart0_tx_running
#define Uart0InitParam uart0_init_param

#define Uart1TxRunning uart1_tx_running
#define Uart1InitParam uart1_init_param

#define Uart0TxRunning uart0_tx_running
#define Uart0InitParam uart0_init_param

#endif /* UART_H */
