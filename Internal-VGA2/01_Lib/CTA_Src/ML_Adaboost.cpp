#ifdef _WIN32
  #include "stdafx.h"
#else // K2 platform
  #include "GlobalTypes.h"
#endif

#include "ML_Adaboost.h"
#include <math.h>       /* exp */

#include "Filter2D.h"

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
ML_Adaboost* ML_Adaboost::instance = NULL;

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
/**
* @brief		 CandidateFeatureUpdate calculate the CandidateRec feature List and put back.
* @param[in,out] CTACandidate		the Target
* @param[in]	 g_img_FrameDiff	the MotionEdge
* @param[in]	 g_vSobel_img		the Sobel V Edge
* @param[in]	 g_hSobel_img		the Sobel H Edge
*/
void ML_Adaboost::CandidateFeatureUpdate_1(CandidateRec CTACandidate[], INT16 inMaxCanNum, UCHAR *g_vSobel_img, UCHAR *g_hSobel_img, UCHAR *g_img_FrameDiff, 
                                           UINT16 imgDimensionX, UINT16 imgDimensionY, HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev,
                                           int pointNumPrev)
{
    /* the Features description
    float GetEdgeRatioH(); -0
    float GetEdgeRatioV(); -1
    float GetMotionEdgeRatio(); -2
    float GetCornerRatio( int in_CORNER_NUM);-3
    float GetMotionVectorVariance(int in_MV_VEC_DIFF , int in_CORNER_NUM);4
    float GetMotionVectorXiproj( int in_MV_VEC_R , int in_CORNER_NUM);5
    float GetROICentralXi(int in_StartXi);6
    */
    const UCHAR THD_SobelEdge = 10;

    for ( int i1 = 0 ; i1 < inMaxCanNum ; ++i1)
    {

//CandidateRec tmp2 = CTACandidate[i1];
        if( Invalid == CTACandidate[i1].isValid ) continue;

//CandidateRec tmp1 = CTACandidate[i1];
        /// Local Variables
        INT32 area1 = (CTACandidate[i1].height * CTACandidate[i1].width);
        FLOAT Area  = MAX( area1,1 ); 

        /// GetEdgeRatioH
        /// GetEdgeRatioV
        /// MotionEdgeRatio
        for ( UINT16 vert = CTACandidate[i1].startyi; vert < (CTACandidate[i1].startyi + CTACandidate[i1].height) ; ++vert)
        {
            for ( UINT16 hori = CTACandidate[i1].startxi; hori < (CTACandidate[i1].startxi + CTACandidate[i1].width) ; ++hori)
            {
                CTACandidate[i1].RecFeatures[0] += (   g_vSobel_img[hori + vert*(imgDimensionX) ] > THD_SobelEdge)?(1):(0);
                CTACandidate[i1].RecFeatures[1] += (   g_hSobel_img[hori + vert*(imgDimensionX) ] > THD_SobelEdge)?(1):(0);
                CTACandidate[i1].RecFeatures[2] += (g_img_FrameDiff[hori + vert*(imgDimensionX) ] > 0)?(1):(0);
            }
        }
        CTACandidate[i1].RecFeatures[0] /= Area;
        CTACandidate[i1].RecFeatures[1] /= Area;
        CTACandidate[i1].RecFeatures[2] /= Area;

        ///GetCornerRatio
        CTACandidate[i1].RecFeatures[3] = (FLOAT)CTACandidate[i1].CornerQuantity / Area;

        ///GetMotionVectorVariance
        FLOAT mean_x = 0;
        FLOAT mean_y = 0;
        FLOAT sumX2  = 0;
        FLOAT sumY2  = 0;
        FLOAT varX   = 0;
        FLOAT varY   = 0;
        FLOAT count  = 0;
        INT32 diff_x, diff_y;

        for( UINT32 i2 = 0 ; i2 < pointNumPrev; ++i2)
        {
            if( TRUE == harrisCorners2Prev[i2].isValid )  /// "Motioned" Harris Corners
            {
                /// Harris Corners in this CTACandidate ROI
                if( ( harrisCornersPrev[i2].x > CTACandidate[i1].startxi ) && (harrisCornersPrev[i2].x < CTACandidate[i1].startxi+CTACandidate[i1].width) &&
                    ( harrisCornersPrev[i2].y > CTACandidate[i1].startyi ) && (harrisCornersPrev[i2].y < CTACandidate[i1].startyi+CTACandidate[i1].height) )
                {
                    diff_x = harrisCorners2Prev[i2].Diff_x;
                    diff_y = harrisCorners2Prev[i2].Diff_y;

                    sumX2  += diff_x * diff_x;
                    sumY2  += diff_y * diff_y;
                    mean_x += diff_x ;
                    mean_y += diff_y ;
                    count++;
                }
            }
        }
        mean_x /= MAX(count,1.0);
        mean_y /= MAX(count,1.0);
        FLOAT tmpfloat = MAX(count,1.0);
        varX = (sumX2/ (FLOAT)(MAX(count,1.0)) ) - (mean_x*mean_x);
        varY = (sumY2/ (FLOAT)(MAX(count,1.0)) ) - (mean_y*mean_y);

        CTACandidate[i1].RecFeatures[4] = (count == 0)?(-100):((FLOAT)(varX + varY) / count); ///< Var = -100 if No motion corners

        ///GetMotionVectorXiproj
        CTACandidate[i1].RecFeatures[5] = (FLOAT)( CTACandidate[i1].CornerVector[0] - CTACandidate[i1].CornerVector[2])/ Area;  ///< Quantity.

        ///GetROICentralXi
        CTACandidate[i1].RecFeatures[6] = (FLOAT)(CTACandidate[i1].startxi + (CTACandidate[i1].width>>1) ); 

    }

}
/**
* @brief		 CandidateFeatureUpdate calculate the CandidateRec feature List and put back.
* @param[in,out] CTACandidate	I/O the Target
* @param[in]	 CandidateCount	Input the totalValidCandidateNum
* @return						the ML Filter Pass Number
*/
UINT16 ML_Adaboost::Predict_1( CandidateRec CTACandidate[] ,INT16 inMaxCand , INT16 validNum )
{
    /* Feature Description
    float GetEdgeRatioH(); -0
    float GetEdgeRatioV(); -1
    float GetMotionEdgeRatio(); -2
    float GetCornerRatio( int in_CORNER_NUM);-3
    float GetMotionVectorVariance(int in_MV_VEC_DIFF , int in_CORNER_NUM);-4
    float GetMotionVectorXiproj( int in_MV_VEC_R , int in_CORNER_NUM);-5
    float GetROICentralXi(int in_StartXi);-6
    */
    
//return validNum; //尚未建立正確Weight資訊

    const FLOAT THD_Adaboost = 0; ///< default = 0 ; Decision Threshold

    /// local variable
    FLOAT  featureArr[ ADA_SampleFeatureNum ] = {0};  ///< Normalized Features
    FLOAT  BOOSTAns                           =  0;   ///< Output of BOOST
    UINT16 PosCount                           =  0 ;

    for ( INT32 i1 = 0 ; i1 < inMaxCand ; ++i1) // CTACandidate Total Num
    {
        if( Invalid == CTACandidate[i1].isValid ) continue;

        /// Normalized features(z-score)
        for ( INT32 i2 = 0 ; i2 < ADA_SampleFeatureNum ;++i2)
        {
            featureArr[i2] = CTACandidate[i1].RecFeatures[i2] ;
            
            featureArr[i2] = (ADA_normalizedStd[i2]==0)?(featureArr[i2]):( MAX_MIN_RANGE( ((featureArr[i2] - ADA_normalizedMean[i2])/ADA_normalizedStd[i2]), -10 , 10 ) );
        }

        /// PREDICTION
        FLOAT sum = 0;
        FLOAT ht  = 0;
        for (INT32 times = 0 ; times < ADA_ROUND ;++times)
        {
            ht = ADA_SelSign[times] *( (featureArr[  ADA_SelFeatureIdx[times] ] < ADA_SelFeatureTHD[times] )?(-1.0):(1.0) );
            sum += ADA_SelAlpha[times] * ht;
        }

        /// Decision
        CTACandidate[i1].isValid = ( BOOSTAns < THD_Adaboost )? (Invalid ):(Valid     );
        PosCount                 = ( BOOSTAns < THD_Adaboost )? (PosCount):(PosCount+1);
    }
    return PosCount; // Filter Pass Number
}

ML_Adaboost* ML_Adaboost::getInstance()
{
    if(!instance)
        instance = new ML_Adaboost;
    return instance;
}
ML_Adaboost::ML_Adaboost(void)
{
}
ML_Adaboost::~ML_Adaboost(void)
{
}

void ML_Adaboost::ResetInstance()
{
    delete instance; 
    instance = NULL;
}
