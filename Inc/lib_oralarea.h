#ifndef _LIB_ORAL_AREA_H_
#define _LIB_ORAL_AREA_H_

#include <stdint.h>

#define JUDGE_PERIOD	0.5f	// unit:s

// 区域			对应数据编号
// 左上内		1 - 1
// 左上外		2 - 1
// 左上咬合面	3 - 1
// 左下内		4 - 1
// 左下外		5 - 1
// 左下咬合面	6 - 1
// 中上内		7 - 1
// 中上外		8 - 1
// 中下内		9 - 1
// 中下外		10 - 1
// 右上内		11 - 1
// 右上外		12 - 1
// 右上咬合面	13 - 1
// 右下内		14 - 1
// 右下外		15 - 1
// 右下咬合面	16 - 1

typedef struct
{
	uint32_t judge_cnt;
	float time;
	float presure;
	float roll;
	float pitch;
	float yaw;
} OralAreaInfo;

extern OralAreaInfo g_teeth_info[16];	// 16 area data

void GetAllAreaRangeByRightHand(float roll, float pitch, float yaw);
uint8_t JudgeOralArea(float roll, float pitch, float yaw, float presure);

#endif
