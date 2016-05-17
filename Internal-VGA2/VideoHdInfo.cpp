#include <Windows.h>
#include "VideoHdInfo.h"



unsigned char GetCheckSum8Bits(unsigned char* a_pData, unsigned int a_unDataSize)
{
    unsigned int unSum;
    unsigned int iloop;

    for (iloop = 0,unSum=0; iloop < a_unDataSize; iloop++)
        unSum += a_pData[iloop];

    unsigned char bCheckSum = 0XFF - (unSum&0xFF);		

    return bCheckSum;
}

bool VerifyCheckSum8Bits(unsigned char * a_pData, unsigned int a_unDataSize, unsigned char a_ubCheckSum)
{
    unsigned int unSum;
    unsigned int iloop;
    for (iloop = 0,unSum=0; iloop < a_unDataSize; iloop++)
        unSum += a_pData[iloop];

    if (0xFF != ((a_ubCheckSum+unSum)&0xFF))		
    {
        return false;
    }

    return true;
}

DWORD GetVideoFrameHdInfo_FrameWidth(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_WIDTH_DW_INDEX];
}

DWORD GetVideoFrameHdInfo_FrameHeight(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_HEIGHT_DW_INDEX];
}

DWORD GetVideoFrameHdInfo_ColorModel(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_COLOR_MODEL_DW_INDEX];
}

DWORD GetVideoFrameHdInfo_FrameCount(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_COUNT_DW_INDEX];
}

DWORD GetVideoFrameHdInfo_TimeStamp(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_TIME_STAMP_DW_INDEX];
}

bool GetVideoFrameHdInfo_IsCarInfoValid(unsigned char *a_pFreamHead)
{
    return VerifyCheckSum8Bits(
        a_pFreamHead+C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX
        , 7
        , a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_CHECKSUM_BYTE_INDEX]);
}

void VideoFrameHdInfo_UpdateAngle_10Times(unsigned char *a_pFreamHead
    ,unsigned short a_usNewAngle)
{
    if (NULL==a_pFreamHead)
        return ;

    //Update new value
    a_pFreamHead[C_VIDEO_FRAME_HD_CAR_ANGLE_LO_BYTE_INDEX] = a_usNewAngle&0x00FF;
    a_pFreamHead[C_VIDEO_FRAME_HD_CAR_ANGLE_HI_BYTE_INDEX] = 
        (a_usNewAngle&0xFF00)>>8;

    //calculate new check sum
    unsigned char bCheckSum = GetCheckSum8Bits(
        a_pFreamHead+C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX
        , 7);
    //update check sum
    a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_CHECKSUM_BYTE_INDEX] = bCheckSum;
}

DWORD GetVideoFrameHdInfo_CarInfo1(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_CAR_INFO1_DW_INDEX];
}

DWORD GetVideoFrameHdInfo_CarInfo2(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;

    if (NULL==a_pFreamHead)
        return 0;

    return pHd[C_VIDEO_FRAME_HD_FRAME_CAR_INFO2_DW_INDEX];
}

DWORD GetVideoFrameHdInfo_GetCompressImgSize(unsigned char *a_pFreamHead)
{
    DWORD *pHd = (DWORD*)a_pFreamHead;
    DWORD dwCompressImgSize=0;

    if (NULL==a_pFreamHead)
        return 0;

    dwCompressImgSize = pHd[C_VIDEO_FRAME_HD_EXT_RESERVED];
    dwCompressImgSize &= C_VIDEO_FRAME_HD_COMPRESS_IMG_SIZE_MASK;
    return dwCompressImgSize;
}

bool		GetVideoFrameHdInfo_HasNextHead(unsigned char *a_pFreamHead)
{	
    if (NULL==a_pFreamHead)
        return false;

    if (a_pFreamHead[C_VIDEO_FRAME_HD_NEXT_HEAD_BYTE_INDEX] & 
        C_VIDEO_FRAME_HD_NEXT_HEAD_MASK)
        return true;

    return false;
}

unsigned char GetVideoFrameHdInfo_GetCompressType(unsigned char *a_pFreamHead)
{	
    unsigned char ucType;
    if (NULL==a_pFreamHead)
        return 0;

    ucType = (a_pFreamHead[C_VIDEO_FRAME_HD_NEXT_HEAD_BYTE_INDEX] & 
        C_VIDEO_FRAME_HD_COMPRESS_TYPE_MASK)>>C_VIDEO_FRAME_HD_COMPRESS_TYPE_BIT0;


    return ucType;
}

void VideoFrameHdInfo_SetCompressType(unsigned char *a_pFreamHead
    , unsigned char a_ucCompressType)
{	
    unsigned char ucType;
    if (NULL==a_pFreamHead)
        return ;

    ucType = (a_ucCompressType) << C_VIDEO_FRAME_HD_COMPRESS_TYPE_BIT0;
    ucType &= C_VIDEO_FRAME_HD_COMPRESS_TYPE_MASK;
    a_pFreamHead[C_VIDEO_FRAME_HD_NEXT_HEAD_BYTE_INDEX] = 
        (a_pFreamHead[C_VIDEO_FRAME_HD_NEXT_HEAD_BYTE_INDEX] & 
        (~C_VIDEO_FRAME_HD_COMPRESS_TYPE_MASK)) | ucType;		
}

unsigned short GetVideoFrameHdInfo_CarSpeed_10Times(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    unsigned short usSpeed = a_pFreamHead[C_VIDEO_FRAME_HD_CAR_SPEED_LO_BYTE_INDEX]
    + (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_SPEED_HI_BYTE_INDEX]<<8);

    return usSpeed;
}

unsigned short GetVideoFrameHdInfo_WheelSpeed_10Times(unsigned char a_ucIndex
    , unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;
    unsigned short usSpeed;
    unsigned short usOffset=0;
    if (1==a_ucIndex)
        usOffset = 2;
    else if (2==a_ucIndex)
        usOffset = 4;
    else if (3==a_ucIndex)
        usOffset = 6;

    usSpeed = (a_pFreamHead[C_VIDEO_FRAME_HD_FRAME_WHEEL1_INFO*4+usOffset]<<8)
        + (a_pFreamHead[C_VIDEO_FRAME_HD_FRAME_WHEEL1_INFO*4+usOffset+1]);

    usSpeed = (short)(((double)usSpeed *0.15625)/2);

    return usSpeed;
}

signed short GetVideoFrameHdInfo_CarAngle_10Times(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    unsigned short usAngle = a_pFreamHead[C_VIDEO_FRAME_HD_CAR_ANGLE_LO_BYTE_INDEX] 
    + (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_ANGLE_HI_BYTE_INDEX]<<8);
    signed short sRealAngle10Times=0;

    if (usAngle>32767)
        sRealAngle10Times = usAngle-65536;
    else
        sRealAngle10Times = usAngle;

    return sRealAngle10Times;
}

unsigned char GetVideoFrameHdInfo_CarInfo_CheckSum(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    return a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_CHECKSUM_BYTE_INDEX] ;
}

bool GetVideoFrameHdInfo_CarInfo_IsLeftTurn(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return false;

    if (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX] & 
        (1<<C_CAR_INFO_DRIVING_LEFT_TURN_BIT))
        return true;

    return false;
}

bool GetVideoFrameHdInfo_CarInfo_IsRightTurn(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return false;

    if (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX] & 
        (1<<C_CAR_INFO_DRIVING_RIGHT_TURN_BIT))
        return true;

    return false;
}

bool GetVideoFrameHdInfo_CarInfo_IsReverse(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return false;

    if (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX] & 
        (1<<C_CAR_INFO_DRIVING_REVERSE_BIT))
        return true;

    return false;
}

bool GetVideoFrameHdInfo_CarInfo_IsBreak(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return false;

    if (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX] & 
        (1<<C_CAR_INFO_DRIVING_BREAK_BIT))
        return true;

    return false;
}

unsigned char GetVideoFrameHdInfo_CtrlInfo_GetLDWSEnableSpeedMode(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    return (a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_RESERVE_BYTE_INDEX] & 0x0F);				
}

WORD GetVideoFrameHdInfo_GetICPMode(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0xFFFF;

    WORD *pDataHd = (WORD*)a_pFreamHead;

    return (pDataHd[C_VIDEO_FRAME_HD_ICP_INFO_ICP_MODE_WORD_INDEX]&0xFF);
}

unsigned char GetVideoFrameHdInfo_GetBenchMarkResult(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    return ((a_pFreamHead[C_VIDEO_FRAME_HD_CAR_INFO_DRIVING_BYTE_INDEX]
    & C_CAR_INFO_BENCHMARK_MASK)>>C_CAR_INFO_BENCHMARK_BIT0);
}
bool GetVideoFrameHdInfo_IsWarning_BenchMarkResult(unsigned char *a_pFreamHead)
{
    unsigned char ucTestData = GetVideoFrameHdInfo_GetBenchMarkResult(a_pFreamHead);

    if ((2==ucTestData) || (3==ucTestData))
        return true;

    return false;
}
unsigned char GetVideoFrameHdInfo_GetICPResult_LDWS(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    return a_pFreamHead[C_VIDEO_FRAME_HD_ICP_INFO_ICP_RESULT_BYTE_INDEX];
}

bool GetVideoFrameHdInfo_IsWarning_ICPResult_LDWS(unsigned char *a_pFreamHead)
{	
    unsigned char ucTestData = GetVideoFrameHdInfo_GetICPResult_LDWS(a_pFreamHead);

    if (ucTestData&LDWS_LINE_WARNING_MASK)
        return true;

    return false;
}
unsigned char GetVideoFrameHdInfo_GetICPResult_MOD(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    return a_pFreamHead[C_VIDEO_FRAME_HD_ICP_INFO_ICP_RESULT_BYTE_INDEX+1];
}

bool GetVideoFrameHdInfo_IsWarning_ICPResult_MOD(unsigned char *a_pFreamHead)
{
    unsigned char ucTestData = GetVideoFrameHdInfo_GetICPResult_MOD(a_pFreamHead);

    if (ucTestData&MOD_WARNING_MASK)
        return true;

    return false;
}

signed short GetVideoFrameHdInfo_GetLDWS_LDist(unsigned char *a_pFreamHead)
{
    signed short *psFrameHd;
    if (NULL==a_pFreamHead)
        return 9999;
    psFrameHd = (signed short*)a_pFreamHead;
    return psFrameHd[C_VIDEO_FRAME_HD_LDWS_LDIST_WORD_INDEX];
}
signed short GetVideoFrameHdInfo_GetLDWS_RDist(unsigned char *a_pFreamHead)
{
    signed short *psFrameHd;
    if (NULL==a_pFreamHead)
        return 9999;
    psFrameHd = (signed short*)a_pFreamHead;
    return psFrameHd[C_VIDEO_FRAME_HD_LDWS_RDIST_WORD_INDEX];
}
double GetVideoFrameHdInfo_GetLDWS_LLine_Equ_M(unsigned char *a_pFreamHead)
{
    signed short *psFrameHd;
    if (NULL==a_pFreamHead)
        return 0;
    psFrameHd = (signed short*)a_pFreamHead;
    return (double)(psFrameHd[C_VIDEO_FRAME_HD_LLINE_INFO_M_WORD_INDEX]/(10000.0));
}
double GetVideoFrameHdInfo_GetLDWS_LLine_Equ_C(unsigned char *a_pFreamHead)
{
    signed short *psFrameHd;
    if (NULL==a_pFreamHead)
        return 0;
    psFrameHd = (signed short*)a_pFreamHead;
    return (double)(psFrameHd[C_VIDEO_FRAME_HD_LLINE_INFO_C_WORD_INDEX]/(10.0));
}
double GetVideoFrameHdInfo_GetLDWS_RLine_Equ_M(unsigned char *a_pFreamHead)
{
    signed short *psFrameHd;
    if (NULL==a_pFreamHead)
        return 0;
    psFrameHd = (signed short*)a_pFreamHead;
    return (double)(psFrameHd[C_VIDEO_FRAME_HD_RLINE_INFO_M_WORD_INDEX]/(10000.0));
}
double GetVideoFrameHdInfo_GetLDWS_RLine_Equ_C(unsigned char *a_pFreamHead)
{
    signed short *psFrameHd;
    if (NULL==a_pFreamHead)
        return 0;
    psFrameHd = (signed short*)a_pFreamHead;
    return (double)(psFrameHd[C_VIDEO_FRAME_HD_RLINE_INFO_C_WORD_INDEX]/(10.0));
}

DWORD GetVideoFrameHdInfo_GetPCTimeStamp(unsigned char *a_pFreamHead)
{
    DWORD *pdwFrameHd;
    if (NULL==a_pFreamHead)
        return 0;
    pdwFrameHd = (DWORD*)a_pFreamHead;
    return pdwFrameHd[C_VIDEO_FRAME_HD_FRAME_PC_TIMESTAMP];
}

unsigned char GetVideoFrameHdInfo_GetComBusFrameCnt(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;
    return a_pFreamHead[C_VIDEO_FRAME_HD_ICP_INFO_ICP_FRAME_COUNT_BYTE_INDEX];
}


WORD GetVideoFrameHdInfo_GetSensorBV(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    WORD *pDataHd = (WORD*)a_pFreamHead;

    return pDataHd[C_VIDEO_FRAME_HD_SENSOR_INFO_BV_WORD_INDEX];
}

WORD GetVideoFrameHdInfo_GetSensorGain(unsigned char *a_pFreamHead)
{
    if (NULL==a_pFreamHead)
        return 0;

    WORD *pDataHd = (WORD*)a_pFreamHead;

    return pDataHd[C_VIDEO_FRAME_HD_SENSOR_INFO_GAIN_WORD_INDEX];
}