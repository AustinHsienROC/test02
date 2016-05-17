#ifdef _WIN32
  #include "stdafx.h"
#else
  #include "GlobalTypes.h"
#endif
#include <string.h>
#include "Patches.h"
#include "CTA_Def.h"

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
Patches* Patches::instance = NULL;

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

/**
* @brief           Check whether the (x,y) in the Candidate
* @return          is (x,y) in this Candidate
*/
inline BOOL Patches::IsInCandRect(CONST INT16 x,CONST INT16 y, CONST CandidateRec* CTACandidate )
{
	return ( x>=CTACandidate->startxi && x<CTACandidate->startxi+CTACandidate->width &&
		y>=CTACandidate->startyi && y<CTACandidate->startyi+CTACandidate->height);
}
/**
* @brief           回傳 無使用的 CTACandidate Index (ex.LabelCount=7 , return 15, 
* @n               means new a CTACandidate[15].GroupID = 8)
* @param[in]       CTACandidate Rectangle candidate
* @param[in,out]   LabelCount   Group ID
* @return          new CTACandidate idx
*/
INT16 Patches::GetNewGroupID( CandidateRec CTACandidate[] , UINT16* LabelCount )
{
	for( INT32 i=0 ; i < MAXCandidateNum; i++)
	{
		if( 0 == CTACandidate[i].GroupID ) // Find the un-used CTACandidate.
		{
			if( LabelCount!=NULL )         //assign the new GroupID for new candidates
				CTACandidate[i].GroupID = ++(*LabelCount);
			return i;
		}
	}
	return -1;
}

///************************************
/// Method:    EdgeTracking
/// 1). tracking point selection
/// 2). motion search
/// 3). decision making by ObjSpeed
/// Parameter: CandidateRec CTACandidate[]
/// Parameter: CandidateRec CTACandidatePrev[]
///************************************
inline INT16 Patches::FindLREdgePoint(CandidateRec CTACandidatePrev[],CONST INT16 i,CONST CHAR Dir,CONST UCHAR* img_SblEdgePrev,BOOL &foundLRedge,INT16 &leftPos,INT16 &rightPos)
{
	CONST INT16 sampleHeight=CTACandidatePrev[i].height*2/3;//height in the white rectangle (0~height-1)
	CONST INT16 edgeValueTH=80;
	CONST INT16 edgeCountTH=10;
	
	INT32 idx;
	INT16 minEdgeX=SrcImg_W,maxEdgeX=0;
	INT16 j;
	//	Rect_t ROI_edge; 

#pragma region //========Find edge point=======
	idx=(CTACandidatePrev[i].startyi+sampleHeight)*SrcImg_W+CTACandidatePrev[i].startxi;
	for(j=0; j<CTACandidatePrev[i].width; j++,idx++)
	{
		if(img_SblEdgePrev[idx]>edgeValueTH)//1.find left/right edge point at specified height in the rectangle
		{
			maxEdgeX=MAX(maxEdgeX,j);
			minEdgeX=MIN(minEdgeX,j);
			foundLRedge=TRUE;
		}
	}
	if(foundLRedge)
	{
		leftPos=CTACandidatePrev[i].startxi+minEdgeX;
		rightPos=CTACandidatePrev[i].startxi+maxEdgeX;
	}
#pragma endregion //========Find edge point=======
	
	return sampleHeight;
}

//assign bottom corner to CTACandidatePrev[i]
inline void Patches::FindBottomCorner(CandidateRec CTACandidatePrev[], CONST INT16 i, CONST CHAR Dir, 
                                      CONST UCHAR *harrisMapPrev, CONST HarrisCornerPos *harrisCornersPrev, CONST HarrisCornerPos2 *harrisCorners2Prev, 
                                      TARGET_NUM *pTargetNum, BOOL &foundLeftCorner,BOOL &foundRightCorner)
{
	INT16 j;
	Rect_t ROI_bottomLeft, ROI_bottomRight;

#pragma region //========Find bottom corner=======
	ROI_bottomLeft.start_col=CTACandidatePrev[i].startxi;
	ROI_bottomLeft.start_row=CTACandidatePrev[i].startyi+(CTACandidatePrev[i].height>>1);
	ROI_bottomLeft.width=(CTACandidatePrev[i].width>>1);
	ROI_bottomLeft.height=(CTACandidatePrev[i].height>>1);
	ROI_bottomRight.start_col=CTACandidatePrev[i].startxi+(CTACandidatePrev[i].width>>1);
	ROI_bottomRight.start_row=ROI_bottomLeft.start_row;//CTACandidatePrev[i].startyi+(CTACandidatePrev[i].height>>1);
	ROI_bottomRight.width=(CTACandidatePrev[i].width>>1);
	ROI_bottomRight.height=ROI_bottomLeft.height;//(CTACandidatePrev[i].height>>1);
/*	
	if(Dir=='R')
	{
		if( CTACandidatePrev[i].ObjMoveXY[0]<0 && 
			CTACandidatePrev[i].boundaryType!=noBound &&
			CTACandidatePrev[i].isInReviveMode!=No_Tracking)
			ROI_bottomLeft.width=MAX(0,CTACandidatePrev[i].ObjTrackPosxy[0]-CTACandidatePrev[i].startxi);
	}
	else//Dir=='L'
	{	
		if( CTACandidatePrev[i].ObjMoveXY[0]>0 && 
			CTACandidatePrev[i].boundaryType!=noBound &&
			CTACandidatePrev[i].isInReviveMode!=No_Tracking)
			ROI_bottomLeft.width=MAX(0,CTACandidatePrev[i].ObjTrackPosxy[0]-CTACandidatePrev[i].startxi);
	}
*/
	CONST INT16 bottomLeft_x=ROI_bottomLeft.start_col;
	CONST INT16 bottomLeft_y=ROI_bottomLeft.start_row+ROI_bottomLeft.height;
	CONST INT16 bottomRight_x=ROI_bottomRight.start_col+ROI_bottomRight.width;
	CONST INT16 bottomRight_y=ROI_bottomRight.start_row+ROI_bottomRight.height;

	INT16 bottomLeftValue=9999,bottomRightValue=9999; 
	INT16 bL,bR;

	//for(j=0; j<harrisCorners2Prev->total_points; j++)
    for(j=0; j<pTargetNum->FeaturePtNumPrev; j++)
	{
		if( !harrisCorners2Prev[j].isValid )
			continue;
		
		//search & assign bottomLeft corner
		if(IsInRect(harrisCornersPrev[j].x,harrisCornersPrev[j].y,&ROI_bottomLeft))
		{
			bL=(harrisCornersPrev[j].x-bottomLeft_x)+(bottomLeft_y-harrisCornersPrev[j].y);
			if(bL<bottomLeftValue)
			{
				foundLeftCorner=TRUE;
				bottomLeftValue=bL;
				CTACandidatePrev[i].BottomLeftCorner[0]=harrisCornersPrev[j].x;
				CTACandidatePrev[i].BottomLeftCorner[1]=harrisCornersPrev[j].y;
			}
		}
		//search & assign bottomRight corner
		if(IsInRect(harrisCornersPrev[j].x,harrisCornersPrev[j].y,&ROI_bottomRight))
		{
			bR=(bottomRight_x-harrisCornersPrev[j].x)+(bottomRight_y-harrisCornersPrev[j].y);
			if(bR<bottomRightValue)
			{
				foundRightCorner=TRUE;
				bottomRightValue=bR;
				CTACandidatePrev[i].BottomRightCorner[0]=harrisCornersPrev[j].x;
				CTACandidatePrev[i].BottomRightCorner[1]=harrisCornersPrev[j].y;
			}
		}
	}
#pragma endregion //========Find bottom corner=======
}

inline BOOL Patches::IsInRect(CONST INT16 x,CONST INT16 y, CONST Rect_t* ROI )
{    
	return ( x>=ROI->start_col && x<ROI->start_col+ROI->width &&
		y>=ROI->start_row && y<ROI->start_row+ROI->height);
}
inline TypeBoundCondition Patches::IsPointAtScreenBoundary( CONST INT16 pt_x,CONST INT16 pt_y, CONST INT32 Region[4] )
{
	INT16 boundL=45,boundR=45,boundD=45;

	if( pt_x -boundL<= 0)
		return leftBound;
	else if(pt_x+boundR>=SrcImg_W)
		return rightBound;
	else if(pt_y <=Region[1])
		return upperBound;
	else if(pt_y+boundD>=Region[3])
		return lowerBound;
	else
		return noBound;

/*
	if( pt_x -boundL<= Region[0])
		return leftBound;
	else if(pt_x+boundR>=Region[2])
		return rightBound;
	else if(pt_y <=Region[1])
		return upperBound;
	else if(pt_y+boundD>=Region[3])
		return lowerBound;
	else
		return noBound;
*/
}
//************************************
// Method:    EdgeTracking  find corespondent candidate
// FullName:  Point_Track::EdgeTracking
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CandidateRec CTACandidate[]
// Parameter: CandidateRec CTACandidatePrev[]
// Parameter: UCHAR * Sbl_img
// Parameter: UCHAR * Sbl_imgPrev
// Parameter: UCHAR * FD_Binary  Frame Differnece
// Parameter: UINT16 & LabelCount: currently used label# (starts from 1)
// Parameter:CONST INT16 TH_failedEdgeTimes: max acceptable EdgeTracking times
//************************************
UINT8 Patches::EdgeTracking( CandidateRec CTACandidate[], CandidateRec CTACandidatePrev[], CONST INT32 Region[4], CONST CHAR Dir, 
	CONST UCHAR* img_track, CONST UCHAR* img_trackPrev, CONST UCHAR* img_SblEdgePrev, 
	CONST UCHAR *harrisMapPrev, CONST HarrisCornerPos *harrisCornersPrev, CONST HarrisCornerPos2 *harrisCorners2Prev, 
	TARGET_NUM *pTargetNum, INT16 &inMaxCand,CONST INT16 inMaxCandPrev ,CONST INT32 imgDimensionX,
	CONST CTATimeManagement* CTATime, CONST INT16 TH_failedEdgeTimes )
{
#define TRACKING_USE_GROUND_SPEED

	CONST INT16 TH_boundary[2]={5,20};//frames that obj keeps at the boundary {static condition, has moving}
	CONST INT16 TH_horiMoveAwaySpd=5;//horizontal speed TH of the Obj
	CONST FLOAT TH_horiCornerMove=0.2;//horizontal displacement TH of Corner
	CONST INT16 TH_staticCounter[2]={8,4};//how long the "continuously judged as static" car will be removed {for small appearCounter, for large appearCounter}
	CONST INT16 TH_AppearCounter_static=10;//how long the existed car will start judging as static or not
	CONST INT16 TH_AppearCounter_moveDir=10;//how long the existed car will lock itself moving direction
	CONST INT16 TH_boundRectangleWidth=(INT16)(0.4*(Region[2]-Region[0]));//if width of obj < the Value, boundaryCount is activated
    CONST INT16 TH_minRectangleWidth  = ROISegBlockH+20   ; //min acceptable width for boundary obj
    CONST INT16 TH_minRectangleHeight = ROISegBlockV+10   ; //min acceptable height for boundary obj
	CONST INT16 TH_smallObjSize = 1500;
	CONST INT16 TH_spdToEnablePredict=10/SPEED_CONST_KMH;
    CONST INT16 TH_feasibleSAD[2]={20,40};
	CONST INT16 TH_TrkPTdistance[3]={1800,800,35};//{horizontal distance(cm) to start, horizontal distance(cm) to end, appearCount to end}, move TrackPT mask to prevent tracking fail due to too much road area  

	INT16 boundaryCountTH=TH_boundary[1];
	INT16 CarHeadxy[2];
    INT16 startxi, startyi;
    INT16 width, height;
    INT16 candIdx;
    INT16 moveX,moveY;
	UINT8 ret=0;
	INT16 i;

	BOOL crossover;//center zone is under crossover
	BOOL updateTrackMoveOnly;
	INT16 preCanIndex=inMaxCandPrev;
	INT16 candMaxIndex=inMaxCand; 
    
	for ( i = 0 ; i < preCanIndex ; ++i) 
    {
        if( CTACandidatePrev[i].isValid==Invalid )
            continue;
        if(CTACandidatePrev[i].retrieveEdgeTimes>=TH_failedEdgeTimes)
		{
			CTACandidatePrev[i].isValid=Invalid;
            continue;
		}

		if(CTACandidatePrev[i].appearCounter>=TH_AppearCounter_static)
		{
			if(CTACandidatePrev[i].staticCounter>=TH_staticCounter[1])//continuously static obj is not tracked//
		{
			CTACandidatePrev[i].isValid=Invalid;
			continue;
		}
		}
		else //<TH_AppearCounter_static
		{
			if(CTACandidatePrev[i].staticCounter>=TH_staticCounter[0])//continuously static obj at initial is not tracked//
			{
				CTACandidatePrev[i].isValid=Invalid;
				continue;
			}
		}
		
		if(CTACandidatePrev[i].isRelatedCandValid==TRUE)
		{
			updateTrackMoveOnly=TRUE;
			candIdx=CTACandidatePrev[i].relatedCandLink;
		}
		else
		{
			updateTrackMoveOnly=FALSE;
//			candIdx=-1;
		}
		crossover = FALSE;
		if(Dir=='L')
		{
			if(CTACandidatePrev[i].boundaryType==rightBound)//&& CTACandidatePrev[i].avgObjMoveXY_w[0]>0) //boundary small obj is not tracked
			{
				if(CTACandidatePrev[i].width<=TH_minRectangleWidth)
				{
					CTACandidatePrev[i].isValid=Invalid;
					continue;
				}
				if(CTACandidatePrev[i].moveDir==dir_Right)
					crossover=TRUE;
			}
			if(CTACandidatePrev[i].CornerMove[0]<-TH_horiCornerMove && CTACandidatePrev[i].ObjSpeedXY[0]<-TH_horiMoveAwaySpd)//obj moving away is not tracked
			{
				CTACandidatePrev[i].isValid=Invalid;
				continue;
			}
		}
		else//Dir=='R'
		{
			if(CTACandidatePrev[i].boundaryType==leftBound) //&& CTACandidatePrev[i].avgObjMoveXY_w[0]<0) //boundary small obj is not tracked
			{
				if(CTACandidatePrev[i].width<=TH_minRectangleWidth)
				{
					CTACandidatePrev[i].isValid=Invalid;
					continue;
				}
				if(CTACandidatePrev[i].moveDir==dir_Left)
					crossover=TRUE;
			}
			if(CTACandidatePrev[i].CornerMove[0]>TH_horiCornerMove && CTACandidatePrev[i].ObjSpeedXY[0]>TH_horiMoveAwaySpd)//obj moving away is not tracked
			{
				CTACandidatePrev[i].isValid=Invalid;
						continue;
			}
		}
#pragma region //========Select Tracking Position=========

		INT16 leftPos=0,rightPos=0;//position of edge point at Left/Right of the rectangle, will be updated
		BOOL foundLeftCorner=FALSE,foundRightCorner=FALSE;//flag: is the left-bottom/right-bottom corner found, will be updated
		BOOL foundLRedge=FALSE;//flag: is the left/right point found, will be updated
		INT16 sampleHeight;//will be updated
		BOOL feasibleTrackPosxy=TRUE;

		//judge ObjTrackPosxy[0] is feasible or not
        //if(CTACandidatePrev[i].ObjTrackSAD>TH_feasibleSAD[1])
        //		feasibleTrackPosxy=FALSE;

        if(feasibleTrackPosxy==TRUE)
		if(ABS(CTACandidatePrev[i].ObjSpeedXY[0])>=TH_spdToEnablePredict)
		if(CTACandidatePrev[i].ObjMovexy[0]*CTACandidatePrev[i].CornerMove[0]>0)
		if(CTACandidatePrev[i].moveDir!=dir_Unknown)
		if(CTACandidatePrev[i].ObjTrackMovexy[0]==0)//tracking vector is different from other info
			feasibleTrackPosxy=FALSE;

		if(feasibleTrackPosxy==TRUE)
		if(CTACandidatePrev[i].moveDir!=dir_Unknown)
		if(CTACandidatePrev[i].boundaryType==noBound)
		if(!IsInCandRect(CTACandidatePrev[i].ObjTrackPosxy[0],CTACandidatePrev[i].ObjTrackPosxy[1],&CTACandidatePrev[i]))//tracking position is out of rect
			feasibleTrackPosxy=FALSE;

		if(Dir=='R')
		{
			if( CTACandidatePrev[i].boundaryType==leftBound || CTACandidatePrev[i].boundaryType==lowerBound )//L: left/bottom boundary
			{
#if DBG_INFO==ON
				//DumpValueToText("g_Sobel_imgPrev.txt",0,160,170,300,SrcImg_W,img_SblEdgePrev);
#endif
				if(/*CTACandidatePrev[i].boundaryCount>=2 && */CTACandidatePrev[i].isInReviveMode!=No_Tracking
					&& IsPointAtScreenBoundary(CTACandidatePrev[i].ObjTrackPosxy[0],CTACandidatePrev[i].ObjTrackPosxy[1],Region)!=leftBound)//use previous tracking point
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjTrackPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjTrackPosxy[1];
				}
				else if(IsInCandRect(CTACandidatePrev[i].ObjPosxy[0],CTACandidatePrev[i].ObjPosxy[1],&CTACandidatePrev[i]))//use car head point
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjPosxy[1];
				}
				else //default position
                {
					FindBottomCorner(CTACandidatePrev,i,Dir,harrisMapPrev,harrisCornersPrev,harrisCorners2Prev, pTargetNum, foundLeftCorner,foundRightCorner);
					if(foundLeftCorner)
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomLeftCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomLeftCorner[1];
					}
/*					else if(foundRightCorner)
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomRightCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomRightCorner[1];
					}*/
					else
					{
						sampleHeight=FindLREdgePoint(CTACandidatePrev,i,Dir,img_SblEdgePrev,foundLRedge,leftPos,rightPos);
						if(foundLRedge)
						{
							if(CTACandidatePrev[i].moveDir!=0)
								CarHeadxy[0]=leftPos;//use edge point at the right most of the center row 
							else
								CarHeadxy[0]=rightPos;//use edge point at the right most of the center row 
							CarHeadxy[1]=CTACandidatePrev[i].startyi+sampleHeight;
						}
						else
						{
							CarHeadxy[0]=CTACandidatePrev[i].startxi+CTACandidatePrev[i].width-1;//use grid 6 in 9-grid-zone as tracking point
							CarHeadxy[1]=CTACandidatePrev[i].startyi+(CTACandidatePrev[i].height>>1);
						}
					}
                }
			}
			else if(CTACandidatePrev[i].boundaryType==rightBound && CTACandidatePrev[i].width<100)//R: right boundary
			{
				FindBottomCorner(CTACandidatePrev,i,Dir,harrisMapPrev,harrisCornersPrev, harrisCorners2Prev, pTargetNum, foundLeftCorner,foundRightCorner);

				if(foundLeftCorner)//use corner
				{
					CarHeadxy[0]=CTACandidatePrev[i].BottomLeftCorner[0];
					CarHeadxy[1]=CTACandidatePrev[i].BottomLeftCorner[1];
				}
				else
				{
					CarHeadxy[0]=CTACandidatePrev[i].startxi;//use left-bottom of the rectangle as tracking point
					CarHeadxy[1]=CTACandidatePrev[i].startyi + CTACandidatePrev[i].height-1;
				}
			}
			else//R @ typical case
			{
				if(feasibleTrackPosxy && CTACandidatePrev[i].isInReviveMode!=No_Tracking)//use previous tracking point
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjTrackPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjTrackPosxy[1];
				}
				else if(IsInCandRect(CTACandidatePrev[i].ObjPosxy[0],CTACandidatePrev[i].ObjPosxy[1],&CTACandidatePrev[i]))//use car head as tracking point
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjPosxy[1];
				}
				else
				{
					FindBottomCorner(CTACandidatePrev,i,Dir,harrisMapPrev,harrisCornersPrev, harrisCorners2Prev, pTargetNum, foundLeftCorner,foundRightCorner);

					if(foundLeftCorner)//use corner as tracking point
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomLeftCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomLeftCorner[1];
					}
					else if(foundRightCorner)
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomRightCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomRightCorner[1];
					}
					else if(CTACandidatePrev[i].size<TH_smallObjSize)
					{
						CarHeadxy[0]=CTACandidatePrev[i].startxi + (CTACandidatePrev[i].width>>1);//以白框中央當作追蹤點//
						CarHeadxy[1]=CTACandidatePrev[i].startyi + (CTACandidatePrev[i].height>>1);
					}
					else
					{
						CarHeadxy[0]=CTACandidatePrev[i].startxi + (CTACandidatePrev[i].width>>1);//use grid 8 in 9-grid-zone as tracking point
						CarHeadxy[1]=CTACandidatePrev[i].startyi + CTACandidatePrev[i].height-1;
//						CarHeadxy[1]=CTACandidatePrev[i].startyi + (CTACandidatePrev[i].height>>1);
					}
				}
			}
		}
		else//********************************** Dir=='L' **********************************************
		{
			if( CTACandidatePrev[i].boundaryType==rightBound || CTACandidatePrev[i].boundaryType==lowerBound )//L: right/bottom boundary case
			{
				if(/*CTACandidatePrev[i].boundaryCount>=2 &&*/ CTACandidatePrev[i].isInReviveMode!=No_Tracking
					&& IsPointAtScreenBoundary(CTACandidatePrev[i].ObjTrackPosxy[0],CTACandidatePrev[i].ObjTrackPosxy[1],Region)!=rightBound)
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjTrackPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjTrackPosxy[1];
				}
				else if(IsInCandRect(CTACandidatePrev[i].ObjPosxy[0],CTACandidatePrev[i].ObjPosxy[1],&CTACandidatePrev[i]))//use car head
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjPosxy[1];
				}
				else //default position
                {				
					FindBottomCorner(CTACandidatePrev,i,Dir,harrisMapPrev,harrisCornersPrev,harrisCorners2Prev, pTargetNum, foundLeftCorner,foundRightCorner);
					if(foundRightCorner)
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomRightCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomRightCorner[1];
					}
/*					else if(foundLeftCorner)
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomLeftCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomLeftCorner[1];
					}*/
					else
					{
						sampleHeight=FindLREdgePoint(CTACandidatePrev,i,Dir,img_SblEdgePrev,foundLRedge,leftPos,rightPos);
						if(foundLRedge)
						{
							if(CTACandidatePrev[i].moveDir!=0)
								CarHeadxy[0]=rightPos;
							else
								CarHeadxy[0]=leftPos;//以白框中央最左方的edge點當作追蹤點//
							CarHeadxy[1]=CTACandidatePrev[i].startyi+sampleHeight;
						}
						else
						{
							CarHeadxy[0]=CTACandidatePrev[i].startxi;//以白框左方中央當作追蹤點//
							CarHeadxy[1]=CTACandidatePrev[i].startyi+(CTACandidatePrev[i].height>>1);
						}
					}
                }
			}
			else if(CTACandidatePrev[i].boundaryType==leftBound && CTACandidatePrev[i].width<100)//L @ leftBound
			{
				FindBottomCorner(CTACandidatePrev,i,Dir,harrisMapPrev,harrisCornersPrev, harrisCorners2Prev, pTargetNum, foundLeftCorner,foundRightCorner);
				if(foundRightCorner)//use corner as tracking point
				{
					CarHeadxy[0]=CTACandidatePrev[i].BottomRightCorner[0];
					CarHeadxy[1]=CTACandidatePrev[i].BottomRightCorner[1];
				}
				else
				{
					CarHeadxy[0]=CTACandidatePrev[i].startxi+ CTACandidatePrev[i].width-1;//以白框右下角當作追蹤點//
					CarHeadxy[1]=CTACandidatePrev[i].startyi + CTACandidatePrev[i].height-1;
				}
			}
			else//L @ typical case
			{
				if(feasibleTrackPosxy && CTACandidatePrev[i].isInReviveMode!=No_Tracking)//use previous tracking point
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjTrackPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjTrackPosxy[1];
				}
				else if(IsInCandRect(CTACandidatePrev[i].ObjPosxy[0],CTACandidatePrev[i].ObjPosxy[1],&CTACandidatePrev[i]))//use car head as tracking point
				{
					CarHeadxy[0]=CTACandidatePrev[i].ObjPosxy[0];
					CarHeadxy[1]=CTACandidatePrev[i].ObjPosxy[1];
				}
				else
				{
					FindBottomCorner(CTACandidatePrev,i,Dir,harrisMapPrev,harrisCornersPrev,harrisCorners2Prev,pTargetNum, foundLeftCorner,foundRightCorner);
					if(foundRightCorner)//use corner as tracking point
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomRightCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomRightCorner[1];
					}
					else if(foundLeftCorner)//use corner as tracking point
					{
						CarHeadxy[0]=CTACandidatePrev[i].BottomLeftCorner[0];
						CarHeadxy[1]=CTACandidatePrev[i].BottomLeftCorner[1];
					}
					else if(CTACandidatePrev[i].size<TH_smallObjSize)
					{
						CarHeadxy[0]=CTACandidatePrev[i].startxi + (CTACandidatePrev[i].width>>1);//以白框中央當作追蹤點//
						CarHeadxy[1]=CTACandidatePrev[i].startyi + (CTACandidatePrev[i].height>>1);
					}
					else
					{
						CarHeadxy[0]=CTACandidatePrev[i].startxi + (CTACandidatePrev[i].width>>1);//以白框下方中央當作追蹤點//
						CarHeadxy[1]=CTACandidatePrev[i].startyi + CTACandidatePrev[i].height-1;
					}
				}
			}
		}
#pragma endregion========Select Tracking Position=========

#pragma region //========Predict Current Tracking Position=========
		
		BOOL predictMotionBySpd=FALSE;
		BOOL jumpSearch=FALSE;
		BOOL unlimitedSearch=FALSE;
		MVector_t jump_mv={0,0,0x7FFF};
		INT32 groundxy_prev[2]={0,0},groundxy[2]={0,0}; //data is re-assigned in multiple locations in the function
		INT32 groundXY_prev[2]={0,0},groundXY[2]={0,0}; //data is re-used in multiple locations in the function 
		INT32 prev_Xdistance=0;//shift origin to car's rear
		
		if( CTACandidatePrev[i].isObjSpeedXYValid==TRUE && ABS(CTACandidatePrev[i].ObjSpeedXY[0])<=TH_spdToEnablePredict)//no jumping at low speed
		{
			//speed is too slow
		}
		else if(CTACandidatePrev[i].isObjSpeedXYValid==TRUE && 	CTACandidatePrev[i].ObjSpeedXY[0]*CTACandidatePrev[i].ObjTrackMovexy[0]>0)//the same direction
		{
			jumpSearch=TRUE;
		}
		else if(crossover)
		{
			jumpSearch=TRUE;
			unlimitedSearch=TRUE;
		}
		else if(Dir=='L')
		{
			if( CTACandidatePrev[i].isInReviveMode!=No_Tracking && CTACandidatePrev[i].ObjTrackSAD<=TH_feasibleSAD[0] )//sad is good enough
			{
				jumpSearch=TRUE;
			}
			else if(CTACandidatePrev[i].moveDir==dir_Right)
			{
				jumpSearch=TRUE;
//				jump_mv.x+=MIN(7,ABS(CTACandidatePrev[i].ObjTrackMovexy[0])>>1);
			}
		}
		else//Dir=='R'
		{
			if( CTACandidatePrev[i].isInReviveMode!=No_Tracking && CTACandidatePrev[i].ObjTrackSAD<=TH_feasibleSAD[0] )//sad is good enough
            {
				jumpSearch=TRUE;
			}
			else if(CTACandidatePrev[i].moveDir==dir_Left)
			{
				jumpSearch=TRUE;
//				jump_mv.x-=MIN(7,ABS(CTACandidatePrev[i].ObjTrackMovexy[0])>>1);
			}
		}

		if(jumpSearch==TRUE)//predict motion
		{
#ifdef TRACKING_USE_GROUND_SPEED
			//look for ground coordinate
			groundxy_prev[0]=CarHeadxy[0];
			groundxy_prev[1]=CarHeadxy[1];//(CarHeadxy[1]+(CTACandidatePrev[i].startyi+CTACandidatePrev[i].height))/2;

			if(CTACandidatePrev[i].isObjSpeedXYValid==TRUE)//predict by ground speedXY
			{
				predictMotionBySpd=WModel->PosImgToWorld( groundXY_prev[0],groundXY_prev[1], groundxy_prev[0], groundxy_prev[1] );
				prev_Xdistance=XwShiftToBackCenter(groundXY_prev[0]);
				groundXY[0]=groundXY_prev[0]+CTACandidatePrev[i].ObjSpeedXY[0]/CTATime->fps;
				groundXY[1]=groundXY_prev[1];
				predictMotionBySpd=predictMotionBySpd && WModel->PosWorldToImg(groundXY[0]/CTA_TV_COMPRESS,groundXY[1]/CTA_TV_COMPRESS,groundxy[0],groundxy[1]);
			}
/*			if(predictMotionBySpd==FALSE)			
			if(CTACandidatePrev[i].isInReviveMode!=No_Tracking && CTACandidatePrev[i].ObjTrackSAD<=TH_feasibleSAD[0])//predict by tracking movementXY		
			{
				predictMotionBySpd=WModel->PosImgToWorld( groundXY_prev[0],groundXY_prev[1], groundxy_prev[0], groundxy_prev[1] );
				groundXY[0]=groundXY_prev[0]+CTACandidatePrev[i].ObjTrackMoveXY[0];
				groundXY[1]=groundXY_prev[1]++CTACandidatePrev[i].ObjTrackMoveXY[1];
				predictMotionBySpd=predictMotionBySpd && WModel->PosWorldToImg(groundXY[0]/CTA_TV_COMPRESS,groundXY[1]/CTA_TV_COMPRESS,groundxy[0],groundxy[1]);
			}
*/
			if(predictMotionBySpd==TRUE)
			{
				jump_mv.x=(groundxy[0]-groundxy_prev[0]);
				jump_mv.y=(groundxy[1]-groundxy_prev[1]);
			}
			else	
#endif
			{
				jump_mv.x=CTACandidatePrev[i].ObjTrackMovexy[0];
				jump_mv.y=CTACandidatePrev[i].ObjTrackMovexy[1];
			}
		}
/*
		CONST INT16 maxLength=CTACandidatePrev[i].width*CTACandidatePrev[i].height;
		INT16 length=0;
		INT8 vx=0,vy=0;
		INT16 coordX,coordY;
		
		TRACK->SpiralOrder(vx,vy,TRUE);//initial 

//		if(CTACandidatePrev[i].isInReviveMode!=No_Tracking)
		if(CTACandidatePrev[i].ObjTrackPosxy[0]!=0 || CTACandidatePrev[i].ObjTrackPosxy[1]!=0)
		{
			while(length<maxLength)
			{
				length=TRACK->SpiralOrder(vx,vy);
				if(Dir=='L' && vx<0 && vy<0)//the second quadrant is not considered
						{
//					if(CTACandidatePrev[i].avgObjMoveXY_w[0]>0)//moving right
						continue;
						}
				else if(Dir=='R' && vx>0 && vy<0)//the first quadrant is not considered
				{
//					if(CTACandidatePrev[i].avgObjMoveXY_w[0]<0)//moving left
						continue;
					}

				coordX=CTACandidatePrev[i].ObjTrackPosxy[0]+vx;
				coordY=CTACandidatePrev[i].ObjTrackPosxy[1]+vy;
				if(! IsInCandRect(coordX,coordY,&CTACandidatePrev[i]))//must in the rectangle
					continue;

				idx=coordY*SrcImg_W+coordX;
				if(harrisMapPrev[idx]>0)
				{
					CTACandidatePrev[i].ObjTrackPosxy[0]=coordX;//update tracking point to corner point in the neighbor
					CTACandidatePrev[i].ObjTrackPosxy[1]=coordY;
					break;
				}
			}
		}
*/
#pragma endregion //========Predict Current Tracking Position=========

#if 0 //find edge of car body
		//edge的使用可參考CandidateLocate(.)
		INT16 col_sum[SrcImg_W];
		INT16 col_width;
		UINT32 max_sum=0,sum=0;
		INT16 max_row=0,max_col=0;
		INT32 idx,idx_start;
		INT16 k;
		memset(col_sum,0,sizeof(col_sum));
		idx_start=CTACandidatePrev[i].startyi*SrcImg_W+CTACandidatePrev[i].startxi;
		for(k=0; k<CTACandidatePrev[i].height; k++,idx_start+=SrcImg_W)
		{
			sum=0;
			for(j=0, idx=idx_start; j<CTACandidatePrev[i].width; j++,idx++)
			{
				col_sum[j]
				sum+=img_edge[idx];
			}
			if(max_sum<sum)
			{
				max_sum=sum;
				max_col=j+CTACandidatePrev[i].startxi;
			}
		}
#endif

#pragma region========Car Head Motion Estimation=========
		CONST INT16 maskWidth   = ROISegBlockH;
		CONST INT16 maskHeight  = ROISegBlockV;
		CONST INT32 maskSize = maskWidth * maskHeight;
		CONST INT32 th_array[4] = { /*TRACK->p_th_array[0]*maskWidth*maskHeight/MATCH_BLOCK_WIDTH/MATCH_BLOCK_HEIGHT*/
           99999,-999,-999,20*maskSize};
		UCHAR src_msk[ maskSize ];
		UCHAR dst_msk[ maskSize ];
		MVector_t mv_sum={0,0,0x7FFF};			        ///< Motion vector value at( corner_x, corner_y)
		TypeMoveDir moveDirForTracking,moveDir;
		INT16 trkPT_yOffset=0;//shift y for tracking point
		INT16 CarHeadxy_dist_x=(Dir=='L')?CarHeadxy[0]-Region[0]:Region[2]-CarHeadxy[0];//x distance between car head and boundary

		moveDir=CTACandidatePrev[i].moveDir;

		if(predictMotionBySpd==TRUE)
			moveDirForTracking=dir_Unknown;
		else
			moveDirForTracking=moveDir;

		if(ABS(CTACandidatePrev[i].appearPosXY[0])>=TH_TrkPTdistance[0])//special process for starting tracking at far away
		if(CTACandidatePrev[i].appearCounter<=TH_TrkPTdistance[2])
		if(CarHeadxy_dist_x<=(Region[2]-Region[0])/3)
//		if(predictMotionBySpd==FALSE || ABS(prev_Xdistance)>=TH_TrkPTdistance[1]) //A+A'B=A+B
			trkPT_yOffset=(maskHeight*2/3);

		if(jumpSearch)//predict current moving
		{
			mv_sum = TRACK->ME_Hexagon_Search_iterative_tracking(img_track , img_trackPrev, SrcImg_W,               //Img
				CarHeadxy[0] , CarHeadxy[1]-trkPT_yOffset,//Src point position
				CarHeadxy[0]+jump_mv.x , CarHeadxy[1]-trkPT_yOffset+jump_mv.y,//Dst point position
				maskWidth , maskHeight,
				maskSize,
				TRACK->MATCH_SHIFT_BIT , th_array, src_msk, dst_msk,	//"TRACK->MATCH_SHIFT_BIT" equals to "MATCH_SHIFT_BIT"
				moveDirForTracking,
				unlimitedSearch,crossover);
		}
		else
		{
			mv_sum = TRACK->ME_Hexagon_Search_iterative_tracking(img_track , img_trackPrev, SrcImg_W,               //Img
				CarHeadxy[0] , CarHeadxy[1]-trkPT_yOffset,//Src point position
				CarHeadxy[0] , CarHeadxy[1]-trkPT_yOffset,//Dst point position                                                                            //PointPosition
				maskWidth , maskHeight,
				maskSize,
				TRACK->MATCH_SHIFT_BIT , th_array,
				src_msk, dst_msk,
				CTACandidatePrev[i].moveDir,
				unlimitedSearch,crossover);
/*
			mv_sum = TRACK->ME_Full_Search_Spiral( img_track , img_trackPrev ,NULL, SrcImg_W,         //Img
				CarHeadxy[0] , CarHeadxy[1]-trkPT_yOffset),   //PointPosition
				maskWidth , maskHeight,
				maskSize,
				TRACK->MATCH_SHIFT_BIT, th_array,
				src_msk, dst_msk, CTACandidatePrev[i].moveDir);//加入moveDirPrev限制MV方向//
		
			mv_sum=TRACK->ME_SmallDimondBlockSearch(img_track,img_trackPrev, SrcImg_W,
				CarHeadxy[0], CarHeadxy[1]-trkPT_yOffset,
				CarHeadxy[0], CarHeadxy[1]-trkPT_yOffset,
				maskWidth , maskHeight,
				maskSize,
				TRACK->MATCH_SHIFT_BIT , th_array,
				src_msk, dst_msk,
				CTACandidatePrev[i].moveDir,
				2,5,
				unlimitedSearch,crossover);

			mv_sum = TRACK->ME_BBGDS_Search(img_track , img_trackPrev, SrcImg_W, //Img
				CarHeadxy[0] , CarHeadxy[1]-trkPT_yOffset,     //PointPosition
				CarHeadxy[0] , CarHeadxy[1]-trkPT_yOffset,     //PointPosition
				maskWidth , maskHeight,
				maskSize,
				TRACK->MATCH_SHIFT_BIT , th_array,
				src_msk, dst_msk);
*/
		}

		if(mv_sum.x==e_mv_invalid)
		{
			if(CTACandidatePrev[i].isObjSpeedXYValid==TRUE && CTACandidatePrev[i].ObjSpeedXY[0]*CTACandidatePrev[i].CornerMove[0]>0)
            {
				moveX=CTACandidatePrev[i].ObjMovexy[0];
				moveY=CTACandidatePrev[i].ObjMovexy[1];
				mv_sum.minSAD=0xFFFF;
			}
			else
			{
				moveX=CTACandidatePrev[i].ObjTrackMovexy[0];
				moveY=CTACandidatePrev[i].ObjTrackMovexy[1];
				mv_sum.minSAD=0x7FFF;
			}
		}
		else
		{
			if(jumpSearch)
			{
				mv_sum.x+=jump_mv.x;
				mv_sum.y+=jump_mv.y;
			}
			moveX=mv_sum.x;
			moveY=mv_sum.y;
		}

		if(Dir=='L')
		{
			if(moveX==0 && moveY==0)//solve: obj moves to the other side of region, but the rectangle was left at original place and was static
			if(CTACandidatePrev[i].boundaryType==rightBound)// || CTACandidatePrev[i].boundaryType==upperBound )
				boundaryCountTH=TH_boundary[0];
			if(moveX<0 && CTACandidatePrev[i].ObjTrackMovexy[0]<0)//遠離車輛不追蹤//
			{
				if(updateTrackMoveOnly)
				{
//					moveDir=dir_Left;
					CTACandidatePrev[i].isValid = Invalid; 
					CTACandidate[ candIdx].isValid = Invalid; 
					continue;
				}
				else
				{
					CTACandidatePrev[i].isValid=Invalid;
					continue;
				}
			}
			if(CTACandidatePrev[i].appearCounter>=TH_AppearCounter_moveDir)
			if(moveX>0 && CTACandidatePrev[i].ObjTrackMovexy[0]>0)//靠近車輛鎖定追蹤方向//
				moveDir=dir_Right;
		}
		else//Dir=='R'
		{
			if(moveX==0 && moveY==0)
			if(CTACandidatePrev[i].boundaryType==leftBound)// || CTACandidatePrev[i].boundaryType==upperBound )
				boundaryCountTH=TH_boundary[0];
			if(moveX>0 && CTACandidatePrev[i].ObjTrackMovexy[0]>0)//遠離車輛不追蹤//
			{
				if(updateTrackMoveOnly)
				{
//					moveDir=dir_Right;
					CTACandidatePrev[i].isValid = Invalid; 
					CTACandidate[ candIdx].isValid = Invalid; 
					continue;
				}
				else
				{
					CTACandidatePrev[i].isValid=Invalid;
					continue;
				}
			}
			if(CTACandidatePrev[i].appearCounter>=TH_AppearCounter_moveDir)
			if(moveX<0 && CTACandidatePrev[i].ObjTrackMovexy[0]<0)//靠近車輛鎖定追蹤方向//
				moveDir=dir_Left;
		}
		if(CTACandidatePrev[i].boundaryCount>=boundaryCountTH && CTACandidatePrev[i].width<TH_boundRectangleWidth)//solve: 追蹤框逗留太久//
		{
			CTACandidatePrev[i].isValid=Invalid;
			continue;
		}
#pragma endregion //========Car Head Motion Estimation=========

		if(updateTrackMoveOnly==FALSE)//pureTracking
		{
			INT16 endxi, endyi;

#pragma region//======Modify Rect @ PureTracking======
			BOOL modifyRect=FALSE;
			INT16 ground_mv[2]={0,0};

			if(Dir=='L')
			{
				if(moveDir==dir_Right && CTACandidatePrev[i].boundaryType==rightBound)//current obj is crossover
				{
					groundxy_prev[0]=CTACandidatePrev[i].startxi;//position to be modified
					groundxy_prev[1]=CTACandidatePrev[i].startyi;
					modifyRect=TRUE;
				}
			}
			else//Dir=='R'
			{
				if(moveDir==dir_Left && CTACandidatePrev[i].boundaryType==leftBound)//current obj is crossover
				{
					groundxy_prev[0]=CTACandidatePrev[i].startxi + CTACandidatePrev[i].width;//position to be modified
					groundxy_prev[1]=CTACandidatePrev[i].startyi + CTACandidatePrev[i].height;
					modifyRect=TRUE;
				}
			}

			if(modifyRect)
			{
				if(predictMotionBySpd==FALSE)//if predictMotionBySpd==TRUE, groundXY_prev[2] are known already
				{
					modifyRect=modifyRect && WModel->PosImgToWorld( groundXY_prev[0],groundXY_prev[1], CarHeadxy[0], CarHeadxy[1] );
				}
				modifyRect=modifyRect && WModel->PosImgToWorld( groundXY[0],groundXY[1], CarHeadxy[0]+moveX, CarHeadxy[1]+moveY );
				ground_mv[0]=groundXY[0]-groundXY_prev[0];
				ground_mv[1]=groundXY[1]-groundXY_prev[1];//now calculated ground movement of tracking point

				//calculate rectangle boundary movement at crossover
				modifyRect=modifyRect && WModel->PosImgToWorld( groundXY_prev[0],groundXY_prev[1], groundxy_prev[0], groundxy_prev[1] );
				groundXY[0]=ground_mv[0]+groundXY_prev[0];
				groundXY[1]=ground_mv[1]+groundXY_prev[1];
				modifyRect=modifyRect && WModel->PosWorldToImg(groundXY[0]/CTA_TV_COMPRESS,groundXY[1]/CTA_TV_COMPRESS,groundxy[0],groundxy[1]);
			}
		
			//===========pure edge tracking ============
			startxi = CTACandidatePrev[i].startxi + moveX;
			startyi = CTACandidatePrev[i].startyi + moveY;
			endxi   = CTACandidatePrev[i].startxi + CTACandidatePrev[i].width  + moveX;
			endyi   = CTACandidatePrev[i].startyi + CTACandidatePrev[i].height + moveY;

			if(modifyRect==TRUE)
			{
				if(Dir=='L')
					startxi=MAX(startxi,groundxy[0]);//move dir_Right
				else//Dir=='R'
					endxi=MIN(endxi,groundxy[0]);//move dir_Left
			}
#pragma endregion//======Modify Rect @ PureTracking======

			startxi = MAX_MIN_RANGE(startxi,Region[0],Region[2]);
			startyi = MAX_MIN_RANGE(startyi,Region[1],Region[3]);
			endxi   = MAX_MIN_RANGE(endxi,Region[0],Region[2]);
			endyi   = MAX_MIN_RANGE(endyi,Region[1],Region[3]);
			width   = MAX( endxi-startxi , 0 );
			height  = MAX( endyi-startyi , 0 );

			// 當框接觸ROI邊緣且變小時//
			if( width <= TH_minRectangleWidth )
			{
				if(( Dir=='R' && startxi <= Region[0] ) || (Dir=='L' && endxi>=Region[2]))
				{
					CTACandidatePrev[i].isValid=Invalid;
					continue;
				}
			}
			if( height <= TH_minRectangleHeight)
			{
				if(endyi>=Region[3])
				{
					CTACandidatePrev[i].isValid=Invalid;
					continue;
				}
			}

			candIdx = GetNewGroupID( CTACandidate );
			if( -1 == candIdx) 
			{
				CTACandidatePrev[i].isValid=Invalid;
				continue;
			}

			CTACandidate[candIdx]                    = CTACandidatePrev[i]; //copy all
			CTACandidate[candIdx].startxi            = startxi;
			CTACandidate[candIdx].startyi            = startyi;
			CTACandidate[candIdx].width              = width;
			CTACandidate[candIdx].height             = height;
			CTACandidate[candIdx].size               = CTACandidate[candIdx].width*CTACandidate[candIdx].height;

			CTACandidate[candIdx].relatedCandLink    = i;
			CTACandidate[candIdx].isRelatedCandValid = TRUE;
//			CTACandidate[candIdx].isValid            = Valid;
	//		CTACandidate[candIdx].boundaryCount    = CTACandidatePrev[i].boundaryCount;
	//		CTACandidate[candIdx].boundaryType    = CTACandidatePrev[i].boundaryType;
	//      CTACandidate[candIdx].countDown          = CTACandidatePrev[i].countDown;
	//		CTACandidate[candIdx].appearCounter=CTACandidatePrev[i].appearCounter;
			CTACandidate[candIdx].isInReviveMode  =  PureTracking;

//調整retrieveEdgeTimes//20141210 spike:僅spatial上的比對,其實需要temporal上的//
#if 0

		if(CTACandidatePrev[i].avgObjMoveXY_w[1]<0 && CTACandidatePrev[i].ObjPosXY[1]<150)//旁邊車即將超過自車//
		{//check whether side car is true?
			int leftX=CTACandidate[candIdx].startxi;
			int rightX=CTACandidate[candIdx].startxi+CTACandidate[candIdx].width;
			int topY=CTACandidate[candIdx].startyi;
			int bottomY=CTACandidate[candIdx].startyi+CTACandidate[candIdx].height;
			int area=(rightX-leftX)*(bottomY-topY);
			int grayLT=0, grayRT=0, grayLB=0, grayRB=0;
			UINT32 voffset;
			for(int j=topY; j<topY+20; j++)//區域內上下左右對稱性//
			{
				voffset=j*imgDimensionX;
				for(int i=leftX; i<leftX+20; i++)
				{
					grayLT+=*(img_track+voffset+i);
				}
			}
			grayLT=grayLT/400;

			for(int j=bottomY; j>bottomY-20; j--)//區域內上下左右對稱性//
			{
				voffset=j*imgDimensionX;
				for(int i=leftX; i<leftX+20; i++)
				{
					grayLB+=*(img_track+voffset+i);
				}
			}
			grayLB=grayLB/400;

			for(int j=topY; j<topY+20; j++)//區域內上下左右對稱性//
			{
				voffset=j*imgDimensionX;
				for(int i=rightX; i>rightX-20; i--)
				{
					grayRT+=*(img_track+voffset+i);
				}
			}
			grayRT=grayRT/400;

			for(int j=bottomY; j>bottomY-20; j--)//區域內上下左右對稱性//
			{
				voffset=j*imgDimensionX;
				for(int i=rightX; i>rightX-20; i--)
				{
					grayRB+=*(img_track+voffset+i);
				}
			}
			grayRB=grayRB/400;
			if( (ABS(grayLT-grayLB)<10) && 
				(ABS(grayRT-grayRB)<10) && 
				(MIN(grayLT,grayRT)*1.35>(float)MAX(grayLT,grayRT)) &&
				(MIN(grayLB,grayRB)*1.35>(float)MAX(grayLB,grayRB)) )
				CTACandidate[candIdx].retrieveEdgeTimes  = MAX(0, CTACandidatePrev[i].retrieveEdgeTimes -1) ;
		}
		//another case: 旁車退離開//
		else
#endif//#if 0			
	
			if(mv_sum.minSAD>30)//too noisy//
				CTACandidate[candIdx].retrieveEdgeTimes  = CTACandidatePrev[i].retrieveEdgeTimes + 1 ;
			else if(moveX==0 && CTACandidatePrev[i].ObjTrackMovexy[0]==0 && CTACandidatePrev[i].appearCounter>=TH_AppearCounter_static)//不追蹤靜止車輛//
					CTACandidate[candIdx].retrieveEdgeTimes  = CTACandidatePrev[i].retrieveEdgeTimes + 1 ;
			else if(mv_sum.minSAD<20 && moveX!=0)//good tracking
				CTACandidate[candIdx].retrieveEdgeTimes  = MAX(0, CTACandidatePrev[i].retrieveEdgeTimes -1) ;

//			if(moveX==0 && CTACandidatePrev[i].appearCounter>=TH_AppearCounter_static)//不追蹤靜止車輛(function is repeated)//
			if(moveX==0)//不追蹤靜止車輛(function is repeated)//
				CTACandidate[candIdx].staticCounter = CTACandidatePrev[i].staticCounter+1;
			else
				CTACandidate[candIdx].staticCounter=0;

			CTACandidatePrev[i].isRelatedCandValid   = TRUE;
			CTACandidatePrev[i].relatedCandLink      = candIdx;

			candMaxIndex=MAX(candIdx+1,candMaxIndex);
			++ret;
		}
		else
		{
			CTACandidate[candIdx].isInReviveMode  =  UpdateMove;
		}
		CTACandidate[candIdx].ObjTrackPosxy[0]    = CarHeadxy[0]+moveX;
		CTACandidate[candIdx].ObjTrackPosxy[1]    = CarHeadxy[1]+moveY;
		CTACandidate[candIdx].ObjTrackMovexy[0]  = moveX;
		CTACandidate[candIdx].ObjTrackMovexy[1]  = moveY;
		CTACandidate[candIdx].ObjTrackSAD = mv_sum.minSAD;
		CTACandidate[candIdx].moveDir  =  moveDir;
    }
	inMaxCand=candMaxIndex;
    return ret;
}
/**
* @brief	 Filter the candidates according to previous ones
* @param[in] CTACandidate		Input Candidates at current frame
* @param[in] CandidateCount     Input the Valid Number in current CTACandidate
* @return	 the result valid number
*/
UINT16 Patches::FilterAfterPredict( CandidateRec CTACandidate[], UINT16 inMaxCand , CandidateRec CTACandidatePrev[], UINT16 inMaxCandPrev )
{
    ///1) DO Filter 
    for ( int i1 = 0 ; i1 < MAXCandidateNum ; ++i1) // CTACandidate Total Number
    {
        if( (Invalid == CTACandidate[i1].isValid) && 
            ( CTACandidate[i1].isRelatedCandValid==FALSE)) 
            continue;

        //Variables
        //CTACandidate[i1];                                     ///< the CTACandidate (T)
        //CTACandidatePrev[CTACandidate[i1].relatedPrevCand];   ///<the  CTACandidate (T-1)
        //CTACandidate[i1].isValid                              ///<the  data
        
        //@TODO: The FilterIII
    }

    //2) count the isValid
    UINT16 countValid = 0 ;
    for ( INT32 i1 = 0 ; i1 < MAXCandidateNum ; ++i1) // CTACandidate Total Number
    {
        countValid = ((Valid == CTACandidate[i1].isValid))?(countValid+1):(countValid);
    }
    return countValid;
}

/**
* @brief	 FilterI after cluster into HeadLightL/R & calculate lampNumR/L
*/
void Patches::Filter01_Night( UCHAR *g_Binar_img, INT32 imgDimensionX, INT32 imgDimensionY, HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] , INT16 MaxLampL , INT16 MaxLampR, INT16 &lampNumL, INT16 &lampNumR,const INT32 RegionL[4], const INT32 RegionR[4] )
{
    /// small area filter 
    CONST UINT16 THD_area = 8;  ///< Pixel
    lampNumL = 0;
    lampNumR = 0;

    for ( INT16 i1 = 0 ; i1< MaxLampL ; ++i1)
    {
        if( (TRUE == lampL[i1].isValid) &&
            (THD_area > lampL[i1].area) )
        {
            lampL[i1].isValid = FALSE;
        }
        if( (TRUE == lampL[i1].isValid) )
        {
            ++lampNumL ;
        }
    }
    for ( INT16 i1 = 0 ; i1< MaxLampR ; ++i1)
    {
        if( (TRUE == lampR[i1].isValid) &&
            (THD_area > lampR[i1].area) )
        {
            lampR[i1].isValid = FALSE;
        }
        if( (TRUE == lampR[i1].isValid) )
        {
            ++lampNumR ;
        }
    }

    return;
}
/**
* @brief	 Save the Map[only for 640*480] into HarrisCornerPos structure
* @return	 the total corner number
*/
INT32 Patches::SaveCornerPos( UCHAR *harrisMap, INT32 imgDimensionX, INT32 imgDimensionY, HarrisCornerPos *harrisCorners, INT32 MAXSize )
{

    UINT16 starty = RegionL[1];
    UINT16 endy   = RegionL[3];
    UINT16 Idx_harrisCorners = 0;    
    UINT16 End_Idx           = MAXSizeHarrisCorner - 1 ; 
    BOOL   isBreak           = FALSE;

    INT32 offsetV;
    for (INT16 vert = endy - 1; vert >= starty ; --vert) // scan from bottom to top
    {
        offsetV = vert * imgDimensionX;
        for ( INT16 hori = 0 ; hori < imgDimensionX ;++hori)
        {
            if( 0 == harrisMap[ offsetV + hori ]  ) 
                continue;

            harrisCorners[Idx_harrisCorners].x = hori;
            harrisCorners[Idx_harrisCorners].y = vert;
            if( (++Idx_harrisCorners >= End_Idx) )
            {
                isBreak = TRUE;
                break;
            }
        } // end of hori < imgDimensionX
        if( TRUE == isBreak )
        {
            break;
        }
    }// end of vert <= endy

    return Idx_harrisCorners; // ex. 500 means 500 corners in harrisCorners

}


Patches* Patches::getInstance()
{
    if(!instance)
        instance = new Patches;
    return instance;
}

Patches::Patches(void)
{
}


Patches::~Patches(void)
{
}

void Patches::ResetInstance()
{
    delete instance; 
    instance = NULL;
}



