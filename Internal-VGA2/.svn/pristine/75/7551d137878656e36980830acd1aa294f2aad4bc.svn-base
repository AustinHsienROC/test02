//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
#include "stdafx.h"
#include "CornerDetector.h"

#include <string.h> //memcpy
#include <stdlib.h> //realloc

#if (defined(_WIN32) )
    #include <stdio.h> 
    #include <math.h>       /* atan */
#endif
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
//--------susan corners----------
typedef  unsigned char uchar;
#define MAX_SUSANCORNERS   15000  /* max corners per frame */
#define FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) ) /*SUSAN corners*/
typedef  struct {int x,y,info, dx, dy, I;} CORNER_LIST[MAX_SUSANCORNERS];
//-------fast corenr-------------
typedef struct { int x, y; } xy; 
typedef unsigned char byte;

CornerDetector* CornerDetector::instance = NULL;

//******************************************************************************
// I N T R E R N A L   F U N C T I O N   D E C L A R A T I O N
//******************************************************************************
static void setup_brightness_lut(uchar **bp ,int thresh, int form);
static void susan_corners(uchar *in, int *r, uchar *bp, int max_no,CORNER_LIST corner_list , int x_size, int y_size);
static void susan_corners_quick(uchar *in, int *r, uchar *bp, int max_no, CORNER_LIST corner_list, int x_size, int y_size);

//******************************************************************************
// I N T R E R N A L   F U N C T I O N   B O D Y S
//******************************************************************************

/**
* @brief	  將data中完成 non-maximun supression in 3x3 window
* @param[out] data		    [corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @return     the total corner Number
*/
static INT32 NON_MAX_SUPRESSION(  INT32 startX, INT32 startY , INT32 endX, INT32 endY, UINT16 *data , INT32 imgDimensionX, INT32 imgDimensionY)
{
    ///1) non-maximum suppress [make the real corners as 0xFFFF]
    INT32 ret = 0;
    INT32  count1 = 0 ;
    INT32  offsetV    ;
    INT32  offsetVup  ;
    INT32  offsetVbot ;
    UINT16 tmp1;
    for ( INT32 vert = startY ; vert < endY ; ++vert)
    {
        offsetV      =    vert * imgDimensionX;
        offsetVup    = offsetV - imgDimensionX;
        offsetVbot   = offsetV + imgDimensionX;

        for ( INT32 hori = startX ; hori< endX ; ++hori)
        {
            if( 0x0 < data[ hori + offsetV ])
            {
                tmp1 = data[ hori + offsetV ];
                if( (tmp1 > data[ (hori-1) + offsetV-imgDimensionX ]) &&
                    (tmp1 > data[ (hori) + offsetV-imgDimensionX ]) &&
                    (tmp1 > data[ (hori+1) + offsetV-imgDimensionX ]) &&
                    (tmp1 > data[ (hori-1) + offsetV ]) &&
                    (tmp1 > data[ (hori+1) + offsetV ]) &&
                    (tmp1 > data[ (hori-1) + offsetV+imgDimensionX ]) &&
                    (tmp1 > data[ (hori) + offsetV+imgDimensionX ]) &&
                    (tmp1 > data[ (hori+1) + offsetV+imgDimensionX ]))
                {
                    data[ hori + offsetV ] = 0xFFFF; //means corner
                    ++ret;
                }
                else
                {
                    /// means filtered corners
                }
            }
        }// end of [hori<(imgDimensionX-1)]
    }// end of [vert<(imgDimensionY-1)]
    return ret ; // ex. 3000 means 3000 corners
}
//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************


/**
* @brief	  Calculate Harris corner saved into HarrisMap[imgDimensionX*imgDimensionY](verified)
* @param[out] HarrisMap		    [corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]  SobelEdge_V		Sobel vertical edge.size:[imgDimensionX*imgDimensionY]
* @param[in]  Img			    Input original image.size:[imgDimensionX*imgDimensionY]
* @return     the corners Number
*/
INT32 CornerDetector::Corners_Harris( UINT16 *HarrisMap ,UCHAR *SobelEdge_V,UCHAR *SobelEdge_H ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    //**************************************
    const INT32 THD_harris    = 800;    //range: 0-65535;
    const double K            = 0.08;   //0.04~ 0.20;///< Harris K //0.1;//0.08;//1.0/128;//generally, 0.04~ 0.20;
    const int harris_width    = 3;      //calculate harris value of each block from harris_width*harris_height pixels 
    const int harris_height   = 3;      
    //**************************************

    INT32   M00, M10, M01, M11;
    INT32   Tr;
    INT32   result;
    INT32   dx,dy, i, j, y_offset;
    UCHAR  *src_h_idx, *src_v_idx ;
    UINT16 *opHarrisTmp;
    INT32   ret = 0;

    ///1) initialize
    memset(HarrisMap , 0 , imgDimensionX*imgDimensionY*sizeof(UINT16) );

    INT32 Idx_harrisCorners  = 0;
    INT32 starty             = 1;
    INT32 endy               = imgDimensionY -1;
    INT32 startx             = 1;
    INT32 endx               = imgDimensionX -1;
    INT32 half_harris_width  = harris_width/2; 
    INT32 half_harris_height = harris_height/2;

    ///2) Harris corner finder and set HarrisMap
    ///   HarrisMap = 0 if  < THD_harris
    INT32 offset01;
    for( dy = starty ; dy < endy ; ++dy )
    {
        offset01    = dy * imgDimensionX + startx + 1;
        src_h_idx   = SobelEdge_H + offset01;
        src_v_idx   = SobelEdge_V + offset01;
        opHarrisTmp	= HarrisMap   + offset01;

        for( dx = startx; dx < endx ; ++dx )
        {
            M00 = 0, M01 = 0 , M10 = 0, M11 = 0;

            /// get the M matrix near each pixel
            for( j = -half_harris_height ; j < half_harris_height ; ++j)
            {
                y_offset = j * imgDimensionX;
                for( i= -half_harris_width; i < half_harris_width ; ++i)
                {
                    M00 += (INT32)(*(src_h_idx+y_offset+i))*(*(src_h_idx+y_offset+i));
                    M01 += (INT32)(*(src_h_idx+y_offset+i))*(*(src_v_idx+y_offset+i));
                    M11 += (INT32)(*(src_v_idx+y_offset+i))*(*(src_v_idx+y_offset+i));	
                }
            }
            M10    = M01;
            Tr     = M00 + M11 ;

            result       = (INT32)( M00 * M11 - M01 * M10 - K * Tr * Tr ); ///< det(M)-K(trace(M))^2
            *opHarrisTmp = ((result>>16) > THD_harris)? ( (UINT16)SAT16( result>>16 )) : 0;

            ++src_h_idx;
            ++src_v_idx;
            ++opHarrisTmp;
        }
    }// end of [dy < ROI->start_row + ROI->height-1]

    ///3) non-Maximun suppress [make the real corners as 0xFFFF]
    ret = NON_MAX_SUPRESSION(1,1, imgDimensionX-1 , imgDimensionY-1 ,HarrisMap,imgDimensionX,imgDimensionY );
    return ret; //ex. 3000 means 3000 corners
}

/**
* @brief	  Calculate Harris corner saved into HarrisMap[imgDimensionX*imgDimensionY](verified)
* @param[out] HarrisMap		    [corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]  SobelEdge_V		Sobel vertical edge.size:[imgDimensionX*imgDimensionY]
* @param[in]  Img			    Input original image.size:[imgDimensionX*imgDimensionY]
* @return     the corners Number
* @note       Noble運算元是一種改進的 Harris運算元 與 H arris 運算元相比, Noble運算元具有較高的特徵點檢測重複度和較強的 抑制誤檢能力.
* @n          因而能更有效地抑制最大回應值點周圍的偽角點. 與 Harris 運算元相比, Noble運算元具有較高的特徵點檢測重複度和較強的抑制誤檢能力. 
*/
INT32 CornerDetector::Corners_Noble( UINT16 *NobleMap ,UCHAR *SobelEdge_V,UCHAR *SobelEdge_H ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    //**************************************
    const INT32 THD_Noble    = 4000;    //range: 0-MAX(INT32);
    const int harris_width    = 3;      //calculate harris value of each block from harris_width*harris_height pixels 
    const int harris_height   = 3;      
    //**************************************

    INT32   M00, M10, M01, M11;
    INT32   Tr;
    INT32   result;
    INT32   dx,dy, i, j, y_offset;
    UCHAR  *src_h_idx, *src_v_idx ;
    UINT16 *opNobleTmp;
    INT32   ret = 0;

    ///1) initialize
    memset(NobleMap , 0 , imgDimensionX*imgDimensionY*sizeof(UINT16) );

    INT32 Idx_harrisCorners  = 0;
    INT32 starty             = 1;
    INT32 endy               = imgDimensionY -1;
    INT32 startx             = 1;
    INT32 endx               = imgDimensionX -1;
    INT32 half_harris_width  = harris_width/2; 
    INT32 half_harris_height = harris_height/2;

    ///2) Harris corner finder and set HarrisMap
    ///   HarrisMap = 0 if  < THD_harris
    INT32 offset01;
    for( dy = starty ; dy < endy ; ++dy )
    {
        offset01    = dy * imgDimensionX + startx + 1;
        src_h_idx   = SobelEdge_H + offset01;
        src_v_idx   = SobelEdge_V + offset01;
        opNobleTmp	= NobleMap   + offset01;

        for( dx = startx; dx < endx ; ++dx )
        {
            M00 = 0, M01 = 0 , M10 = 0, M11 = 0;

            /// get the M matrix near each pixel
            for( j = -half_harris_height ; j < half_harris_height ; ++j)
            {
                y_offset = j * imgDimensionX;
                for( i= -half_harris_width; i < half_harris_width ; ++i)
                {
                    M00 += (INT32)(*(src_h_idx+y_offset+i))*(*(src_h_idx+y_offset+i));
                    M01 += (INT32)(*(src_h_idx+y_offset+i))*(*(src_v_idx+y_offset+i));
                    M11 += (INT32)(*(src_v_idx+y_offset+i))*(*(src_v_idx+y_offset+i));	
                }
            }
            M10    = M01;
            Tr     = M00 + M11 ;
            result = (INT32)((M00 * M11 - M01 * M10)/FLOAT(Tr+0.001));
            *opNobleTmp = ((result) > THD_Noble)? ( (UINT16)SAT16( result )) : 0;

            ++src_h_idx;
            ++src_v_idx;
            ++opNobleTmp;
        }
    }// end of [dy < ROI->start_row + ROI->height-1]

    ///3) non-Maximun suppress [make the real corners as 0xFFFF]
    ret = NON_MAX_SUPRESSION(1,1, imgDimensionX-1 , imgDimensionY-1 ,NobleMap,imgDimensionX,imgDimensionY );
    return ret; //ex. 3000 means 3000 corners
}

/**
* @brief	  Calculate ShiTomasi corner saved into ShiTomasiMap[imgDimensionX*imgDimensionY](verified)
* @param[out] ShiTomasiMap	    [corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]  SobelEdge_V		Sobel vertical edge.size:[imgDimensionX*imgDimensionY]
* @param[in]  Img			    Input original image.size:[imgDimensionX*imgDimensionY]
* @return     the corners Number
* @note       Shi和Tomasi的方法比較充分，並且在很多情況下可以得到比使用Harris算法更好的結果
* @ref        http://docs.opencv.org/trunk/doc/py_tutorials/py_feature2d/py_shi_tomasi/py_shi_tomasi.html
*/
INT32 CornerDetector::Corners_ShiTomasi( UINT16 *ShiTomasiMap ,UCHAR *SobelEdge_V,UCHAR *SobelEdge_H ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    //**************************************
    const FLOAT THD_ShiTomasi = 100000;    //range: TBD
    const int harris_width    = 3;      //calculate harris value of each block from harris_width*harris_height pixels 
    const int harris_height   = 3;      
    //**************************************

    INT32   M00, M10, M01, M11;
//    INT32   Tr;
    FLOAT   eigValue1,eigValue2;
    FLOAT   tmp1;
    FLOAT   result;
    INT32   dx,dy, i, j, y_offset;
    UCHAR  *src_h_idx, *src_v_idx ;
    UINT16 *opShiTmp;
    INT32   ret = 0;

    ///1) initialize
    memset( ShiTomasiMap , 0 , imgDimensionX*imgDimensionY*sizeof(UINT16) );

    INT32 Idx_harrisCorners  = 0;
    INT32 starty             = 1;
    INT32 endy               = imgDimensionY -1;
    INT32 startx             = 1;
    INT32 endx               = imgDimensionX -1;
    INT32 half_harris_width  = harris_width/2; 
    INT32 half_harris_height = harris_height/2;

    ///2) Harris corner finder and set HarrisMap
    ///   HarrisMap = 0 if  < THD_harris
    INT32 offset01;
    for( dy = starty ; dy < endy ; ++dy )
    {
        offset01    = dy * imgDimensionX + startx + 1;
        src_h_idx   = SobelEdge_H + offset01;
        src_v_idx   = SobelEdge_V + offset01;
        opShiTmp	= ShiTomasiMap+ offset01;

        for( dx = startx; dx < endx ; ++dx )
        {
            M00 = 0, M01 = 0 , M10 = 0, M11 = 0;

            /// get the M matrix near each pixel
            for( j = -half_harris_height ; j < half_harris_height ; ++j)
            {
                y_offset = j * imgDimensionX;
                for( i= -half_harris_width; i < half_harris_width ; ++i)
                {
                    M00 += (INT32)(*(src_h_idx+y_offset+i))*(*(src_h_idx+y_offset+i));
                    M01 += (INT32)(*(src_h_idx+y_offset+i))*(*(src_v_idx+y_offset+i));
                    M11 += (INT32)(*(src_v_idx+y_offset+i))*(*(src_v_idx+y_offset+i));	
                }
            }
            M10    = M01;
            tmp1 = sqrt(DOUBLE((M00+M11)*(M00+M11) - 4*(M00*M11-M10*M01)));
            eigValue1 = ((M00+M11)+ tmp1 )/ FLOAT(2);
            eigValue2 = ((M00+M11)- tmp1 )/ FLOAT(2);
            result = MIN(eigValue1,eigValue2);

            *opShiTmp = ((result) > THD_ShiTomasi)? ( (UINT16)SAT16( result )) : 0;

            ++src_h_idx;
            ++src_v_idx;
            ++opShiTmp;
        }
    }// end of [dy < ROI->start_row + ROI->height-1]

    ///3) non-Maximun suppress [make the real corners as 0xFFFF]
    ret = NON_MAX_SUPRESSION(1,1, imgDimensionX-1 , imgDimensionY-1 ,ShiTomasiMap,imgDimensionX,imgDimensionY );
    return ret; //ex. 3000 means 3000 corners
}

/////////////////////////////FAST Corner v1.2/////////////////////

xy* fast_corner_detect_10(const byte* im, int xsize, int ysize, int barrier, int* num)				
{																								
    int boundary = 3, y, cb, c_b;																
    const byte  *line_max, *line_min;															
    int			rsize=512, total=0;																
    xy	 		*ret = (xy*)malloc(rsize*sizeof(xy));											
    const byte* cache_0;
    const byte* cache_1;
    const byte* cache_2;
    int	pixel[16];																				
    pixel[0] = 0 + 3 * xsize;		
    pixel[1] = 1 + 3 * xsize;		
    pixel[2] = 2 + 2 * xsize;		
    pixel[3] = 3 + 1 * xsize;		
    pixel[4] = 3 + 0 * xsize;		
    pixel[5] = 3 + -1 * xsize;		
    pixel[6] = 2 + -2 * xsize;		
    pixel[7] = 1 + -3 * xsize;		
    pixel[8] = 0 + -3 * xsize;		
    pixel[9] = -1 + -3 * xsize;		
    pixel[10] = -2 + -2 * xsize;		
    pixel[11] = -3 + -1 * xsize;		
    pixel[12] = -3 + 0 * xsize;		
    pixel[13] = -3 + 1 * xsize;		
    pixel[14] = -2 + 2 * xsize;		
    pixel[15] = -1 + 3 * xsize;		
    for(y = boundary ; y < ysize - boundary; y++)												
    {																							
        cache_0 = im + boundary + y*xsize;														
        line_min = cache_0 - boundary;															
        line_max = im + xsize - boundary + y * xsize;											

        cache_1 = cache_0 + pixel[9];
        cache_2 = cache_0 + pixel[3];

        for(; cache_0 < line_max;cache_0++, cache_1++, cache_2++)
        {																						
            cb = *cache_0 + barrier;															
            c_b = *cache_0 - barrier;															
            if(*cache_1 > cb)
                if(*(cache_0 + pixel[2]) > cb)
                    if(*(cache_0+3) > cb)
                        if(*(cache_0 + pixel[6]) > cb)
                            if(*(cache_0 + pixel[0]) > cb)
                                if(*cache_2 > cb)
                                    if(*(cache_0 + pixel[5]) > cb)
                                        if(*(cache_1+2) > cb)
                                            if(*(cache_0 + pixel[1]) > cb)
                                                if(*(cache_1+1) > cb)
                                                    goto success;
                                                else if(*(cache_1+1) < c_b)
                                                    continue;
                                                else
                                                    if(*(cache_0 + pixel[14]) > cb)
                                                        if(*(cache_0 + pixel[15]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else if(*(cache_0 + pixel[1]) < c_b)
                                                continue;
                                            else
                                                if(*(cache_0 + pixel[11]) > cb)
                                                    if(*(cache_1+1) > cb)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else if(*(cache_1+2) < c_b)
                                            continue;
                                        else
                                            if(*(cache_0 + pixel[14]) > cb)
                                                if(*(cache_2+-6) > cb)
                                                    if(*(cache_0 + pixel[15]) > cb)
                                                        if(*(cache_0 + pixel[1]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else if(*(cache_0 + pixel[5]) < c_b)
                                        continue;
                                    else
                                        if(*(cache_0+-3) > cb)
                                            if(*(cache_2+-6) > cb)
                                                if(*(cache_0 + pixel[11]) > cb)
                                                    if(*(cache_0 + pixel[14]) > cb)
                                                        if(*(cache_0 + pixel[15]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else if(*cache_2 < c_b)
                                    continue;
                                else
                                    if(*(cache_0+-3) > cb)
                                        if(*(cache_0 + pixel[11]) > cb)
                                            if(*(cache_2+-6) > cb)
                                                if(*(cache_0 + pixel[10]) > cb)
                                                    if(*(cache_1+2) > cb)
                                                        goto success;
                                                    else if(*(cache_1+2) < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_0 + pixel[15]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else if(*(cache_0 + pixel[0]) < c_b)
                                if(*(cache_0 + pixel[11]) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_1+2) > cb)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                if(*(cache_0 + pixel[10]) > cb)
                                    if(*(cache_1+2) > cb)
                                        if(*(cache_0 + pixel[11]) > cb)
                                            if(*(cache_0 + pixel[5]) > cb)
                                                if(*(cache_1+1) > cb)
                                                    if(*cache_2 > cb)
                                                        goto success;
                                                    else if(*cache_2 < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_2+-6) > cb)
                                                            if(*(cache_0+-3) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(*(cache_0 + pixel[11]) < c_b)
                                            if(*(cache_0 + pixel[1]) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            if(*(cache_0 + pixel[1]) > cb)
                                                if(*(cache_0 + pixel[5]) > cb)
                                                    if(*cache_2 > cb)
                                                        if(*(cache_1+1) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[6]) < c_b)
                            continue;
                        else
                            if(*(cache_0 + pixel[15]) > cb)
                                if(*(cache_0+-3) > cb)
                                    if(*(cache_0 + pixel[14]) > cb)
                                        if(*(cache_0 + pixel[0]) > cb)
                                            if(*(cache_0 + pixel[11]) > cb)
                                                if(*(cache_2+-6) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            goto success;
                                                        else if(*(cache_0 + pixel[10]) < c_b)
                                                            continue;
                                                        else
                                                            if(*cache_2 > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if(*(cache_0 + pixel[11]) < c_b)
                                                continue;
                                            else
                                                if(*(cache_0 + pixel[5]) > cb)
                                                    if(*cache_2 > cb)
                                                        if(*(cache_2+-6) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else if(*(cache_0+3) < c_b)
                        if(*(cache_0 + pixel[11]) > cb)
                            if(*(cache_2+-6) > cb)
                                if(*(cache_0 + pixel[0]) > cb)
                                    if(*(cache_0 + pixel[10]) > cb)
                                        if(*(cache_0+-3) > cb)
                                            if(*(cache_0 + pixel[15]) > cb)
                                                if(*(cache_0 + pixel[14]) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        goto success;
                                                    else if(*(cache_0 + pixel[1]) < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_0 + pixel[6]) < c_b)
                                                            continue;
                                                        else
                                                            goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*(cache_0 + pixel[0]) < c_b)
                                    continue;
                                else
                                    if(*(cache_0 + pixel[6]) > cb)
                                        goto success;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        if(*(cache_0+-3) > cb)
                            if(*(cache_0 + pixel[11]) > cb)
                                if(*(cache_0 + pixel[14]) > cb)
                                    if(*(cache_0 + pixel[15]) > cb)
                                        if(*(cache_2+-6) > cb)
                                            if(*(cache_0 + pixel[10]) > cb)
                                                if(*(cache_0 + pixel[0]) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        goto success;
                                                    else if(*(cache_0 + pixel[1]) < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_1+2) > cb)
                                                            if(*(cache_1+1) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else if(*(cache_0 + pixel[0]) < c_b)
                                                    continue;
                                                else
                                                    if(*(cache_0 + pixel[6]) > cb)
                                                        if(*(cache_1+2) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(*(cache_0 + pixel[15]) < c_b)
                                        continue;
                                    else
                                        if(*(cache_0 + pixel[5]) > cb)
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_1+2) > cb)
                                                    if(*(cache_2+-6) > cb)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            if(*(cache_1+1) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                else if(*(cache_0 + pixel[2]) < c_b)
                    if(*(cache_0 + pixel[14]) > cb)
                        if(*(cache_0 + pixel[6]) > cb)
                            if(*(cache_0 + pixel[5]) > cb)
                                if(*(cache_0 + pixel[11]) > cb)
                                    if(*(cache_1+2) > cb)
                                        if(*(cache_0+-3) > cb)
                                            if(*(cache_0 + pixel[10]) > cb)
                                                if(*(cache_1+1) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_0 + pixel[5]) < c_b)
                                if(*(cache_0 + pixel[15]) > cb)
                                    if(*(cache_0+-3) > cb)
                                        if(*(cache_0 + pixel[11]) > cb)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                if(*(cache_0 + pixel[15]) > cb)
                                    if(*(cache_0+-3) > cb)
                                        if(*(cache_0 + pixel[11]) > cb)
                                            if(*(cache_0 + pixel[10]) > cb)
                                                if(*(cache_1+2) > cb)
                                                    if(*(cache_2+-6) > cb)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[6]) < c_b)
                            if(*(cache_0 + pixel[1]) > cb)
                                if(*(cache_1+1) > cb)
                                    if(*(cache_0+-3) > cb)
                                        goto success;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_0 + pixel[1]) < c_b)
                                continue;
                            else
                                if(*(cache_1+2) > cb)
                                    if(*(cache_0 + pixel[0]) > cb)
                                        if(*(cache_0+-3) > cb)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(*(cache_0 + pixel[0]) > cb)
                                if(*(cache_0+-3) > cb)
                                    if(*(cache_1+1) > cb)
                                        if(*(cache_0 + pixel[10]) > cb)
                                            if(*(cache_0 + pixel[11]) > cb)
                                                if(*(cache_0 + pixel[15]) > cb)
                                                    if(*(cache_1+2) > cb)
                                                        if(*(cache_2+-6) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else if(*(cache_1+2) < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_0 + pixel[1]) > cb)
                                                            if(*(cache_2+-6) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else if(*(cache_0 + pixel[14]) < c_b)
                        if(*(cache_0 + pixel[5]) > cb)
                            if(*(cache_0 + pixel[10]) > cb)
                                if(*cache_2 > cb)
                                    if(*(cache_0+-3) > cb)
                                        if(*(cache_0 + pixel[6]) > cb)
                                            if(*(cache_0 + pixel[11]) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*cache_2 < c_b)
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        if(*(cache_0+3) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_0 + pixel[10]) < c_b)
                                if(*cache_2 < c_b)
                                    goto success;
                                else
                                    continue;
                            else
                                if(*(cache_0+3) < c_b)
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        goto success;
                                    else
                                        continue;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[5]) < c_b)
                            if(*(cache_2+-6) > cb)
                                if(*(cache_1+2) < c_b)
                                    if(*(cache_0+3) < c_b)
                                        goto success;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_2+-6) < c_b)
                                if(*(cache_0 + pixel[6]) > cb)
                                    if(*(cache_0+-3) < c_b)
                                        if(*(cache_0 + pixel[15]) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*(cache_0 + pixel[6]) < c_b)
                                    if(*(cache_0 + pixel[15]) < c_b)
                                        if(*cache_2 < c_b)
                                            if(*(cache_0 + pixel[1]) < c_b)
                                                if(*(cache_0+3) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    if(*(cache_0+-3) < c_b)
                                        if(*(cache_0+3) < c_b)
                                            if(*(cache_0 + pixel[15]) < c_b)
                                                if(*cache_2 < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(*(cache_1+2) < c_b)
                                    if(*(cache_0 + pixel[15]) < c_b)
                                        if(*(cache_0 + pixel[6]) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(*(cache_0 + pixel[11]) < c_b)
                                if(*(cache_0+3) < c_b)
                                    if(*(cache_0+-3) < c_b)
                                        goto success;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else
                        if(*(cache_0+3) > cb)
                            if(*(cache_2+-6) > cb)
                                if(*(cache_1+1) > cb)
                                    if(*(cache_0 + pixel[15]) < c_b)
                                        if(*(cache_1+2) > cb)
                                            if(*(cache_0 + pixel[10]) > cb)
                                                if(*(cache_0+-3) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        goto success;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                else
                    if(*(cache_0+-3) > cb)
                        if(*(cache_0 + pixel[15]) > cb)
                            if(*(cache_1+1) > cb)
                                if(*(cache_0 + pixel[14]) > cb)
                                    if(*(cache_0 + pixel[11]) > cb)
                                        if(*(cache_0 + pixel[0]) > cb)
                                            if(*(cache_2+-6) > cb)
                                                if(*(cache_0 + pixel[10]) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        goto success;
                                                    else if(*(cache_0 + pixel[1]) < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_1+2) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else if(*(cache_2+-6) < c_b)
                                                continue;
                                            else
                                                if(*cache_2 > cb)
                                                    if(*(cache_0 + pixel[5]) > cb)
                                                        if(*(cache_0 + pixel[6]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else if(*(cache_0 + pixel[0]) < c_b)
                                            continue;
                                        else
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_1+2) > cb)
                                                    if(*(cache_0 + pixel[10]) > cb)
                                                        if(*(cache_2+-6) > cb)
                                                            goto success;
                                                        else if(*(cache_2+-6) < c_b)
                                                            continue;
                                                        else
                                                            if(*cache_2 > cb)
                                                                if(*(cache_0+3) > cb)
                                                                    goto success;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else if(*(cache_0 + pixel[14]) < c_b)
                                    continue;
                                else
                                    if(*(cache_0+3) > cb)
                                        if(*cache_2 > cb)
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_0 + pixel[11]) > cb)
                                                    if(*(cache_0 + pixel[5]) > cb)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            if(*(cache_1+2) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(*cache_2 < c_b)
                                            continue;
                                        else
                                            if(*(cache_2+-6) > cb)
                                                if(*(cache_0 + pixel[10]) > cb)
                                                    if(*(cache_0 + pixel[6]) > cb)
                                                        if(*(cache_0 + pixel[5]) > cb)
                                                            if(*(cache_0 + pixel[11]) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                            else
                                continue;
                        else if(*(cache_0 + pixel[15]) < c_b)
                            if(*(cache_0+3) > cb)
                                if(*(cache_1+2) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_0 + pixel[10]) > cb)
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_0 + pixel[5]) > cb)
                                                    if(*(cache_1+1) > cb)
                                                        if(*(cache_0 + pixel[11]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(*cache_2 < c_b)
                                        continue;
                                    else
                                        if(*(cache_2+-6) > cb)
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_0 + pixel[10]) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else if(*(cache_0+3) < c_b)
                                continue;
                            else
                                if(*(cache_0 + pixel[14]) > cb)
                                    if(*(cache_0 + pixel[5]) > cb)
                                        if(*(cache_0 + pixel[0]) > cb || *(cache_0 + pixel[0]) < c_b)
                                            continue;
                                        else
                                            goto success;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(*(cache_0 + pixel[5]) > cb)
                                if(*(cache_1+2) > cb)
                                    if(*(cache_0 + pixel[14]) > cb)
                                        if(*(cache_0 + pixel[11]) > cb)
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_1+1) > cb)
                                                    if(*(cache_0 + pixel[10]) > cb)
                                                        if(*(cache_2+-6) > cb)
                                                            goto success;
                                                        else if(*(cache_2+-6) < c_b)
                                                            continue;
                                                        else
                                                            if(*cache_2 > cb)
                                                                if(*(cache_0+3) > cb)
                                                                    goto success;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(*(cache_0 + pixel[14]) < c_b)
                                        if(*(cache_0+3) > cb)
                                            if(*cache_2 > cb)
                                                goto success;
                                            else if(*cache_2 < c_b)
                                                continue;
                                            else
                                                if(*(cache_2+-6) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        if(*(cache_0+3) > cb)
                                            if(*(cache_0 + pixel[6]) > cb)
                                                if(*(cache_2+-6) > cb)
                                                    if(*(cache_1+1) > cb)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            if(*(cache_0 + pixel[11]) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if(*(cache_2+-6) < c_b)
                                                    continue;
                                                else
                                                    if(*cache_2 > cb)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            if(*(cache_1+1) > cb)
                                                                if(*(cache_0 + pixel[11]) > cb)
                                                                    goto success;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                    else
                        continue;
            else if(*cache_1 < c_b)
                if(*(cache_0 + pixel[1]) > cb)
                    if(*(cache_0 + pixel[6]) > cb)
                        if(*(cache_2+-6) > cb)
                            if(*(cache_0+3) > cb)
                                if(*(cache_0 + pixel[15]) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_0 + pixel[0]) > cb)
                                            if(*(cache_0 + pixel[5]) > cb)
                                                if(*(cache_0 + pixel[2]) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if(*(cache_2+-6) < c_b)
                            if(*(cache_0 + pixel[14]) > cb)
                                if(*(cache_1+2) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_0 + pixel[2]) > cb)
                                            if(*(cache_0+3) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            if(*(cache_1+2) > cb)
                                if(*(cache_0 + pixel[14]) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_0+3) > cb)
                                            if(*(cache_0 + pixel[15]) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else if(*(cache_0 + pixel[6]) < c_b)
                        if(*(cache_0 + pixel[14]) > cb)
                            if(*(cache_0+3) > cb)
                                if(*(cache_0+-3) > cb)
                                    if(*(cache_0 + pixel[11]) > cb)
                                        if(*(cache_0 + pixel[2]) > cb)
                                            if(*cache_2 > cb)
                                                if(*(cache_2+-6) > cb)
                                                    if(*(cache_0 + pixel[15]) > cb)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        if(*(cache_0 + pixel[5]) > cb)
                                            goto success;
                                        else
                                            continue;
                                else
                                    continue;
                            else if(*(cache_0+3) < c_b)
                                if(*(cache_0 + pixel[2]) > cb)
                                    continue;
                                else if(*(cache_0 + pixel[2]) < c_b)
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        if(*(cache_0+-3) > cb)
                                            continue;
                                        else if(*(cache_0+-3) < c_b)
                                            if(*(cache_0 + pixel[5]) < c_b)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            goto success;
                                    else
                                        continue;
                                else
                                    if(*(cache_0+-3) < c_b)
                                        if(*(cache_2+-6) > cb)
                                            if(*cache_2 < c_b)
                                                if(*(cache_0 + pixel[10]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(*(cache_2+-6) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(*(cache_0 + pixel[10]) > cb)
                                    if(*cache_2 > cb)
                                        goto success;
                                    else
                                        continue;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[14]) < c_b)
                            if(*(cache_0 + pixel[5]) > cb)
                                if(*(cache_0 + pixel[15]) < c_b)
                                    if(*(cache_1+2) < c_b)
                                        goto success;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_0 + pixel[5]) < c_b)
                                if(*(cache_0+-3) < c_b)
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        if(*(cache_1+2) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                if(*(cache_0 + pixel[15]) < c_b)
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        if(*(cache_1+2) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(*(cache_0+3) < c_b)
                                if(*(cache_0+-3) > cb)
                                    continue;
                                else if(*(cache_0+-3) < c_b)
                                    if(*(cache_2+-6) > cb)
                                        continue;
                                    else if(*(cache_2+-6) < c_b)
                                        if(*(cache_1+2) < c_b)
                                            if(*(cache_0 + pixel[10]) < c_b)
                                                if(*(cache_0 + pixel[5]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        if(*cache_2 < c_b)
                                            if(*(cache_1+2) < c_b)
                                                if(*(cache_1+1) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(*(cache_0 + pixel[2]) < c_b)
                                        if(*(cache_0 + pixel[11]) < c_b)
                                            if(*(cache_0 + pixel[5]) < c_b)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                    else
                        if(*(cache_0+-3) > cb)
                            if(*(cache_0+3) > cb)
                                if(*(cache_0 + pixel[5]) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_0 + pixel[15]) > cb)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*(cache_0 + pixel[5]) < c_b)
                                    continue;
                                else
                                    if(*(cache_1+2) > cb || *(cache_1+2) < c_b)
                                        continue;
                                    else
                                        if(*(cache_0 + pixel[11]) > cb)
                                            if(*(cache_0 + pixel[14]) > cb)
                                                if(*(cache_0 + pixel[0]) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                            else if(*(cache_0+3) < c_b)
                                continue;
                            else
                                if(*(cache_0 + pixel[10]) > cb)
                                    if(*cache_2 > cb)
                                        if(*(cache_2+-6) > cb)
                                            if(*(cache_0 + pixel[2]) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            continue;
                else if(*(cache_0 + pixel[1]) < c_b)
                    if(*(cache_0+3) > cb)
                        if(*(cache_0+-3) < c_b)
                            if(*(cache_0 + pixel[11]) < c_b)
                                if(*(cache_0 + pixel[14]) < c_b)
                                    if(*(cache_1+1) > cb)
                                        goto success;
                                    else if(*(cache_1+1) < c_b)
                                        if(*(cache_0 + pixel[10]) < c_b)
                                            if(*(cache_2+-6) < c_b)
                                                if(*(cache_0 + pixel[15]) > cb)
                                                    continue;
                                                else if(*(cache_0 + pixel[15]) < c_b)
                                                    if(*(cache_0 + pixel[0]) > cb)
                                                        continue;
                                                    else if(*(cache_0 + pixel[0]) < c_b)
                                                        goto success;
                                                    else
                                                        if(*(cache_0 + pixel[6]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    if(*(cache_0 + pixel[5]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        if(*(cache_0 + pixel[2]) < c_b)
                                            if(*(cache_0 + pixel[10]) < c_b)
                                                if(*(cache_0 + pixel[15]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if(*(cache_0+3) < c_b)
                        if(*(cache_0 + pixel[6]) > cb)
                            if(*(cache_0 + pixel[2]) > cb || *(cache_0 + pixel[2]) < c_b)
                                continue;
                            else
                                if(*(cache_1+1) < c_b)
                                    if(*(cache_1+2) > cb || *(cache_1+2) < c_b)
                                        continue;
                                    else
                                        goto success;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[6]) < c_b)
                            if(*cache_2 > cb)
                                continue;
                            else if(*cache_2 < c_b)
                                if(*(cache_0 + pixel[5]) > cb)
                                    continue;
                                else if(*(cache_0 + pixel[5]) < c_b)
                                    if(*(cache_0 + pixel[0]) > cb)
                                        continue;
                                    else if(*(cache_0 + pixel[0]) < c_b)
                                        if(*(cache_0 + pixel[2]) > cb)
                                            continue;
                                        else if(*(cache_0 + pixel[2]) < c_b)
                                            if(*(cache_1+2) > cb)
                                                continue;
                                            else if(*(cache_1+2) < c_b)
                                                if(*(cache_1+1) > cb)
                                                    continue;
                                                else if(*(cache_1+1) < c_b)
                                                    goto success;
                                                else
                                                    if(*(cache_0 + pixel[14]) < c_b)
                                                        if(*(cache_0 + pixel[15]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(*(cache_2+-6) < c_b)
                                                    if(*(cache_0 + pixel[14]) < c_b)
                                                        if(*(cache_0 + pixel[15]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(*(cache_0+-3) < c_b)
                                                if(*(cache_0 + pixel[11]) < c_b)
                                                    if(*(cache_1+2) < c_b)
                                                        if(*(cache_0 + pixel[10]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(*(cache_0 + pixel[10]) < c_b)
                                            if(*(cache_1+2) < c_b)
                                                if(*(cache_0 + pixel[2]) > cb)
                                                    continue;
                                                else if(*(cache_0 + pixel[2]) < c_b)
                                                    if(*(cache_1+1) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    if(*(cache_0+-3) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(*(cache_0+-3) < c_b)
                                        if(*(cache_0 + pixel[15]) < c_b)
                                            if(*(cache_0 + pixel[11]) < c_b)
                                                if(*(cache_0 + pixel[14]) < c_b)
                                                    if(*(cache_2+-6) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(*(cache_0+-3) < c_b)
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        if(*(cache_2+-6) < c_b)
                                            if(*(cache_0 + pixel[10]) < c_b)
                                                if(*(cache_1+1) < c_b)
                                                    if(*(cache_0 + pixel[5]) > cb)
                                                        continue;
                                                    else if(*(cache_0 + pixel[5]) < c_b)
                                                        goto success;
                                                    else
                                                        if(*(cache_0 + pixel[0]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(*(cache_0 + pixel[14]) < c_b)
                                if(*(cache_0+-3) < c_b)
                                    if(*(cache_0 + pixel[15]) < c_b)
                                        if(*(cache_2+-6) < c_b)
                                            if(*(cache_0 + pixel[11]) > cb)
                                                continue;
                                            else if(*(cache_0 + pixel[11]) < c_b)
                                                if(*(cache_0 + pixel[2]) > cb)
                                                    continue;
                                                else if(*(cache_0 + pixel[2]) < c_b)
                                                    if(*(cache_0 + pixel[0]) < c_b)
                                                        if(*(cache_0 + pixel[10]) > cb)
                                                            continue;
                                                        else if(*(cache_0 + pixel[10]) < c_b)
                                                            goto success;
                                                        else
                                                            if(*cache_2 < c_b)
                                                                goto success;
                                                            else
                                                                continue;
                                                    else
                                                        continue;
                                                else
                                                    if(*(cache_1+1) < c_b)
                                                        if(*(cache_0 + pixel[10]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(*(cache_0 + pixel[5]) < c_b)
                                                    if(*(cache_0 + pixel[0]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                    else
                        if(*(cache_0+-3) < c_b)
                            if(*(cache_0 + pixel[14]) < c_b)
                                if(*(cache_0 + pixel[11]) < c_b)
                                    if(*(cache_2+-6) < c_b)
                                        if(*(cache_0 + pixel[10]) < c_b)
                                            if(*(cache_1+1) > cb)
                                                continue;
                                            else if(*(cache_1+1) < c_b)
                                                if(*(cache_0 + pixel[15]) > cb)
                                                    continue;
                                                else if(*(cache_0 + pixel[15]) < c_b)
                                                    if(*(cache_0 + pixel[0]) > cb)
                                                        continue;
                                                    else if(*(cache_0 + pixel[0]) < c_b)
                                                        goto success;
                                                    else
                                                        if(*(cache_0 + pixel[6]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    if(*(cache_0 + pixel[5]) < c_b)
                                                        if(*(cache_1+2) < c_b)
                                                            if(*(cache_0 + pixel[6]) < c_b)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                            else
                                                if(*(cache_0 + pixel[2]) < c_b)
                                                    if(*(cache_0 + pixel[15]) < c_b)
                                                        if(*(cache_0 + pixel[0]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                else
                    if(*(cache_0 + pixel[11]) < c_b)
                        if(*(cache_0 + pixel[15]) > cb)
                            if(*cache_2 > cb)
                                continue;
                            else if(*cache_2 < c_b)
                                if(*(cache_0 + pixel[2]) > cb)
                                    continue;
                                else if(*(cache_0 + pixel[2]) < c_b)
                                    if(*(cache_0 + pixel[5]) < c_b)
                                        if(*(cache_0 + pixel[6]) < c_b)
                                            if(*(cache_0+3) < c_b)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    if(*(cache_0+-3) < c_b)
                                        if(*(cache_0 + pixel[6]) < c_b)
                                            if(*(cache_0+3) < c_b)
                                                if(*(cache_0 + pixel[5]) < c_b)
                                                    if(*(cache_1+2) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(*(cache_2+-6) < c_b)
                                    if(*(cache_0+3) > cb)
                                        continue;
                                    else if(*(cache_0+3) < c_b)
                                        if(*(cache_0 + pixel[6]) < c_b)
                                            if(*(cache_0 + pixel[2]) > cb || *(cache_0 + pixel[2]) < c_b)
                                                continue;
                                            else
                                                if(*(cache_0 + pixel[10]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        if(*(cache_0 + pixel[14]) < c_b)
                                            if(*(cache_0 + pixel[5]) < c_b)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[15]) < c_b)
                            if(*(cache_1+2) < c_b)
                                if(*(cache_0+-3) > cb)
                                    continue;
                                else if(*(cache_0+-3) < c_b)
                                    if(*(cache_0 + pixel[14]) > cb)
                                        continue;
                                    else if(*(cache_0 + pixel[14]) < c_b)
                                        if(*(cache_0 + pixel[6]) > cb)
                                            if(*(cache_0 + pixel[0]) < c_b)
                                                if(*(cache_1+1) < c_b)
                                                    if(*(cache_0 + pixel[10]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(*(cache_0 + pixel[6]) < c_b)
                                            if(*(cache_2+-6) > cb)
                                                continue;
                                            else if(*(cache_2+-6) < c_b)
                                                if(*(cache_0 + pixel[10]) < c_b)
                                                    if(*(cache_1+1) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                if(*cache_2 < c_b)
                                                    if(*(cache_0 + pixel[5]) < c_b)
                                                        if(*(cache_0+3) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            if(*(cache_0 + pixel[0]) < c_b)
                                                if(*(cache_0 + pixel[10]) < c_b)
                                                    if(*(cache_2+-6) < c_b)
                                                        if(*(cache_1+1) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(*(cache_0+3) < c_b)
                                            if(*(cache_2+-6) > cb)
                                                continue;
                                            else if(*(cache_2+-6) < c_b)
                                                if(*(cache_0 + pixel[6]) < c_b)
                                                    if(*(cache_0 + pixel[10]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                if(*cache_2 < c_b)
                                                    if(*(cache_0 + pixel[6]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                else
                                    if(*cache_2 < c_b)
                                        if(*(cache_0+3) < c_b)
                                            if(*(cache_0 + pixel[2]) < c_b)
                                                if(*(cache_0 + pixel[5]) < c_b)
                                                    if(*(cache_1+1) < c_b)
                                                        if(*(cache_0 + pixel[6]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else
                            if(*(cache_0+3) > cb)
                                if(*(cache_0 + pixel[5]) < c_b)
                                    if(*(cache_0 + pixel[14]) < c_b)
                                        if(*(cache_1+2) < c_b)
                                            if(*(cache_2+-6) < c_b)
                                                if(*(cache_0 + pixel[6]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_0+3) < c_b)
                                if(*cache_2 > cb)
                                    if(*(cache_2+-6) < c_b)
                                        if(*(cache_0+-3) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*cache_2 < c_b)
                                    if(*(cache_0 + pixel[6]) < c_b)
                                        if(*(cache_0 + pixel[2]) > cb)
                                            continue;
                                        else if(*(cache_0 + pixel[2]) < c_b)
                                            if(*(cache_1+2) < c_b)
                                                if(*(cache_0 + pixel[5]) < c_b)
                                                    if(*(cache_1+1) < c_b)
                                                        if(*(cache_0 + pixel[10]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            if(*(cache_0+-3) < c_b)
                                                if(*(cache_1+2) < c_b)
                                                    if(*(cache_0 + pixel[5]) < c_b)
                                                        if(*(cache_1+1) < c_b)
                                                            if(*(cache_0 + pixel[10]) < c_b)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                                else
                                    if(*(cache_2+-6) < c_b)
                                        if(*(cache_0 + pixel[6]) < c_b)
                                            if(*(cache_0+-3) < c_b)
                                                if(*(cache_1+2) < c_b)
                                                    if(*(cache_0 + pixel[10]) < c_b)
                                                        if(*(cache_0 + pixel[5]) < c_b)
                                                            if(*(cache_1+1) < c_b)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(*(cache_0 + pixel[14]) < c_b)
                                    if(*(cache_0 + pixel[5]) < c_b)
                                        if(*(cache_0+-3) < c_b)
                                            if(*(cache_1+2) < c_b)
                                                if(*(cache_2+-6) < c_b)
                                                    if(*(cache_0 + pixel[6]) < c_b)
                                                        if(*(cache_0 + pixel[10]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                    else
                        continue;
            else
                if(*cache_2 > cb)
                    if(*(cache_0 + pixel[15]) > cb)
                        if(*(cache_0 + pixel[6]) > cb)
                            if(*(cache_0+3) > cb)
                                if(*(cache_2+-6) > cb)
                                    if(*(cache_0 + pixel[2]) > cb)
                                        if(*(cache_0 + pixel[0]) > cb)
                                            if(*(cache_0 + pixel[5]) > cb)
                                                if(*(cache_0 + pixel[14]) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        goto success;
                                                    else
                                                        continue;
                                                else if(*(cache_0 + pixel[14]) < c_b)
                                                    continue;
                                                else
                                                    if(*(cache_1+1) > cb)
                                                        goto success;
                                                    else
                                                        continue;
                                            else if(*(cache_0 + pixel[5]) < c_b)
                                                continue;
                                            else
                                                if(*(cache_0 + pixel[11]) > cb)
                                                    if(*(cache_0+-3) > cb)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*(cache_2+-6) < c_b)
                                    if(*(cache_1+1) > cb)
                                        if(*(cache_0 + pixel[2]) > cb)
                                            if(*(cache_0 + pixel[5]) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(*(cache_1+1) < c_b)
                                        continue;
                                    else
                                        if(*(cache_0 + pixel[14]) > cb)
                                            if(*(cache_1+2) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(*(cache_1+2) > cb)
                                        if(*(cache_0 + pixel[14]) > cb)
                                            if(*(cache_0 + pixel[5]) > cb)
                                                if(*(cache_0 + pixel[2]) > cb)
                                                    if(*(cache_0 + pixel[0]) > cb)
                                                        if(*(cache_0 + pixel[1]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if(*(cache_0 + pixel[14]) < c_b)
                                            if(*(cache_1+1) > cb)
                                                goto success;
                                            else
                                                continue;
                                        else
                                            if(*(cache_1+1) > cb)
                                                if(*(cache_0 + pixel[2]) > cb)
                                                    if(*(cache_0 + pixel[5]) > cb)
                                                        if(*(cache_0 + pixel[0]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        continue;
                            else if(*(cache_0+3) < c_b)
                                continue;
                            else
                                if(*(cache_0+-3) > cb)
                                    if(*(cache_0 + pixel[10]) > cb)
                                        if(*(cache_2+-6) > cb)
                                            if(*(cache_0 + pixel[11]) > cb)
                                                if(*(cache_0 + pixel[14]) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        if(*(cache_0 + pixel[2]) > cb)
                                                            if(*(cache_0 + pixel[0]) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else if(*(cache_0 + pixel[6]) < c_b)
                            if(*(cache_0 + pixel[10]) > cb)
                                if(*(cache_0+-3) > cb)
                                    if(*(cache_0 + pixel[2]) > cb)
                                        if(*(cache_0 + pixel[11]) > cb)
                                            if(*(cache_2+-6) > cb)
                                                if(*(cache_0 + pixel[1]) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if(*(cache_0 + pixel[10]) < c_b)
                                continue;
                            else
                                if(*(cache_0+3) > cb)
                                    if(*(cache_0+-3) > cb)
                                        if(*(cache_1+1) > cb || *(cache_1+1) < c_b)
                                            continue;
                                        else
                                            if(*(cache_0 + pixel[11]) > cb)
                                                if(*(cache_2+-6) > cb)
                                                    if(*(cache_0 + pixel[5]) > cb)
                                                        continue;
                                                    else if(*(cache_0 + pixel[5]) < c_b)
                                                        goto success;
                                                    else
                                                        if(*(cache_0 + pixel[14]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else if(*(cache_0 + pixel[11]) < c_b)
                                                continue;
                                            else
                                                if(*(cache_0 + pixel[5]) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                    else
                                        continue;
                                else
                                    continue;
                        else
                            if(*(cache_0+-3) > cb)
                                if(*(cache_0 + pixel[10]) > cb)
                                    if(*(cache_0 + pixel[14]) > cb)
                                        if(*(cache_0 + pixel[1]) > cb)
                                            if(*(cache_2+-6) > cb)
                                                if(*(cache_0 + pixel[2]) > cb)
                                                    if(*(cache_0 + pixel[11]) > cb)
                                                        if(*(cache_0 + pixel[0]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else if(*(cache_0 + pixel[11]) < c_b)
                                                        continue;
                                                    else
                                                        if(*(cache_0 + pixel[5]) > cb)
                                                            goto success;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*(cache_0 + pixel[10]) < c_b)
                                    if(*(cache_0 + pixel[5]) > cb)
                                        if(*(cache_0+3) > cb)
                                            if(*(cache_1+2) > cb || *(cache_1+2) < c_b)
                                                continue;
                                            else
                                                goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    if(*(cache_0+3) > cb)
                                        if(*(cache_0 + pixel[14]) > cb)
                                            if(*(cache_0 + pixel[5]) > cb)
                                                if(*(cache_0 + pixel[1]) > cb)
                                                    if(*(cache_2+-6) > cb)
                                                        if(*(cache_0 + pixel[2]) > cb)
                                                            if(*(cache_0 + pixel[0]) > cb)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if(*(cache_0 + pixel[5]) < c_b)
                                                if(*(cache_0 + pixel[11]) > cb)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                if(*(cache_0 + pixel[11]) > cb)
                                                    if(*(cache_0 + pixel[1]) > cb)
                                                        if(*(cache_2+-6) > cb)
                                                            if(*(cache_0 + pixel[2]) > cb)
                                                                if(*(cache_0 + pixel[0]) > cb)
                                                                    goto success;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                    else
                        continue;
                else if(*cache_2 < c_b)
                    if(*(cache_0 + pixel[15]) < c_b)
                        if(*(cache_0 + pixel[5]) > cb)
                            if(*(cache_0 + pixel[10]) < c_b)
                                if(*(cache_1+2) > cb)
                                    if(*(cache_0 + pixel[2]) < c_b)
                                        if(*(cache_0 + pixel[14]) < c_b)
                                            goto success;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if(*(cache_1+2) < c_b)
                                    continue;
                                else
                                    if(*(cache_0 + pixel[11]) < c_b)
                                        if(*(cache_0 + pixel[2]) < c_b)
                                            if(*(cache_0 + pixel[1]) < c_b)
                                                if(*(cache_0 + pixel[0]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                continue;
                        else if(*(cache_0 + pixel[5]) < c_b)
                            if(*(cache_1+2) > cb)
                                if(*(cache_2+-6) < c_b)
                                    if(*(cache_0 + pixel[6]) > cb)
                                        continue;
                                    else if(*(cache_0 + pixel[6]) < c_b)
                                        goto success;
                                    else
                                        if(*(cache_0+-3) < c_b)
                                            if(*(cache_0+3) > cb)
                                                continue;
                                            else if(*(cache_0+3) < c_b)
                                                if(*(cache_0 + pixel[14]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                if(*(cache_0 + pixel[10]) < c_b)
                                                    if(*(cache_0 + pixel[2]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else if(*(cache_1+2) < c_b)
                                if(*(cache_0+3) > cb)
                                    continue;
                                else if(*(cache_0+3) < c_b)
                                    if(*(cache_0 + pixel[14]) > cb)
                                        if(*(cache_1+1) < c_b)
                                            if(*(cache_0 + pixel[6]) < c_b)
                                                if(*(cache_0 + pixel[2]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if(*(cache_0 + pixel[14]) < c_b)
                                        if(*(cache_0 + pixel[6]) > cb)
                                            continue;
                                        else if(*(cache_0 + pixel[6]) < c_b)
                                            if(*(cache_0 + pixel[1]) < c_b)
                                                if(*(cache_0 + pixel[2]) < c_b)
                                                    if(*(cache_0 + pixel[0]) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            if(*(cache_0+-3) < c_b)
                                                if(*(cache_0 + pixel[2]) < c_b)
                                                    if(*(cache_2+-6) < c_b)
                                                        goto success;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(*(cache_1+1) < c_b)
                                            if(*(cache_0 + pixel[6]) < c_b)
                                                if(*(cache_0 + pixel[2]) < c_b)
                                                    if(*(cache_0 + pixel[1]) < c_b)
                                                        if(*(cache_0 + pixel[0]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    if(*(cache_0+-3) < c_b)
                                        if(*(cache_0 + pixel[10]) < c_b)
                                            if(*(cache_2+-6) < c_b)
                                                if(*(cache_0 + pixel[14]) < c_b)
                                                    goto success;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                            else
                                if(*(cache_2+-6) < c_b)
                                    if(*(cache_0 + pixel[6]) > cb)
                                        if(*(cache_0 + pixel[10]) > cb)
                                            goto success;
                                        else
                                            continue;
                                    else if(*(cache_0 + pixel[6]) < c_b)
                                        if(*(cache_0+3) > cb)
                                            continue;
                                        else if(*(cache_0+3) < c_b)
                                            if(*(cache_0 + pixel[1]) < c_b)
                                                if(*(cache_0 + pixel[14]) < c_b)
                                                    if(*(cache_0 + pixel[2]) < c_b)
                                                        if(*(cache_0 + pixel[0]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            if(*(cache_0+-3) < c_b)
                                                if(*(cache_0 + pixel[11]) < c_b)
                                                    if(*(cache_0 + pixel[10]) < c_b)
                                                        if(*(cache_0 + pixel[1]) < c_b)
                                                            if(*(cache_1+1) > cb || *(cache_1+1) < c_b)
                                                                continue;
                                                            else
                                                                goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                    else
                                        if(*(cache_0+-3) < c_b)
                                            if(*(cache_0 + pixel[14]) < c_b)
                                                if(*(cache_0 + pixel[0]) < c_b)
                                                    if(*(cache_0+3) > cb)
                                                        continue;
                                                    else if(*(cache_0+3) < c_b)
                                                        if(*(cache_0 + pixel[2]) < c_b)
                                                            if(*(cache_0 + pixel[1]) < c_b)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        if(*(cache_0 + pixel[10]) < c_b)
                                                            if(*(cache_0 + pixel[11]) < c_b)
                                                                if(*(cache_0 + pixel[2]) < c_b)
                                                                    goto success;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                        else
                            if(*(cache_0 + pixel[11]) < c_b)
                                if(*(cache_0+-3) < c_b)
                                    if(*(cache_0 + pixel[10]) > cb)
                                        continue;
                                    else if(*(cache_0 + pixel[10]) < c_b)
                                        if(*(cache_0 + pixel[14]) < c_b)
                                            if(*(cache_0 + pixel[2]) < c_b)
                                                if(*(cache_2+-6) < c_b)
                                                    if(*(cache_0 + pixel[1]) < c_b)
                                                        if(*(cache_0 + pixel[0]) < c_b)
                                                            goto success;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        if(*(cache_0+3) < c_b)
                                            if(*(cache_0 + pixel[14]) < c_b)
                                                if(*(cache_0 + pixel[1]) < c_b)
                                                    if(*(cache_2+-6) < c_b)
                                                        if(*(cache_0 + pixel[0]) < c_b)
                                                            if(*(cache_0 + pixel[2]) < c_b)
                                                                goto success;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                else
                                    continue;
                            else
                                continue;
                    else
                        continue;
                else
                    continue;
success:																			
                if(total >= rsize)																
                {																				
                    rsize *=2;																	
                    ret=(xy*)realloc(ret, rsize*sizeof(xy));									
                }																				
                ret[total].x = cache_0-line_min;												
                ret[total++].y = y;																
        }																						
    }																							
    *num = total;																				
    return ret;																					
}	
//1.2


int corner_score(const byte*  imp, const int *pointer_dir, int barrier)
{
	/*The score for a positive feature is sum of the difference between the pixels
	  and the barrier if the difference is positive. Negative is similar.
	  The score is the max of those two.
	  
	   B = {x | x = points on the Bresenham circle around c}
	   Sp = { I(x) - t | x E B , I(x) - t > 0 }
	   Sn = { t - I(x) | x E B, t - I(x) > 0}
	  
	   Score = max sum(Sp), sum(Sn)*/

	int cb = *imp + barrier;
	int c_b = *imp - barrier;
	int sp=0, sn = 0;

	int i=0;

	for(i=0; i<16; i++)
	{
		int p = imp[pointer_dir[i]];

		if(p > cb)
			sp += p-cb;
		else if(p < c_b)
			sn += c_b-p;
	}
	
	if(sp > sn)
		return sp;
	else 
		return sn;
}
xy*  fast_nonmax(const byte* im, int xsize, int ysize, xy* corners, int numcorners, int barrier, int* numnx)
{

    /*Create a list of integer pointer offstes, corresponding to the */
    /*direction offsets in dir[]*/
    int	pointer_dir[16];
    int* row_start = (int*) malloc(ysize * sizeof(int));
    int* scores    = (int*) malloc(numcorners * sizeof(int));
    xy*  nonmax_corners=(xy*)malloc(numcorners* sizeof(xy));
    int num_nonmax=0;
    int prev_row = -1;
    int i, j;
    int point_above = 0;
    int point_below = 0;


    pointer_dir[0] = 0 + 3 * xsize;		
    pointer_dir[1] = 1 + 3 * xsize;		
    pointer_dir[2] = 2 + 2 * xsize;		
    pointer_dir[3] = 3 + 1 * xsize;		
    pointer_dir[4] = 3 + 0 * xsize;		
    pointer_dir[5] = 3 + -1 * xsize;		
    pointer_dir[6] = 2 + -2 * xsize;		
    pointer_dir[7] = 1 + -3 * xsize;		
    pointer_dir[8] = 0 + -3 * xsize;		
    pointer_dir[9] = -1 + -3 * xsize;		
    pointer_dir[10] = -2 + -2 * xsize;		
    pointer_dir[11] = -3 + -1 * xsize;		
    pointer_dir[12] = -3 + 0 * xsize;		
    pointer_dir[13] = -3 + 1 * xsize;		
    pointer_dir[14] = -2 + 2 * xsize;		
    pointer_dir[15] = -1 + 3 * xsize;		

    if(numcorners < 5)
    {
        free(row_start);
        free(scores);
        free(nonmax_corners);
        return 0;
    }

    /*xsize ysize numcorners corners*/

    /*Compute the score for each detected corner, and find where each row begins*/
    /* (the corners are output in raster scan order). A beginning of -1 signifies*/
    /* that there are no corners on that row.*/


    for(i=0; i <ysize; i++)
        row_start[i] = -1;


    for(i=0; i< numcorners; i++)
    {
        if(corners[i].y != prev_row)
        {
            row_start[corners[i].y] = i;
            prev_row = corners[i].y;
        }

        scores[i] = corner_score(im + corners[i].x + corners[i].y * xsize, pointer_dir, barrier);
    }


    /*Point above points (roughly) to the pixel above the one of interest, if there*/
    /*is a feature there.*/

    for(i=0; i < numcorners; i++)
    {
        int score = scores[i];
        xy pos = corners[i];

        //Check left 
        if(i > 0)
            if(corners[i-1].x == pos.x-1 && corners[i-1].y == pos.y && scores[i-1] > score)
                continue;

        //Check right
        if(i < (numcorners - 1))
            if(corners[i+1].x == pos.x+1 && corners[i+1].y == pos.y && scores[i+1] > score)
                continue;

        //Check above (if there is a valid row above)
        if(pos.y != 0 && row_start[pos.y - 1] != -1) 
        {
            //Make sure that current point_above is one
            //row above.
            if(corners[point_above].y < pos.y - 1)
                point_above = row_start[pos.y-1];

            //Make point_above point to the first of the pixels above the current point,
            //if it exists.
            for(; corners[point_above].y < pos.y && corners[point_above].x < pos.x - 1; point_above++)
            {}


            for(j=point_above; corners[j].y < pos.y && corners[j].x <= pos.x + 1; j++)
            {
                int x = corners[j].x;
                if( (x == pos.x - 1 || x ==pos.x || x == pos.x+1) && (scores[j] > score))
                    goto cont;
            }

        }

        //Check below (if there is anything below)
        if(pos.y != ysize-1 && row_start[pos.y + 1] != -1 && point_below < numcorners) //Nothing below
        {
            if(corners[point_below].y < pos.y + 1)
                point_below = row_start[pos.y+1];

            // Make point below point to one of the pixels belowthe current point, if it
            // exists.
            for(; point_below < numcorners && corners[point_below].y == pos.y+1 && corners[point_below].x < pos.x - 1; point_below++)
            {}

            for(j=point_below; j < numcorners && corners[j].y == pos.y+1 && corners[j].x <= pos.x + 1; j++)
            {
                int x = corners[j].x;
                if( (x == pos.x - 1 || x ==pos.x || x == pos.x+1) && (scores[j] >score))
                    goto cont;
            }
        }

        nonmax_corners[num_nonmax].x = corners[i].x;
        nonmax_corners[num_nonmax].y = corners[i].y;

        num_nonmax++;

cont:
        ;
    }

    *numnx = num_nonmax;

    free(row_start);
    free(scores);
    return nonmax_corners;
}
	
/**
* @brief	  Calculate FAST(features from accelerated segment test)corner saved into FASTMap [imgDimensionX*imgDimensionY](verified)
* @param[out] FASTMap    	    [corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]  SobelEdge_V		Sobel vertical edge.size:[imgDimensionX*imgDimensionY]
* @param[in]  Img			    Input original image.size:[imgDimensionX*imgDimensionY]
* @return     the corners Number
* @note       在SUSAN(1997年)方法的基礎上，2005年, Rosten在論文[2]中提出基於The segment test criterion的角點檢測方法
 可以獲得更快的結果，還採用了額外的加速辦法。如果測試了候選點周圍每隔90度角的4個點，應該至少有3個和候選點的灰度值差足夠大，否則則不用再計算其他點，直接認為該候選點不是特徵點。候選點周圍的圓的選取半徑是一個很重要的參數，這裡我為了簡單高效，採用半徑為3，共有16個周邊像素需要比較。為了提高比較的效率，通常只使用N個周邊像素來比較，也就是大家經常說的FAST-N。我看很多文獻推薦FAST-9，作者的主頁上有FAST-9、FAST-10、FAST-11、FAST-12，大家使用比較多的是FAST-9和FAST-12。
* @ref        http://blog.csdn.net/anshan1984/article/details/8867653
http://users.fmrib.ox.ac.uk/~steve/susan/
http://blog.csdn.net/yang_xian521/article/details/7411438
*/
INT32 CornerDetector::Corners_FAST( UINT16 *FASTMap ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    const INT32 THD1_FAST = 33;  ///< for fast corner detect
    const INT32 THD2_FAST = 0;  ///< for non-max
    INT32 ret_fastcornerNum = 0;
    ///1) initialize
    memset(FASTMap , 0, sizeof(UINT16)*imgDimensionX*imgDimensionY);

    ///2) do fast corner
    xy *data = fast_corner_detect_10(Img , imgDimensionX, imgDimensionY , 33 , &ret_fastcornerNum);
    data = fast_nonmax(Img,imgDimensionX,imgDimensionY,data,ret_fastcornerNum,11,&ret_fastcornerNum);

    for ( int i1 = 0 ;i1 < ret_fastcornerNum ; ++i1)
    {
        FASTMap[data[i1].x + data[i1].y*imgDimensionX] =0xFFFF;
    }
    return ret_fastcornerNum; //ex. 1500 means 1500 fast corners
}

/**
* @brief	  Calculate the sum of absolute difference of (X1,Y1) and (X2,Y2) with 3x3 window
* @param[out] MoravecMap		is corners[corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]  Img			    Input original image.size:[imgDimensionX*imgDimensionY]
* @return     the corners Number
*/
static INT32 calAbsSum3(INT32 posX1, INT32 posY1, INT32 posX2, INT32 posY2, UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY)
{
    INT32 ret = 0;

    ret = ABS(Img[ (posX1-1) + (posY1-1)*imgDimensionX ] - Img[ (posX2-1) + (posY2-1)*imgDimensionX ])
        + ABS(Img[ (posX1  ) + (posY1-1)*imgDimensionX ] - Img[ (posX2  ) + (posY2-1)*imgDimensionX ])
        + ABS(Img[ (posX1+1) + (posY1-1)*imgDimensionX ] - Img[ (posX2+1) + (posY2-1)*imgDimensionX ])
        + ABS(Img[ (posX1-1) + (posY1  )*imgDimensionX ] - Img[ (posX2-1) + (posY2  )*imgDimensionX ])
        + ABS(Img[ (posX1  ) + (posY1  )*imgDimensionX ] - Img[ (posX2  ) + (posY2  )*imgDimensionX ])
        + ABS(Img[ (posX1+1) + (posY1  )*imgDimensionX ] - Img[ (posX2+1) + (posY2  )*imgDimensionX ])
        + ABS(Img[ (posX1-1) + (posY1+1)*imgDimensionX ] - Img[ (posX2-1) + (posY2+1)*imgDimensionX ])
        + ABS(Img[ (posX1  ) + (posY1+1)*imgDimensionX ] - Img[ (posX2  ) + (posY2+1)*imgDimensionX ])
        + ABS(Img[ (posX1+1) + (posY1+1)*imgDimensionX ] - Img[ (posX2+1) + (posY2+1)*imgDimensionX ]);
    return ret; //ex. 30 means SAD
}

/**
* @brief	  Calculate Moravec corner (verified)
* @param[out] MoravecMap		is corners[corner:0xFFFF , supressed:>0 , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]  Img			    Input original image.size:[imgDimensionX*imgDimensionY]
* @return     the corners Number
* @ref        http://arxiv.org/ftp/arxiv/papers/1209/1209.1558.pdf
*/
INT32 CornerDetector::Corners_Moravec( UINT16 *MoravecMap , UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    //**************************************
    const INT32 THD_Moravec    = 100;   //range: 0-65535; 
    //**************************************
    const INT16 shift[8][2] = {{-1,-1},{0,-1},{1,-1},
                                {-1,0},        {1,0},
                                {-1,1},{0,1} ,{1,1}};

    ///1) initilize
    memset( MoravecMap,0, sizeof(UINT16)*imgDimensionX*imgDimensionY );

    INT32 ret = 0;
    INT32 offsetV;
    INT32 SADminValue;
    INT32 tmp1;
    for ( INT32 vert = 1 ; vert< (imgDimensionY-1) ; ++vert)
    {
        offsetV = vert * imgDimensionX;
        for ( INT32 hori = 1 ; hori< (imgDimensionX-1) ; ++hori)
        {
            /// cal min SAD
            SADminValue = 0x7FFFFFFF;
            for(int i1 = 0 ; i1 < 8; ++i1)
            {
                tmp1 = calAbsSum3(hori,vert,hori+shift[i1][0] ,vert+shift[i1][1],Img,imgDimensionX,imgDimensionY);
                SADminValue = MIN(SADminValue , tmp1);
            }

            /// Threshold  the  interest  map  by  setting  all  C(x,  y)  below a threshold T to zero.
            MoravecMap[ hori + offsetV ] = ( SADminValue < THD_Moravec )?(0):(SADminValue);
        }
    }

    ///3) non-Maximun suppress [make the real corners as 0xFFFF]
    ret = NON_MAX_SUPRESSION(1,1, imgDimensionX-1 , imgDimensionY-1 ,MoravecMap,imgDimensionX,imgDimensionY );

    return ret;
}

static void susan_corners(uchar *in, int *r, uchar *bp, int max_no,CORNER_LIST corner_list , int x_size, int y_size)
{
    int   n,x,y,sq,xx,yy,
        i,j,*cgx,*cgy;
    float divide;
    uchar c,*p,*cp;

    memset (r,0,x_size * y_size * sizeof(int));

    cgx=(int *)malloc(x_size*y_size*sizeof(int));
    cgy=(int *)malloc(x_size*y_size*sizeof(int));

    for (i=5;i<y_size-5;i++)
        for (j=5;j<x_size-5;j++) {
            n=100;
            p=in + (i-3)*x_size + j - 1;
            cp=bp + in[i*x_size+j];

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            p+=x_size-3; 

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            p+=x_size-5;

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            p+=x_size-6;

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            if (n<max_no){    /* do this test early and often ONLY to save wasted computation */
                p+=2;
                n+=*(cp-*p++);
                if (n<max_no){
                    n+=*(cp-*p++);
                    if (n<max_no){
                        n+=*(cp-*p);
                        if (n<max_no){
                            p+=x_size-6;

                            n+=*(cp-*p++);
                            if (n<max_no){
                                n+=*(cp-*p++);
                                if (n<max_no){
                                    n+=*(cp-*p++);
                                    if (n<max_no){
                                        n+=*(cp-*p++);
                                        if (n<max_no){
                                            n+=*(cp-*p++);
                                            if (n<max_no){
                                                n+=*(cp-*p++);
                                                if (n<max_no){
                                                    n+=*(cp-*p);
                                                    if (n<max_no){
                                                        p+=x_size-5;

                                                        n+=*(cp-*p++);
                                                        if (n<max_no){
                                                            n+=*(cp-*p++);
                                                            if (n<max_no){
                                                                n+=*(cp-*p++);
                                                                if (n<max_no){
                                                                    n+=*(cp-*p++);
                                                                    if (n<max_no){
                                                                        n+=*(cp-*p);
                                                                        if (n<max_no){
                                                                            p+=x_size-3;

                                                                            n+=*(cp-*p++);
                                                                            if (n<max_no){
                                                                                n+=*(cp-*p++);
                                                                                if (n<max_no){
                                                                                    n+=*(cp-*p);

                                                                                    if (n<max_no)
                                                                                    {
                                                                                        x=0;y=0;
                                                                                        p=in + (i-3)*x_size + j - 1;

                                                                                        c=*(cp-*p++);x-=c;y-=3*c;
                                                                                        c=*(cp-*p++);y-=3*c;
                                                                                        c=*(cp-*p);x+=c;y-=3*c;
                                                                                        p+=x_size-3; 

                                                                                        c=*(cp-*p++);x-=2*c;y-=2*c;
                                                                                        c=*(cp-*p++);x-=c;y-=2*c;
                                                                                        c=*(cp-*p++);y-=2*c;
                                                                                        c=*(cp-*p++);x+=c;y-=2*c;
                                                                                        c=*(cp-*p);x+=2*c;y-=2*c;
                                                                                        p+=x_size-5;

                                                                                        c=*(cp-*p++);x-=3*c;y-=c;
                                                                                        c=*(cp-*p++);x-=2*c;y-=c;
                                                                                        c=*(cp-*p++);x-=c;y-=c;
                                                                                        c=*(cp-*p++);y-=c;
                                                                                        c=*(cp-*p++);x+=c;y-=c;
                                                                                        c=*(cp-*p++);x+=2*c;y-=c;
                                                                                        c=*(cp-*p);x+=3*c;y-=c;
                                                                                        p+=x_size-6;

                                                                                        c=*(cp-*p++);x-=3*c;
                                                                                        c=*(cp-*p++);x-=2*c;
                                                                                        c=*(cp-*p);x-=c;
                                                                                        p+=2;
                                                                                        c=*(cp-*p++);x+=c;
                                                                                        c=*(cp-*p++);x+=2*c;
                                                                                        c=*(cp-*p);x+=3*c;
                                                                                        p+=x_size-6;

                                                                                        c=*(cp-*p++);x-=3*c;y+=c;
                                                                                        c=*(cp-*p++);x-=2*c;y+=c;
                                                                                        c=*(cp-*p++);x-=c;y+=c;
                                                                                        c=*(cp-*p++);y+=c;
                                                                                        c=*(cp-*p++);x+=c;y+=c;
                                                                                        c=*(cp-*p++);x+=2*c;y+=c;
                                                                                        c=*(cp-*p);x+=3*c;y+=c;
                                                                                        p+=x_size-5;

                                                                                        c=*(cp-*p++);x-=2*c;y+=2*c;
                                                                                        c=*(cp-*p++);x-=c;y+=2*c;
                                                                                        c=*(cp-*p++);y+=2*c;
                                                                                        c=*(cp-*p++);x+=c;y+=2*c;
                                                                                        c=*(cp-*p);x+=2*c;y+=2*c;
                                                                                        p+=x_size-3;

                                                                                        c=*(cp-*p++);x-=c;y+=3*c;
                                                                                        c=*(cp-*p++);y+=3*c;
                                                                                        c=*(cp-*p);x+=c;y+=3*c;

                                                                                        xx=x*x;
                                                                                        yy=y*y;
                                                                                        sq=xx+yy;
                                                                                        if ( sq > ((n*n)/2) )
                                                                                        {
                                                                                            if(yy<xx) {
                                                                                                divide=(float)y/(float)abs(x);
                                                                                                sq=abs(x)/x;
                                                                                                sq=*(cp-in[(i+FTOI(divide))*x_size+j+sq]) +
                                                                                                    *(cp-in[(i+FTOI(2*divide))*x_size+j+2*sq]) +
                                                                                                    *(cp-in[(i+FTOI(3*divide))*x_size+j+3*sq]);}
                                                                                            else {
                                                                                                divide=(float)x/(float)abs(y);
                                                                                                sq=abs(y)/y;
                                                                                                sq=*(cp-in[(i+sq)*x_size+j+FTOI(divide)]) +
                                                                                                    *(cp-in[(i+2*sq)*x_size+j+FTOI(2*divide)]) +
                                                                                                    *(cp-in[(i+3*sq)*x_size+j+FTOI(3*divide)]);}

                                                                                            if(sq>290){
                                                                                                r[i*x_size+j] = max_no-n;
                                                                                                cgx[i*x_size+j] = (51*x)/n;
                                                                                                cgy[i*x_size+j] = (51*y)/n;}
                                                                                        }
                                                                                    }
                                                                                }}}}}}}}}}}}}}}}}}}

        /* to locate the local maxima */
        n=0;
        for (i=5;i<y_size-5;i++)
            for (j=5;j<x_size-5;j++) {
                x = r[i*x_size+j];
                if (x>0)  {
                    /* 5x5 mask */
                        if ( 
                            (x>r[(i-3)*x_size+j-3]) &&
                            (x>r[(i-3)*x_size+j-2]) &&
                            (x>r[(i-3)*x_size+j-1]) &&
                            (x>r[(i-3)*x_size+j  ]) &&
                            (x>r[(i-3)*x_size+j+1]) &&
                            (x>r[(i-3)*x_size+j+2]) &&
                            (x>r[(i-3)*x_size+j+3]) &&

                            (x>r[(i-2)*x_size+j-3]) &&
                            (x>r[(i-2)*x_size+j-2]) &&
                            (x>r[(i-2)*x_size+j-1]) &&
                            (x>r[(i-2)*x_size+j  ]) &&
                            (x>r[(i-2)*x_size+j+1]) &&
                            (x>r[(i-2)*x_size+j+2]) &&
                            (x>r[(i-2)*x_size+j+3]) &&

                            (x>r[(i-1)*x_size+j-3]) &&
                            (x>r[(i-1)*x_size+j-2]) &&
                            (x>r[(i-1)*x_size+j-1]) &&
                            (x>r[(i-1)*x_size+j  ]) &&
                            (x>r[(i-1)*x_size+j+1]) &&
                            (x>r[(i-1)*x_size+j+2]) &&
                            (x>r[(i-1)*x_size+j+3]) &&

                            (x>r[(i)*x_size+j-3]) &&
                            (x>r[(i)*x_size+j-2]) &&
                            (x>r[(i)*x_size+j-1]) &&
                            (x>=r[(i)*x_size+j+1]) &&
                            (x>=r[(i)*x_size+j+2]) &&
                            (x>=r[(i)*x_size+j+3]) &&

                            (x>=r[(i+1)*x_size+j-3]) &&
                            (x>=r[(i+1)*x_size+j-2]) &&
                            (x>=r[(i+1)*x_size+j-1]) &&
                            (x>=r[(i+1)*x_size+j  ]) &&
                            (x>=r[(i+1)*x_size+j+1]) &&
                            (x>=r[(i+1)*x_size+j+2]) &&
                            (x>=r[(i+1)*x_size+j+3]) &&

                            (x>=r[(i+2)*x_size+j-3]) &&
                            (x>=r[(i+2)*x_size+j-2]) &&
                            (x>=r[(i+2)*x_size+j-1]) &&
                            (x>=r[(i+2)*x_size+j  ]) &&
                            (x>=r[(i+2)*x_size+j+1]) &&
                            (x>=r[(i+2)*x_size+j+2]) &&
                            (x>=r[(i+2)*x_size+j+3]) &&

                            (x>=r[(i+3)*x_size+j-3]) &&
                            (x>=r[(i+3)*x_size+j-2]) &&
                            (x>=r[(i+3)*x_size+j-1]) &&
                            (x>=r[(i+3)*x_size+j  ]) &&
                            (x>=r[(i+3)*x_size+j+1]) &&
                            (x>=r[(i+3)*x_size+j+2]) &&
                            (x>=r[(i+3)*x_size+j+3]) )
                        {
                            corner_list[n].info=0;
                            corner_list[n].x=j;
                            corner_list[n].y=i;
                            corner_list[n].dx=cgx[i*x_size+j];
                            corner_list[n].dy=cgy[i*x_size+j];
                            corner_list[n].I=in[i*x_size+j];
                            n++;
                            if(n==MAX_SUSANCORNERS){
                                fprintf(stderr,"Too many corners.\n");
                                exit(1);
                            }}}}
            corner_list[n].info=7;

            free(cgx);
            free(cgy);

}

static void susan_corners_quick(uchar *in, int *r, uchar *bp, int max_no, CORNER_LIST corner_list, int x_size, int y_size)
{
    int   n,x,y,i,j;
    uchar *p,*cp;

    memset( r , 0 , x_size * y_size * sizeof(int) );

    for (i=7;i<y_size-7;i++)
        for (j=7;j<x_size-7;j++) {
            n=100;
            p=in + (i-3)*x_size + j - 1;
            cp=bp + in[i*x_size+j];

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            p+=x_size-3;

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            p+=x_size-5;

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            p+=x_size-6;

            n+=*(cp-*p++);
            n+=*(cp-*p++);
            n+=*(cp-*p);
            if (n<max_no){
                p+=2;
                n+=*(cp-*p++);
                if (n<max_no){
                    n+=*(cp-*p++);
                    if (n<max_no){
                        n+=*(cp-*p);
                        if (n<max_no){
                            p+=x_size-6;

                            n+=*(cp-*p++);
                            if (n<max_no){
                                n+=*(cp-*p++);
                                if (n<max_no){
                                    n+=*(cp-*p++);
                                    if (n<max_no){
                                        n+=*(cp-*p++);
                                        if (n<max_no){
                                            n+=*(cp-*p++);
                                            if (n<max_no){
                                                n+=*(cp-*p++);
                                                if (n<max_no){
                                                    n+=*(cp-*p);
                                                    if (n<max_no){
                                                        p+=x_size-5;

                                                        n+=*(cp-*p++);
                                                        if (n<max_no){
                                                            n+=*(cp-*p++);
                                                            if (n<max_no){
                                                                n+=*(cp-*p++);
                                                                if (n<max_no){
                                                                    n+=*(cp-*p++);
                                                                    if (n<max_no){
                                                                        n+=*(cp-*p);
                                                                        if (n<max_no){
                                                                            p+=x_size-3;

                                                                            n+=*(cp-*p++);
                                                                            if (n<max_no){
                                                                                n+=*(cp-*p++);
                                                                                if (n<max_no){
                                                                                    n+=*(cp-*p);

                                                                                    if (n<max_no)
                                                                                        r[i*x_size+j] = max_no-n;
                                                                                }}}}}}}}}}}}}}}}}}}

        /* to locate the local maxima */
        n=0;
        for (i=7;i<y_size-7;i++)
            for (j=7;j<x_size-7;j++) {
                x = r[i*x_size+j];
                if (x>0)  {
                    /* 5x5 mask */

                        if ( 
                            (x>r[(i-3)*x_size+j-3]) &&
                            (x>r[(i-3)*x_size+j-2]) &&
                            (x>r[(i-3)*x_size+j-1]) &&
                            (x>r[(i-3)*x_size+j  ]) &&
                            (x>r[(i-3)*x_size+j+1]) &&
                            (x>r[(i-3)*x_size+j+2]) &&
                            (x>r[(i-3)*x_size+j+3]) &&

                            (x>r[(i-2)*x_size+j-3]) &&
                            (x>r[(i-2)*x_size+j-2]) &&
                            (x>r[(i-2)*x_size+j-1]) &&
                            (x>r[(i-2)*x_size+j  ]) &&
                            (x>r[(i-2)*x_size+j+1]) &&
                            (x>r[(i-2)*x_size+j+2]) &&
                            (x>r[(i-2)*x_size+j+3]) &&

                            (x>r[(i-1)*x_size+j-3]) &&
                            (x>r[(i-1)*x_size+j-2]) &&
                            (x>r[(i-1)*x_size+j-1]) &&
                            (x>r[(i-1)*x_size+j  ]) &&
                            (x>r[(i-1)*x_size+j+1]) &&
                            (x>r[(i-1)*x_size+j+2]) &&
                            (x>r[(i-1)*x_size+j+3]) &&

                            (x>r[(i)*x_size+j-3]) &&
                            (x>r[(i)*x_size+j-2]) &&
                            (x>r[(i)*x_size+j-1]) &&
                            (x>=r[(i)*x_size+j+1]) &&
                            (x>=r[(i)*x_size+j+2]) &&
                            (x>=r[(i)*x_size+j+3]) &&

                            (x>=r[(i+1)*x_size+j-3]) &&
                            (x>=r[(i+1)*x_size+j-2]) &&
                            (x>=r[(i+1)*x_size+j-1]) &&
                            (x>=r[(i+1)*x_size+j  ]) &&
                            (x>=r[(i+1)*x_size+j+1]) &&
                            (x>=r[(i+1)*x_size+j+2]) &&
                            (x>=r[(i+1)*x_size+j+3]) &&

                            (x>=r[(i+2)*x_size+j-3]) &&
                            (x>=r[(i+2)*x_size+j-2]) &&
                            (x>=r[(i+2)*x_size+j-1]) &&
                            (x>=r[(i+2)*x_size+j  ]) &&
                            (x>=r[(i+2)*x_size+j+1]) &&
                            (x>=r[(i+2)*x_size+j+2]) &&
                            (x>=r[(i+2)*x_size+j+3]) &&

                            (x>=r[(i+3)*x_size+j-3]) &&
                            (x>=r[(i+3)*x_size+j-2]) &&
                            (x>=r[(i+3)*x_size+j-1]) &&
                            (x>=r[(i+3)*x_size+j  ]) &&
                            (x>=r[(i+3)*x_size+j+1]) &&
                            (x>=r[(i+3)*x_size+j+2]) &&
                            (x>=r[(i+3)*x_size+j+3]) )

                        {
                            corner_list[n].info=0;
                            corner_list[n].x=j;
                            corner_list[n].y=i;
                            x = in[(i-2)*x_size+j-2] + in[(i-2)*x_size+j-1] + in[(i-2)*x_size+j] + in[(i-2)*x_size+j+1] + in[(i-2)*x_size+j+2] +
                                in[(i-1)*x_size+j-2] + in[(i-1)*x_size+j-1] + in[(i-1)*x_size+j] + in[(i-1)*x_size+j+1] + in[(i-1)*x_size+j+2] +
                                in[(i  )*x_size+j-2] + in[(i  )*x_size+j-1] + in[(i  )*x_size+j] + in[(i  )*x_size+j+1] + in[(i  )*x_size+j+2] +
                                in[(i+1)*x_size+j-2] + in[(i+1)*x_size+j-1] + in[(i+1)*x_size+j] + in[(i+1)*x_size+j+1] + in[(i+1)*x_size+j+2] +
                                in[(i+2)*x_size+j-2] + in[(i+2)*x_size+j-1] + in[(i+2)*x_size+j] + in[(i+2)*x_size+j+1] + in[(i+2)*x_size+j+2];

                            corner_list[n].I=x/25;
                            /*corner_list[n].I=in[i*x_size+j];*/
                            x = in[(i-2)*x_size+j+2] + in[(i-1)*x_size+j+2] + in[(i)*x_size+j+2] + in[(i+1)*x_size+j+2] + in[(i+2)*x_size+j+2] -
                                (in[(i-2)*x_size+j-2] + in[(i-1)*x_size+j-2] + in[(i)*x_size+j-2] + in[(i+1)*x_size+j-2] + in[(i+2)*x_size+j-2]);
                            x += x + in[(i-2)*x_size+j+1] + in[(i-1)*x_size+j+1] + in[(i)*x_size+j+1] + in[(i+1)*x_size+j+1] + in[(i+2)*x_size+j+1] -
                                (in[(i-2)*x_size+j-1] + in[(i-1)*x_size+j-1] + in[(i)*x_size+j-1] + in[(i+1)*x_size+j-1] + in[(i+2)*x_size+j-1]);

                            y = in[(i+2)*x_size+j-2] + in[(i+2)*x_size+j-1] + in[(i+2)*x_size+j] + in[(i+2)*x_size+j+1] + in[(i+2)*x_size+j+2] -
                                (in[(i-2)*x_size+j-2] + in[(i-2)*x_size+j-1] + in[(i-2)*x_size+j] + in[(i-2)*x_size+j+1] + in[(i-2)*x_size+j+2]);
                            y += y + in[(i+1)*x_size+j-2] + in[(i+1)*x_size+j-1] + in[(i+1)*x_size+j] + in[(i+1)*x_size+j+1] + in[(i+1)*x_size+j+2] -
                                (in[(i-1)*x_size+j-2] + in[(i-1)*x_size+j-1] + in[(i-1)*x_size+j] + in[(i-1)*x_size+j+1] + in[(i-1)*x_size+j+2]);
                            corner_list[n].dx=x/15;
                            corner_list[n].dy=y/15;
                            n++;
                            if(n==MAX_SUSANCORNERS){
                                fprintf(stderr,"Too many corners.\n");
                                exit(1);
                            }}}}
            corner_list[n].info=7;

}

static void setup_brightness_lut(uchar **bp ,int thresh, int form)
{
    int   k;
    float temp;

    *bp=(unsigned char *)malloc(516);
    *bp=*bp+258;

    for(k=-256;k<257;k++)
    {
        temp=((float)k)/((float)thresh);
        temp=temp*temp;
        if (form==6)
            temp=temp*temp*temp;
        temp=100.0*exp(-temp);
        *(*bp+k)= (uchar)temp;
    }
}
/**
* @brief	   Calculate SUSAN corner saved into SUSANMap[imgDimensionX*imgDimensionY]
* @param[out]  SUSANMap	         [corner:0xFFFF , Not corner:0]. size:[imgDimensionX*imgDimensionY]
* @param[in]   Img			     Input original image.size:[imgDimensionX*imgDimensionY]
* @param[out]  harrisCorners	 SUSAN corner save structure
* @param[in]   sizeHarrisCorner  the MAX size of structure
* @return      the corners Number ex. 400 means 400 corners in LEFT or RIGHT ROI
* @ref       http://users.fmrib.ox.ac.uk/~steve/susan/  / http://baike.baidu.com/view/2956084.htm?fr=iciba
*/
INT32 CornerDetector::Corners_SUSAN( UINT16 *SUSANMap ,UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY, HarrisCornerPos *harrisCorners , const INT32 sizeHarrisCorner , const INT32 RegionL[4] ,const INT32 RegionR[4] )
{
    ///THD
    INT32 max_no_corners = 600;

    UCHAR   *bp;//, *mid; *in,
    INT32  *r;
    INT32  mode;//,i;
    CORNER_LIST corner_list;
    INT32  bt  = 20;
    INT32  ret = 0;

    ///1) initial
    memset( SUSANMap , 0 , imgDimensionX*imgDimensionY*sizeof(UINT16) );
    r   = (int *) malloc(imgDimensionX * imgDimensionY * sizeof(int));
    setup_brightness_lut(&bp,bt,6);

    ///2) SUSAN Alg
    mode = 0;
    if(0==mode)
        susan_corners_quick( Img ,r,bp,max_no_corners,corner_list,imgDimensionX,imgDimensionY);  
    else if (1==mode)
        susan_corners      ( Img ,r,bp,max_no_corners,corner_list,imgDimensionX,imgDimensionY);  

    ///3) SUSANMap & harrisCorners establish
    //INT32 offsetV;
    INT32 Idx_SusanCorners = 0;
    INT32 tmpx,tmpy;
    for ( int i1 = 0 ;i1 < max_no_corners ; ++i1)
    {
        if(corner_list[i1].info == 0)
        {
            tmpx = corner_list[i1].x;
            tmpy = corner_list[i1].y;

            SUSANMap[ tmpx + tmpy * imgDimensionX ] = 0xFFFF;

            if( (tmpx>= RegionL[0]) && (tmpx< RegionL[2]) && (tmpy>= RegionL[1]) && (tmpy < RegionL[3]))//left
            {
                harrisCorners[Idx_SusanCorners].x = tmpx;
                harrisCorners[Idx_SusanCorners].y = tmpy;
                ++Idx_SusanCorners ;
                ++ret;
            }
            //Right
            else if( (tmpx>= RegionR[0]) && (tmpx< RegionR[2]) && (tmpy>= RegionR[1]) && (tmpy < RegionR[3]))
            {
                harrisCorners[Idx_SusanCorners].x = tmpx;
                harrisCorners[Idx_SusanCorners].y = tmpy;
                ++Idx_SusanCorners ;
                ++ret;
            }
        }
    }

    ///4 delete
    delete [] r; r = NULL;

    return ret; //ex. 400 means 400 corners in LEFT or RIGHT ROI
}

CornerDetector* CornerDetector::getInstance()
{
    if(!instance)
        instance = new CornerDetector;
    return instance;
}

CornerDetector::CornerDetector(void)
{
}
CornerDetector::~CornerDetector(void)
{
}


#endif // END #ifdef _WIN32
