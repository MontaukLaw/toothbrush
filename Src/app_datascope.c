#include "app_datascope.h"
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

#include <math.h>

#include "bsp_mpu6050.h"
#include "MadgwickAHRS.h"
// #include "MahonyAHRS.h"
// #include "lib_ahrs.h"
#include "DataScope_DP.h"


void DataScopeTask(void const *arg)
{
	unsigned char i;          //计数变量
	unsigned char send_count; //串口需要发送的数据个数

	float acce_x = 0;
	float acce_y = 0;
	float acce_z = 0;
	float gyro_x = 0;
	float gyro_y = 0;
	float gyro_z = 0;
	float temper = 0;

	float roll = 0;
	float pitch = 0;
	float yaw = 0;

	float mag_x = 0;
	float mag_y = 0;
	float mag_z = 0;

	while(1)
	{
		acce_x = GetAcceX();// * 9.18f;
		acce_y = GetAcceY();// * 9.18f;
		acce_z = GetAcceZ();// * 9.18f;
		gyro_x = GetGyroX();
		gyro_y = GetGyroY();
		gyro_z = GetGyroZ();
		temper = GetTemper();

		roll = GetRoll();
		pitch = GetPitch();
		yaw = GetYaw();

		mag_x = GetMagX();
		mag_y = GetMagY();
		mag_z = GetMagZ();

		// roll = getRoll();
		// pitch = getPitch();
		// yaw = getYaw();

		// LOG("acce_x:%.2f\t acce_y:%.2f\t acce_z:%.2f\t ", acce_x, acce_y, acce_z);
		// LOG("gyro_x:%.2f\t gyro_y:%.2f\t gyro_z:%.2f\t\n", gyro_x, gyro_y, gyro_z);
		// LOG("roll:%.2f\t pitch:%.2f\t yaw:%.2f\t\n", roll, pitch, yaw);

		DataScope_Get_Channel_Data(pitch, 1);
		DataScope_Get_Channel_Data(roll, 2);
		DataScope_Get_Channel_Data(yaw, 3);
		DataScope_Get_Channel_Data(acce_x, 4);  //将数据写入通道 1
		DataScope_Get_Channel_Data(acce_y, 5);  //将数据写入通道 2
		DataScope_Get_Channel_Data(acce_z, 6);  //将数据写入通道 3
		DataScope_Get_Channel_Data(mag_x, 7);
		DataScope_Get_Channel_Data(mag_y, 8);
		DataScope_Get_Channel_Data(mag_z, 9);
		DataScope_Get_Channel_Data(temper, 10);

		send_count = DataScope_Data_Generate(10); //生成10个通道的 格式化帧数据，返回帧数据长度

		for( i = 0 ; i < send_count; i++)  //循环发送,直到发送完毕
		{
			HAL_UART_Transmit(&huart2, (uint8_t *)&DataScope_OutPut_Buffer[i], 1, 0xFFFF);
		}

		osDelay(10);
	}
}

void DataScopeTaskInit(void)
{
	static osThreadId dp_task_handle;

	osThreadDef(dp_task, DataScopeTask, osPriorityNormal, 0, 256);
	dp_task_handle = osThreadCreate(osThread(dp_task), NULL);
}
