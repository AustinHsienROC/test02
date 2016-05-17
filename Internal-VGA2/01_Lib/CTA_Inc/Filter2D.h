/*
===================================================================
Name		: Filter2D.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement Filter2D. & Extract_Y_From_YCbYCr
              -Smooth:                      Gaussian 
              -Background subtraction:      FrameDiff
              -Morphology:                  Dilation / Erosion
              -1's Differential Equations:  Sobel / 
              -2's Differential Equations:  Laplacian / Marr_Hildreth / CannyEdge
              -Corners:                     Harris
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/29	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/
//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifndef _FILTER2D_H_
#define _FILTER2D_H_

#include "CTA_Type.h"
#include "CTA_Def.h"
#ifdef _WIN32
  #include "OSD_Ycc422.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

#define ImgFilter Filter2D::getInstance()
//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
        
class Filter2D
{
private:
    static Filter2D *instance;

public:
    Filter2D(void);
    ~Filter2D(void);

    static Filter2D *getInstance();
    static void ResetInstance();
    
#ifdef _WIN32
    void Apex_Process_0_Gaussian(unsigned char* Gaussian_img, unsigned char *src_Y , int imgDimensionX, int imgDimensionY);
    void Apex_Process_1_0_SobelErosion(unsigned char* Sobel_img, unsigned char* vSobel_img, unsigned char* hSobel_img, unsigned char* Gaussian_img ,int imgDimensionX, int imgDimensionY);

    void FLT_GAUSSIAN_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height); //2014/7/6
    void FLT_GAUSSIAN_3x3_sub(unsigned char *dst, unsigned char *src, int img_width, int img_height);
    void FLT_SOBEL_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height, int contrast_numerator, int contrast_denominator, int &edgeCounter);
    void FLT_SOBEL_3x3_H(unsigned char *dst, unsigned char *src, int img_width, int img_height, int contrast_numerator, int contrast_denominator, int &edgeCounter);
    void FLT_SOBEL_3x3_BDIR(unsigned char *dst, unsigned char *src, unsigned char *tag, int img_width, int img_height, unsigned short *contrast_numerator, unsigned short *contrast_denominator, int &edgeCounter);
    void FLT_SOBEL_3x3_REGION(unsigned char *dst, unsigned char* vSobel_img, unsigned char* hSobel_img, unsigned char *src, int img_width, int img_height);
    void FLT_Laplacian(unsigned char *dst, unsigned char *src, int img_width, int img_height);
    void FLT_Marr_Hildreth(unsigned char *dst, unsigned char *src, int img_width, int img_height);
    void FLT_EROSION_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height, unsigned int coef);
    void FLT_EROSION_5(UCHAR *src, INT32 img_width, INT32 img_height);
    void FLT_DILATION_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height);
    void FLT_EdgeAdjustment(unsigned char *dst, int img_width, int img_height, int &edgeCounter);

    void FLT_ABSDiff(UCHAR *dst, UCHAR *src1 , UCHAR *src2, INT32 Thd ,int img_width, int img_height);
    void FLT_SobelMap( INT16 * vSobelMap, INT16* hSobelMap, UCHAR *src,INT32 img_width, INT32 img_height );    
    void CalculateHarrisValue(UINT16 *o_harrisValue, UCHAR *i_h_value, UCHAR *i_v_value, INT32 DimensionX, INT32 DimensionY);    
    void SupressNonMaxCornerRemoveSimilar(UCHAR *o_validFP, UINT16 *i_harris_value, UCHAR *Gaussian, INT32 DimensionX, INT32 DimensionY, const int TH, const UCHAR e_value);

    void SupressNonMaxRemoveStill(UCHAR *o_validFP, UINT16 *i_harris_value, UCHAR *Gaussian, UCHAR *DifBin, INT32 DimensionX, INT32 DimensionY, const int TH, const UCHAR e_value);


    void CntQty16x16(UINT16 *BlockCount, UCHAR *DifBin, int img_width, int img_height, 
                     UINT16 start_x, UINT16 start_y, UINT16 end_x, UINT16 end_y, unsigned char threshold);

    //-------------Night_Mode--------------//
    void ImageThreshold_01(UCHAR *g_bin_img, CONST UCHAR *Gaussian_img, INT32 imgDimensionX, INT32 imgDimensionY, UCHAR THD_Binary );
    
    //============ImageFilterFamily===================
    void HistogramEq(UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY );
    void SobelEdge(UCHAR *Sobel_Magnitude, CHAR* Sobel_Theta, UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY);
    void CannyEdge(UCHAR *Canny_img , UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY); //http://140.115.11.235/~chen/course/vision/ch6/ch6.htm
#endif // #ifdef _WIN32
};

#ifdef __cplusplus
    }
#endif

#endif // END #ifndef _FILTER2D_H_

