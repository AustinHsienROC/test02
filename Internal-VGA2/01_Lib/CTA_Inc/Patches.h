/*
===================================================================
Name		: Patches.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: These functions are plug-in of the CTA

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/09/01	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/
#ifndef _Patches_H_
#define _Patches_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
  #include "OpencvPlug.h"		     ///< Need Before "RSW_Type.h" due to VARIABLE File
  #include "FilesInfo.h"
  #include "VideoHdInfo.h"
  #include "OSD_Ycc422.h"
  #include "CTA_PCLog.h"
#endif

#include "CoordinateConversion.h" // Calib LUT
#include "CTA_Type.h"
#include "CTA_Def.h"
#include "CTAApi.h"

#include "CTA_CustomData.h"

#include "Filter2D.h"
#include "Point_Track.h"
#include "Point_Group.h"
#include "ML_NeuralNet.h"
#include "Estimation.h"
#include "WarningDecision.h"
#include "WhiteBox.h"

/*
#include "OpencvPlug.h"		     ///< Need Before "RSW_Type.h" due to VARIABLE File

#include "CoordinateConversion.h" // Calib LUT
#include "CTA_Type.h"
#include "CTA_Def.h"

#include "FilesInfo.h"		
#include "CTAApi.h"
#include "VideoHdInfo.h"
#include "CTA_CustomData.h"
#include "Filter2D.h"
#include "Point_Track.h"
#include "Point_Group.h"
#include "ML_NeuralNet.h"
#include "Estimation.h"
#include "WarningDecision.h"
#include "WhiteBox.h"
#include "CTA_PCLog.h"
*/
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
#define CTAPatch Patches::getInstance()

//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
class Patches
{
private:
    static Patches *instance;

public:
    Patches(void);
    ~Patches(void);

    static Patches* getInstance();
    static void ResetInstance();

    INT16 GetNewGroupID( CandidateRec CTACandidate[], UINT16* LabelCount=NULL);
    inline BOOL IsInCandRect(CONST INT16 x,CONST INT16 y, CONST CandidateRec* CTACandidate );
    UINT8 EdgeTracking( CandidateRec CTACandidate[], CandidateRec CTACandidatePrev[], CONST INT32 Region[4], CONST CHAR Dir, 
                        CONST UCHAR* img_track, CONST UCHAR* img_trackPrev, CONST UCHAR* img_SblEdgePrev,
		CONST UCHAR *harrisMapPrev, CONST HarrisCornerPos *harrisCornersPrev, CONST HarrisCornerPos2 *harrisCorners2Prev,
		TARGET_NUM *pTargetNum, INT16 &inMaxCand,CONST INT16 inMaxCandPrev ,CONST INT32 imgDimensionX,
		CONST CTATimeManagement* CTATime, CONST INT16 TH_failedEdgeTimes );

	inline INT16 FindLREdgePoint(CandidateRec CTACandidatePrev[],CONST INT16 i,CONST CHAR Dir,CONST UCHAR* img_SblEdgePrev,BOOL &foundLRedge,INT16 &leftPos,INT16 &rightPos);
	inline void FindBottomCorner(CandidateRec CTACandidatePrev[],CONST INT16 i,CONST CHAR Dir,
                                 CONST UCHAR *harrisMapPrev, CONST HarrisCornerPos *harrisCornersPrev, CONST HarrisCornerPos2 *harrisCorners2Prev,
                                 TARGET_NUM *pTargetNum, BOOL &foundLeftCorner,BOOL &foundRightCorner);

    UINT16 FilterAfterPredict( CandidateRec CTACandidate[], UINT16 inMaxCand , CandidateRec CTACandidatePrev[], UINT16 inMaxCandPrev  );

    void Filter01_Night(UCHAR *g_Binar_img, INT32 imgDimensionX, INT32 imgDimensionY, HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] , INT16 MaxLampL , INT16 MaxLampR, INT16 &lampNumL, INT16 &lampNumR,const INT32 RegionL[4], const INT32 RegionR[4] );
    INT32 SaveCornerPos(UCHAR *harrisMap, INT32 imgDimensionX, INT32 imgDimensionY, HarrisCornerPos *harrisCorners, INT32 MAXSize );
                         
    inline BOOL IsInRect(CONST INT16 x,CONST INT16 y, CONST Rect_t* ROI );
	inline TypeBoundCondition IsPointAtScreenBoundary( CONST INT16 pt_x,CONST INT16 pt_y, CONST INT32 Region[4] );
};

#endif //end of _Patches_H_