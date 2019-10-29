#include "app_oralarea.h"

#include <stdio.h>
#include "bsp_uart.h"
#include "main.h"
#include "cmsis_os.h"

#include "MadgwickAHRS.h"
#include "lib_oralarea.h"
#include "app_uart.h"

void ShowAreaInfo(uint8_t area)
{
	// 区域			对应数据编号
	// 左上内		0
	// 左上外		1
	// 左上咬合面	2
	// 左下内		3
	// 左下外		4
	// 左下咬合面	5
	// 中上内		6
	// 中上外		7
	// 中下内		8
	// 中下外		9
	// 右上内		10
	// 右上外		11
	// 右上咬合面	12
	// 右下内		13
	// 右下外		14
	// 右下咬合面	15
	switch (area)
	{
	case 0:
		DBG_INFO("%d, 当前刷牙区域为： 左上内\r\n", area);
		break;
	case 1:
		DBG_INFO("%d, 当前刷牙区域为： 左上外\r\n", area);
		break;
	case 2:
		DBG_INFO("%d, 当前刷牙区域为： 左上咬合面\r\n", area);
		break;
	case 3:
		DBG_INFO("%d, 当前刷牙区域为： 左下内\r\n", area);
		break;
	case 4:
		DBG_INFO("%d, 当前刷牙区域为： 左下外\r\n", area);
		break;
	case 5:
		DBG_INFO("%d, 当前刷牙区域为： 左下咬合面\r\n", area);
		break;
	case 6:
		DBG_INFO("%d, 当前刷牙区域为： 中上内\r\n", area);
		break;
	case 7:
		DBG_INFO("%d, 当前刷牙区域为： 中上外\r\n", area);
		break;
	case 8:
		DBG_INFO("%d, 当前刷牙区域为： 中下内\r\n", area);
		break;
	case 9:
		DBG_INFO("%d, 当前刷牙区域为： 中下外\r\n", area);
		break;
	case 10:
		DBG_INFO("%d, 当前刷牙区域为： 右上内\r\n", area);
		break;
	case 11:
		DBG_INFO("%d, 当前刷牙区域为： 右上外\r\n", area);
		break;
	case 12:
		DBG_INFO("%d, 当前刷牙区域为： 右上咬合面\r\n", area);
		break;
	case 13:
		DBG_INFO("%d, 当前刷牙区域为： 右下内\r\n", area);
		break;
	case 14:
		DBG_INFO("%d, 当前刷牙区域为： 右下外\r\n", area);
		break;
	case 15:
		DBG_INFO("%d, 当前刷牙区域为： 右下咬合面\r\n", area);
		break;
	default:
		DBG_INFO("run line:%d\r\n", __LINE__);
		DBG_INFO("%d, 未定义区域\r\n", area);
		break;
	}
}

void OralTask(void const *arg)
{
	uint8_t start_statu = 0;
	uint8_t area = 16;
	float roll = 0;
	float pitch = 0;
	float yaw = 0;
	float temper = 0;

	osDelay(3000);

	while(1)
	{
		roll = GetRoll();
		pitch = GetPitch();
		yaw = GetYaw();
		
		//GetAllAreaRangeByRightHand(roll, pitch, yaw);
		if(BrushIsStart())
		{
			switch(start_statu)
			{
			case 0:	// just start now
				start_statu = 1;
				GetAllAreaRangeByRightHand(roll, pitch, yaw);
				break;
			case 1:
				area = JudgeOralArea(roll, pitch, yaw, 10);
				ShowAreaInfo(area);
				area = 16;
				break;
			default:
				break;
			}
		}
		else
		{
			start_statu = 0;
		}

		DBG_INFO("roll: %.2f, pitch: %.2f, yaw: %.2f\r\n", roll, pitch, yaw);

		osDelay(500);
	}
}

void OralTaskInit(void)
{
	static osThreadId oral_task_handle;

	osThreadDef(oral_task, OralTask, osPriorityNormal, 0, 128);
	oral_task_handle = osThreadCreate(osThread(oral_task), NULL);
}
