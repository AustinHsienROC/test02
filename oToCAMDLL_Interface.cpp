//******************************************************************************
//  I N C L U D E     F I L E S
//******************************************************************************
#include "stdafx.h"
#include "CTA_Def.h"

#ifdef _WIN32
    #include <stdlib.h>
    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>
    #include <math.h>
    #include <time.h>
    #include <iostream>
    #include <string>   // for strings
    #include <cassert>  // assert
    #if( ON == PC_FILE_INFO )
        #include <fstream>
    #endif
#endif

#include "OpencvPlug.h"		///< Need Before "CTA_Type.h" due to VARIABLE File
#include "oToCAMDLL_Interface.h"
#include "CTAApi.h"
#include "VideoHdInfo.h"

#define C_CAT_INVALIDATE_DATA		0x7FFFFFFF
//******************************************************************************
// F U N C T I O N S
//******************************************************************************

/**
* @brief FW(RemoteCtrl) know the project Type  
*/
__declspec(dllexport) int __cdecl oTo_GetAlgorithmID(void)
{
    return ALGORITHM_ID; // 3
}

/**
* @brief DebugInfo show on the RemoteCtrl.exe for each cmds
*/
__declspec(dllexport) char* __cdecl oTo_GetFrameInfo(void)
{
    /* old version
    int i=MAX(100-g_FrameReportIdx,0);
    while(i--)
    g_FrameReportIdx+=sprintf(g_FrameReport+g_FrameReportIdx," ");
    */
    return g_FrameReport;
}
/**
* @brief    (for VideoTester.exe) Return specific result data for each Frame
* @n        It will be called after oTo_ImgProcess2.
* @n The function will return information needed by auto test process
* @n caller shall prepare the buffer
* @n a_unFrameResultDataID means what data to get
* @n return value > 0, means return data size
* @n return value ==0, means error happen
*/
__declspec(dllexport) int __cdecl oTo_GetAutoTestFrameInfo(
    unsigned int a_unFrameResultDataID
    , unsigned char * a_pBuf, unsigned int a_unBufSize)
{
    int *pIntBuf = NULL;
    if (NULL == a_pBuf)
        return 0;

	if (0== a_unBufSize )
		return 0;

	if ( (a_unBufSize<sizeof(int)) && (a_unFrameResultDataID>=2) )
		return 0;
    /// ID: [ALL 4bytes data] and only if 0x7FFFFF means in-valid.    
    ///0: CTA_GetResult_Alarm
	///1: CTA_GetResult_isDay
    ///2: CTA_GetResult_ClosedPosL
    ///3: CTA_GetResult_ClosedPosR
    ///4: CTA_GetResult_ClosedSpeedL
    ///5: CTA_GetResult_ClosedSpeedR
    ///6: CTA_GetResult_ClosedTTCL
    ///7: CTA_GetResult_ClosedTTCR
    switch (a_unFrameResultDataID){    
    case 0:
        {
            a_pBuf[0] = CTA_GetResult_Alarm();
            return 1;
        }
    case 1:
        {
            bool ret = CTA_GetResult_isDay();           
            a_pBuf[0] = (ret) ? 0x01 : 0x00;
            return 1;
        }
    case 2:
        {
            int tmp1;
            if (!CTA_GetResult_ClosedPosL(tmp1))  
               tmp1 = C_CAT_INVALIDATE_DATA;
			memcpy(a_pBuf, &tmp1, sizeof(int));
            return sizeof(int);
        }
    case 3:
        {
            int tmp1;
            if (!CTA_GetResult_ClosedPosR(tmp1))
				tmp1 = C_CAT_INVALIDATE_DATA;
			memcpy(a_pBuf, &tmp1, sizeof(int));
            return sizeof(int);
        }
    case 4:
        {
            int tmp1;
            if (!CTA_GetResult_ClosedSpeedL(tmp1))
				tmp1 = C_CAT_INVALIDATE_DATA;
			memcpy(a_pBuf, &tmp1, sizeof(int));
            return sizeof(int);
        }
    case 5:
        {
            int tmp1;
            if (!CTA_GetResult_ClosedSpeedR(tmp1))
				tmp1 = C_CAT_INVALIDATE_DATA;
			memcpy(a_pBuf, &tmp1, sizeof(int));
            return sizeof(int);
        }
    case 6:
        {
            int tmp1;
            if (!CTA_GetResult_ClosedTTCL(tmp1))
				tmp1 = C_CAT_INVALIDATE_DATA;
			memcpy(a_pBuf, &tmp1, sizeof(int));
            return sizeof(int);
        }

	case 7:
            {
            int tmp1;
            if (!CTA_GetResult_ClosedTTCR(tmp1))
				tmp1 = C_CAT_INVALIDATE_DATA;
			memcpy(a_pBuf, &tmp1, sizeof(int));
            return sizeof(int);
        }
    /* not enable now
    case 1:
        if (a_unBufSize < 8)
            return 0;
        pIntBuf = (int*)a_pBuf;
        //			pIntBuf[0] = dbgInfo->leftTireDistance;
        //			pIntBuf[1] = dbgInfo->rightTireDistance;
        return 1;
    */
    }

    return 0;
}

/**
* @brief  The function will return version information needed by auto test process
*/
__declspec(dllexport) int __cdecl oTo_GetVersion()
{
    return C_CTA_DLL_VERSION; //ex. 2015020301
}

/**
* @brief  Return Current Image Resolution
*/
__declspec(dllexport) void __cdecl oTo_GetDisplayImgResolution(int* a_dstImgWidth, int* a_dstImgHeight
    ,int a_SrcImgWidth, int a_SrcImgHeight)
{
    if ( NULL != a_dstImgWidth)
        *a_dstImgWidth = a_SrcImgWidth;

    if ( NULL != a_dstImgHeight)
        *a_dstImgHeight = a_SrcImgHeight;
}

/**
*@brief						Frame Input Start function
*@param[in] src_Y			the YCbCr ImgData
*@param[in] frame_pitch		FrameWidth = frame_pitch/2
*@param[in] FrameHd			Car Info[Speed, angle, FrameCount] from VideoHdInfo.h
*/
__declspec(dllexport) void oTo_ImgProcess2(unsigned char *dst_Y, int dst_img_pitch, int dst_img_height
                                ,unsigned char *src_Y, int frame_pitch, int frame_height
                                ,unsigned int FrameIndex_1Base, unsigned int FrameTimeStamp
                                ,unsigned char *FrameHd)
{
    
    ///FrameHeaderExtract Info Extract
    CTA_UpdateCarDrivingInfo((UINT32) GetVideoFrameHdInfo_TimeStamp(FrameHd)
                             ,(FLOAT) GetVideoFrameHdInfo_CarSpeed_10Times(FrameHd)/(FLOAT)10
                             ,(FLOAT) GetVideoFrameHdInfo_CarAngle_10Times(FrameHd)/(FLOAT)10
                             ,(UINT32)GetVideoFrameHdInfo_FrameCount(FrameHd));

    CTA_ImageProcess( dst_Y, src_Y);


    /// debug info
    if (85 == FrameIndex_1Base)
        printf("hit");
}   

/**
* @brief	 AOK file extension string
*/
__declspec(dllexport) bool __cdecl oTo_GetAOKExtFileName(unsigned int *a_punAlgorithmType, char *a_pAOKExtFileName)
{	
    //only support standard
    return false;
}
//////////////////////////////////////////////////
__declspec(dllexport) bool __cdecl oTo_GetParameterFileName(unsigned int a_unParameterID,char *a_pFileName)
{	
    //Need file name, so it will return true
    if (C_ALGORITHM_DLL_PARAMETER_ID_GET_FILENAME == a_unParameterID)
        return true;

    #pragma region [ID/hex]pair
    switch (a_unParameterID){
    case 0:
        strcpy(a_pFileName, "CustomData.hex");
        return true;
        break;

    case 1:
        strcpy(a_pFileName, "CTA_calib.hex");
        return true;
        break;

    case 2:
        strcpy(a_pFileName, "CTA_TopView1152x120_lut.hex");
        return true;
        break;

    default:
        return false;
    }
    #pragma endregion
    return false;
}
/**
* @brief	 Update the Calibration LUT oTo_SetData will be called after called oTo_WorkBuf_Allocate()
* @param[in] a_unParameterID	Type of LUT(xiToXw or XwtoXi)
* @param[in] a_pData			Input Data
* @param[in] a_pData			Input DataSize
*/
__declspec(dllexport) bool __cdecl oTo_UpdateParameter(unsigned int a_unParameterID
                    , unsigned char * a_pData, unsigned int a_unDataSize)
{
    if ( (NULL==a_pData) || (0==a_unDataSize) )
        return false;

    switch (a_unParameterID){
    case 0:
        CTA_UpdateCarParameter(a_pData, a_unDataSize);
        return TRUE;
        break;

    case 1:
        CTA_Update_LUT(a_unParameterID, a_pData, a_unDataSize);
        return true;
        break;

    case 2:
        CTA_Update_LUT(a_unParameterID, a_pData, a_unDataSize);
        setImgProcROI( WModel->pCTA_Cali );
        return true;
        break;

    case C_ALGORITHM_DLL_PARAMETER_ID_GET_FILENAME:
        memcpy( VideoFilePath ,a_pData, a_unDataSize );
        return true;
        break;

    default:
        return false;
    }

    assert(!"Should not allow here");
    return false;
}

/**
* @brief Initial CTA Buffer at start.
*/
__declspec(dllexport) void __cdecl oTo_WorkBuf_Allocate(void)
{
    CTA_WorkBuf_Allocate(); 
}

/**
* @brief Release Memory
*/
__declspec(dllexport) void oTo_WorkBuf_Release()
{
    CTA_WorkBuf_Release(); ///< API & RemoteCtrl Interface cleave
}

/**
* @brief CTA 軟體Trigger, 
*/
__declspec(dllexport) void oTo_Parameter_Reset()
{  
    CTA_Reset();
}
