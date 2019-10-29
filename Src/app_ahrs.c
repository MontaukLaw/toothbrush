#include "app_ahrs.h"

#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"

#include "bsp_mpu6050.h"
#include "MadgwickAHRS.h"

void AhrsTask(void const *arg)
{
	osDelay(1000);
	
	while(1)
	{
		// MadgwickAHRSupdateIMU(GetGyroX(), GetGyroY(), GetGyroZ(), GetAcceX(), GetAcceY(), GetAcceZ());
		MadgwickAHRSupdate(GetGyroX(), GetGyroY(), GetGyroZ(), GetAcceX(), GetAcceY(), GetAcceZ(), GetMagX(), GetMagY(), GetMagZ());
		QuaternionToEulerAngle();

		osDelay(5);
	}
}

void AhrsTaskInit(void)
{
	static osThreadId ahrs_task_handle;

	osThreadDef(ahrs_task, AhrsTask, osPriorityNormal, 0, 128);
	ahrs_task_handle = osThreadCreate(osThread(ahrs_task), NULL);
}
