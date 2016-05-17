/**

    Copyright (C) 2014 By oToBrite Electronics Corp.

    @file 	CornerDetector.h
    @brief  This function implement corners detector methods. 

    @version	1nd
    @author		Austin
    @date		2014/10/20
    @n  * Implement Functions. -moravec / harris / nobel / shi-tomasi / Rosten-Drummond(FAST)/ SUSAN / SIFT/ SURF

*/////////////////////////////////////////////////////////////////
//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifndef _CornerDetector_H_
#define _CornerDetector_H_

#include "CTA_Type.h"
#include "CTA_Def.h"

#ifdef __cplusplus
    extern "C" {
#endif

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

#define CORNERS CornerDetector::getInstance()
//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
        
class CornerDetector
{
private:
    static CornerDetector *instance;

public:
    CornerDetector(void);
    ~CornerDetector(void);

    static CornerDetector* getInstance();

    //============CornerDetectorsFamily===================
    INT32 Corners_Moravec( UINT16 *MoravecMap , UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY );
    INT32 Corners_Harris( UINT16 *HarrisMap ,UCHAR *SobelEdge_V,UCHAR *SobelEdge_H ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY );
    INT32 Corners_Noble(UINT16 *NobleMap ,UCHAR *SobelEdge_V,UCHAR *SobelEdge_H ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY);//
    INT32 Corners_ShiTomasi(UINT16 *ShiTomasiMap ,UCHAR *SobelEdge_V,UCHAR *SobelEdge_H ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY );
    INT32 Corners_FAST(UINT16 *FASTMap ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY );
    INT32 Corners_SUSAN(UINT16 *SUSANMap ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY, HarrisCornerPos *harrisCorners , const INT32 sizeHarrisCorner , const INT32 RegionL[4] ,const INT32 RegionR[4] );
    INT32 Corners_SIFT(UINT16 *SIFTMap ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY ); //not imp yet
    INT32 Corners_SURF(UINT16 *SURFMap ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY ); //not imp yet

};

#ifdef __cplusplus
    }
#endif

#endif // END #ifndef _CornerDetector_H_

