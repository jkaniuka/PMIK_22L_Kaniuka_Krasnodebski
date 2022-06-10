/**
  ******************************************************************************
  * @file    ring_buffer.c
  * @brief   The file contains ring buffer implementation for nRF24L01+ transreceiver.
  * This file was written by Mateusz Salamon (mateusz@msalamon.pl) and is distributed on terms of MIT License.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ring_buffer.h"
#include "stdlib.h"

RB_Status RB_CreateBuffer(RingBuffer **Buffer, uint8_t Size)
{
	*Buffer = malloc(sizeof(RingBuffer) + (sizeof(uint8_t) * Size));

	if(Buffer == NULL)
	{
		return RB_NOTCREATED;
	}

	(*Buffer)->Size = Size;
	(*Buffer)->Head = 0;
	(*Buffer)->Tail = 0;
	(*Buffer)->Elements = 0;

	return RB_OK;
}

RB_Status RB_WriteToBuffer(RingBuffer *Buffer, uint8_t Data)
{
	uint8_t TempHead;

	TempHead = (Buffer->Head + 1) % Buffer->Size;

	if( TempHead == Buffer->Tail) // No room for new data
	{
		return RB_NOFREESPACE;
	}
	else
	{
		Buffer->Buffer[Buffer->Head] = Data;

		Buffer->Head++;
		Buffer->Head %= Buffer->Size;

		Buffer->Elements++;
	}

	return RB_OK;
}

RB_Status RB_ReadFromBuffer(RingBuffer *Buffer, uint8_t *Data)
{
	if( Buffer->Tail == Buffer->Head) // No data to read
	{
		return RB_NOELEMENTS;
	}
	else
	{
		*Data = Buffer->Buffer[Buffer->Tail];

		Buffer->Tail++;
		Buffer->Tail %= Buffer->Size;

		Buffer->Elements--;
	}
	return RB_OK;
}

uint8_t RB_ElementsAvailable(RingBuffer *Buffer)
{
	return Buffer->Elements;
}
