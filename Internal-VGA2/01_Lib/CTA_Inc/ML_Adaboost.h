/*
===================================================================
Name		: ML_Adaboost.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement Adaboost prediction Function

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/12/22	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

//---------------------------------------------------------------------------
#ifndef _ML_Adaboost_H_
#define _ML_Adaboost_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Type.h"
#include "CTA_Def.h"

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
#define BOOST ML_Adaboost::getInstance()
//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************
//===================Day============================//
#define ADA_SampleFeatureNum  7
#define ADA_ROUND  20
const FLOAT ADA_normalizedMean[ADA_SampleFeatureNum]={
0.6719728 , 0.6883813 , 0.1015968 , 0.006536354 , -42.6124 , -0.0042957 , 285.093 , };
const FLOAT ADA_normalizedStd[ADA_SampleFeatureNum]={
0.1851692 , 0.1178164 , 0.08494588 , 0.01338862 , 49.89874 , 0.01981468 , 113.7977 , };
const FLOAT ADA_SelSign[ ADA_ROUND ]={
1 , -1 , 1 , 1 , -1 , -1 , 1 , 1 , -1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , -1 , 1 , 1 , };
const INT32 ADA_SelFeatureIdx[ ADA_ROUND ]={
2 , 3 , 6 , 2 , 3 , 0 , 2 , 6 , 0 , 2 , 6 , 2 , 2 , 6 , 2 , 2 , 6 , 0 , 2 , 6 , };
const FLOAT ADA_SelFeatureTHD[ ADA_ROUND ]={
1.16999999999997 , -0.294000000000034 , 0.275999999999966 , 0.635999999999966 , -0.456000000000035 , 0.419999999999966 , 1.52399999999997 , 0.275999999999966 , 0.419999999999966 , 1.52399999999997 , 0.275999999999966 , -0.0300000000000343 , 1.52399999999997 , 0.275999999999966 , -0.282000000000034 , 1.52399999999997 , 0.275999999999966 , 0.419999999999966 , 1.52399999999997 , 0.275999999999966 , };
const FLOAT ADA_SelAlpha[ ADA_ROUND ]={
0.862755044037776 , 0.864619560509589 , 0.53764379946921 , 0.755751078140292 , 0.751903401963661 , 0.597414915844054 , 0.626676462218458 , 0.851222904361962 , 0.690587452813787 , 0.741700724115377 , 0.671379737126304 , 0.686641952701582 , 0.572445940112264 , 0.659310667621987 , 0.63570321214639 , 0.684226689698434 , 0.57498981465707 , 0.661095885995463 , 0.667148813582262 , 0.56547250448114 , };
//===================Night============================//

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

class ML_Adaboost
{
private:
    static ML_Adaboost *instance;
public:

    ML_Adaboost(void);
    ~ML_Adaboost(void);
    static ML_Adaboost* getInstance();
    static void ResetInstance();

    /** Update Features */
    void CandidateFeatureUpdate_1(CandidateRec CTACandidate[], INT16 inMaxCanNum, UCHAR *g_vSobel_img, UCHAR *g_hSobel_img, UCHAR *g_img_FrameDiff, 
    							  UINT16 imgDimensionX, UINT16 imgDimensionY, HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, int pointNumPrev);
    
    /** PREDICTION */
    UINT16 Predict_1(CandidateRec CTACandidate[] ,INT16 inMaxCand , INT16 validNum);

/*
    //-------------FeatureExtractionFromImg-----------//
    void  GetFeature_LBP81(UCHAR *img, INT32 ImgWidth , INT32 ImgHeight , INT32 startx,INT32 starty,INT32 windowWidth, INT32 windowHeight,INT32 *outLBP);
    FLOAT GetFeature_Symmetry( UCHAR *SrcImg, INT32 ImgWidth , INT32 ImgHeight , INT32 centerx,INT32 starty,INT32 windowHalfWidth, INT32 windowHeight );
    INT32 GetFeature_SymmetryCentralAxis(UCHAR *img, INT32 ImgWidth , INT32 ImgHeight , INT32 centerx,INT32 starty,INT32 windowHalfWidth, INT32 windowHeight,UINT32 moveLimitX);
    void  HOG_48_64(UCHAR *img , FLOAT HOGDescriptor[432]);
    void  SIFT_XXX();
    void  SURF_XXX();
*/
};


#endif // END #ifndef _ML_Adaboost_H_




