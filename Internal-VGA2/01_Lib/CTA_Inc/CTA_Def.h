/*
===================================================================
Name		: CTA_Def.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function declare the  
                1. #define PLATFORM Flags / OSD_SHOW / XML.CSV.TXT File Dump flag
                2. #define MACROD  OPERATION
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/31	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/
#ifndef __CTA_DEF_H
#define __CTA_DEF_H
#ifndef _WIN32 // for K2 platform
  #include "CTA_Type.h"
#endif
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
//--------------define Plug in Algorithm id---------------
#define C_PLUGIN_ALOGRITHM_ID_MOD			1
#define C_PLUGIN_ALOGRITHM_ID_LDWS			2
#define C_PLUGIN_ALOGRITHM_ID_CTA           3
#define C_PLUGIN_ALOGRITHM_ID_RSW           4
#define C_PLUGIN_ALOGRITHM_ID_LCV           5
#define C_PLUGIN_ALOGRITHM_ID_MOD_CTA       6
    #define ALGORITHM_ID                    C_PLUGIN_ALOGRITHM_ID_CTA

#define C_CTA_DLL_VERSION          2015020301

//--------------SWITCH---------------
    #define DBG_TXT		                    OFF     ///< PC Development TXT Out
    #define DBG_INFO                        OFF
    #define OFFLINE_TRAIN_FOLDER		    L"C:\\CTA_OFFLINE_TRAIN_DATA\\"
        #define OFFLINE_TRAIN_DUMPIMG		   OFF		                    ///< Need [OPENCV_EN]
        #define OFFLINE_TRAIN_DUMPIMG_NIGHT    OFF                          ///< Need [OPENCV_EN]
    #define PCLogFilePath                    "C:\\CTA_Log\\CTA_Log.xml"     ///< XML FilePath for regression test
    #define PC_FILE_INFO	                OFF                             ///< PC Folder information  #include<FilesInfo.h>

//--------------PLOT_OSD_ENABLE-------------------
    #define OSD_verify_SrcImg_Header                     OFF
    #define OSD_verify_WeatherType                       ON
    #define OSD_verify_GaussianImg_HarrisCorner          OFF	//draw harris corner point <GREEN>
    #define OSD_verify_MotionCorner                      OFF    //<YELLOW>
    #define OSD_verify_FLT_ABSDiff                       OFF
    #define OSD_verify_MinorBlockUpdate                  ON
    #define OSD_verify_Labeling                          ON
    #define OSD_verify_CandidateUpdate                   OFF
    #define OSD_verify_CandidateFeatureUpdate            ON
    #define OSD_verify_CNNPredict                        ON
    #define OSD_verify_Estimation                        ON
        #define OSD_RULLER                               OFF
        #define UI_DRAW_TRACK_PT			             ON		//draw tracking point <PURPLE or ORANGE>	
        #define UI_DRAW_CAR_HEAD                         ON		//draw car head point <RED>
        #define UI_DRAW_GROUND_LINE					     ON		//draw ground line <CYAN>
    #define OSD_verify_CTADog                            ON
    #define OSD_verify_CornersMap                        ON
    #define OSD_verify_Update_Data_Before_Next_Frame     ON
    #define OSD_verify_TimeCost                          ON


//--------------[NightMode]_PLOT_OSD_ENABLE-------------------
    #define OSD_verify_APEX01_Night                          OFF
    #define OSD_verify_ClusterIntoHeadlight                  ON
    #define OSD_verify_HeadLightFeatureUpdate_1              ON
    #define OSD_verify_HeadLightPredict_1                    ON
    #define OSD_verify_Estimation_Night                      ON
    #define OSD_verify_CTADog_Night                          ON
    #define OSD_verify_Update_Data_Before_Next_Frame_Night   ON

//--------------[SYNC_ACROSS_PLATFORM]--------------
//#define  SYNC_ACROSS_PLATFORM                 /// Enable the Dump(Load) functions
//#define  _DUMP_OUTPUT_
//    #define DATA_PATTERN_FOUNTAINHEAD	    /// ON:Load/OFF:Dump switch
//#define _USE_CTA_OWN_IO_FUNCTION_

#define ON		    1
#define OFF		    0
#define SAT8(x)		(((x) > 255)? 255 : (((x) < 0)? 0 : (x)))
#define SAT16(x)	(((x) > 65535)? 65535 : (((x) < 0)? 0 : (x)))
#define ABS(x)		(((x) > 0)? (x) : -(x))
#define ROUND(x)	(((x)>=0)?((int)(x+0.5)):((int)(x-0.5)))
#define MAX_MIN_RANGE(data,bot,up)    ( MIN((up), MAX((data),(bot))) )
#ifdef _WIN32
  #define MIN(a,b)  ((a<b)?(a):(b))
  #define MAX(a,b)  ((a>b)?(a):(b))
#else // for K2 platform
   // MIN and MAX macro is already defined in ICP_SDK/inc/GlobalTypes.h
  //#define MIN(a,b)    ((a<b)?(a):(b))
  //#define MAX(a,b)   ((a>b)?(a):(b))
  #define CONST const
#endif

#define PRECISION_2(x)  (ROUND(x*100.0)/(FLOAT)100.0)   ///0.01 precision, round 

#ifdef _WIN32
    #pragma warning( disable : 4819 4554 4996 4018 4244 4018 4305) 
#endif


//-------------Sensor_Resolution-----------------------------//
#define SrcImg_W   VGA_W
#define SrcImg_H   VGA_H

#define VGA_W   640
#define VGA_H   480
#define WVGA_W  744	
#define WVGA_H  520	

//-------------ImageProcessROI-----------------------------//
#define   ROISegBlockNum_H   20     ///< number of horizontal blocks
#define   ROISegBlockNum_V	 13		///< number of vertical   blocks
CONST INT32  MAXCandidateNum = ROISegBlockNum_H * ROISegBlockNum_V;

CONST INT32   ROISegBlockH = 16;	///< BlockSize Horizontal
CONST INT32   ROISegBlockV = 16;    ///< BlockSize Vertical

extern INT32  RegionL[4];
extern INT32  RegionR[4];

#define   MAXSizeHarrisCorner   250 
#define   MAX_HeadLightNum      300

#ifdef _WIN32
    #define MAXSizeEventResult 8000  // 4000 = 3min
#endif

//-------------OTHERS-----------------------------//
#define   MIN_MOVE_SPEED_STRAIGHT 70	    //56*0.036~=2km/h,70*0.036~=2.5km/h
#define   MIN_MOVE_SPEED_ROTATE 111			//111*0.036~=4km/h, 125*0.036~=4.5km/h
#define   SPEED_CONST_KMH 0.036             //1cm/sec=0.036km/h

#define C_CTA_STATUS_LEFT_WARNING		0x02
#define C_CTA_STATUS_RIGHT_WARNING		0x01

#ifdef _WIN32
    #define WIN32_ASSERT(expr)     assert(expr)
#else // K2 platform
    #define WIN32_ASSERT(expr)    ((void *)0)
#endif

#define ForEach(x,a,b)  for(INT32 x = a; x < b ; ++x)

#endif