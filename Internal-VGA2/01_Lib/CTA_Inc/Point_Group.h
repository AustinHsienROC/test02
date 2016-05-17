/*
===================================================================
Name		: Point_Group.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement Point_Group .

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/09/01	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/
#ifndef _Point_Group_H_
#define _Point_Group_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Type.h"
#include "CTA_Def.h"
#include "CTAApi.h"
#include <stdio.h> 
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

enum ENUM_INT8_T{ MinorBlockTypeNoise = 0 , MinorBlockType1 = 1, MinorBlockType2  };

#define Cluster Point_Group::getInstance()

//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

class Point_Group
{
private:
    static Point_Group *instance;

public:
    Point_Group(void);
    ~Point_Group(void);

    static Point_Group* getInstance();
    static void ResetInstance();

    UINT16 MinorBlockUpdate(MinorBlock CTAMinorBlock[], UINT16 CTAMinorBlock_EdgeQuantity[], const INT32 *Region, 
							UCHAR *g_img_FrameDiff, HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, 
							INT32 imgDimensionX, INT32 imgDimensionY , const DOUBLE THD[3], int pointNumberPrev);

    UINT16 MinorBlockLabeling(MinorBlock inCTAMinorBlock[] );
    UINT16 CandidateUpdate(CandidateRec outCTACandidate[] , MinorBlock inCTAMinorBlock[] , UINT16 inCTAMinorBlock_EdgeQuantity[], INT16 LabelCount , const INT32 *Region , INT32 ImageWidth ,const FLOAT THD_CandidateUpdate[3], CAN_Info *i_carState);


    //---------Night_Mode---------//
    void ClusterIntoHeadlight(UCHAR *Binar_img, INT32 imgDimensionX, INT32 imgDimensionY, HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] , INT16 &_L_Num , INT16 &_R_Num,const INT32 RegionL[4], const INT32 RegionR[4]);
    void HeadlightPatch();
};


#endif // END #ifndef _LDWS_H_

