/**
 * Copyright (c) 2017, Afuiot Tech, Co., Ltd
 * All rights reserverd.
 * File name: lib_frameseparate.c
 * Description: 
 * Other: none
 * Version: V1.0
 * History: Version, Writer, modifier, modify time, modify content
             V1.0     wucb     wucb    2019.02.20   first version
 */

#include "lib_frameseparate.h"

#include <string.h>
#include <stdlib.h>

/**
 * Description: find frame head and tail.
 * Parameter: data - the char that need to check.
 * Return: 1 - frame is complete.
           0 - frame is uncomplete.
 * Others: None
 */
static uint8_t DataFrameCheck(uint8_t data, DataFrameStruct *frame)
{
	if (0 != frame->second && 1 == frame->frame_flag && 1 == frame->frame_index)
	{
		// if (1 == frame->frame_flag && 1 == frame->frame_index)
		// {
		if (frame->second != data)
		{
			frame->frame_index = 0;
			frame->frame_flag = 0;
			memset(frame->frame_buff, 0, frame->buff_len);
		}
		// }
	}

	if (0 == frame->frame_flag)
	{
		if (frame->first == data)
		{
			frame->frame_index = 1;
			memset(frame->frame_buff, 0, frame->buff_len);
			frame->frame_buff[0] = data;
			frame->frame_flag = 1;
		}

		return 0;
	}

	if ((frame->buff_len - 2) <= frame->frame_index)
	{
		frame->frame_index = 0;
		frame->frame_flag = 0;
		memset(frame->frame_buff, 0, frame->buff_len);
	}
	else
	{
		frame->frame_buff[frame->frame_index] = data;

		if (4 <= frame->frame_index && frame->last == frame->frame_buff[frame->frame_index])
		{
			if (0 != frame->second_to_last && frame->second_to_last == frame->frame_buff[frame->frame_index - 1])
			{
				frame->frame_flag = 0;
				return 1;
			}
		}

		frame->frame_index++;
	}

	return 0;
}

/**
 * Description: Decode the data and ensure that it is complete.
 * Parameter: len - the lengh that read from fifo.
              buff - data buffer that read from fifo.
 * Return: None
 * Others: None
 */
void DataFrameDecode(uint8_t *buff, uint32_t len,
					 DataFrameStruct *frame,
					 void (*FrameProcess)(uint8_t *, uint32_t))
{
	uint32_t i = 0;

	for(; i < len; i++)
	{
		if (1 == DataFrameCheck(buff[i], frame))
		{
			/* Got a frame complete, call back the process function. */
			FrameProcess(frame->frame_buff, frame->frame_index + 1);

			frame->frame_index = 0;
			frame->frame_flag = 0;
			memset(frame->frame_buff, 0, frame->buff_len);
		}
	}
}

void FrameStructInit(DataFrameStruct *frame,  char first,
					 char second,  char second_to_last,
					 char last, uint32_t buff_len)
{
	frame->frame_flag = 0;
	frame->frame_index = 0;
	frame->first = first;
	frame->second = second;
	frame->second_to_last = second_to_last;
	frame->last = last;
	frame->buff_len = buff_len;
	frame->frame_buff = (uint8_t *) malloc(buff_len);
}

void FrameStructDelete(DataFrameStruct *frame)
{
	free(frame->frame_buff);
}
