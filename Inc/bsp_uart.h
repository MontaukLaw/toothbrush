#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include <stdint.h>
#include "lib_fifo.h"
#include "stm32f1xx_hal.h"

#include "stm32f1xx_hal_uart.h"

#include "main.h"

#define UART1_RX_BUFFER_SIZE	64
#define UART2_RX_BUFFER_SIZE	64
#define UART1_TX_BUFFER_SIZE	64
#define UART2_TX_BUFFER_SIZE	4096

#define DBG_LEVEL  6


extern Kfifo uart1rx_fifo;
extern Kfifo uart2rx_fifo;
extern Kfifo uart1tx_fifo;
extern Kfifo uart2tx_fifo;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void DBGInit();
void UartSendData(UART_HandleTypeDef *huart, uint8_t *data, uint32_t len);
void UartPrintf(UART_HandleTypeDef *huart, uint8_t level, char *format, ...);


#if DBG_LEVEL > 0

#define DBG_LEVEL_ERR           1
#define DBG_LEVEL_WAR           2
#define DBG_LEVEL_LOG           3
#define DBG_LEVEL_INFO          4
#define DBG_LEVEL_DEBUG         5
#define DBG_LEVEL_TEMP          6

#define DBG_ERR(format, ...)    UartPrintf(&huart1, DBG_LEVEL_ERR,   "error> file:%s line:%d "format"\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define DBG_WAR(format, ...)    UartPrintf(&huart1, DBG_LEVEL_WAR,   "warring> "format"\r\n", ##__VA_ARGS__)
#define DBG_LOG(format, ...)    UartPrintf(&huart1, DBG_LEVEL_LOG,   "log> "format"\r\n", ##__VA_ARGS__)
#define DBG_INFO(format, ...)   UartPrintf(&huart1, DBG_LEVEL_INFO,  "info> "format"\r\n", ##__VA_ARGS__)
#define DBG_DBG(format, ...)    UartPrintf(&huart1, DBG_LEVEL_DEBUG, "dbg> "format"\r\n", ##__VA_ARGS__)
#define DBG_TEMP(format, ...)   UartPrintf(&huart1, DBG_LEVEL_TEMP,  "temp> "format"\r\n", ##__VA_ARGS__)

#else

#define DBG_ERR(format, ...)
#define DBG_WAR(format, ...)
#define DBG_LOG(format, ...)
#define DBG_INFO(format, ...)
#define DBG_DBG(format, ...)
#define DBG_TEMP(format, ...)

#endif

#endif
