#include "app_test.h"

#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

#include "bsp_uart.h"

uint8_t test_buff[16];

void TestTask(void const *arg)
{
	uint32_t len = 0;

	while(1)
	{
		// len = KfifoGet(&uart2rx_fifo, test_buff, 16);
		
		// if (len)
		// {
		// 	DBG_INFO("##TEST:%s\r\n", test_buff);
		// }
		
		osDelay(1000);
	}
}

void TestTaskInit(void)
{
	static osThreadId test_task_handle;

	osThreadDef(test_task, TestTask, osPriorityNormal, 0, 128);
	test_task_handle = osThreadCreate(osThread(test_task), NULL);
}
