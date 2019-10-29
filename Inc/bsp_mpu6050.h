/*-------------------------------------------
mpu6050.h

加速计、陀螺仪的相关函数调用
-------------------------------------------*/
#ifndef _MPU6050_H_
#define _MPU6050_H_

#include <stdint.h>

//*************************************
//定义MPU6050内部地址
//*************************************
#define	SMPLRT_DIV			0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG				0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG			0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG		0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H		0x3B
#define	ACCEL_XOUT_L		0x3C
#define	ACCEL_YOUT_H		0x3D
#define	ACCEL_YOUT_L		0x3E
#define	ACCEL_ZOUT_H		0x3F
#define	ACCEL_ZOUT_L		0x40
#define TEMP_OUT_H			0x41
#define TEMP_OUT_L			0x42
#define	GYRO_XOUT_H			0x43
#define	GYRO_XOUT_L			0x44
#define	GYRO_YOUT_H			0x45
#define	GYRO_YOUT_L			0x46
#define	GYRO_ZOUT_H			0x47
#define	GYRO_ZOUT_L			0x48
#define	PWR_MGMT_1			0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SLAVE_ADDRESS		0xD0	//IIC写入时的地址字节数据，+1为读取

#define MPU6050_RA_USER_CTRL        0x6A
#define MPU6050_RA_INT_PIN_CFG      0x37

#define RATE_GYRO			(16.4f * 57.29578f)	// 16.4f取决于GYRO_CONFIG寄存器中FS_SEL的配置
#define RATE_ACCE			(9.8f / 16383.5f)	// 168383.5f取决于ACCEL_CONFIG寄存器中AFS_SEL的配置

//***********************************
//定义类型及变量
//***********************************
void Mpu6050Init(void);					//初始化MPU6050
void ReadImu(void);
uint8_t ImuIsReady(void);
float GetAcceX(void);
float GetAcceY(void);
float GetAcceZ(void);
float GetGyroX(void);
float GetGyroY(void);
float GetGyroZ(void);
float GetTemper(void);

#endif
