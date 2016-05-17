﻿//******************************************************************************
//  I N C L U D E     F I L E S
//******************************************************************************
#if defined(_WIN32)
  #include "stdafx.h"
  #include <stdlib.h>
  #include <malloc.h>
  #include <stdio.h>
  #include <math.h>
  #include <time.h>
  #include <iostream>

  #if( ON == PC_FILE_INFO )
    #include <fstream>
  #endif
  
  #include "OpencvPlug.h"		///< Need Before "CTA_Type.h" due to VARIABLE File
  #include "Filter2D.h"
  #include "CTA_PCLog.h"
  #include "WhiteBox.h"
  #include "OSD_Ycc422.h"
  #include "FilesInfo.h"
  #include "VideoHdInfo.h"

#else // for K2 platform
  #include "oTo_MemMgr.h"
  #include "Utilities.h"
  #include "MemMgr.h"
  #include "CTA_APEX.h"
  #include "ICP_Image_ROI.hpp"
  #include "SmartPark.h"                // [Mark 2015/02/05] for MOD data pattern consistency check between PC and K2 platform  

#endif

#include <string.h>
#include "CTA_Def.h"
#include "CTAApi.h"
#include "CTA_CustomData.h"
#include "Point_Track.h"
#include "Point_Group.h"
#include "ML_NeuralNet.h"
#include "Estimation.h"
#include "WarningDecision.h"
#include "Patches.h"
#include "profile.h"	// [Mark 2015/02/05]

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
#ifdef _WIN32
    #define WIN32_StampTime(idx)  QueryPerformanceCounter( &CTATime.TicCounter[idx++] )
#else // K2 platform
    #define WIN32_StampTime(idx)        // dummy function
#endif

//******************************************************************************
// V A R I A B L E S
//******************************************************************************
//-------------Customer_ROI----------------------------//
static INT16 CTACustROI[4]  ={0,0,0,0};	//cm [ex. 180, 1800, 0, 600]

//-------------------Weather-------------------------//
static INT32 weatherCtn  = 0;
static FLOAT weatherMean = 70;    ///< Start with Day Mode

//-------------------Car-------------------------//
static SimpleCarSpec   carSpec;   
static CAN_Info       *carState;  

//-------------------FrameTimeStamp_FunctionStatus---------------------
static CTATimeManagement CTATime ={ 0 };
static weatherType_t CTAWeather  = Day ;
static INT32 TicIdx              =  0 ;
static TARGET_NUM CTATargetNum   ={ 0,0,0,0,0,0,0 };

//-------------------PointTrack-------------------------//
static HarrisCornerPos  *harrisCorners      = NULL ;
static HarrisCornerPos  *harrisCornersPrev  = NULL ;
static HarrisCornerPos2 *harrisCorners2     = NULL ;
static HarrisCornerPos2 *harrisCorners2Prev = NULL ;

//-------------------Img/DataBuf-------------------------//
static INT32  g_unTotalRequestSize    = 0;    ///< All Memory Request size
CONST  UINT16 g_TH                   = 14000; ///< Harris Thd of R
CONST  UCHAR  g_e_value              = 1;
CONST  UCHAR  g_ThdBinary            = 100;
CONST  UCHAR  g_THD_Binary           = 220;   ///< image binarilization
CONST  UCHAR  g_THD_SobelEdge        = 10;    ///< threshold for vSobelQty16x16 and hSobelQty16x16s
static FLOAT  g_car_speed_constraint = 1.2;   ///< car speed constraint
static UCHAR  g_dif_th;

static UCHAR *g_Sobel_img             = NULL;
static UCHAR *g_Sobel_imgPrev         = NULL;
static UCHAR *g_vSobel_img            = NULL;
static UCHAR *g_hSobel_img            = NULL;

#ifdef _WIN32
    static UCHAR  *g_dst_YCbCr        = NULL;   ///< YCbCr Img
    static UCHAR  *g_SrcY             = NULL;   ///< Y     Img
    static UINT16 *g_harrisValue      = NULL;
#endif

static UCHAR *g_Gaussian_img          = NULL;   ///< src Img + Gaussian
static UCHAR *g_Gaussian_imgPrev      = NULL;
static UCHAR *g_harrisMap             = NULL;
static UCHAR *g_harrisMapPrev         = NULL;
static UCHAR *g_img_FrameDiff         = NULL;

//-------[NightModeUsedOnly]--------
static UCHAR *g_bin_img               = NULL;   ///< Binary Image
static UCHAR *g_bin_PrevImg           = NULL;   ///< Binary Image previously

static UINT16 *CTAMinorBlockL_EdgeQuantity;
static UINT16 *CTAMinorBlockR_EdgeQuantity;

static UINT16 *g_VSobelQty16x16 = NULL;
static UINT16 *g_HSobelQty16x16 = NULL;

// g_bCopySrcImg: true means copy source image to dst image at first,false means do nothing
static BOOL g_bCopySrcImg = true;

#ifndef _WIN32 // for K2 platform
  static UCHAR  *g_dst_YCbCr = NULL;   // dummy varible in K2 platform, only for parameter passing in function verify_XXXX()
  static ICP_Image *g_picp_src_Y = NULL;
    
#ifdef SYNC_ACROSS_PLATFORM
  #define IsForkApexComplete(a_unFunPtr)		// dummy function
  typedef void (*ApexFun)(void);        

    void ForkApex(uint32 a_unFunPtr)
    {    
        ((ApexFun) a_unFunPtr)();
    }
  #define WaitForkApexComplete(a_unFunPtr)	// dummy function
#else
  extern signed char IsForkApexComplete(uint32 a_unFunPtr);
  extern LIB_RESULT ForkApex(uint32 a_unFunPtr);
  extern bool WaitForkApexComplete(uint32 a_unFunPtr);
#endif


  static ICP_Image gicp_Sobel;
  static ICP_Image gicp_SobelPrev;
  static ICP_Image gicp_vSobel;
  static ICP_Image gicp_hSobel;
  //static ICP_Image gicp_SobelEroded;
  //static ICP_Image gicp_SobelErodedPrev;
    //static ICP_Image gicp_SrcY;
  static ICP_Image gicp_Gaussian;
  static ICP_Image gicp_GaussianPrev;  
  static ICP_Image gicp_harrisMap;
  static ICP_Image gicp_harrisMapPrev;  
  static ICP_Image gicp_FrameDiff;
  static ICP_Image gicp_bin;
  static ICP_Image gicp_binPrev;
  static ICP_Image gicp_CTAMinorBlockL_EdgeQuantity;
  static ICP_Image gicp_CTAMinorBlockR_EdgeQuantity;
  static ICP_Image gicp_HSobelQty16x16;
  static ICP_Image gicp_VSobelQty16x16;
  
  static ICP_Image gicp_roi_Sobel;
  static ICP_Image gicp_roi_SobelPrev;
  static ICP_Image gicp_roi_vSobel;
  static ICP_Image gicp_roi_hSobel;
  //static ICP_Image gicp_roi_SobelEroded;
  //static ICP_Image gicp_roi_SobelErodedPrev;
  static ICP_Image gicp_roi_SrcY;
  static ICP_Image gicp_roi_left;
  static ICP_Image gicp_roi_right;
  
  static ICP_Image gicp_roi_Gaussian;
  static ICP_Image gicp_roi_GaussianPrev;  
  static ICP_Image gicp_roi_harrisMap;
  static ICP_Image gicp_roi_harrisMapPrev;  
  static ICP_Image gicp_roi_FrameDiff;
  static ICP_Image gicp_roi_bin;
  static ICP_Image gicp_roi_binPrev;
  
  static ICP_Image *pROI_Gaussian        = &gicp_roi_Gaussian;    
  static ICP_Image *pROI_GaussianPrev    = &gicp_roi_GaussianPrev;
  static ICP_Image *pROI_Sobel           = &gicp_roi_Sobel;
  static ICP_Image *pROI_SobelPrev       = &gicp_roi_SobelPrev;
  static ICP_Image *pROI_vSobel          = &gicp_roi_vSobel;
  static ICP_Image *pROI_hSobel          = &gicp_roi_hSobel;
  //static ICP_Image *pROI_SobelEroded     = &gicp_roi_SobelEroded;
  //static ICP_Image *pROI_SobelErodedPrev = &gicp_roi_SobelErodedPrev;    
  static ICP_Image *pROI_harrisMap       = &gicp_roi_harrisMap;
  static ICP_Image *pROI_harrisMapPrev   = &gicp_roi_harrisMapPrev;  
  static ICP_Image *pROI_FrameDiff       = &gicp_roi_FrameDiff;
  static ICP_Image *pROI_bin             = &gicp_roi_bin;
  static ICP_Image *pROI_binPrev         = &gicp_roi_binPrev;
#endif // #ifndef _WIN32 // for K2 platform

//-------------------Block/Candidates-------------------------//
static MinorBlock   CTAMinorBlockL		[ MAXCandidateNum ];
static MinorBlock   CTAMinorBlockR		[ MAXCandidateNum ];
       CandidateRec g_CTACandidateL	    [ MAXCandidateNum ];
       CandidateRec g_CTACandidateR	    [ MAXCandidateNum ];
static CandidateRec g_CTACandidatePrevL	[ MAXCandidateNum ];
static CandidateRec g_CTACandidatePrevR	[ MAXCandidateNum ];
//-------------------HeadLight-------------------------//
static HeadLight lampL     [ MAX_HeadLightNum ];
static HeadLight lampR     [ MAX_HeadLightNum ];
static HeadLight lampPrevL [ MAX_HeadLightNum ];
static HeadLight lampPrevR [ MAX_HeadLightNum ];

INT32  RegionL[4];
INT32  RegionR[4]; 
UCHAR  CTAAlarmState   =  0;
UCHAR  CTAAlarmTrigger =  0;

#ifdef _WIN32
    int  g_FrameReportIdx = 0;     ///< index of g_FrameReport
    char g_FrameReport[500];       ///< show information to debug UI< oTo_GetFrameInfo()
    char VideoFilePath[240];       ///< VideoFileName(ex.20150207_16_56_41_772.YUV)

    //-------------------CTA_Log-------------------------//
    BOOL EventResultL [ MAXSizeEventResult ] = {0};
    BOOL EventResultR [ MAXSizeEventResult ] = {0};
#endif

//******************************************************************************
// I N T E R N A L   F U N C T I O N S
//****************************************************************************** 
/**
* @brief huge memory Allocation
*/
static void* CTA_calloc(UINT32 a_unDataCnt, UINT32 a_unDataSize)
{
    g_unTotalRequestSize += (a_unDataCnt*a_unDataSize);
    #ifdef _WIN32
        void *p = NULL;
        if ( ! a_unDataCnt || (p = (void *) calloc(a_unDataCnt, a_unDataSize)) )
            return p;

        WIN32_ASSERT(!"NoMem");
        return 0;
    #else
        return oTo_MemMgr_Allocate(a_unDataCnt * a_unDataSize);
    #endif
}

/**
* @brief huge memory Release
*/
static void CTA_free(void* a_pBuf)
{
    #ifdef _WIN32	
        if(NULL == a_pBuf) 
            return;
        return free( a_pBuf );
    #else
        return oTo_MemMgr_Release(a_pBuf) ;
    #endif
}

/**
* @brief [PC] Processed ROI(RegionL/RegionR)[Img] and CTACustROI[World]
* @param[in] i_LUT	image 2 World LUT
*/
void setImgProcROI( CTA_CaliResult_t *i_LUT )
{
    RegionL[0]   = 0;
    RegionL[2]   = RegionL[0] + (ROISegBlockNum_H * ROISegBlockH) ;
    RegionR[0]   = (SrcImg_W) - (ROISegBlockNum_H * ROISegBlockH) ;
    RegionR[2]   = (SrcImg_W) ;

    INT32 tmpx , tmpy;
    INT32 endy;
    WModel->PosWorldToImg( (CTA_TV_WIDTH>>1) + ((carSpec.CarWidth_cm>>1)+100)/CTA_TV_COMPRESS, CTA_TV_HEIGHT-1, tmpx, tmpy);
    endy = tmpy;
    WModel->PosWorldToImg( (CTA_TV_WIDTH>>1) - ((carSpec.CarWidth_cm>>1)+100)/CTA_TV_COMPRESS, CTA_TV_HEIGHT-1, tmpx, tmpy);
    endy = MAX(endy, tmpy ) + 1;
    
    INT32 starty = endy - ROISegBlockNum_V * ROISegBlockV ;
    RegionL[3] = RegionR[3] = endy;
    RegionL[1] = RegionR[1] = starty;

    /// Customer Boundary
    CTACustROI[0] = (carSpec.CarWidth_cm >>1) + 100;
    CTACustROI[1] = (carSpec.CarWidth_cm >>1) + 1000;
    CTACustROI[2] = 0;
    CTACustROI[3] = 600;

    #ifndef _WIN32    // [Mark 2015/02/02] for K2 platform
    if (RegionL[1] < 0 ||  RegionL[1] >= SrcImg_H || RegionL[3] < 0 || RegionL[3] >= SrcImg_H || RegionL[1] >= RegionL[3])
    {
        oTo_DEBUG_printf("CTA ROI error, calibration data may be incorrect.\n");
        while (1) ; // dummy loop
    }

    if (0 != ((RegionL[3] - RegionL[1])%16)) {
        oTo_DEBUG_printf("CTA ROI error, calibration data may be incorrect.\n");
        while (1) ; // dummy loop
    }

    LIB_RESULT lib_ret;
    ICP_Image_ROI roi; 
    
    roi.xOffset = 0;
    roi.yOffset = RegionL[1];
    roi.width = SrcImg_W;
    roi.height = RegionL[3] - RegionL[1];

    if (false == gicp_roi_Gaussian.IsEmpty()) {  // already created
        gicp_roi_Gaussian.Destroy();
        lib_ret = gicp_roi_Gaussian.Create(&gicp_Gaussian, &roi, 0);
    }

    if (false == gicp_roi_GaussianPrev.IsEmpty()) { // already created
        gicp_roi_GaussianPrev.Destroy();
        lib_ret = gicp_roi_GaussianPrev.Create(&gicp_GaussianPrev, &roi, 0);
    }

    if (false == gicp_roi_Sobel.IsEmpty()) { // already created
        gicp_roi_Sobel.Destroy();
        lib_ret = gicp_roi_Sobel.Create(&gicp_Sobel, &roi, 0);
    }

    if (false == gicp_roi_SobelPrev.IsEmpty()) { // already created
        gicp_roi_SobelPrev.Destroy();
        lib_ret = gicp_roi_SobelPrev.Create(&gicp_SobelPrev, &roi, 0);
    }

    if (false == gicp_roi_harrisMap.IsEmpty()) { // already created
        gicp_roi_harrisMap.Destroy();
        lib_ret = gicp_roi_harrisMap.Create(&gicp_harrisMap, &roi, 0);        
    }

    if (false == gicp_roi_harrisMapPrev.IsEmpty()) {  // already created
        gicp_roi_harrisMapPrev.Destroy();
        lib_ret = gicp_roi_harrisMapPrev.Create(&gicp_harrisMapPrev, &roi, 0);
        
    }

    if (false == gicp_roi_vSobel.IsEmpty()) {    // already created
        gicp_roi_vSobel.Destroy();
        lib_ret = gicp_roi_vSobel.Create(&gicp_vSobel, &roi, 0);
    }

    if (false == gicp_roi_hSobel.IsEmpty()) {   // already created
        gicp_roi_hSobel.Destroy();
        lib_ret = gicp_roi_hSobel.Create(&gicp_hSobel, &roi, 0);
    }
    
    if (false == gicp_roi_FrameDiff.IsEmpty()) {
        gicp_roi_FrameDiff.Destroy();
        lib_ret = gicp_roi_FrameDiff.Create(&gicp_FrameDiff, &roi, 0);
    }

    if (false == gicp_roi_bin.IsEmpty()) {
        gicp_roi_bin.Destroy();
        lib_ret = gicp_roi_bin.Create(&gicp_bin, &roi, 0);
    }
    
    if (false == gicp_roi_binPrev.IsEmpty()) {
        gicp_roi_binPrev.Destroy();
    lib_ret = gicp_roi_binPrev.Create(&gicp_binPrev, &roi, 0);
    }    
    #endif // END #ifndef _WIN32
}

/**
* @brief [K2] Processed ROI(RegionL/RegionR) assignment.
*/
void setImgProcROI( void *a_i_LUT )
{
    setImgProcROI((CTA_CaliResult_t*)a_i_LUT);
}

/**
*@brief		[PC] Show the basic UI information 
*@n         1.[Frame Numbers] [Boundary]
*@n         2.[CandidateInfoTemplate][TXT][TopView][ROI]
*/
#if defined(_WIN32)
static void OSD_PLOT_GeneralUI( UCHAR *dst_Y ,INT32 imgDimensionX,INT32 imgDimensionY, INT32 &in_FrameNum , SimpleCarSpec &carSpec, INT16 i_CTACustROI[4] )
{
    ///1) Show Frame Numbers
    OSD_Ycc422::OSD_SetColor(255, 255, 255);
    OSD_Ycc422::OSD_Draw_Text((const char*)"Frame" , dst_Y, imgDimensionX, imgDimensionY, 0, 0, 0);
    OSD_Ycc422::OSD_Draw_INT32( in_FrameNum , dst_Y, imgDimensionX, imgDimensionY,  10, 24, 3 ,10);

    ///2) Show Boundary(0-6m,3-15m)
    OSD_Ycc422::OSD_SetColor( 255 , 255 , 255 );

    INT32 Start_Xcm = (CTA_TV_WIDTH>>1)*CTA_TV_COMPRESS - i_CTACustROI[1];
    INT32 End_Xcm   = (CTA_TV_WIDTH>>1)*CTA_TV_COMPRESS - i_CTACustROI[0] ;

    INT32 Start_Ycm = i_CTACustROI[2];     // Customer Spec 
    INT32 End_Ycm   = i_CTACustROI[3] - 1 ;

    /// 1) Left Side
    INT32 xi,yi;
    for ( INT32 hori = Start_Xcm/CTA_TV_COMPRESS ; hori < End_Xcm/CTA_TV_COMPRESS; ++hori )
    {
        /// top
        WModel->PosWorldToImg( hori, Start_Ycm/CTA_TV_COMPRESS ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y , imgDimensionX, imgDimensionY, xi, yi );
        /// bottom
        WModel->PosWorldToImg( hori, End_Ycm/CTA_TV_COMPRESS ,xi,yi);   //119
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y , imgDimensionX, imgDimensionY, xi, yi );

    }
    for ( INT32 vert = Start_Ycm/CTA_TV_COMPRESS ; vert < End_Ycm/CTA_TV_COMPRESS; ++vert)
    {
        /// left
        WModel->PosWorldToImg( Start_Xcm/CTA_TV_COMPRESS, vert ,xi,yi);
        //WModel->PosWorldToImg( 276, vert ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y, imgDimensionX, imgDimensionY, xi, yi );
        /// right
        WModel->PosWorldToImg( End_Xcm/CTA_TV_COMPRESS, vert ,xi,yi);
        //WModel->PosWorldToImg( 516, vert ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y, imgDimensionX, imgDimensionY, xi, yi );
    }

    /// 2) Right Side
    Start_Xcm = (CTA_TV_WIDTH>>1)*CTA_TV_COMPRESS + (carSpec.CarWidth_cm>>1)+100;
    End_Xcm   = Start_Xcm + 900;
    for ( INT32 hori = Start_Xcm/CTA_TV_COMPRESS ; hori < End_Xcm/CTA_TV_COMPRESS; ++hori)
    {
        /// top
        WModel->PosWorldToImg( hori, Start_Ycm/CTA_TV_COMPRESS ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y, imgDimensionX,  imgDimensionY, xi, yi );
        /// bottom
        WModel->PosWorldToImg( hori, End_Ycm/CTA_TV_COMPRESS ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y, imgDimensionX, imgDimensionY, xi, yi );
    }
    for ( INT32 vert = Start_Ycm/CTA_TV_COMPRESS ; vert < End_Ycm/CTA_TV_COMPRESS; ++vert)
    {
        /// left
        WModel->PosWorldToImg( Start_Xcm/CTA_TV_COMPRESS, vert ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y, imgDimensionX, imgDimensionY, xi, yi );
        /// right
        WModel->PosWorldToImg( End_Xcm/CTA_TV_COMPRESS, vert ,xi,yi);
        OSD_Ycc422::OSD_Draw_Pixel( dst_Y, imgDimensionX, imgDimensionY, xi, yi );
    }

    //OSD_Show_Boundary( dst_Y , imgDimensionX , imgDimensionY ,carSpec, i_CTACustROI );

    /* @note the Special UI

    ///4) Show CandidateInfoTemplate at Left-Bottom
    OSD_SetColor(255, 255, 0);
    OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,5,400-10-5,110,470-40,1,false);
//    OSD_Draw_Text( "Stable" , dst_Y, imgDimensionX, imgDimensionY, 0, 400-25-6, 0);
    OSD_Draw_Text( "POSXooom" , dst_Y, imgDimensionX, imgDimensionY, 0, 415-25-6-4, 0);
    OSD_Draw_Text( "SPEEDookph" , dst_Y, imgDimensionX, imgDimensionY, 0, 430-25-6-4, 0);
    OSD_Draw_Text( "TTCoooosec" , dst_Y, imgDimensionX, imgDimensionY, 0, 445-25-6-4, 0);
    */

    ///5) Plot Motion Estimation ROI
    OSD_Ycc422::OSD_SetColor(155, 155, 155);
    OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, RegionL[0], RegionL[1], RegionL[2], RegionL[3],3,FALSE ,1);
    OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, RegionR[0], RegionR[1], RegionR[2], RegionR[3],3,FALSE, 1);

    ///6) EXPORT TXT
    #if ( ON == DBG_TXT)
    //FILE *fptr;
    //errno_t err=fopen_s(&fptr,"c:\\test\\CTA_output.txt","a");
    //fprintf(fptr,"idx(FrameNum)=%d, TH(Harris)=%d, PointNum=%d \n", FrameNum, TH, pointNumber);
    //fclose(fptr);
    #endif

    //5) OpenCV show TopView
    #if (( ON == OPENCV_EN ) && (ON == SHOW_BV)) 
    UCHAR BVImage[CTA_TV_WIDTH*CTA_TV_HEIGHT];
    INT32 xi,yi;

    for ( int vert = 0 ; vert < CTA_TV_HEIGHT;++vert)
        for ( int hori=0 ; hori < CTA_TV_WIDTH;++hori)
        {
            WModel->PosWorldToImg( hori, vert , xi, yi );
            BVImage[ hori + vert*CTA_TV_WIDTH ] = dst_Y[ xi + imgDimensionX * yi ];
        }

    OPENCV->ShowImg(BVImage,CTA_TV_WIDTH,CTA_TV_HEIGHT);
    #endif
}
#endif // #if defined(_WIN32)

/** This function is called only at end of each frame
*@brief		save the Image & struct data for next frame estimation purpose.
*@param[in] g_Gaussian_img	Gaussian image
*@param[in] g_Sobel_img		Gradient sobel image
*@param[in] harrisCorners	Harris corners structure
*@param[in] CTACandidateL	Objects Info.
*@param[in] FrameNum		Frame Number
*/
void Update_Data_Before_Next_Frame( CandidateRec *CTACandidateL, CandidateRec *CTACandidatePrevL, CandidateRec *CTACandidateR, CandidateRec *CTACandidatePrevR, TARGET_NUM &inCTATarget, CTATimeManagement &CTATime ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    CTATime.FrameNum++;

    /// Prev [Image/ g_Sobel_imgPrev ] Frame Saving
    UCHAR *p_tmp;
    p_tmp              = g_Gaussian_imgPrev;
    g_Gaussian_imgPrev = g_Gaussian_img;
    g_Gaussian_img     = p_tmp;

    p_tmp              = g_Sobel_imgPrev;
    g_Sobel_imgPrev    = g_Sobel_img;
    g_Sobel_img        = p_tmp;

    /// Prev  Harris Corners Frame
    HarrisCornerPos *p_tmp2;
    p_tmp2            = harrisCornersPrev;
    harrisCornersPrev = harrisCorners;
    harrisCorners     = p_tmp2;


    HarrisCornerPos2 *p_tmp3;
    p_tmp3             = harrisCorners2Prev;
    harrisCorners2Prev = harrisCorners2;
    harrisCorners2     = p_tmp3;
#if 1 // cause different from Rev605_Mark if define 0
    memset(harrisCorners2 , 0 , sizeof(HarrisCornerPos2)*MAXSizeHarrisCorner );
#endif

    p_tmp           = g_harrisMapPrev;
    g_harrisMapPrev = g_harrisMap;
    g_harrisMap     = p_tmp;

#ifndef _WIN32  // for K2 platform
    ICP_Image *picp_tmp;

    picp_tmp           = pROI_GaussianPrev;
    pROI_GaussianPrev = pROI_Gaussian;
    pROI_Gaussian     = picp_tmp;
    
    picp_tmp           = pROI_SobelPrev;
    pROI_SobelPrev    = pROI_Sobel;
    pROI_Sobel        = picp_tmp;

    picp_tmp            = pROI_harrisMapPrev;
    pROI_harrisMapPrev = pROI_harrisMap;
    pROI_harrisMap     = picp_tmp;
#endif // END #ifndef _WIN32  // for K2 platform

    CTATime.fpsPrev              = CTATime.fps;
    inCTATarget.FeaturePtNumPrev = inCTATarget.FeaturePtNum;

    /// reset previous CTACandidate
    memset(&CTACandidatePrevL[0] , 0 , inCTATarget._L_MaxCandNumPrev * sizeof(CandidateRec) );
    memset(&CTACandidatePrevR[0] , 0 , inCTATarget._R_MaxCandNumPrev * sizeof(CandidateRec) );
        
    INT32 counter,i;
    for( counter = i = 0; i < inCTATarget._L_MaxCandNum; i++) {
        if( Invalid != CTACandidateL[i].isValid ) {
            memcpy( &CTACandidatePrevL[counter] , &CTACandidateL[i] , sizeof(CandidateRec) );
            CTACandidatePrevL[counter].relatedCandLink      = 0 ;
            CTACandidatePrevL[counter++].isRelatedCandValid = FALSE ;
        }
    }
    inCTATarget._L_MaxCandNumPrev = counter ;

    for( counter= i=0; i<inCTATarget._R_MaxCandNum; i++) {
        if( Invalid != CTACandidateR[i].isValid) {
            memcpy( &CTACandidatePrevR[counter] , &CTACandidateR[i] , sizeof(CandidateRec) );
            CTACandidatePrevR[counter  ].relatedCandLink    = 0 ;
            CTACandidatePrevR[counter++].isRelatedCandValid = FALSE ;
        }
    }
    inCTATarget._R_MaxCandNumPrev = counter;

    ///0) Clear Candidates
    memset(&CTACandidateL[0] , 0 , inCTATarget._L_MaxCandNum * sizeof(CTACandidatePrevL[0]) );
    memset(&CTACandidateR[0] , 0 , inCTATarget._R_MaxCandNum * sizeof(CTACandidatePrevR[0]) );
    inCTATarget._L_MaxCandNum = 0;
    inCTATarget._R_MaxCandNum = 0;

}

/**
*@brief	    [NightMode)] save the Image & struct data for next frame estimation purpose.
*/
static void Update_Data_Before_Next_Frame_Night( HeadLight *lampL, HeadLight *lampR, HeadLight *lampPrevL,HeadLight *lampPrevR, TARGET_NUM &CTATargetNum,INT32 &inFrameNum, INT32 imgDimensionX , INT32 imgDimensionY )
{
    ++inFrameNum;

    /// Prev [Image] Frame Saving
    UCHAR *tmp1;
    tmp1               = g_Gaussian_imgPrev;
    g_Gaussian_imgPrev = g_Gaussian_img;
    g_Gaussian_img     = tmp1;

    tmp1               = g_bin_PrevImg;
    g_bin_PrevImg      = g_bin_img;
    g_bin_img          = tmp1;

    //INT32 size_image = imgDimensionX*imgDimensionY*sizeof( UCHAR );
    //memcpy( Gaussian_imgPrev , Gaussian_img , size_image );
    //memcpy( bin_Previmg      , bin_img      , size_image );

    /// HeadLight Restore and Reset
    INT32 size_HeadLight = sizeof(HeadLight) * MAX_HeadLightNum;
    
    HeadLight *tmp2;
    tmp2      = lampPrevL;
    lampL     = lampPrevL;
    lampPrevL = tmp2;
    tmp2      = lampPrevR;
    lampR     = lampPrevR;
    lampPrevR = tmp2;
    // old version
    //memcpy(lampPrevL , lampL , size_HeadLight );
    //memcpy(lampPrevR , lampR , size_HeadLight );

    memset(lampL , 0  , size_HeadLight );
    memset(lampR , 0  , size_HeadLight );

    /// Target Number
    CTATargetNum._L_MaxCandNumPrev = CTATargetNum._L_MaxCandNum;
    CTATargetNum._R_MaxCandNumPrev = CTATargetNum._R_MaxCandNum;
}

/**
*@brief					Return the DayNightMode by average means in block
*@param[in] dst_Y		YCbCr ImgData
*@param[in] isDay		the current weatherType_t mode
*@return                decision the weatherType_t mode
*/
static weatherType_t GetWheatherType(UCHAR *dst_Y, INT32 imgDimensionX,INT32 imgDimensionY , INT32 inFrameNum , weatherType_t isDay)
{
    assert("Advance: Using ML for regions color histogram");

    const  INT32 THD_DetectInterval = 5*1;  ///< 5 Sec
    const  INT32 THD_DaytoNight     = 40;   ///< graylevel = 30
    const  INT32 THD_NighttoDay     = 55;   ///< graylevel = 30

    const  FLOAT SmoothRatio     = 0.1;   ///< (SmoothRatio)*MeanNow + ( 1- SmoothRatio)MeanPrev

    /// block position
    const INT16 CenterWidth  = 74; ///< SrcImg_W/10; 
    const INT16 CenterHeight = 52; ///<SrcImg_W/10; 
    const INT16 startRow     = (imgDimensionY>>1) + 150;
    const INT16 startCol     = (imgDimensionX>>1) -(CenterWidth>>1); 

//    static INT32 weatherCtn    = 0;
//    static FLOAT weatherMean = 70;    ///< Start with Day Mode

    weatherType_t ret = isDay;
    
    if( ABS((weatherCtn-inFrameNum)) < THD_DetectInterval ){ return ret; }
    
    {
        weatherType_t retWeather = Day;
        UCHAR imgCenter[CenterWidth * CenterHeight] = {0};
        weatherCtn = inFrameNum;

        /// 1) image copy to imgCenter
        for ( INT32 vert = 0; vert < CenterHeight; ++vert)
        {
            for (INT32 hori = 0; hori < CenterWidth; ++hori)
            {
                imgCenter[hori + vert*CenterWidth] = dst_Y[(  startCol + hori + imgDimensionX*(startRow + vert) )*2];
            }
        }

        ///2) Get Mean
        FLOAT mean = 0;
        INT32 imgCenterSize = (CenterWidth*CenterHeight);
        for( INT32 i1 = 0 ; i1 < imgCenterSize ; ++i1)
            mean += imgCenter[i1];
        mean /= (FLOAT)imgCenterSize;

        ///3) Smooth Ratio
        weatherMean = SmoothRatio*(mean) + (1-SmoothRatio) * weatherMean;

        ///3) Decision Making
        if( 0 == (CTATargetNum._L_CandNum + CTATargetNum._R_CandNum) )
        {
            if( (Day == isDay)&&( THD_DaytoNight >weatherMean  ) )
            {
                ret = Night;
            }
            else if ( (Night == isDay) &&( THD_NighttoDay <weatherMean ) )
            {
                ret = Day;
            }
            return ret;
        }
        else /// Candidates Tracking
        {
            return ret;
        }
        assert(!"Cannot goto here");
    }

}

static void CTA_ImageProcess_06Day();
static void CTA_ProcessNight();

//******************************************************************************
// F U N C T I O N S
//******************************************************************************
/**
* @brief         Find First Idx ObjRect
* @param[in,out] a_sObjIndexs   start to find out object index. 1st time, it shall be 0, if find object, it will be changed to next start to search position
* @param[in]     a_bLeftSide	true means to find out left side object, false means to find out right side object
* @return        true means find object, and a_start_x...a_end_y are validate
* @n             false means not find object,and a_start_x...a_end_y are invalidate
*/
BOOL CTA_FindObjRect(BOOL a_bLeftSide, INT16& a_sObjIndex
                        , INT16& a_start_x, INT16& a_start_y
                        , INT16& a_end_x, INT16& a_end_y)
{
    CandidateRec *pResultData = NULL;
    short MaxObjNum           = 0;
    INT16 i1;

    if (a_bLeftSide)
        pResultData = CTA_GetResult_TargetCarsL(MaxObjNum);
    else
        pResultData = CTA_GetResult_TargetCarsR(MaxObjNum);
    
    //no object
    if ( (0>=MaxObjNum) || (0>a_sObjIndex) || (a_sObjIndex>=MaxObjNum))
        return FALSE;
    
    //search 1st obj	
    for ( i1 = a_sObjIndex, a_sObjIndex = MaxObjNum; i1 < MaxObjNum ; ++i1 )
    {		
        if( TRUE == pResultData[i1].isAlarm )
        {
            a_sObjIndex = i1;
            break;
        }
    }
    
    if (a_sObjIndex == MaxObjNum)
    {
        a_sObjIndex = -1;
        return FALSE;
    }

    a_start_x = pResultData[a_sObjIndex].startxi;
    a_start_y = pResultData[a_sObjIndex].startyi;
    a_end_x   = pResultData[a_sObjIndex].startxi+pResultData[a_sObjIndex].width; 
    a_end_y   = pResultData[a_sObjIndex].startyi+pResultData[a_sObjIndex].height;
        
    //move to next start search position
    a_sObjIndex++;

    return TRUE;
}

/**
* @brief         FindObjFrontLine
* @param[in,out] a_sObjIndex    start to find out object index. 1st time, it shall be 0, if find object, it will be changed to next start to search position
* @param[in]     a_bLeftSide	true means to find out left side object, false means to find out right side object
* @return        true means find object, and a_ObjDist is  validate
* @n             false means not find object,and a_ObjDist is invalidate
*/
BOOL CTA_FindObjFrontLine(BOOL a_bLeftSide, INT16& a_sObjIndex
                        , INT32& a_ObjDist)
{
    CandidateRec *pResultData = NULL;
    short MaxObjNum           = 0;
    INT16 i1;

    if (a_bLeftSide)
        pResultData = CTA_GetResult_TargetCarsL(MaxObjNum);
    else
        pResultData = CTA_GetResult_TargetCarsR(MaxObjNum);
    
    //no object
    if ( (0>=MaxObjNum) || (0>a_sObjIndex) || (a_sObjIndex>=MaxObjNum))
        return FALSE;
    
    //search 1st obj	
    for (i1 = a_sObjIndex, a_sObjIndex = MaxObjNum; i1 < MaxObjNum ; ++i1)
    {		
        if( Valid == pResultData[i1].isValid )
        {
            if( pResultData[i1].isObjPosxyValid && pResultData[i1].isObjPosXYValid)
            {
                a_sObjIndex = i1;
                break;
            }
        }
    }
    
    if (a_sObjIndex == MaxObjNum)
    {
        a_sObjIndex = -1;
        return FALSE;
    }

    a_ObjDist = pResultData[a_sObjIndex].ObjPosXY[0];

    //move to next start search position
    a_sObjIndex++;
    return true;
}

void CTA_DrawObjFrontLine(CTA_DrawObjFrontLine_CB a_pCbFun, INT32 a_ObjDist)
{
    INT32 tmpxi,tmpyi,tmpxi2,tmpyi2;
    CONST INT32 iv1 = 50 ; //cm
    INT32 count1    = iv1;

    if (NULL==a_pCbFun)
        return;

    for ( INT32 Yw = (CTA_TV_HEIGHT * CTA_TV_COMPRESS -1) ; Yw > 1 ; --Yw)
    {
        count1 = (count1<=-iv1)?iv1:(count1-1);
        if( count1 < 0 )continue;
        WModel->PosWorldToImg( INV_XwShiftToBackCenter(a_ObjDist)/CTA_TV_COMPRESS ,Yw/CTA_TV_COMPRESS, tmpxi,tmpyi );
        WModel->PosWorldToImg( INV_XwShiftToBackCenter(a_ObjDist)/CTA_TV_COMPRESS ,(Yw-1)/CTA_TV_COMPRESS, tmpxi2,tmpyi2 );
        a_pCbFun( tmpxi, tmpyi, tmpxi2, tmpyi2);
    }
}

/**
* @brief     [k2/PC] Global Variable CTA Buffer Allocate at start.
*/
void CTA_WorkBuf_Allocate(void) 
{
    INT32 img_width      = SrcImg_W ;
    INT32 img_height     = SrcImg_H ;
    INT32 img_size       = img_width * img_height ;
    g_unTotalRequestSize = 0;                       ///< Reset heap counter

    carState			     = (CAN_Info *)CTA_calloc(sizeof(CAN_Info), 1);
#ifdef _WIN32
    WModel->pCTA_Cali        = (CTA_CaliResult_t *)CTA_calloc(sizeof(CTA_CaliResult_t), 1 );
    WModel->pCTA_LUT_TOPVIEW = (UINT32 *)CTA_calloc(sizeof(UINT32), CTA_TV_WIDTH*CTA_TV_HEIGHT);    
    
    g_harrisValue            = (UINT16 *)CTA_calloc(sizeof(UINT16), img_size);
    g_SrcY                   = (UCHAR  *)CTA_calloc(sizeof(UCHAR) , img_size);

#endif
    
    g_harrisMap            = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_harrisMapPrev        = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_Sobel_img            = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_vSobel_img           = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_hSobel_img           = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);

    g_Gaussian_img         = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_Gaussian_imgPrev     = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    harrisCorners          = (HarrisCornerPos  *) CTA_calloc(sizeof(HarrisCornerPos) , MAXSizeHarrisCorner);
    harrisCornersPrev      = (HarrisCornerPos  *) CTA_calloc(sizeof(HarrisCornerPos) , MAXSizeHarrisCorner);
    harrisCorners2         = (HarrisCornerPos2 *) CTA_calloc(sizeof(HarrisCornerPos2), MAXSizeHarrisCorner);
    harrisCorners2Prev     = (HarrisCornerPos2 *) CTA_calloc(sizeof(HarrisCornerPos2), MAXSizeHarrisCorner);        
    
    g_Sobel_imgPrev        = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_img_FrameDiff        = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_bin_img              = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    g_bin_PrevImg          = (UCHAR *) CTA_calloc(sizeof(UCHAR), img_size);
    
    CTAMinorBlockL_EdgeQuantity = (UINT16 *) CTA_calloc(sizeof(UINT16), MAXCandidateNum);
    CTAMinorBlockR_EdgeQuantity = (UINT16 *) CTA_calloc(sizeof(UINT16), MAXCandidateNum);

    g_HSobelQty16x16 = (UINT16 *) CTA_calloc(sizeof(UINT16), ROISegBlockNum_H*2*ROISegBlockNum_V);
    g_VSobelQty16x16 = (UINT16 *) CTA_calloc(sizeof(UINT16), ROISegBlockNum_H*2*ROISegBlockNum_V);    
    
#ifndef _WIN32 // for K2 platform
    ICP_Image_Descriptor dpr;
    LIB_RESULT lib_ret;
        
    dpr.Configure(img_width, img_height, ICP_COLOR_GRAYSCALE);
    lib_ret = gicp_Gaussian.Create(&dpr, g_Gaussian_img);    
    lib_ret = gicp_GaussianPrev.Create(&dpr, g_Gaussian_imgPrev);   
    lib_ret = gicp_Sobel.Create(&dpr, g_Sobel_img);    
    lib_ret = gicp_SobelPrev.Create(&dpr, g_Sobel_imgPrev);
    lib_ret = gicp_vSobel.Create(&dpr, g_vSobel_img);
    lib_ret = gicp_hSobel.Create(&dpr, g_hSobel_img);    
    //lib_ret = gicp_SobelEroded.Create(&dpr, g_SobelEroded_img);
    //lib_ret = gicp_SobelErodedPrev.Create(&dpr, g_SobelEroded_imgPrev);    
    lib_ret = gicp_harrisMap.Create(&dpr, g_harrisMap);
    lib_ret = gicp_harrisMapPrev.Create(&dpr, g_harrisMapPrev);
    lib_ret = gicp_FrameDiff.Create(&dpr, g_img_FrameDiff);
    lib_ret = gicp_bin.Create(&dpr, g_bin_img);
    lib_ret = gicp_binPrev.Create(&dpr, g_bin_PrevImg);
    
    dpr.Configure(ROISegBlockNum_H, ROISegBlockNum_V, ICP_COLOR_INT16U(1));
    lib_ret = gicp_CTAMinorBlockL_EdgeQuantity.Create(&dpr, CTAMinorBlockL_EdgeQuantity);
    lib_ret = gicp_CTAMinorBlockR_EdgeQuantity.Create(&dpr, CTAMinorBlockR_EdgeQuantity);

    dpr.Configure(ROISegBlockNum_H*2, ROISegBlockNum_V, ICP_COLOR_INT16U(1));
    lib_ret = gicp_HSobelQty16x16.Create(&dpr, g_HSobelQty16x16);
    lib_ret = gicp_VSobelQty16x16.Create(&dpr, g_VSobelQty16x16);
    
    ICP_Image_ROI roi; 
    roi.xOffset = 0;
    roi.yOffset = RegionL[1];
    roi.width = SrcImg_W;
    roi.height = RegionL[3] - RegionL[1];

    lib_ret = gicp_roi_Gaussian.Create(&gicp_Gaussian, &roi, 0);
    lib_ret = gicp_roi_GaussianPrev.Create(&gicp_GaussianPrev, &roi, 0);    
    lib_ret = gicp_roi_Sobel.Create(&gicp_Sobel, &roi, 0);
    lib_ret = gicp_roi_SobelPrev.Create(&gicp_SobelPrev, &roi, 0);
    lib_ret = gicp_roi_vSobel.Create(&gicp_vSobel, &roi, 0);
    lib_ret = gicp_roi_hSobel.Create(&gicp_hSobel, &roi, 0);
    lib_ret = gicp_roi_harrisMap.Create(&gicp_harrisMap, &roi, 0);
    lib_ret = gicp_roi_harrisMapPrev.Create(&gicp_harrisMapPrev, &roi, 0);
    lib_ret = gicp_roi_FrameDiff.Create(&gicp_FrameDiff, &roi, 0);
    lib_ret = gicp_roi_bin.Create(&gicp_bin, &roi, 0);
    lib_ret = gicp_roi_binPrev.Create(&gicp_binPrev, &roi, 0);
    
    pROI_Sobel           = &gicp_roi_Sobel;
    pROI_SobelPrev       = &gicp_roi_SobelPrev;
    pROI_vSobel          = &gicp_roi_vSobel;
    pROI_hSobel          = &gicp_roi_hSobel;
    //pROI_SobelEroded     = &gicp_roi_SobelEroded;
    //pROI_SobelErodedPrev = &gicp_roi_SobelErodedPrev;
    pROI_Gaussian        = &gicp_roi_Gaussian;
    pROI_GaussianPrev    = &gicp_roi_GaussianPrev;
    pROI_harrisMap       = &gicp_roi_harrisMap;
    pROI_harrisMapPrev   = &gicp_roi_harrisMapPrev;  
    pROI_FrameDiff       = &gicp_roi_FrameDiff;
    pROI_bin             = &gicp_roi_bin;
    pROI_binPrev         = &gicp_roi_binPrev;       
#endif // #ifndef _WIN32 // for K2 platform    

    /// reset global stack
    CTA_Reset();
}

/**
* @brief	 [k2/PC] Update the Calibration LUT 
             -----------------+-------------------------------------------------------+------------
              a_unParameterID  |   DESCRIPTION  					
             ------------------+-------------------------------------------------------+------------
              1                | [ Image   to TopView ]  LUT   	
             ------------------+-------------+-----------------------------------------+------------
              2                | [ TOPView to Image   ]  LUT 
             ------------------+-------------------------------------------------------+------------
* @param[in] a_unParameterID	Type of LUT(xiToXw or XwtoXi)
* @param[in] a_pData			Input Data
* @param[in] a_pData			Input DataSize
*/
bool CTA_Update_LUT(UINT32 a_unParameterID, UCHAR * a_pData, UINT32 a_unDataSize)
{
#ifdef _WIN32    
    if ( 1 == a_unParameterID )
    {
        UINT32 deb01 = sizeof( CTA_CaliResult_t );//923532
        if( a_unDataSize != sizeof(CTA_CaliResult_t)){ assert(!"dead in oTo_UpdateParameter"); return false; }
        if( NULL == WModel->pCTA_Cali ){ assert(!"dead in oTo_UpdateParameter"); return false; }
        
        memcpy( WModel->pCTA_Cali , a_pData, sizeof(CTA_CaliResult_t) ); ///< img to TOPView LUT
        
    }
    else if(2 == a_unParameterID)
    {
        UINT32 deb01 = sizeof(UINT32) * ( CTA_TV_WIDTH * CTA_TV_HEIGHT ); // 552960 = 138240*4(UINT32) = 576*2 * 120 *4
        if( a_unDataSize != ( sizeof(UINT32) *  CTA_TV_WIDTH * CTA_TV_HEIGHT )){ assert(!"dead in oTo_UpdateParameter"); return false; }
        if( NULL == WModel->pCTA_LUT_TOPVIEW ){ assert(!"dead in oTo_UpdateParameter"); return false; }
        
        memcpy( WModel->pCTA_LUT_TOPVIEW ,a_pData, sizeof(UINT32) * CTA_TV_WIDTH * CTA_TV_HEIGHT );///< TOPView to img LUT
  
    }

    #if defined(SYNC_ACROSS_PLATFORM)
        #ifdef DATA_PATTERN_FOUNTAINHEAD 
            if ( 1 == a_unParameterID ) 
            {
                LoadLUT_Img2World(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_LUT_Img2World.DAT"), WModel->pCTA_Cali);
            }
            if ( 2 == a_unParameterID ) 
            {
                LoadLUT_World2Img(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_LUT_World2Img.DAT"), WModel->pCTA_LUT_TOPVIEW);
                setImgProcROI( WModel->pCTA_Cali );
            }
        #else
            if ( 1 == a_unParameterID ) // [ Image to TopView ]  LUT   	
                DumpLUT_Img2World(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_LUT_Img2World.DAT"), WModel->pCTA_Cali);
            if ( 2 == a_unParameterID )
                DumpLUT_World2Img(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_LUT_World2Img.DAT"), WModel->pCTA_LUT_TOPVIEW);
        #endif
    #endif

#else // K2 platform
    if ( 1 == a_unParameterID )
    {
        WModel->pCTA_Cali =  (CTA_CaliResult_t*)a_pData;
        
        #ifdef SYNC_ACROSS_PLATFORM
            #ifdef DATA_PATTERN_FOUNTAINHEAD 
                LoadLUT_Img2World(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_LUT_Img2World.DAT"), WModel->pCTA_Cali);
            #endif
        #endif        
    }
    else if(2 == a_unParameterID)
    {
        WModel->pCTA_LUT_TOPVIEW = (UINT32*)a_pData;

        #ifdef SYNC_ACROSS_PLATFORM
            #ifdef DATA_PATTERN_FOUNTAINHEAD 
                LoadLUT_World2Img(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_LUT_World2Img.DAT"), WModel->pCTA_LUT_TOPVIEW);
            #endif
        #endif

        setImgProcROI( WModel->pCTA_Cali );
    }
#endif

    return true;
}

/**
* @brief	 [PC] Update the Basic Car Info; thus setImgProcROI()
*/
bool CTA_UpdateCarParameter(UCHAR * a_pData, UINT32 a_unDataSize)  // (oTo_UpdateParameter) LUT, Custom Car data
{
    if( a_unDataSize != sizeof(CustomData1_st) ){ assert(!"dead in oTo_UpdateParameter"); return false;}

    CustomData1_st *pCustomData = (CustomData1_st*) a_pData;

    carSpec.CarWidth_cm             = (UINT32)pCustomData->usCarWidth;             ///< about Benson Car: 171cm
    carSpec.RearTireToBumper_cm     = (UINT32)pCustomData->usRearTireToBumper_cm;  ///< about 100cm
    carSpec.SteerOverFrontTireRatio = (DOUBLE)pCustomData->unSTEER_OVER_FRONT_TIRE_Numerator / (DOUBLE)pCustomData->unSTEER_OVER_FRONT_TIRE_Denominator; ///< about 15.6
    carSpec.WheelBase_cm            = (UINT32)pCustomData->usWheelBase_cm;         ///< about 260cm

#ifdef SYNC_ACROSS_PLATFORM 
    #ifdef DATA_PATTERN_FOUNTAINHEAD
        LoadCarInfo(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_carSpec.DAT"),&carSpec);
    #else
        DumpCarInfo(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_carSpec.DAT"),&carSpec);
    #endif
#endif

    setImgProcROI( WModel->pCTA_Cali );  // [Mark 2015/02/05] needed since  setImgProcROI need carSpec in win32 platform
    return true;
}
/**
* @brief	 [K2] Update the Basic Car Info
*/
bool CTA_UpdateCarParameter(UINT32 a_CarWidth_cm, UINT32 a_RearTireToBumper_cm, DOUBLE a_SteerOverFrontTireRatio, UINT32 a_WheelBase_cm)
{
    carSpec.CarWidth_cm             = a_CarWidth_cm;             ///< about Benson Car: 171cm
    carSpec.RearTireToBumper_cm     = a_RearTireToBumper_cm;  ///< about 100cm
    carSpec.SteerOverFrontTireRatio = a_SteerOverFrontTireRatio;//(DOUBLE)pCustomData->unSTEER_OVER_FRONT_TIRE_Numerator / (DOUBLE)pCustomData->unSTEER_OVER_FRONT_TIRE_Denominator; ///< about 15.6
    carSpec.WheelBase_cm            = a_WheelBase_cm;         ///< about 260cm

#ifdef SYNC_ACROSS_PLATFORM 
    #ifdef DATA_PATTERN_FOUNTAINHEAD
        LoadCarInfo(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_carSpec.DAT"), &carSpec);
    #else
        DumpCarInfo(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_carSpec.DAT"), &carSpec);
    #endif
#endif
    return true;
}
/**
* @brief	 [K2/PC] Update the Car state[carState], and CTATime
*/
void CTA_UpdateCarDrivingInfo(UINT32 a_FrameTimeStamp_ms, FLOAT a_CraSpeed, FLOAT a_SteerAngle, UINT32 a_FrameCnt)
{
     ///1) [TimeStamp / FrameHeaderExtract] Info Extract
        static DOUBLE lastTime, currentTime; ///< Time(ms) at the Video 
        DOUBLE timeInterval;				 ///< Frame Interval when Recorder[sec] (ex. 0.032)
        currentTime           = (DOUBLE)a_FrameTimeStamp_ms;
        carState->time_stamp   = (UINT32)a_FrameTimeStamp_ms;
        carState->carSpeed     = a_CraSpeed;
        carState->steerAngle   = a_SteerAngle;
        carState->frameCounter = a_FrameCnt;
        if( CTATime.FrameNum > 0 )
        {
            timeInterval           = (currentTime>lastTime)?( ( currentTime - lastTime )*0.001 ):(( currentTime+(DOUBLE)4294967295 - lastTime  )*0.001 );	//Unit:(sec)
            carState->timeInterval = (currentTime>lastTime)?( ( currentTime - lastTime ) )      :(( currentTime+(DOUBLE)4294967295 - lastTime  )       );	//Unit:(ms)
            CTATime.fps            = PRECISION_2( ((FLOAT)1000/(FLOAT)carState->timeInterval) );
        }
        lastTime = currentTime;
}

/**
*@brief	 [K2/PC] APEX Day Pre-Filter
*/
void CTA_ImageProcess_02APEX_Day(void)
{
#ifdef _WIN32
    QueryPerformanceFrequency(&CTATime.cpuFreq);
    TicIdx = 0;
    WIN32_StampTime(TicIdx);

    #pragma region ============Image_PreProcess_&_GetFeaturePoints============
    /// Gaussian Image[ g_Gaussian_img ]
    ImgFilter->Apex_Process_0_Gaussian( g_Gaussian_img, g_SrcY, SrcImg_W, SrcImg_H );

    /// Sobel image [ g_Sobel_img , g_vSobel_img , g_hSobel_img]
    ImgFilter->Apex_Process_1_0_SobelErosion(g_Sobel_img, g_vSobel_img, g_hSobel_img, g_Gaussian_img, SrcImg_W, SrcImg_H);

    // Frame Difference [ g_img_FrameDiff ]
    // [Mark 2014/02/06] temporary disabled part of code for firmware release, need furthuer verfication
    //if (carState->carSpeed < g_car_speed_constraint)
    //    ImgFilter->FLT_ABSDiff( g_img_FrameDiff,  g_Gaussian_img , g_Gaussian_imgPrev, g_dif_th ,imgDimensionX, imgDimensionY);
    //else
        ImgFilter->FLT_ABSDiff( g_img_FrameDiff,  g_Sobel_img , g_Sobel_imgPrev, g_ThdBinary ,SrcImg_W, SrcImg_H);
    // END [Mark 2014/02/06] temporary disabled part of code for firmware release, need furthuer verfication
    
    /// Frame Difference [ g_img_FrameDiff ]
   // ImgFilter->FLT_ABSDiff( g_img_FrameDiff,  g_Sobel_img , g_Sobel_imgPrev, g_ThdBinary ,SrcImg_W, SrcImg_H);

    /// Binarilize Image of V channel of HSV [ g_bin_img ]    
    ImgFilter->ImageThreshold_01( g_bin_img, g_Gaussian_img, SrcImg_W, SrcImg_H, g_THD_Binary );

    /// ==VERIFY== ABS Diff of Sobel Image with PURPLE COLOR
    verify_FLT_ABSDiff(g_dst_YCbCr , SrcImg_W , SrcImg_H , g_img_FrameDiff,  g_Sobel_img , g_Sobel_imgPrev );
 
    #pragma endregion    
#else // for K2 platform
    ICP_Image_ROI roi;    
    LIB_RESULT lib_ret;
    
    roi.xOffset = 0;
    roi.yOffset = RegionL[1];
    roi.width = SrcImg_W;
    roi.height = RegionL[3] - RegionL[1];

    lib_ret = gicp_roi_SrcY.Create(g_picp_src_Y, &roi, 0);
    
    APEX_CTA_DayProcess_Start(&gicp_roi_SrcY, pROI_Gaussian, pROI_Sobel, pROI_SobelPrev, 
                              pROI_hSobel, pROI_vSobel, pROI_FrameDiff, g_ThdBinary
    #if _ENABLE_APEX_CTA_BINIMG_OUT_
                              , pROI_bin, g_THD_Binary
    #endif // END #if _ENABLE_APEX_CTA_BINIMG_OUT_
                              );
    
    APEX_CTA_DayProcess_Wait();
    gicp_roi_SrcY.Destroy();
#endif
}

void CTA_ImageProcess_03APEX_CntEdgeQty(void)
{
    WIN32_StampTime( TicIdx );
#ifdef _WIN32
    ImgFilter->CntQty16x16(CTAMinorBlockL_EdgeQuantity, g_img_FrameDiff, SrcImg_W, SrcImg_H, 
                           RegionL[0], RegionL[1], RegionL[2], RegionL[3], 0);
    
    ImgFilter->CntQty16x16(CTAMinorBlockR_EdgeQuantity, g_img_FrameDiff, SrcImg_W, SrcImg_H, 
                           RegionR[0], RegionR[1], RegionR[2], RegionR[3], 0);            

    ImgFilter->CntQty16x16(g_HSobelQty16x16, g_hSobel_img, SrcImg_W, SrcImg_H,
                           0, RegionL[1], SrcImg_W, RegionL[3], g_THD_SobelEdge); 
    ImgFilter->CntQty16x16(g_VSobelQty16x16, g_vSobel_img, SrcImg_W, SrcImg_H,
                           0, RegionL[1], SrcImg_W, RegionL[3], g_THD_SobelEdge ); 

    WIN32_StampTime( TicIdx );  
  
#else // for K2 platform
    ICP_Image_ROI roi;    
    LIB_RESULT lib_ret;
    
    roi.yOffset = RegionL[1];
    roi.height = RegionL[3] - RegionL[1];
    roi.xOffset = 0;
    roi.width = SrcImg_W/2;
    lib_ret = gicp_roi_left.Create(&gicp_FrameDiff, &roi, 0);
    
    roi.xOffset = SrcImg_W/2;
    lib_ret = gicp_roi_right.Create(&gicp_FrameDiff, &roi, 0);

    APEX_CTA_CntEdgeQty16x16_Start(&gicp_roi_left, &gicp_CTAMinorBlockL_EdgeQuantity, &gicp_roi_right, &gicp_CTAMinorBlockR_EdgeQuantity);
    APEX_CTA_CntEdgeQty16x16_Wait();
        
    gicp_roi_left.Destroy();
    gicp_roi_right.Destroy();

    roi.xOffset = 0;
    roi.yOffset = RegionL[1];
    roi.width = SrcImg_W;
    roi.height = RegionL[3] - RegionL[1];

    APEX_CTA_CntSobelQty16x16_Start(pROI_hSobel, &gicp_HSobelQty16x16, pROI_vSobel, &gicp_VSobelQty16x16, g_THD_SobelEdge);
    APEX_CTA_CntSobelQty16x16_Wait();
#endif
}

/**
*@brief      Back-door
*/
void CTA_EnableCopySourceImg(BOOL a_bEnableCopy)
{
    g_bCopySrcImg = a_bEnableCopy;
}

/**
*@brief      [PC] Prepare threshold or some parameters before process
*@param[in]  i_src_YCbCr	 the YCbCr ImgData
*@param[out] i_dst_YCbCr	 the YCbCr ImgData
*/
#ifdef _WIN32
static void CTA_ImageProcess_01Ini( UCHAR *i_dst_YCbCr, UCHAR *i_src_YCbCr )
{
    g_dst_YCbCr = i_dst_YCbCr;

    #pragma region    ============PLOT_GeneralUI============
    ///1) Copy src_Y[YCbCr] to g_dst_YCbCr[YCbCr].
    if(g_bCopySrcImg)
        memcpy(g_dst_YCbCr , i_src_YCbCr , SrcImg_W * SrcImg_H * 2);

    ///2) Extract Y channel from src_Y(YCbCr) to g_SrcY(Y)
    OSD_Ycc422::Extract_Y( g_SrcY, i_src_YCbCr, SrcImg_W, SrcImg_H );

    ///3)Additional UI for debug[show TopView/Boundary(3-15m)/Frame Numbers/ Export TXT]
    OSD_PLOT_GeneralUI( g_dst_YCbCr ,SrcImg_W , SrcImg_H , CTATime.FrameNum, carSpec, CTACustROI );

    #pragma endregion

    /// ==VERIFY== Plot carSpeed/steerAngle
    verify_SrcImg_Header(g_dst_YCbCr, SrcImg_W, SrcImg_H,
                         g_SrcY, SrcImg_W, SrcImg_H, carState, CTATime, CTATargetNum,g_Gaussian_imgPrev,g_Sobel_imgPrev,g_harrisMapPrev,harrisCornersPrev,g_CTACandidatePrevL,g_CTACandidatePrevR,
                         CTATargetNum.FeaturePtNumPrev);

    /// [Mark 2015/02/02] to adjust threshold to constrain number of harris corner points
    g_dif_th = (CTATargetNum.FeaturePtNumPrev > 160)?(32):\
              ((CTATargetNum.FeaturePtNumPrev < 80)?(8):\
               (8 + (32 - 8)*(CTATargetNum.FeaturePtNumPrev-80)/(160-80)));
}
#else
/** [K2] Prepare threshold or some parameters before process*/
static void CTA_ImageProcess_01Ini(ICP_Image *picp_src_Y)
{
    UCHAR *g_SrcY = NULL;
    g_picp_src_Y = picp_src_Y;
    g_SrcY = (UCHAR *)picp_src_Y->ReturnPlanePtrRW(0);

    /// ==VERIFY== Plot carSpeed/steerAngle
    verify_SrcImg_Header(g_dst_YCbCr, SrcImg_W, SrcImg_H,
                         g_SrcY, SrcImg_W, SrcImg_H, carState, CTATime, CTATargetNum,g_Gaussian_imgPrev,g_Sobel_imgPrev,g_harrisMapPrev,harrisCornersPrev,g_CTACandidatePrevL,g_CTACandidatePrevR,
        CTATargetNum.FeaturePtNumPrev);

    /// [Mark 2015/02/02] to adjust threshold to constrain number of harris corner points
    g_dif_th = (CTATargetNum.FeaturePtNumPrev > 160)?(32):\
              ((CTATargetNum.FeaturePtNumPrev < 80)?(8):\
               (8 + (32 - 8)*(CTATargetNum.FeaturePtNumPrev-80)/(160-80)));
}
#endif

void CTA_ImageProcess_04WheatherType(void)
{
#ifndef _WIN32  // for K2 platform
    UCHAR *g_dst_YCbCr = NULL;  // dummy variable in K2 platform
#endif

    WIN32_StampTime( TicIdx );

    //CTAWeather = GetWheatherType(g_SrcY, SrcImg_W,SrcImg_H , CTATime.FrameNum , CTAWeather);
    CTAWeather = Day;//Night;

    WIN32_StampTime( TicIdx );
    /// ==VERIFY== Plot Weather Information
    verify_WeatherType( g_dst_YCbCr , SrcImg_W, SrcImg_H, CTAWeather );

}

static void CTA_ImageProcess_05Run(void)
{
    if( Day == CTAWeather)
    {
       CTA_ImageProcess_06Day();
    }
    else if( Night == CTAWeather )
    {
       CTA_ProcessNight();
    }
}

void CTA_ImageProcess_90TimeSpend(void)
{

#ifndef _WIN32  // for K2 platform
        UCHAR *g_dst_YCbCr = NULL;  // dummy variable in K2 platform
#endif

    verify_TimeCost( g_dst_YCbCr , SrcImg_W ,SrcImg_H, CTATime , TicIdx );

/*@note: debug info
    printf("TargetNo: FP:%d, FPP:%d, L (%d, M:%d, MP:%d), R (%d, M:%d, MP:%d)\r\n"
            ,CTATargetNum.FeaturePtNum, CTATargetNum.FeaturePtNumPrev
            ,CTATargetNum._L_CandNum, CTATargetNum._L_MaxCandNum,CTATargetNum._L_MaxCandNumPrev
            ,CTATargetNum._R_CandNum, CTATargetNum._R_MaxCandNum,CTATargetNum._R_MaxCandNumPrev
            );
*/
}

/**
*@brief      [k2/PC] Main
*@param[in]  src_YCbCr	 the YCbCr ImgData
*@param[out] dst_YCbCr	 the YCbCr ImgData
*/
#ifdef _WIN32
void CTA_ImageProcess(UCHAR *dst_YCbCr, UCHAR *src_YCbCr) 
{	

    CTA_ImageProcess_01Ini( dst_YCbCr, src_YCbCr);

#else // K2 platform
static UINT16 *g_harrisValue = NULL;	// dummy pointer in K2
void CTA_ImageProcess(ICP_Image *picp_src_Y)
{    
    CTA_ImageProcess_01Ini(picp_src_Y);
    Profile_Reset();    

#endif    
    
#ifdef _WIN32
    CTA_ImageProcess_02APEX_Day();
#else // for K2 platform
    //oTo_DEBUG_printf("CTA_ImageProcess_02APEX_Day start");
    ForkApex( (uint32)CTA_ImageProcess_02APEX_Day );	
    WaitForkApexComplete( (uint32)CTA_ImageProcess_02APEX_Day );

#endif
      
#ifdef _WIN32
    CTA_ImageProcess_03APEX_CntEdgeQty();

#else
    //oTo_DEBUG_printf("CTA_ImageProcess_02APEX_Day end");
    //oTo_DEBUG_printf("CTA_ImageProcess_03APEX_CntEdgeQty start");
    ForkApex( (uint32)CTA_ImageProcess_03APEX_CntEdgeQty );
    WaitForkApexComplete( (uint32)CTA_ImageProcess_03APEX_CntEdgeQty );

#endif
             
    CTA_ImageProcess_04WheatherType();

    CTA_ImageProcess_05Run();

#ifdef _WIN32
    CTA_ImageProcess_90TimeSpend();
#else
    Profile_PrintReslult();
#endif
    
}

/**
* @brief  [k2/PC] trigger to reset stack, 
*/
void CTA_Reset(void)
{
    CTAAlarmState   = 0x00; 
    CTAAlarmTrigger = 0x00;
    memset(g_CTACandidateL,     0, sizeof(CandidateRec) * MAXCandidateNum );
    memset(g_CTACandidateR,     0, sizeof(CandidateRec) * MAXCandidateNum );
    memset(g_CTACandidatePrevL, 0, sizeof(CandidateRec) * MAXCandidateNum );
    memset(g_CTACandidatePrevR, 0, sizeof(CandidateRec) * MAXCandidateNum );

    memset(lampL,     0, sizeof(HeadLight) * MAX_HeadLightNum );
    memset(lampR,     0, sizeof(HeadLight) * MAX_HeadLightNum );
    memset(lampPrevL, 0, sizeof(HeadLight) * MAX_HeadLightNum );
    memset(lampPrevR, 0, sizeof(HeadLight) * MAX_HeadLightNum );
    
    memset(carState  , 0, sizeof(CAN_Info) );
    memset(&CTATime  , 0, sizeof(CTATimeManagement) );
    CTAWeather = Day;

    memset(&CTATargetNum       , 0, sizeof(TARGET_NUM) );
    memset( harrisCorners      , 0, sizeof(HarrisCornerPos) * MAXSizeHarrisCorner );
    memset( harrisCornersPrev  , 0, sizeof(HarrisCornerPos) * MAXSizeHarrisCorner );
    memset( harrisCorners2     , 0, sizeof(HarrisCornerPos2) * MAXSizeHarrisCorner );
    memset( harrisCorners2Prev , 0, sizeof(HarrisCornerPos2) * MAXSizeHarrisCorner );

    memset( CTAMinorBlockL , 0, sizeof(MinorBlock) * MAXCandidateNum );
    memset( CTAMinorBlockR , 0, sizeof(MinorBlock) * MAXCandidateNum );

    weatherCtn  = 0;
    weatherMean = 70;    
}

/**
* @brief [k2/PC] Release Memory
*/
void CTA_WorkBuf_Release(void)
{ 
#ifdef _WIN32
    if( NULL != WModel->pCTA_Cali )
        { CTA_free(WModel->pCTA_Cali); WModel->pCTA_Cali = NULL ;}
    if( NULL != WModel->pCTA_LUT_TOPVIEW )
        { CTA_free(WModel->pCTA_LUT_TOPVIEW); WModel->pCTA_LUT_TOPVIEW = NULL ;}
    
    /// XML File Export.("C:\\CTA_Log\\CTA_Log.xml")
    CTALog->ExportCTALog_ALARM(VideoFilePath , EventResultL , EventResultR , MAXSizeEventResult);

    CTA_free(g_Sobel_img);                  g_Sobel_img         = NULL;
    CTA_free(g_vSobel_img);                 g_vSobel_img        = NULL;
    CTA_free(g_hSobel_img);                 g_hSobel_img        = NULL;
    CTA_free(g_Gaussian_img);               g_Gaussian_img      = NULL;
    CTA_free(g_SrcY);                       g_SrcY              = NULL;
    CTA_free(g_Gaussian_imgPrev);           g_Gaussian_imgPrev  = NULL;
    CTA_free(g_harrisValue);                g_harrisValue       = NULL;
    CTA_free(g_harrisMap);                  g_harrisMap         = NULL;
    CTA_free(g_harrisMapPrev);              g_harrisMapPrev     = NULL;

    CTA_free(CTAMinorBlockL_EdgeQuantity);  CTAMinorBlockL_EdgeQuantity = NULL;
    CTA_free(CTAMinorBlockR_EdgeQuantity);  CTAMinorBlockR_EdgeQuantity = NULL;
    CTA_free(g_HSobelQty16x16);             g_HSobelQty16x16            = NULL;
    CTA_free(g_VSobelQty16x16);             g_VSobelQty16x16            = NULL;

    //-----[NightMode]-----
    CTA_free( g_bin_img );                  g_bin_img           = NULL;
    CTA_free( g_bin_PrevImg );              g_bin_PrevImg       = NULL;
#else // for K2 platform
    // destroy ROI image
    gicp_roi_Gaussian.Destroy();
    gicp_roi_GaussianPrev.Destroy();
    gicp_roi_Sobel.Destroy();
    gicp_roi_SobelPrev.Destroy();
    gicp_roi_vSobel.Destroy();
    gicp_roi_hSobel.Destroy();
    gicp_roi_harrisMap.Destroy();
    gicp_roi_harrisMapPrev.Destroy();
    gicp_roi_FrameDiff.Destroy();
    gicp_roi_bin.Destroy();
    gicp_roi_binPrev.Destroy();

    // do not free allocated memory used in ICP_Image, since ICP_Image::Destory() will destory allocated memory automatically    
    gicp_Gaussian.Destroy();
    gicp_GaussianPrev.Destroy();
    gicp_Sobel.Destroy();
    gicp_SobelPrev.Destroy();
    gicp_vSobel.Destroy();
    gicp_hSobel.Destroy();
    //gicp_SobelEroded_img.Destroy();
    //gicp_SobelEroded_imgPrev.Destroy();        
    gicp_harrisMap.Destroy();
    gicp_harrisMapPrev.Destroy();
    gicp_FrameDiff.Destroy();
    gicp_bin.Destroy();
    gicp_binPrev.Destroy();   

    gicp_CTAMinorBlockL_EdgeQuantity.Destroy();
    gicp_CTAMinorBlockR_EdgeQuantity.Destroy();

    gicp_HSobelQty16x16.Destroy();
    gicp_VSobelQty16x16.Destroy();
#endif
    CTA_free( harrisCorners);       harrisCorners       = NULL;
    CTA_free( harrisCornersPrev);   harrisCornersPrev   = NULL;
    CTA_free( harrisCorners2);      harrisCorners2      = NULL;
    CTA_free( harrisCorners2Prev);  harrisCorners2Prev  = NULL;
    CTA_free( carState );           carState            = NULL;

    WModel          ->ResetInstance();
    CTACanEstimation->ResetInstance();
#ifdef _WIN32    
    ImgFilter       ->ResetInstance();
#endif
    CTAPatch        ->ResetInstance();
    Cluster         ->ResetInstance();
    TRACK           ->ResetInstance();
    CTADog          ->ResetInstance();
    CNN             ->ResetInstance();
}


//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
///////////////////////////////////////////////////
void CTA_ImageProcess_07PointTracking(void)
{
    WIN32_StampTime( TicIdx );
    Profile_CTA_Start(e_SetPara);
    // p_th_array[i]
    // 0 : if SAD > the value, moving is too large
    // 1 : if Diff(SAD, original point)< the value, assign moving value
    // 2 : if SAD < the value, moving is too small
    CONST INT32 p_th_array[3]   = { 200<<2 , -999 , -999 }; ///< Threshold [0] targetTH [1] sad_diff_TH [2] zero_motion_sad_TH
    CONST INT32 MATCH_SHIFT_BIT = 0;                        ///< ">>MATCH_SHIFT_BIT
    TRACK->setPara( p_th_array , MATCH_SHIFT_BIT , RegionL , RegionR );

    Profile_CTA_End(e_SetPara);

    Profile_CTA_Start(e_CornersMotionEstimation);
    TRACK->CornersMotionEstimation( harrisCornersPrev, harrisCorners2Prev, g_Gaussian_img, g_Gaussian_imgPrev, SrcImg_W, CTATargetNum.FeaturePtNumPrev);

    Profile_CTA_End(e_CornersMotionEstimation);

    Profile_CTA_Start(e_PointTrackPatch);        
    TRACK->PointTrackPatch(harrisCornersPrev, harrisCorners2Prev, g_Gaussian_img, SrcImg_W, carSpec, carState,
                           CTATargetNum.FeaturePtNumPrev);    
    Profile_CTA_End(e_PointTrackPatch);

    WIN32_StampTime( TicIdx );
    /// ==VERIFY== PLOT Optical flow[Yellow]/ speed compensation flow[Red]
    verify_MotionCorner(g_dst_YCbCr , SrcImg_W , SrcImg_H , harrisCornersPrev, harrisCorners2Prev, RegionL ,RegionR, 
                        CTATargetNum.FeaturePtNumPrev);

}
///////////////////////////////////////////////////
void CTA_ImageProcess_08PointGrouping(void)
{
    WIN32_StampTime( TicIdx );
    Profile_CTA_Start(e_MinorBlockUpdate);
    // THD_MinorBlockUpdate[i1]
    // 0 : THD_MotionEdgeQ = 1;
    // 1 : THD_MotionVectorQ = 2;
    // 2 :  THD_MotionVectorDir = (FLOAT)0.9; /// the ratio of 2 most dir >90% 
    CONST DOUBLE THD_MinorBlockUpdate[3] = { 10 , 2 , 0.85 };
    if(CTATargetNum.FeaturePtNumPrev>0)
    {
        Cluster->MinorBlockUpdate( CTAMinorBlockL ,
            CTAMinorBlockL_EdgeQuantity,
            RegionL,
            g_img_FrameDiff,
            harrisCornersPrev,
            harrisCorners2Prev,
            SrcImg_W, SrcImg_H,
            THD_MinorBlockUpdate,
            CTATargetNum.FeaturePtNumPrev); ///< number of MinorBlockType1
        Cluster->MinorBlockUpdate( CTAMinorBlockR ,
            CTAMinorBlockR_EdgeQuantity,
            RegionR,
            g_img_FrameDiff,
            harrisCornersPrev,
            harrisCorners2Prev,
            SrcImg_W, SrcImg_H,
            THD_MinorBlockUpdate,
            CTATargetNum.FeaturePtNumPrev);
    }
    Profile_CTA_End(e_MinorBlockUpdate);

    WIN32_StampTime( TicIdx );                        
    /// ==VERIFY== MinorBlockUpdate
    verify_MinorBlockUpdate(g_dst_YCbCr , SrcImg_W , SrcImg_H , 
                            ROISegBlockV , ROISegBlockH, CTAMinorBlockL , CTAMinorBlockR , RegionL , RegionR);
    
    WIN32_StampTime( TicIdx );   
    Profile_CTA_Start(e_MinorBlockLabeling);         
    CTATargetNum._L_MaxCandNum = Cluster->MinorBlockLabeling( CTAMinorBlockL ); ///< Max Label Idx
    CTATargetNum._R_MaxCandNum = Cluster->MinorBlockLabeling( CTAMinorBlockR ); ///< Max Label Idx(ex.9 mean GroupID 1 - 9 may exist )

    Profile_CTA_End(e_MinorBlockLabeling);
    WIN32_StampTime( TicIdx );                                           
    /// ==VERIFY== Labeling
    verify_Labeling(g_dst_YCbCr , SrcImg_W , SrcImg_H , 
                    ROISegBlockH , ROISegBlockV,
                    RegionL , RegionR,
                    CTATargetNum._L_MaxCandNum , CTATargetNum._R_MaxCandNum ,
                    CTAMinorBlockL , CTAMinorBlockR,
                    CTAMinorBlockL_EdgeQuantity, CTAMinorBlockR_EdgeQuantity);
    
    WIN32_StampTime( TicIdx );      
    Profile_CTA_Start(e_CandidateUpdate);
    ///3)Find the Proper Candidate and Filter(I) Candidate contains correct motion
    // THD_CandidateUpdate[i1]
    // 0 :	THD_MotionEdgeQ     = 1;
    // 1 :  THD_MotionVectorQ   = 2;
    // 2 :  THD_MotionVectorInnerDir = 2.9; /// Motion(inner)/Motion(out) > 2.9
    // 3 :  THD_MotionVectorInnerDir = 1.6; for near camera use   
    CONST FLOAT THD_CandidateUpdate[4] = {40, 3  ,2.9} ;//{ 20 , 7 , 2.9, 1.6 };
    CTATargetNum._L_CandNum = Cluster->CandidateUpdate( g_CTACandidateL , CTAMinorBlockL , CTAMinorBlockL_EdgeQuantity , CTATargetNum._L_MaxCandNum , RegionL , SrcImg_W, THD_CandidateUpdate, carState);//( g_CTACandidateL , CTAMinorBlockL , CTAMinorBlockL_EdgeQuantity, CTACandidatePrevL , CTATargetNum._L_MaxCandNum , RegionL , imgDimensionX, THD_CandidateUpdate);
    CTATargetNum._R_CandNum = Cluster->CandidateUpdate( g_CTACandidateR , CTAMinorBlockR , CTAMinorBlockR_EdgeQuantity , CTATargetNum._R_MaxCandNum , RegionR , SrcImg_W, THD_CandidateUpdate, carState);//( CTACandidateR , CTAMinorBlockR , CTAMinorBlockR_EdgeQuantity, CTACandidatePrevR , CTATargetNum._R_MaxCandNum , RegionR , imgDimensionX, THD_CandidateUpdate);

    Profile_CTA_End(e_CandidateUpdate);
    WIN32_StampTime( TicIdx );
    /// ==VERIFY== Plot Candidate Info ( Filter I )
    verify_CandidateUpdate( g_dst_YCbCr , SrcImg_W , SrcImg_H , 
                            CTATargetNum._L_CandNum , CTATargetNum._R_CandNum , g_CTACandidateL , g_CTACandidateR );
}
///////////////////////////////////////////////////
void CTA_ImageProcess_09Estimation(void)
{
    WIN32_StampTime( TicIdx );

    Profile_CTA_Start(e_SetStartupFlag);
    CTACanEstimation->SetStartupFlag( g_CTACandidateL , CTATargetNum._L_MaxCandNum ); //set IsValid=Startup
    CTACanEstimation->SetStartupFlag( g_CTACandidateR , CTATargetNum._R_MaxCandNum );
    Profile_CTA_End(e_SetStartupFlag);

    CTACanEstimation->UpdateMoveByFPS( g_CTACandidateL , CTATargetNum._L_MaxCandNum, g_CTACandidatePrevL ,CTATargetNum._L_MaxCandNumPrev ,'L', &CTATime );
    CTACanEstimation->UpdateMoveByFPS( g_CTACandidateR , CTATargetNum._R_MaxCandNum, g_CTACandidatePrevR ,CTATargetNum._R_MaxCandNumPrev ,'R', &CTATime );

    Profile_CTA_Start(e_CandAssociation);              
    // THD_RelatedCand[]
    // 0 : = 14*1.5; //REF to BlockSize(not used)
    // 1 : = 0.3; //overlapped area ratio at coming near
    // 1 : = 0.3; //overlapped area ratio at coming far
    CONST FLOAT THD_RelatedCand[3] = { 0 , 0.6, 0.8 }; 
    //  THD_RetrieveObj[0] = 2: max edge tracking times=2
    //  THD_RetrieveObj[1] = 1: if CandAssociation() succeeds, edge tracking times -=1
    CONST INT16 THD_RetrieveObj[2]= { 3, 1 }; 

    UINT16 LinkNumL  = CTACanEstimation->CandAssociation(g_CTACandidateL ,CTATargetNum._L_MaxCandNum ,
        g_CTACandidatePrevL ,CTATargetNum._L_MaxCandNumPrev ,'L', THD_RelatedCand,THD_RetrieveObj[1] ); 
    UINT16 LinkNumR  = CTACanEstimation->CandAssociation(g_CTACandidateR , CTATargetNum._R_MaxCandNum,
        g_CTACandidatePrevR ,CTATargetNum._R_MaxCandNumPrev ,'R', THD_RelatedCand,THD_RetrieveObj[1] );
    Profile_CTA_End(e_CandAssociation);

    Profile_CTA_Start(e_EdgeTracking);
    /// find all the T-1 cand position in T.(cand in T ignore)
    LinkNumL+=CTAPatch->EdgeTracking( g_CTACandidateL, g_CTACandidatePrevL, RegionL,'L', 
        g_Gaussian_img, g_Gaussian_imgPrev, g_Sobel_imgPrev, g_harrisMapPrev, 
        harrisCornersPrev, harrisCorners2Prev, 
        &CTATargetNum, CTATargetNum._L_MaxCandNum, CTATargetNum._L_MaxCandNumPrev,
        SrcImg_W,&CTATime,THD_RetrieveObj[0]);
    LinkNumR+=CTAPatch->EdgeTracking( g_CTACandidateR, g_CTACandidatePrevR, RegionR,'R', 
        g_Gaussian_img , g_Gaussian_imgPrev, g_Sobel_imgPrev, g_harrisMapPrev, 
        harrisCornersPrev, harrisCorners2Prev, 
        &CTATargetNum, CTATargetNum._R_MaxCandNum, CTATargetNum._R_MaxCandNumPrev,
        SrcImg_W,&CTATime,THD_RetrieveObj[0]);
    Profile_CTA_End(e_EdgeTracking);

    Profile_CTA_Start(e_UpdateBoundaryInfo);    
    CTACanEstimation->UpdateBoundaryCounter( g_CTACandidateL ,'L',RegionL, CTATargetNum._L_MaxCandNum );
    CTACanEstimation->UpdateBoundaryCounter( g_CTACandidateR ,'R',RegionR, CTATargetNum._R_MaxCandNum );
    Profile_CTA_End(e_UpdateBoundaryInfo); 

    Profile_CTA_Start(e_EliminateCrossover);  
    //must be after "UpdateBoundaryCounter()"
    CTACanEstimation->EliminateCrossover( g_CTACandidateL , g_CTACandidateR, &CTATargetNum,RegionL,RegionR );
    Profile_CTA_End(e_EliminateCrossover);

    Profile_CTA_Start(e_GetCandInfo);            
    /// 3) User View: KeepFrame, Size , Speed , Position
    // THD_WeightSpeed
    // 0 : Weight_SmoothSpeed = 0.1; // means (Weight)*PreFrame+(1-Weight)*CurFrame
    // 1 : Weight_SmoothPos = 0.1; // means (Weight)*PreFrame+(1-Weight)*CurFrame
    CONST FLOAT THD_WeightSpeed[2] = { 0.9 , 0.0 };
    CTACanEstimation->GetCandInfo( g_CTACandidateL ,CTATargetNum._L_MaxCandNum , g_CTACandidatePrevL , g_Sobel_img,g_Gaussian_img, SrcImg_W , THD_WeightSpeed,CTATime.fps,g_dst_YCbCr, carSpec,CTACustROI );
    CTACanEstimation->GetCandInfo( g_CTACandidateR ,CTATargetNum._R_MaxCandNum , g_CTACandidatePrevR , g_Sobel_img,g_Gaussian_img, SrcImg_W , THD_WeightSpeed,CTATime.fps,g_dst_YCbCr, carSpec,CTACustROI );
    Profile_CTA_End(e_GetCandInfo);    

    Profile_CTA_Start(e_DecideMoveDir);
    CTACanEstimation->DecideMoveDir( g_CTACandidateL ,CTATargetNum._L_MaxCandNum , g_CTACandidatePrevL , 'L',carState, RegionL );
    CTACanEstimation->DecideMoveDir( g_CTACandidateR ,CTATargetNum._R_MaxCandNum , g_CTACandidatePrevR , 'R',carState, RegionR );
    Profile_CTA_End(e_DecideMoveDir);

    Profile_CTA_Start(e_MergeOverlappedCand);
    CTACanEstimation->MergeOverlappedCand( g_CTACandidateL ,g_CTACandidatePrevL, 'L',CTATargetNum._L_MaxCandNum );
    CTACanEstimation->MergeOverlappedCand( g_CTACandidateR ,g_CTACandidatePrevR, 'R',CTATargetNum._R_MaxCandNum );
    Profile_CTA_End(e_MergeOverlappedCand);    

    Profile_CTA_Start(e_UpdateFlagTHD);
    CTACanEstimation->UpdateFlagTHD( g_CTACandidateL , g_CTACandidateR,CTATargetNum._L_MaxCandNum,CTATargetNum._R_MaxCandNum  );
    Profile_CTA_End(e_UpdateFlagTHD);   

    WIN32_StampTime(TicIdx);
    /// ==VERIFY== [ Relation, GetObjInfo , UpdateFlag ] 
    verify_Estimation(g_dst_YCbCr ,SrcImg_W ,SrcImg_H ,RegionL,RegionR,
        g_CTACandidateL , g_CTACandidateR ,
        g_CTACandidatePrevL, g_CTACandidatePrevR,
        CTACanEstimation->CountDownFlagAdpL,CTACanEstimation->CountDownFlagAdpR, 
        &CTATargetNum);
}
///////////////////////////////////////////////////
void CTA_ImageProcess_10CandidateFeatureUpdate(void)
{
    WIN32_StampTime(TicIdx);
    Profile_CTA_Start(e_CandidateFeatureUpdate_1);   
    CNN->CandidateFeatureUpdate_1(g_CTACandidateL , CTATargetNum._L_MaxCandNum ,
                                SrcImg_W, SrcImg_H, harrisCornersPrev, harrisCorners2Prev, CTATargetNum.FeaturePtNumPrev,
                                g_VSobelQty16x16, g_HSobelQty16x16, CTAMinorBlockL_EdgeQuantity);
    CNN->CandidateFeatureUpdate_1(g_CTACandidateR , CTATargetNum._R_MaxCandNum ,
                                SrcImg_W , SrcImg_H , harrisCornersPrev, harrisCorners2Prev, CTATargetNum.FeaturePtNumPrev,
                                g_VSobelQty16x16, g_HSobelQty16x16, CTAMinorBlockR_EdgeQuantity);

    Profile_CTA_End(e_CandidateFeatureUpdate_1);
    WIN32_StampTime(TicIdx);
    verify_CandidateFeatureUpdate(g_Gaussian_img , SrcImg_W , SrcImg_H ,
                                  g_CTACandidateL , g_CTACandidateR, CTATime.FrameNum );

}
///////////////////////////////////////////////////
void CTA_ImageProcess_11Predict(void)
{
    WIN32_StampTime(TicIdx);
    Profile_CTA_Start(e_Predict_1);    
    CTATargetNum._L_CandNum = CNN->Predict_1( g_CTACandidateL , CTATargetNum._L_MaxCandNum,CTATargetNum._L_CandNum );// effective Candidate after final filtering
    CTATargetNum._R_CandNum = CNN->Predict_1( g_CTACandidateR , CTATargetNum._R_MaxCandNum,CTATargetNum._R_CandNum );
    
    Profile_CTA_End(e_Predict_1);

    WIN32_StampTime(TicIdx);
    /// ==VERIFY== Candidate after FilterII
    verify_CNNPredict(g_dst_YCbCr ,SrcImg_W ,SrcImg_H ,CTATargetNum._L_CandNum , CTATargetNum._R_CandNum,
        g_CTACandidateL , g_CTACandidateR );

}
///////////////////////////////////////////////////
void CTA_ImageProcess_12AlarmStrategy(void)
{
    WIN32_StampTime(TicIdx);
    Profile_CTA_Start(e_bawl);

    //INT16 CTACustROI[4]  ={0,0,0,0};	//cm [ex. 180, 1800, 0, 600]
    CTADog->bawl( g_CTACandidateL, g_CTACandidateR, CTATargetNum._L_MaxCandNum, CTATargetNum._R_MaxCandNum, CTAAlarmState, CTAAlarmTrigger, CTACustROI);

    Profile_CTA_End(e_bawl);     

    WIN32_StampTime(TicIdx);
    /// ==VERIFY== CTADog[Draw (RED/YELLOW/WHITE)Rectanle][AudioWarnning/ImageWarnning]
    verify_CTADog( g_dst_YCbCr ,SrcImg_W ,SrcImg_H ,RegionL,RegionR, g_CTACandidateL, g_CTACandidateR, CTATime.FrameNum );

}
///////////////////////////////////////////////////
void CTA_ImageProcess_13DumpTrackingInfo(void)
{
#ifdef _WIN32
    verify_DumpTrackingInfo(g_FrameReport,g_FrameReportIdx,g_CTACandidateL,g_CTACandidateR,&CTATargetNum);
#endif
}
///////////////////////////////////////////////////
void CTA_ImageProcess_14APEX_HarrisCorner(void)
{
#ifdef _WIN32
    ImgFilter->CalculateHarrisValue(g_harrisValue, g_hSobel_img, g_vSobel_img, SrcImg_W, SrcImg_H);

    if (carState->carSpeed < g_car_speed_constraint)
        ImgFilter->FLT_ABSDiff( g_img_FrameDiff,  g_Gaussian_img , g_Gaussian_imgPrev, g_dif_th ,SrcImg_W, SrcImg_H);
    else
        ImgFilter->FLT_ABSDiff( g_img_FrameDiff,  g_Sobel_img , g_Sobel_imgPrev, g_ThdBinary ,SrcImg_W, SrcImg_H);

    ImgFilter->SupressNonMaxRemoveStill(g_harrisMap, g_harrisValue, g_Gaussian_img, g_img_FrameDiff, SrcImg_W, SrcImg_H, g_TH, g_e_value);

#else
    if (carState->carSpeed < g_car_speed_constraint) 
    {
        APEX_CTA_HarrisCorner2_Start(pROI_hSobel, pROI_vSobel, pROI_Gaussian, pROI_GaussianPrev, pROI_harrisMap, g_TH, g_dif_th, g_e_value);
        APEX_CTA_HarrisCorner2_Wait();
    }
    else 
    {
    APEX_CTA_HarrisCorner_Start(pROI_hSobel, pROI_vSobel, pROI_Gaussian, pROI_FrameDiff, pROI_harrisMap, g_TH, g_e_value);
    APEX_CTA_HarrisCorner_Wait();
    }
#endif
}
///////////////////////////////////////////////////
void CTA_ImageProcess_15CornerSave(void)
{
    WIN32_StampTime( TicIdx );
    Profile_CTA_Start(e_SaveCornerPos);
    CTATargetNum.FeaturePtNum = CTAPatch->SaveCornerPos( g_harrisMap, SrcImg_W, SrcImg_H, harrisCorners, MAXSizeHarrisCorner);
    Profile_CTA_End(e_SaveCornerPos);

    WIN32_StampTime( TicIdx );
    /// ==VERIFY== Plot Corners Informatio with GREEN COLOR
    // g_Gaussian_img  / g_Sobel_img / g_vSobel_img / g_hSobel_img /g_harrisValue / g_harrisMap
    verify_GaussianImg_HarrisCorner( g_dst_YCbCr , g_Gaussian_img, g_harrisMap,g_Sobel_img,g_vSobel_img,g_hSobel_img,g_harrisValue, 
                                     g_VSobelQty16x16, g_HSobelQty16x16,
                                     CTATargetNum.FeaturePtNum , SrcImg_W , SrcImg_H );        
        
#if 0 /// Note: for k2 debug 
    // [debug code]
    int rect_cnt_left, rect_cnt_right, ln_cnt_left, ln_cnt_right;
    rect_cnt_left = rect_cnt_right = ln_cnt_left = ln_cnt_right = 0;

    for ( int i1 = 0 ; i1< CTATargetNum._L_MaxCandNum; ++i1)
    {
        if(CTACandidateL[i1].isValid==Valid) {
            rect_cnt_left++;
            if( CTACandidateL[i1].isObjPosxyValid && CTACandidateL[i1].isObjPosXYValid) 
                ln_cnt_left++;
        }

    }

    for ( int i1 = 0 ; i1< CTATargetNum._R_MaxCandNum; ++i1) {
        if(CTACandidateR[i1].isValid==Valid) {            
            rect_cnt_right++;

            if( CTACandidateR[i1].isObjPosxyValid && CTACandidateR[i1].isObjPosXYValid) 
            ln_cnt_right++;
        }
    }

#ifdef _WIN32
    char str[64];
    sprintf(str, "%d: (%d, %d) - (%d, %d) \n", CTATime.FrameNum, rect_cnt_left, ln_cnt_left, rect_cnt_right, ln_cnt_right);
    //OutputDebugString("%d: (%d, %d) - (%d, %d) \n", CTATime.FrameNum, rect_cnt_left, ln_cnt_left, rect_cnt_right, ln_cnt_right);
    OutputDebugString ((LPCTSTR)str);
#else // K2 platform
    printf("%d: (%d, %d) - (%d, %d) \n", CTATime.FrameNum, rect_cnt_left, ln_cnt_left, rect_cnt_right, ln_cnt_right);
#endif
        
    // END [debug code]
#endif
}
///////////////////////////////////////////////////
void CTA_ImageProcess_16Refresh(void)
{
    WIN32_StampTime( TicIdx );
    Profile_CTA_Start(e_Update_Data_Before_Next_Frame);              
    Update_Data_Before_Next_Frame( g_CTACandidateL,g_CTACandidatePrevL,
                                   g_CTACandidateR,g_CTACandidatePrevR,CTATargetNum,
                                   CTATime, SrcImg_W, SrcImg_H );

    Profile_CTA_End(e_Update_Data_Before_Next_Frame);           

    WIN32_StampTime( TicIdx );
    verify_Update_Data_Before_Next_Frame(   g_Gaussian_img,g_Gaussian_imgPrev,
                                            g_Sobel_img,g_Sobel_imgPrev,
                                            harrisCorners,harrisCornersPrev,
                                            harrisCorners2, harrisCorners2Prev,
                                            g_CTACandidateL,g_CTACandidatePrevL,
                                            g_CTACandidateR,g_CTACandidatePrevR,
                                            CTATime.FrameNum, SrcImg_W ,SrcImg_H ,
                                            &CTATargetNum);
}

/**
*@brief	    [k2/PC] DayMode CTA Process
*/
static void CTA_ImageProcess_06Day()
{
    CTA_ImageProcess_07PointTracking();

    CTA_ImageProcess_08PointGrouping();

    CTA_ImageProcess_09Estimation();

    CTA_ImageProcess_10CandidateFeatureUpdate();

    CTA_ImageProcess_11Predict();

    CTA_ImageProcess_12AlarmStrategy();

    CTA_ImageProcess_13DumpTrackingInfo();
            
#ifdef _WIN32	 
    CTA_ImageProcess_14APEX_HarrisCorner();    
#else	
    // [Mark 2014/02/06] temporary disabled part of code for firmware release, need furthuer verfication
    ForkApex((uint32)CTA_ImageProcess_14APEX_HarrisCorner);
    // END [Mark 2014/02/06] temporary disabled part of code for firmware release, need furthuer verfication
    
    // [Mark 2014/02/06] temporary disabled part of code for firmware release, need furthuer verfication    
    WaitForkApexComplete((uint32)CTA_ImageProcess_14APEX_HarrisCorner);    
    // END [Mark 2014/02/06] temporary disabled part of code for firmware release, need furthuer verfication 

#endif // END #ifndef _WIN32	 

    CTA_ImageProcess_15CornerSave();

    CTA_ImageProcess_16Refresh();
}
/**
*@brief	    [NightMode] CTA Process
*@param[in] dst_Y			the YCbCr ImgData show in
*/
static void CTA_ProcessNight()
{
    #pragma region    ============LightCluster============
    /// Cluster to lampL/R
    Cluster->ClusterIntoHeadlight(g_bin_img, SrcImg_W, SrcImg_H,
        lampL , lampR ,
        CTATargetNum._L_MaxCandNum, CTATargetNum._R_MaxCandNum ,
        RegionL , RegionR);
    CTAPatch->Filter01_Night(g_bin_img, SrcImg_W, SrcImg_H,
        lampL , lampR ,
        CTATargetNum._L_MaxCandNum, CTATargetNum._R_MaxCandNum ,
        CTATargetNum._L_CandNum, CTATargetNum._R_CandNum ,
        RegionL , RegionR);

    /// ==VERIFY== [ show 1) HeadLight Number ]
    verify_ClusterIntoHeadlight(g_dst_YCbCr ,SrcImg_W ,SrcImg_H ,RegionL,RegionR,
        lampL , lampR ,
        CTATargetNum._L_MaxCandNum , CTATargetNum._R_MaxCandNum);
    WIN32_StampTime( TicIdx );
    #pragma endregion 
    #pragma region    ============Headlight_Features_Extraction============
    CNN->HeadLightFeatureUpdate_1( lampL , CTATargetNum._L_MaxCandNum);
    CNN->HeadLightFeatureUpdate_1( lampR , CTATargetNum._R_MaxCandNum);

    /// ==VERIFY== [ 1)Export Training data/img ]
    verify_HeadLightFeatureUpdate_1( g_dst_YCbCr , SrcImg_W ,SrcImg_H ,RegionL,RegionR,
                                    lampL , lampR );
    #pragma endregion 
    #pragma region    ============Classifier============
    CTATargetNum._L_CandNum = CNN->HeadLightPredict_1( lampL , CTATargetNum._L_MaxCandNum);
    CTATargetNum._R_CandNum = CNN->HeadLightPredict_1( lampR , CTATargetNum._R_MaxCandNum);

    /// ==VERIFY== [ lamp after Filter II ]
    verify_HeadLightPredict_1( g_dst_YCbCr , SrcImg_W ,SrcImg_H ,RegionL,RegionR,
                                lampL , lampR , CTATargetNum._L_CandNum , CTATargetNum._R_CandNum);
    WIN32_StampTime( TicIdx );
    #pragma endregion 
    #pragma region    ====HeadlightEstimation[Association & Tracking & Grouping & LampInfo]============
        /// 1) Association
        //  0: ex. 0.5 means overlap region>50%(0~1)
        //  1: ex. 3 means area scale ratio is 1/3<ratio<3
        CONST FLOAT THD_RelatedLamp[2] = { 0.2 , 3.0 }; ///< Fit "OverlapRatio(0-1)" and size(area)" for similar Candidates
        UINT16 LinkNumL = CTACanEstimation->LampAssociation(lampL ,CTATargetNum._L_MaxCandNum, 
                                                            lampPrevL , CTATargetNum._L_MaxCandNumPrev , 
                                                            THD_RelatedLamp ); 
        UINT16 LinkNumR = CTACanEstimation->LampAssociation(lampR ,CTATargetNum._R_MaxCandNum, 
                                                            lampPrevR , CTATargetNum._L_MaxCandNumPrev , 
                                                            THD_RelatedLamp );
        ///2) Lamp Tracking 
        CTACanEstimation->LampSuperglue( lampPrevL,CTATargetNum._L_MaxCandNumPrev,
                                         lampL ,CTATargetNum._L_MaxCandNum,
                                         g_Gaussian_img , g_Gaussian_imgPrev, SrcImg_W);
        CTACanEstimation->LampSuperglue( lampPrevR,CTATargetNum._R_MaxCandNumPrev,
                                         lampR ,CTATargetNum._R_MaxCandNum,
                                         g_Gaussian_img , g_Gaussian_imgPrev, SrcImg_W);

        ///3) Lamps Grouping: pair two headlights into a group(car)
        // TODO: Not Ready yet

        ///4) lamp Info: [ Distance , distance from car head light to ground , speed , TTC ,(KeepFrame/Size) ]
        //      THD_WeightSpeed[0] = Weight_SmoothSpeed = 0.1; //  means (Weight)*CurFrame+(1-Weight)*PreFrame
        //      THD_WeightSpeed[1] = Weight_SmoothPos   = 0.1; //  means (Weight)*CurFrame+(1-Weight)*PreFrame
        CONST FLOAT THD_WeightSpeedPos[2] = { 0.2 , 0.5 };// { 0.0 , 0.0 };
        CTACanEstimation->GetLampInfo( g_bin_img, SrcImg_W, lampL , CTATargetNum._L_MaxCandNum, lampPrevL,CTATargetNum._L_MaxCandNumPrev ,CTATime.fps, THD_WeightSpeedPos );
        CTACanEstimation->GetLampInfo( g_bin_img, SrcImg_W, lampR , CTATargetNum._R_MaxCandNum, lampPrevR,CTATargetNum._R_MaxCandNumPrev ,CTATime.fps, THD_WeightSpeedPos );

        ///5) Flag Update
        //CTACanEstimation->UpdateFlagTHD_Night( lampL , lampR );

        /// ==VERIFY== [ Relation, GetLampInfo , UpdateFlagTHD_Night ] 
        verify_Estimation_Night( g_dst_YCbCr ,SrcImg_W ,SrcImg_H ,RegionL,RegionR, 
                                 lampL , lampR ,
                                 lampPrevL, lampPrevR);
    WIN32_StampTime( TicIdx );
    #pragma endregion 
    #pragma region    ============Warning_Decision============
    CTADog->bowwow_Night( lampL ,lampPrevL, CTATargetNum._L_MaxCandNum, CTATargetNum._R_MaxCandNum, CTAAlarmState );

    /// ==VERIFY== CTADog(drawing obj- yellow/red rectangle)[AudioWarnning/ImageWarnning]
    verify_CTADog_Night( g_dst_YCbCr ,SrcImg_W ,SrcImg_H ,RegionL,RegionR, lampL , lampR , CTAAlarmState , CTATime.FrameNum);
    WIN32_StampTime( TicIdx );
    #pragma endregion 
    #pragma region    ============Update Data Before Next Frame============
    Update_Data_Before_Next_Frame_Night(
        lampL,lampR,lampPrevL,lampPrevR,
        CTATargetNum ,
        CTATime.FrameNum, SrcImg_W, SrcImg_H);

    verify_Update_Data_Before_Next_Frame_Night( g_Gaussian_img,g_Gaussian_imgPrev,
        g_bin_img,g_bin_PrevImg,
        lampL,lampPrevL,
        lampR,lampPrevR,
        CTATime.FrameNum, SrcImg_W,SrcImg_H);
    WIN32_StampTime( TicIdx );
    #pragma endregion
        
}

/**
* @brief   get current CTA weather Mode
* @return  [TRUE: Day / FALSE: Night]
*/
BOOL CTA_GetResult_isDay(void)
{
    return (Day == CTAWeather) ? (TRUE) : (FALSE) ;
}

/**
* @brief get the alarm state at this frame
* @return  [0:No alarm / 1:Right Alarm  /2:LeftAlarm / 3: Both side alarm]
*/
UCHAR CTA_GetResult_Alarm(void){ return CTAAlarmTrigger; }

/**
* @brief get the alarm state at this frame
* @return  [FALSE:No Object]
*/
BOOL  CTA_GetResult_ClosedPosL(INT32 &posL_cm)
{
    BOOL ret            = FALSE;
    posL_cm             = 0x80000000;
    short maxCand       = 0;
    CandidateRec *pCand = CTA_GetResult_TargetCarsL(maxCand);
    
    ForEach(idx, 0, maxCand)
    {
        if((Valid == pCand[idx].isValid)
          &&(pCand[idx].isObjPosxyValid)
          &&(pCand[idx].isObjPosXYValid))
        {
            ret     = TRUE;
            posL_cm = MAX(posL_cm, pCand[idx].ObjPosXY[0]);
        }
    }
    return ret;
}
BOOL  CTA_GetResult_ClosedPosR(INT32 &posR_cm)
{
    BOOL ret            = FALSE;
    posR_cm             = 0x80000000;
    short maxCand       = 0;
    CandidateRec *pCand = CTA_GetResult_TargetCarsR(maxCand);

    ForEach(idx, 0, maxCand)
    {
        if((Valid == pCand[idx].isValid)
            &&(pCand[idx].isObjPosxyValid)
            &&(pCand[idx].isObjPosXYValid))
        {
            ret     = TRUE;
            posR_cm = MAX(posR_cm, pCand[idx].ObjPosXY[0]);
        }
    }
    return ret;

}

/**
* @brief get the Closed obj speed
* @return  [FALSE:No Object]
*/
BOOL  CTA_GetResult_ClosedSpeedL(INT32 &speedL_kph)
{
    BOOL ret = FALSE;
    return ret;
}
BOOL  CTA_GetResult_ClosedSpeedR(INT32 &speedR_kph)
{
    BOOL ret = FALSE;
    return ret;
}
/**
* @brief get the Closed obj TTC
* @return  [FALSE:No Object]
*/
BOOL  CTA_GetResult_ClosedTTCL(INT32 &ttcL_ms)
{
    BOOL ret = FALSE;
    return ret;
}
BOOL  CTA_GetResult_ClosedTTCR(INT32 &ttcR_ms)
{
    BOOL ret = FALSE;
    return ret;
}
/**
*@brief   Return the CTA Left Target Cars Information & Numbers
*@param[out]  MaxNum    the MaxNum of Cars
*/
CandidateRec *CTA_GetResult_TargetCarsL( short &MaxNum )
{
    MaxNum = CTATargetNum._L_MaxCandNumPrev ;
    return   g_CTACandidatePrevL;
}

/**
*@brief   Return the CTA Right Target Cars Information & Numbers
*@param[out]  MaxNum    the MaxNum of Cars
*/
CandidateRec *CTA_GetResult_TargetCarsR( short &MaxNum )
{
    MaxNum = CTATargetNum._R_MaxCandNumPrev;
    return   g_CTACandidatePrevR;
}