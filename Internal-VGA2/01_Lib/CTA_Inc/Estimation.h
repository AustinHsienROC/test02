/*
===================================================================
Name		: Estimation.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement Estimation related Candidate and Get the info .
              1. Candidate Tracking
              2. Candidate Info Update
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/29	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/
#ifndef _Estimation_H_
#define _Estimation_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Type.h"
#include "CTA_Def.h"
#include "CoordinateConversion.h"
  #include "CTA_CustomData.h"
#ifdef _WIN32
  #include "OSD_Ycc422.h"
#endif


//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
//-------------Array Length----------------------------//
#define LINK_ARRAY_NUM 128
//-------------THRESHOLD----------------------------//
#define CountDownFlagMax         2
#define CountDownFlagAdaptiveMax 1

//-------------Speed/TIME_ESTIMATION----------------------------//
#define CTATTC			2000	//ms

#define CTACanEstimation Estimation::getInstance()
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

class Estimation
{
private:
    static Estimation *instance;
    //------------[DayMode]-------------
    BOOL isCandAtBoundary( CandidateRec i_CTACandidate[], CONST BOOL isleft);
    void GetCandPos ( CandidateRec CTACandidate[] , UCHAR *Sobel_img ,UCHAR *gaussImg , INT32 imgDimensionX, UCHAR *dst_Y); //motionEdge
    BOOL  GetCandPos2( CandidateRec CTACandidate[] , CandidateRec CTACandidatePrev[] , UCHAR *g_img_FrameDiff , INT32 imgDimensionX  );

    BOOL GetCandSpeedbyHeadPos ( CandidateRec CTACandidate[] , CandidateRec CTACandidatePrev[], FLOAT infps );
//    BOOL GetCandSpeedbyMotionCorner( CandidateRec CTACandidate[], FLOAT infps ); //old version
    void GetEstimatedSpeed( CandidateRec CTACandidate[], CandidateRec CTACandidatePrev[], BOOL isLeft);

    BOOL GetCandTTC ( CandidateRec CTACandidate[] , INT32 imgDimensionX, BOOL isLeft,  SimpleCarSpec &i_carSpec );
    INT16 countDownRevise( CandidateRec CTACandidate[] , INT32 imgDimensionX, INT16 i_CTACustROI[4] );
    
    void MergeCandidate( INT16 &revIdx, INT16 &delIdx,CandidateRec CTACandidate[] ,CandidateRec CTACandidatePrev[] );
    Rect_t MergeRect(CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2 );
    FLOAT CandIntersectAreaRatio_Union(CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2);
    FLOAT CandIntersectAreaRatio_Small(CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2);
    INT32 CandIntersectArea(CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2);
    INT16 IsCandIntersection(CandidateRec* CTACandidate1,CandidateRec* CTACandidate2);
    INT16 ShouldCandMerge(CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2);
    INT16 TwoCandDistance( CandidateRec* CTACandidate1,CandidateRec* CTACandidate2 );

    //------------[NightMode]----------
    void GetLampPosNight( HeadLight inlamp[] ,HeadLight inPrevlamp[],UCHAR *inBinImg , INT32 inImgWidth , FLOAT insmoothRatio); 
    
    void GetLampSpeedNight(HeadLight inlamp[] ,HeadLight inPrevlamp[], FLOAT infps, FLOAT insmoothRatio);

    void GetLampTTCNight( HeadLight inlamp[]);

    void CalLampCountDown(HeadLight inlamp[] ,HeadLight inPrevlamp[]);

public:
    //------------[DayMode]-------------
    INT16 CountDownFlagAdpL;	///< Dynamic Tunning Max Flag in LEFT  ROI
    INT16 CountDownFlagAdpR;	///< Dynamic Tunning Max Flag in Right ROI

    INT16 CountDownFlagAdpL_Night;	///< Dynamic Tunning Max Flag in LEFT  ROI
    INT16 CountDownFlagAdpR_Night;	///< Dynamic Tunning Max Flag in Right ROI

    void SetStartupFlag( CandidateRec CTACandidate[] , INT16 inMaxCand );

	UINT16 CandAssociation( CandidateRec CTACandidate[] , INT16 inMaxCand, CandidateRec CTACandidatePrev[] , INT16 inMaxCandPrev , CONST CHAR Dir, CONST FLOAT THD_RelatedCand[], CONST INT16 failedEdgeSuccess);
	INT16 MergeOverlappedCand( CandidateRec CTACandidate[] ,CandidateRec CTACandidatePrev[] ,CONST CHAR Dir, INT16 &inMaxCand);
	void EliminateCrossover( CandidateRec CTACandidateL[] , CandidateRec CTACandidateR[], TARGET_NUM *targetNum,CONST INT32 RegionL[4],CONST INT32 RegionR[4] );

	void UpdateMoveByFPS( CandidateRec CTACandidate[] , INT16 inMaxCand, CandidateRec CTACandidatePrev[] , INT16 inMaxCandPrev , CONST CHAR Dir,CONST CTATimeManagement* CTATime );
	void UpdateBoundaryInfo( CandidateRec CTACandidate[] ,CONST CHAR Dir, CONST INT32 Region[4], CONST INT16 inMaxCand );
    void UpdateBoundaryCounter( CandidateRec CTACandidate[] ,CONST CHAR Dir, CONST INT32 Region[4], CONST INT16 inMaxCand );
	void DecideMoveDir( CandidateRec CTACandidate[], INT16 inMaxCand , CandidateRec CTACandidatePrev[] ,CONST CHAR Dir,CONST CAN_Info *carState, CONST INT32 Region[4] );
    void GetCandInfo( CandidateRec CTACandidate[], INT16 inMaxCand , CandidateRec CTACandidatePrev[] ,UCHAR *Sobel_img, UCHAR *g_GaussImg , INT32 imgDimensionX , const FLOAT THD_WeightSpeed[2], FLOAT infps, UCHAR *dst_Y, SimpleCarSpec &i_carSpec, INT16 i_CTACustROI[4] );
    void UpdateFlagTHD( CandidateRec CTACandidateL[] , CandidateRec CTACandidateR[], CONST INT32 inMaxCandL, CONST INT32 inMaxCandR );
    void EstimationPatch();

    //------------[NightMode]----------
    UINT16 LampAssociation( HeadLight lamp[MAX_HeadLightNum] , INT16 in_MaxNum, HeadLight lampPrev[MAX_HeadLightNum], INT16 in_MaxNumPrev , const FLOAT THD_RelatedLamp[2] );
    void LampSuperglue( HeadLight lampPrev[MAX_HeadLightNum] , INT16 &MaxNumPrev, HeadLight lamp[MAX_HeadLightNum] , INT16 &MaxNum , UCHAR *g_Gaussian_img , UCHAR *g_Gaussian_PrevImg, INT32 imgDimensionX );
    void   LampMergeIn2Frame(HeadLight lamp[MAX_HeadLightNum] , UINT16 in_MaxNum, HeadLight lampPrev[MAX_HeadLightNum] , UINT16 in_MaxNumPrev  );
    void GetLampInfo( UCHAR *inBinImg,INT32 inImgWidth, HeadLight lamp[MAX_HeadLightNum], INT16 in_MaxNum ,HeadLight lampPrev[MAX_HeadLightNum], INT16 in_MaxNumPrev ,FLOAT infps, const FLOAT THD_WeightSpeed[2] );
    
    void   UpdateFlagTHD_Night(HeadLight lampL[MAX_HeadLightNum] ,HeadLight lampR[MAX_HeadLightNum] );

    Estimation(void);
    ~Estimation(void);

    static Estimation* getInstance();
    static void ResetInstance();
};

#endif // end of _Estimation_H_