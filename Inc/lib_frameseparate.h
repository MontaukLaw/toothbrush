/**
 * Copyright (c) 2017, Afuiot Tech, Co., Ltd
 * All rights reserverd.
 * File name: lib_frameseparate.h
 * Description: 
 * Other: none
 * Version: V1.0
 * History: Version, Writer, modifier, modify time, modify content
             V1.0     wucb     wucb    2019.02.20   first version
 */

#ifndef _LIB_FRAMESEPARATE_H_
#define _LIB_FRAMESEPARATE_H_

#include <stdint.h>

typedef struct
{
	uint8_t frame_flag;
	uint32_t frame_index;
	char first;
	char second;
	char second_to_last;
	char last;
	uint32_t buff_len;
	uint8_t *frame_buff;
} DataFrameStruct;

void FrameStructInit(DataFrameStruct *frame,  char first,
					 char second,  char second_to_last,
					 char last, uint32_t buff_len);
void FrameStructDelete(DataFrameStruct *frame);
void DataFrameDecode(uint8_t *buff, uint32_t len,
					 DataFrameStruct *frame,
					 void (*FrameProcess)(uint8_t *, uint32_t));

#endif
