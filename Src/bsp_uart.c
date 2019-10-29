#include "bsp_uart.h"

#include <string.h>
#include <stdarg.h>

#include "cmsis_os.h"



uint8_t ch = 0;

uint8_t send_buff[512];

Kfifo uart1rx_fifo;
Kfifo uart2rx_fifo;
Kfifo uart1tx_fifo;
Kfifo uart2tx_fifo;

static osMutexId uart1_lock;
static osMutexId uart2_lock;

static unsigned char UART1_RX_BUFFER[UART1_RX_BUFFER_SIZE] = {0};
static unsigned char UART2_RX_BUFFER[UART2_RX_BUFFER_SIZE] = {0};
static unsigned char UART1_TX_BUFFER[UART1_TX_BUFFER_SIZE] = {0};
static unsigned char UART2_TX_BUFFER[UART2_TX_BUFFER_SIZE] = {0};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t rd = 0;

	if (huart == &huart1)
	{
		rd = (uint8_t)(huart1.Instance->DR);
		KfifoPut(&uart1rx_fifo, &rd, 1);
	}
	if (huart == &huart2)
	{
		rd = (uint8_t)(huart2.Instance->DR);
		KfifoPut(&uart2rx_fifo, &rd, 1);
	}

	HAL_UART_Receive_IT(huart, &ch, 1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != RESET)
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
	}

	HAL_UART_Receive_IT(huart, &ch, 1);
}

void UartSendData(UART_HandleTypeDef *huart, uint8_t *data, uint32_t len)
{
	HAL_UART_Transmit_DMA(huart, data, len);
}

void UartPrintf(UART_HandleTypeDef *huart, uint8_t level, char *format, ...)
{
	char *buff = NULL;
	va_list args;
	
	if (level > 0 && level <= DBG_LEVEL)
	{
		buff = (char *)pvPortMalloc(256);
        //buff = (uint8_t *)malloc(256);

		va_start(args, format);
		vsnprintf(buff, 128, format, args);
		va_end(args);

		//UartSendData(huart, buff, strlen(buff));
		// osMutexWait(uart2_lock, osWaitForever);
		// KfifoPut(&uart2tx_fifo, (uint8_t *)buff, strlen(buff));
		HAL_UART_Transmit(&huart1, (uint8_t *)buff, strlen(buff), 0xFFFF);
		// HAL_UART_Transmit_DMA(huart, (uint8_t *)buff, strlen(buff));
		// UartSendData(huart, (uint8_t *)buff, strlen(buff));
		// osMutexRelease(uart2_lock);

		vPortFree(buff);
	}
}

void UartTransmitTask(void const *arg)
{
	uint32_t len = 0;

	while(1)
	{
		len = KfifoGet(&uart2tx_fifo, send_buff, 512);

		if(len)
		{
			UartSendData(&huart1, send_buff, len);
		}

		osDelay(2);
	}
}

void DBGInit()
{
	static osThreadId uart_task_handle;

	osThreadDef(uart_task, UartTransmitTask, osPriorityNormal, 0, 128);
	uart_task_handle = osThreadCreate(osThread(uart_task), NULL);

	// osMutexDef(uart1);
	// osMutexDef(uart2);
	// uart1_lock = osMutexCreate(osMutex(uart1));
	// uart2_lock = osMutexCreate(osMutex(uart2));

	// HAL_UART_Receive_IT(&huart1,(uint8_t *)&ch,1);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&ch, 1);
	KfifoInit(&uart1rx_fifo, UART1_RX_BUFFER, UART1_RX_BUFFER_SIZE);
	KfifoInit(&uart2rx_fifo, UART2_RX_BUFFER, UART2_RX_BUFFER_SIZE);
	KfifoInit(&uart1tx_fifo, UART1_TX_BUFFER, UART1_TX_BUFFER_SIZE);
	KfifoInit(&uart2tx_fifo, UART2_TX_BUFFER, UART2_TX_BUFFER_SIZE);
}
