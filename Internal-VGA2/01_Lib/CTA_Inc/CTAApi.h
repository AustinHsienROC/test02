﻿/*
===================================================================
Name		: CTAApi.h                ╭══╮
Author		: Austin                ╭╯ΘΘ║
Version		: v0.0030               ╰⊙═⊙╯。oо○
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement CTA main process.

<Brief>
void CTA_WorkBuf_Allocate( void );          //1.1) [k2/PC] Memory alloc 
bool CTA_Update_LUT();                      //1.2) [k2/PC] Import LUT
    - void setImgProcROI();                 //1.2.1 Update ROI of Image Process
bool CTA_UpdateCarParameter();              //1.3)  Custom Car data 
void CTA_Reset( void );                     //1.4) [k2/PC] Reset necessary stack

bool CTA_UpdateCarDrivingInfo();            //2.1) [k2/PC] Car driving state 
void CTA_ImageProcess();                    //2.2) Main Process 

void CTA_WorkBuf_Release(void);             //3.1) [k2/PC] Release Heap

UCHAR CTA_GetResult_Alarm(void);            //4.1) [k2/PC] Alarm state
BOOL CTA_FindObjRect();                     //4.2) [k2/PC] 
void CTA_FindObjLine();                     //4.3) [k2/PC] 
void CTA_DrawObjFrontLine();                //4.4) [k2/PC] 

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/09	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
2014/12/30	  | v0.0020  	|API for K2/PC platform        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
2015/02/24	  | v0.0030  	|API standardization        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef _CTAApi_H_
#define _CTAApi_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************

#include "CTA_Type.h"
#include "CTA_Def.h"
#include "CoordinateConversion.h"
#ifndef _WIN32
  #include "ICP_Image.hpp"
#endif


//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************
extern UCHAR CTAAlarmState;   //isCTAAlarm ;
extern UCHAR CTAAlarmTrigger; //isCTAAlarm ;
extern CandidateRec g_CTACandidateL [ MAXCandidateNum ];
extern CandidateRec g_CTACandidateR [ MAXCandidateNum ];

#ifdef _WIN32
    extern char g_FrameReport[ 500 ];       ///< RemoteCtrl Debug Info
    extern int  g_FrameReportIdx;           ///< used index for g_FrameReport
    extern char VideoFilePath[ 240 ];

    //-------------------XMLInfo-------------------------//
    extern BOOL EventResultL[MAXSizeEventResult];
    extern BOOL EventResultR[MAXSizeEventResult];
#endif

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
#ifdef _WIN32
    //---------Initialize-----------------
    void CTA_WorkBuf_Allocate(void); 
    bool CTA_Update_LUT(UINT32 a_unParameterID, UCHAR * a_pData, UINT32 a_unDataSize = 0);
    void setImgProcROI( CTA_CaliResult_t *i_LUT );
    bool CTA_UpdateCarParameter(UCHAR * a_pData, UINT32 a_unDataSize);
    void CTA_Reset( void );
    
    //---------Process-----------------
    void CTA_UpdateCarDrivingInfo( UINT32 a_FrameTimeStamp_ms, FLOAT a_CraSpeed, FLOAT a_SteerAngle, UINT32 a_FrameCnt);
    void CTA_ImageProcess(UCHAR *dst_YCbCr, UCHAR *src_YCbCr); 

    //---------Release-----------------
    void CTA_WorkBuf_Release(void); 

#else // k2 platform
    //---------Initialize-----------------
    void CTA_WorkBuf_Allocate(void); 
    bool CTA_Update_LUT(UINT32 a_unParameterID, UCHAR * a_pData, UINT32 a_unDataSize = 0);
    void setImgProcROI( void *i_LUT );  
    bool CTA_UpdateCarParameter(UINT32 a_CarWidth_cm, UINT32 a_RearTireToBumper_cm, DOUBLE a_SteerOverFrontTireRatio, UINT32 a_WheelBase_cm);
    void CTA_Reset( void );
    //---------Process-----------------
    void CTA_UpdateCarDrivingInfo( UINT32 a_FrameTimeStamp_ms, FLOAT a_CraSpeed, FLOAT a_SteerAngle, UINT32 a_FrameCnt);
    void CTA_ImageProcess(ICP_Image *picp_src_Y);

    //---------Release-----------------
    void CTA_WorkBuf_Release(void); 

#endif

//---------[k2/PC]GetResult-----------------
BOOL  CTA_GetResult_isDay(void);
UCHAR CTA_GetResult_Alarm(void);
BOOL  CTA_GetResult_ClosedPosL(INT32 &posL_cm);
BOOL  CTA_GetResult_ClosedPosR(INT32 &posR_cm);
BOOL  CTA_GetResult_ClosedSpeedL(INT32 &speedL_kph);
BOOL  CTA_GetResult_ClosedSpeedR(INT32 &speedR_kph);
BOOL  CTA_GetResult_ClosedTTCL(INT32 &ttcL_ms);
BOOL  CTA_GetResult_ClosedTTCR(INT32 &ttcR_ms);

CandidateRec *CTA_GetResult_TargetCarsL( short &MaxNum );
CandidateRec *CTA_GetResult_TargetCarsR( short &MaxNum );

BOOL CTA_FindObjRect( BOOL a_bLeftSide,INT16& a_sObjIndex
                    , INT16& a_start_x, INT16& a_start_y
                    , INT16& a_end_x, INT16& a_end_y);

BOOL CTA_FindObjFrontLine(BOOL a_bLeftSide,INT16& a_sObjIndex
                        , INT32& a_ObjDist);

typedef void (*CTA_DrawObjFrontLine_CB)(INT32 a_start_x, INT32 a_start_y, INT32 a_end_x, INT32 a_end_y);

void CTA_DrawObjFrontLine(CTA_DrawObjFrontLine_CB a_pCbFun, INT32 a_ObjDist);

/// back-door
void CTA_EnableCopySourceImg(BOOL a_bEnableCopy);

//******************************************************************************
//   C T A     I N T E R N A L     F U N C T I O N S 
//******************************************************************************

void CTA_ImageProcess_02APEX_Day(void);
void CTA_ImageProcess_03APEX_CntEdgeQty(void);
void CTA_ImageProcess_04WheatherType(void);
void CTA_ImageProcess_07PointTracking(void);
void CTA_ImageProcess_08PointGrouping(void);
void CTA_ImageProcess_10CandidateFeatureUpdate(void);
void CTA_ImageProcess_11Predict(void);
void CTA_ImageProcess_09Estimation(void);
void CTA_ImageProcess_12AlarmStrategy(void);
void CTA_ImageProcess_13DumpTrackingInfo(void);
void CTA_ImageProcess_14APEX_HarrisCorner(void);
void CTA_ImageProcess_15CornerSave(void);
void CTA_ImageProcess_16Refresh(void);
void CTA_ImageProcess_90TimeSpend(void);

#endif // END #ifndef _CTAApi_H_
