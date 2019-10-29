#include "user_com.h"

#define  PAGE_Gyro   (0x08000000 + 60 * 1024) 
#define  PAGE_Magic  (0x08000000 + 61 * 1024)

// #define   uchar unsigned char
// #define   uint unsigned int

//定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
#define    HMC5883_Addr   0x3C    //磁场传感器器件地址

// unsigned char BUF[8];                         //接收数据缓存区
// extern uchar    SlaveAddress;               //IIC写入时的地址字节数据，+1为读取

extern int magX, magY, magZ;    //hmc最原始数据

// extern uchar SlaveAddress;       //IIC写入时的地址字节数据，+1为读取

extern I2C_HandleTypeDef hi2c1;


static float mag_x = 0;
static float mag_y = 0;
static float mag_z = 0;

static int16_t  HMC5883_maxx = 0, HMC5883_maxy = 0, HMC5883_maxz = 0, HMC5883_minx = -0, HMC5883_miny = -0, HMC5883_minz = -0;

static void Read_Magic_Offset(int16_t *min_mx, int16_t *min_my, int16_t *min_mz, int16_t *max_mx, int16_t *max_my, int16_t *max_mz)
{

	// int16_t *temp_addr = (int16_t *)PAGE_Magic;
	// FLASH_Unlock();
	// *min_mx = *temp_addr;
	// temp_addr++;
	// *min_my = *temp_addr;
	// temp_addr++;
	// *min_mz = *temp_addr;
	// temp_addr++;

	// *max_mx = *temp_addr;
	// temp_addr++;
	// *max_my = *temp_addr;
	// temp_addr++;
	// *max_mz = *temp_addr;
	// FLASH_Lock();
}

static void MagWrite(uint16_t RegAddress, uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, HMC5883_Addr, RegAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);
}

static uint8_t MagRead(uint8_t RegAddress)
{
	uint8_t data;

	HAL_I2C_Mem_Read(&hi2c1, HMC5883_Addr + 1, RegAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);

	return data;
}

static int16_t GetData(uint8_t RegAddress)		//传感器现值采集
{
	uint8_t H, L;

	L = MagRead(RegAddress);
	H = MagRead(RegAddress + 1);

	return (H << 8) + L;				//合成数据
}

//**************************************
//初始化HMC5883，根据需要请参考pdf进行修改
//**************************************
void InitHMC5883()
{
	MagWrite(0x02, 0x00); //
	MagWrite(0x01, 0xE0); //
	// Read_Magic_Offset(&HMC5883_minx, &HMC5883_miny, &HMC5883_minz, &HMC5883_maxx, &HMC5883_maxy, &HMC5883_maxz);
	// MagWrite(HMC58X3_R_MODE, 0);
	osDelay(1);

	// MagWrite(HMC58X3_R_CONFA, 0x70); // 8 samples averaged, 75Hz frequency, no artificial bias.
	// MagWrite(HMC58X3_R_CONFB, 0xA0);
	// MagWrite(HMC58X3_R_MODE, 0x00);
}
//**************************************
//从HMC5883连续读取6个数据放在BUF中
//**************************************
void updateHMC5883()
{
	// SlaveAddress = HMC5883_Addr;
	MagWrite(0x00, 0x14);
	MagWrite(0x02, 0x00);
	//    Delayms(10);

	// BUF[1] = MagRead(0x03); //OUT_X_L_A
	// BUF[2] = MagRead(0x04); //OUT_X_H_A
	// BUF[3] = MagRead(0x07); //OUT_Y_L_A
	// BUF[4] = MagRead(0x08); //OUT_Y_H_A
	// BUF[5] = MagRead(0x05); //OUT_Z_L_A
	// BUF[6] = MagRead(0x06); //OUT_Y_H_A

	mag_x = (GetData(0x04) - ((HMC5883_maxx + HMC5883_minx) / 2)) * 1.0;
	mag_y = (GetData(0x08) - ((HMC5883_maxy + HMC5883_miny) / 2)) * 1.0;
	mag_z = (GetData(0x06) - ((HMC5883_maxz + HMC5883_minz) / 2)) * 1.0;

	//    if(magX>0x7fff)magX-=0xffff;//补码表示滴~所以要转化一下
	//    if(magY>0x7fff)magY-=0xffff;
	//     if(magZ>0x7fff)magZ-=0xffff;
}

float GetMagX(void)
{
	return mag_x;
}

float GetMagY(void)
{
	return mag_y;
}

float GetMagZ(void)
{
	return mag_z;
}

// //***************************************
// // HMC5883 51串口测试程序
// // 使用单片机STC89C51
// // 晶振：11.0592M
// // 显示：PC串口
// // 编译环境 Keil uVision2
// // 参考宏晶网站24c04通信程序
// // 时间：2011年3月1日
// //****************************************
// #include  <REG51.H>
// #include  <math.h>    //Keil library  
// #include  <stdio.h>   //Keil library 
// #include  <INTRINS.H>
// #define   uchar unsigned char
// #define   uint  unsigned int
// //使用的端口，请按照以下接线
// #define DataPort P0 //LCD1602数据端口
// sbit SCL = P1 ^ 0;  //IIC时钟引脚定义
// sbit SDA = P1 ^ 1;  //IIC数据引脚定义

// #define SlaveAddress   0x3C   //定义器件5883在IIC总线中的从地址
// typedef unsigned char BYTE;
// typedef unsigned short WORD;
// BYTE BUF[8];                         //接收数据缓存区
// uchar ge, shi, bai, qian, wan;       //显示变量
// int  dis_data;                       //变量
// //************
// //************
// void delay(unsigned int k);
// void Init_HMC5883(void);            //初始化5883
// void conversion(uint temp_data);
// void  Single_Write_HMC5883(uchar REG_Address, uchar REG_data);  //单个写入数据
// //uchar Single_Read_HMC5883(uchar REG_Address);                   //单个读取内部寄存器数据
// void  Multiple_Read_HMC5883();                                  //连续的读取内部寄存器数据
// //以下是模拟iic使用函数-------------
// void Delay5us();
// void Delay5ms();
// void HMC5883_Start();
// void HMC5883_Stop();
// void HMC5883_SendACK(bit ack);
// bit  HMC5883_RecvACK();
// void HMC5883_SendByte(BYTE dat);
// BYTE HMC5883_RecvByte();
// void HMC5883_ReadPage();
// void HMC5883_WritePage();
// //-----------------------------------
// //*********************************************
// //串口初始化
// //9600 bps @ 11.059 MHz
// void init_uart()
// {
// 	TMOD = 0x21;
// 	TH1 = 0xfd;
// 	TL1 = 0xfd;
// 	SCON = 0x50;
// 	PS = 1;    //串口中断设为高优先级别
// 	TR0 = 1;  //启动定时器
// 	TR1 = 1;
// 	ET0 = 1;   //打开定时器0中断
// 	ES = 1;
// }
// //*********串口数据发送******************
// void  SeriPushSend(uchar send_data)
// {
// 	SBUF = send_data;
// 	while(!TI);
// 	TI = 0;
// }
// //*********************************************************
// void conversion(uint temp_data)
// {
// 	wan = temp_data / 10000 + 0x30 ;
// 	temp_data = temp_data % 10000; //取余运算
// 	qian = temp_data / 1000 + 0x30 ;
// 	temp_data = temp_data % 1000; //取余运算
// 	bai = temp_data / 100 + 0x30   ;
// 	temp_data = temp_data % 100; //取余运算
// 	shi = temp_data / 10 + 0x30    ;
// 	temp_data = temp_data % 10;  //取余运算
// 	ge = temp_data + 0x30;
// }
// /*******************************/
// void delay(unsigned int k)
// {
// 	unsigned int i, j;
// 	for(i = 0; i < k; i++)
// 	{
// 		for(j = 0; j < 121; j++)
// 		{;}
// 	}
// }

// /**************************************
// 延时5微秒(STC90C52RC@12M)
// 不同的工作环境,需要调整此函数，注意时钟过快时需要修改
// 当改用1T的MCU时,请调整此延时函数
// **************************************/
// void Delay5us()
// {
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// 	_nop_();
// }
// /**************************************
// 延时5毫秒(STC90C52RC@12M)
// 不同的工作环境,需要调整此函数
// 当改用1T的MCU时,请调整此延时函数
// **************************************/
// void Delay5ms()
// {
// 	WORD n = 560;
// 	while (n--);
// }
// /**************************************
// 起始信号
// **************************************/
// void HMC5883_Start()
// {
// 	SDA = 1;                    //拉高数据线
// 	SCL = 1;                    //拉高时钟线
// 	Delay5us();                 //延时
// 	SDA = 0;                    //产生下降沿
// 	Delay5us();                 //延时
// 	SCL = 0;                    //拉低时钟线
// }
// /**************************************
// 停止信号
// **************************************/
// void HMC5883_Stop()
// {
// 	SDA = 0;                    //拉低数据线
// 	SCL = 1;                    //拉高时钟线
// 	Delay5us();                 //延时
// 	SDA = 1;                    //产生上升沿
// 	Delay5us();                 //延时
// }
// /**************************************
// 发送应答信号
// 入口参数:ack (0:ACK 1:NAK)
// **************************************/
// void HMC5883_SendACK(bit ack)
// {
// 	SDA = ack;                  //写应答信号
// 	SCL = 1;                    //拉高时钟线
// 	Delay5us();                 //延时
// 	SCL = 0;                    //拉低时钟线
// 	Delay5us();                 //延时
// }
// /**************************************
// 接收应答信号
// **************************************/
// bit HMC5883_RecvACK()
// {
// 	SCL = 1;                    //拉高时钟线
// 	Delay5us();                 //延时
// 	CY = SDA;                   //读应答信号
// 	SCL = 0;                    //拉低时钟线
// 	Delay5us();                 //延时
// 	return CY;
// }
// /**************************************
// 向IIC总线发送一个字节数据
// **************************************/
// void HMC5883_SendByte(BYTE dat)
// {
// 	BYTE i;
// 	for (i = 0; i < 8; i++)     //8位计数器
// 	{
// 		dat <<= 1;              //移出数据的最高位
// 		SDA = CY;               //送数据口
// 		SCL = 1;                //拉高时钟线
// 		Delay5us();             //延时
// 		SCL = 0;                //拉低时钟线
// 		Delay5us();             //延时
// 	}
// 	HMC5883_RecvACK();
// }
// /**************************************
// 从IIC总线接收一个字节数据
// **************************************/
// BYTE HMC5883_RecvByte()
// {
// 	BYTE i;
// 	BYTE dat = 0;
// 	SDA = 1;                    //使能内部上拉,准备读取数据,
// 	for (i = 0; i < 8; i++)     //8位计数器
// 	{
// 		dat <<= 1;
// 		SCL = 1;                //拉高时钟线
// 		Delay5us();             //延时
// 		dat |= SDA;             //读数据
// 		SCL = 0;                //拉低时钟线
// 		Delay5us();             //延时
// 	}
// 	return dat;
// }
// //***************************************************
// void Single_Write_HMC5883(uchar REG_Address, uchar REG_data)
// {
// 	HMC5883_Start();                  //起始信号
// 	HMC5883_SendByte(SlaveAddress);   //发送设备地址+写信号
// 	HMC5883_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf
// 	HMC5883_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
// 	HMC5883_Stop();                   //发送停止信号
// }
// /*
// //********单字节读取内部寄存器*************************
// uchar Single_Read_HMC5883(uchar REG_Address)
// {  uchar REG_data;
//    HMC5883_Start();                          //起始信号
//    HMC5883_SendByte(SlaveAddress);           //发送设备地址+写信号
//    HMC5883_SendByte(REG_Address);                   //发送存储单元地址，从0开始
//    HMC5883_Start();                          //起始信号
//    HMC5883_SendByte(SlaveAddress+1);         //发送设备地址+读信号
//    REG_data=HMC5883_RecvByte();              //读出寄存器数据
// HMC5883_SendACK(1);
// HMC5883_Stop();                           //停止信号
//    return REG_data;
// }
// */
// //******************************************************
// //
// //连续读出HMC5883内部角度数据，地址范围0x3~0x5
// //
// //******************************************************
// void Multiple_Read_HMC5883(void)
// {
// 	uchar i;
// 	HMC5883_Start();                          //起始信号
// 	HMC5883_SendByte(SlaveAddress);           //发送设备地址+写信号
// 	HMC5883_SendByte(0x03);                   //发送存储单元地址，从0x3开始
// 	HMC5883_Start();                          //起始信号
// 	HMC5883_SendByte(SlaveAddress + 1);       //发送设备地址+读信号
// 	for (i = 0; i < 7; i++)                  //连续读取6个地址数据，存储中BUF
// 	{
// 		BUF[i] = HMC5883_RecvByte();          //BUF[0]存储0x32地址中的数据
// 		if (i == 6)
// 		{
// 			HMC5883_SendACK(1);                //最后一个数据需要回NOACK
// 		}
// 		else
// 		{
// 			HMC5883_SendACK(0);                //回应ACK
// 		}
// 	}
// 	HMC5883_Stop();                          //停止信号
// 	Delay5ms();
// }
// //初始化HMC5883，根据需要请参考pdf进行修改****
// void Init_HMC5883()
// {
// 	Single_Write_HMC5883(0x02, 0x00); //
// 	Single_Write_HMC5883(0x01, 0xE0); //
// }
// //*********************************************************
// //主程序********
// //*********************************************************
// void main()
// {
// 	// bit sign_bit;
// 	unsigned int i;
// 	int x, y, z;
// 	double angle;

// 	delay(100);
// 	init_uart();
// 	Init_HMC5883();
// 	while(1)            //循环
// 	{
// 		Multiple_Read_HMC5883();      //连续读出数据，存储在BUF中
// 		x = BUF[0] << 8 | BUF[1];
// 		y = BUF[4] << 8 | BUF[5];
// 		z = BUF[2] << 8 | BUF[3];
// 		angle = atan2((double)y, (double)x) * (180 / 3.14159265) + 180; // angle in degrees
// 		conversion(angle);       //计算数据和显示
// 		SeriPushSend(bai);
// 		SeriPushSend(shi);
// 		SeriPushSend(ge);
// 		SeriPushSend(0x0d);
// 		SeriPushSend(0x0a);
// 		for (i = 0; i < 10000; i++); //延时
// 	}
// }

