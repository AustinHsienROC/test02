//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
  #include "stdafx.h"
  #include "WhiteBox.h"
#else
  #include "GlobalTypes.h"
#endif
#include <string.h>
#include "CTA_Type.h"
#include "Point_Track.h"
#include <math.h>

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
typedef enum ENUM_INT8_T {Alg_FullSearch , Alg_Hexagon}MV_Alg;
#define MATH_PI	 (3.14159265358979323846)


Point_Track* Point_Track::instance = NULL;

//******************************************************************************
// I N T E R N A L  F U N C T I O N
//******************************************************************************
static INT32 ME_GetSAD(const unsigned char* msk1,const unsigned char* msk2,const int size,const int shiftBit);
static INT32 FindMinIdx(int* src,int length,int TH);
static INT32 FindMinIdx(int* src,int length);

#define _2x2_WIDTH          (SrcImg_W>>1)
#define _2x2_PROCESS_HEIGHT (SrcImg_H>>1)
#define _4x4_WIDTH          (SrcImg_W>>2)
#define _4x4_PROCESS_HEIGHT (SrcImg_H>>2)

/**
* @brief		 Fill the msk from img data
* @param[in] s_x	mask centerX
* @param[in] s_y	mask centerY
* @param[in] img	Source image
* @return	 isSuccessful(Fail: the mask is out of SourceImage)			
*/
bool Point_Track::ME_FillMask( UCHAR* msk,const INT32 s_x, const INT32 s_y,const INT32 mask_width,const INT32 mask_height,const UCHAR* img,const INT32 img_width,const INT32 img_height)
{
    INT32 i,j,p;
	INT32 idx;
    CONST INT32 stry  = s_y - (mask_height>>1); ///< left-top point
    CONST INT32 strx  = s_x - (mask_width >>1);
    INT32 str_idx     = stry * img_width + strx;

	CONST INT32 endy = (stry + mask_height);
	CONST INT32 endx = (strx + mask_width );

    /// check if out of boundary
    if((strx<0) || (stry<0) || (endx>=img_width) || (endy>=img_height) )return false;
    
    /// fill the image data into mask
    for( p = 0 , j = stry  ; j<endy  ; ++j , str_idx += img_width)
    {
        for( i = strx , idx = str_idx ; i<endx ; ++i , ++p, ++idx)
        {
            msk[p]=img[idx];
        }
    }
    return true;
}

/**
* @brief     Fill the msk from img data
* @param[in] coord	mask centerX , and centerY
* @param[in] img	Source image
* @return	 isSuccessful(Fail: the mask is out of SourceImage)			
*/
bool Point_Track::ME_FillMask(UCHAR* msk,Coordinate_t& coord,INT32 m_width,INT32 m_height,const UCHAR* img)
{
    return ME_FillMask(msk,coord.x,coord.y,m_width,m_height,img);
}

/**
* @brief     SAD Calculation between 2 masks
* @param[in] msk1	mask 1 with size = size
* @param[in] msk2	mask 2 with size = size
* @return	 SAD(sum of absolute difference) value with shiftBit
*/
static INT32 ME_GetSAD( UCHAR* msk1, UCHAR* msk2,const INT32 size,const INT32 shiftBit = 0)
{
    register INT32 sad = 0;
    UCHAR *p_m1 = msk1;
    UCHAR *p_m2 = msk2;
    for( INT32 count = 0 ; count < size ; ++count, ++p_m1, ++p_m2)
    {
        sad += ABS( (INT32)(*p_m1) - (INT32)(*p_m2) );

    }
#if DBG_INFO==ON
	INT32 *sad_map=(INT32*)calloc(size,sizeof(sad_map[0]));
	for( INT32 i=0 ; i<size ; ++i)
	{
		sad_map[i] = ABS( (int)msk1[i]-msk2[i] );
	}
	free(sad_map);
#endif

    return sad>>shiftBit;
}
/*
static int FindMinIdx(int* src,int length,int TH)
{
    int i,idx=0x7FFFFFFF,min=0x7FFFFFFF;
    for(i=0; i<length; i++)
        if(src[i]<min && src[i]<=TH)
        {
            min=src[i];
            idx=i;
        }
        return idx;
}
*/



/**
* @brief     Find the index of minimal value in the array 
* @param[in] src	the array 
* @param[in] length	the array length
* @return	 the index of minimal value
*/
static INT32 FindMinIdx(int* src,int length)
{
    INT32 idx = 0 ;
    INT32 min = src[0] ;
    for( INT32 i = 1; i < length; ++i )
    {
        if( src[i] <= min )
        {
            min = src[i];
            idx = i;
        }
    }
    return idx;
}

static unsigned char g_src_msk[ MATCH_BLOCK_WIDTH * MATCH_BLOCK_HEIGHT ]; ///< default mask used internal
static unsigned char g_dst_msk[ MATCH_BLOCK_WIDTH * MATCH_BLOCK_HEIGHT ]; ///< default mask used internal

//---------------------Hexagon Search----------------
/**
* @brief  The search order when initial stage       
* @n            [6]   [1]
* @n         [5]   [0]  [2]
* @n            [4]   [3]
*/
const MVector_t hexagon_mv_ini[7]={{0,0},{1,-2},{2,0},{1,2},{-1,2},{-2,0},{-1,-2}};
/**
* @brief  The 4 search orders when searching stage according to previous direction       
*/
const MVector_t hexagon_mv_go[7][4]=
{
    {{0,0},{0,0},{0,0},{0,0}},				//0
    {{0,0},{-1,-2},{1,-2},{2,0}},			//1 ex. previous direction is (1)right-top, then the following points are searched {0,0},{-1,-2},{1,-2},{2,0}//
    {{0,0},{1,-2},{2,0},{1,2}},				//2
    {{0,0},{2,0},{1,2},{-1,2}},				//3
    {{0,0},{1,2},{-1,2},{-2,0}},			//4
    {{0,0},{-1,2},{-2,0},{-1,-2}},			//5
    {{0,0},{-2,0},{-1,-2},{1,-2}}			//6
};
/**
* @brief  The 5 search orders when Ending stage
*/
const MVector_t hexagon_mv_end[5]={{0,0},{0,-1},{1,0},{0,1},{-1,0}};
typedef enum ENUM_INT8_T { hs_Stop , hs_Starting, hs_Searching, hs_Ending , hs_OutBoundary} HexgonSearchStatus;

//---------------------SDBS-----------------
#define SDBS_MODIFY
/*
* @n              [4]  
* @n         [2][0][1]
* @n              [3]
*/
#ifndef SDBS_MODIFY	//original paper
//const MVector_t sdbs_mv_layer1plus2[9]={{0,0},{1,0},{-1,0},{0,1},{0,-1},{2,1},{-2,-1},{-1,2},{1,-2}};
const MVector_t sdbs_mv_layer2[5]={{0,0},{2,1},{-2,-1},{-1,2},{1,-2}};//(paper) original
const MVector_t sdbs_mv_layer2_go_0[4]={{0,0},{0,0},{0,0},{0,0}};				//0
const MVector_t sdbs_mv_layer2_go_1[4]={{0,0},{2,1},{-1,2},{1,-2}};			//1
const MVector_t sdbs_mv_layer2_go_2[4]={{0,0},{-2,-1},{-1,2},{1,-2}};		//2
const MVector_t sdbs_mv_layer2_go_3[4]={{0,0},{2,1},{-2,-1},{-1,2}};		//3
const MVector_t sdbs_mv_layer2_go_4[4]={{0,0},{2,1},{-2,-1},{1,-2}};		//4
/*
const MVector_t sdbs_mv_layer2_go[5][4]=	//(paper) original
{
	{{0,0},{0,0},{0,0},{0,0}},				//0
	{{0,0},{2,1},{-1,2},{1,-2}},			//1
	{{0,0},{-2,-1},{-1,2},{1,-2}},		//2
	{{0,0},{2,1},{-2,-1},{-1,2}},		//3
	{{0,0},{2,1},{-2,-1},{1,-2}}			//4
};*/
#else		//modify for CTA
//const MVector_t sdbs_mv_layer1plus2[9]={{0,0},{1,0},{-1,0},{0,1},{0,-1},{2,1},{-2,1},{-2,-1},{2,-1}};
const MVector_t sdbs_mv_layer2[5]={{0,0},{2,1},{-2,1},{-2,-1},{2,-1}};//rectangle
const MVector_t sdbs_mv_layer2_go_0[4]={{0,0},{0,0},{0,0},{0,0}};				//0
const MVector_t sdbs_mv_layer2_go_1[4]={{0,0},{2,1},{-2,1},{2,-1}};			//1
const MVector_t sdbs_mv_layer2_go_2[4]={{0,0},{2,1},{-2,1},{-2,-1}};		//2
const MVector_t sdbs_mv_layer2_go_3[4]={{0,0},{-2,1},{-2,-1},{2,-1}};		//3
const MVector_t sdbs_mv_layer2_go_4[4]={{0,0},{2,1},{-2,-1},{2,-1}};		//4
/*
const MVector_t sdbs_mv_layer2_go[5][4]=	//rectangle
{
	{{0,0},{0,0},{0,0},{0,0}},				//0
	{{0,0},{2,1},{-2,1},{2,-1}},			//1
	{{0,0},{2,1},{-2,1},{-2,-1}},		//2
	{{0,0},{-2,1},{-2,-1},{2,-1}},		//3
	{{0,0},{2,1},{-2,-1},{2,-1}}			//4
};
*/
#endif
const MVector_t sdbs_mv_layer1[5]={{0,0},{1,0},{-1,0},{0,1},{0,-1}};
const MVector_t sdbs_mv_layer1plus2[9]=
{
	sdbs_mv_layer1[0],
	sdbs_mv_layer1[1],sdbs_mv_layer1[2],sdbs_mv_layer1[3],sdbs_mv_layer1[4],
	sdbs_mv_layer2[1],sdbs_mv_layer2[2],sdbs_mv_layer2[3],sdbs_mv_layer2[4]
};
const MVector_t sdbs_mv_layer3[5]={{0,0},{5,0},{-5,0},{0,5},{0,-5}};
const MVector_t sdbs_mv_layer4[5]={{0,0},{10,0},{-10,0},{0,10},{0,-10}};
const MVector_t sdbs_mv_layer5[5]={{0,0},{20,0},{-20,0},{0,20},{0,-20}};
const MVector_t sdbs_mv_layer6[5]={{0,0},{40,0},{-40,0},{0,40},{0,-40}};
const MVector_t *p_sdbs_mv_layer[6]={sdbs_mv_layer1,sdbs_mv_layer2,sdbs_mv_layer3,sdbs_mv_layer4,sdbs_mv_layer5,sdbs_mv_layer6};

const MVector_t sdbs_mv_layer1_go_0[4]={{0,0},{0,0},{0,0},{0,0}};				//0
const MVector_t sdbs_mv_layer1_go_1[4]={{0,0},{1,0},{0,1},{0,-1}};			//1
const MVector_t sdbs_mv_layer1_go_2[4]={{0,0},{-1,0},{0,1},{0,-1}};			//2
const MVector_t sdbs_mv_layer1_go_3[4]={{0,0},{1,0},{-1,0},{0,1}};			//3
const MVector_t sdbs_mv_layer1_go_4[4]={{0,0},{1,0},{-1,0},{0,-1}};			//4
const MVector_t *p_sdbs_mv_layer1_go[5]={sdbs_mv_layer1_go_0,sdbs_mv_layer1_go_1,sdbs_mv_layer1_go_2,sdbs_mv_layer1_go_3,sdbs_mv_layer1_go_4};

const MVector_t *p_sdbs_mv_layer2_go[5]={sdbs_mv_layer2_go_0,sdbs_mv_layer2_go_1,sdbs_mv_layer2_go_2,sdbs_mv_layer2_go_3,sdbs_mv_layer2_go_4};

const MVector_t sdbs_mv_layer3_go_0[4]={{0,0},{0,0},{0,0},{0,0}};				//0
const MVector_t sdbs_mv_layer3_go_1[4]={{0,0},{5,0},{0,5},{0,-5}};			//1
const MVector_t sdbs_mv_layer3_go_2[4]={{0,0},{-5,0},{0,5},{0,-5}};			//2
const MVector_t sdbs_mv_layer3_go_3[4]={{0,0},{5,0},{-5,0},{0,5}};			//3
const MVector_t sdbs_mv_layer3_go_4[4]={{0,0},{5,0},{-5,0},{0,-5}};			//4
const MVector_t *p_sdbs_mv_layer3_go[5]={sdbs_mv_layer3_go_0,sdbs_mv_layer3_go_1,sdbs_mv_layer3_go_2,sdbs_mv_layer3_go_3,sdbs_mv_layer3_go_4};
const MVector_t **pp_sdbs_mv_layer_go[3]={p_sdbs_mv_layer1_go,p_sdbs_mv_layer2_go,p_sdbs_mv_layer3_go};

/*
const MVector_t sdbs_mv_layer1_go[5][4]=
{
	{{0,0},{0,0},{0,0},{0,0}},				//0
	{{0,0},{1,0},{0,1},{0,-1}},			//1
	{{0,0},{-1,0},{0,1},{0,-1}},			//2
	{{0,0},{1,0},{-1,0},{0,1}},			//3
	{{0,0},{1,0},{-1,0},{0,-1}}			//4
};

const MVector_t sdbs_mv_layer3_go[5][4]=
{
	{{0,0},{0,0},{0,0},{0,0}},				//0
	{{0,0},{5,0},{0,5},{0,-5}},			//1
	{{0,0},{-5,0},{0,5},{0,-5}},			//2
	{{0,0},{5,0},{-5,0},{0,5}},			//3
	{{0,0},{5,0},{-5,0},{0,-5}}			//4
};
*/

typedef enum ENUM_INT8_T { sdbs_Stop , sdbs_StageA, sdbs_StageB, 
	sdbs_StageC_start, sdbs_StageC_go, sdbs_StageD_start, sdbs_StageD_end, sdbs_OutBoundary} SDBSStatus;

//---------------------BBGDS----------------
//8 1 2
//7 0 3
//6 5 4
const MVector_t bbgds_mv_ini[9]={{0,0},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1}};
const MVector_t bbgds_mv_go[9][6]=
{
    {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},				//0
    {{0,0},{-1,-1},{0,-1},{1,-1},{0,0},{0,0}},		//1
    {{0,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}},		//2
    {{0,0},{1,-1},{1,0},{1,1},{0,0},{0,0}},				//3
    {{0,0},{1,-1},{1,0},{1,1},{0,1},{-1,1}},			//4
    {{0,0},{1,1},{0,1},{-1,1},{0,0},{0,0}},				//5
    {{0,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1}}	,		//6
    {{0,0},{-1,1},{-1,0},{-1,-1},{0,0},{0,0}},		//7
    {{0,0},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}}		//8
};
//---------------------FULL_SEARCH-------------------------------------------//
//s_x,s_y:center pixel of the mask
//in the bitmap, if any pixel inside the mask is active, the center pixel is valid. the function returns true. 
static bool	ME_CheckBitMap(unsigned char* o_validMCpoint,int s_x,int s_y,int mask_width,int mask_height,const unsigned char* bitmap,int map_width,int map_height,
    int* p_dst_index=NULL,short* p_dst_x=NULL,short* p_dst_y=NULL)
{	
    const int stry=s_y-mask_height/2;//left-top point
    const int strx=s_x-mask_width/2;

    int str_idx=stry*map_width+strx;
    int dst_idx;
    short i,j;
    bool result=false;
    int tmp_idx=-1;

    for(j=stry; j<stry+mask_height; j++,str_idx+=map_width)
        for(dst_idx=str_idx,i=strx; i<strx+mask_width; i++,dst_idx++)
        {
            if(j<0 || j>=map_height || i<0 || i>=map_width)
                continue;
            else
            {
                //result=result||(bitmap[j*map_width+i]!=0);
                if(bitmap[dst_idx]!=0)
                {	
                    if(o_validMCpoint!=NULL)
                    {
                        if(!o_validMCpoint[dst_idx])//found unoccupied location
                        {
                            *p_dst_index=dst_idx;
                            return true;
                        }
                        else
                            tmp_idx=dst_idx;//current location is occupied, try finding unoccupied location
                    }
                    else
                        return true;
                }
            }
        };

    if(o_validMCpoint!=NULL)
        if(tmp_idx!=-1)//found locations but they are all occupied
        {
            *p_dst_index=tmp_idx;//overwrite occupied location
            result=true;
        }

        return result;
}

/**
* @brief Get Hori/Vert derivative Sobel Map with specific mask  
*        and time difference.
* @n        [Time difference mask]
* @n [-1 -2 -1]       [+1 +2 +1]
* @n [-2 -4 -2]  +    [+2 +4 +2]
* @n [-1 -2 -1]       [+1 +2 +1]
             [SobelX]
* @note sobel_H = hSobel_img = 
* @n   [-1 +0 +1]         [-1 +0 +1]
* @n   [-2 +0 +2]    +    [-2 +0 +2]
* @n   [-1 +0 +1]         [-1 +0 +1]
* @note       [SobelY]
* @n   [+1 +2 +1]         [+1 +2 +1]
* @n   [+0 +0 +0]    +    [+0 +0 +0]
* @n   [-1 -2 -1]         [-1 -2 -1]
*/
static void OF_LucasMap(INT16* tDiffMap, INT16* vSobelMap, INT16* hSobelMap, UCHAR *src1,UCHAR *src2,INT32 img_width, INT32 img_height )
{
    /// Ignore the First Frame
    static BOOL isfirstFrame = TRUE;
    if( TRUE == isfirstFrame )
    {
        isfirstFrame = FALSE;
        return;
    }

    /// Time Difference Map
    /// the ROI from RegionL/R
    INT32 startx = 1;
    INT32 endx   = img_width -2 ;
    INT32 starty = RegionL[1];
    INT32 endy   = RegionL[3];

    INT32 offsetVup   ;
    INT32 offsetV     ;
    INT32 offsetVdown ;
    INT32 sum1 ,sum2  ;  ///< src1/src2 mask sum
    for ( INT32 vert = starty; vert < endy ;++vert)
    {
        offsetVup   = (vert-1) * img_width;
        offsetV     = vert * img_width;
        offsetVdown = (vert+1) * img_width;

        for ( INT32 hori = startx; hori < endx ;++hori)
        {
            sum1 =  ( src1[offsetV + hori]<<2 )+
                    ( src1[offsetV + hori-1]<<1)+(src1[offsetV + hori+1]<<1)+(src1[offsetVup + hori]<<1)+(src1[offsetVdown + hori]<<1)+
                    ( src1[offsetVup + hori-1])+(src1[offsetVup + hori+1])+(src1[offsetVdown + hori+1])+(src1[offsetVdown + hori-1]);
            sum2 =  ( src2[offsetV + hori]<<2 )+
                    (src2[offsetV + hori-1]<<1)+(src2[offsetV + hori+1]<<1)+(src2[offsetVup + hori]<<1)+(src2[offsetVdown + hori]<<1)+
                    (src2[offsetVup + hori-1])+(src2[offsetVup + hori+1])+(src2[offsetVdown + hori+1])+(src2[offsetVdown + hori-1]);

            tDiffMap[ offsetV + hori ] = (INT16)(sum2-sum1);

        } //end of [hori < endx]
    }//end of [vert < endy]

    /// sobel

    INT32 row, col, rowidx;
    UCHAR *top_line, *cen_line, *bot_line;
    INT16 H_left, H_cen, H_right;
    INT16 V_left, V_cen, V_right;
    UCHAR *top_line2, *cen_line2, *bot_line2;
    INT16 H_left2, H_cen2, H_right2;
    INT16 V_left2, V_cen2, V_right2;
    INT16 sobel_H, sobel_V;

    startx = 1;
    endx   = img_width-1;
    starty = RegionL[1];
    endy   = RegionL[3];

    for( row =starty ; row < endy; ++row)
    {
        rowidx    = row * img_width;           
        top_line  = src1 + rowidx - img_width;  ///< offsetVup
        cen_line  = src1 + rowidx;               ///< offsetV
        bot_line  = src1 + rowidx + img_width;   ///< offsetVdown
        top_line2 = src2 + rowidx - img_width;  ///< offsetVup
        cen_line2 = src2 + rowidx;               ///< offsetV
        bot_line2 = src2 + rowidx + img_width;   ///< offsetVdown

        //left column
        H_left = (short)*top_line - (short)*bot_line;
        V_left = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;
        H_left2 = (short)*top_line2 - (short)*bot_line2;
        V_left2 = *top_line2 + (*cen_line2 << 1) + *bot_line2;
        bot_line2++;
        cen_line2++;
        top_line2++;

        //shift to center column
        H_cen  = (short)*top_line - (short)*bot_line;
        V_cen = *top_line + (*cen_line << 1) + *bot_line;
        bot_line++;
        cen_line++;
        top_line++;
        H_cen2  = (short)*top_line2 - (short)*bot_line2;
        V_cen2 = *top_line2 + (*cen_line2 << 1) + *bot_line2;
        bot_line2++;
        cen_line2++;
        top_line2++;

        //shift to right column
        for( col = startx; col<endx; col++)
        {
            H_right =  (short)*top_line -  (short)*bot_line;
            V_right = *top_line + (*cen_line << 1) + *bot_line;
            H_right2 =  (short)*top_line2 -  (short)*bot_line2;
            V_right2 = *top_line2 + (*cen_line2 << 1) + *bot_line2;

            sobel_V = H_left + (H_cen << 1) + H_right  +  H_left2 + (H_cen2 << 1) + H_right2;
            sobel_H = V_right - V_left + V_right2 - V_left2;

            /// assign to Array
            *(hSobelMap + row*img_width + col)= (INT16)sobel_H;
            *(vSobelMap + row*img_width + col)= (INT16)sobel_V;

            H_left = H_cen;
            H_cen  = H_right;
            H_left2 = H_cen2;
            H_cen2  = H_right2;

            V_left = V_cen;
            V_cen  = V_right;
            V_left2 = V_cen2;
            V_cen2  = V_right2;

            bot_line++;
            cen_line++;
            top_line++;
            bot_line2++;
            cen_line2++;
            top_line2++;
        }
    } // END for(row=1; row<img_height-1; row++) 
}

//******************************************************************************
// O P E R A T O R  O V E R R I D E
//******************************************************************************


//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
/**
* @brief	 setPara is ready for using ME_Hexagon_Search_iterative find the Point Optical Flow
* @param[in] inp_th_array	
  @n							p_th_array[0] = SAD超過此值 認定移動過大 而不合格
  @n							p_th_array[1] = 與原點SAD相差 低於此值   認定移動量=0
  @n							p_th_array[2] = SAD低過此值 認定移動過小 而移動量=0
* @param[in] inMATCH_SHIFT_BIT	Input ME_Hexagon_Search_iterative Parameter
* @param[in] inRegionL			Input CTA ROI of LeftSide
* @param[in] inRegionR			Input CTA ROI of RightSide
*/
void Point_Track::setPara( const INT32 *inp_th_array , const INT32 inMATCH_SHIFT_BIT , const INT32 *inRegionL , const INT32 *inRegionR )
{
    memcpy( p_th_array , inp_th_array , sizeof(p_th_array) ); ///< [3]
    memcpy( RegionL , inRegionL , sizeof(RegionL) );
    memcpy( RegionR , inRegionR , sizeof(RegionR) );

    countMV         = 0;                ///< count Number of Motion Vector
    MATCH_SHIFT_BIT = inMATCH_SHIFT_BIT;
    
}

/**
* @brief	 CornerMotionEstimation use the method to calculate the optical flow at each harris corner
*/
void Point_Track::CornersMotionEstimation(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img , UCHAR *Gaussian_imgPrev , INT32 imgDimensionX, int pointNumberPrev)
{
    CONST MV_Alg choice = Alg_Hexagon;  ///< Define the MotionVector Alg

    MVector_t mv_sum;			///< Motion vector value at( corner_x, corner_y)
    for ( INT32 i1 = 0 ; i1 < pointNumberPrev; ++i1)
    {
        mv_sum = ME_Hexagon_SearchV2(Gaussian_img ,Gaussian_imgPrev, imgDimensionX,
                harrisCornersPrev[i1].x, harrisCornersPrev[i1].y, p_th_array);
        // old version
        //mv_sum = ME_Hexagon_Search_iterative(Gaussian_img ,Gaussian_imgPrev, imgDimensionX,
        //        harrisCornersPrev[i1].x, harrisCornersPrev[i1].y,0,0,MATCH_BLOCK_WIDTH,MATCH_BLOCK_HEIGHT,MATCH_BLOCK_SIZE,0, p_th_array);


    if( ( e_mv_invalid == mv_sum.x ) || 
            ( ( e_mv_zero== mv_sum.x ) && ( e_mv_zero == mv_sum.y ) )) //Invalid 
    {
            harrisCorners2Prev[i1].Diff_x  = mv_sum.x; // Debug Purpose
            harrisCorners2Prev[i1].Diff_y  = mv_sum.y; // Debug Purpose
            harrisCorners2Prev[i1].isValid = FALSE;
        }
        else
        {
            /// save data if the Correct Points
            harrisCorners2Prev[i1].Diff_x  = mv_sum.x;
            harrisCorners2Prev[i1].Diff_y  = mv_sum.y;
            harrisCorners2Prev[i1].isValid = TRUE;

            countMV++;
        } // end of if(e_mv_invalid == mv_sum.x)
    }// end of (i1 < MAXSizeHarrisCorner)
    return;
}

/**
* @brief	 Check the point if in the ROI 
* @param[in] RegionL	Input 2-point Rectangle at ROI left [0]LeftTopX[1]LeftTopY [2]RightBotX [3]RightBotY
* @param[in] RegionR	Input 2-point Rectangle at ROI right
* @Note                 Not Used
*/
ROI_TrackPoint Point_Track::isTrackPointInROI(UINT32 *RegionL , UINT32 *RegionR ,UINT32 PosX , UINT32 PosY)        // Pre-Process
{
    if ( (RegionL[0] <= PosX) && (PosX < RegionL[2]) &&
         (RegionL[1] <= PosY) && (PosY < RegionL[3]))
    {
        return ROI_L;
    } 
    else if ( (RegionR[0] <= PosX) && (PosX < RegionR[2]) &&
              (RegionR[1] <= PosY) && (PosY < RegionR[3]))
    {
        return ROI_R;
    }

    return ROI_Out;
}

//-------------------------------------------------
CONST CHAR CTA_hexagon_mv_ini[ 7*2 ] = {0,0 ,1,-2, 2,0, 1,2, -1,2, -2,0, -1,-2};
CONST CHAR CTA_hexagon_mv_go[ 7*4*2 ] ={0,0,0,0,0,0,0,0,
                                        0,0,-1,-2,1,-2,2,0,
                                        0,0,1,-2,2,0,1,2,
                                        0,0,2,0,1,2,-1,2,
                                        0,0,1,2,-1,2,-2,0,			
                                        0,0,-1,2,-2,0,-1,-2,			
                                        0,0,-2,0,-1,-2,1,-2};
CONST CHAR CTA_hexagon_mv_end[ 5*2 ] = {0,0,0,-1,1,0,0,1,-1,0};
/**
* @brief Do CTA Motion Estimation at the Point(bx,by)
* @n     General hexagon search Alg
* @param dst_img		Frame(T)
* @param src_img		Frame(T-1)
* @param bx				Pos(bx,by)
* @param blk_shiftBit_h (default = 0)
* @param blk_shiftBit_v	(default = 0)
* @param mask_width		(default = MATCH_BLOCK_WIDTH(4))
* @param mask_height	(default = MATCH_BLOCK_HEIGHT(8))
* @param mask_size		(=mask_width * mask_height)
* @param SadShiftBit    ( MotionValue denominator, MAD = SAD/(2^SadShiftBit))
* @param p_th_array		( Threshold [0] zero_motion_sad_TH [1] sad_diff_TH [2] zero_motion_sad_TH )
* @return               the Motion Vector 
*/
MVector_t Point_Track::ME_Hexagon_Search_iterative( const UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 bx,INT32 by,INT32 blk_shiftBit_h,INT32 blk_shiftBit_v, const INT32 mask_width,const INT32 mask_height,const INT32 mask_size, INT32 SadShiftBit,const INT32* p_th_array,UCHAR* src_msk /*= NULL*/, UCHAR* dst_msk /*= NULL*/ )
{
    CONST INT32 targetTH           = p_th_array[0];
    CONST INT32 sad_diff_TH        = p_th_array[1];
    CONST INT32 zero_motion_sad_TH = p_th_array[2];
    INT32 curr_x,curr_y;

    MVector_t mv_sum = {0,0,0};                ///< motion vector sum(Diff_x,y)
    MVector_t mv;
    INT32 sad[7] ,min_sad ,zero_motion_sad ;
    BOOL  isFillMask;
    INT32 min_dir ,base_dir ,dirs ;
    INT32 i;

	if( src_msk==NULL || dst_msk==NULL) ///< assign the default mask space
	{
		src_msk = g_src_msk;
		dst_msk = g_dst_msk;
        WIN32_ASSERT( (MATCH_BLOCK_WIDTH*MATCH_BLOCK_HEIGHT) == mask_size );
	}

    curr_x=bx<<blk_shiftBit_h;
    curr_y=by<<blk_shiftBit_v;

    isFillMask = ME_FillMask( src_msk ,curr_x ,curr_y ,mask_width ,mask_height ,src_img );
    if(!isFillMask)
    {
        mv_sum.x = mv_sum.y = e_mv_invalid; 
        return mv_sum;
    }

    HexgonSearchStatus stage = hs_Starting;
    ///1) 0:stop 1:ini search, 2:move search, 3:final search
    while( hs_Stop != stage)
    {
        switch( stage )
        {
        case hs_Starting: /// initial search
            
            ///1.1) cal sad at 6 point 
            /*
            * @n            [6]   [1]
            * @n         [5]   [0]  [2]
            * @n            [4]   [3]
            */
            dirs = 7;							
            for(i=0; i<dirs; i++)						
            {			
//                isFillMask = ME_FillMask(dst_msk,curr_x+hexagon_mv_ini[i].x,curr_y+hexagon_mv_ini[i].y,mask_width,mask_height,dst_img);
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_ini[i<<1] ,curr_y + CTA_hexagon_mv_ini[(i<<1)+1],mask_width,mask_height,dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
                else
                    sad[i]=0x7FFFFFFF;
            }

            ///1.2) find the minimal sad and its shift
            min_dir         = FindMinIdx(sad,dirs);
            min_sad         = sad[min_dir];
//            mv.x            = hexagon_mv_ini[min_dir].x;
//            mv.y            = hexagon_mv_ini[min_dir].y;
            mv.x            = CTA_hexagon_mv_ini[(min_dir<<1)];
            mv.y            = CTA_hexagon_mv_ini[(min_dir<<1)+1];
            zero_motion_sad = sad[0];

            ///1.3) check the next status according to the minimal sad idx.
            stage = (0 == min_dir)? hs_Ending : hs_Searching ;

            break;

        case hs_Searching://move search
            dirs     = 4;            ///< the 4 directions
            sad[0]   = sad[min_dir];//set min_dir as new center point
            base_dir = min_dir;

            for(i = 1; i < dirs; i++)
            {
//                isFillMask = ME_FillMask(dst_msk, curr_x + hexagon_mv_go[min_dir][i].x, curr_y + hexagon_mv_go[min_dir][i].y, mask_width, mask_height, dst_img);
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_go[ (min_dir<<3)+(i<<1) ], curr_y + CTA_hexagon_mv_go[(min_dir<<3)+(i<<1)+1], mask_width, mask_height, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, mask_size, SadShiftBit );
                else
                    sad[i] = 0x7FFFFFFF;
            }
            min_dir = FindMinIdx(sad,dirs);
            min_sad = sad[min_dir];
//            mv.x    = hexagon_mv_go[base_dir][min_dir].x;
//            mv.y    = hexagon_mv_go[base_dir][min_dir].y;
            mv.x    = CTA_hexagon_mv_go[(base_dir<<3)+ (min_dir<<1)    ] ;
            mv.y    = CTA_hexagon_mv_go[(base_dir<<3)+ (min_dir<<1) + 1] ;

            stage = (0 == min_dir)? hs_Ending: hs_Searching;
            stage = ( ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1)|| ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1) )?
                    hs_OutBoundary:stage;
            break;
        case hs_Ending://final search

            dirs = 5;
            for(i=1; i< dirs; i++)
            {
//                isFillMask = ME_FillMask(dst_msk, curr_x + hexagon_mv_end[i].x, curr_y + hexagon_mv_end[i].y, mask_width, mask_height, dst_img);
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_end[i<<1], curr_y + CTA_hexagon_mv_end[(i<<1)+1], mask_width, mask_height, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, mask_size, SadShiftBit);
                else
                    sad[i] = 0x7FFFFFFF;
            }
            min_dir = FindMinIdx(sad,dirs);
            min_sad = sad[min_dir];
//            mv.x    = hexagon_mv_end[min_dir].x;
//            mv.y    = hexagon_mv_end[min_dir].y;
            mv.x    = CTA_hexagon_mv_end[(min_dir<<1)  ] ;
            mv.y    = CTA_hexagon_mv_end[(min_dir<<1)+1] ;

            stage = hs_Stop;
            break;

        case hs_OutBoundary:

            dirs = 5;
            for( i = 1 ; i < dirs; i++ )
            {
//                isFillMask = ME_FillMask(dst_msk, curr_x + hexagon_mv_end[i].x, curr_y + hexagon_mv_end[i].y, mask_width, mask_height, dst_img);
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_end[(i<<1)], curr_y + CTA_hexagon_mv_end[(i<<1)+1], mask_width, mask_height, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, mask_size, SadShiftBit);
                else
                    sad[i]=0x7FFFFFFF;
            }
            min_dir = 0;
            min_sad = 0;
            mv.x    = 0;
            mv.y    = 0;

            stage = hs_Stop;
            break;
        }
        mv_sum.x += mv.x;
        mv_sum.y += mv.y;
        curr_x   += mv.x;
        curr_y   += mv.y;
    }

    //validation check
    if(min_sad>targetTH || ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1) || ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1))
        mv_sum.x=mv_sum.y=e_mv_invalid; 

    //return mv_sum;
    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
        mv_sum.x=mv_sum.y=e_mv_zero;

    return mv_sum;
}




/**
* @brief Do Fast CTA Motion Estimation at the Point(bx,by)
* @n     General hexagon search Alg
*/
MVector_t Point_Track::ME_Hexagon_SearchV2(UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 bx,INT32 by, const INT32* p_th_array)
{
    CONST INT32 targetTH           = p_th_array[0];
    CONST INT32 sad_diff_TH        = p_th_array[1];
    CONST INT32 zero_motion_sad_TH = p_th_array[2];
    INT32 curr_x = bx;
    INT32 curr_y = by;

    MVector_t mv_sum = {0,0,0};                ///< motion vector sum(Diff_x,y)
    MVector_t mv;
    INT32 sad[7] ,min_sad ,zero_motion_sad ;
    BOOL  isFillMask;
    INT32 min_dir ,base_dir ,dirs ;
    INT32 i;
    UCHAR *src_msk = g_src_msk;
    UCHAR *dst_msk = g_dst_msk;

    isFillMask = ME_FillMask( src_msk ,curr_x ,curr_y ,MATCH_BLOCK_WIDTH ,MATCH_BLOCK_HEIGHT ,src_img );
    if(!isFillMask)
    {
        mv_sum.x = mv_sum.y = e_mv_invalid; 
        return mv_sum;
    }

    HexgonSearchStatus stage = hs_Starting;
    ///1) 0:stop 1:ini search, 2:move search, 3:final search
    while( hs_Stop != stage)
    {
        switch( stage )
        {
        case hs_Starting: /// initial search
            
            ///1.1) cal sad at 6 point 
            /*
            * @n            [6]   [1]
            * @n         [5]   [0]  [2]
            * @n            [4]   [3]
            */
            dirs = 7;							
            for( i = 0; i < dirs; ++i )						
            {		
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_ini[i<<1] ,curr_y + CTA_hexagon_mv_ini[(i<<1)+1], MATCH_BLOCK_WIDTH , MATCH_BLOCK_HEIGHT, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, MATCH_BLOCK_SIZE ,0);
                else
                    sad[i]=0x7FFFFFFF;
            }

            ///1.2) find the minimal sad and its shift
            min_dir         = FindMinIdx( sad ,dirs );
            min_sad         = sad[min_dir];
            mv.x            = CTA_hexagon_mv_ini[(min_dir<<1)];
            mv.y            = CTA_hexagon_mv_ini[(min_dir<<1)+1];
            zero_motion_sad = sad[0];

            ///1.3) check the next status according to the minimal sad idx.
            stage = (0 == min_dir)? hs_Ending : hs_Searching ;

            break;

        case hs_Searching://move search
            dirs     = 4;            ///< the 4 directions
            sad[0]   = sad[min_dir]; //set min_dir as new center point
            base_dir = min_dir;

            for(i = 1; i < dirs; i++)
            {
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_go[ (min_dir<<3)+(i<<1) ], curr_y + CTA_hexagon_mv_go[(min_dir<<3)+(i<<1)+1], MATCH_BLOCK_WIDTH , MATCH_BLOCK_HEIGHT, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, MATCH_BLOCK_SIZE, 0 );
                else
                    sad[i] = 0x7FFFFFFF;
            }
            min_dir = FindMinIdx( sad , dirs );
            min_sad = sad[min_dir];
            mv.x    = CTA_hexagon_mv_go[(base_dir<<3)+ (min_dir<<1)    ] ;
            mv.y    = CTA_hexagon_mv_go[(base_dir<<3)+ (min_dir<<1) + 1] ;

            stage = (0 == min_dir)? hs_Ending : hs_Searching;
            stage = ( ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1)|| ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1) )?
                    hs_OutBoundary:stage;
            break;
        case hs_Ending://final search

            dirs = 5;
            for(i=1; i< dirs; i++)
            {
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_end[i<<1], curr_y + CTA_hexagon_mv_end[(i<<1)+1], MATCH_BLOCK_WIDTH, MATCH_BLOCK_HEIGHT, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, MATCH_BLOCK_SIZE, 0);
                else
                    sad[i] = 0x7FFFFFFF;
            }
            min_dir = FindMinIdx( sad, dirs );
            min_sad = sad[min_dir];
            mv.x    = CTA_hexagon_mv_end[(min_dir<<1)  ] ;
            mv.y    = CTA_hexagon_mv_end[(min_dir<<1)+1] ;
            
            stage = hs_Stop;
            break;

        case hs_OutBoundary:

            dirs = 5;
            for( i = 1 ; i < dirs; i++ )
            {
                isFillMask = ME_FillMask(dst_msk, curr_x + CTA_hexagon_mv_end[(i<<1)], curr_y + CTA_hexagon_mv_end[(i<<1)+1], MATCH_BLOCK_WIDTH, MATCH_BLOCK_HEIGHT, dst_img);
                if( isFillMask )
                    sad[i] = ME_GetSAD(src_msk, dst_msk, MATCH_BLOCK_SIZE, 0);
                else
                    sad[i]=0x7FFFFFFF;
            }
            min_dir = 0;
            min_sad = 0;
            mv.x    = 0;
            mv.y    = 0;

            stage = hs_Stop;
            break;
        }
        mv_sum.x += mv.x;
        mv_sum.y += mv.y;
        curr_x   += mv.x;
        curr_y   += mv.y;
    }

    //validation check
    if(min_sad>targetTH || ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1) || ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1))
        mv_sum.x=mv_sum.y=e_mv_invalid; 

    //return mv_sum;
    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
        mv_sum.x=mv_sum.y=e_mv_zero;

    return mv_sum;

}
/**
* @brief        FullSearch
* @n            原理: 每一個座標點形成的Mask，走在一區域(16x16)中 求取最小的SAD, 回傳其移動量
* @param[in]    dst_img         Frame(T)
* @param[in]    src_img         Frame(T-1)
* @param[in]    bx              corner position.
* @param[in]    shiftBit        the SAD calculation precision
* @param[in]    p_th_array      Threshold array
* @return       the MotionVector Value(SAD) & its position
*/
MVector_t Point_Track::ME_Full_Search( const UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 bx,INT32 by ,const INT32 shiftBit,const INT32* p_th_array )
{
    const INT32 targetTH           = p_th_array[0];
    const INT32 sad_diff_TH        = p_th_array[1];
    const INT32 zero_motion_sad_TH = p_th_array[2];

    const INT32 size = MATCH_BLOCK_WIDTH * MATCH_BLOCK_HEIGHT ;
    UCHAR src_msk[MATCH_BLOCK_WIDTH * MATCH_BLOCK_HEIGHT];
    UCHAR dst_msk[MATCH_BLOCK_WIDTH * MATCH_BLOCK_HEIGHT];
    MVector_t mv_sum;

    INT32 sad,min_sad     = 255;
    INT32 zero_motion_sad = 255;
    mv_sum.x=mv_sum.y=e_mv_invalid; 

    if( ME_FillMask(src_msk,bx,by , MATCH_BLOCK_WIDTH , MATCH_BLOCK_HEIGHT , src_img) )
    {
        INT32 halfHeight = MATCH_MAX_SEARCH_HEIGHT>>1;
        INT32 halfWidth  = MATCH_MAX_SEARCH_WIDTH>>1;

        for(INT32 j = -halfHeight; j < halfHeight ; ++j)
            for(INT32 i = -halfWidth ; i< halfWidth ; ++i)
            {
                sad = 255;
                if( ME_FillMask(dst_msk,bx+i,by+j, MATCH_BLOCK_WIDTH , MATCH_BLOCK_HEIGHT,dst_img) )
                    sad = ME_GetSAD(src_msk,dst_msk,size,shiftBit);

                if(i==0 && j==0)
                    zero_motion_sad=sad;

                if( sad < min_sad )
                {
                    mv_sum.x=i;
                    mv_sum.y=j;
                    min_sad=sad;
                }
            }
    }
    else//current mask filling failed
    {}

    /// THD 優化
    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
        mv_sum.x = mv_sum.y = 0;
    
    /// real motion vector assign compared with THD
    if(min_sad>targetTH)
        mv_sum.x=mv_sum.y=e_mv_invalid; 

    return mv_sum;
}

/**
* @brief        Block-Based Gradient Descent Search
* @param[in]    dst_img         Frame(T)
* @param[in]    src_img         Frame(T-1)
* @param[in]    src_x           corner original position at T-1.
* @param[in]    dst_x           corner original position at T.
* @param[in]    shiftBit        the SAD calculation precision
* @param[in]    p_th_array      Threshold array
* @return       the MotionVector Value(SAD) & its position
* @ref  http://dcmc.ee.ncku.edu.tw/pdf/course/Reports/Fast_Block_Motion_Estimation.pdf
*/
MVector_t  Point_Track::ME_BBGDS_Search(const UCHAR* dst_img,const UCHAR* src_img,const INT32 img_width,
                                    INT32 dst_x,INT32 dst_y,INT32 src_x,INT32 src_y,
                                    const INT32 mask_width,const INT32 mask_height,const INT32 mask_size,
                                    INT32 SadShiftBit,const INT32* p_th_array,
                                    UCHAR* src_msk, UCHAR* dst_msk)
{
    const int targetTH           = p_th_array[0];
    const int sad_diff_TH        = p_th_array[1];
    const int zero_motion_sad_TH = p_th_array[2];
    int curr_x,curr_y;

    MVector_t mv;
    MVector_t mv_sum;
    int sad[9],min_sad,zero_motion_sad;
    bool ret;
    int stage = 1 ; //0:stop 1:ini search, 2:move search, 3:final search
    int min_dir,base_dir,dirs;
    int i;

    if(src_msk==NULL || dst_msk==NULL) // use the default mask space
    {
        src_msk = g_src_msk;
        dst_msk = g_dst_msk;
        assert((MATCH_BLOCK_WIDTH*MATCH_BLOCK_HEIGHT) == mask_size);
    }

    ret = ME_FillMask(src_msk,src_x,src_y,mask_width,mask_height,src_img);
    if(!ret)
    {
        mv_sum.x=mv_sum.y=e_mv_invalid; 
        return mv_sum;
    }

    //move to true (rotated) location
    curr_x = dst_x;
    curr_y = dst_y;

    mv_sum.x = mv_sum.y = 0; 

    while(stage) //0:stop 1:ini search, 2:move search, 3:final search
    {
        switch(stage)
        {
        case 1://initial search
            dirs=9;
            for(i=0; i<dirs; i++)
            {
                ret = ME_FillMask( dst_msk,curr_x+bbgds_mv_ini[i].x,curr_y+bbgds_mv_ini[i].y,mask_width,mask_height,dst_img );
                if(ret)
                    sad[i] = ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
                else
                    sad[i]  =0x7FFFFFFF;
            }
            min_dir = FindMinIdx(sad,dirs);
            min_sad = sad[min_dir];
            if( min_sad == 0x7FFFFFFF ) //prevent out of range
            {
                mv_sum.x = e_mv_invalid;
                mv_sum.y = e_mv_invalid; 
                return mv_sum;
            }
            mv.x = bbgds_mv_ini[min_dir].x;
            mv.y = bbgds_mv_ini[min_dir].y;

            zero_motion_sad=sad[0];

            stage = (0 == min_dir)? (0):(2); ///< Next stage assign

            break;
        case 2://move search
            dirs     = (min_dir & 0x1)?4:6; ///< 角落用6 邊用4 空間
            sad[0]   = sad[min_dir];        ///< set min_dir as new center point
            base_dir = min_dir;

            for(i=1; i<dirs; i++)
            {
                ret = ME_FillMask( dst_msk,curr_x+bbgds_mv_go[min_dir][i].x,curr_y+bbgds_mv_go[min_dir][i].y,mask_width,mask_height,dst_img );
                if( ret )
                    sad[i] = ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
                else
                    sad[i] = 0x7FFFFFFF;
            }
            min_dir = FindMinIdx(sad,dirs);
            min_sad = sad[min_dir];
            mv.x    = bbgds_mv_go[base_dir][min_dir].x;
            mv.y    = bbgds_mv_go[base_dir][min_dir].y;

            if(min_dir==0)//center min
                stage=0;

            break;
        }
        mv_sum.x += mv.x;
        mv_sum.y += mv.y;
        curr_x   += mv.x;
        curr_y   += mv.y;

        if( ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1) || ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1)) ///< out of boundary
        {
            mv_sum.x = e_mv_invalid;
            mv_sum.y = e_mv_invalid; 
            return mv_sum;
        }
    }

    /// optical flow validation check
    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
        mv_sum.x=mv_sum.y=e_mv_zero;

    if(min_sad>targetTH)
        mv_sum.x=mv_sum.y=e_mv_invalid; 

    return mv_sum;
}
//old quadrant (not use) 
//	2 5 1
// 6 0 8
//	3 7 4
//new quadrant 
//	3 5 4
// 3 0 4
//	3 7 4
inline unsigned char Point_Track::GetMoveDirection(signed char mvx,signed char mvy)
{
	if(mvx==0 && mvy==0)
		return 0;
	else if(mvx<0 && mvy>0)
		return dir_Left;//3
	else if(mvx>0 && mvy>0)
		return dir_Right;//4
	else if(mvx>0 && mvy==0)
		return dir_Right;//8; 
	else if(mvx<0 && mvy==0)
		return dir_Left;//6; 
	else if(mvx==0 && mvy>0)
		return 7; 
	else if(mvx==0 && mvy<0)
		return 5; 
	else if(mvx>0 && mvy<0)
		return dir_Right;//1;
	else //if(mvx<0 && mvy<0)
		return dir_Left;//2;
}
//中央=0
//其他為時鐘方向1~12
inline unsigned char Point_Track::GetPolarDirection(signed char mvx,signed char mvy)
{
	const static double degree30=tan(3.1415926/6);
	const static double degree60=tan(3.1415926/3);

	int quadrant,part;
	double tanValue;
	unsigned char clock;

	if(mvx==0 && mvy==0)
	{
		return 0;
	}
	else if(mvx>0 && mvy<=0)
		quadrant=1;
	else if(mvx<=0 && mvy<0)
		quadrant=2;
	else if(mvx<0 && mvy>=0)
		quadrant=3;
	else
		quadrant=4;

	if(mvx==0)
		part=3;
	else if((tanValue=(double)ABS(mvy)/(double)ABS(mvx))<degree30)
		part=1;
	else if(tanValue<degree60)
		part=2;
	else
		part=3;

	switch(quadrant)
	{
	case 1:
		if(part==1)
			clock=2;
		else if(part==2)
			clock=1;
		else
			clock=12;
		break;
	case 2:
		if(part==1)
			clock=9;
		else if(part==2)
			clock=10;
		else
			clock=11;
		break;
	case 3:
		if(part==1)
			clock=8;
		else if(part==2)
			clock=7;
		else
			clock=6;
		break;
	case 4:
		if(part==1)
			clock=3;
		else if(part==2)
			clock=4;
		else
			clock=5;
		break;
	}
	return clock;
}
//Full Search with spiral scanning order: L-Up-R-Down
inline int Point_Track::CircularIdxAdd1(INT32 input, INT32 total )
{
    return (++input)>=total? 0:input;
}

/**
* @brief         the Pos(x,y) and calculate the next Pos(x,y) for Spiral Full Search
* @param[in,out]    i_x       Pos(x,y)
* @param[in,out]    i_y       Pos(x,y)
* @retu
*/
inline int Point_Track::SpiralOrder(CHAR& i_x,CHAR& i_y,BOOL reset/*=FALSE*/)
{

    const MVector_t dir[4]          ={{-1,0},{0,-1},{1,0},{0,1}};
    static MVector_t curr           = {0,0,0};    ///< accumulated pixel position
    static INT32 dir_idx            = 0;        ///< idx of dir[4]
    static INT32 total_count        = 0;        ///< count 
    static INT32 max_step_distance  = 0;        ///< if curr_step_distance equals max_step_distance, change direction
    static INT32 curr_step_distance = 0;        ///< accumulated repeat times for current direction
    static INT32 change_idx         = 0;        ///< every 2 change_idx adds 1 to max_step_distance

    if( TRUE == reset )
    {
        curr.x             = 0;
        curr.y             = 0;
        dir_idx            = 0;
        total_count        = 0;
        max_step_distance  = 0;
        curr_step_distance = 0;
        change_idx         = 0;
        return 0;
    }
    if( 0 == total_count )//base case
    {
        dir_idx = CircularIdxAdd1(dir_idx,4); //change direction
        ++max_step_distance;
    }
    else
    {
        if( max_step_distance == curr_step_distance )
        {
            curr_step_distance = 0;
            dir_idx = CircularIdxAdd1(dir_idx,4); //change direction
            ++change_idx;
            
            if( 2 == change_idx )
            {
                change_idx = 0;
                ++max_step_distance;
            }
        }
        curr.x += dir[dir_idx].x;
        curr.y += dir[dir_idx].y;
        ++curr_step_distance;
    }

    i_x = curr.x;
    i_y = curr.y;
    return ++total_count;
}

inline BOOL Point_Track::IsInRect(CONST INT16 x,CONST INT16 y, CONST Rect_t* ROI )
{
	return ( x>=ROI->start_col && x<ROI->start_col+ROI->width &&
		y>=ROI->start_row && y<ROI->start_row+ROI->height);
}
MVector_t Point_Track::ME_SmallDimondBlockSearch( CONST UCHAR* dst_img,CONST UCHAR* src_img, CONST INT32 img_width,INT32 src_x,INT32 src_y,INT32 dst_x,INT32 dst_y, const INT32 mask_width,CONST INT32 mask_height,CONST INT32 mask_size, INT32 SadShiftBit,CONST INT32* p_th_array,UCHAR* src_msk , UCHAR* dst_msk ,CONST TypeMoveDir Dir, CONST INT16 minLayerIdx, CONST INT16 maxLayerIdx, CONST BOOL unlimitedSearch,CONST BOOL horizontal )
{
    const int targetTH           = p_th_array[0];
    const int sad_diff_TH        = p_th_array[1];
    const int zero_motion_sad_TH = p_th_array[2];

	MVector_t mv_sum = {0,0,0x7FFF};                ///< motion vector sum(Diff_x,y)
    MVector_t mv_predict = {0,0,0x7FFF};                ///< motion vector
    MVector_t mv;
    int sad[9],min_sad,zero_motion_sad, base_sad;
    bool isFillMask;
    int min_dir,base_dir,dirs;
	int stageB_layerIdx=2;
    int stageC_layerIdx=2;
	int i;

	if(src_msk==NULL || dst_msk==NULL) ///< assign the default mask space
	{
		src_msk = g_src_msk;
		dst_msk = g_dst_msk;
		if ( (MATCH_BLOCK_WIDTH*MATCH_BLOCK_HEIGHT) !=mask_size)
		{
			mv_sum.x = mv_sum.y = e_mv_invalid; 
			return mv_sum;
		}
	}

    isFillMask = ME_FillMask( src_msk,src_x,src_y,mask_width,mask_height,src_img );
    if(!isFillMask)
    {
        mv_sum.x=mv_sum.y=e_mv_invalid; 
        return mv_sum;
    }

    SDBSStatus stage = sdbs_StageA;
    while( sdbs_Stop != stage)
    {
        switch( stage )
        {
        case sdbs_StageA: 
            dirs = 9;
/*			if(Dir==dir_Right)
			{	
			}*/
            for(i=0; i<dirs; i++)						
            {			
/*    #pragma region ====restrict search direction
				if(Dir==dir_Left)
				if(i>0 && i<=3)
				{
					sad[i]=0x7FFFFFFF;
					continue;
				}
    #pragma endregion
*/
                isFillMask = ME_FillMask(dst_msk,dst_x+sdbs_mv_layer1plus2[i].x,dst_y+sdbs_mv_layer1plus2[i].y,mask_width,mask_height,dst_img);
                if( isFillMask )
                    sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
                else
                    sad[i]=0x7FFFFFFF;
            }
            min_dir         = FindMinIdx(sad,dirs);
            min_sad         = sad[min_dir];
#if DBG_INFO==ON
            mv.x            = sdbs_mv_layer1plus2[min_dir].x;
            mv.y            = sdbs_mv_layer1plus2[min_dir].y;
#endif
            zero_motion_sad = sad[0];

			mv_sum.x = sdbs_mv_layer1plus2[min_dir].x;
			mv_sum.y = sdbs_mv_layer1plus2[min_dir].y;

            stage = (0 == min_dir)? sdbs_Stop : sdbs_StageB ;
            break;

        case sdbs_StageB:
            dirs     = 5;            ///< the 5 directions
            sad[0]   = sad[min_dir];//set min_dir as new point
			
			if(Dir!=dir_Unknown)
				dirs=4;

            for(i=1; i< dirs; i++)
            {
#pragma region ====restrict search direction
				if(Dir==dir_Right)
				{
					if(i==2)
						sad[i]=0x7FFFFFFF;
				}
				else if(Dir==dir_Left)
				{
					if(i==1)
						sad[i]=0x7FFFFFFF;
				}
				else
#pragma endregion
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+p_sdbs_mv_layer[stageB_layerIdx][i].x,dst_y+p_sdbs_mv_layer[stageB_layerIdx][i].y,mask_width,mask_height,dst_img);
                
					if( isFillMask )
						sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
						sad[i]=0x7FFFFFFF;
				}
            }
            min_dir = FindMinIdx(sad,dirs);
#if DBG_INFO==ON
			min_sad = sad[min_dir];
			mv.x  = p_sdbs_mv_layer[stageB_layerIdx][min_dir].x;
			mv.y  = p_sdbs_mv_layer[stageB_layerIdx][min_dir].y;
#endif
			if(min_dir!=0)
			{
				mv_sum.x = p_sdbs_mv_layer[stageB_layerIdx][min_dir].x;
				mv_sum.y = p_sdbs_mv_layer[stageB_layerIdx][min_dir].y;
				stageB_layerIdx++;
			}

			if( (min_dir==0 && stageB_layerIdx>=minLayerIdx) || (stageB_layerIdx>=maxLayerIdx) )
			{
				dst_x += mv_sum.x;
				dst_y += mv_sum.y;
				stage=sdbs_StageC_start;
			}
            break;

		case sdbs_StageC_start:
			dirs     = 5;            
			sad[0]   = sad[min_dir];

			for(i=1; i< dirs; i++)
			{
				mv_predict.x=mv_sum.x+p_sdbs_mv_layer[stageC_layerIdx][i].x;
				if( (Dir==dir_Left && mv_predict.x>0) || 
					(Dir==dir_Right && mv_predict.x<0) || 
					(horizontal==TRUE && (ABS(mv_predict.x)<ABS(mv_sum.y+p_sdbs_mv_layer[stageC_layerIdx][i].y))) )
					sad[i]=0x7FFFFFFF;
				else
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+p_sdbs_mv_layer[stageC_layerIdx][i].x,dst_y+p_sdbs_mv_layer[stageC_layerIdx][i].y,mask_width,mask_height,dst_img);

					if( isFillMask )
						sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
						sad[i]=0x7FFFFFFF;
				}
			}
			min_dir = FindMinIdx(sad,dirs);
			min_sad = sad[min_dir];
			mv.x    = p_sdbs_mv_layer[stageC_layerIdx][min_dir].x;
			mv.y    = p_sdbs_mv_layer[stageC_layerIdx][min_dir].y;

			mv_sum.x += mv.x;
			mv_sum.y += mv.y;
			dst_x += mv.x;
			dst_y += mv.y;

			if(stageC_layerIdx==0)
			{
				if(min_dir==0)
					stage=sdbs_Stop;//output
				else
					stage=sdbs_StageD_start;//next stage
			}
			else if(min_dir==0)
				stageC_layerIdx--;//smaller mask
			else			
				stage=sdbs_StageC_go;//moving

			break;

		case sdbs_StageC_go:
			dirs     = 4;            
			sad[0]   = sad[min_dir];//set min_dir as new center point
			base_dir = min_dir;

			for(i=1; i< dirs; i++)
			{
				mv_predict.x=mv_sum.x+pp_sdbs_mv_layer_go[stageC_layerIdx][base_dir][i].x;
				if( (Dir==dir_Left && mv_predict.x>0) || 
					(Dir==dir_Right && mv_predict.x<0) || 
					(horizontal==TRUE && (ABS(mv_predict.x)<ABS(mv_sum.y+pp_sdbs_mv_layer_go[stageC_layerIdx][base_dir][i].y))) )
					sad[i]=0x7FFFFFFF;
				else
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+pp_sdbs_mv_layer_go[stageC_layerIdx][base_dir][i].x,dst_y+pp_sdbs_mv_layer_go[stageC_layerIdx][base_dir][i].y,mask_width,mask_height,dst_img);

					if( isFillMask )
						sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
						sad[i]=0x7FFFFFFF;
				}
			}
			min_dir = FindMinIdx(sad,dirs);
#if DBG_INFO==ON
			min_sad = sad[min_dir];
#endif
			mv.x    = pp_sdbs_mv_layer_go[stageC_layerIdx][base_dir][min_dir].x;
			mv.y    = pp_sdbs_mv_layer_go[stageC_layerIdx][base_dir][min_dir].y;

			mv_sum.x += mv.x;
			mv_sum.y += mv.y;
			dst_x += mv.x;
			dst_y += mv.y;

			if(min_dir==0)
			{
				stageC_layerIdx--;//smaller mask
				stage=sdbs_StageC_start;
			}
			break;

		case sdbs_StageD_start:
			dirs = 5;            
			base_sad=sad[0]=sad[min_dir];//position of black triangle
			base_dir=min_dir;//save previous min_dir(position of black triangle)

			for(i=1; i< dirs; i++)
			{
				mv_predict.x=mv_sum.x+sdbs_mv_layer2[i].x;
				if( (Dir==dir_Left && mv_predict.x>0) || 
					(Dir==dir_Right && mv_predict.x<0) || 
					(horizontal==TRUE && (ABS(mv_predict.x)<ABS(mv_sum.y+sdbs_mv_layer2[i].y))) )
					sad[i]=0x7FFFFFFF;
				else
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+sdbs_mv_layer2[i].x,dst_y+sdbs_mv_layer2[i].y,mask_width,mask_height,dst_img);

					if( isFillMask )
						sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
						sad[i]=0x7FFFFFFF;
				}
			}
			min_dir = FindMinIdx(sad,dirs);
#if DBG_INFO==ON
			min_sad = sad[min_dir];
			mv.x    = sdbs_mv_layer2[min_dir].x;
			mv.y    = sdbs_mv_layer2[min_dir].y;
#endif

			if(min_dir!=0)
			{
				stageC_layerIdx=2;
				min_dir=0;//to get center(black triangle) sad@ sdbs_StageC_start
				stage=sdbs_StageC_start;
			}
			else //min_dir==0
			{
				stage=sdbs_StageD_end;
			}
			break;

		case sdbs_StageD_end:
			dirs    = 4;            //< the 4 directions
			sad[0] = sad[min_dir];//==base_sad==sad[min_dir];//min_dir must=0

			for(i=1; i< dirs; i++)
			{
				mv_predict.x=mv_sum.x+p_sdbs_mv_layer1_go[base_dir][i].x;
				if( (Dir==dir_Left && mv_predict.x>0) || 
					(Dir==dir_Right && mv_predict.x<0) || 
					(horizontal==TRUE && (ABS(mv_predict.x)<ABS(mv_sum.y+p_sdbs_mv_layer1_go[base_dir][i].y))) )
					sad[i]=0x7FFFFFFF;
				else
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+p_sdbs_mv_layer1_go[base_dir][i].x,dst_y+p_sdbs_mv_layer1_go[base_dir][i].y,mask_width,mask_height,dst_img);

					if( isFillMask )
						sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
						sad[i]=0x7FFFFFFF;
				}
			}
			min_dir = FindMinIdx(sad,dirs);
			min_sad = sad[min_dir];
			mv.x    = p_sdbs_mv_layer1_go[base_dir][min_dir].x;
			mv.y    = p_sdbs_mv_layer1_go[base_dir][min_dir].y;
			
			mv_sum.x += mv.x;
			mv_sum.y += mv.y;
			dst_x += mv.x;
			dst_y += mv.y;

			stage = (0 == min_dir)? sdbs_Stop: sdbs_StageD_start;
			break;
        }
		
		if (min_sad>targetTH) 
		{
			mv_sum.x=mv_sum.y=e_mv_invalid; 
			break;//while( Stop != stage)
		}

		if(!unlimitedSearch)
		if(ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1) || ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1))//validation check
		{
			mv_sum.x=mv_sum.y=e_mv_invalid; 
			break;//while( Stop != stage)
		}
    }

    //return mv_sum;
    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
        mv_sum.x=mv_sum.y=e_mv_zero;
	mv_sum.minSAD=min_sad/mask_size;

    return mv_sum;
}
MVector_t Point_Track::ME_Hexagon_Search_iterative_tracking( const UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 src_x,INT32 src_y,INT32 dst_x,INT32 dst_y, const INT32 mask_width,const INT32 mask_height,const INT32 mask_size, INT32 SadShiftBit,const INT32* p_th_array,UCHAR* src_msk , UCHAR* dst_msk ,CONST TypeMoveDir Dir,CONST BOOL unlimitedSearch,CONST BOOL horizonal )
{
    const int targetTH           = p_th_array[0];
    const int sad_diff_TH        = p_th_array[1];
    const int zero_motion_sad_TH = p_th_array[2];

	MVector_t mv_sum = {0,0,0x7FFF};                ///< motion vector sum(Diff_x,y)
    MVector_t mv_predict = {0,0,0x7FFF};                ///< motion vector
    MVector_t mv;
    int sad[7],min_sad,zero_motion_sad;
    bool isFillMask;
    int min_dir,base_dir,dirs;
    int i;

	if(src_msk==NULL || dst_msk==NULL) ///< assign the default mask space
	{
		src_msk = g_src_msk;
		dst_msk = g_dst_msk;
		if ( (MATCH_BLOCK_WIDTH*MATCH_BLOCK_HEIGHT) !=mask_size)
		{
			mv_sum.x = mv_sum.y = e_mv_invalid; 
			return mv_sum;
		}
	}

    isFillMask = ME_FillMask( src_msk,src_x,src_y,mask_width,mask_height,src_img );
    if(!isFillMask)
    {
        mv_sum.x=mv_sum.y=e_mv_invalid; 
        return mv_sum;
    }

    HexgonSearchStatus stage = hs_Starting;
    ///1) 0:stop 1:ini search, 2:move search, 3:final search
    while( hs_Stop != stage)
    {
        switch( stage )
        {
        case hs_Starting: /// initial search
            
            ///1.1) cal sad at 6 point 
            /*
            * @n            [6]   [1]
            * @n         [5]   [0]  [2]
            * @n            [4]   [3]
            */
            dirs = 7;
			if(Dir==dir_Right)
				dirs=4;
            for(i=0; i<dirs; i++)						
            {			
    #pragma region ====restrict search direction
				if(Dir==dir_Left)
				if(i>0 && i<=3)
				{
					sad[i]=0x7FFFFFFF;
					continue;
				}
    #pragma endregion
                isFillMask = ME_FillMask(dst_msk,dst_x+hexagon_mv_ini[i].x,dst_y+hexagon_mv_ini[i].y,mask_width,mask_height,dst_img);
                if( isFillMask )
                    sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
                else
                    sad[i]=0x7FFFFFFF;
            }

            ///1.2) find the minimal sad and its shift
            min_dir         = FindMinIdx(sad,dirs);
            min_sad         = sad[min_dir];
            mv.x            = hexagon_mv_ini[min_dir].x;
            mv.y            = hexagon_mv_ini[min_dir].y;
            zero_motion_sad = sad[0];

            ///1.3) check the next status according to the minimal sad idx.
            stage = (0 == min_dir)? hs_Ending : hs_Searching ;

            break;

        case hs_Searching://move search
            dirs     = 4;            ///< the 4 directions
            sad[0]   = sad[min_dir];//set min_dir as new center point
            base_dir = min_dir;

            for(i=1; i< dirs; i++)
            {
				mv_predict.x=mv_sum.x+hexagon_mv_go[base_dir][i].x;
				if( (Dir==dir_Left && mv_predict.x>0) || 
					(Dir==dir_Right && mv_predict.x<0) || 
					(horizonal==TRUE && (ABS(mv_predict.x)<ABS(mv_sum.y+hexagon_mv_go[base_dir][i].y))) )
					sad[i]=0x7FFFFFFF;
				else
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+hexagon_mv_go[base_dir][i].x,dst_y+hexagon_mv_go[base_dir][i].y,mask_width,mask_height,dst_img);
                
					if( isFillMask )
					    sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
					    sad[i]=0x7FFFFFFF;
				}
            }
            min_dir = FindMinIdx(sad,dirs);
            min_sad = sad[min_dir];
            mv.x    = hexagon_mv_go[base_dir][min_dir].x;
            mv.y    = hexagon_mv_go[base_dir][min_dir].y;

            stage = (0 == min_dir)? hs_Ending: hs_Searching;
            break;
        case hs_Ending://final search

            dirs = 5;
            for(i=1; i< dirs; i++)
            {
				mv_predict.x=mv_sum.x+hexagon_mv_end[i].x;
				if( (Dir==dir_Left && mv_predict.x>0) || 
					(Dir==dir_Right && mv_predict.x<0) ||
					(horizonal==TRUE && (ABS(mv_predict.x)<ABS(mv_sum.y+hexagon_mv_end[i].y))) ) 
					sad[i]=0x7FFFFFFF;
				else
				{
					isFillMask = ME_FillMask(dst_msk,dst_x+hexagon_mv_end[i].x,dst_y+hexagon_mv_end[i].y,mask_width,mask_height,dst_img);
					if( isFillMask )
						sad[i]=ME_GetSAD(src_msk,dst_msk,mask_size,SadShiftBit);
					else
						sad[i]=0x7FFFFFFF;
				}
            }
            min_dir = FindMinIdx(sad,dirs);
            min_sad = sad[min_dir];
            mv.x    = hexagon_mv_end[min_dir].x;
            mv.y    = hexagon_mv_end[min_dir].y;

            stage = hs_Stop;
            break;
        }
        mv_sum.x += mv.x;
        mv_sum.y += mv.y;
        dst_x   += mv.x;
        dst_y   += mv.y;
		
		if (min_sad>targetTH) 
		{
			mv_sum.x=mv_sum.y=e_mv_invalid; 
			break;//while( Stop != stage)
		}

		if(!unlimitedSearch)
		if(ABS(mv_sum.x)>(MATCH_MAX_SEARCH_WIDTH>>1) || ABS(mv_sum.y)>(MATCH_MAX_SEARCH_HEIGHT>>1))//validation check
		{
			mv_sum.x=mv_sum.y=e_mv_invalid; 
			break;//while( Stop != stage)
		}
    }

    //return mv_sum;
    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
        mv_sum.x=mv_sum.y=e_mv_zero;
	mv_sum.minSAD=min_sad/mask_size;

    return mv_sum;
}
/**
* @brief        ME_Full_Search_Spiral
* @param[in]    dst_img         Frame(T)
* @param[in]    src_img         Frame(T-1)
* @param[in]    src_x           corner original position at T-1.
* @param[in]    dst_x           corner original position at T.
* @param[in]    shiftBit        the SAD calculation precision
* @param[in]    p_th_array      Threshold array
* @return       the MotionVector Value(SAD) & its position
* @ref  http://www.radioeng.cz/fulltexts/2002/02_01_36_42.pdf
*/
MVector_t Point_Track::ME_Full_Search_Spiral( const UCHAR* dst_img,const UCHAR* src_img,const UCHAR* dst_bitmap, const INT32 img_width,INT32 bx,INT32 by,const INT32 mask_width,const INT32 mask_height,const INT32 size,const INT32 shiftBit,const INT32* p_th_array, UCHAR* src_msk /*= NULL*/, UCHAR* dst_msk /*= NULL*/, CONST UCHAR quadDir/*=0*/, CONST Rect_t *ROI/*=NULL*/ )
{
#if DBG_INFO==ON
	INT16 dump_sad_map[SrcImg_W*SrcImg_H];
	memset(dump_sad_map,0xFF,sizeof(dump_sad_map));
#endif
    const INT32 targetTH           = p_th_array[0];
    const INT32 sad_diff_TH        = p_th_array[1];
    const INT32 zero_motion_sad_TH = p_th_array[2];
    const INT32 early_terminate_TH = p_th_array[3];
    const INT32 max_count=(ROI==NULL)? 
		((quadDir==0)?MATCH_MAX_SEARCH_WIDTH*MATCH_MAX_SEARCH_HEIGHT:MATCH_MAX_SEARCH_WIDTH*MATCH_MAX_SEARCH_HEIGHT) : 
		ROI->width*ROI->height; // default 32x16

    MVector_t mv_sum;
    CHAR i=0,j=0;
    INT32 count=0; //count=1~max_count

    INT32 sad;
    INT32 min_sad         = 0x7FFFFFFF;
    INT32 zero_motion_sad = 0x7FFFFFFF;
	UCHAR quadrant;//方向限制

    mv_sum.x = e_mv_invalid;
    mv_sum.y = e_mv_invalid; 

    if(src_msk==NULL || dst_msk==NULL) ///< assign the default mask space
    {
        src_msk = g_src_msk;
        dst_msk = g_dst_msk;
        assert((MATCH_BLOCK_WIDTH*MATCH_BLOCK_HEIGHT) ==size);
    }

    SpiralOrder(i,j,true);

    //assign current block value to src_msk
    if( ME_FillMask(src_msk,bx,by,mask_width,mask_height,src_img) )//fill src_msk
    {
        while( count <= max_count )	//start block matching
        {
			SpiralOrder(i,j);
			if(ROI!=NULL)
			{	 
				if(IsInRect(i,j,ROI))//限制vector一定要在ROI內
					count++;
				else
					continue;
			}
			else if(i<=(MATCH_MAX_SEARCH_WIDTH>>1) && j<=(MATCH_MAX_SEARCH_HEIGHT>>1))//vector must be in the specified direction
			{
				if(quadDir!=0)
				{
					quadrant=GetMoveDirection(i,j);
					//	3 5 4
					// 3 0 4
					//	3 7 4
					if(quadrant==quadDir || quadrant==0 )//vector must be in the specified direction
						count++;
					else
						continue;
				}
				else
					count++;
			}
			else
				continue;

            if(dst_bitmap!=NULL)    
                if( !ME_CheckBitMap(NULL,(bx+i)>>1,(by+j)>>1,3,3,dst_bitmap,SrcImg_W,SrcImg_H) )
                    continue;

            if( ME_FillMask(dst_msk,bx+i,by+j,mask_width,mask_height,dst_img) )
                sad=ME_GetSAD(src_msk,dst_msk,size,shiftBit);

#if DBG_INFO==ON
			dump_sad_map[ (by+j)*img_width+(bx+i) ]=sad;
#endif

            if(i==0 && j==0)
                zero_motion_sad=sad;
            if(sad<min_sad)
            {
                mv_sum.x = i;
                mv_sum.y = j;
                min_sad  = sad;
                if(sad<early_terminate_TH)
                    break;
            }
        }
    }
    else//current mask filling failed
    {}

    if(zero_motion_sad<zero_motion_sad_TH || zero_motion_sad-min_sad<sad_diff_TH)
    {
        mv_sum.x=mv_sum.y=e_mv_zero;
    }

    //	if(mv_sum.x==0 && mv_sum.y==0)
    //		mv_sum.x=mv_sum.y=e_mv_zero;
    if(min_sad>targetTH)
        mv_sum.x=mv_sum.y=e_mv_invalid; 
    mv_sum.minSAD=min_sad/size;

#if DBG_INFO==ON
	DumpValueToText("g_sad_dumpMap.txt",
		MAX(0,bx-MATCH_MAX_SEARCH_WIDTH-mask_width),
		MAX(0,by-MATCH_MAX_SEARCH_HEIGHT-mask_height/2),
		MIN(img_width,bx+MATCH_MAX_SEARCH_WIDTH+mask_width/2),
		MIN(SrcImg_H,by+MATCH_MAX_SEARCH_HEIGHT+mask_height/2),
		img_width,dump_sad_map);
#endif

    return mv_sum;
}
/**
* @brief        Optical flow - lucas-Kanade method ( not finished yet)
* @param[in]    hSobelImg       sobel(Gx)
* @param[in]    vSobelImg       sobel(Gy)
* @return       the optical flow of the specific position
* @ref http://iamhahar.blogspot.tw/2012/04/lucas-kanade-optical-flow.html/http://www.cse.psu.edu/~rcollins/CSE486/lecture30.pdf / http://zh.wikipedia.org/wiki/%E5%8D%A2%E5%8D%A1%E6%96%AF-%E5%8D%A1%E7%BA%B3%E5%BE%B7%E6%96%B9%E6%B3%95
http://crcv.ucf.edu/source/optical /http://www.mathworks.com/matlabcentral/fileexchange/22950-lucas---kanade-pyramidal-refined-optical-flow-implementation
http://blog.csdn.net/hades_326/article/details/38130785 / http://vis-www.cs.umass.edu/~narayana/motionsegmentation.html
*/
MVector_t Point_Track::OF_lucasKanade(const INT16* hSobelMap,const INT16* vSobelMap ,const INT16* tDiffMap, const INT32 img_width,INT32 posX,INT32 posY )
{
    //(╯°□°）╯︵ 
    MVector_t ret             = {0,0,0};
    const INT32 window_width  =  9;
    const INT32 window_height =  9;

    FLOAT M[4] = {0};
    FLOAT b[2] = {0};
    INT32 startx = posX-(window_width>>1);
    INT32 endx   = posX+(window_width>>1);
    INT32 starty = posY-(window_height>>1);
    INT32 endy   = posY+(window_height>>1);

    if( (startx < 0) || (endx>=(img_width-1)) ) return ret; ///< out of boundary
    INT32 offsetV;
    for ( INT32 vert = starty ;vert<=endy ;++vert)
    {
        offsetV = vert * img_width;
        for ( INT32 hori = startx ;hori<=endx ;++hori)
        {
            M[0] += ((FLOAT)(hSobelMap[offsetV + hori]>>2) * (FLOAT)(hSobelMap[offsetV + hori]>>2));
            M[1] += ((FLOAT)(hSobelMap[offsetV + hori]>>2) * (FLOAT)(vSobelMap[offsetV + hori]>>2));
            M[3] += ((FLOAT)(vSobelMap[offsetV + hori]>>2) * (FLOAT)(vSobelMap[offsetV + hori]>>2));
            
            b[0] -= ((FLOAT)(hSobelMap[offsetV + hori]>>2) * (FLOAT)(tDiffMap[offsetV + hori]>>4));
            b[1] -= ((FLOAT)(vSobelMap[offsetV + hori]>>2) * (FLOAT)(tDiffMap[offsetV + hori]>>4));
            
            if( ABS(hori - (startx+(window_width>>1))) + ABS(vert-(starty+(window_height>>1))) <=2)
            {
                M[0] += ((FLOAT)(hSobelMap[offsetV + hori]>>2) * (FLOAT)(hSobelMap[offsetV + hori]>>2));
                M[1] += ((FLOAT)(hSobelMap[offsetV + hori]>>2) * (FLOAT)(vSobelMap[offsetV + hori]>>2));
                M[3] += ((FLOAT)(vSobelMap[offsetV + hori]>>2) * (FLOAT)(vSobelMap[offsetV + hori]>>2));

                b[0] -= ((FLOAT)(hSobelMap[offsetV + hori]>>2) * (FLOAT)(tDiffMap[offsetV + hori]>>4));
                b[1] -= ((FLOAT)(vSobelMap[offsetV + hori]>>2) * (FLOAT)(tDiffMap[offsetV + hori]>>4));
            }
        }
    }
    M[2] = M[1];
    
    /// inverse Matrix(Verified) (*^3^)/~☆
    FLOAT det     = M[0]*M[3]-M[1]*M[2];
    FLOAT invM[4] = {0};
    if( 0==det ) return ret;

    invM[0] = (FLOAT)M[3];
    invM[1] = (FLOAT)-M[1];
    invM[2] = (FLOAT)-M[2];
    invM[3] = (FLOAT)M[0];

    /// product Matrix(Verified) (^0_0^)
    FLOAT u = (invM[0]*b[0]+invM[1]*b[1])/det;
    FLOAT v = (invM[2]*b[0]+invM[3]*b[1])/det;
    ret.x =(CHAR)ROUND(u);
    ret.y =(CHAR)ROUND(v);
    return ret;
}

/**
* @brief        Mean-shift iterative tracking
* @param[in]    dst_img       current image
* @param[in]    src_img       previous image
* @return       the shift direction of the specific position
*/
MVector_t Point_Track::IT_MeanShift( const UCHAR* dst_img,const UCHAR* src_img,UCHAR *Edge_img, const INT32 img_width, const INT32 img_height,INT32 posX,INT32 posY )
{
    //(╯°□°）╯
    MVector_t ret             = {0,0,0};
    const INT32 window_width  =  51;     /// < window size ex. 3,5,7,9
    const INT32 window_height =  51;
    const CHAR  MEANSHIFT_ITARATION_NO = 5;

    CHAR  iteration               =  0;
    FLOAT iniHistogram    [256*2] = {0};
    FLOAT currentHistogram[256*2] = {0};
    FLOAT windowSize              = window_width * window_width;
    INT32 startx                  = posX-(window_width>>1);
    INT32 endx                    = posX+(window_width>>1);
    INT32 starty                  = posY-(window_height>>1);
    INT32 endy                    = posY+(window_height>>1);
    INT32 offsetV;
    
    ///1) FindHistogram(frame,m_sTrackingObjectTable[m_cActiveObject].initHistogram);
    if( (startx <0)||(endx>=(img_width-1))||(starty<0)||(endy>=(img_height-1))) return ret;
    
    BOOL isEdge;
    for (INT32 vert = starty ; vert <= endy ; ++vert )
    {
        offsetV = vert * img_width ;
        for (INT32 hori = startx ; hori <= endx ; ++hori)
        {
            isEdge = ( 255 == Edge_img[offsetV + hori])?(TRUE):(FALSE);
            if ( TRUE == isEdge )
                ++iniHistogram[ (src_img[offsetV + hori]) + 256 ];
            else
                ++iniHistogram[ (src_img[offsetV + hori]) + 0  ];
        }
    }
    for (INT32 i1 = 0 ; i1 < 512 ; ++i1 )
        iniHistogram[i1] /= windowSize;

    ///2) iterative solution
    FLOAT newX,newY;
    FLOAT weights[512]; 
    FLOAT sumOfWeights;
    INT32 Idx1;
    for ( iteration = 0; iteration < MEANSHIFT_ITARATION_NO; ++iteration)
    {
        ///2.1) FindHistogram(frame,currentHistogram); //current frame histogram
        /// out of boundary
        if( (startx <0)||(endx>=(img_width-1))||(starty<0)||(endy>=(img_height-1))) break;

        for (INT32 vert = starty ; vert <= endy ; ++vert )
        {
            offsetV = vert * img_width ;
            for (INT32 hori = startx ; hori <= endx ; ++hori)
            {
                isEdge = ( 255 == Edge_img[offsetV + hori])?(TRUE):(FALSE);
                if ( TRUE == isEdge )
                    ++currentHistogram[ (dst_img[offsetV + hori]) + 256 ];
                else
                    ++currentHistogram[ (dst_img[offsetV + hori]) + 0  ];
            }
        }
        for (INT32 i1 = 0 ; i1 < 512 ; ++i1 )
            currentHistogram[i1] /= windowSize;

        ///2.2) FindWightsAndCOM(frame,currentHistogram);//derive weights and new location
        for ( INT32 i = 0 ;i < 512 ; ++i)
            weights[i] = ( 0 == currentHistogram[i])?(0):( iniHistogram[i]/currentHistogram[i]);
        
        newX = 0.0;
        newY = 0.0;
        sumOfWeights = 0;
        for (INT32 vert = starty ; vert <= endy ; ++vert )
        {
            offsetV = vert * img_width ;
            for (INT32 hori = startx ; hori <= endx ; ++hori)
            {
                isEdge = ( 255 == Edge_img[offsetV + hori])?(TRUE):(FALSE);
                Idx1 = (dst_img[offsetV + hori]) +((TRUE == isEdge)?(256):(0));
                newX += weights[Idx1] * hori;
                newY += weights[Idx1] * vert;
                sumOfWeights += weights[Idx1];
            }
        }

        startx  = (INT32)((newX/sumOfWeights)+0.5) - (window_width >>1);
        endx    = (INT32)((newX/sumOfWeights)+0.5) + (window_width >>1);
        starty  = (INT32)((newY/sumOfWeights)+0.5) - (window_height>>1);
        endy    = (INT32)((newY/sumOfWeights)+0.5) + (window_height>>1);

        ret.x = (INT32)((newX/sumOfWeights)+0.5) - posX;
        ret.y = (INT32)((newY/sumOfWeights)+0.5) - posY;
    }
    return ret; /// ex. [-1,3] means shift x:-1 y:3 direction
}


static void Prob_ParticleFilter1DInit()
{

}
/**
* @brief        particle filter 1D Algorithm(NotimplementYet)
* @param[in]    dst_img       
* @param[in]    src_img       previous image
* @return       the shift direction of the specific position
*/
FLOAT Point_Track::Prob_ParticleFilter1D( FLOAT *x, INT32 Ns , FLOAT *w , FLOAT z )
{
    static BOOL isInitialize = FALSE;
    if(FALSE == isInitialize)
    {

        isInitialize = TRUE;
    }
    return 1;
}

/**
* @brief Patch  FilterI  : corners which shift less than 5kph.
* @n            FilterII : corners which ( Speed == CarSpeed ) and keeps the others
* @param 
*/
void Point_Track::PointTrackPatch(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img , INT32 imgDimensionX , SimpleCarSpec &carSpec ,CAN_Info *carState,
                                  int pointNumPrev)
{
//    MotionCornerFilterI( harrisCornersPrev , Gaussian_img ,  imgDimensionX);
//    MotionCornerFilterII(harrisCornersPrev, harrisCorners2Prev, Gaussian_img, imgDimensionX, 
//                          carSpec , carState, pointNumPrev);
    return;
}
/**
* @brief get the destination(cm) according to rotate theta, [ref]:totation matrix
* @param[out]   dstX            Xw'
* @param[in]    srcX            Xw 
* @param[in]    circleCenterX   Ackermann Circle center
* @param[in]    theta           rotation theta
*/
static void RotatePoint(FLOAT& dstX,FLOAT& dstY,FLOAT srcX,FLOAT srcY,FLOAT circleCenterX,FLOAT circleCenterY,FLOAT theta)
{
    double vX,vY;

    if(theta==0)
    {
        dstX=srcX;
        dstY=srcY;
        return;
    }
    vX=srcX-circleCenterX;
    vY=-(srcY-circleCenterY);//y-axis is downward
    dstX=(vX*cos(theta)+vY*sin(theta)+circleCenterX);
    dstY=(vX*sin(theta)-vY*cos(theta)+circleCenterY);
}
/**
* @brief            Filter Out the corners which ( Speed == CarSpeed ) and keeps the others
* @param[in,out]    harrisCornersPrev Corners
*/
void Point_Track::MotionCornerFilterII(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img , INT32 imgDimensionX , SimpleCarSpec &carSpec , CAN_Info *carState,
                                       int pointNumPrev)
{
    /// THD
    const UINT32 THD_SubjectMove = 15;             ///< (cm) 誤差值極限

    /// Global Variables
    FLOAT g_manual_ME_X , g_manual_ME_Y;
    INT16 rotatePointX         = (CTA_TV_WIDTH>>1), rotatePointY = (CTA_TV_HEIGHT>>1) ;
    
    /// steering angle
    FLOAT front_tire_angle_deg = carState->steerAngle  / carSpec.SteerOverFrontTireRatio;
    FLOAT front_tire_angle_rad = front_tire_angle_deg * MATH_PI / 180.0f;
    
    /// multiply speed, into rotate angle
    FLOAT rotateTheta = 0; // sim of unknown steer angle
    //FLOAT rotateTheta          = (carState->carSpeed) * (carState->timeInterval) * (0.02777777777) * sin(front_tire_angle_rad)/carSpec.WheelBase_cm;//theta to rotation circle
    
    INT32 tmpXw1,tmpYw1,tmpXw2,tmpYw2;
    FLOAT tmpDiffXw , tmpDiffYw;
    for( UINT32 i1 = 0 ; i1 < pointNumPrev; ++i1)
    {
        if( harrisCorners2Prev[i1].isValid ) 
        {
            /// Motion vector 的世界位移
            WModel->PosImgToWorld( tmpXw1,tmpYw1, harrisCornersPrev[i1].x + 0                            , harrisCornersPrev[i1].y + 0                            );
            WModel->PosImgToWorld( tmpXw2,tmpYw2, harrisCornersPrev[i1].x + harrisCorners2Prev[i1].Diff_x , harrisCornersPrev[i1].y + harrisCorners2Prev[i1].Diff_y );

            /// Check the ( corner Speed > THD_MinTargetSpeed )
            if( (tmpXw1 != 0xFFFF) && (tmpXw2 != 0xFFFF) )
            {
                tmpDiffXw = tmpXw2 - tmpXw1;
                tmpDiffYw = tmpYw2 - tmpYw1;
                
                /// X/Y world coordinate  Diff_X/Diff_Y world coordinate shift(cm)
                harrisCorners2Prev[i1].Diff_X = tmpDiffXw; harrisCorners2Prev[i1].Diff_Y = tmpDiffYw;
                harrisCorners2Prev[i1].X = tmpXw1;         harrisCorners2Prev[i1].Y = tmpYw1;

                /// 車速排除計算
                if(ABS(rotateTheta) >0.003)
                {
                    UINT32 circleY = CTA_TV_HEIGHT*(5) + carSpec.RearTireToBumper_cm ; ///< cm
                    UINT32 circleX = (CTA_TV_WIDTH>>1)*(5) - (carSpec.WheelBase_cm) / tan(front_tire_angle_rad); ///< cm
                    rotatePointX   = tmpXw1;
                    rotatePointY   = tmpYw1;
                    FLOAT destX,destY;

                    RotatePoint(destX,destY,rotatePointX,rotatePointY,circleX,circleY,rotateTheta);

                    g_manual_ME_X = rotatePointX - destX; 
                    g_manual_ME_Y = rotatePointY - destY;

                    harrisCorners2Prev[i1].moveCompensation_X = g_manual_ME_X; // difference cm
                    harrisCorners2Prev[i1].moveCompensation_X = g_manual_ME_Y;

                    INT32 tmp_x1,tmp_y1;
                    WModel->PosWorldToImg( (harrisCorners2Prev[i1].X + g_manual_ME_X)/5,
                                           (harrisCorners2Prev[i1].Y + g_manual_ME_Y)/5,
                                           tmp_x1, tmp_y1 );
                    harrisCorners2Prev[i1].moveCompensation_x = tmp_x1;
                    harrisCorners2Prev[i1].moveCompensation_y = tmp_y1;
                }
                else if(ABS(rotateTheta) <= 0.003)
                {
                    g_manual_ME_X=0;
                    g_manual_ME_Y=-carState->carSpeed * carState->timeInterval *0.02777777777;//1km/h=0.02777cm/ms

                    harrisCorners2Prev[i1].moveCompensation_X = g_manual_ME_X; // difference cm
                    harrisCorners2Prev[i1].moveCompensation_X = g_manual_ME_Y;
                    INT32 tmp_x1,tmp_y1;
                    WModel->PosWorldToImg( (harrisCorners2Prev[i1].X + g_manual_ME_X)/5,
                        (harrisCorners2Prev[i1].Y + g_manual_ME_Y)/5,
                                           tmp_x1, tmp_y1 );
                    harrisCorners2Prev[i1].moveCompensation_x = tmp_x1;
                    harrisCorners2Prev[i1].moveCompensation_y = tmp_y1;

                }
                // then will plot in verify_MotionCorner()
#ifdef _WIN32
  int deb01 = harrisCorners2Prev[i1].X;
  int deb02 = harrisCorners2Prev[i1].Y;
  int deb03 = harrisCornersPrev[i1].x;
  int deb04 = harrisCornersPrev[i1].y;
  int deb05 = harrisCorners2Prev[i1].Diff_X;
  int deb06 = harrisCorners2Prev[i1].Diff_Y;
  int deb07 = harrisCorners2Prev[i1].moveCompensation_X;
  int deb08 = harrisCorners2Prev[i1].moveCompensation_Y;
  int deb09 = harrisCorners2Prev[i1].moveCompensation_x;
  int deb10 = harrisCorners2Prev[i1].moveCompensation_y;
  int dd = 33;
#endif
                /* not enable now.
                /// compare rotation criteria
                if( (ABS(rotateTheta)>0.03) )
                {
                    UINT32 circleY = CTA_TV_HEIGHT*(5) + carSpec.RearTireToBumper_cm ; ///< cm
                    UINT32 circleX = (CTA_TV_WIDTH>>1)*(5) - (carSpec.WheelBase_cm) / tan(front_tire_angle_rad); ///< cm
                    rotatePointX   = tmpXw1;
                    rotatePointY   = tmpYw1;
                    FLOAT destX,destY;

                    RotatePoint(destX,destY,rotatePointX,rotatePointY,circleX,circleY,rotateTheta);
                    
                    g_manual_ME_X = rotatePointX - destX; 
                    g_manual_ME_Y = rotatePointY - destY;

                    if( (ABS(tmpDiffXw - g_manual_ME_X) + ABS(tmpDiffYw - g_manual_ME_Y)) <= THD_SubjectMove )  ///若subjectMove預測跟實際move 相同，需移除
                    {
//                        harrisCornersPrev[i1].isValid = FALSE;
                    }
                }
                */
            }
        } // end of (harrisCornersPrev[i1].isValid)
    }// end of ( i1 < MAXSizeHarrisCorner )

/*
    if( !ABS(rotateTheta)>0.03 )
    {
        g_manual_ME_X=0;
        g_manual_ME_Y=- (carState->carSpeed) * (carState->timeInterval) * (0.02777777777) ;//1km/h=0.02777cm/ms ?? 1km/h = 0.2778cm/ms
    }
    else
    {
        UINT32 circleY = CTA_TV_HEIGHT*(5) + carSpec.RearTireToBumper_cm ; ///< cm
        UINT32 circleX = (CTA_TV_WIDTH>>1)*(5) - (carSpec.WheelBase_cm) / tan(front_tire_angle_rad); ///< cm
        FLOAT destX,destY;
        RotatePoint(destX,destY,rotatePointX,rotatePointY,circleX,circleY,rotateTheta);
        g_manual_ME_X=rotatePointX-destX; 
        g_manual_ME_Y=rotatePointY-destY;
    }

*/
}

/**
* @brief            Filter Out the corners(地面點) which shift less than 5kph.
* @param[in,out]    harrisCornersPrev Corners
*/
void Point_Track::MotionCornerFilterI(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img , INT32 imgDimensionX )
{
    const FLOAT THD_MinTargetSpeed = 5.5;           ///< 5kph = 7cm/frame;

    INT32 tmpXw1,tmpYw1,tmpXw2,tmpYw2;
    FLOAT tmpSpeed;
    for( UINT32 i1 = 0 ; i1 < MAXSizeHarrisCorner; ++i1)
    {
        if( harrisCorners2Prev[i1].isValid ) 
        {
            WModel->PosImgToWorld( tmpXw1,tmpYw1, harrisCornersPrev[i1].x + 0                            , harrisCornersPrev[i1].y + 0                            );
            WModel->PosImgToWorld( tmpXw2,tmpYw2, harrisCornersPrev[i1].x + harrisCorners2Prev[i1].Diff_x , harrisCornersPrev[i1].y + harrisCorners2Prev[i1].Diff_y );
            
            /// Check the ( corner Speed > THD_MinTargetSpeed ) [地面點]
            if( (tmpXw1 != 0xFFFF) && (tmpXw2 != 0xFFFF) &&(tmpXw2 != 0xFFFF ) && (tmpYw2 != 0xFFFF))
            {
                tmpSpeed = ABS((tmpYw2-tmpYw1))+ABS((tmpXw2-tmpXw1)); ///< (cm/frame) Norm1
                harrisCorners2Prev[i1].isValid = ( tmpSpeed>=THD_MinTargetSpeed )?TRUE:FALSE;
            }
        }
    } // end of (i1 < MAXSizeHarrisCorner)
}
Point_Track* Point_Track::getInstance()
{
    if(!instance)
        instance = new Point_Track;
    return instance;
}

Point_Track::Point_Track(void)
{
}
Point_Track::~Point_Track(void)
{
}

void Point_Track::ResetInstance()
{
    delete instance; 
    instance = NULL;
}


