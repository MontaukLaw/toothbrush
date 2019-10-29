#include "user_com.h"

//extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

static uint8_t ready = 0;
static float temper = 0;
static float acce_x = 0;
static float acce_y = 0;
static float acce_z = 0;
static float gyro_x = 0;
static float gyro_y = 0;
static float gyro_z = 0;

static float acce_offset_x = 210;	// 200;
static float acce_offset_y = 1760;	// 460;
static float acce_offset_z = -3170;	// 5960;
static float gyro_offset_x = -22;	// -77;
static float gyro_offset_y = 10;		// -47;
static float gyro_offset_z = 16;		// -38;

static void ImuWrite(uint16_t RegAddress, uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, SLAVE_ADDRESS, RegAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);
}

static uint8_t ImuRead(uint8_t RegAddress)
{
	uint8_t data;

	HAL_I2C_Mem_Read(&hi2c1, SLAVE_ADDRESS + 1, RegAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);

	return data;
}

#define MPU6050_USERCTRL_I2C_MST_EN_BIT         5
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT    	1

/**************************实现函数********************************************
*函数原型:		void MPU6050_setI2CMasterModeEnabled(uint8_t enabled)
*功　　能:	    设置 MPU6050 是否为AUX I2C线的主机
*******************************************************************************/
void MPU6050_setI2CMasterModeEnabled(uint8_t enabled) 
{
	uint8_t byte = 0;
	byte = ImuRead(MPU6050_RA_USER_CTRL);
	byte = (enabled != 0) ? (byte | (1 << 5)) : (byte & ~(1 << 5));
	ImuWrite(MPU6050_RA_USER_CTRL, byte);
    // IICwriteBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setI2CBypassEnabled(uint8_t enabled)
*功　　能:	    设置 MPU6050 是否为AUX I2C线的主机
*******************************************************************************/
void MPU6050_setI2CBypassEnabled(uint8_t enabled) 
{
	uint8_t byte = 0;
	byte = ImuRead(MPU6050_RA_INT_PIN_CFG);
	byte = (enabled != 0) ? (byte | (1 << 1)) : (byte & ~(1 << 1));
	ImuWrite(MPU6050_RA_INT_PIN_CFG, byte);
    // IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

static int16_t GetData(uint8_t RegAddress)		//传感器现值采集
{
	uint8_t H, L;

	H = ImuRead(RegAddress);
	L = ImuRead(RegAddress + 1);

	return (H << 8) + L;				//合成数据
}

float GetAcceX(void)
{
	return acce_x;
	// return acce_y;
}

float GetAcceY(void)
{
	return acce_y;
	// return -acce_x;
}

float GetAcceZ(void)
{
	return acce_z;
}

float GetGyroX(void)
{
	return gyro_x;
	// return gyro_y;
}

float GetGyroY(void)
{
	return gyro_y;
	// return -gyro_x;
}

float GetGyroZ(void)
{
	return gyro_z;
}

float GetTemper(void)
{
	return temper;
}

uint8_t ImuIsReady(void)
{
	return ready;
}

#define FILTER_NUM 10
void AvergeFilter(float accx, float accy, float accz, float *accoutx, float *accouty, float *accoutz)
{
	static uint8_t  filter_cnt = 0;
	static float  ACC_X_BUF[FILTER_NUM], ACC_Y_BUF[FILTER_NUM], ACC_Z_BUF[FILTER_NUM];
	int32_t temp1 = 0, temp2 = 0, temp3 = 0;
	uint8_t i;

	ACC_X_BUF[filter_cnt] = accx;
	ACC_Y_BUF[filter_cnt] = accy;
	ACC_Z_BUF[filter_cnt] = accz;

	for(i = 0; i < FILTER_NUM; i++) //滑动平滑滤波
	{
		temp1 += ACC_X_BUF[i];
		temp2 += ACC_Y_BUF[i];
		temp3 += ACC_Z_BUF[i];
	}
	*accoutx = temp1 / FILTER_NUM;
	*accouty = temp2 / FILTER_NUM;
	*accoutz = temp3 / FILTER_NUM;
	filter_cnt++;
	if(filter_cnt == FILTER_NUM)    filter_cnt = 0;

}

void AcceOffset(void)
{
	int16_t cnt = 0;
	int32_t temp_x = 0;
	int32_t temp_y = 0;
	int32_t temp_z = 0;

	for (cnt = 0; cnt < 100; cnt++)
	{
		temp_x += GetData(ACCEL_XOUT_H);
		temp_y += GetData(ACCEL_YOUT_H);
		temp_z += GetData(ACCEL_ZOUT_H);
	}

	acce_offset_x = 1.0f * temp_x / cnt;
	acce_offset_y = 1.0f * temp_y / cnt;
	acce_offset_z = 1.0f * temp_z / cnt - 16383.5;
}

void GyroOffset(void)
{
	int16_t cnt = 0;
	int32_t temp_x = 0;
	int32_t temp_y = 0;
	int32_t temp_z = 0;

	for (cnt = 0; cnt < 100; cnt++)
	{
		temp_x += GetData(GYRO_XOUT_H);
		temp_y += GetData(GYRO_YOUT_H);
		temp_z += GetData(GYRO_ZOUT_H);
	}

	gyro_offset_x = 1.0f * temp_x / cnt;
	gyro_offset_y = 1.0f * temp_y / cnt;
	gyro_offset_z = 1.0f * temp_z / cnt;
}

void ReadImu(void)
{
	acce_x = (GetData(ACCEL_XOUT_H) - acce_offset_x) * RATE_ACCE;
	acce_y = (GetData(ACCEL_YOUT_H) - acce_offset_y) * RATE_ACCE;
	acce_z = (GetData(ACCEL_ZOUT_H) - acce_offset_z) * RATE_ACCE;

	// acce_x = (GetData(ACCEL_XOUT_H) - 357) * RATE_ACCE;
	// acce_y = (GetData(ACCEL_YOUT_H) - 267) * RATE_ACCE;
	// acce_z = (GetData(ACCEL_ZOUT_H) - 5435) * RATE_ACCE;
	gyro_x = (GetData(GYRO_XOUT_H) - gyro_offset_x) / RATE_GYRO;	// offset 67.5
	gyro_y = (GetData(GYRO_YOUT_H) - gyro_offset_y) / RATE_GYRO;	// 57.5
	gyro_z = (GetData(GYRO_ZOUT_H) - gyro_offset_z) / RATE_GYRO;	// 37.5
	temper = GetData(TEMP_OUT_H) / 340.0f + 36.53;
}

void ImuTask(void const *arg)
{
	osDelay(10);					// wait for power stable
	ImuWrite(PWR_MGMT_1, 0x00);		// 解除休眠状态
	ImuWrite(SMPLRT_DIV, 0x07);
	ImuWrite(CONFIG, 0x06);
	ImuWrite(GYRO_CONFIG, 0x18);
	ImuWrite(ACCEL_CONFIG, 0x01);

	MPU6050_setI2CMasterModeEnabled(0);	 //不让MPU6050 控制AUXI2C
	MPU6050_setI2CBypassEnabled(1);	 //主控制器的I2C与	MPU6050的AUXI2C	直通。控制器可以直接访问HMC5883L
	osDelay(1);
    
	InitHMC5883();
	
	osDelay(200);					// wait for mpu6050 stable
	// AcceOffset();
	GyroOffset();
    
	// DBG_INFO("%.2f,%.2f,%.2f\r\n", acce_offset_x, acce_offset_y, acce_offset_z);
	DBG_INFO("%.2f,%.2f,%.2f\r\n", gyro_offset_x, gyro_offset_y, gyro_offset_z);
	DBG_INFO("teethbrush is ready!!!!\r\n");
	ready = 1;

	while(1)
	{
		ReadImu();
		updateHMC5883();

		osDelay(1);
	}
}

void Mpu6050Init(void)
{
	static osThreadId imu_task_handle;

	// osDelay(1000);
	// ImuWrite(PWR_MGMT_1, 0x00);			//解除休眠状态
	// ImuWrite(SMPLRT_DIV, 0x07);
	// ImuWrite(CONFIG, 0x06);
	// ImuWrite(GYRO_CONFIG, 0x18);
	// ImuWrite(ACCEL_CONFIG, 0x01);
	// ready = 1;

	osThreadDef(imu_task, ImuTask, osPriorityNormal, 0, 128);
	imu_task_handle = osThreadCreate(osThread(imu_task), NULL);
}
