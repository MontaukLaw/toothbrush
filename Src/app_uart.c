#include "app_uart.h"

#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

#include "bsp_uart.h"
#include "lib_frameseparate.h"

static uint8_t brush_start_flag = 0;
static DataFrameStruct control_frame = {0};

uint8_t BrushIsStart(void)
{
	return brush_start_flag;
}

void ControlCmd(uint8_t *str, uint32_t str_len)
{
	char *p = str;
	
	if (str[str_len - 1] == '\n' && str[str_len - 2] == '\r')
	{
		str[str_len - 2] = 0;

		if (strncmp(p, "ys+start", 8) == 0)
		{
			brush_start_flag = 1;
		}
		else if (strncmp(p, "ys+stop", 8) == 0)
		{
			brush_start_flag = 0;
		}
		else
		{
			DBG_INFO("##cmd error:%s\r\n", str);
		}
	}
}

void UartTask(void const *arg)
{
	uint32_t len = 0;
	uint8_t rec_buff[64];

	FrameStructInit(&control_frame, 'y', 's', '\r', '\n', 64);

	while(1)
	{
		len = KfifoGet(&uart2rx_fifo, rec_buff, 64);
		
		if (len)
		{
			// DBG_INFO("##TEST:%s\r\n", rec_buff);
			DataFrameDecode(rec_buff, len, &control_frame, ControlCmd);
		}
		
		osDelay(10);
	}

	FrameStructDelete(&control_frame);
}

void UartTaskInit(void)
{
	static osThreadId uart_task_handle;

	osThreadDef(uart_task, UartTask, osPriorityNormal, 0, 256);
	uart_task_handle = osThreadCreate(osThread(uart_task), NULL);
}

