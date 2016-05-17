//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
  #include "stdafx.h"
  #include <string.h> //memcpy
  #include <stdlib.h> //realloc
  #include <stdio.h> 
  #include <math.h>       /* atan */
  #include "ARRAY.h" /// canny edge
#else // K2 platform
  #include "GlobalTypes.h"
#endif

#include "Filter2D.h"
#include "CTAApi.h"

#ifdef _WIN32
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
//=============Canny================
/* Scale floating point magnitudes and angles to 8 bits */
#define ORI_SCALE 40.0
#define MAG_SCALE 20.0

/* Biggest possible filter mask */
#define MAX_MASK_SIZE 20

int WIDTH = 0;      // canny edge
typedef uc2D IMAGE; // canny edge


Filter2D* Filter2D::instance = NULL;
#endif
//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

/**
*@brief	    Get Gaussian-smooth Gray Image
*@param[in] src_Y YCbCr Image
*@note      ROI(RegionL) has indent into FLT_GAUSSIAN_3x3_sub
*/
#ifdef _WIN32
void Filter2D::Apex_Process_0_Gaussian( unsigned char* Gaussian_img, unsigned char *src_Y , int imgDimensionX, int imgDimensionY )
{
    FLT_GAUSSIAN_3x3_sub(Gaussian_img, src_Y, imgDimensionX, imgDimensionY);
}
#endif // #ifdef _WIN32

/**
* @brief     Get unsigned char Sobel_img = |vSobel_img|+|vSobel_img| from vSobel_img and hSobel_img by using mask
             of Sobel mask.
* @param[in] Gaussian_img	Input Gaussian image
* @param[in] imgDimensionX	Input FrameWidth
* @param[in] imgDimensionY	Input FrameHeight
* @param[in] ROI			Input ROI (RegionL has indent into FLT_SOBEL_3x3_REGION)
*/
#ifdef _WIN32
void Filter2D::Apex_Process_1_0_SobelErosion( unsigned char* Sobel_img, unsigned char* vSobel_img, unsigned char* hSobel_img, unsigned char* Gaussian_img ,int imgDimensionX, int imgDimensionY )
{
    /// add FLT_SOBEL_3x3_H, FLT_EdgeAdjustment
    
    FLT_SOBEL_3x3_REGION( Sobel_img, vSobel_img, hSobel_img, Gaussian_img, imgDimensionX, imgDimensionY);
    /*Warning : Performance Bad
    //FLT_EROSION_3x3( Sobel_img, g_tSobel_img, imgDimensionX, imgDimensionY, 1);
    //FLT_DILATION_3x3(Sobel_img, g_tSobel_img, imgDimensionX, imgDimensionY);
    */
}
#endif // #ifdef _WIN32

/**
* @brief	Gaussian 3x3 kernel
                   [1   2   1]
            1/16 * [2   4   2]
                   [1   2   1]
            and calculate image average Y_value 
*/
#ifdef _WIN32
void Filter2D::FLT_GAUSSIAN_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height)
{
    unsigned int sum;
    int p[3][3];		
    int end_y, end_x;
    int x, y;
    int top_y, bot_y; 
    int left_x, right_x;  
    
    end_y = img_height - 1;
    end_x = img_width - 1;

    for (y=0; y<img_height; y++)
    {
        for (x=0; x<img_width; x++)
        {        
            top_y = y - 1;
            top_y = MAX(0, top_y);
            bot_y = y + 1;
            bot_y = MIN(end_y, bot_y);
            left_x = x - 1;
            left_x = MAX(0, left_x);
            right_x = x + 1;
            right_x = MIN(end_x, right_x);
                                    
            p[0][0] = src[top_y*img_width +  left_x];
            p[0][1] = src[top_y*img_width +       x];
            p[0][2] = src[top_y*img_width + right_x];
            p[1][0] = src[    y*img_width +  left_x];
            p[1][1] = src[    y*img_width +       x];
            p[1][2] = src[    y*img_width + right_x];
            p[2][0] = src[bot_y*img_width +  left_x];
            p[2][1] = src[bot_y*img_width +       x];
            p[2][2] = src[bot_y*img_width + right_x];
            sum = p[0][0] * 1 + p[0][1] * 2 + p[0][2] * 1 +
                  p[1][0] * 2 + p[1][1] * 4 + p[1][2] * 2 +
                  p[2][0] * 1 + p[2][1] * 2 + p[2][2] * 1;

            *(dst + y*img_width + x) = SAT8(sum >> 4);
        }
    } // END for (y=0; y<img_height; y++)
}
#endif // #ifdef _WIN32

/**
* @brief        run Gaussian smoothed Image in sub-region and result image(dst) is same as APEX process.
* @param[in]    src     Image input
* @param[out]   dst     Image with Gaussian Smoothed in sub-region
*/
#ifdef _WIN32
void Filter2D::FLT_GAUSSIAN_3x3_sub(UCHAR *dst, UCHAR *src, INT32 img_width, INT32 img_height)
{
    unsigned int sum;
    int p[3][3];		
    int start_y;
    int end_y;
    int x, y;
    int top_y, bot_y; 
    int left_x, right_x;  
    
    start_y = RegionL[1];
    end_y   = RegionL[3];
    
    for ( y = start_y ; y<end_y; ++y)
    {
        for ( x = 0; x < img_width; ++x)
        {        
            top_y = y - 1;
            top_y = MAX(start_y, top_y);
            bot_y = y + 1;
            bot_y = MIN(end_y-1, bot_y);
            left_x = x - 1;
            left_x = MAX(0, left_x);
            right_x = x + 1;
            right_x = MIN(img_width-1, right_x);
                                    
            p[0][0] = src[top_y*img_width +  left_x];
            p[0][1] = src[top_y*img_width +       x];
            p[0][2] = src[top_y*img_width + right_x];
            p[1][0] = src[    y*img_width +  left_x];
            p[1][1] = src[    y*img_width +       x];
            p[1][2] = src[    y*img_width + right_x];
            p[2][0] = src[bot_y*img_width +  left_x];
            p[2][1] = src[bot_y*img_width +       x];
            p[2][2] = src[bot_y*img_width + right_x];
            sum = p[0][0] * 1 + p[0][1] * 2 + p[0][2] * 1 +
                  p[1][0] * 2 + p[1][1] * 4 + p[1][2] * 2 +
                  p[2][0] * 1 + p[2][1] * 2 + p[2][2] * 1;

            *(dst + y*img_width + x) = SAT8(sum >> 4);
            //debug			
            //*(dst + y*img_width + x) = p[1][1];
        }
    } // END for (y=0; y<img_height; y++)
}
#endif // #ifdef _WIN32

/**
* @brief Do Sobel 1's Differential edge detection
* @NOTE		Sobel 3x3 kernel
           [1   0  -1		or		-1	0	1
            2   0  -2				-2	0	2
            1   0  -1]				-1	0	1	

    @n		float contrast_ratio: 0.10 ~ 0.15 (use numerator/denominator)
    @n		255: right direction gradient, desired edge
    @n		128: not an edge
    @n		0: contrary direction gradient edge
*/
#ifdef _WIN32
void Filter2D::FLT_SOBEL_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height, int contrast_numerator, int contrast_denominator, int &edgeCounter)
{	
    //bit 1, 0: 00:plane
    //			01:downward	1	0	-1
    //			10:upward			2	0	-2
    //			11: null				1	0	-1
    //direction: left--0; right--1; find the lane's inner edge
    // img_width: whole image width
    int row, col;
    unsigned char *top_line, *cen_line, *bot_line;

    short V_left, V_right;//, D_out, D_inner;
    // if deltaI/I>contrast, can be simplified==> if deltaI*contrast_denominator > I*contrast_numerator
    int counter=0;

    for(row=1; row<img_height-1; row++)
    {
        top_line = src+ (row - 1)*img_width;
        cen_line = src+ (row    )*img_width;
        bot_line = src+ (row + 1)*img_width;

        for(col=1; col<img_width-1; col++)
        {
            V_left = *top_line + (*cen_line << 1) + *bot_line;
            V_right = *(top_line+2) + (*(cen_line+2) << 1) + *(bot_line+2);

            if( (V_left-V_right)>0 && (V_left-V_right)*contrast_denominator> V_right*contrast_numerator )
            {//downward
                //*(dst + row*img_width + col) = 255;
                *(dst + row*img_width + col) = (*(dst + row*img_width + col)) +1;//| 0x01;
                counter++;
            }
            else if((V_left-V_right)<0 && (V_right-V_left)*contrast_denominator> V_left*contrast_numerator)//for recording reverse gradient
            {//upward
                //*(dst + row*img_width + col) = 0;
                *(dst + row*img_width + col) = (*(dst + row*img_width + col)) +2;//| 0x10;
                counter++;
            }

            bot_line++;
            cen_line++;
            top_line++;            
        }//END for(col=1; col<img_width-1; col++)
    } // END for(row=1; row<img_height-1; row++)    
    edgeCounter=counter;
}
#endif

#ifdef _WIN32
void Filter2D::FLT_SOBEL_3x3_H(unsigned char *dst, unsigned char *src, int img_width, int img_height, int contrast_numerator, int contrast_denominator, int &edgeCounter)
{
    //unsigned char *dst, unsigned char *src
    //bit 2, 3:	00xx:plane								for sobel horizontal
    //				01xx:downward	1	2	1
    //				10xx:upward			0	0	0
    //				11xx: null				-1 -2 -1
    //direction: left--0; right--1; find the lane's inner edge
    // img_width: whole image width
    int row, col;
    unsigned char *top_line, *cen_line, *bot_line;

    short H_upper, H_lower;//, D_out, D_inner;
    // if deltaI/I>contrast, can be simplified==> if deltaI*contrast_denominator > I*contrast_numerator
    int counter=0;
    
    for(row=1; row<img_height-1; row++)
    {
        top_line = src+ (row - 1)*img_width;
        cen_line = src+ (row    )*img_width;
        bot_line = src+ (row + 1)*img_width;

       // *(dst + row*img_width ) = *cen_line;     // duplicate for the boundary, left-side
     
        for(col=1; col<img_width-1; col++)
        {
            H_upper = *(top_line-1)+ ((*top_line)<<1) + *(top_line+1) ;
            H_lower = *(bot_line-1)+ ((*bot_line)<<1) + *(bot_line+1) ;

            if( (H_upper-H_lower)>0 && (H_upper-H_lower)*contrast_denominator> H_lower*contrast_numerator )
            {
                //*(dst + row*img_width + col) = 255;
                *(dst + row*img_width + col) = (*(dst + row*img_width + col))|0x4  ;//+4;// 0100
                counter++;
            }
            else if((H_upper-H_lower)<0 && (H_lower-H_upper)*contrast_denominator> H_upper*contrast_numerator)//for recording reverse gradient
            {
                //*(dst + row*img_width + col) = 0;
                *(dst + row*img_width + col) =  (*(dst + row*img_width + col))|0x8  ;//+8;// 1000 
                counter++;
            }//else do nothing
            //else
            //{
            //		*(dst + row*img_width + col) = 128;
            //}

            bot_line++;
            cen_line++;
            top_line++;            
        }//END for(col=1; col<img_width-1; col++)
    } // END for(row=1; row<img_height-1; row++)    
    edgeCounter=counter;
}
#endif // #ifdef _WIN32

/**
* @brief 
        add ��tag�� image (size: 640*120), left-side is filled with 1, right-side is filled with 0
        designed by Mark 20140106
        Sobel 3x3 kernel
       [1   0  -1     or      -1  0   1
        2   0  -2             -2  0   2
        1   0  -1]            -1  0   1
    @n-------------------------------------------------
          V_left;  V_right
    float contrast_ratio: 0.10 ~ 0.15 (use numerator/denominator)
    255: right direction gradient, desired edge
    128: not an edge
    0: contrary direction gradient edge		 
*/
#ifdef _WIN32
void Filter2D::FLT_SOBEL_3x3_BDIR(unsigned char *dst, unsigned char *src, unsigned char *tag, int img_width, int img_height, unsigned short *numerator, unsigned short *denominator, int &edgeCounter)
{
    //direction: left--0; right--1; find the lane's inner edge
    // img_width: whole image width
    int row, col;
    unsigned char *top_line, *cen_line, *bot_line;
    short V_left, V_right, D_out, D_inner;
    int column_offset=-1;
    int copy_width=img_width;
    int counter=0;
    int contrast_numerator, contrast_denominator;
    column_offset=0;
    memcpy(dst, src+column_offset, copy_width);
    //memcpy(dst + (img_height-1)*img_width, src+(img_height-1)*img_width, img_width);
    memcpy(dst + (img_height-1)*copy_width, src+column_offset+(img_height-1)*img_width, copy_width);
    for(row=1; row<img_height-1; row++)
    {
        top_line = src+column_offset + (row - 1)*img_width;
        cen_line = src+column_offset + (row    )*img_width;
        bot_line = src+column_offset + (row + 1)*img_width;
        *(dst + row*copy_width ) = *cen_line;     // duplicate for the boundary, left-side
        for(col=1; col<copy_width-1; col++)
        {
            if (*(tag+col) == 1) { // left-side
                contrast_numerator   = numerator[0];
                contrast_denominator = denominator[0];
            }
            else {
                contrast_numerator   = numerator[1];
                contrast_denominator = denominator[1];
            }
            V_left = *top_line + (*cen_line << 1) + *bot_line;
            V_right = *(top_line+2) + (*(cen_line+2) << 1) + *(bot_line+2);
            if( (V_left-V_right)>0 && (V_left-V_right)*contrast_denominator> V_right*contrast_numerator )
            {
                *(dst + row*copy_width + col) = 255;
                counter++;
            }
            else if((V_left-V_right)<0 && (V_right-V_left)*contrast_denominator> V_left*contrast_numerator)//for recording reverse gradient
            {
                *(dst + row*copy_width + col) = 0;
                counter++;
            }
            else
            {
                if (*(tag+col) == 1) { // left-side
                    D_out=*cen_line+ ((*top_line)<<1)+(*(top_line+1));
                    D_inner=*(cen_line+2) + ((*(bot_line+2))<<1) + *(bot_line+1);
                    if(D_out>D_inner && (D_out-D_inner)*contrast_denominator> D_inner*contrast_numerator )
                    {
                        *(dst + row*copy_width + col) = 255;
                        counter++;
                    }
                    else if(D_out<D_inner && (D_inner-D_out)*contrast_denominator> D_out*contrast_numerator )
                    {
                        *(dst + row*copy_width + col) = 0;
                        counter++;
                    }
                    else
                        *(dst + row*copy_width + col) = 128;
                }
                else // *(tag+col) == 0, right-side
                {
                    D_out=*(cen_line+2) + ((*(top_line+2)) <<1)+(*(top_line+1));
                    D_inner= *cen_line + ( (*bot_line)<<1 ) + *(bot_line+1);
                    if(D_out>D_inner && (D_out-D_inner)*contrast_denominator> D_inner*contrast_numerator )
                    {
                        *(dst + row*copy_width + col) = 0;
                        counter++;
                    }
                    else if(D_out<D_inner && (D_inner-D_out)*contrast_denominator> D_out*contrast_numerator )
                    {
                        *(dst + row*copy_width + col) = 255;
                        counter++;
                    }
                    else
                        *(dst + row*copy_width + col) = 128;
                }
            }// end for 45-deg or 135-deg gradient
            bot_line++;
            cen_line++;
            top_line++;            
        }//END for(col=1; col<img_width-1; col++)
        *(dst + row*copy_width + col) = *(cen_line-1-(copy_width-column_offset));     // duplicate for the boundary, right-side
    } // END for(row=1; row<img_height-1; row++)    
    edgeCounter=counter;//just for reference, not used
}
#endif // #ifdef _WIN32

/**
* @brief Do Sobel Edge
* @note sobel_H = hSobel_img = 
* @n    [-1 -2 -1]
* @n    [+0 +0 +0]
* @n    [+1 +2 +1]
* @note sobel_V = vSobel_img = 
* @n    [+1 +0 -1]
* @n    [+2 +0 -2]
* @n    [+1 +0 -1]
*/
#ifdef _WIN32
void Filter2D::FLT_SOBEL_3x3_REGION( unsigned char *dst, unsigned char* vSobel_img, unsigned char* hSobel_img, unsigned char *src, int img_width, int img_height)
{
    short sobel_H, sobel_V;	
    short sum;
    int p[3][3];		
    int start_y, end_y;
    int x, y;
    int top_y, bot_y; 
    int left_x, right_x;    
    
    start_y = RegionL[1];
    end_y = RegionL[3];

    for(y=start_y; y<end_y; y++)
    {
        for (x=0; x<img_width; x++) 
        {
            top_y = y - 1;
            top_y = MAX(start_y, top_y);
            bot_y = y + 1;
            bot_y = MIN(end_y-1, bot_y);
            left_x = x - 1;
            left_x = MAX(0, left_x);
            right_x = x + 1;
            right_x = MIN(img_width-1, right_x);
                                    
            p[0][0] = src[top_y*img_width +  left_x];
            p[0][1] = src[top_y*img_width +       x];
            p[0][2] = src[top_y*img_width + right_x];
            p[1][0] = src[    y*img_width +  left_x];
            p[1][1] = src[    y*img_width +       x];
            p[1][2] = src[    y*img_width + right_x];
            p[2][0] = src[bot_y*img_width +  left_x];
            p[2][1] = src[bot_y*img_width +       x];
            p[2][2] = src[bot_y*img_width + right_x];

            sobel_H = (p[2][0] + (p[2][1]<<1) + p[2][2]) - (p[0][0] + (p[0][1]<<1) + p[0][2]);
            sobel_V = (p[0][0] + (p[1][0]<<1) + p[2][0]) - (p[0][2] + (p[1][2]<<1) + p[2][2]);

            sum = abs(sobel_H) + abs(sobel_V);	
            
            dst[y*img_width + x] = SAT8(sum);    // old version			
               
            vSobel_img[y*img_width + x] = SAT8(abs(sobel_V));
            hSobel_img[y*img_width + x] = SAT8(abs(sobel_H));				
        } // END for (x=0; x<img_width; x++) 
    } // END for(y=start_y; y<end_y; y++)
    
    /*
    INT32 row, col, rowidx;
    UCHAR *top_line, *cen_line, *bot_line;
    INT16 H_left, H_cen, H_right;
    INT16 V_left, V_cen, V_right;
    INT16 sobel_H, sobel_V;
    INT16 H_top_row, H_bot_row;

    //INT32 column_offset = -1;
    //INT32 copy_width    = img_width;
    //INT32 counter       = 0;
 
    for(row=RegionL[1]; row<RegionL[3]; ++row)
    {
        rowidx   = row * img_width;//ar center row
        top_line = src + rowidx - img_width;
        cen_line = src + rowidx;
        bot_line = src + rowidx + img_width;

        *(dst + row*img_width)=0;
        //left column
        H_left = (short)*bot_line - (short)*top_line;
        V_left = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;

        //shift to center column
        H_cen  = (short)*bot_line - (short)*top_line;
        V_cen = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;
        //shift to right column

        for(col=1; col<img_width-1; col++)
        {
            H_right = (short)*bot_line - (short)*top_line;
            H_top_row=(short)*(top_line-2)+(((short)*(top_line-1))<<1)+(short)*top_line;
            H_bot_row=(short)*(bot_line-2)+(((short)*(bot_line-1))<<1)+(short)*bot_line;;
            V_right = *top_line + (*cen_line << 1) + *bot_line;

            sobel_H = H_left + (H_cen << 1) + H_right;
            sobel_V = V_left - V_right;

            *(dst + row*img_width + col)= SAT8 (abs(sobel_H) + abs(sobel_V));
            *(vSobel_img + row*img_width + col)= SAT8 (abs(sobel_V));
            *(hSobel_img + row*img_width + col)= SAT8 (abs(sobel_H) );

            H_left = H_cen;
            H_cen = H_right;

            V_left = V_cen;
            V_cen = V_right;

            bot_line++;
            cen_line++;
            top_line++;
        }
        *(dst + row*img_width + col) = 0;
        *(vSobel_img + row*img_width + col)= 0;
        *(hSobel_img + row*img_width + col)= 0;
    } // END for(row=1; row<img_height-1; row++)  
    */
}
#endif // #ifdef _WIN32

/**
* @brief Get Hori/Vert derivative Sobel Map with specific mask  
* @vSobel, hSobel data type is INT16
* @note sobel_H = hSobel_img = 
* @n         [-1 +0 +1]
* @n   Gx =  [-2 +0 +2]
* @n         [-1 +0 +1]
* @note sobel_V = vSobel_img = 
* @n         [+1 +2 +1]
* @n   Gy =  [+0 +0 +0]
* @n         [-1 -2 -1]
*/
#ifdef _WIN32
void Filter2D::FLT_SobelMap( INT16* vSobelMap, INT16* hSobelMap, UCHAR *src,INT32 img_width, INT32 img_height )
{
    INT32 row, col, rowidx;
    UCHAR *top_line, *cen_line, *bot_line;
    INT16 H_left, H_cen, H_right;
    INT16 V_left, V_cen, V_right;
    INT16 sobel_H, sobel_V;
    
    INT16 startx = 1;
    INT16 endx   = img_width-1;
    INT16 starty = RegionL[1];
    INT16 endy   = RegionL[3];

    for( row =starty ; row < endy; ++row)
    {
        rowidx   = row * img_width;           
        top_line = src + rowidx - img_width;  ///< offsetVup
        cen_line = src + rowidx;              ///< offsetV
        bot_line = src + rowidx + img_width;  ///< offsetVdown

        //left column
        H_left = (short)*top_line - (short)*bot_line;
        V_left = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;

        //shift to center column
        H_cen  = (short)*top_line - (short)*bot_line;
        V_cen = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;
        //shift to right column

        for( col = startx; col<endx; col++)
        {
            H_right =  (short)*top_line -  (short)*bot_line;
            V_right = *top_line + (*cen_line << 1) + *bot_line;

            sobel_V = H_left + (H_cen << 1) + H_right;
            sobel_H = V_right - V_left;

            /// assign to Array
            *(hSobelMap + row*img_width + col)= SAT8(sobel_H);
            *(vSobelMap + row*img_width + col)= SAT8(sobel_V);

            H_left = H_cen;
            H_cen  = H_right;

            V_left = V_cen;
            V_cen  = V_right;

            bot_line++;
            cen_line++;
            top_line++;
        }
    } // END for(row=1; row<img_height-1; row++) 
}
#endif // #ifdef _WIN32

/**
* @brief Do Sobel Edge and find the magnitude and theta. (Verified)
* @note sobel_H = hSobel_img = 
* @n    [-1 -2 -1]
* @n    [+0 +0 +0]
* @n    [+1 +2 +1]
* @note sobel_V = vSobel_img = 
* @n    [+1 +0 -1]
* @n    [+2 +0 -2]
* @n    [+1 +0 -1]
* @param[in]	Sobel_Magnitude     (UCHAR)(|hSobel_img|+|vSobel_img|);       //0-255
* @param[in]	Sobel_Theta		    (CHAR) arctan(yGradient/xGradient)* 180 / PI ;  //-90~90 degree
*/
#ifdef _WIN32
void Filter2D::SobelEdge( UCHAR *Sobel_Magnitude, CHAR* Sobel_Theta, UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY )
{
    INT32 row, col, rowidx;
    UCHAR *top_line, *cen_line, *bot_line;
    INT16 H_left, H_cen, H_right;
    INT16 V_left, V_cen, V_right;
    INT16 sobel_H, sobel_V;
    INT16 H_top_row, H_bot_row;

    INT32 column_offset = -1;
    INT32 copy_width    = imgDimensionX;
    INT32 counter       = 0;
    const FLOAT PI      = 3.14159265;
    ///1) initial
    memset(Sobel_Magnitude,0,imgDimensionX*imgDimensionY);
    memset(Sobel_Theta    ,0,imgDimensionX*imgDimensionY);

    ///2) Sobel Edge Magnitude & theta
    for( row = 1 ; row < (imgDimensionY-1); ++row)
    {
        rowidx   = row * imgDimensionX;         //ar center row
        top_line = src + rowidx - imgDimensionX;
        cen_line = src + rowidx;
        bot_line = src + rowidx + imgDimensionX;

        //left column
        H_left = (short)*bot_line - (short)*top_line;     ///< [-1;0;1]
        V_left = *top_line + (*cen_line << 1) + *bot_line;///< [1;2;1]
        
        bot_line++;
        cen_line++;
        top_line++;

        //shift to center column
        H_cen  = (short)*bot_line - (short)*top_line;
        V_cen  = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;
        //shift to right column

        for(col=1; col<(imgDimensionX-1); ++col)
        {
            H_right   = (short)*bot_line - (short)*top_line;
            H_top_row = (short)*(top_line-2)+(((short)*(top_line-1))<<1)+(short)*top_line;
            H_bot_row = (short)*(bot_line-2)+(((short)*(bot_line-1))<<1)+(short)*bot_line;;
            V_right   = *top_line + (*cen_line << 1) + *bot_line;

            sobel_H   = H_left + (H_cen << 1) + H_right;
            sobel_V   = V_left - V_right;

            *(Sobel_Magnitude + row*imgDimensionX + col) = SAT8 (abs(sobel_H) + abs(sobel_V));
            //* TODO: �Фť�atan
            *(Sobel_Theta     + row*imgDimensionX + col) = (CHAR)(atan((float)sobel_H/(float)sobel_V)* 180 / PI); // �o�O�]��Verical/Horizontal �g�ϤF�GH/V�A��

            H_left = H_cen;
            H_cen  = H_right;

            V_left = V_cen;
            V_cen = V_right;

            bot_line++;
            cen_line++;
            top_line++;
        }
        *(Sobel_Magnitude + row*imgDimensionX + col) = 0 ;
        *(Sobel_Theta     + row*imgDimensionX + col) = 0 ;
    } // END for(row=1; row<img_height-1; row++)  
    return;
}
#endif // #ifdef _WIN32

/**
* @brief Do Laplacian Filter
* @note dst
* @n    [-1 -1 -1]
* @n    [-1 +8 -1]
* @n    [-1 -1 -1]
*/
#ifdef _WIN32
void Filter2D::FLT_Laplacian(unsigned char *dst,  unsigned char *src, int img_width, int img_height)
{
    /// initial
    memset(dst,0, img_width*img_height*sizeof(UCHAR));
    
    INT16 tmpIdx;
    INT16 tmp1;
    for(INT32 vert = 1 ; vert < (img_height-1); ++vert)
    {
        tmpIdx = vert * img_width;
        for(INT32 hori = 1 ; hori < (img_width-1) ; ++hori)
        {
            tmp1 =  src[hori+0 +(vert-1)*img_width ]+
                    src[hori+1 +(vert-1)*img_width ]+
                    src[hori+1 +(vert+0)*img_width ]+
                    src[hori+1 +(vert+1)*img_width ]+
                    src[hori+0 +(vert+1)*img_width ]+
                    src[hori-1 +(vert+1)*img_width ]+
                    src[hori-1 +(vert+0)*img_width ]+
                    src[hori-1 +(vert-1)*img_width ];

            dst[ hori + tmpIdx ] = SAT8( INT16(src[hori+tmpIdx ]<<3) - tmp1);
        }
    }
    return;  
}
#endif // #ifdef _WIN32

/**
* @brief Do LoG (Marr-Hildreth operator)
* @note dst
* @n    [+0 +0 -1 +0 +0]
* @n    [+0 -1 -2 -1 +0]
* @n    [-1 -2 +16-2 -1]
* @n    [+0 -1 -2 -1 +0]
* @n    [+0 +0 -1 +0 +0]
*/
#ifdef _WIN32
void Filter2D::FLT_Marr_Hildreth(unsigned char *dst,  unsigned char *src, int img_width, int img_height)
{
    /// initial
    memset(dst , 0, img_width*img_height*sizeof(UCHAR));

    INT16 tmpIdx;
    INT16 tmp1;
    for(INT32 vert = 2 ; vert < (img_height-2); ++vert)
    {
        tmpIdx = vert * img_width;
        for(INT32 hori = 2 ; hori < (img_width-2) ; ++hori)
        {
            tmp1 =  src[hori+0 +(vert-2)*img_width ]+
                src[hori-1 +(vert-1)*img_width ]+(src[hori+0 +(vert-1)*img_width ]<<1)+src[hori+1 +(vert-1)*img_width ]+
                src[hori-2 +(vert+0)*img_width ]+(src[hori-1 +(vert+0)*img_width ]<<1)+(src[hori+1 +(vert+0)*img_width ]<<1)+src[hori+2 +(vert+0)*img_width ]+
                src[hori-1 +(vert+1)*img_width ]+(src[hori+0 +(vert+1)*img_width ]<<1)+src[hori+1 +(vert+1)*img_width ]+
                src[hori+0 +(vert+2)*img_width ];

            dst[ hori + tmpIdx ] = SAT8( INT16(src[hori+tmpIdx ]<<4) - tmp1);
        }
    }
    return;  
}
#endif // #ifdef _WIN32

#ifdef _WIN32
void Filter2D::FLT_EdgeAdjustment(unsigned char *dst, int img_width, int img_height, int &edgeCounter)
{//bit 1, 0:		00:plane								for sobel vertical :0x0
    //				01:downward	1	0	-1										0x1
    //				10:upward			2	0	-2										0x2
    //				11: null				1	0	-1										0x3
    //bit 2, 3:	00xx:plane						for sobel horizontal:
    //				01xx:downward	1	2	1
    //				10xx:upward			0	0	0
    //				11xx: null				-1 -2 -1
    //bit 7= 0: left half image
    //bit 7= 1: right half image
    int row, col;
    unsigned char *img_ptr;
    int counter=0, tmpCounter=0;
    for(row=1; row<img_height-1; row++)
    {
        img_ptr= dst+row*img_width;
        for(col=1; col<img_width-1; col++)
        {
            img_ptr=img_ptr++;
            if( (*(img_ptr) & 0xf)==0)//plane: 0000; 1111(should be not happened
            {
                *(img_ptr)=128;
                tmpCounter++;
                continue;
            }
            if( (*(img_ptr)&0xf0)==0)//left
            {
                if( (*(img_ptr) & 0x3)==1 || (*(img_ptr) & 0xc)==0x4 )//1100; 0100
                    *(img_ptr)=255;
                else
                    *(img_ptr)=0;
            }
            else//right
            {
                if( (*(img_ptr) & 0x3)==0x2 || (*(img_ptr) & 0xc)==0x4 )//1100; 0100
                    *(img_ptr)=255;
                else
                    *(img_ptr)=0;
            }

            counter++;
        }//END for(col=1; col<img_width-1; col++)
    } // END for(row=1; row<img_height-1; row++)    
    edgeCounter=counter;
}
#endif // #ifdef _WIN32

/**
* @brief	  FLT_ABSDiff is Absolute value of 2 images and then pixel equals 255 if Value > Thd, 0 as Value < Thd
* @param[out] dst	Output Binary Image
* @param[in]  src1	Input SourceImage
* @param[in]  src2	Input SourceImage
* @param[in]  Thd	Threshold
* @Note     Eric:can run @ APEX
*/
#ifdef _WIN32
void Filter2D::FLT_ABSDiff(UCHAR *dst, UCHAR *src1 , UCHAR *src2, INT32 Thd ,int img_width, int img_height)
{
    /// the ROI from RegionL/R
    INT32 offsetV;
    INT16 pixel1,pixel2;
    UCHAR val;
    INT32 startY = RegionL[1];
    INT32 endY   = RegionL[3];
    
    for ( INT32 i1 = startY; i1 < endY ;++i1)
    {
        offsetV = img_width * i1 ;

        for ( INT32 i2 = 0; i2 < img_width ;++i2)
        {
            pixel1 = src1[offsetV+i2];
            pixel2 = src2[offsetV+i2];
            val = (ABS( (pixel1- pixel2) )>Thd )?(255):(0);
            dst[ offsetV + i2 ] = val ;
        }
    }
}
#endif // #ifdef _WIN32

/**
* @brief	  1)�Nsrc copy row = 0 ,end ; col = 0 ,end copy to dst 
              2)dst = 0 if src �|�P��0 , dst = 255 if src �|�P����
              
* @param[out] dst	Output Binary Image
* @param[in]  src	Input SourceImage
*/
#ifdef _WIN32
void Filter2D:: FLT_DILATION_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height)
{
    INT32  row, col;
    UCHAR *top_line, *cen_line, *bot_line;
    UINT32 sum;
    UINT32 tmpIdx;

    ///1) duplicate for boundary region
    memcpy(dst, src, img_width);                                                        ///< top side
    memcpy(dst + (img_height-1)*img_width, src+(img_height-1)*img_width, img_width);    ///< bottom side

    ///2) Dilate process
    for ( row = 1; row < (img_height-1) ; ++row )
    {
        top_line = src + (row - 1)*img_width;
        cen_line = src + (row    )*img_width;
        bot_line = src + (row + 1)*img_width;

        tmpIdx = row * img_width;
        *(dst + tmpIdx ) = *cen_line;     // duplicate for the boundary, left-side

        top_line++;
        cen_line++;
        bot_line++;
        for( col = 1 ; col<(img_width-1) ; ++col)
        {
            sum =   *(top_line - 1) + *top_line + *(top_line + 1) +
                    *(cen_line - 1) + *cen_line + *(cen_line + 1) +
                    *(bot_line - 1) + *bot_line + *(bot_line + 1) ;

            *(dst + tmpIdx + col) = (sum > 0)?(255):(0);

            top_line++;
            cen_line++;
            bot_line++;
        }

        *(dst + tmpIdx + col) = *(cen_line-1);     // duplicate for the boundary, right-side
    } // END for (row=1; row<img_height-1; row++)
}
#endif // #ifdef _WIN32

/**
* @brief         Image Erosion with 4-connected neighbor
* @n             ��h: mask��>1�I�����I�h�]�����I
* @param[in,out] src		input binary image and export eroded binary image
* @return        the number of output pixel with gray level = 255
*/
#ifdef _WIN32
INT32 FLT_EROSION_5(UCHAR *src, INT32 img_width, INT32 img_height)
{
    UCHAR *top_line, *cen_line, *bot_line;
    INT32 ret_counter = 0;
    CHAR  point;

    ///1) set the process ROI
    INT32 startx = 1;
    INT32 endx   = img_width-1;
    INT32 starty = 1;
    INT32 endy   = img_height-1;
    
    ///2) image scan and erosion
    for (INT32 row = starty; row < endy ; ++row)
    {
        top_line = src + (row - 1) * img_width;
        cen_line = top_line + img_width;
        bot_line = cen_line + img_width;

        for( INT32 col = startx; col < endx ; ++col)
        {
            if( 255 == *(cen_line))
            {
                point = 1; ///< itself
                if( 255 == *(top_line) )	point++;
                if( 255 == *(cen_line - 1) )point++;
                if( 255 == *(cen_line + 1))	point++;
                if( 255 == *(bot_line) )	point++;
                if (5 > point)
                {
                    *(src + row*img_width + col) = 0;
                }
                else
                    ++ret_counter;  ///< �O�ƫG�I
            }
            top_line++;
            cen_line++;
            bot_line++;
        } //end of col < endx
    } // END for (row=1; row<img_height-1; row++)
    return ret_counter; // ex. 300 means 300 pixels with 255 gray level 
}
#endif // #ifdef _WIN32

//erode >2 when row<=img_height/2
//erode >4 when row> img_height/2
#ifdef _WIN32
void Filter2D::FLT_EROSION_3x3(unsigned char *dst, unsigned char *src, int img_width, int img_height, unsigned int ori_coef)
{
    int row, col;
    unsigned char *top_line, *cen_line, *bot_line;
    //unsigned int sum;
    int counter=0;
    unsigned int coef=ori_coef, point;//=0;

    // duplicate 
    for(row=0; row<img_height; row++)
        memcpy(dst + row*img_width, src+row*img_width, img_width);    // 

    for (row=1; row<img_height-1; row++)
    {
        if(row>img_height*3/4-1)
            coef=ori_coef+3;
        else if(row>img_height/2-1)
            coef=ori_coef+2;
        else if(row>img_height/3-1)
            coef=ori_coef+1;

        top_line = src + (row - 1)*img_width;
        cen_line = src + (row    )*img_width;
        bot_line = src + (row + 1)*img_width;

        top_line++;
        cen_line++;
        bot_line++;
        //for(col=img_width/4; col<img_width*3/4; col++)
        for(col=1; col<img_width-1; col++)
        {
            //point=0;
            if( *(cen_line) ==255)
            {
                point=1;//itself
                if( *(top_line) ==255)	point++;
                if( *(cen_line - 1) ==255)	point++;
                if( *(cen_line + 1)==255)	point++;
                if( *(bot_line) ==255)	point++;
                if (point> coef)
                {
                    *(dst + row*img_width + col) = 255;
                    counter++;
                }
                else
                    *(dst + row*img_width + col) = 128;
            }
            else
            {
                if( *(cen_line) ==0)
                {
                    point=1;
                    if( *(top_line) ==0)	point++;
                    if( *(cen_line - 1) ==0)	point++;
                    if( *(cen_line + 1)==0)	point++;
                    if( *(bot_line) ==0)	point++;
                    if (point>coef)
                    {
                        *(dst + row*img_width + col) = 0;
                        counter++;
                    }
                    else
                        *(dst + row*img_width + col) = 128;
                }
                else
                    *(dst + row*img_width + col) = 128;
            }
            top_line++;
            cen_line++;
            bot_line++;
        }
    } // END for (row=1; row<img_height-1; row++)
}
#endif // #ifdef _WIN32

/**
* @brief	  Calculate HarrisValue map(o_harrisValue)
* @param[out] o_harrisValue		output Harris map ( R=det(M)-K(trace(M))^2 )
* @param[in]  i_h_value			Input g_hSobel_img
* @param[in]  i_v_value			Input g_vSobel_img
* @NOTE		  dst_width=src_width=hv_width, dst_height=src_height=hv_height
              ROI = {0,0,SrcImg_W,SrcImg_H}
*/
#ifdef _WIN32
void Filter2D::CalculateHarrisValue(UINT16 *o_harrisValue, UCHAR *i_h_value, UCHAR *i_v_value, INT32 DimensionX, INT32 DimensionY)
{
    
    const int K_shift         = 7;    ///< 0.008 = 1.0/128;
    const int harris_width    = 5;    ///< calculate harris value of each block from harris_width*harris_height pixels 
    const int harris_height   = 5;
    const int FinalShiftBIT   = 10;
    const int ElementShiftBit = 6;    //must >=6 for APEX
    INT32 M[2][2];
    INT32 Tr;
    INT32 result;                     ///< Harris corner response
    INT32 start_y, end_y;
    int x, y; 
    int bx, by;
    INT32 i, j;
    int idx;
    
    start_y = RegionL[1];
    end_y   = RegionL[3];
    
    memset(o_harrisValue, 0, DimensionX*DimensionY*sizeof(UINT16));
    for (y=start_y; y<end_y; y++) {
        for (x=0; x<DimensionX; x++) {
            M[0][0] = M[0][1] = M[1][0] = M[1][1] = 0;
            
            for (i=-(harris_height/2); i<=(harris_height/2); i++) {
                for (j= -(harris_width/2); j<=(harris_width/2); j++) {

                    by = MAX_MIN_RANGE((y + i),start_y,(end_y-1));
                    bx = MAX_MIN_RANGE((x + j),0,(DimensionX-1));

                    // old version
                    //by = y + i;
                    //bx = x + j;
                    //by = MAX(0, MIN(by, DimensionY-1));
                    //bx = MAX(0, MIN(bx, DimensionX-1));
                    
                    idx = by * DimensionX + bx;

                    M[0][0] += (((int)i_h_value[idx]*i_h_value[idx])>>ElementShiftBit);
                    M[0][1] += (((int)i_h_value[idx]*i_v_value[idx])>>ElementShiftBit);
                    M[1][1] += (((int)i_v_value[idx]*i_v_value[idx])>>ElementShiftBit);	                
                } // END for (j= -harris_width/2; j<harris_width/2; j++) {
            } // END for (i=-harris_height/2; i<harris_height/2; i++) {
            
            M[1][0]  = M[0][1];
            Tr       = M[0][0] + M[1][1];
            result   = (INT32)( M[0][0]*M[1][1]-M[0][1]*M[1][0] - ((Tr*Tr)>>K_shift) );
            result >>= FinalShiftBIT;//result may be minus. be careful about shift bit   
            
            o_harrisValue[y*DimensionX + x] = SAT16(result);
        } // END for (x=0; x<DimensionX; x++) {
    } // END for (y=start_y; y<end_y; y++)

}

#endif // #ifdef _WIN32

#ifdef _WIN32

/**
* @brief	  improved HarrisValue map(o_validFP)
* @param[out] o_validFP		    output improved Harris map(0: Not Corner 1: Corner)
* @param[in]  i_harris_value	Input Harris map(Harris Value ex.200,800,3000)
* @param[in]  i_v_value			Input g_vSobel_img
* @NOTE		  dst_width=src_width=hv_width, dst_height=src_height=hv_height
              ROI = {0,0,SrcImg_W,SrcImg_H}
*/
void Filter2D::SupressNonMaxCornerRemoveSimilar(UCHAR *o_validFP, UINT16 *i_harris_value, UCHAR *Gaussian, INT32 DimensionX, INT32 DimensionY, const int TH, const UCHAR e_value)
{
    const INT16 similar_th= 10;
    int peak;
    int x, y;
    int i, j;
    int bx, by;
    int center;
    UCHAR centerGrayValue;
    int counter;
    int start_y, end_y;
    
    start_y = RegionL[1];
    end_y   = RegionL[3];
    for(y=start_y; y<end_y; y++) {

    for (x=0; x<DimensionX; x++)  {           
        
        /// Harris Value THD
        center          = i_harris_value[y*DimensionX + x];
        centerGrayValue = Gaussian[y*DimensionX + x];

        if (center <= TH)       // change from '<' to '<=' for APEX porting
        {
            o_validFP[y*DimensionX + x] = 0;
            continue;
        }
        
        peak = 0;
        counter=0;
               
        for (i=-1; i<=1; i++) {
        for (j=-1; j<=1; j++) {

            by = MAX_MIN_RANGE( (y + i),start_y,end_y-1);
            bx = MAX_MIN_RANGE( (x + j),0,DimensionX);

            /* old version
            by = y + i;
            bx = x + j;
            by = MAX(0, MIN(end_y, by));
            bx = MAX(0, MIN(end_x, bx));
            */

            if ( i_harris_value[ by * DimensionX + bx ] <= center)
                peak++;
            if(ABS((INT16)centerGrayValue - (INT16)Gaussian[by*DimensionX + bx])>=similar_th)//shoud use gaussian
                counter++;
        } }

        if (peak > 8)
            o_validFP[y*DimensionX + x] = e_value;  //add to feature point map
        else            
            o_validFP[y*DimensionX + x] = 0;    

        if(counter<2 || counter>6)
            o_validFP[y*DimensionX + x] = 0;

        
    } } // END for(y=start_y; y<end_y; y++) {
}
#endif // #ifdef _WIN32


#ifdef _WIN32
/**
* @brief	  improved HarrisValue map(o_validFP)
* @param[out] o_validFP		    output improved Harris map(0: Not Corner 1: Corner)
* @param[in]  i_harris_value	Input Harris map(Harris Value ex.200,800,3000)
* @param[in]  i_v_value			Input g_vSobel_img
* @NOTE		  dst_width=src_width=hv_width, dst_height=src_height=hv_height
              ROI = {0,0,SrcImg_W,SrcImg_H}
*/
void Filter2D::SupressNonMaxRemoveStill(UCHAR *o_validFP, UINT16 *i_harris_value, UCHAR *Gaussian, UCHAR *DifBin, 
                                        INT32 DimensionX, INT32 DimensionY, const int TH, const UCHAR e_value)
{
    const INT16 similar_th= 10;
    int peak;
    int x, y;
    int i, j;
    int bx, by;
    int center;
    UCHAR centerGrayValue;
    int counter;
    int start_y, end_y;
    UCHAR mask, result;
    
    start_y = RegionL[1];
    end_y   = RegionL[3];
    for(y=start_y; y<end_y; y++) {

    for (x=0; x<DimensionX; x++)  {           
        
        /// Harris Value THD
        center          = i_harris_value[y*DimensionX + x];
        centerGrayValue = Gaussian[y*DimensionX + x];

        if (center <= TH)       // change from '<' to '<=' for APEX porting
        {
            o_validFP[y*DimensionX + x] = 0;
            continue;
        }
        
        peak = 0;
        counter=0;
               
        for (i=-1; i<=1; i++) {
        for (j=-1; j<=1; j++) {

            by = MAX_MIN_RANGE( (y + i),start_y,end_y-1);
            bx = MAX_MIN_RANGE( (x + j),0,DimensionX);

            if ( i_harris_value[ by * DimensionX + bx ] <= center)
                peak++;
            if(ABS((INT16)centerGrayValue - (INT16)Gaussian[by*DimensionX + bx])>=similar_th)//shoud use gaussian
                counter++;
        } }

        if (peak > 8)
            result = e_value;  //add to feature point map
        else            
            result = 0;    

        mask = DifBin[y*DimensionX + x];
        if(counter<2 || counter>6)
            mask = 0;

        result = result & mask;

        o_validFP[y*DimensionX + x] = result;        
    } } // END for(y=start_y; y<end_y; y++) {
}
#endif // #ifdef _WIN32


void Filter2D::CntQty16x16(UINT16 *BlockCount, UCHAR *DifBin, int img_width, int img_height, 
 		 		           UINT16 start_x, UINT16 start_y, UINT16 end_x, UINT16 end_y, unsigned char threshold)
{
    int dst_width, dst_height;
    int i, j;
    int x, y;
    UINT16 count;    

    dst_width = (end_x - start_x) / 16;
    dst_height = (end_y - start_y) / 16;

    for (i=0; i<dst_height; i++) {
        for (j=0; j<dst_width; j++) {
            count = 0;
            for (y=0; y<16; y++) {
                for (x=0; x<16; x++) {
                    if (DifBin[(start_y + i*16+y)*img_width + start_x + (j*16)+x] > threshold)
                        count++;

                }
            }
            BlockCount[i * dst_width + j] = count;
        }
    }
    
}


#ifdef _WIN32
#if 0   // obsolete
void Filter2D::Apex_CalculateHarrisValue( UINT16 *o_harrisMap, UCHAR *Sobel_img, UCHAR* i_h_value,UCHAR* i_v_value, UCHAR *src, INT32 DimensionX, INT32 DimensionY, INT32 TH)
{
    //======================================
    const double K            = 0.08;   //0.11;//0.12;//0.14;//0.08;///< Harris K //0.1;//0.08;//1.0/128;//generally, 0.06~0.08 
    const int harris_width    = 3;      //calculate harris value of each block from harris_width*harris_height pixels 
    const int harris_height   = 3;      //5, harris_width=5
    const int FinalShiftBIT   = 10;
    const int ElementShiftBit = 5;
    //======================================
    unsigned char similar_th  = 10;     //15;//10
    int M00, M10, M01, M11;
    int Tr;
    int result;
    int dx,dy, i, j, y_offset;
    unsigned char  *src_h_idx, *src_v_idx ; //*src_sobel_idx;
    unsigned short *opHarrisTmp;
    //int debug_saved_max_value = 0;

    //INT32 Idx_harrisCorners = 0;
    INT32 starty            = RegionL[1] + 1;
    INT32 endy              = RegionL[3] + 1;

    memset(o_harrisMap   , 0 , DimensionX*DimensionY*sizeof(unsigned short) );

    /// scan all pixel
    for( dy = starty; dy < endy; ++dy )
    {
        src_h_idx   = i_h_value  +dy*DimensionX + 1;
        src_v_idx   = i_v_value  +dy*DimensionX + 1;
        //src_sobel_idx= Sobel_img +dy*DimensionX+ROI->start_col + 1;
        
        opHarrisTmp	= o_harrisMap+dy*DimensionX + 1;
        
        for(dx = 1;  dx < (DimensionX-1); ++dx)
        {
            M00 = 0, M01 = 0 , M10 = 0, M11 = 0;

            /// get the M matrix near each pixel
            for( j = -harris_height/2 ; j < harris_height/2 ; ++j)
            {
                y_offset = j * DimensionX;
                for( i= -harris_width/2; i < harris_width/2; ++i)
                {
                    M00 += (int)(*(src_h_idx+y_offset+i))*(*(src_h_idx+y_offset+i));
                    M01 += (int)(*(src_h_idx+y_offset+i))*(*(src_v_idx+y_offset+i));
                    M11 += (int)(*(src_v_idx+y_offset+i))*(*(src_v_idx+y_offset+i));	
                }
            }
            M10    = M01;
            Tr     = M00 + M11 ;
            result = (int)( M00 * M11 - M01 * M10 - K * Tr * Tr ); ///< det(M)-K(trace(M))^2

            if (result < TH)
                result = 0;

            *opHarrisTmp = (unsigned short)SAT16(result);

            src_h_idx++;
            src_v_idx++;
            //src_sobel_idx++;
            opHarrisTmp++;
        }
    }// end of [dy < ROI->start_row + ROI->height-1]

    //===================================================
    //improved Harris, use gray level similarity
    //===================================================
    
    unsigned char *comparison;
    unsigned char k_value = 0;
    int counter           = 0, peak=0;
    opHarrisTmp           = o_harrisMap;

    for( dy = starty ; dy < endy; ++dy)
    {
        for( dx = 1 ; dx < DimensionX-1; ++dx)
        {
            if( *( opHarrisTmp + dy*DimensionX+dx ) == 0 )
                continue;

            counter    = 0;  
            peak       = 0;
            k_value    = *(src+dy*DimensionX+dx);
            comparison =   src+dy*DimensionX+dx;

            for( j = -harris_height/2; j < harris_height/2; ++j)
            {
                
                y_offset = j*DimensionX;
                for( i =  -harris_width/2; i < harris_width/2; ++i)
                {
                    if( abs( k_value-*(comparison+y_offset+i) )>=similar_th )
                        counter++;
                    if(k_value>=*(comparison+y_offset+i))
                        peak++;
                }
            }
            if( counter<2 || counter>6 || peak<4 )
                *(opHarrisTmp+dy*DimensionX+dx)=0;
            else
            {
                /* remove to outside function
                /// add the save dx dy as Harris corner
                //Left
                if( (dx>= RegionL[0]) && (dx< RegionL[2]) && (dy>= RegionL[1]) && (dy < RegionL[3]))
                {
                    harrisCorners[Idx_harrisCorners].x = dx;
                    harrisCorners[Idx_harrisCorners].y = dy;
                    Idx_harrisCorners++;
                }
                //Right
                else if( (dx>= RegionR[0]) && (dx< RegionR[2]) && (dy>= RegionR[1]) && (dy < RegionR[3]))
                {
                    harrisCorners[Idx_harrisCorners].x = dx;
                    harrisCorners[Idx_harrisCorners].y = dy;
                    Idx_harrisCorners++;
                }
                */
                //debug_saved_max_value++;
            }
        }
    }
}
#endif // #if 0
#endif // #ifdef _WIN32

/**
* @brief            Image Binarization with const threshold 
* @param[out]       g_bin_img       Output Binary image
* @param[in]        Gaussian_img  The input gray image 
* @param[in]        THD_Binary      the Threshold[0-255]
*/
#ifdef _WIN32
void Filter2D::ImageThreshold_01( UCHAR *g_bin_img, CONST UCHAR *Gaussian_img, INT32 imgDimensionX, INT32 imgDimensionY, UCHAR THD_Binary )
{
    UINT16 startx = 0;
    UINT16 endx   = imgDimensionX;
    UINT16 starty = RegionL[1];
    UINT16 endy   = RegionR[3];

    INT32 tmp01;
    for( INT32 vert = starty ; vert < endy ; ++vert)
    {
        tmp01 =  vert * imgDimensionX;
        for( INT32 hori = startx ; hori < endx ; ++hori)
            g_bin_img[ hori + tmp01 ] = ( Gaussian_img[ hori + tmp01 ] > THD_Binary)? 255 : 0 ; // suitable for APEX porting
    }
}
#endif // #ifdef _WIN32

/**
* @brief            Histogram Equalization is a contrast-adjustment method (Verified)
* @param[in,out]    Img  Image
* @ref     http://en.wikipedia.org/wiki/Histogram_equalization
*/
#ifdef _WIN32
void Filter2D::HistogramEq( UCHAR *Img ,INT32 imgDimensionX,INT32 imgDimensionY )
{
    ///1) initilization
    INT32  length = imgDimensionX*imgDimensionY;
    UINT32 bins[256] = {0};

    ///2) pdf
    for (INT32 count1 = 0 ; count1<length ; ++count1)
        bins[Img[ count1 ]] ++;

    ///3) normalized-cdf
    for (INT32 i1 = 1 ; i1<256 ; ++i1)
        bins[ i1 ] += bins[ i1-1 ];

    FLOAT tmp1;
    for (INT32 i1 = 0 ; i1<256 ; ++i1)
    {
        tmp1 = (FLOAT)bins[ i1 ] / (FLOAT)bins[ 255 ] * (FLOAT)255;
        bins[ i1 ] = (UCHAR)tmp1;                                   ///normalized cdf [value=0-255]
    }
    ///4) processing
    for (INT32 count1 = 0 ; count1<length ; ++count1)
        Img[ count1 ] = (UCHAR)bins[ Img[ count1 ] ];

    return;
}
#endif // #ifdef _WIN32

#ifdef _WIN32
static int trace (int i, int j, int low, IMAGE& im,IMAGE& mag, IMAGE& ori);
static float gauss(float x, float sigma);
static float dGauss (float x, float sigma);
static float meanGauss (float x, float sigma);
static void hysteresis (int high, int low, IMAGE& im, IMAGE& mag, IMAGE& oriim);
static void canny (float s, IMAGE& im, IMAGE& mag, IMAGE& ori);
static void seperable_convolution (IMAGE& im, float *gau, int width,
        f2D& smx, f2D& smy);
static void dxy_seperable_convolution (f2D& im, int nr, int nc, float *gau,
        int width, f2D& sm, int which);
static void nonmax_suppress (f2D& dx, f2D& dy, int nr, int nc,
        IMAGE& mag, IMAGE& ori);
static void estimate_thresh (IMAGE& mag, int *low, int *hi);
static int range (IMAGE& im, int i, int j);


static float norm (float x, float y)
{
    return (float) sqrt ( (double)(x*x + y*y) );
}

static void canny (float s, IMAGE& im, IMAGE& mag, IMAGE& ori)
{
    int width;
    f2D smx, smy, dx, dy;
    int i,j,n;
    float gau[MAX_MASK_SIZE], dgau[MAX_MASK_SIZE], z;

/* Create a Gaussian and a derivative of Gaussian filter mask */
    for(i=0; i<MAX_MASK_SIZE; i++)
    {
      gau[i] = meanGauss ((float)i, s);
      if (gau[i] < 0.005)
      {
        width = i;
        break;
      }
      dgau[i] = dGauss ((float)i, s);
    }

    n = width+width + 1;
    WIDTH = width/2;
    //cout<<"Smoothing with a Gaussian ="<<n<<endl;

    smx.Initialize(im.nr, im.nc);
    smy.Initialize(im.nr, im.nc);

/* Convolution of source image with a Gaussian in X and Y directions  */
    seperable_convolution (im, gau, width, smx, smy);

/* Now convolve smoothed data with a derivative */
    //cout<<"Convolution with the derivative of a Gaussian...\n";
    dx.Initialize(im.nr, im.nc);
    dxy_seperable_convolution (smx, im.nr, im.nc,
         dgau, width, dx, 1);
    smx.Finish();

    dy.Initialize(im.nr, im.nc);
    dxy_seperable_convolution (smy, im.nr, im.nc,
         dgau, width, dy, 0);
    smy.Finish();

/* Create an image of the norm of dx,dy */
    for (i=0; i<im.nr; i++)
      for (j=0; j<im.nc; j++)
      {
          z = norm (dx.m[i][j], dy.m[i][j]);
          mag.m[i][j] = (unsigned char)(z*MAG_SCALE);
      }

/* Non-maximum suppression - edge pixels should be a local max */

    nonmax_suppress (dx, dy, (int)im.nr, (int)im.nc, mag, ori);

    dx.Finish();dy.Finish();
}

/*      Gaussian        */
static float gauss(float x, float sigma)
{
    float xx;

    if (sigma == 0) return 0.0;
    xx = (float)exp((double) ((-x*x)/(2*sigma*sigma)));
    return xx;
}

static float meanGauss (float x, float sigma)
{
    float z;

    z = (gauss(x,sigma)+gauss(x+0.5,sigma)+gauss(x-0.5,sigma))/3.0;
    z = z/(3.14159*2.0*sigma*sigma);
    return z;
}

/*      First derivative of Gaussian    */
static float dGauss (float x, float sigma)
{
    return -x/(sigma*sigma) * gauss(x, sigma);
}

/*      HYSTERESIS thersholding of edge pixels. Starting at pixels with a
    value greater than the HIGH threshold, trace a connected sequence
    of pixels that have a value greater than the LOW threhsold.        */

static void hysteresis (int high, int low, IMAGE& im, IMAGE& mag, IMAGE& oriim)
{
    int i,j;//,k;

    //cout<<"Beginning hysteresis thresholding...\n";
    for (i=0; i<im.nr; i++)
      for (j=0; j<im.nc; j++)
        im.m[i][j] = 0;

    if (high<low)
    {
      estimate_thresh (mag, &high, &low);
//	  printf ("Hysteresis thresholds (from image): HI %d LOW %D\n",high, low);
    }
/* For each edge with a magnitude above the high threshold, begin
   tracing edge pixels that are above the low threshold.                */

    for (i=0; i<im.nr; i++)
      for (j=0; j<im.nc; j++)
        if (mag.m[i][j] >= high)
          trace (i, j, low, im, mag, oriim);

/* Make the edge black (to be the same as the other methods) */
    for (i=0; i<im.nr; i++)
      for (j=0; j<im.nc; j++)
        if (im.m[i][j] == 0) im.m[i][j] = 255;
        else im.m[i][j] = 0;
}

/*      TRACE - recursively trace edge pixels that have a threshold > the low
    edge threshold, continuing from the pixel at (i,j).                     */

static int trace (int i, int j, int low, IMAGE& im,IMAGE& mag, IMAGE& ori)
{
    int n,m;
    char flag = 0;

    if (im.m[i][j] == 0)
    {
      im.m[i][j] = 255;
      flag=0;
      for (n= -1; n<=1; n++)
      {
        for(m= -1; m<=1; m++)
        {
          if (i==0 && m==0) continue;
          if (range(mag, i+n, j+m) && mag.m[i+n][j+m] >= low)
        if (trace(i+n, j+m, low, im, mag, ori))
        {
            flag=1;
            break;
        }
        }
        if (flag) break;
      }
      return(1);
    }
    return(0);
}

static void seperable_convolution (IMAGE& im, float *gau, int width,
        f2D& smx, f2D& smy)
{
    int i,j,k, I1, I2, nr, nc;
    float x, y;

    nr = im.nr;
    nc = im.nc;

    for (i=0; i<nr; i++)
      for (j=0; j<nc; j++)
      {
        x = gau[0] * im.m[i][j]; y = gau[0] * im.m[i][j];
        for (k=1; k<width; k++)
        {
          I1 = (i+k)%nr; I2 = (i-k+nr)%nr;
          y += gau[k]*im.m[I1][j] + gau[k]*im.m[I2][j];
          I1 = (j+k)%nc; I2 = (j-k+nc)%nc;
          x += gau[k]*im.m[i][I1] + gau[k]*im.m[i][I2];
        }
        smx.m[i][j] = x; smy.m[i][j] = y;
      }
}

static void dxy_seperable_convolution (f2D& im, int nr, int nc,  float *gau,
        int width, f2D& sm, int which)
{
    int i,j,k, I1, I2;
    float x;

    for (i=0; i<nr; i++)
      for (j=0; j<nc; j++)
      {
        x = 0.0;
        for (k=1; k<width; k++)
        {
          if (which == 0)
          {
        I1 = (i+k)%nr; I2 = (i-k+nr)%nr;
        x += -gau[k]*im.m[I1][j] + gau[k]*im.m[I2][j];
          }
          else
          {
        I1 = (j+k)%nc; I2 = (j-k+nc)%nc;
        x += -gau[k]*im.m[i][I1] + gau[k]*im.m[i][I2];
          }
        }
        sm.m[i][j] = x;
      }
}

static void nonmax_suppress (f2D& dx, f2D& dy, int nr, int nc,
        IMAGE& mag, IMAGE& ori)
{
    int i,j;
//	int top, left, right;
    float xx, yy, g2, g1, g3, g4, g, xc, yc;

    for (i=1; i<mag.nr-1; i++)
    {
      for (j=1; j<mag.nc-1; j++)
      {
        mag.m[i][j] = 0;

/* Treat the x and y derivatives as components of a vector */
        xc = dx.m[i][j];
        yc = dy.m[i][j];
        if (fabs(xc)<0.01 && fabs(yc)<0.01) continue;

        g  = norm (xc, yc);

/* Follow the gradient direction, as indicated by the direction of
   the vector (xc, yc); retain pixels that are a local maximum. */

        if (fabs(yc) > fabs(xc))
        {

/* The Y component is biggest, so gradient direction is basically UP/DOWN */
          xx = fabs(xc)/fabs(yc);
          yy = 1.0;

          g2 = norm (dx.m[i-1][j], dy.m[i-1][j]);
          g4 = norm (dx.m[i+1][j], dy.m[i+1][j]);
          if (xc*yc > 0.0)
          {
        g3 = norm (dx.m[i+1][j+1], dy.m[i+1][j+1]);
        g1 = norm (dx.m[i-1][j-1], dy.m[i-1][j-1]);
          } else
          {
        g3 = norm (dx.m[i+1][j-1], dy.m[i+1][j-1]);
        g1 = norm (dx.m[i-1][j+1], dy.m[i-1][j+1]);
          }

        } else
        {

/* The X component is biggest, so gradient direction is basically LEFT/RIGHT */
          xx = fabs(yc)/fabs(xc);
          yy = 1.0;

          g2 = norm (dx.m[i][j+1], dy.m[i][j+1]);
          g4 = norm (dx.m[i][j-1], dy.m[i][j-1]);
          if (xc*yc > 0.0)
          {
        g3 = norm (dx.m[i-1][j-1], dy.m[i-1][j-1]);
        g1 = norm (dx.m[i+1][j+1], dy.m[i+1][j+1]);
          }
          else
          {
        g1 = norm (dx.m[i-1][j+1], dy.m[i-1][j+1]);
        g3 = norm (dx.m[i+1][j-1], dy.m[i+1][j-1]);
          }
        }

/* Compute the interpolated value of the gradient magnitude */
        if ( (g > (xx*g1 + (yy-xx)*g2)) &&
         (g > (xx*g3 + (yy-xx)*g4)) )
        {
          if (g*MAG_SCALE <= 255)
        mag.m[i][j] = (unsigned char)(g*MAG_SCALE);
          else
        mag.m[i][j] = 255;
          ori.m[i][j] = atan2 (yc, xc) * ORI_SCALE;
        } else
        {
        mag.m[i][j] = 0;
        ori.m[i][j] = 0;
        }

      }
    }
}

static void estimate_thresh (IMAGE& mag, int *hi, int *low)
{
    int i,j,k, hist[256], count;

/* Build a histogram of the magnitude image. */
    for (k=0; k<256; k++) hist[k] = 0;

    for (i=WIDTH; i<mag.nr-WIDTH; i++)
      for (j=WIDTH; j<mag.nc-WIDTH; j++)
        hist[mag.m[i][j]]++;

/* The high threshold should be > 80 or 90% of the pixels
    j = (int)(ratio*mag.nr*mag.nc);
*/
    j = mag.nr;
    if (j<mag.nc) j = mag.nc;
    j = (int)(0.9*j);
    k = 255;

    count = hist[255];
    while (count < j)
    {
      k--;
      if (k<0) break;
      count += hist[k];
    }
    *hi = k;

    i=0;
    while (hist[i]==0) i++;

    *low = (*hi+i)/2.0;
}

static int range (IMAGE& im, int i, int j)
{
    if ((i<0) || (i>=im.nr)) return 0;
    if ((j<0) || (j>=im.nc)) return 0;
    return 1;
}

/**
* @brief            Canny edge (verified)
* @param[out]    Canny_img  canny Image
* @ref     http://140.115.11.235/~chen/course/vision/ch6/ch6.htm
*/
void Filter2D::CannyEdge( UCHAR *Canny_img , UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY )
{

    /// 3 adjustable parameters : Lower threshold(low), High threshold(high), Gaussian standard deviation(sigma)
    // (1) Defaut value for Lower threshold, High threshold, Gaussian standard deviation
    int low    =5; //50
    int high   =150; //-5
    float sigma=1.0; //1.0
    
    ///1) initial
    int i,j;//,k,n;
    IMAGE im, magim, oriim;
    //cout<<"CANNY: Apply the Canny edge detector to an image.\n";

    ///2) copy to uc2D type
    im.Initialize(imgDimensionY,imgDimensionX);
    for(int i=0 , counter1 = 0;i<im.nr;i++)for(int j=0;j<im.nc;j++)im.m[i][j]=src[counter1++];

    // Create local image space 
    magim.Initialize(im.nr, im.nc);
    oriim.Initialize(im.nr, im.nc);

    // Apply the filter
    //cout<<"canny edge detection...\n";
    canny(sigma, im, magim, oriim);

    // Hysteresis thresholding of edge pixels 
    //cout<<"Hysteresis thresholding\n";
    hysteresis (high, low, im, magim, oriim);

    // Compensation of edge region of image(�Ĥ@��Ĥ@�C���)
    for (i=0; i<WIDTH; i++) for (j=0; j<im.nc; j++) im.m[i][j] = 255;
    for (i=im.nr-1; i>im.nr-1-WIDTH; i--) for (j=0; j<im.nc; j++) im.m[i][j] = 255;
    for (i=0; i<im.nr; i++) for (j=0; j<WIDTH; j++) im.m[i][j] = 255;
    for (i=0; i<im.nr; i++) for (j=im.nc-WIDTH-1; j<im.nc; j++) im.m[i][j] = 255;

    // output the image to Canny_img
    for(int i=0 , counter2 = 0;i<im.nr;i++)for(int j=0;j<im.nc;j++) Canny_img[counter2++] = im.m[i][j];

}
#endif // #ifdef _WIN32

Filter2D* Filter2D::getInstance()
{
    if(!instance)
        instance = new Filter2D;
    return instance;
}

Filter2D::Filter2D(void)
{
}
Filter2D::~Filter2D(void)
{
}
void Filter2D::ResetInstance()
{
    delete instance; 
    instance = NULL;
}