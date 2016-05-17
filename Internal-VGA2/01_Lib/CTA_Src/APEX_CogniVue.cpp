//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
#   include "stdafx.h"
#endif

#include "APEX_CogniVue.h"
//******************************************************************************
// I N T E R N A L   V A R I A B L E S
//******************************************************************************

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

/**
* @brief     3x3 Gaussian 2D IMG Filter
* @param[out] dst	        Output Gaussian image
* @param[in]  src	        Input image
* @param[in] imgDimensionX	Input FrameWidth
* @param[in] imgDimensionY	Input FrameHeight
*/
void Apex_Gaussian( UCHAR *dst, UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY )
{
    //@TODO: 
    INT32 starty = RegionL[1];
    INT32 endy   = RegionL[3];

}
/**
* @brief     Get unsigned char Sobel_img = |vSobel_img|+|vSobel_img| from vSobel_img and hSobel_img by using mask
             of 3x3 Sobel mask.
* @param[in] src	        Input Gaussian image
* @param[in] imgDimensionX	Input FrameWidth
* @param[in] imgDimensionY	Input FrameHeight
*/
void Apex_SobelErosion( UCHAR *dst, UCHAR* vSobel_img, UCHAR* hSobel_img, UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY )
{
    //@TODO: 
    INT32 starty = RegionL[1];
    INT32 endy   = RegionL[3];
}

/**
* @brief     Get Harris corner
* @param[out] o_harrisMap   Output harris corner map
* @param[in] i_h_value	        Input horizontal Sobel image
* @param[in] i_v_value	        Input vertical   Sobel image
*/
void Apex_HarrisValue( UINT16 *o_harrisMap, UCHAR *Sobel_img, UCHAR* i_h_value,UCHAR* i_v_value, UCHAR *src, INT32 DimensionX, INT32 DimensionY, INT32 TH )
{
    //@TODO: 
    INT32 starty = RegionL[1]+1;
    INT32 endy   = RegionL[3];

}

/**
* @brief     Get Frame Difference
* @param[out] dst   Output Frame Difference( SAT8(ABS(src1-src2)) )
* @param[in] src1	Input img1
* @param[in] src2   Input img2
*/
void Apex_ABSDiff( UCHAR *dst, UCHAR *src1 , UCHAR *src2, INT32 Thd ,int img_width, int img_height )
{
    //@TODO: 
    INT32 starty = RegionL[1];
    INT32 endy   = RegionL[3];

}

/**
* @brief      Get threshold image 
* @param[out] g_bin_img     Output Binary image
* @param[in]  Gaussian_img	Input img
*/
void Apex_Threshold( UCHAR *g_bin_img, CONST UCHAR *Gaussian_img, INT32 imgDimensionX, INT32 imgDimensionY, UCHAR THD_Binary )
{
    //@TODO: 
    INT32 starty = RegionL[1];
    INT32 endy   = RegionL[3];

}


#ifndef _WIN32  //k2
/**
* @brief  Memory heap allocation
*/
void* CTA_MemMgr_Allocate( UINT32 a_unBufSize )
{
    return oTo_MemMgr_Allocate( a_unBufSize ) ;
}
/**
* @brief  Memory heap release
*/
void CTA_MemMgr_Release(void* a_pBuf)
{
    return oTo_MemMgr_Release(a_pBuf) ;
}


#endif
