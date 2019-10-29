#include "lib_oralarea.h"

#include <string.h>

#include "main.h"
#include "bsp_uart.h"

#define OPEN_MOUTH_ANGLE	15
#define TEETH_HIGH_ANGLE	20
#define TEMP_ANGLE	17.5

OralAreaInfo g_teeth_info[16] = {0};

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

static void TeethInfoRecord(uint8_t num, float presure)
{
	g_teeth_info[num].judge_cnt += 1;
	g_teeth_info[num].time      += JUDGE_PERIOD;
	g_teeth_info[num].presure   += presure;
}

static uint8_t PitchRollIsOverARange(float base_angle, float new_angle, float range)
{
	if((new_angle - base_angle <= range && new_angle - base_angle >= 0) || 
		(base_angle - new_angle <= range && base_angle - new_angle >= 0))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static uint8_t YawIsOverARange(float base_yaw, float new_yaw, float range)
{
	if ((base_yaw - new_yaw <= range && 0 <= base_yaw - new_yaw)
			|| (base_yaw + 360 - new_yaw <= range && 0 <= base_yaw + 360 - new_yaw)
			|| (new_yaw - base_yaw <= range && 0 <= new_yaw - base_yaw)
			|| (new_yaw + 360 - base_yaw <= range && 0 <= new_yaw + 360 - base_yaw))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static float RollLimit(float roll)
{
	if(roll > 180)
	{
		return (360 - roll);
	}
	else if(roll < -180)
	{
		return (-360 - roll);
	}

	return roll;
}

static float PitchLimit(float pitch)
{
	if(pitch > 90)
	{
		return (180 - pitch);
	}
	else if(pitch < -90)
	{
		return (-180 - pitch);
	}

	return pitch;
}

static float YawLimit(float yaw)
{
	if(yaw > 360)
	{
		return (yaw - 360);
	}
	else if(yaw < 0)
	{
		return (yaw + 360);
	}

	return yaw;
}

void GetAllAreaRangeByRightHand(float roll, float pitch, float yaw)	// 起始区域为门牙
{
	float head_pitch = 0;		// 头的俯仰角

	head_pitch = roll - 90;		// 右手模式下，roll初始值为0~180，左手为-180~0

	// 中上外
	g_teeth_info[7].roll = roll - 17.5;
	g_teeth_info[7].pitch = 0;
	g_teeth_info[7].yaw = yaw;
	g_teeth_info[7].roll = RollLimit(g_teeth_info[7].roll);

	// 中下外
	g_teeth_info[9].roll = roll + 17.5;
	g_teeth_info[9].pitch = 0;
	g_teeth_info[9].yaw = yaw;
	g_teeth_info[9].roll = RollLimit(g_teeth_info[9].roll);

	// 中上内
	g_teeth_info[6].roll = roll - 100;
	g_teeth_info[6].pitch = 0 - TEETH_HIGH_ANGLE;
	g_teeth_info[6].yaw = yaw - 10;
	g_teeth_info[6].roll = RollLimit(g_teeth_info[6].roll);
	g_teeth_info[6].yaw = YawLimit(g_teeth_info[6].yaw);

	// 中下内
	g_teeth_info[8].roll = roll + 120;
	g_teeth_info[8].pitch = pitch;
	g_teeth_info[8].yaw = yaw - 20;
	g_teeth_info[8].roll = RollLimit(g_teeth_info[8].roll);
	g_teeth_info[8].yaw = YawLimit(g_teeth_info[8].yaw);

	// 左上外
	g_teeth_info[1].roll = 90 - 17.5;
	g_teeth_info[1].pitch = head_pitch;
	g_teeth_info[1].yaw = yaw - 50;
	g_teeth_info[1].pitch = PitchLimit(g_teeth_info[1].pitch);
	g_teeth_info[1].yaw = YawLimit(g_teeth_info[1].yaw);

	// 左下外
	g_teeth_info[4].roll = 90 + 17.5;
	g_teeth_info[4].pitch = head_pitch;
	g_teeth_info[4].yaw = g_teeth_info[1].yaw;

	// 左下咬合面
	g_teeth_info[5].roll = roll + 100;
	g_teeth_info[5].pitch = head_pitch - OPEN_MOUTH_ANGLE;	// OPEN_MOUTH_ANGLE为张嘴补偿角度
	g_teeth_info[5].yaw = yaw - 45;
	g_teeth_info[5].roll = RollLimit(g_teeth_info[5].roll);
	g_teeth_info[5].pitch = PitchLimit(g_teeth_info[5].pitch);
	g_teeth_info[5].yaw = YawLimit(g_teeth_info[5].yaw);

	// 左上咬合面
	g_teeth_info[2].roll = roll - 85;
	g_teeth_info[2].pitch = head_pitch - 15;
	g_teeth_info[2].yaw = yaw - 60;
	g_teeth_info[2].roll = RollLimit(g_teeth_info[2].roll);
	g_teeth_info[2].pitch = PitchLimit(g_teeth_info[2].pitch);
	g_teeth_info[2].yaw = YawLimit(g_teeth_info[2].yaw);

	// 左下内
	g_teeth_info[3].roll = roll + 145; //-90 - 17.5;
	g_teeth_info[3].pitch = head_pitch - 10;	// OPEN_MOUTH_ANGLE为张嘴补偿角度
	g_teeth_info[3].yaw = yaw - 60;
	g_teeth_info[3].roll = RollLimit(g_teeth_info[3].roll);
	g_teeth_info[3].pitch = PitchLimit(g_teeth_info[3].pitch);
	g_teeth_info[3].yaw = YawLimit(g_teeth_info[3].yaw);

	// 左上内
	g_teeth_info[0].roll = roll -90 + 17.5;
	g_teeth_info[0].pitch = head_pitch - TEETH_HIGH_ANGLE;
	g_teeth_info[0].yaw = yaw - 50;
	g_teeth_info[0].roll = RollLimit(g_teeth_info[0].roll);
	g_teeth_info[0].pitch = PitchLimit(g_teeth_info[0].pitch);
	g_teeth_info[0].yaw = YawLimit(g_teeth_info[0].yaw);

	// 右上外
	g_teeth_info[11].roll = -90 + 17.5; // roll - 110;
	g_teeth_info[11].pitch = head_pitch - 20;
	g_teeth_info[11].yaw = yaw - 100;
	g_teeth_info[11].pitch = PitchLimit(g_teeth_info[11].pitch);
	g_teeth_info[11].yaw = YawLimit(g_teeth_info[11].yaw);

	// 右下外
	g_teeth_info[14].roll = -90 - 17.5;
	g_teeth_info[14].pitch = head_pitch;
	g_teeth_info[14].yaw = yaw - 100;
	g_teeth_info[14].pitch = PitchLimit(g_teeth_info[14].pitch);
	g_teeth_info[14].yaw = YawLimit(g_teeth_info[14].yaw);

	// 右下咬合面
	g_teeth_info[15].roll = roll + 120;
	g_teeth_info[15].pitch = head_pitch - OPEN_MOUTH_ANGLE;	// OPEN_MOUTH_ANGLE为张嘴补偿角度
	g_teeth_info[15].yaw = yaw - 85;
	g_teeth_info[15].roll = RollLimit(g_teeth_info[15].roll);
	g_teeth_info[15].pitch = PitchLimit(g_teeth_info[15].pitch);
	g_teeth_info[15].yaw = YawLimit(g_teeth_info[15].yaw);

	// 右上咬合面
	g_teeth_info[12].roll = roll - 60;
	g_teeth_info[12].pitch = head_pitch - 15;
	g_teeth_info[12].yaw = yaw - 95;
	g_teeth_info[12].roll = RollLimit(g_teeth_info[12].roll);
	g_teeth_info[12].pitch = PitchLimit(g_teeth_info[12].pitch);
	g_teeth_info[12].yaw = YawLimit(g_teeth_info[12].yaw);

	// 右下内
	g_teeth_info[13].roll = roll + 55; //90 - 17.5;
	g_teeth_info[13].pitch = head_pitch - OPEN_MOUTH_ANGLE + TEETH_HIGH_ANGLE;	// OPEN_MOUTH_ANGLE为张嘴补偿角度
	g_teeth_info[13].yaw = yaw - 80;
	g_teeth_info[13].roll = RollLimit(g_teeth_info[13].roll);
	g_teeth_info[13].pitch = PitchLimit(g_teeth_info[13].pitch);
	g_teeth_info[13].yaw = YawLimit(g_teeth_info[13].yaw);

	// 右上内
	g_teeth_info[10].roll = roll -20;
	g_teeth_info[10].pitch = head_pitch - 15;
	g_teeth_info[10].yaw = yaw - 80;
	g_teeth_info[10].roll = RollLimit(g_teeth_info[10].roll);
	g_teeth_info[10].pitch = PitchLimit(g_teeth_info[10].pitch);
	g_teeth_info[10].yaw = YawLimit(g_teeth_info[10].yaw);

	DBG_INFO(" 0\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[0].roll, g_teeth_info[0].pitch, g_teeth_info[0].yaw);
	DBG_INFO(" 1\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[1].roll, g_teeth_info[1].pitch, g_teeth_info[1].yaw);
	DBG_INFO(" 2\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[2].roll, g_teeth_info[2].pitch, g_teeth_info[2].yaw);
	DBG_INFO(" 3\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[3].roll, g_teeth_info[3].pitch, g_teeth_info[3].yaw);
	DBG_INFO(" 4\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[4].roll, g_teeth_info[4].pitch, g_teeth_info[4].yaw);
	DBG_INFO(" 5\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[5].roll, g_teeth_info[5].pitch, g_teeth_info[5].yaw);
	DBG_INFO(" 6\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[6].roll, g_teeth_info[6].pitch, g_teeth_info[6].yaw);
	DBG_INFO(" 7\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[7].roll, g_teeth_info[7].pitch, g_teeth_info[7].yaw);
	DBG_INFO(" 8\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[8].roll, g_teeth_info[8].pitch, g_teeth_info[8].yaw);
	DBG_INFO(" 9\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[9].roll, g_teeth_info[9].pitch, g_teeth_info[9].yaw);
	DBG_INFO("10\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[10].roll, g_teeth_info[10].pitch, g_teeth_info[10].yaw);
	DBG_INFO("11\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[11].roll, g_teeth_info[11].pitch, g_teeth_info[11].yaw);
	DBG_INFO("12\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[12].roll, g_teeth_info[12].pitch, g_teeth_info[12].yaw);
	DBG_INFO("13\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[13].roll, g_teeth_info[13].pitch, g_teeth_info[13].yaw);
	DBG_INFO("14\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[14].roll, g_teeth_info[14].pitch, g_teeth_info[14].yaw);
	DBG_INFO("15\troll:%.2f\tpitch:%.2f\tyaw:%.2f\r\n", g_teeth_info[15].roll, g_teeth_info[15].pitch, g_teeth_info[15].yaw);
}

uint8_t JudgeOralArea(float roll, float pitch, float yaw, float presure)
{
	if(presure > 2)		// 压力值到达判断标准
	{
		if((0 == PitchRollIsOverARange(g_teeth_info[0].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[0].roll, roll, 23)) && 
			(0 == YawIsOverARange(g_teeth_info[0].yaw, yaw, 20)))
		{
			return 0;	// 左上内
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[1].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[1].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[1].yaw, yaw, 20)))
		{
			return 1;	// 左上外
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[2].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[2].roll, roll, 12)) && 
			(0 == YawIsOverARange(g_teeth_info[2].yaw, yaw, 20)))
		{
			return 2;	// 左上咬合面
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[3].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[3].roll, roll, 20)) && 
			(0 == YawIsOverARange(g_teeth_info[3].yaw, yaw, 20)))
		{
			return 3;	// 左下内
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[4].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[4].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[4].yaw, yaw, 20)))
		{
			return 4;	// 左下外
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[5].pitch, pitch, 20)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[5].roll, roll, 15)) && 
			(0 == YawIsOverARange(g_teeth_info[5].yaw, yaw, 20)))
		{
			return 5;	// 左下咬合面
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[6].pitch, pitch, 50)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[6].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[6].yaw, yaw, 20)))
		{
			return 6;	// 中上内
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[7].pitch, pitch, 15)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[7].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[7].yaw, yaw, 20)))
		{
			return 7;	// 中上外
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[8].pitch, pitch, 60)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[8].roll, roll, 23)) && 
			(0 == YawIsOverARange(g_teeth_info[8].yaw, yaw, 20)))
		{
			return 8;	// 中下内
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[9].pitch, pitch, 15)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[9].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[9].yaw, yaw, 20)))
		{
			return 9;	// 中下外
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[10].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[10].roll, roll, 23)) && 
			(0 == YawIsOverARange(g_teeth_info[10].yaw, yaw, 25)))
		{
			return 10;	// 右上内
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[11].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[11].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[11].yaw, yaw, 20)))
		{
			return 11;	// 右上外
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[12].pitch, pitch, 15)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[12].roll, roll, 12)) && 
			(0 == YawIsOverARange(g_teeth_info[12].yaw, yaw, 20)))
		{
			return 12;	// 右上咬合面
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[13].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[13].roll, roll, 25)) && 
			(0 == YawIsOverARange(g_teeth_info[13].yaw, yaw, 20)))
		{
			return 13;	// 右下内
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[14].pitch, pitch, 25)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[14].roll, roll, 17.5)) && 
			(0 == YawIsOverARange(g_teeth_info[14].yaw, yaw, 20)))
		{
			return 14;	// 右下外
		}

		if((0 == PitchRollIsOverARange(g_teeth_info[15].pitch, pitch, 15)) && 
			(0 == PitchRollIsOverARange(g_teeth_info[15].roll, roll, 12)) && 
			(0 == YawIsOverARange(g_teeth_info[15].yaw, yaw, 20)))
		{
			return 15;	// 右下咬合面
		}
	}
	
	return 16;
}
