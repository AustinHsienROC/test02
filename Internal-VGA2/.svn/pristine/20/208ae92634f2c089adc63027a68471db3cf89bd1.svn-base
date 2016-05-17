/******************************************************************************
 *  (C) Copyright oToBrite Electronic Inc.. 2014 All right reserved.
 *
 *  Confidential Information
 *
 *  All parts of the oToBrite Program Source are protected by copyright law
 *  and all rights are reserved.
 *  This documentation may not, in whole or in part, be copied, photocopied,
 *  reproduced, translated, or reduced to any electronic medium or machine
 *  readable form without prior consent, in writing, from oToBrite.
 ******************************************************************************/

#ifndef __OTOCAMDLL_INTERFACE_H
#define __OTOCAMDLL_INTERFACE_H

//******************************************************************************
// F i r m w a r e   T r i g g e r   O r d e r
//******************************************************************************
/* 
*@Brief     RemoteCtrl Usage
            oTo_WorkBuf_Allocate()			    ///< Memory allocate
            oTo_GetDisplayImgResolution()		///< Return resolution
            oTo_GetAOKExtFileName()
            oTo_GetAlgorithmID()				///< Return ID ex. C_PLUGIN_ALOGRITHM_ID_CTA = 3
            oTo_GetParameterFileName()          ///< Initial the pair[ID,FileName] array before calling oTo_UpdateParameter()
            oTo_UpdateParameter()				///< set TopViewLUT/FileName according AlgorithmID
            while(true)
            {
                oTo_ImgProcess2()				///< Main function
                oTo_GetVersion()			    ///< ex.C_CTA_DLL_VERSION = 2014081400
                oTo_GetFrameInfo()              ///< console debug string
                oTo_GetAutoTestFrameInfo()      
            }
            oTo_WorkBuf_Release()				///< HEAP Free
*@DATE     2015/2/12
*@ref      Algorithm DLL Interface
*/
//******************************************************************************
// M A C R O    D E F I N I T I O N 
//******************************************************************************
//If you want to know video file name, need to return true when 
// oTo_GetParameterFileName() get the ID. And
// You will receive video file name in oTo_UpdateParameter() 
#define C_ALGORITHM_DLL_PARAMETER_ID_GET_FILENAME	101


//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

#ifdef __cplusplus
    extern "C" {
#endif


// It will be called before other functions are called
// You shall allocate your memory and initial your code in the function
__declspec(dllexport) void __cdecl oTo_WorkBuf_Allocate();

//It will be called after all functions will not be called
// You shall free your memory here
__declspec(dllexport) void __cdecl oTo_WorkBuf_Release();

/* not used in CTA
__declspec(dllexport) void __cdecl oTo_SmartPark(unsigned char *dst_Y, unsigned char *dst_Cb, unsigned char *dst_Cr,
                   unsigned char *src_Y, unsigned char *src_Cb, unsigned char *src_Cr,
                   int Y_width, int Cb_width);
*/

// It will be called when you need to know what kind of alogrithm to support by this DLL
// The function will return algorithm id
// return value >= 0, means return correct alogrithm id
// return value < 0, means error happen
__declspec(dllexport) int __cdecl oTo_GetAlgorithmID(void);

// image processing function
// dst_Y : result image that generated from src_Y. the resolution can be not same as src_Y
//			the buffer will be prepare by caller (according oTo_GetDisplayImgResolution())
// dst_img_pitch :  width bytes of result image
// dst_img_height : height of the result image
// src_Y : original image (read from video file, it always is YUV422 format)
// frame_pitch : width bytes of source image
// frame_height : height of the source image
// FrameIndex_1Base : it means nth frame in the video file. It is 1 base, not 0 base
// FrameTimeStamp :  time stamp of the frame is captured
// FrameHd : header of the frame
__declspec(dllexport) void __cdecl oTo_ImgProcess2(unsigned char *dst_Y 
                ,int dst_img_pitch, int dst_img_height
                ,unsigned char *src_Y, int frame_pitch, int frame_height
                ,unsigned int FrameIndex_1Base, unsigned int FrameTimeStamp
                   , unsigned char *FrameHd);

// We will call this function after oTo_WorkBuf_Allocateis called, before oTo_ImgProcess2 is called
// It shall return width/height of dst image
// We will prepare dst image buffer before call oTo_ImgProcess2
__declspec(dllexport) void __cdecl oTo_GetDisplayImgResolution(int* a_dstImgWidth, int* a_dstImgHeight
                                                ,int a_SrcImgWidth, int a_SrcImgHeight);

// It will be called after oTo_ImgProcess2.
// UI will show the return string, only for display
// caller does not free the string
__declspec(dllexport) char* __cdecl oTo_GetFrameInfo(void);

// It will be called after oTo_ImgProcess2.
// UI will show the return buffer, the buffer will be stored
// caller does not free the buffer
//__declspec(dllexport) unsigned char* __cdecl oTo_GetFrameSaveInfo(int* a_nBufferSize);

               
// oTo_UpdateParameter will be called after called oTo_WorkBuf_Allocate(),oTo_GetParameterFileName()
// It will update parameters
// a_unParameterID = 101 is special ID, it means video file name
__declspec(dllexport) bool __cdecl oTo_UpdateParameter(unsigned int a_unParameterID
                    , unsigned char * a_pData, unsigned int a_unDataSize);

// oTo_GetParameterFileName will be called before calle oTo_UpdateParameter()
// It shall be called ay time.
// It will get parameters file name for parameterID
// It shall return a_pFileName[0] = 0 when the ID is invalid
// Caller shall prepare enough buffer (more than or equal 4096 bytes) for a_pFileName
__declspec(dllexport) bool __cdecl oTo_GetParameterFileName(unsigned int a_unParameterID
                    ,char *a_pFileName);

// oTo_GetAOKExtFileName will be called any time
// It shall be called ay time.
// a_punAlgorithmType [Output] => return Algorithm need to run what type AOK file
// a_pAOKExtFileName [Output] => AOK file name shall be standard AOK filename_AOKExtFileName.csv
//													Different AlgorithmType will have different extend file name
// It shall return a_pFileName[0] = 0 when no AOK extend file name
// Caller shall prepare enough buffer (more than or equal 4096 bytes) for a_pAOKExtFileName
// Video tester will call this function to get correct AOK file name.
__declspec(dllexport) bool __cdecl oTo_GetAOKExtFileName(unsigned int *a_punAlgorithmType
                    ,char *a_pAOKExtFileName);

// It will be called after oTo_ImgProcess2.
// The function will return information needed by auto test process
// caller shall prepare the buffer
// a_unFrameResultDataID means what data to get
// return value > 0, means return data size
// return value ==0, means error happen
__declspec(dllexport) int __cdecl oTo_GetAutoTestFrameInfo(unsigned int a_unFrameResultDataID
                    , unsigned char * a_pBuf, unsigned int a_unBufSize);

// It can be called any time
// The function will return version information needed by auto test process
//
__declspec(dllexport) int __cdecl oTo_GetVersion();
                   
#ifdef __cplusplus
}
#endif

#endif //__OTOCAMDLL_INTERFACE_H