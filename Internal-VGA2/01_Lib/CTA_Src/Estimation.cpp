#ifdef _WIN32
  #include "stdafx.h"
#else // K2 platform
  #include "GlobalTypes.h"
#endif
#include <string.h>
#include "Estimation.h"
#include "CTA_Def.h"
//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
Estimation* Estimation::instance = NULL;

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

//************************************
// Method:  Merge CTACandidate[revIdx] & CTACandidate[delIdx]
// FullName:  Estimation::MergeCandidate
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: INT16 revIdx revive Idx
// Parameter: INT16 delIdx delete Idx
// Parameter: CandidateRec CTACandidate[]
// Parameter: CandidateRec CTACandidatePrev[]
//************************************
void Estimation::MergeCandidate( INT16 &revIdx, INT16 &delIdx,CandidateRec CTACandidate[] ,CandidateRec CTACandidatePrev[] )
{
    CTACandidate[delIdx].isValid=Invalid;

    Rect_t dim=MergeRect(&CTACandidate[revIdx],&CTACandidate[delIdx]);
    CTACandidate[revIdx].startxi=dim.start_col;
    CTACandidate[revIdx].startyi=dim.start_row;
    CTACandidate[revIdx].width=dim.width;
    CTACandidate[revIdx].height=dim.height;
    CTACandidate[revIdx].size=dim.size;

	if(CTACandidate[delIdx].appearCounter>=CTACandidate[revIdx].appearCounter)
	{
		CTACandidate[revIdx].appearPosxy[0]=CTACandidate[delIdx].appearPosxy[0];
		CTACandidate[revIdx].appearPosxy[1]=CTACandidate[delIdx].appearPosxy[1];
		CTACandidate[revIdx].appearPosXY[0]=CTACandidate[delIdx].appearPosXY[0];
		CTACandidate[revIdx].appearPosXY[1]=CTACandidate[delIdx].appearPosXY[1];
	}

    CTACandidate[revIdx].appearCounter=MAX(CTACandidate[revIdx].appearCounter,CTACandidate[delIdx].appearCounter);
    CTACandidate[revIdx].staticCounter=MIN(CTACandidate[revIdx].staticCounter,CTACandidate[delIdx].staticCounter);
    CTACandidate[revIdx].bondaryLR[0]=CTACandidate[revIdx].startxi;
    CTACandidate[revIdx].bondaryLR[1]=CTACandidate[revIdx].startxi+CTACandidate[revIdx].width;
    CTACandidate[revIdx].boundaryCount=MAX(CTACandidate[revIdx].boundaryCount,CTACandidate[delIdx].boundaryCount);
    CTACandidate[revIdx].boundaryType=MAX(CTACandidate[revIdx].boundaryType,CTACandidate[delIdx].boundaryType);
    CTACandidate[revIdx].CandTTC=MIN(CTACandidate[revIdx].CandTTC,CTACandidate[delIdx].CandTTC);
    if(CTACandidate[revIdx].isRelatedCandValid==TRUE && CTACandidate[delIdx].isRelatedCandValid==TRUE)////countdown value is meaningful only while cand link is successful
        CTACandidate[revIdx].countDown=MIN(CTACandidate[revIdx].countDown,CTACandidate[delIdx].countDown);
    else if(CTACandidate[delIdx].isRelatedCandValid==TRUE)//means CTACandidate[revIdx].isRelatedCandValid==FALSE
        CTACandidate[revIdx].countDown=CTACandidate[delIdx].countDown;

	CTACandidate[revIdx].PosXYKeepTime=MAX(CTACandidate[revIdx].PosXYKeepTime,CTACandidate[delIdx].PosXYKeepTime);

    if(CTACandidate[delIdx].isObjPosxyValid==TRUE && CTACandidate[revIdx].isObjPosxyValid==FALSE)
    {
        CTACandidate[revIdx].ObjPosxy[0]=CTACandidate[delIdx].ObjPosxy[0];
        CTACandidate[revIdx].ObjPosxy[1]=CTACandidate[delIdx].ObjPosxy[1];
    }
    if(CTACandidate[delIdx].isObjPosXYValid==TRUE && CTACandidate[revIdx].isObjPosXYValid==FALSE)
    {
        CTACandidate[revIdx].ObjPosXY[0]=CTACandidate[delIdx].ObjPosXY[0];
        CTACandidate[revIdx].ObjPosXY[1]=CTACandidate[delIdx].ObjPosXY[1];
    }
    if(CTACandidate[delIdx].isObjSpeedXYValid==TRUE && CTACandidate[revIdx].isObjSpeedXYValid==FALSE)
    {
        CTACandidate[revIdx].ObjSpeedXY[0]=CTACandidate[delIdx].ObjSpeedXY[0];
        CTACandidate[revIdx].ObjSpeedXY[1]=CTACandidate[delIdx].ObjSpeedXY[1];
    }
    if(CTACandidate[revIdx].isRelatedCandValid==FALSE && CTACandidate[delIdx].isRelatedCandValid==TRUE)
    {
        CTACandidate[revIdx].isRelatedCandValid=TRUE;
        CTACandidate[revIdx].relatedCandLink=CTACandidate[delIdx].relatedCandLink;
        CTACandidatePrev[CTACandidate[revIdx].relatedCandLink].relatedCandLink=revIdx;
        CTACandidate[revIdx].ObjTrackPosxy[0]=CTACandidate[delIdx].ObjTrackPosxy[0];
        CTACandidate[revIdx].ObjTrackPosxy[1]=CTACandidate[delIdx].ObjTrackPosxy[1];
        CTACandidate[revIdx].ObjTrackMovexy[0]=CTACandidate[delIdx].ObjTrackMovexy[0];
		CTACandidate[revIdx].ObjTrackMovexy[1]=CTACandidate[delIdx].ObjTrackMovexy[1];
        CTACandidate[revIdx].ObjTrackSAD=CTACandidate[delIdx].ObjTrackSAD;
    }
    CTACandidate[revIdx].retrieveEdgeTimes=MAX(CTACandidate[revIdx].retrieveEdgeTimes,CTACandidate[delIdx].retrieveEdgeTimes);

	CTACandidate[revIdx].moveDir=MAX(CTACandidate[revIdx].moveDir,CTACandidate[delIdx].moveDir);
//    CTACandidate[revIdx].passThrough=MAX(CTACandidate[revIdx].passThrough,CTACandidate[delIdx].passThrough);

    ///merge other elements & remove CTACandidate[delIdx]
    ///need to be appended by Austin	
    //???CTACandidate[revIdx].GroupID=???
}

//return merged rectangle from CTACandidate1 & CTACandidate2
Rect_t Estimation::MergeRect(CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2 )
{
    INT16 endx1 = CTACandidate1->startxi + CTACandidate1->width;
    INT16 endy1 = CTACandidate1->startyi + CTACandidate1->height;
    INT16 endx2 = CTACandidate2->startxi + CTACandidate2->width;
    INT16 endy2 = CTACandidate2->startyi + CTACandidate2->height;
    Rect_t ret;

    ret.start_col = MIN(CTACandidate1->startxi,CTACandidate2->startxi);
    ret.start_row = MIN(CTACandidate1->startyi,CTACandidate2->startyi);
    ret.width     = MAX(endx1,endx2)-ret.start_col;
    ret.height    = MAX(endy1,endy2)-ret.start_row;
    ret.size      = ret.width*ret.height;
    return ret;
}
//************************************
// Method:    檢查小框的質心是否在大框中
// FullName:  Estimation::ShouldCandMerge
// Access:    public 
// Returns:   INT16
// Qualifier:
// Parameter: CandidateRec * CTACandidate
// Parameter: UINT8 canIdx1
// Parameter: UINT8 canIdx2
// Return: 
// (0):小框質心不在大框中
// (1):小框質心在大框中, 且CTACandidate1是小框
// (2):小框質心在大框中, 且CTACandidate2是小框
//************************************
INT16 Estimation::ShouldCandMerge( CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2 )
{
    CONST CandidateRec* smallCand, *bigCand;
    INT16 centerX,centerY;
    INT16 ret=0;
    if(CTACandidate1->size<CTACandidate2->size)
    {
        smallCand=CTACandidate1;
        bigCand=CTACandidate2;
        ret=1;
    }
    else
    {
        smallCand=CTACandidate2;
        bigCand=CTACandidate1;
        ret=2;
    }

    centerX=smallCand->startxi+(smallCand->width>>1);
    centerY=smallCand->startyi+(smallCand->height>>1);
    //not the larger one must be better, need to modify
    if(smallCand->appearCounter>bigCand->appearCounter)
        return 0;
    if( centerX>=bigCand->startxi && centerX<bigCand->startxi+bigCand->width &&
        centerY>=bigCand->startyi && centerY<bigCand->startyi+bigCand->height )
    {
        //check width-height ratio
        int newLeft=MIN(bigCand->startxi,smallCand->startxi);
        int newRight=MAX(bigCand->startxi+bigCand->width,smallCand->startxi+smallCand->width);
        int newTop=MIN(bigCand->startyi,smallCand->startyi);
        int newBottom=MAX(bigCand->startyi+bigCand->height,smallCand->startyi+smallCand->height);
        if((FLOAT)(newRight-newLeft)/(FLOAT)(newBottom-newTop)>2.4)
            return 0;
        int overlapArea= ABS(CandIntersectArea(smallCand, bigCand));
        FLOAT overlapRatio=(FLOAT)overlapArea/(FLOAT)(smallCand->size+bigCand->size-overlapArea);
        if(overlapRatio==1.0)
            return ret;
        if(overlapArea>13000 || overlapRatio<0.38)//two large area, no recommend to merge
            return 0;
        return ret;
    }
    else
        return 0;
}

//回傳交集比例=交集面積/兩框聯集面積---
FLOAT Estimation::CandIntersectAreaRatio_Union( CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2 )
{
    INT32 union_area;
    INT32 insect_area;
    insect_area=CandIntersectArea(CTACandidate1,CTACandidate2);
    union_area=CTACandidate1->size+CTACandidate2->size-insect_area;
    return (FLOAT)insect_area/union_area;
}
//回傳交集比例=交集面積/小框面積---
FLOAT Estimation::CandIntersectAreaRatio_Small( CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2 )
{
    INT32 union_area;
    INT32 insect_area;
    insect_area=CandIntersectArea(CTACandidate1,CTACandidate2);
    union_area=MIN(CTACandidate1->size,CTACandidate2->size);
    return (FLOAT)insect_area/union_area;
}
//回傳交集面積
INT32 Estimation::CandIntersectArea( CONST CandidateRec* CTACandidate1,CONST CandidateRec* CTACandidate2 )
{
    BOOL found=FALSE;
    INT32 area_insect=0;
    INT16 type=0;
//	INT16 startx1,endx1,starty1,endy1;
//	INT16 startx2,endx2,starty2,endy2;
    INT32 Ax,Bx,Ay,Cy;
    INT32 ax,bx,ay,cy;
    
    Ax=CTACandidate1->startxi;
    Bx=CTACandidate1->startxi+CTACandidate1->width;
    Ay=CTACandidate1->startyi;
    Cy=CTACandidate1->startyi+CTACandidate1->height;

    ax=CTACandidate2->startxi;
    bx=CTACandidate2->startxi+CTACandidate2->width;
    ay=CTACandidate2->startyi;
    cy=CTACandidate2->startyi+CTACandidate2->height;

    if(!found)
    if(Bx>ax)
    if(Bx<bx)
    if(Ax<=ax)
    if(cy>Ay)//2,3,4,5,6
    if(ay<Ay)//2,6
    {
        if(cy>Cy)//6
            type=16;
        else//2
            type=12;
        found=TRUE;
    }
    else if(ay<Cy)//3,5
    {
        if(cy>Cy)//3
            type=13;
        else//5
            type=15;
        found=TRUE;
    }
    //else //4

    if(!found)
    if(ax>=Ax)
    if(Bx>=bx)
    if(cy>Ay)//2,3,4,5,6
    if(ay<Cy)//2,3,4,6
    if(ay<Ay)//2,3
    {
        if(cy<=Cy)//2
            type=22;
        else//3
            type=23;
        found=TRUE;
    }
    else//4,6
    {
        if(cy<=Cy)//6
            type=26;
        else//4
            type=24;
        found=TRUE;
    }

    if(!found)
    if(ax<Ax)
    if(bx>Ax)
    if(Bx>=bx)
    if(cy>Ay)//2,3,4,5,6
    if(ay<Ay)//2,6
    {
        if(cy<=Cy)//2
            type=32;
        else//6
            type=36;
        found=TRUE;
    }
    else if(ay<Cy)//3,5
    {
        if(cy>Cy)//3
            type=33;
        else//5
            type=35;
        found=TRUE;
    }
    //else//4

    if(!found)
    if(ax<=Ax)
    if(bx>=Bx)
    if(ay<=Ay)//1,2,5
    {
    if(cy>=Cy)//2
        {
        type=42;
            found=TRUE;
        }
        else if(cy>Ay)//5
        {
            type=45;
            found=TRUE;
        }
    }
    else if(ay<Cy)//4
    {
        type=44;
        found=TRUE;
    }

    switch(type)
    {
    case 12:
        area_insect=(Bx-ax)*(cy-Ay);
        break;
    case 13:
        area_insect=(Bx-ax)*(Cy-ay);
        break;
    case 15:
        area_insect=(Bx-ax)*(cy-ay);
        break;
    case 16:
        area_insect=(Bx-ax)*(Cy-Ay);
        break;
    case 22:
        area_insect=(bx-ax)*(cy-Ay);
        break;
    case 23:
        area_insect=(bx-ax)*(Cy-Ay);
        break;
    case 24:
        area_insect=(bx-ax)*(Cy-ay);
        break;
    case 26:
        area_insect=(bx-ax)*(cy-ay);
        break;
    case 32:
        area_insect=(bx-Ax)*(cy-Ay);
        break;
    case 33:
        area_insect=(bx-Ax)*(Cy-ay);
        break;
    case 35:
        area_insect=(bx-Ax)*(cy-ay);
        break;
    case 36:
        area_insect=(bx-Ax)*(Cy-Ay);
        break;
    case 42:
        area_insect=(Bx-Ax)*(Cy-Ay);
        break;
    case 44:
        area_insect=(Bx-Ax)*(Cy-ay);
        break;
    case 45:
        area_insect=(Bx-Ax)*(cy-Ay);
        break;
    }
#if (DBG_INFO==ON)
    assert(area_insect>=0);
#endif
    return area_insect;
}
//************************************
// Method:    Check if the two candidates are overlapped
// FullName:  Estimation::IsCandIntersection
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: CandidateRec * CTACandidate1
// Parameter: CandidateRec * CTACandidate2
// Return: 
// (0):兩框不交集
// (1):兩框交集, 且CTACandidate1是小框
// (2):兩框交集, 且CTACandidate2是小框
//************************************
INT16 Estimation::IsCandIntersection( CandidateRec* CTACandidate1,CandidateRec* CTACandidate2 )
{
    INT16 CheckX_A, CheckX_B,CheckY_A,CheckY_B;
    INT16 CheckW_A, CheckW_B, CheckH_A, CheckH_B;

    INT16 ret_FALSE=0,ret_TRUE;
    if(CTACandidate1->size<CTACandidate2->size)
        ret_TRUE=1;
    else
        ret_TRUE=2;

    CheckX_A = CTACandidate1->startxi;
    CheckX_B = CTACandidate2->startxi;
    CheckY_A = CTACandidate1->startyi;
    CheckY_B = CTACandidate2->startyi;

    CheckW_A = CTACandidate1->width;
    CheckW_B = CTACandidate2->width;
    CheckH_A = CTACandidate1->height;
    CheckH_B = CTACandidate2->height;

    //check x
    if (CheckX_A < CheckX_B)
    {
        if ((CheckX_A + CheckW_A) < CheckX_B)
            return ret_FALSE;

        // intersection at Axis-X
        // Check if intersection at Axis-Y
        if (CheckY_A < CheckY_B)
        {
            if ((CheckY_A + CheckH_A) < CheckY_B)
                return ret_FALSE;

            return ret_TRUE;
        }
        else
        {
            if ((CheckY_B + CheckH_B) < CheckY_A)
                return ret_FALSE;

            return ret_TRUE;
        }
    }
    else
    {
        if ((CheckX_B + CheckW_B) < CheckX_A)
            return ret_FALSE;

        // intersection at Axis-X
        // Check if intersection at Axis-Y
        if (CheckY_A < CheckY_B)
        {
            if ((CheckY_A + CheckH_A) < CheckY_B)
                return ret_FALSE;

            return ret_TRUE;
        }
        else
        {
            if ((CheckY_B + CheckH_B) < CheckY_A)
                return ret_FALSE;

            return ret_TRUE;
        }
    }
    return ret_FALSE;
}

INT16 Estimation::TwoCandDistance( CandidateRec* CTACandidate1,CandidateRec* CTACandidate2 )
{
    INT16 CheckX_A, CheckX_B,CheckY_A,CheckY_B;
    INT16 CheckW_A, CheckW_B, CheckH_A, CheckH_B;
	INT16 distance=0;

    INT16 ret_FALSE=0,ret_TRUE;
    if(CTACandidate1->size<CTACandidate2->size)
        ret_TRUE=1;
    else
        ret_TRUE=2;

    CheckX_A = CTACandidate1->startxi;
    CheckX_B = CTACandidate2->startxi;
    CheckY_A = CTACandidate1->startyi;
    CheckY_B = CTACandidate2->startyi;

    CheckW_A = CTACandidate1->width;
    CheckW_B = CTACandidate2->width;
    CheckH_A = CTACandidate1->height;
    CheckH_B = CTACandidate2->height;

    //check x
    if (CheckX_A < CheckX_B)
    {
        if ((distance=(CheckX_A+CheckW_A-CheckX_B))<0)
            return ABS(distance);

        // intersection at Axis-X
        // Check if intersection at Axis-Y
        if (CheckY_A < CheckY_B)
        {
            if ((distance=(CheckY_A+CheckH_A-CheckY_B))<0)
                return ABS(distance);

            return 0;
        }
        else
        {
            if ((distance=CheckY_B+CheckH_B-CheckY_A)<0)
				return ABS(distance);

            return 0;
        }
    }
    else
    {
        if ((distance=CheckX_B+CheckW_B-CheckX_A)<0)
			return ABS(distance);

        // intersection at Axis-X
        // Check if intersection at Axis-Y
        if (CheckY_A < CheckY_B)
        {
            if ((distance=CheckY_A+CheckH_A-CheckY_B)<0)
				return ABS(distance);

            return ret_TRUE;
        }
        else
        {
            if ((distance=CheckY_B+CheckH_B-CheckY_A)<0)
				return ABS(distance);

            return ret_TRUE;
        }
    }
    return ret_FALSE;
}
//deal with crossover traffic
void Estimation::EliminateCrossover( CandidateRec CTACandidateL[] , CandidateRec CTACandidateR[], TARGET_NUM *targetNum,CONST INT32 RegionL[4],CONST INT32 RegionR[4] )
{
    CONST INT16 appearCounterTH=10;
    CONST INT16 crossCountDownTH=5;
	CONST INT16 boundX_L=RegionL[2]-(RegionL[2]-RegionL[0])/4;
	CONST INT16 boundX_R=RegionR[0]+(RegionR[2]-RegionR[0])/4;

    INT16 crossingID_L=-1,crossingID_R=-1;
	INT16 firstAppearID_Lc=-1,firstAppearID_Rc=-1;//only while crossing
    INT16 firstAppearID_Lb=-1,firstAppearID_Rb=-1;//at boundary
    INT16 i;

    for(i=0; i<targetNum->_L_MaxCandNum; ++i)
    {
        if(CTACandidateL[i].isValid==Invalid)
			continue;
        if(CTACandidateL[i].boundaryType==rightBound)
        {
            if(CTACandidateL[i].appearCounter>=appearCounterTH )
                crossingID_L=i;//(before) crossover L->R
			if(CTACandidateL[i].appearCounter<=1 )
				firstAppearID_Lb=i;
        }
		if(CTACandidateL[i].startxi+CTACandidateL[i].width>boundX_L)
		if(CTACandidateL[i].appearCounter<=1 )
			firstAppearID_Lc=i;
    }		

    for(i=0; i<targetNum->_R_MaxCandNum; ++i)
    {
        if(CTACandidateR[i].isValid==Invalid)
			continue;
        if(CTACandidateR[i].boundaryType==leftBound)
        {
            if(CTACandidateR[i].appearCounter>=appearCounterTH )
                crossingID_R=i;//(before) crossover R->L
			if(CTACandidateR[i].appearCounter<=1 )
				firstAppearID_Rb=i;
        }
		if(CTACandidateR[i].startxi<=boundX_R)
		if(CTACandidateR[i].appearCounter<=1 )
			firstAppearID_Rc=i;
    }

    if(crossingID_L!=-1 || crossingID_R!=-1)
        targetNum->CrossoverCountDown=crossCountDownTH;
    else
        targetNum->CrossoverCountDown=MAX(0,targetNum->CrossoverCountDown-1);

	if(firstAppearID_Rb!=-1)//eliminate R car
		CTACandidateR[firstAppearID_Rb].isValid=Invalid;
	if(firstAppearID_Lb!=-1)//eliminate L car
		CTACandidateL[firstAppearID_Lb].isValid=Invalid;

	if(targetNum->CrossoverCountDown!=0)
    {
        if(firstAppearID_Rc!=-1)//eliminate R car
            CTACandidateR[firstAppearID_Rc].isValid=Invalid;
        if(firstAppearID_Lc!=-1)//eliminate L car
            CTACandidateL[firstAppearID_Lc].isValid=Invalid;
    }
}
/**
* @brief     show the cand in frame T
* @n         
* @param[in] CTACandidate		Input Valid Candidate at current  frame
* @param[in] CTACandidatePrev	Input Valid Candidate at previous frame
* @return						the Number of merged candidates
*/
INT16 Estimation::MergeOverlappedCand( CandidateRec CTACandidate[] ,CandidateRec CTACandidatePrev[] ,CONST CHAR Dir, INT16 &inMaxCand )
{
	CONST FLOAT TH_sizeRatio=6.5;
    CONST FLOAT TH_widthRatio=3;
    CONST FLOAT TH_overlapAreaRatio=0.9;

    INT16 i1,i2;
    INT16 ID_minSize;
    INT16 revIdx,delIdx;//candIdx to be revived or deleted
    INT16 mergeCount=0;
    INT16 mergeRet=0;
	FLOAT sizeRatio,widthRatio,overlapAreaRatio;
//	INT16 groundPos_i1[2],groundPos_i2[2];

    for(i1=0; i1<inMaxCand; ++i1)
    {
        if(CTACandidate[i1].isValid==Valid)
//      if(CTACandidate[i1].isValid>=Startup)//include startup cand
            for(i2=i1+1; i2<inMaxCand; ++i2)
            {
				if(CTACandidate[i2].isValid==Valid)
//			     if(CTACandidate[i2].isValid>=Startup)//include startup cand                
                {
//					if(CTACandidate[i1].appearCounter == CTACandidate[i2].appearCounter)
//					if((mergeRet=IsCandIntersection(&CTACandidate[i1],&CTACandidate[i2]))!=0)
                    if(TwoCandDistance(&CTACandidate[i1],&CTACandidate[i2])<=ROISegBlockH)
//					if((mergeRet=ShouldCandMerge(&CTACandidate[i1],&CTACandidate[i2]))!=0)
                    {
#if 1
                        //overlapAreaRatio=CandIntersectAreaRatio_Union(&CTACandidate[i1],&CTACandidate[i2]);
                        overlapAreaRatio=CandIntersectAreaRatio_Small(&CTACandidate[i1],&CTACandidate[i2]);
						ID_minSize=(CTACandidate[i1].size<CTACandidate[i2].size)? i1:i2;
                        sizeRatio=(float)MAX(CTACandidate[i1].size,CTACandidate[i2].size)/CTACandidate[ID_minSize].size;
						widthRatio=(float)MAX(CTACandidate[i1].width,CTACandidate[i2].width)/MIN(CTACandidate[i1].width,CTACandidate[i2].width);

#endif
//						if((overlapAreaRatio>=TH_overlapAreaRatio) || ((overlapAreaRatio<TH_overlapAreaRatio) && (sizeRatio<TH_sizeRatio)))
                        if( (sizeRatio<TH_sizeRatio && widthRatio<TH_widthRatio) || 
							(CTACandidate[i1].moveDir==CTACandidate[i2].moveDir && CTACandidate[i1].moveDir!=0) ||
							(CTACandidate[i1].ObjTrackMovexy[0]*CTACandidate[i2].ObjTrackMovexy[0]>0))
                        {							
							if(CTACandidate[i1].appearCounter>CTACandidate[i2].appearCounter)
                            {
                                revIdx=i1;
                                delIdx=i2;
                            }
							else
							{ 
                                revIdx=i2;
                                delIdx=i1;
                            }
                            //=========Merge DATA============
                            MergeCandidate( revIdx, delIdx, CTACandidate , CTACandidatePrev );

                            ///merge other elements & remove CTACandidate[delIdx]
                            ///need to be appended by Austin	
                            //???CTACandidate[revIdx].GroupID=???

                            mergeCount++;

                            if(delIdx==i1)
                                break;//break loop i2
                        }
						else if(overlapAreaRatio>TH_overlapAreaRatio)
						{
							CTACandidate[ID_minSize].isValid=Invalid;
						}
					}
				}
			}
    }
    /*// 20150128, close to meet Mark's modification 
    //adjust existCarInfo->currMaxIndex
    for(i1=inMaxCand-1; i1>=0; i1--)
        if(CTACandidate[i1].isValid!=Invalid)
            break;
    inMaxCand=i1+1;
    */
    return mergeCount;
}

void Estimation::SetStartupFlag( CandidateRec CTACandidate[] , INT16 inMaxCand )
{
	INT16 i1;
	for ( i1 = 0 ; i1 < inMaxCand ; ++i1) 
	{
		if(CTACandidate[i1].isValid==Valid)
			CTACandidate[i1].isValid=Startup;
	}
}
void Estimation::UpdateMoveByFPS( CandidateRec CTACandidate[] , INT16 inMaxCand, CandidateRec CTACandidatePrev[] , INT16 inMaxCandPrev , CONST CHAR Dir,CONST CTATimeManagement* CTATime )
{
	DOUBLE timeRatio;//=PRECISION_2((DOUBLE)CTATime->fps/CTATime->fpsPrev);
	INT16 i1;

	if(ABS(CTATime->fps-CTATime->fpsPrev)<2.0)//no dropping frame
		return;
	if(CTATime->fps==0 || CTATime->fpsPrev==0)
		return;

	timeRatio=PRECISION_2((DOUBLE)CTATime->fps/CTATime->fpsPrev);
//	timeRatio=(DOUBLE)CTATime->fps/CTATime->fpsPrev;

	for ( i1 = 0 ; i1 < inMaxCandPrev ; ++i1) // CTACandidatePrev Total Num
	{
		if( Invalid == CTACandidatePrev[i1].isValid ) 
			continue;
		CTACandidatePrev[i1].ObjMovexy[0]=ROUND(CTACandidatePrev[i1].ObjMovexy[0]*timeRatio);
		CTACandidatePrev[i1].ObjMovexy[1]=ROUND(CTACandidatePrev[i1].ObjMovexy[1]*timeRatio);
		CTACandidatePrev[i1].ObjMoveXY[0]=ROUND(CTACandidatePrev[i1].ObjMoveXY[0]*timeRatio);
		CTACandidatePrev[i1].ObjMoveXY[1]=ROUND(CTACandidatePrev[i1].ObjMoveXY[1]*timeRatio);
		CTACandidatePrev[i1].ObjTrackMovexy[0]=ROUND(CTACandidatePrev[i1].ObjTrackMovexy[0]*timeRatio);
		CTACandidatePrev[i1].ObjTrackMovexy[1]=ROUND(CTACandidatePrev[i1].ObjTrackMovexy[1]*timeRatio);
		CTACandidatePrev[i1].CornerMove[0]=ROUND(CTACandidatePrev[i1].CornerMove[0]*timeRatio);
		CTACandidatePrev[i1].CornerMove[1]=ROUND(CTACandidatePrev[i1].CornerMove[1]*timeRatio);
	}
}

/**
* @brief	 Associate from T-1 to T candidates
* @param[in] CTACandidate		Input Valid Candidate at current  frame
* @param[in] CTACandidatePrev	Input Valid Candidate at previous frame
* @param[in] ValidNum			Input Valid CTACandidate Number
* @CONST INT16 failedEdgeSuccess在強制復活前, CandAssociation成功後, 可以消除1次//
* @return						the Number that successfully map from previous to current Candidates.
*/
UINT16 Estimation::CandAssociation( CandidateRec CTACandidate[] , INT16 inMaxCand, CandidateRec CTACandidatePrev[] , INT16 inMaxCandPrev , CONST CHAR Dir, CONST FLOAT THD_RelatedCand[], CONST INT16 failedEdgeSuccess )
{
    CONST INT16 farROIwidth = 90;
    //CONST FLOAT THD_CenterDiff  = THD_RelatedCand[0];	 // REF to BlockSize
	FLOAT THD_AreaRatio = THD_RelatedCand[1];	 ///< 3 or 1/3 times  then Previous 
    
    FLOAT overlapRatio[MAXCandidateNum];//overlap between CTACandidatePrev[i1] & all CTACandidate[0~255] 
    UINT16 ret_validNum = 0 ;          ///< return value
    UINT16 LinkListNum  = 0 ;          ///< length of LinkList
    UINT8  LinkList[ LINK_ARRAY_NUM ]; ///< number of objects in i2 links to obj i1
    BOOL flag_prevAtFar;//Is prevCandidate is at ROI far region
    FLOAT ratio;
    INT16  candIdx;
	INT16 centerX_prev, centerY_prev;
	INT16 centerX, centerY;

	INT32 i1,i2,i3,i4;

    /// initialize Candidate in Previous Frame
    memset(overlapRatio,0,sizeof(overlapRatio[0])*MAXCandidateNum);

    /// Associate from T-1 to T candidates
    for ( i1 = 0 ; i1 < inMaxCandPrev ; ++i1) // CTACandidatePrev Total Num
    {
        if( Invalid == CTACandidatePrev[i1].isValid ) continue;

        /// T-1 candidate
        LinkListNum       =  0 ;
        flag_prevAtFar = FALSE;
		centerX_prev =  CTACandidatePrev[i1].startxi + (CTACandidatePrev[i1].width >>1);
		centerY_prev =  CTACandidatePrev[i1].startyi + (CTACandidatePrev[i1].height>>1);
        
        if(Dir=='L')
        {
            if( centerX_prev<=farROIwidth )
                flag_prevAtFar=TRUE;
        }
        else//Dir=='R'
        {
            if( SrcImg_W-centerX_prev<=farROIwidth )
                flag_prevAtFar=TRUE;
        }		
        
		for ( i2 = 0 ; i2 < inMaxCand ; ++i2)
        {
            if( Invalid == CTACandidate[i2].isValid ) continue;
            /// T valid candidates
            centerX =  CTACandidate[i2].startxi + (CTACandidate[i2].width >>1);
            centerY =  CTACandidate[i2].startyi + (CTACandidate[i2].height>>1);
#if 0 //原本FOR RSW,尚未修改//
            if( ( ABS((centerX-centerX_prev))<= THD_CenterDiff ) &&  /// Satisfy Center Similarity
                ( ABS((centerY-centerY_prev))<= THD_CenterDiff ) )
            {
                INT32 overlapArea= ABS(CandIntersectArea(&CTACandidatePrev[i1],&CTACandidate[i2]));
                FLOAT  AreaRatio= (FLOAT)(overlapArea)/(FLOAT)(CTACandidate[i2].size+ CTACandidatePrev[i1].size- overlapArea);
                overlapRatio[i2]=AreaRatio;
                FLOAT minAreaRatio=(FLOAT)overlapArea/(FLOAT)MIN(CTACandidatePrev[i1].size, CTACandidate[i2].size);
                if((CTACandidatePrev[i1].size- overlapArea)*2< (CTACandidate[i2].size-overlapArea) && CTACandidate[i2].width-CTACandidatePrev[i1].width>5)
                    continue; 
//				if(minAreaRatio>=0.95 && AreaRatio<0.7)//can be changed to use MergeOverlappedCandFinal 
//					continue; 
                if( AreaRatio >= THD_AreaRatio)
                {
                    /// find valid candidates at T
                    LinkList[ MIN( LinkListNum , LINK_ARRAY_NUM-1 ) ] = i2;
                    ++LinkListNum;
                }
            }
            else if(  (overlapArea=ABS(CandIntersectArea(&CTACandidatePrev[i1],&CTACandidate[i2]))) >0)
            {//but overlap?
                //overlapArea= ABS(CandIntersectArea(&CTACandidatePrev[i1],&CTACandidate[i2]));
                overlapRatio[i2]=AreaRatio = (FLOAT)(overlapArea)/(FLOAT)(CTACandidate[i2].size+ CTACandidatePrev[i1].size- overlapArea);
                if(AreaRatio<0.1&& AreaRatio>0)
                {
                    CTACandidate[i2].isValid =Invalid ;
                    continue;
                }
                if(AreaRatio<0.3&& AreaRatio>0 && CTACandidatePrev[i1].size*3<CTACandidate[i2].size )
                {
                    CTACandidate[i2].isValid =Invalid ;
                    continue;
                }
                //if(AreaRatio<0.3&& AreaRatio>0 && CTACandidatePrev[i1].size*1.6<CTACandidate[i2].size && CTACandidatePrev[i1].avgObjSpeedXY[1]<0 
                //	&& CTACandidatePrev[i1].boundaryCount>50 && CTACandidatePrev[i1].boundaryType!=noBound )
                //{
                //	CTACandidate[i2].isValid =Invalid ;
                //	continue;
                //}
            }
#else
            ratio = CandIntersectAreaRatio_Union(&CTACandidatePrev[i1],&CTACandidate[i2]); // Fast speed
            if(flag_prevAtFar)
            {
                if(Dir=='L')
                {
                    if( centerX<=farROIwidth )
                        THD_AreaRatio = THD_RelatedCand[2];
                }
                else//Dir=='R'
                {
                    if( SrcImg_W-centerX<=farROIwidth )
                        THD_AreaRatio = THD_RelatedCand[2];
                }
            }
            else
                 THD_AreaRatio = THD_RelatedCand[1];//near region

//			overlapAreaRatio[ i2 ] = ratio;//statistical all ratio

            if(ratio>THD_AreaRatio)
            {
                /// find valid candidates at T
                LinkList[ MIN( LinkListNum , LINK_ARRAY_NUM-1 ) ] = i2;
                ++LinkListNum;
            }
#endif
        }// end of (i2 < MAXCandidateNum)
        candIdx=-1; 
        /// ==== 處理一對多的狀況 ====

#if 0 //只選一組做1-1 mapping    
        FLOAT maxValue =  0 ;
        for( i3 = 0 ; i3 < LinkListNum ; ++i3 ) //choose the best(符合大小/中心點條件下 最大面積的)
            {
            if( CTACandidate[ LinkList[i3] ].size > maxValue ), old comparison
            //if( overlapAreaRatio[ LinkList[i3]] > maxValue )
                {
                maxValue = CTACandidate[ LinkList[i3] ].size;
                //maxValue =overlapAreaRatio[ LinkList[i3]];
                candIdx = LinkList[i3];
            }
                }
#else	//merge 所有可能的candidate

        INT16 revIdx,delIdx;
        if(LinkListNum>0)
            candIdx=LinkList[0];
        for( i3 = 0; i3 < LinkListNum-1 ; ++i3 ) //choose the best(符合大小/中心點條件下 最大面積的)
        {
            i4 = i3+1;
            if( CTACandidate[ LinkList[i3] ].size > CTACandidate[ LinkList[i4] ].size )
            {
                revIdx = LinkList[i3];
                delIdx = LinkList[i4];
            }
            else
            {
                revIdx = LinkList[i4];
                delIdx = LinkList[i3];
            }
            MergeCandidate(revIdx,delIdx,CTACandidate,CTACandidatePrev);
            LinkList[i4]=revIdx;
            candIdx=revIdx;
        }
#endif
        if( -1 != candIdx )
        {
            BOOL replace=FALSE;
            if(CTACandidate[ candIdx].isRelatedCandValid == FALSE)
            {
                replace=TRUE;
            }
            else if( CTACandidate[ candIdx].countDown > CTACandidatePrev[i1].countDown ||
                     CTACandidate[ candIdx].appearCounter < CTACandidatePrev[i1].appearCounter)
            {
                replace = TRUE;/// ==== 處理多對一的狀況 ====
            }
            if(replace)
            {
                CTACandidatePrev[i1].isRelatedCandValid   = TRUE;
                CTACandidatePrev[i1].relatedCandLink      = candIdx;               ///< record Prev to Current index
                CTACandidate[ candIdx].isValid            = CTACandidatePrev[i1].isValid;
                CTACandidate[ candIdx].isRelatedCandValid = TRUE;
                CTACandidate[ candIdx].relatedCandLink    = i1;                    ///< record Current to Prev 的 index
                CTACandidate[ candIdx].retrieveEdgeTimes  = MAX(CTACandidate[i1].retrieveEdgeTimes-failedEdgeSuccess,0);
                CTACandidate[ candIdx].boundaryCount      = CTACandidatePrev[i1].boundaryCount;
                CTACandidate[ candIdx].boundaryType       = CTACandidatePrev[i1].boundaryType;
                CTACandidate[ candIdx].appearCounter	  = CTACandidatePrev[i1].appearCounter;
                CTACandidate[ candIdx].staticCounter	  = CTACandidatePrev[i1].staticCounter;
				CTACandidate[ candIdx].countDown          = CTACandidatePrev[i1].countDown; 
                CTACandidate[ candIdx].PosXYKeepTime   = CTACandidatePrev[i1].PosXYKeepTime; 
				CTACandidate[ candIdx].moveDir = CTACandidatePrev[i1].moveDir; 
				CTACandidate[ candIdx].appearPosxy[0] = CTACandidatePrev[i1].appearPosxy[0]; 
                CTACandidate[ candIdx].appearPosxy[1] = CTACandidatePrev[i1].appearPosxy[1]; 
				CTACandidate[ candIdx].appearPosXY[0] = CTACandidatePrev[i1].appearPosXY[0]; 
				CTACandidate[ candIdx].appearPosXY[1] = CTACandidatePrev[i1].appearPosXY[1]; 
            }
            //need to fine tune the object dimension
            //BOOL result=FineTuneObject(&CTACandidate[ candIdx], &CTACandidatePrev[i1], Region);
            ret_validNum++;

#pragma region//======Modify Rect @ CandAssociation======
			BOOL modifyRect=FALSE;
			INT16 endxi,endxi_prev;
			INT16 displace_startxi,displace_endxi;
			INT16 boundMove=0;//boundary condition

			endxi=CTACandidate[ candIdx].startxi+CTACandidate[ candIdx].width;
			endxi_prev=CTACandidatePrev[i1].startxi+CTACandidatePrev[i1].width;
			
			if(Dir=='L')
			{
				if(CTACandidatePrev[i1].moveDir==dir_Right)
				{
					modifyRect=TRUE;
					if(CTACandidatePrev[i1].boundaryType==rightBound)
						boundMove=MAX(CTACandidatePrev[i1].ObjTrackMovexy[0],CTACandidatePrev[i1].ObjMovexy[0]);

					displace_endxi=MAX(0,endxi-endxi_prev);//must be >=0
					displace_startxi=CTACandidate[ candIdx].startxi-CTACandidatePrev[i1].startxi;
					displace_startxi=MIN(displace_endxi+boundMove,displace_startxi);//displace_startxi<=displace_endxi*(2 or 1)
				}
			}
			else//Dir=='R'
			{
				if(CTACandidatePrev[i1].moveDir==dir_Left)
				{
					modifyRect=TRUE;
					if(CTACandidatePrev[i1].boundaryType==leftBound)
						boundMove=MIN(CTACandidatePrev[i1].ObjTrackMovexy[0],CTACandidatePrev[i1].ObjMovexy[0]);

					displace_startxi=MIN(0,CTACandidate[ candIdx].startxi-CTACandidatePrev[i1].startxi);//must be <=0
					displace_endxi=endxi-endxi_prev;
					displace_endxi=MAX(displace_startxi+boundMove , displace_endxi);//displace_startxi*(2 or 1)<=displace_endxi
				}
			}
			if(modifyRect)
			{
				CTACandidate[ candIdx].startxi=CTACandidatePrev[i1].startxi+displace_startxi;
				endxi=endxi_prev+displace_endxi;
				CTACandidate[ candIdx].width=endxi-CTACandidate[ candIdx].startxi;
			}
#pragma endregion//======Modify Rect @ CandAssociation======
        }
    }// end of CTACandidatePrev

    return ret_validNum; //ex. 5 means there are 5 CTACandidates linked
}

//update info only, counter is not updated
void Estimation::UpdateBoundaryInfo( CandidateRec CTACandidate[] ,CONST CHAR Dir, CONST INT32 Region[4], CONST INT16 inMaxCand )
{
	INT16 boundL=30,boundR=30,boundD=30;

	if(Dir=='L')
		boundL=0;
	else
		boundR=0;

	for ( INT32 i1 = 0 ; i1 < inMaxCand ; ++i1) // CTACandidate Total Num
	{
		if( Invalid == CTACandidate[i1].isValid ) continue;

		if( CTACandidate[i1].startxi -boundL<= Region[0])
			CTACandidate[i1].boundaryType=leftBound;
		else if(CTACandidate[i1].startxi+CTACandidate[i1].width+boundR>=Region[2])
			CTACandidate[i1].boundaryType=rightBound;
		else if(CTACandidate[i1].startyi <=Region[1])
			CTACandidate[i1].boundaryType=upperBound;
		else if(CTACandidate[i1].startyi+CTACandidate[i1].height+boundD>=Region[3])
			CTACandidate[i1].boundaryType=lowerBound;
		else
			CTACandidate[i1].boundaryType=noBound;

	} // end of i1 < inMaxCand
}

/**
* @brief	 Update  boundaryType  in valid CTACandidates
* @n         * leftBound / rightBound / lowerBound / upperBound/ noBound
* @param[in] CTACandidate	Input Valid Candidate
*/
void Estimation::UpdateBoundaryCounter( CandidateRec CTACandidate[] ,CONST CHAR Dir, CONST INT32 Region[4], CONST INT16 inMaxCand )
{
	CONST INT16 appearCountTH=3;

    INT16 boundL=30,boundR=30,boundD=30;

    if(Dir=='L')
        boundL=0;
    else
        boundR=0;

    for ( INT32 i1 = 0 ; i1 < inMaxCand ; ++i1) // CTACandidate Total Num
    {
        if( Invalid == CTACandidate[i1].isValid ) continue;

		if(CTACandidate[i1].isValid!=Invalid)
			CTACandidate[i1].appearCounter++;

        if( CTACandidate[i1].startxi -boundL<= Region[0])
            CTACandidate[i1].boundaryType=leftBound;
        else if(CTACandidate[i1].startxi+CTACandidate[i1].width+boundR>=Region[2])
            CTACandidate[i1].boundaryType=rightBound;
        else if(CTACandidate[i1].startyi <=Region[1])
            CTACandidate[i1].boundaryType=upperBound;
        else if(CTACandidate[i1].startyi+CTACandidate[i1].height+boundD>=Region[3])
            CTACandidate[i1].boundaryType=lowerBound;
        else
            CTACandidate[i1].boundaryType=noBound;

        if(CTACandidate[i1].boundaryType==leftBound || CTACandidate[i1].boundaryType==rightBound)	
            CTACandidate[i1].boundaryCount++;
        else if(CTACandidate[i1].boundaryType==noBound)
            CTACandidate[i1].boundaryCount=MAX(CTACandidate[i1].boundaryCount-2,0);
    } // end of i1 < inMaxCand
}

//decide [IsValid=Valid] only here
void Estimation::DecideMoveDir( CandidateRec CTACandidate[], INT16 inMaxCand , CandidateRec CTACandidatePrev[] ,CONST CHAR Dir,CONST CAN_Info *carState, CONST INT32 Region[4] )
{
	CONST INT16 appearCountTH=3;
	CONST INT16 TH_minSpeed=(carState->carSpeed>=1 && ABS(carState->steerAngle)>=300)? MIN_MOVE_SPEED_ROTATE : MIN_MOVE_SPEED_STRAIGHT; //56*0.036=2km/h
	
	INT16 awayCount;
	INT16 incomingCount;
	INT16 noSpeedCount;
	INT16 PreviousCanIdx;
	INT32 posXY[2];

	for ( int i1 = 0 ; i1 < inMaxCand ; ++i1) // CTACandidate Total No.
	{
		if( Invalid == CTACandidate[i1].isValid ) continue;
		if( Valid == CTACandidate[i1].isValid && CTACandidate[i1].moveDir!=dir_Unknown ) continue;
		if(CTACandidate[i1].isRelatedCandValid==FALSE) continue;
		
		awayCount=0;
		incomingCount=0;
		noSpeedCount=0;
		PreviousCanIdx = CTACandidate[i1].relatedCandLink;

		if(Dir=='L')
		{
			if(CTACandidatePrev[ PreviousCanIdx ].isObjSpeedXYValid==TRUE)
			{
				if(CTACandidatePrev[ PreviousCanIdx ].ObjSpeedXY[0]<-TH_minSpeed)
					awayCount++;
				else if(CTACandidatePrev[ PreviousCanIdx ].ObjSpeedXY[0]>TH_minSpeed)
					incomingCount++;
				else
					noSpeedCount++;
			}
			else
				noSpeedCount++;

			if(CTACandidate[i1].isObjSpeedXYValid==TRUE)
			{
				if(CTACandidate[i1].ObjSpeedXY[0]<-TH_minSpeed)
					awayCount++;
				else if(CTACandidate[i1].ObjSpeedXY[0]>TH_minSpeed)
				{
					if(carState->carSpeed*carState->steerAngle>450 && CTACandidate[i1].startxi<0.5*(Region[2]-Region[0]))//160  )
					{//left-end side
						noSpeedCount++;
					}
					else if(carState->carSpeed*carState->steerAngle>500 && CTACandidate[i1].startxi< 0.6*(Region[2]-Region[0]))//160  )
					{//right-end side
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle>400 && CTACandidate[i1].startxi<0.4*(Region[2]-Region[0]))//130  )
					{//left-end side
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle>400 && CTACandidate[i1].startxi+CTACandidate[i1].width>=Region[2]*0.7 )
					{//left side at center region
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle<-45 && carState->steerAngle <-450 && CTACandidate[i1].ObjMoveXY[0]>100)//100: speed=72km/h )
					{//at leave half image 
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle>300 && carState->steerAngle>450 && ABS(CTACandidate[i1].ObjSpeedXY[0] )<80
						&& ABS(CTACandidate[i1].ObjPosXY[0])+ABS(CTACandidate[i1].ObjPosXY[1])>500)
					{//left side at center region
						noSpeedCount++;
					}
					else	if( ABS(CTACandidate[i1].ObjMoveXY[1])>5*ABS(CTACandidate[i1].ObjMoveXY[0]) && ABS(CTACandidate[i1].ObjMoveXY[0])>10)//100: speed=72km/h )
					{//at leave half image 
						noSpeedCount++;
					}
					else if(CTACandidate[i1].ObjPosxy[1]>Region[3]-(Region[3]-Region[1])*0.1 &&ABS(CTACandidate[i1].ObjPosXY[0])<300 && CTACandidate[i1].ObjSpeedXY[1]<-10)
					{//at rear end and far side
						noSpeedCount++;
					}
					else
					incomingCount++;
				}
				else
					noSpeedCount++;
			}
			else
				noSpeedCount++;

			if(CTACandidate[i1].isInReviveMode!=No_Tracking)
			{
				if(CTACandidate[i1].ObjTrackMovexy[0]<0)
					awayCount++;
				else if(CTACandidate[i1].ObjTrackMovexy[0]>0)
					incomingCount++;
			}
			if(awayCount>=2 && CTACandidate[i1].moveDir==dir_Unknown)
			{
				CTACandidate[i1].moveDir=dir_Left;
				CTACandidate[i1].isValid=Invalid;
			}
			else if(incomingCount>=2)
			{
				CTACandidate[i1].moveDir=dir_Right;
				CTACandidate[i1].isValid=Valid;
			}
			else if(noSpeedCount>=2 && CTACandidate[i1].isValid==Startup)
			{
				CTACandidate[i1].isValid=Invalid;
			}
			if(CTACandidate[i1].appearCounter<=appearCountTH)
			{//final check
				if(carState->steerAngle> 500 && carState->carSpeed*ABS(carState->steerAngle)>300 &&awayCount>0 )
					CTACandidate[i1].isValid=Invalid;
				if(carState->steerAngle> 500 && carState->carSpeed*ABS(carState->steerAngle)>300 && incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
				if(carState->steerAngle< -400 &&awayCount>0 &&CTACandidate[i1].ObjMoveXY[0]>100 )
					CTACandidate[i1].isValid=Invalid;
				if(5*ABS(CTACandidate[i1].ObjMoveXY[0])<ABS(CTACandidate[i1].ObjMoveXY[1])&& 10*ABS(CTACandidate[i1].ObjSpeedXY[0])<ABS(CTACandidate[i1].ObjSpeedXY[1]))
					CTACandidate[i1].isValid=Invalid;
				if(CTACandidate[i1].ObjPosXY[1]<15 && CTACandidate[i1].ObjSpeedXY[1]<-10 && incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
				if(CTACandidate[i1].ObjSpeedXY[0]<-50 && ABS(CTACandidate[i1].ObjSpeedXY[0])<ABS(CTACandidate[i1].ObjSpeedXY[1]) && incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
				if(CTACandidate[i1].ObjPosXY[0]<-200&& CTACandidate[i1].ObjPosXY[1]<50 && ABS(CTACandidate[i1].ObjMoveXY[0])<2&&incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
			}
		}
		else//Dir=='R'
		{
			if(CTACandidatePrev[ PreviousCanIdx ].isObjSpeedXYValid==TRUE)
			{
				if(CTACandidatePrev[ PreviousCanIdx ].ObjSpeedXY[0]>TH_minSpeed)
					awayCount++;
				else if(CTACandidatePrev[ PreviousCanIdx ].ObjSpeedXY[0]<-TH_minSpeed)
					incomingCount++;
				else
					noSpeedCount++;
			}
			else
				noSpeedCount++;

			if(CTACandidate[i1].isObjSpeedXYValid==TRUE)
			{
				if(CTACandidate[i1].ObjSpeedXY[0]>TH_minSpeed)
					awayCount++;
				else if(CTACandidate[i1].ObjSpeedXY[0]<-TH_minSpeed)
				{
					if(carState->carSpeed*carState->steerAngle<-450 && CTACandidate[i1].startxi+CTACandidate[i1].width> Region[2] -0.5*(Region[2]-Region[0]))//160  )
					{//right-end side
						noSpeedCount++;
					}
					else if(carState->carSpeed*carState->steerAngle<-500 && CTACandidate[i1].startxi+CTACandidate[i1].width> Region[2] -0.6*(Region[2]-Region[0]))//160  )
					{//right-end side
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle<-400 && CTACandidate[i1].startxi+CTACandidate[i1].width> Region[2] - 0.4*(Region[2]-Region[0]))//130  )
					{//right-end side
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle<-400 && CTACandidate[i1].startxi<=Region[0]+(Region[2]-Region[0])*0.3 )
					{//left side at center region
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle<-300 && carState->steerAngle<-450 && ABS(CTACandidate[i1].ObjSpeedXY[0] )<80
						&& ABS(CTACandidate[i1].ObjPosXY[0])+ABS(CTACandidate[i1].ObjPosXY[1])>500)
					{//left side at center region
						noSpeedCount++;
					}
					else	if(carState->carSpeed*carState->steerAngle>45 && carState->steerAngle >450 && CTACandidate[i1].ObjMoveXY[0]>100)//100: speed=72km/h )
					{//at leave half image 
						noSpeedCount++;
					}
					else if(CTACandidate[i1].ObjPosxy[1]>Region[3]-(Region[3]-Region[1])*0.1 &&CTACandidate[i1].ObjPosXY[0]<300 && CTACandidate[i1].ObjSpeedXY[1]<-10)
					{//at rear end and far side
						noSpeedCount++;
					}
					else
					incomingCount++;
				}
				else
					noSpeedCount++;
			}
			else
				noSpeedCount++;

			if(CTACandidate[i1].isInReviveMode!=No_Tracking)
			{
				if(CTACandidate[i1].ObjTrackMovexy[0]>0)
					awayCount++;
				else if(CTACandidate[i1].ObjTrackMovexy[0]<0)
					incomingCount++;
			}
			if(awayCount>=2 && CTACandidate[i1].moveDir==dir_Unknown)
			{
				CTACandidate[i1].moveDir=dir_Right;
				CTACandidate[i1].isValid=Invalid;
			}
			else if(incomingCount>=2)
			{
				CTACandidate[i1].moveDir=dir_Left;
				CTACandidate[i1].isValid=Valid;
			}
			else if(noSpeedCount>=2 && CTACandidate[i1].isValid==Startup)
			{
				CTACandidate[i1].isValid=Invalid;
			}
			if(CTACandidate[i1].appearCounter<=appearCountTH)
			{//final check
				if(carState->steerAngle< -500 && carState->carSpeed*ABS(carState->steerAngle)>300 &&awayCount>0 )
					CTACandidate[i1].isValid=Invalid;
				if(carState->steerAngle< -500 && carState->carSpeed*ABS(carState->steerAngle)>300 && incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
				if(carState->steerAngle> 400 && awayCount>0 &&CTACandidate[i1].ObjMoveXY[0]>100 )
					CTACandidate[i1].isValid=Invalid;
				if(5*ABS(CTACandidate[i1].ObjMoveXY[0])<ABS(CTACandidate[i1].ObjMoveXY[1])&& 10*ABS(CTACandidate[i1].ObjSpeedXY[0])<ABS(CTACandidate[i1].ObjSpeedXY[1]))
					CTACandidate[i1].isValid=Invalid;
				if(CTACandidate[i1].ObjPosXY[1]<15 && CTACandidate[i1].ObjSpeedXY[1]<-10 && incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
				if(CTACandidate[i1].ObjPosXY[0]>200&& CTACandidate[i1].ObjPosXY[1]<50 && ABS(CTACandidate[i1].ObjMoveXY[0])<2&&incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;
				if(CTACandidate[i1].ObjPosXY[0]<200&& CTACandidate[i1].ObjPosXY[1]<70 && incomingCount-noSpeedCount-awayCount<=0 )
					CTACandidate[i1].isValid=Invalid;//near
			}
		}

		if(CTACandidate[i1].isValid==Startup)
		{
			if(carState->carSpeed*ABS(carState->steerAngle)>1000)
			{
				CTACandidate[i1].appearCounter=MAX(0,CTACandidate[i1].appearCounter-1);
            }
            else if(awayCount==incomingCount && awayCount==noSpeedCount)
            {
                //too noisy, keep current state
            }
			else	if(CTACandidate[i1].appearCounter>=appearCountTH)//appearCounter=3時,此時剛好經過兩次tracking,兩次ground speed//
				CTACandidate[i1].isValid=Valid;
		}
		if(CTACandidate[i1].appearCounter<=appearCountTH && CTACandidate[i1].isValid==Valid)
		{//remove un-reasonable movement, the parameters should be re-finetune
			if(ABS(CTACandidate[i1].ObjMoveXY[0])*1.1<ABS(CTACandidate[i1].ObjMoveXY[1]) &&ABS(CTACandidate[i1].ObjSpeedXY[1])>1500)
				CTACandidate[i1].isValid=Invalid;
			if(ABS(CTACandidate[i1].ObjMoveXY[0])*2<ABS(CTACandidate[i1].ObjMoveXY[1]) &&ABS(CTACandidate[i1].ObjSpeedXY[0])>400)
				CTACandidate[i1].isValid=Invalid;
			if(ABS(CTACandidate[i1].ObjMoveXY[1])>40 &&ABS(CTACandidate[i1].ObjSpeedXY[0])>800 && incomingCount<3 && noSpeedCount+awayCount>0)
				CTACandidate[i1].isValid=Invalid;
			//if(ABS(CTACandidate[i1].ObjSpeedXY[0])*2.1<ABS(CTACandidate[i1].ObjSpeedXY[1]) &&ABS(CTACandidate[i1].ObjSpeedXY[1])>200 && incomingCount<3)
			//	CTACandidate[i1].isValid=Invalid;
			if(ABS(CTACandidate[i1].ObjSpeedXY[0])*8<ABS(CTACandidate[i1].ObjSpeedXY[1]) &&ABS(CTACandidate[i1].ObjSpeedXY[1])>100 && incomingCount<2)
				CTACandidate[i1].isValid=Invalid;
			if(incomingCount-noSpeedCount-awayCount<=0 && ABS(CTACandidate[i1].ObjPosXY[0])>1800 && ABS(CTACandidate[i1].ObjSpeedXY[1])>1000 )
				CTACandidate[i1].isValid=Invalid;
		}

		//assign info for valid candidate
		if(CTACandidate[i1].isValid==Valid)
		{
			CTACandidate[i1].appearPosxy[0]=CTACandidate[i1].ObjTrackPosxy[0];
			CTACandidate[i1].appearPosxy[1]=CTACandidate[i1].ObjTrackPosxy[1];
			WModel->PosImgToWorld( posXY[0],posXY[1], CTACandidate[i1].appearPosxy[0], CTACandidate[i1].appearPosxy[1] );
			CTACandidate[i1].appearPosXY[0]=XwShiftToBackCenter(posXY[0]);
			CTACandidate[i1].appearPosXY[1]=YwShiftToBackCenter(posXY[1]);
		}
	}
}
/**
* @brief	 GetObjInfo is Update Candidates[Flag,Speed,Pos] info referred to Previous related Candidates
* @param[in] CTACandidate		Input Valid Candidate at current  frame
* @param[in] CTACandidatePrev	Input Valid Candidate at previous frame
* @Note      ObjPosXY[2]        unit: cm/frame
* @n         ObjSpeedXY[2]      unit: cm
* @n         CandTTC            unit: ms
*/
void Estimation::GetCandInfo( CandidateRec CTACandidate[], INT16 inMaxCand , CandidateRec CTACandidatePrev[] ,UCHAR *Sobel_img, UCHAR *g_GaussImg , INT32 imgDimensionX , const FLOAT THD_WeightSpeed[2], FLOAT infps, UCHAR *dst_Y, SimpleCarSpec &i_carSpec, INT16 i_CTACustROI[4] )
{
    /// THRESHOLD
    FLOAT Weight_SmoothSpeed = THD_WeightSpeed[0]; //  means (Weight)*PreFrame+(1-Weight)*CurFrame
    FLOAT Weight_SmoothPos   = THD_WeightSpeed[1]; //  means (Weight)*PreFrame+(1-Weight)*CurFrame
    INT16 centroidXmove,centroidYmove;
    INT16 PreviousCanIdx;
    INT16 AddFlag;
    BOOL isLeft;
    
    for ( int i1 = 0 ; i1 < inMaxCand ; ++i1) // CTACandidate Total No.
    {
        if( Invalid == CTACandidate[i1].isValid ) continue;

        CTACandidate[i1].bondaryLR[0] = CTACandidate[i1].startxi ;
        CTACandidate[i1].bondaryLR[1] = CTACandidate[i1].startxi + CTACandidate[i1].width ;

        isLeft = ( CTACandidate[i1].startxi < (imgDimensionX>>1) )? TRUE:FALSE;

		if(CTACandidate[i1].isRelatedCandValid==TRUE)//spike
		{
			PreviousCanIdx = CTACandidate[i1].relatedCandLink ;
			
			if(CTACandidate[i1].isInReviveMode==No_Tracking)
			if( CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[0]!=0 || CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[1]!=0)
			{
				centroidXmove=CTACandidate[i1].startxi+(CTACandidate[i1].width>>1)-CTACandidatePrev[PreviousCanIdx].startxi-(CTACandidatePrev[PreviousCanIdx].width>>1);
				centroidYmove=CTACandidate[i1].startyi+(CTACandidate[i1].height>>1)-CTACandidatePrev[PreviousCanIdx].startyi-(CTACandidatePrev[PreviousCanIdx].height>>1);
				CTACandidate[i1].ObjTrackPosxy[0]=CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[0]+centroidXmove;
				CTACandidate[i1].ObjTrackPosxy[1]=CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[1]+centroidYmove;
			}
		}

        #pragma region=====update CornerMove=====
		FLOAT xDirMean = (INT32)CTACandidate[i1].CornerVector[0]-(INT32)CTACandidate[i1].CornerVector[2] ;//Right
		FLOAT yDirMean = (INT32)CTACandidate[i1].CornerVector[1]-(INT32)CTACandidate[i1].CornerVector[3] ;//Up

		xDirMean /= (FLOAT)( MAX(1.0, CTACandidate[i1].CornerQuantity) );
		yDirMean /= (FLOAT)( MAX(1.0, CTACandidate[i1].CornerQuantity) );
		CTACandidate[i1].CornerMove[0]=xDirMean;
		CTACandidate[i1].CornerMove[1]=-yDirMean;
        #pragma endregion=====update CornerMove=====
	
        if( isCandAtBoundary(&CTACandidate[i1], isLeft) ) 
        {
            CTACandidate[i1].isObjPosXYValid = FALSE;
            CTACandidate[i1].isObjPosxyValid = FALSE;  
            continue;
        }

        ///1) Cal Current ObjPosxy/ObjPosXY
        GetCandPos( &CTACandidate[i1], Sobel_img, g_GaussImg, imgDimensionX, dst_Y );

        ///3) Update Pos/Speed/TTC according smooth Weight
        if( FALSE == CTACandidate[i1].isRelatedCandValid)//New Candidate
        {
            if( (CTACandidate[i1].startxi <= (imgDimensionX>>1)) ) //Left
                 CTACandidate[i1].countDown = CountDownFlagAdpL;
            else
                 CTACandidate[i1].countDown = CountDownFlagAdpR;

            #pragma region assign [PosXYKeepTime]
            if(isLeft)
            {
                if (CTACandidate[i1].isObjPosXYValid && 
                    (-i_CTACustROI[0]>CTACandidate[i1].ObjPosXY[0]) &&
                    (-i_CTACustROI[1]<CTACandidate[i1].ObjPosXY[0]))
                {
                    CTACandidate[i1].PosXYKeepTime = 1;
                }
                else
                {
                    CTACandidate[i1].PosXYKeepTime = 0;
                }
            }
            else // right
            {
                if (CTACandidate[i1].isObjPosXYValid && 
                    (i_CTACustROI[1]>CTACandidate[i1].ObjPosXY[0]) &&
                    (i_CTACustROI[0]<CTACandidate[i1].ObjPosXY[0]))
                {
                    CTACandidate[i1].PosXYKeepTime = 1;
                }
                else
                {
                    CTACandidate[i1].PosXYKeepTime = 0;
                }
            }
            #pragma endregion
            /// New Candidate exist no Speed.
        }
        else // Tracked-Candidates
        {
            /// find the Previous Candidate Idx
//            PreviousCanIdx      = CTACandidate[i1].relatedCandLink ;
            
            #pragma region assign [PosXYKeepTime]
            if(isLeft)
            {
                if (CTACandidate[i1].isObjPosXYValid && 
                    (-i_CTACustROI[0]>CTACandidate[i1].ObjPosXY[0]) &&
                    (-i_CTACustROI[1]<CTACandidate[i1].ObjPosXY[0]))
                {
                    ++CTACandidate[i1].PosXYKeepTime;
                }
                else
                {
                    CTACandidate[i1].PosXYKeepTime = MAX( (CTACandidate[i1].PosXYKeepTime - 1 ), 0);
                }
            }
            else // right
            {
                if (CTACandidate[i1].isObjPosXYValid && 
                    (i_CTACustROI[1]>CTACandidate[i1].ObjPosXY[0]) &&
                    (i_CTACustROI[0]<CTACandidate[i1].ObjPosXY[0]))
                {
                    ++CTACandidate[i1].PosXYKeepTime;
                }
                else
                {
                    CTACandidate[i1].PosXYKeepTime = MAX( (CTACandidate[i1].PosXYKeepTime - 1 ), 0);
                }
            }
            #pragma endregion

            /// Get precisely speed if Tracked-Candidates, overwritten if previous MtionCorner speed exists
            GetCandSpeedbyHeadPos( &CTACandidate[i1], &CTACandidatePrev[PreviousCanIdx], infps ); // cm/sec
#if 0
            //spike
            if(CTACandidate[i1].isInReviveMode==No_Tracking)
            if( CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[0]!=0 || CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[1]!=0)
            {
                centroidXmove=CTACandidate[i1].startxi+(CTACandidate[i1].width>>1)-CTACandidatePrev[PreviousCanIdx].startxi-(CTACandidatePrev[PreviousCanIdx].width>>1);
                centroidYmove=CTACandidate[i1].startyi+(CTACandidate[i1].height>>1)-CTACandidatePrev[PreviousCanIdx].startyi-(CTACandidatePrev[PreviousCanIdx].height>>1);
                CTACandidate[i1].ObjTrackPosxy[0]=CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[0]+centroidXmove;
                CTACandidate[i1].ObjTrackPosxy[1]=CTACandidatePrev[PreviousCanIdx].ObjTrackPosxy[1]+centroidYmove;
            }
#endif            
            /// Weight the Estimation info when Valid at this and previous frame
            if( CTACandidate[i1].isObjPosxyValid && CTACandidatePrev[ PreviousCanIdx ].isObjPosxyValid )
            {
                CTACandidate[i1].ObjMovexy[0]=CTACandidate[i1].ObjPosxy[0] - CTACandidatePrev[ PreviousCanIdx ].ObjPosxy[0];
                CTACandidate[i1].ObjMovexy[1]=CTACandidate[i1].ObjPosxy[1] - CTACandidatePrev[ PreviousCanIdx ].ObjPosxy[1];
            }
            if( CTACandidate[i1].isObjPosXYValid && CTACandidatePrev[ PreviousCanIdx ].isObjPosXYValid )
            {
                CTACandidate[i1].ObjMoveXY[0]=CTACandidate[i1].ObjPosXY[0] - CTACandidatePrev[ PreviousCanIdx ].ObjPosXY[0];
                CTACandidate[i1].ObjMoveXY[1]=CTACandidate[i1].ObjPosXY[1] - CTACandidatePrev[ PreviousCanIdx ].ObjPosXY[1];
            }

            /// smooth ground speed
            GetEstimatedSpeed( &CTACandidate[i1], &CTACandidatePrev[ PreviousCanIdx ], isLeft);

            GetCandTTC( &CTACandidate[i1] , imgDimensionX, isLeft, i_carSpec );

            if(CTACandidate[i1].countDown!=0) //countdown Update
            {
                //if match the Position(ROI) ,Speed(5-24), TTC(<2sec) criteria [DecisionTree]
                AddFlag = countDownRevise( &CTACandidate[i1] , imgDimensionX, i_CTACustROI );
            
                /// countDown according to TTC and Position criteria.
                CTACandidate[i1].countDown += AddFlag;
                CTACandidate[i1].countDown = MAX_MIN_RANGE(CTACandidate[i1].countDown,0,CountDownFlagMax); 
            }
        } // end of Tracked-Candidates
    } // end of (i1 < MAXCandidateNum)
}

/**
* @brief	 Cal the proper CountDown Addition.
* @param[in] CTACandidate	Input Valid Candidate
* @return					the Flag modification value
*/
INT16 Estimation::countDownRevise( CandidateRec CTACandidate[] , INT32 imgDimensionX, INT16 i_CTACustROI[4] )
{
    enum ENUM_INT8_T{ FlagKeep =0, FlagAdd1 = 1 , FlagMin1 = -1};
    
    BOOL isLeft = ( CTACandidate->startxi<=(imgDimensionX>>1) )? (TRUE):(FALSE);

    BOOL isInROI  = FALSE; // X,Y
    BOOL isTTCLow = FALSE;

    if ( isLeft )		   // Left ROI
    {
        if( CTACandidate->isObjPosXYValid && CTACandidate->isObjSpeedXYValid )
        {
            FLOAT Xdist     = CTACandidate->ObjPosXY[0];  //cm
            FLOAT Ydist     = CTACandidate->ObjPosXY[1];  //cm

            if( (CTACandidate->CandTTC < CTATTC) &&( CTACandidate->CandTTC>0 ) ) // in TTC
            {

                if( (Xdist > -i_CTACustROI[0] ) &&			// in ROI
                    (Xdist < -i_CTACustROI[1] ) &&
                    (Ydist <  i_CTACustROI[3]  ) &&
                    (Ydist >  i_CTACustROI[2]) ) 
                {
                    return FlagMin1;
                }
                return FlagMin1; ///< out of ROI is Okay for demo
            }
        } //end of (isObjPosXYValid & isObjSpeedXYValid)
        return FlagAdd1;
    }// end of Left ROI
    else // Right ROI
    {
        if( CTACandidate->isObjPosXYValid && CTACandidate->isObjSpeedXYValid )
        {
            FLOAT Xdist     = CTACandidate->ObjPosXY[0];  //cm
            FLOAT Ydist     = CTACandidate->ObjPosXY[1];  //cm

            if( (CTACandidate->CandTTC < CTATTC)  &&( CTACandidate->CandTTC>0 )) // in TTC
            {

                if( (Xdist < i_CTACustROI[1]) &&			// in ROI
                    (Xdist > i_CTACustROI[0]) &&
                    (Ydist < i_CTACustROI[3]) &&
                    (Ydist > i_CTACustROI[2])) 
                {
                    return FlagMin1;
                }
                return FlagMin1;    ///< out of ROI is Okay for demo
            }
        }
        return FlagAdd1;
    }// end of Right ROI


}

/**
* @brief	  GetCandPos is the Candidate With Correct Image Info and find the World Position of closed part
* @param[in]  CTACandidate		Input Valid Candidate
* @param[in]  CTACandidatePrev	Input Valid Candidate at previous frame
* @param[out] g_img_FrameDiff	Input Image of Motion Edge
* @param[out] PosXY				Output World Position(Start from leftTop)
* @return						is Output Position(xiyi) valid
*/
BOOL Estimation::GetCandPos2( CandidateRec CTACandidate[] , CandidateRec CTACandidatePrev[] , UCHAR *g_img_FrameDiff , INT32 imgDimensionX  )
{
    INT16 startx = CTACandidate->startxi;
    INT16 starty = CTACandidate->startyi;
    INT16 endx   = CTACandidate->startxi + CTACandidate->width;
    INT16 endy   = CTACandidate->startyi + CTACandidate->height;
    INT16 width  = CTACandidate->width;
    INT16 height = CTACandidate->height;
    BOOL isLeft   = (startx <= (imgDimensionX>>1))?(TRUE):(FALSE);
    BOOL isFindX  = FALSE;
    BOOL isFindY  = FALSE;
    BOOL isDecision1 = FALSE;
    BOOL isDecision2 = FALSE;

    ///1) Decision 1. ( HeadPosition at current frame)
    /// Find the Headxi
    INT16 smallPiece_xi = ((FLOAT)width/(FLOAT)2.0); 
    INT16 closest       = (isLeft)?(endx):(startx);
    INT16 farest        = (isLeft)?(endx-smallPiece_xi):(startx+smallPiece_xi);
    INT16 Headxi;
    INT32 vOffset;
    for ( INT16 hori = closest ; (isLeft)?(hori >= farest):(hori <= farest) ; (isLeft)?(--hori):(++hori) )
    {
        vOffset=endy*imgDimensionX+closest;
        for ( INT16 vert =endy ; vert >= starty; --vert )
        {
            if( 255 <= g_img_FrameDiff[vOffset] )
            {
                Headxi  = hori;
                isFindX = TRUE;
                break;
            }
                vOffset-=imgDimensionX;
        }
        if( isFindX ) 
            break;
    }
    //if(!isFindX) return FALSE;

    /// Find the Headyi
    INT16 smallPiece_yi, Headyi, tmpxstart, tmpxend     ;
    if(isFindX)
    {
        smallPiece_yi =((FLOAT)width/(FLOAT)10.0); 
        Headyi        = (isLeft)?(0):(0);
        tmpxstart     = (isLeft)?(Headxi-smallPiece_yi ):(Headxi);
        tmpxend       = (isLeft)?( Headxi ):(Headxi+smallPiece_yi);
        for ( INT16 vert = endy; vert >=starty  ; --vert )
        {
            vOffset=(vert)*imgDimensionX;
            for (INT16 hori = tmpxstart ; hori <= tmpxend ; ++hori  )
            {
                if( 255 <= g_img_FrameDiff[hori+vOffset] )
                {
                    isFindY = TRUE;
                    Headyi  = MAX(vert,Headyi);
                    break;
                }// end of g_img_FrameDiff
            }// end of hori
            if( isFindY ) 
                break;
        }// end of vert
    }
    isDecision1 = (isFindY && isFindX)? TRUE:FALSE;
    
    ///2) Decision 2. ( HeadPosition by previous frame)
    if( (TRUE == CTACandidatePrev->isObjPosxyValid) &&(TRUE == CTACandidatePrev->isObjPosXYValid))
    {
        //2.1 Get the xi/yi from CTACandidatePrev
        //2.2 Find the proper XwYw near the xi/yi

        isDecision2 = TRUE;
    }

    if( !isDecision1 && !isDecision2)
    {
        return FALSE;
    }
    else if( isDecision1 && !isDecision2)
    {
        //new object creation
        CTACandidate->ObjPosxy[0]     = Headxi;
        CTACandidate->ObjPosxy[1]     = Headyi;
        CTACandidate->isObjPosxyValid = TRUE;	///< Image_xy Find

        INT32 tmpXw,tmpYw;
        WModel->PosImgToWorld( tmpXw,tmpYw, Headxi,Headyi );
        if( tmpXw != 0xFFFF )
        {
            CTACandidate->ObjPosXY[0] = XwShiftToBackCenter( tmpXw );//(tmpXw - ((CTA_TV_WIDTH)>>1))*CTA_TV_COMPRESS ; //cm(Ori:back center of self-car)
            CTACandidate->ObjPosXY[1] = YwShiftToBackCenter( tmpYw );//(CTA_TV_HEIGHT - tmpYw)*CTA_TV_COMPRESS;	   //cm()

            CTACandidate->isObjPosXYValid = TRUE; ///< World XwYw Find
        }
        return TRUE;
    }

    //compare this two result, and choose better one
    //search point from previous one
    startx = CTACandidatePrev->ObjPosxy[0];
    starty = CTACandidatePrev->ObjPosxy[1];
    UINT16 smallPiece_x = width>>1; 
    UINT16 smallPiece_y = width*0.1+0.5; 
    if(isLeft)
        endx   = CTACandidate->startxi -smallPiece_x;
    else
        endx   = CTACandidate->startxi + smallPiece_x;
    endy   = CTACandidate->startyi + smallPiece_y;
    isFindX=FALSE;
    for ( UINT16 hori = startx ; (isLeft)?(hori >= endx):(hori <= endx) ; (isLeft)?(--hori):(++hori) )
    {
        vOffset=endy*imgDimensionX+closest;
        for ( UINT16 vert =endy ; vert >= (MAX((starty-smallPiece_y),0)); --vert )
        {
            if( 255 <= g_img_FrameDiff[vOffset] )
            {
                Headxi = hori;
                Headyi = vert;
                isFindX = TRUE;
                break;
            }
        }
        vOffset-=imgDimensionX;
        if( isFindX ) 
            break;
    }
    INT32 pre_x, pre_y, preX, preY;
    if( isFindX ) 
    {
        pre_x     = Headxi;
        pre_y     = Headyi;
        INT32 tmpXw,tmpYw;
        WModel->PosImgToWorld( tmpXw,tmpYw, Headxi,Headyi );
        if( tmpXw != 0xFFFF )
        {
            preX = XwShiftToBackCenter( tmpXw );//(tmpXw - ((CTA_TV_WIDTH)>>1))*CTA_TV_COMPRESS ; //cm(Ori:back center of self-car)
            preY = YwShiftToBackCenter( tmpYw );//
        }
        else
            isDecision2 = FALSE;
    }
    if(isFindX && isDecision2 && ABS(preX)<ABS(CTACandidate->ObjPosXY[0]))
    {
        CTACandidate->ObjPosXY[0] = preX;//; //cm(Ori:back center of self-car)
        CTACandidate->ObjPosXY[1] = preY;
        CTACandidate->ObjPosxy[0] = pre_x;//; //cm(Ori:back center of self-car)
        CTACandidate->ObjPosxy[1] = pre_y;
    }
   //CTACandidatePrev->ObjPosXY[1];//; //cm()
    CTACandidate->isObjPosXYValid = TRUE; ///< World XwYw Find

    return TRUE;

}

/**
* @brief	  is Candidate Boundary equals to ReiognROI.
* @param[in]  CTACandidate		Input Valid Candidate
* @return	  is Candidate stick on Boundary
*/
BOOL Estimation::isCandAtBoundary( CandidateRec i_CTACandidate[], CONST BOOL isleft )
{
    INT16 startx = i_CTACandidate->startxi;
    INT16 endx   = i_CTACandidate->startxi + i_CTACandidate->width;
    BOOL  ret    = FALSE;
    if( isleft )
    {
        ret = ((RegionL[2] - endx)<= ROISegBlockH)? TRUE:FALSE;
    }
    else
    {
        ret = (( startx - RegionR[0] )<= ROISegBlockH)? TRUE:FALSE;
    }
    return ret; //is Candidate stick on Boundary
}

/**
* @brief	  GetCandPos is the Candidate With Correct Image Info and find the World Position of closed part
* @n	      horizontal scan line method.
* @param[in]  CTACandidate		Input Valid Candidate
* @param[in]  gaussImg	        Input Image of source image
* @param[out] PosXY				Output World Position(Start from leftTop)
*/
void Estimation::GetCandPos( CandidateRec CTACandidate[] , UCHAR *Sobel_img ,UCHAR *gaussImg , INT32 imgDimensionX, UCHAR *dst_Y )
{
    CONST INT16 THD_GaussImg[2] = {20,55};  ///< (min,max)thd
    CONST INT16 THD_GaussImgVer2= THD_GaussImg[1];  ///< (max)thd
    CONST INT16 THD_XwFocus     = 25;       ///< (cm)
    CONST INT16 LineNum         = 8;
    CONST INT32 THD_XwDiff      = 500; ///<(unit:cm)
    CONST INT32 THD_XwDiff2     = 800;  ///<(unit:cm)

    INT16 startx            = CTACandidate->startxi;
    INT16 starty            = CTACandidate->startyi;
    INT16 endx              = CTACandidate->startxi + CTACandidate->width;
    INT16 endy              = CTACandidate->startyi + CTACandidate->height;
    INT16 width             = CTACandidate->width;
    INT16 height            = CTACandidate->height;
    BOOL  isLeft            = (startx <= (imgDimensionX>>1))?(TRUE):(FALSE);
    INT16 scanRowy[LineNum];
    INT16 interval           = MAX(((FLOAT)(endy-starty)/(FLOAT)(2*(LineNum+2))),1);

    /// Initial x,y
    CTACandidate->ObjPosXY[0]     = 0 ; 
    CTACandidate->ObjPosXY[1]     = 0 ; 
    CTACandidate->isObjPosXYValid = FALSE;
    CTACandidate->ObjPosxy[ 0 ]   = 0;
    CTACandidate->ObjPosxy[ 1 ]   = 0;
    CTACandidate->isObjPosxyValid = FALSE;  

    /// Initial scanRowy[ LineNum ]
    for(INT16 i1 = 0 ; i1 < LineNum ; ++i1)
        scanRowy[i1] = endy - (i1+1)*interval;

    #pragma region ------FistTime_CTACandidate->ObjPosXY------
    BOOL isSuccessOnce = FALSE ;
    ///1) Initial scanRowx[ LineNum ]
    INT16 scanRowx[ LineNum ];
    for ( INT16 i1 = 0; i1 < LineNum  ; ++i1 )
        scanRowx[i1] = -1;

    INT32  offsetV;
    for ( INT16 vertIdx = 0; vertIdx < LineNum  ; ++vertIdx )
    {
        offsetV = scanRowy[ vertIdx ] * imgDimensionX;
        for ( INT16 hori = (isLeft)?(endx):(startx) ; hori != ((isLeft)?(startx):(endx)) ; (isLeft)?(--hori):(++hori)  )
        {
            if( THD_GaussImg[0] >= gaussImg[ offsetV + hori ] )
            {
                scanRowx[ vertIdx ] = hori ;
                break;
            }
        }// end of [  hori <= endx ]
    } // end of [ vertIdx <= LineNum ]

    ///2) Transfer to the scanRowX[LineNum]
    INT32 scanRowX[ LineNum ];
    for ( INT16 i1 = 0; i1 < LineNum  ; ++i1 )
        scanRowX[i1] = -1;

    INT32 tmpXw,tmpYw;
    INT32 ValidNumXw = 0 ;
    for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
    {
        WModel->PosImgToWorld( tmpXw ,tmpYw, scanRowx[i1],scanRowy[i1] );
        if( tmpXw != 0xFFFF )
        {
            scanRowX[i1] = tmpXw ; ///< cm
            ++ValidNumXw;
        }
    }

///debug
//for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
//{
//        if(scanRowx[i1] < 0) continue;
//        OSD_SetColor(255,0,0);
//        OSD_Draw_Rec(dst_Y,640,480,scanRowx[i1], scanRowy[i1],scanRowx[i1]+2, scanRowy[i1]+2,1,true );
//}

    ///3) isValidNum
    if( 3 <= ValidNumXw  )
    {
        /// sort
        INT32 tmp1;
        INT16 tmp2;
    for ( INT32 i1 = 0 ; i1 < (LineNum-1) ; ++i1)
    {
        for ( INT32 i2 = (i1+1) ; i2 < LineNum ; ++i2 )
        {
            if( scanRowX[i1] > scanRowX[i2])
            {
                tmp1         = scanRowX[i1];
                scanRowX[i1] = scanRowX[i2];
                scanRowX[i2] = tmp1;

                    tmp2 = scanRowx[i1];
                    scanRowx[i1] = scanRowx[i2];
                    scanRowx[i2] = tmp2;

                    tmp2 = scanRowy[i1];
                    scanRowy[i1] = scanRowy[i2];
                    scanRowy[i2] = tmp2;
            }
        }
    }

        /// Xw focus
    INT16 starIdx = -1;
    for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
        starIdx = ( scanRowX[i1]<=0 )? (i1) : starIdx;
        INT16 Idx1 =  (starIdx) + ((LineNum - starIdx)>>1);

        INT16 minXw = scanRowX[starIdx];
        INT16 maxXw = scanRowX[ (LineNum - 1) ];

        if( (maxXw - minXw) < THD_XwFocus )
        {
            /// Success
            isSuccessOnce = TRUE;
            INT32 MedianX = scanRowX[ Idx1 ]; //cm   
            INT32 Medianx = scanRowx[ Idx1 ];   
            INT32 Mediany = scanRowy[ Idx1 ]; 
            INT32 tmpXw3,tmpYw3;
            WModel->PosImgToWorld( tmpXw3 ,tmpYw3, Medianx,Mediany );

            CTACandidate->ObjPosXY[0]     =  XwShiftToBackCenter(MedianX) ; ///< cm(Ori:back center of self-car)
            CTACandidate->ObjPosXY[1]     =  YwShiftToBackCenter(tmpYw3);
            CTACandidate->isObjPosXYValid = TRUE;      ///< World Xw Find
            CTACandidate->ObjPosxy[ 0 ]   = Medianx;
            CTACandidate->ObjPosxy[ 1 ]   = Mediany;
            CTACandidate->isObjPosxyValid = TRUE;  
// debug
//OSD_SetColor(255,255,0);
//OSD_Draw_Rec(dst_Y,640,480,CTACandidate->ObjPosxy[ 0 ], CTACandidate->ObjPosxy[ 1 ],CTACandidate->ObjPosxy[ 0 ]+2, CTACandidate->ObjPosxy[ 1 ]+2,1,true );
//OSD_SetColor(255,0,0);
//OSD_Draw_Rec(dst_Y,640,480,scanRowx[Idx1-1], scanRowy[Idx1-1],scanRowx[Idx1-1]+2, scanRowy[Idx1-1]+2,1,true );
//OSD_Draw_Rec(dst_Y,640,480,scanRowx[Idx1+1], scanRowy[Idx1+1],scanRowx[Idx1+1]+2, scanRowy[Idx1+1]+2,1,true );
            return;
        }
    } // end of if( 3 <= ValidNumXw  )
    #pragma endregion

    #pragma region ====CTACandidate->ObjPosXYMethod2==========
    INT32 minXw = 99999;
    INT32 maxXw = -1;
    ///1) Initial scanRowx[ LineNum ]
    for ( INT16 i1 = 0; i1 < LineNum  ; ++i1 )
        scanRowx[i1] = -1;

    INT32  moveDir = (isLeft)?(-1):(+1);
    CONST INT32 THD_BlackWidth = 4; //< (unit: pixel)
    for ( INT16 vertIdx = 0; vertIdx < LineNum  ; ++vertIdx )
    {
        offsetV = scanRowy[ vertIdx ] * imgDimensionX;
        for ( INT16 hori = (isLeft)?(endx):(startx) ; hori != ((isLeft)?(startx):(endx)) ; (isLeft)?(--hori):(++hori)  )
        {
            if( THD_GaussImgVer2 >= gaussImg[ offsetV + hori ] )
            {
                BOOL isBigWidth = TRUE;
                for ( INT32 i1 = 0 ; i1 < THD_BlackWidth ; ++i1)
                {
                    if( THD_GaussImgVer2 < gaussImg[ offsetV + hori + moveDir*i1 ] )
                    {
                        isBigWidth = FALSE;
                    }
                }
                if( TRUE == isBigWidth)
                {
                    scanRowx[ vertIdx ] = hori ;
                    break;
                }
            }// end of if( THD_GaussImgVer2 >= gaussImg[ offsetV + hori ] )
        }// end of [  hori <= endx ]
    } // end of [ vertIdx <= LineNum ]

    ///2) Transfer to the scanRowX[LineNum]
    for ( INT16 i1 = 0; i1 < LineNum  ; ++i1 )
        scanRowX[i1] = -1;

    for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
    {
        WModel->PosImgToWorld( tmpXw ,tmpYw, scanRowx[i1],scanRowy[i1] );
        if( tmpXw != 0xFFFF )
        {
            scanRowX[i1] = tmpXw ; ///< cm
            ++ValidNumXw;
        }
    }

///debug
//for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
//{
//        if(scanRowx[i1] < 0) continue;
//        OSD_SetColor(255,255,0);
//        OSD_Draw_Rec(dst_Y,640,480,scanRowx[i1], scanRowy[i1],scanRowx[i1]+2, scanRowy[i1]+2,1,true );
//}
    maxXw = -1;
    minXw = 9999;
    for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
    {
        WModel->PosImgToWorld( tmpXw ,tmpYw, scanRowx[i1],scanRowy[i1] );
        if( tmpXw != 0xFFFF )
        {
            maxXw = MAX(maxXw, tmpXw);
            minXw = MIN(minXw, tmpXw);
        }
    }

    ///3) isValidNum  & Xs focus
    BOOL isPassMethod2 = FALSE;
    if( (2 < ValidNumXw) && ((ABS(maxXw - minXw)) < THD_XwDiff ) ) isPassMethod2 = TRUE;
    if( (5 < ValidNumXw) && ((ABS(maxXw - minXw)) < THD_XwDiff2) ) isPassMethod2 = TRUE;
    if( (6 < ValidNumXw)                                         ) isPassMethod2 = TRUE;

    maxXw = -1;
    minXw = 9999;
    if ( isPassMethod2 )
    {
        if (isLeft)
        {
            /// 4) Save the min Xw
            for( INT16 i1 = 0 ; i1 < LineNum ; ++i1)
            {
                if( (scanRowX[i1] > maxXw) && (scanRowX[i1]>0) )
                {
                    maxXw = scanRowX[i1];
                    INT32 MedianX = scanRowX[ i1 ]; //cm   
                    INT32 Medianx = scanRowx[ i1 ];   
                    INT32 Mediany = scanRowy[ i1 ];    
                    INT32 tmpXw3,tmpYw3;
                    WModel->PosImgToWorld( tmpXw3 ,tmpYw3, Medianx,Mediany );

                    CTACandidate->ObjPosXY[0]     =  XwShiftToBackCenter(MedianX) ; ///< cm(Ori:back center of self-car)
                    CTACandidate->ObjPosXY[1]     =  YwShiftToBackCenter(tmpYw3)  ; ///< cm(Ori:back center of self-car)
                    CTACandidate->isObjPosXYValid = TRUE;      ///< World Xw Find
                    CTACandidate->ObjPosxy[ 0 ]   = Medianx;
                    CTACandidate->ObjPosxy[ 1 ]   = Mediany;
                    CTACandidate->isObjPosxyValid = TRUE;  
                }
            } // end of i1 < LineNum
        }
        else
        {
            /// 4) Save the max Xw
            for( INT16 i1 = 0 ; i1 < LineNum ; ++i1)
            {
                if( (scanRowX[i1] < minXw) && (scanRowX[i1]>0))
                {
                    minXw = scanRowX[i1];
                    INT32 MedianX = scanRowX[ i1 ]; //cm   
                    INT32 Medianx = scanRowx[ i1 ];   
                    INT32 Mediany = scanRowy[ i1 ];   
                    INT32 tmpXw3,tmpYw3;
                    WModel->PosImgToWorld( tmpXw3 ,tmpYw3, Medianx,Mediany ); 

                    CTACandidate->ObjPosXY[0]     =  XwShiftToBackCenter(MedianX) ; ///< cm(Ori:back center of self-car)
                    CTACandidate->ObjPosXY[1]     =  YwShiftToBackCenter(tmpYw3) ;  ///< cm(Ori:back center of self-car)
                    CTACandidate->isObjPosXYValid = TRUE;      ///< World Xw Find
                    CTACandidate->ObjPosxy[ 0 ]   = Medianx;
                    CTACandidate->ObjPosxy[ 1 ]   = Mediany;
                    CTACandidate->isObjPosxyValid = TRUE;  
                }
            } // end of i1 < LineNum
        } // end of isLeft

    }
    #pragma endregion
    
    #if 0 /// Multi-THD_CTACandidate->ObjPosXY
    #pragma region ====Multi-THD_CTACandidate->ObjPosXY==========

    INT32 minXw = 99999;
    INT32 maxXw = -1;
    for( INT16 THD_Gauss = THD_GaussImg[0] ; THD_Gauss < THD_GaussImg[1] ; THD_Gauss += 3 )
    {
        ///1) Initial scanRowx[ LineNum ]
        INT16 scanRowx[ LineNum ];
        for ( INT16 i1 = 0; i1 < LineNum  ; ++i1 )
            scanRowx[i1] = -1;

        INT32  offsetV;
        for ( INT16 vertIdx = 0; vertIdx < LineNum  ; ++vertIdx )
        {
            offsetV = scanRowy[ vertIdx ] * imgDimensionX;
            for ( INT16 hori = (isLeft)?(endx):(startx) ; hori != ((isLeft)?(startx):(endx)) ; (isLeft)?(--hori):(++hori)  )
            {
                if( THD_Gauss >= gaussImg[ offsetV + hori ] )
                {
                    scanRowx[ vertIdx ] = hori ;
                break;
                }
            }// end of [  hori <= endx ]
        } // end of [ vertIdx <= LineNum ]

        ///2) Transfer to the scanRowX[LineNum]
        INT32 scanRowX[ LineNum ];
        for ( INT16 i1 = 0; i1 < LineNum  ; ++i1 )
            scanRowX[i1] = -1;

        INT32 tmpXw,tmpYw;
        INT32 ValidNumXw = 0 ;
        for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
        {
            WModel->PosImgToWorld( tmpXw ,tmpYw, scanRowx[i1],scanRowy[i1] );
            if( tmpXw != 0xFFFF )
            {
                scanRowX[i1] = tmpXw ; ///< cm
                ++ValidNumXw;
            }
        }

///debug
//for ( INT32 i1 = 0 ; i1 < LineNum ; ++i1)
//{
//    if(scanRowx[i1] < 0) continue;
//    OSD_SetColor(0,255,0);
//    OSD_Draw_Rec(dst_Y,640,480,scanRowx[i1], scanRowy[i1],scanRowx[i1]+2, scanRowy[i1]+2,1,true );
//}
        ///3) isValidNum  & Xs focus
        if ( 2 >= ValidNumXw)
            continue;

        if (isLeft)
        {
            /// 4) Save the min Xw
            for( INT16 i1 = 0 ; i1 < LineNum ; ++i1)
            {
                if( (scanRowX[i1] > maxXw) && (scanRowX[i1]>0) )
                {
                    maxXw = scanRowX[i1];
                    INT32 MedianX = scanRowX[ i1 ]; //cm   
                    INT32 Medianx = scanRowx[ i1 ];   
                    INT32 Mediany = scanRowy[ i1 ];    
                    INT32 tmpXw3,tmpYw3;
                    WModel->PosImgToWorld( tmpXw3 ,tmpYw3, Medianx,Mediany );

                    CTACandidate->ObjPosXY[0]     =  XwShiftToBackCenter(MedianX) ; ///< cm(Ori:back center of self-car)
                    CTACandidate->ObjPosXY[1]     =  YwShiftToBackCenter(tmpYw3)  ; ///< cm(Ori:back center of self-car)
                    CTACandidate->isObjPosXYValid = TRUE;      ///< World Xw Find
                    CTACandidate->ObjPosxy[ 0 ]   = Medianx;
                    CTACandidate->ObjPosxy[ 1 ]   = Mediany;
                    CTACandidate->isObjPosxyValid = TRUE;  
                }
            } // end of i1 < LineNum
        }
        else
        {
            /// 4) Save the max Xw
            for( INT16 i1 = 0 ; i1 < LineNum ; ++i1)
            {
                if( (scanRowX[i1] < minXw) && (scanRowX[i1]>0))
                {
                    minXw = scanRowX[i1];
                    INT32 MedianX = scanRowX[ i1 ]; //cm   
                    INT32 Medianx = scanRowx[ i1 ];   
                    INT32 Mediany = scanRowy[ i1 ];   
                    INT32 tmpXw3,tmpYw3;
                    WModel->PosImgToWorld( tmpXw3 ,tmpYw3, Medianx,Mediany ); 

                    CTACandidate->ObjPosXY[0]     =  XwShiftToBackCenter(MedianX) ; ///< cm(Ori:back center of self-car)
                    CTACandidate->ObjPosXY[1]     =  YwShiftToBackCenter(tmpYw3) ;  ///< cm(Ori:back center of self-car)
                    CTACandidate->isObjPosXYValid = TRUE;      ///< World Xw Find
                    CTACandidate->ObjPosxy[ 0 ]   = Medianx;
                    CTACandidate->ObjPosxy[ 1 ]   = Mediany;
                    CTACandidate->isObjPosxyValid = TRUE;  
                }
            } // end of i1 < LineNum
        } // end of isLeft
    }// end of THD_Gauss < THD_GaussImg[1]
    #pragma endregion
    #endif
}
/**
* @brief	 Get the Object Speed(Right Dir is Positive , Left Dir is Negative)
* @param[in] CTACandidate	Input Valid Candidate
* @return					is Output Speed valid
*/
/*
BOOL Estimation::GetCandSpeedbyMotionCorner( CandidateRec CTACandidate[], FLOAT infps )
{
  
        FLOAT xDirMean = (INT32)CTACandidate->CornerVector[0]-(INT32)CTACandidate->CornerVector[2] ;//Right
        FLOAT yDirMean = (INT32)CTACandidate->CornerVector[1]-(INT32)CTACandidate->CornerVector[3] ;//Up
        
        xDirMean /= (FLOAT)( MAX(1.0, CTACandidate->CornerQuantity) );
        yDirMean /= (FLOAT)( MAX(1.0, CTACandidate->CornerQuantity) );

        CTACandidate->CornerMove[0]=xDirMean;//紀錄corner移動量
        CTACandidate->CornerMove[1]=-yDirMean;
        
    if( CTACandidate->isObjPosXYValid ) // ObjPosXY , ObjPosxy are Valid
    {
        INT32 Xw,Yw;
        INT32 tmpDiff_x = ROUND(xDirMean);
        INT32 tmpDiff_y = ROUND(yDirMean);
        WModel->PosImgToWorld( Xw,Yw, CTACandidate->ObjPosxy[0]-tmpDiff_x,CTACandidate->ObjPosxy[1] - tmpDiff_y );
        if ( Xw != 0xFFFF )
        {
            FLOAT FromXw = XwShiftToBackCenter( Xw ); //cm
            FLOAT FromYw = YwShiftToBackCenter( Yw ); //cm

            CTACandidate->ObjSpeedXY[0] = (FLOAT)(CTACandidate->ObjPosXY[0] - FromXw)*(FLOAT)infps; // cm/sec(due to CTAFPS =20 , CTA_TV_COMPRESS=5, so,數值是100*N)
            CTACandidate->ObjSpeedXY[1] = (FLOAT)(CTACandidate->ObjPosXY[1] - FromYw)*(FLOAT)infps; // cm/sec
            
            CTACandidate->ObjSpeedXY[0] = PRECISION_2( CTACandidate->ObjSpeedXY[0] );//395.76999
            CTACandidate->ObjSpeedXY[1] = PRECISION_2( CTACandidate->ObjSpeedXY[1] );//-3936.87012
            CTACandidate->isObjSpeedXYValid = TRUE;

            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}
*/
/**
* @brief	 Get the Object Speed(Right Dir is Positive , Left Dir is Negative) by the related last frame Position
* @param[in] CTACandidate	Input Valid Candidate
* @return					is Output Speed valid
*/
BOOL Estimation::GetCandSpeedbyHeadPos( CandidateRec CTACandidate[] , CandidateRec CTACandidatePrev[], FLOAT infps )
{
    if( (CTACandidate->isObjPosXYValid) && (CTACandidatePrev->isObjPosXYValid) )
    {
        CTACandidate->ObjSpeedXY[0] = (FLOAT)(CTACandidate->ObjPosXY[0] - CTACandidatePrev->ObjPosXY[0])*(FLOAT)infps; // cm/sec(due to CTAFPS =20 )
        CTACandidate->ObjSpeedXY[1] = (FLOAT)(CTACandidate->ObjPosXY[1] - CTACandidatePrev->ObjPosXY[1])*(FLOAT)infps; // cm/sec,vertical speed

        CTACandidate->ObjSpeedXY[0] = PRECISION_2( CTACandidate->ObjSpeedXY[0] );
        CTACandidate->ObjSpeedXY[1] = PRECISION_2( CTACandidate->ObjSpeedXY[1] );

        CTACandidate->isObjSpeedXYValid = TRUE;
        return TRUE;
    }
	else
		CTACandidate->isObjSpeedXYValid = FALSE;
    return FALSE;
}

/**
* @brief	  Speed smooth by adaptive weight
* @param[in]  PosXY		Input World Position of Xw [Ref:GetCandPos() ]
* @param[in]  Speed		Input Speed X and Y		   [Ref:GetCandSpeed()]
* @param[out] TTC		Output Time to collision(ms)
* @return				is Output TTC valid
*/
void Estimation::GetEstimatedSpeed( CandidateRec CTACandidate[], CandidateRec CTACandidatePrev[], BOOL isLeft )
{

    if( (CTACandidatePrev->isObjSpeedXYValid) && (CTACandidate->isObjSpeedXYValid) ) //Tracking
    {
        FLOAT speed1 = CTACandidatePrev->ObjSpeedXY[0]; // cm/sec
        FLOAT speed2 = CTACandidate    ->ObjSpeedXY[0];
        FLOAT w      = CTACandidatePrev->ObjSpeedWeight;

        CONST FLOAT MaxSpeed = 1722.2; //cm/sec
        speed1 = MAX_MIN_RANGE(speed1, -MaxSpeed,MaxSpeed);
        speed2 = MAX_MIN_RANGE(speed2, -MaxSpeed,MaxSpeed);

		CTACandidate->ObjSpeedXY[0] = (FLOAT)( w * speed1 +(1.0 - w) * speed2 );
        CTACandidate->ObjSpeedXY[0] = PRECISION_2( CTACandidate->ObjSpeedXY[0] );

        /// update weight
        if((ABS(speed1-speed2)) < 278 )//diffSpeed<[278(cm/s)*0.036=]10(km/h)
        {
            w = MAX_MIN_RANGE( (w + 0.2) , 0.1 , 0.9);
        }
        else if((ABS(speed1-speed2)) < 556 )//diffSpeed<20(km/h)
        {
            w = MAX_MIN_RANGE( (w - 0.1) , 0.1 , 0.9);
        }
        else//>20(km/h)
        {
            w = MAX_MIN_RANGE( (w - 0.15) , 0.1 , 0.9);
        }
        CTACandidate->ObjSpeedWeight = w ;
    }
    else if( (FALSE == CTACandidatePrev->isObjSpeedXYValid) && (CTACandidate->isObjSpeedXYValid) )//New Cand
    {
        FLOAT speed2 = CTACandidate->ObjSpeedXY[0];

        CONST FLOAT MaxSpeed = 1722.2;//==62(km/h), why?? what is CTA spec?
        speed2 = MAX_MIN_RANGE(speed2, -MaxSpeed, MaxSpeed);

        CTACandidate->ObjSpeedXY[0] = (FLOAT) (0.5 * speed2);//why 1/2 measured speed?
        CTACandidate->ObjSpeedXY[0] = PRECISION_2( CTACandidate->ObjSpeedXY[0] );
        CTACandidate->ObjSpeedWeight = 0.5 ;
    }
    else
    {
        CTACandidate->ObjSpeedXY[0] = 0.0;
    }
}


/**
* @brief	  Get the Object Speed(Right Dir is Positive , Left Dir is Negative)
* @param[in]  PosXY		Input World Position of Xw [Ref:GetCandPos() ]
* @param[in]  Speed		Input Speed X and Y		   [Ref:GetCandSpeed()]
* @param[out] TTC		Output Time to collision(ms)
* @return				is Output TTC valid
*/
BOOL Estimation::GetCandTTC( CandidateRec CTACandidate[] , INT32 imgDimensionX, BOOL isLeft, SimpleCarSpec &i_carSpec )
{
    CONST FLOAT MIN_TTC = 0;    ///< the Lowest TTC Num
    CONST FLOAT MAX_TTC = 9999; ///< (ms) Max TTC Num
    if( CTACandidate->isObjPosXYValid &&
        CTACandidate->isObjSpeedXYValid ) 
    {
        if( isLeft )
        {
            INT32 tmp01     = CTACandidate->ObjPosXY[0];			//cm
            INT32 carcenter = 0;									//cm
            FLOAT TTCdist   = carcenter - (INT32)(i_carSpec.CarWidth_cm>>1) - tmp01; //cm
            FLOAT TTCSpeed  = CTACandidate->ObjSpeedXY[0];			//cm/sec
            TTCSpeed        = ( 0 != TTCSpeed)? TTCSpeed : +1;

            CTACandidate->CandTTC = PRECISION_2( (TTCdist / TTCSpeed)) * 1000 ;
            CTACandidate->CandTTC = MAX_MIN_RANGE(CTACandidate->CandTTC, MIN_TTC, MAX_TTC );
        }
        else //Right
        {
            INT32 tmp01     = CTACandidate->ObjPosXY[0];            //cm
            INT32 carcenter = 0;                                    //cm
            FLOAT TTCdist   = carcenter + (INT32)(i_carSpec.CarWidth_cm>>1) - tmp01;   //cm
            FLOAT TTCSpeed  = CTACandidate->ObjSpeedXY[0];          //cm/sec
            TTCSpeed        = ( 0 != TTCSpeed)? TTCSpeed : -1;

            CTACandidate->CandTTC = PRECISION_2( (TTCdist / TTCSpeed)) * 1000 ;
            CTACandidate->CandTTC = MAX_MIN_RANGE(CTACandidate->CandTTC, MIN_TTC, MAX_TTC );
            //CTACandidate->CandTTC = ( 0 != CTACandidate->ObjSpeedXY[0] )?( PRECISION_2(TTCdist / TTCSpeed) * 1000):( MAX_TTC ); //ms
        }
        return TRUE;
    }
    else  /// != isObjPosXYValid & isObjSpeedXYValid
    {
        CTACandidate->CandTTC = MAX_TTC;
    }
    return FALSE;
}

/**
* @brief	 UpdateFlagTHD is Recalculate the CountDownFlagAdpL/CountDownFlagAdpR 2 Flag Parameter
* @param[in] CTACandidateL		Input Valid Candidate at current  frame
* @param[in] CTACandidateR		Input Valid Candidate at current  frame
*/
void Estimation::UpdateFlagTHD( CandidateRec CTACandidateL[] , CandidateRec CTACandidateR[], CONST INT32 inMaxCandL, CONST INT32 inMaxCandR )
{
    BOOL isROIhasCandL = FALSE;	///< Decide if CountDownFlagAdpL add;
    BOOL isROIhasCandR = FALSE; ///< Decide if CountDownFlagAdpR add;

    // PreProcess of CountDownFlagAdpL
    INT16 MaxCountL = 0; 
    for ( INT32 i1 = 0 ; i1 < inMaxCandL ; ++i1) // CTACandidate Total number
    {
        if( Valid == CTACandidateL[i1].isValid )
        {
            isROIhasCandL = TRUE;
            MaxCountL = MAX(CTACandidateL[i1].countDown , MaxCountL);
        }
    }

    INT16 MaxCountR = 0; 
    for ( INT32 i1 = 0 ; i1 < inMaxCandR ; ++i1) // CTACandidate Total number
    {
        if( Valid == CTACandidateR[i1].isValid )
        {
            isROIhasCandR = TRUE;
            MaxCountR = MAX(CTACandidateR[i1].countDown , MaxCountR);
        }
    }

    if( isROIhasCandL )
        CountDownFlagAdpL = MAX( CountDownFlagAdaptiveMax , MaxCountL );
    else
        CountDownFlagAdpL = MIN( (CountDownFlagAdpL+1) , CountDownFlagMax );
    if( isROIhasCandR )
        CountDownFlagAdpR = MAX( CountDownFlagAdaptiveMax , MaxCountR );
    else
        CountDownFlagAdpR = MIN( (CountDownFlagAdpR+1) , CountDownFlagMax );
}


/**
* @brief	 關聯性 PrevLamp <-> CurrentLamp [relatedLamp] 
* @n         原則: Overlap Similarity / Area Similarity 
* @param[in] lamp		           Input lamp at current  frame
* @param[in] lampPrev	           Input lamp at previous frame
* @param[in] in_MaxNum	           Input maximum lamp Number
* @param[in] THD_RelatedLamp[2]    符合"OverlapRatio(0-1)"與"大小(Area)" 相似 的 Threshold
* @return	 the Number that successfully map from previous to current lamp.
*/
UINT16 Estimation::LampAssociation( HeadLight lamp[MAX_HeadLightNum] , INT16 in_MaxNum, HeadLight lampPrev[MAX_HeadLightNum], INT16 in_MaxNumPrev , const FLOAT THD_RelatedLamp[2] )
{

    ///符合Overlap與大小 相似 的 Candidates
    FLOAT THD_OverlapRatio = THD_RelatedLamp[0];	///< [0-1]
    FLOAT THD_AREA         = THD_RelatedLamp[1];	///< 3 or 1/3 times  then Previous 

    UINT16 retCount     = 0;    
    for ( INT32 i1 = 0 ; i1 < in_MaxNumPrev ; ++i1) // PrevHeadLight
    {
        if( FALSE == lampPrev[i1].isValid )continue;

        /// Cal the Overlapped Area
        UINT16 areaA = lampPrev[i1].width * lampPrev[i1].height ;

        UINT16 areaB;
        UINT16 areaA_and_B;
        FLOAT score1 , score2;
        for ( INT32 i2 = 0 ; i2 < in_MaxNum ; ++i2)
        {
            if(FALSE == lamp[i2].isValid )continue;

            areaB = lamp[i2].width * lamp[i2].height ;

            areaA_and_B = 0 ;
            for( INT32 vert = lampPrev[i1].startyi ; vert< (lampPrev[i1].startyi + lampPrev[i1].height+1);++vert)
            {
                for( INT32 hori = lampPrev[i1].startxi ; hori< (lampPrev[i1].startxi+lampPrev[i1].width+1);++hori)
                {
                    if( ( vert >= lamp[i2].startyi )&&(vert<=(lamp[i2].startyi+lamp[i2].height))&&
                        ( hori >= lamp[i2].startxi )&&(hori<=(lamp[i2].startxi+lamp[i2].width)))
                    {
                        areaA_and_B ++;
                    }
                }
            }
            score1 = FLOAT(areaA_and_B<<1)/(areaA+areaB);   ///< overlapped score
            score2 = FLOAT(areaA) / (FLOAT)areaB; //AreaCmp

            if( THD_OverlapRatio < score1 ) /// Satisfy Overlap Similarity
            {
                if( (score2 <= THD_AREA) && (score2*THD_AREA)>=1 )   //( AreaCmp >= (1/THD_AREA) )
                {
                    lampPrev[i1].relatedLamp = i2; // Index紀錄 lampPrev -> lamp
                    lampPrev[i1].isRelatedLampValid = TRUE; // Index紀錄 lampPrev -> lamp
                    lamp[i2].relatedLamp = i1;     // Index紀錄 lamp -> lampPrev
                    lamp[i2].isRelatedLampValid     = TRUE; // Index紀錄 lamp -> lampPrev
                    ++retCount; // success Related
                }
            }
        }// end of (i2 < MAX_HeadLightNum)
    }// end of (i1 < MAX_HeadLightNum)
    return retCount;  
}

/**
* @brief	 Tracking the Prevlamp that not matches successfully. 
* @param[in] lamp		        Input lamp at current  frame
* @param[in] lampPrev	        Input lamp  at previous frame
* @param[in] THD_WeightSpeed    smooth the Pos/Speed in temporal domain
*/
void Estimation::LampSuperglue( HeadLight lampPrev[MAX_HeadLightNum] , INT16 &MaxNumPrev, HeadLight lamp[MAX_HeadLightNum] , INT16 &MaxNum , UCHAR *g_Gaussian_img , UCHAR *g_Gaussian_PrevImg, INT32 imgDimensionX )
{
    ///@todo:  
    ///1) find Prevlamp that not matches successfully.
    ///2) tracking by gauss img
    ///3) new a lamp that represent it.
    ///4) revise the lamp number.
}

/**
* @brief	 Update lamp[Flag,Speed,Pos,ttc] info.
* @param[in] lamp		        Input lamp at current  frame
* @param[in] lampPrev	        Input lamp  at previous frame
* @param[in] THD_WeightSpeed    smooth the Pos/Speed in temporal domain
*/
void Estimation::GetLampInfo( UCHAR *inBinImg,INT32 inImgWidth, HeadLight lamp[MAX_HeadLightNum], INT16 in_MaxNum ,HeadLight lampPrev[MAX_HeadLightNum], INT16 in_MaxNumPrev ,FLOAT infps, const FLOAT THD_WeightSpeed[2] )
{
    /// THRESHOLD
    FLOAT Weight_SmoothSpeed = THD_WeightSpeed[0]; //  means (Weight)*PreFrame+(1-Weight)*CurFrame
    FLOAT Weight_SmoothPos   = THD_WeightSpeed[1]; //  means (Weight)*PreFrame+(1-Weight)*CurFrame
    INT16 AddFlag = -1;
    for ( int i1 = 0 ; i1 < in_MaxNum ; ++i1)    
    {
        if( FALSE == lamp[i1].isValid ) continue;

        if( TRUE == lamp[i1].isRelatedLampValid ) // tracked lamp
        {
            /// Position [ObjSpeedXY/ObjPosXY]
            GetLampPosNight( &lamp[i1], &lampPrev[lamp[i1].relatedLamp], inBinImg, inImgWidth,Weight_SmoothPos );

            /// Speed
            GetLampSpeedNight(&lamp[i1], &lampPrev[lamp[i1].relatedLamp], infps, Weight_SmoothSpeed);

            /// TTC
            GetLampTTCNight(&lamp[i1]);

            /// CountDown
            CalLampCountDown( &lamp[i1], &lampPrev[lamp[i1].relatedLamp] );

        }
        else    // New Candidate
        {
            /// Position [ObjSpeedXY/ObjPosXY]
            GetLampPosNight( &lamp[i1], &lampPrev[lamp[i1].relatedLamp], inBinImg, inImgWidth,Weight_SmoothPos );

            lamp[i1].countDown = CountDownFlagMax;

        }

    }

}

/**
* @brief	 find Lamp position(Xw,Yw)[xi,yi -> Xw,Yw]
* @param[in] inlamp		Input lamp at current  frame
* @param[in] inPrevlamp Input lamp at previous frame
*/
void Estimation::GetLampPosNight( HeadLight inlamp[] ,HeadLight inPrevlamp[],UCHAR *inBinImg , INT32 inImgWidth , FLOAT insmoothRatio )
{
    ///by LUT
    CONST FLOAT HeightCam  = 108; ///< (cm)
    CONST FLOAT HeightLamp = 60;  ///< (cm)

    #pragma region ======ObjPosxy======
    INT32 posxi = inlamp->startxi + (inlamp->width>>1);
    INT32 posyi = inlamp->startyi + (inlamp->height  );

    inlamp->ObjPosxy[0]     = posxi;
    inlamp->ObjPosxy[1]     = posyi;
    inlamp->isObjPosxyValid = TRUE;
    #pragma endregion
    #pragma region ======ObjPosXY======
    INT32 tmpXw,tmpYw;
    WModel->PosImgToWorld( tmpXw ,tmpYw, posxi, posyi );
    if( tmpXw != 0xFFFF )
    {
        tmpXw = XwShiftToBackCenter( tmpXw ) ; ///< cm(Ori:back center of self-car)
        tmpYw = YwShiftToBackCenter( tmpYw ) ; ///< cm(Ori:back center of self-car)
        
        tmpXw *= (HeightCam - HeightLamp)/HeightCam;
        tmpYw *= (HeightCam - HeightLamp)/HeightCam;

        insmoothRatio       = ( TRUE ==inPrevlamp->isObjPosXYValid )? insmoothRatio : 1;  // No smooth in case of T-1 is not valid
        inlamp->ObjPosXY[0] = (insmoothRatio)*(float)tmpXw + (1-insmoothRatio)*(float)inPrevlamp->ObjPosXY[0] ;
        inlamp->ObjPosXY[1] = (insmoothRatio)*(float)tmpYw + (1-insmoothRatio)*(float)inPrevlamp->ObjPosXY[1] ;

        inlamp->isObjPosXYValid = TRUE;
    }
    else
    {
        inlamp->isObjPosXYValid = FALSE;
    }
    #pragma endregion
    
}

/**
* @brief	 find Lamp Speed and Smooth
* @param[in] inlamp		Input tracked lamp at current  frame
* @param[in] inPrevlamp Input lamp at previous frame
* @param[in] infps      FPS(frame numbers per second)
*/
void Estimation::GetLampSpeedNight( HeadLight inlamp[] ,HeadLight inPrevlamp[], FLOAT infps, FLOAT insmoothRatio )
{
    #pragma region ======ObjSpeedXY======
    if( (TRUE == inlamp->isObjPosXYValid) && (TRUE == inPrevlamp->isObjPosXYValid) )
    {
        inlamp->ObjSpeedXY[ 0 ]   = (inlamp->ObjPosXY[0] - inPrevlamp->ObjPosXY[0]) * (FLOAT)infps ; //cm/sec
        inlamp->ObjSpeedXY[ 1 ]   = (inlamp->ObjPosXY[1] - inPrevlamp->ObjPosXY[1]) * (FLOAT)infps ; //cm/sec

        if( TRUE == inPrevlamp->isObjSpeedXYValid )
        {
            inlamp->ObjSpeedXY[0] = (insmoothRatio)*inlamp->ObjSpeedXY[0] + (1-insmoothRatio)* inPrevlamp->ObjSpeedXY[0] ;
            inlamp->ObjSpeedXY[1] = (insmoothRatio)*inlamp->ObjSpeedXY[1] + (1-insmoothRatio)* inPrevlamp->ObjSpeedXY[1] ;
        }

        inlamp->isObjSpeedXYValid = TRUE;
    }

    #pragma endregion
    
}

/**
* @brief	 find Lamp TTC and smooth
* @param[in] inlamp		Input lamp at current  frame
*/
void Estimation::GetLampTTCNight( HeadLight inlamp[] )
{
    const FLOAT MAX_TTC = 8888; ///< the invalid TTC Num
    if( (TRUE == inlamp->isObjSpeedXYValid) && (TRUE == inlamp->isObjPosXYValid) )
    {
        inlamp->TTC = - inlamp->ObjPosXY[0] / inlamp->ObjSpeedXY[0]; //sec
        inlamp->TTC *= 1000; //ms
        inlamp->TTC = MAX(inlamp->TTC , 0); 
    }
}

/**
* @brief	 find Lamp TTC and smooth
* @param[in] inlamp		Input lamp at current  frame
* @param[in] inPrevlamp Input lamp at previous frame
*/
void Estimation::CalLampCountDown( HeadLight inlamp[] ,HeadLight inPrevlamp[] )
{
    INT16 AddFlag = 1;
    if( (inlamp->TTC < CTATTC) &&( inlamp->TTC > 1 ) )
        AddFlag = -1;

    inlamp->countDown = inPrevlamp->countDown + AddFlag ;
    inlamp->countDown = MAX_MIN_RANGE( inlamp->countDown,0,CountDownFlagMax) ;

}


/**
* @brief	 Re-calculate the CountDownFlagAdpL_Night/CountDownFlagAdpR_Night 2 Flag Parameter
* @param[in] lampL		Input lamp at current  frame
* @param[in] lampR		Input lamp at current  frame
*/
void   Estimation::UpdateFlagTHD_Night(HeadLight lampL[MAX_HeadLightNum] ,HeadLight lampR[MAX_HeadLightNum] )
{

}


Estimation* Estimation::getInstance()
{
    if(!instance)
        instance = new Estimation;
    return instance;
}

void Estimation::EstimationPatch()
{

}
Estimation::Estimation(void)
{
    CountDownFlagAdpL = CountDownFlagMax;
    CountDownFlagAdpR = CountDownFlagMax;
}
Estimation::~Estimation(void)
{
}

void Estimation::ResetInstance()
{
    delete instance; 
    instance = NULL;
}
