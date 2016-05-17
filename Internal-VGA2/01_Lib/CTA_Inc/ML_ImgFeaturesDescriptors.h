/*
===================================================================
Name		: ML_ImgFeaturesDescriptors.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement modern image feature descriptors for the targets that frequently
              used to separate pedestrian, vehicles , motors.

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/12/22	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

//---------------------------------------------------------------------------
#ifndef _ML_ImgFeaturesDescriptors_H_
#define _ML_ImgFeaturesDescriptors_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Type.h"
#include "CTA_Def.h"

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

#define Descriptors ImgFeaturesDescriptors::getInstance()
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

class ImgFeaturesDescriptors
{
private:
    static ImgFeaturesDescriptors *instance;
public:

    ImgFeaturesDescriptors(void);
    ~ImgFeaturesDescriptors(void);
    static ImgFeaturesDescriptors* getInstance();
    static void ResetInstance();

    //-------------FeatureExtractionFromImg-----------//
    /** common used to find the face */
    void  GetFeature_LBP81(UCHAR *img, INT32 ImgWidth , INT32 ImgHeight , INT32 startx,INT32 starty,INT32 windowWidth, INT32 windowHeight,INT32 *outLBP);
    
    /** common used to find the symmetry obj , ex vehicle */
    FLOAT GetFeature_Symmetry( UCHAR *SrcImg, INT32 ImgWidth , INT32 ImgHeight , INT32 centerx,INT32 starty,INT32 windowHalfWidth, INT32 windowHeight );
    INT32 GetFeature_SymmetryCentralAxis(UCHAR *img, INT32 ImgWidth , INT32 ImgHeight , INT32 centerx,INT32 starty,INT32 windowHalfWidth, INT32 windowHeight,UINT32 moveLimitX);
    
    /** common used to express the silhouette , ex. pedestrian */
    void  HOG_48_64(UCHAR *img , FLOAT HOGDescriptor[432]);
    
    /** common used to express the texture , ex. pedestrian , vehicles*/
    void  SIFT_XXX();
    void  SURF_XXX();
    //-----------------------------------------------//
};


#endif // END #ifndef _ML_ImgFeaturesDescriptors_H_




