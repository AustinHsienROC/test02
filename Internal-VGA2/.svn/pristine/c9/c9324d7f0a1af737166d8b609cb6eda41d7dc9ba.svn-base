/*
===================================================================
Name		: VideoHdInfo.h
Author		: Brant
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: 
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2015/02/26	  | v0.0010  	|First setup functions        	    	  | Brant
--------------+-------------+-----------------------------------------+------------
*/

#ifndef __VIDEO_HD_INFO_H
#define __VIDEO_HD_INFO_H

#define C_VIDEO_FRAME_HD_FRAME_WIDTH_DW_INDEX			0
#define C_VIDEO_FRAME_HD_FRAME_HEIGHT_DW_INDEX			1
#define C_VIDEO_FRAME_HD_FRAME_COLOR_MODEL_DW_INDEX		2
#define C_VIDEO_FRAME_HD_FRAME_COUNT_DW_INDEX			3
#define C_VIDEO_FRAME_HD_FRAME_TIME_STAMP_DW_INDEX		4
#define C_VIDEO_FRAME_HD_FRAME_CAR_INFO1_DW_INDEX		5
#define C_VIDEO_FRAME_HD_FRAME_CAR_INFO2_DW_INDEX		6
#define C_VIDEO_FRAME_HD_FRAME_ICP_INFO					7
#define C_VIDEO_FRAME_HD_FRAME_SENSOR_INFO				8
#define C_VIDEO_FRAME_HD_FRAME_LDWS_LLINE_INFO		9
#define C_VIDEO_FRAME_HD_FRAME_LDWS_RLINE_INFO		10
#define C_VIDEO_FRAME_HD_FRAME_LDWS_DIST					11
#define C_VIDEO_FRAME_HD_FRAME_PC_TIMESTAMP			12
#define C_VIDEO_FRAME_HD_FRAME_WHEEL1_INFO				13
#define C_VIDEO_FRAME_HD_FRAME_WHEEL2_INFO				14
#define C_VIDEO_FRAME_HD_EXT_RESERVED						15

#define C_VIDEO_FRAME_HD_FRAME_CAR_INFO_BYTES			7
#define C_VIDEO_FRAME_HD_FRAME_WHEEL_INFO_BYTES	8

#define C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX	20
#define C_VIDEO_FRAME_HD_CAR_SPEED_LO_BYTE_INDEX		21
#define C_VIDEO_FRAME_HD_CAR_SPEED_HI_BYTE_INDEX		22
#define C_VIDEO_FRAME_HD_CAR_ANGLE_LO_BYTE_INDEX		23
#define C_VIDEO_FRAME_HD_CAR_ANGLE_HI_BYTE_INDEX		24
#define C_VIDEO_FRAME_HD_CAR_INFO_MULTI_VIEW_BYTE_INDEX	25
#define C_VIDEO_FRAME_HD_CAR_INFO_RESERVE_BYTE_INDEX		26
#define C_VIDEO_FRAME_HD_CAR_INFO_CHECKSUM_BYTE_INDEX		27
#define C_VIDEO_FRAME_HD_WHEEL_INFO_CHECKSUM_BYTE_INDEX	44
#define C_VIDEO_FRAME_HD_NEXT_HEAD_BYTE_INDEX				63

#define C_VIDEO_FRAME_HD_ICP_INFO_ICP_MODE_WORD_INDEX		15
#define C_VIDEO_FRAME_HD_ICP_INFO_ICP_RESULT_BYTE_INDEX		28
#define C_VIDEO_FRAME_HD_ICP_INFO_ICP_FRAME_COUNT_BYTE_INDEX	31
#define C_VIDEO_FRAME_HD_LDWS_LDIST_WORD_INDEX					22
#define C_VIDEO_FRAME_HD_LDWS_RDIST_WORD_INDEX					23
#define C_VIDEO_FRAME_HD_LLINE_INFO_M_WORD_INDEX				18
#define C_VIDEO_FRAME_HD_LLINE_INFO_C_WORD_INDEX				19
#define C_VIDEO_FRAME_HD_RLINE_INFO_M_WORD_INDEX				20
#define C_VIDEO_FRAME_HD_RLINE_INFO_C_WORD_INDEX				21

#define C_VIDEO_FRAME_HD_SENSOR_INFO_GAIN_WORD_INDEX		16
#define C_VIDEO_FRAME_HD_SENSOR_INFO_BV_WORD_INDEX			17

//C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX	Bits definition
#define C_CAR_INFO_DRIVING_RIGHT_TURN_BIT	0
#define C_CAR_INFO_DRIVING_LEFT_TURN_BIT	1
#define C_CAR_INFO_DRIVING_REVERSE_BIT		2
#define C_CAR_INFO_DRIVING_BREAK_BIT		3

#define C_CAR_INFO_BENCHMARK_BIT0		4
#define C_CAR_INFO_BENCHMARK_BIT1		5
#define C_CAR_INFO_BENCHMARK_BIT2		6

#define C_CAR_INFO_BENCHMARK_MASK		((0x1<<C_CAR_INFO_BENCHMARK_BIT0) | (0x1<<C_CAR_INFO_BENCHMARK_BIT1) |	(0x1<<C_CAR_INFO_BENCHMARK_BIT2))
#define C_VIDEO_FRAME_HD_COMPRESS_IMG_SIZE_MASK	0x00FFFFFF
#define C_VIDEO_FRAME_HD_NEXT_HEAD_MASK	0x80

#define C_VIDEO_FRAME_HD_COMPRESS_TYPE_MASK	0x07
#define C_VIDEO_FRAME_HD_COMPRESS_TYPE_BIT0		0
//Define compress type
#define C_VIDEO_FRAME_HD_COMPRESS_TYPE_NO		0
#define C_VIDEO_FRAME_HD_COMPRESS_TYPE_ZLIB		1
#define C_VIDEO_FRAME_HD_COMPRESS_TYPE_JPEG		2

//
// Algorithm Status definition
//
#define LDWS_LEFT_LINE_FOUND_BIT	6
#define LDWS_RIGHT_LINE_FOUND_BIT	5
#define LDWS_LEFT_LINE_WARNING_BIT	4
#define LDWS_RIGHT_LINE_WARNING_BIT	3

#define LDWS_LEFT_LINE_FOUND_MASK		(1<<LDWS_LEFT_LINE_FOUND_BIT)
#define LDWS_RIGHT_LINE_FOUND_MASK		(1<<LDWS_RIGHT_LINE_FOUND_BIT)
#define LDWS_LEFT_LINE_WARNING_MASK		(1<<LDWS_LEFT_LINE_WARNING_BIT)
#define LDWS_RIGHT_LINE_WARNING_MASK	(1<<LDWS_RIGHT_LINE_WARNING_BIT)
#define LDWS_LINE_WARNING_MASK			(LDWS_LEFT_LINE_WARNING_MASK|LDWS_RIGHT_LINE_WARNING_MASK)

#define MOD_WARNING_BIT		1
#define MOD_WORKING_BIT		0

#define MOD_WORKING_MASK			(1<<MOD_WORKING_BIT)
#define MOD_WARNING_MASK			(1<<MOD_WARNING_BIT)

unsigned char GetCheckSum8Bits(unsigned char* a_pData, unsigned int a_unDataSize);
bool VerifyCheckSum8Bits(unsigned char * a_pData, unsigned int a_unDataSize, unsigned char a_ubCheckSum);

//frame head
DWORD GetVideoFrameHdInfo_FrameWidth(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_FrameHeight(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_ColorModel(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_FrameCount(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_TimeStamp(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_CarInfo1(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_CarInfo2(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_GetCompressImgSize(unsigned char *a_pFreamHead);
bool  GetVideoFrameHdInfo_HasNextHead(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_GetCompressType(unsigned char *a_pFreamHead);
void VideoFrameHdInfo_SetCompressType(unsigned char *a_pFreamHead
    , unsigned char a_ucCompressType);

//car information
//You must call this function to check if car information is validate
bool GetVideoFrameHdInfo_IsCarInfoValid(unsigned char *a_pFreamHead);
void VideoFrameHdInfo_UpdateAngle_10Times(unsigned char *a_pFreamHead
    ,unsigned short a_usNewAngle);
unsigned short GetVideoFrameHdInfo_CarSpeed_10Times(unsigned char *a_pFreamHead);
unsigned short GetVideoFrameHdInfo_WheelSpeed_10Times(unsigned char a_ucIndex
    , unsigned char *a_pFreamHead);
signed short GetVideoFrameHdInfo_CarAngle_10Times(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_CarInfo_CheckSum(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_CarInfo_IsLeftTurn(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_CarInfo_IsRightTurn(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_CarInfo_IsReverse(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_CarInfo_IsBreak(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_CtrlInfo_GetLDWSEnableSpeedMode(unsigned char *a_pFreamHead);

WORD GetVideoFrameHdInfo_GetICPMode(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_GetBenchMarkResult(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_IsWarning_BenchMarkResult(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_GetICPResult_LDWS(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_IsWarning_ICPResult_LDWS(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_GetICPResult_MOD(unsigned char *a_pFreamHead);
bool GetVideoFrameHdInfo_IsWarning_ICPResult_MOD(unsigned char *a_pFreamHead);
signed short GetVideoFrameHdInfo_GetLDWS_LDist(unsigned char *a_pFreamHead);
signed short GetVideoFrameHdInfo_GetLDWS_RDist(unsigned char *a_pFreamHead);
double GetVideoFrameHdInfo_GetLDWS_LLine_Equ_M(unsigned char *a_pFreamHead);
double GetVideoFrameHdInfo_GetLDWS_LLine_Equ_C(unsigned char *a_pFreamHead);
double GetVideoFrameHdInfo_GetLDWS_RLine_Equ_M(unsigned char *a_pFreamHead);
double GetVideoFrameHdInfo_GetLDWS_RLine_Equ_C(unsigned char *a_pFreamHead);
DWORD GetVideoFrameHdInfo_GetPCTimeStamp(unsigned char *a_pFreamHead);
unsigned char GetVideoFrameHdInfo_GetComBusFrameCnt(unsigned char *a_pFreamHead);


WORD GetVideoFrameHdInfo_GetSensorBV(unsigned char *a_pFreamHead);
WORD GetVideoFrameHdInfo_GetSensorGain(unsigned char *a_pFreamHead);
#endif