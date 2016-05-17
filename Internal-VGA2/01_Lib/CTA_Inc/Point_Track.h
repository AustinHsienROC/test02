/*
===================================================================
Name		: Point_Track.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: These Motion Estimation functions 
                -block match(hexgon/spiral/full/BBGDS search)[�ɤO�k] -refer to MOD - Motion.cpp[K2_BackwardMVector()] / Motion.h / MotionAPI.h 
                -optical flow(lucas / horn1981)              [�̤p�~�t]
                -mean-shift/cam-shift                        [iterative run]
                -PorbabilityModel- particle filter/ HMM / Kalman filter

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/08/02	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
2014/08/02	  | v0.0020  	|+BlockMath Alg  / Refactor   	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef _POINT_TRACK_H_
#define _POINT_TRACK_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifndef _WIN32 // K2 platform
  #include "GlobalTypes.h"
#endif

#include "CTA_Def.h"
#include "CTA_Type.h"
#include "CTAApi.h"
#include "CoordinateConversion.h"
#include "CTA_CustomData.h"
#include "Filter2D.h"
//******************************************************************************
// S T R U C T / M A C R O   D E F I N I T I O N S
//******************************************************************************
#define MATCH_BLOCK_WIDTH				8       
#define MATCH_BLOCK_HEIGHT				4       
CONST INT32 MATCH_BLOCK_SIZE  = MATCH_BLOCK_WIDTH * MATCH_BLOCK_HEIGHT;

#define MATCH_MAX_SEARCH_WIDTH			32		//block move range
#define MATCH_MAX_SEARCH_HEIGHT			16		//block move range

typedef enum ENUM_INT8_T {e_mv_initial = 127, e_mv_zero = 0, e_mv_invalid = -128} e_MVectorValue;
typedef enum ENUM_INT8_T {ROI_Out, ROI_L, ROI_R } ROI_TrackPoint; 

#define TRACK Point_Track::getInstance()

typedef struct _Coordinate_t_tag
{
    INT16 x;	///< in x direction, unit: pixel
    INT16 y;    ///< in y direction, unit: pixel
    INT32 value;  ///<usage: sadValue of mask@ME
}Coordinate_t ;  

typedef struct MVector_t_tag
{
    CHAR x;	        ///< in x direction, unit: pixel
    CHAR y;	        ///< in y direction, unit: pixel
    UINT16 minSAD;
} MVector_t;  


//******************************************************************************
// F U N C T I O N  D E C L A R A T I O N
//******************************************************************************
class Point_Track
{
private:
    static Point_Track *instance;
public:
    INT32	p_th_array[3];
    UINT32  countMV;
    INT32   MATCH_SHIFT_BIT;
    UINT32  RegionL[4];
    UINT32  RegionR[4];

public:
    Point_Track(void);
    ~Point_Track(void);

    static Point_Track* getInstance();
    static void ResetInstance();

	void setPara( const INT32 *inp_th_array , const INT32 inMATCH_SHIFT_BIT , const INT32 *RegionL , const INT32 *RegionR);
	void CornersMotionEstimation(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img, UCHAR *Gaussian_imgPrev, INT32 imgDimensionX, int pointNumberPrev);

    ROI_TrackPoint isTrackPointInROI(UINT32 *RegionL , UINT32 *RegionR ,UINT32 PosX , UINT32 PosY);
	void PointTrackPatch(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img, INT32 imgDimensionX, SimpleCarSpec &carSpec ,CAN_Info *carState,
		                 int pointNumPrev);

public:
 //--------------------------OPTICAL_FLOW_SEARCH_FAMILY-------------------------//
	MVector_t ME_SmallDimondBlockSearch( CONST UCHAR* dst_img,CONST UCHAR* src_img, CONST INT32 img_width,INT32 src_x,INT32 src_y,INT32 dst_x,INT32 dst_y, const INT32 mask_width,CONST INT32 mask_height,CONST INT32 mask_size, INT32 SadShiftBit,CONST INT32* p_th_array,UCHAR* src_msk , UCHAR* dst_msk ,CONST TypeMoveDir Dir, CONST INT16 minLayerIdx, CONST INT16 maxLayerIdx, CONST BOOL unlimitedSearch,CONST BOOL horizontal );
    
    /** Block match -[hexagon]*/
	MVector_t ME_Hexagon_Search_iterative(const UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 bx,INT32 by,INT32 blk_shiftBit_h,INT32 blk_shiftBit_v, const INT32 mask_width,const INT32 mask_height,const INT32 mask_size, INT32 SadShiftBit,const INT32* p_th_array,UCHAR* src_msk = NULL, UCHAR* dst_msk = NULL);
	MVector_t ME_Hexagon_SearchV2(UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 bx,INT32 by, const INT32* p_th_array);
    MVector_t ME_Hexagon_Search_iterative_tracking( const UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 src_x,INT32 src_y,INT32 dst_x,INT32 dst_y, const INT32 mask_width,const INT32 mask_height,const INT32 mask_size, INT32 SadShiftBit,const INT32* p_th_array,UCHAR* src_msk , UCHAR* dst_msk ,CONST TypeMoveDir Dir,CONST BOOL unlimitedSearch,CONST BOOL horizonal);
    
    /** Block match -[Full]*/
    MVector_t ME_Full_Search( const UCHAR* dst_img,const UCHAR* src_img, const INT32 img_width,INT32 bx,INT32 by ,const INT32 shiftBit,const INT32* p_th_array);

    /** Block match -[BBGDS]*/
    MVector_t  ME_BBGDS_Search(const UCHAR* dst_img,const UCHAR* src_img,const INT32 img_width,
        INT32 dst_x,INT32 dst_y,INT32 src_x,INT32 src_y,
        const INT32 mask_width,const INT32 mask_height,const INT32 mask_size,
        INT32 SadShiftBit,const INT32* p_th_array,
        UCHAR* src_msk, UCHAR* dst_msk);

    /** Block match -[SpiralFullSearch]*/
	MVector_t ME_Full_Search_Spiral(const UCHAR* dst_img,const UCHAR* src_img,const UCHAR* dst_bitmap, const INT32 img_width,INT32 bx,INT32 by,const INT32 mask_width,const INT32 mask_height,const INT32 size,const INT32 shiftBit,const INT32* p_th_array, UCHAR* src_msk = NULL, UCHAR* dst_msk = NULL, CONST UCHAR quadDir=0, CONST Rect_t *ROI=NULL);
	inline int SpiralOrder(CHAR& i_x,CHAR& i_y,BOOL reset=FALSE);
	inline int CircularIdxAdd1(INT32 input, INT32 total );
	bool ME_FillMask( UCHAR* msk,const INT32 s_x, const INT32 s_y,const INT32 mask_width,const INT32 mask_height,const UCHAR* img,const INT32 img_width=SrcImg_W,const INT32 img_height=SrcImg_H);
	bool ME_FillMask(UCHAR* msk,Coordinate_t& coord,INT32 m_width,INT32 m_height,const UCHAR* img);
	inline BOOL IsInRect(CONST INT16 x,CONST INT16 y, CONST Rect_t* ROI );
	inline unsigned char GetPolarDirection(signed char mvx,signed char mvy);
	inline unsigned char GetMoveDirection(signed char mvx,signed char mvy);

    MVector_t OF_lucasKanade(const INT16* hSobelMap,const INT16* vSobelMap ,const INT16* tDiffMap, const INT32 img_width,INT32 posX,INT32 posY);
    MVector_t IT_MeanShift(const UCHAR* dst_img,const UCHAR* src_img,UCHAR *Edge_img, const INT32 img_width, const INT32 img_height,INT32 posX,INT32 posY);
    
    FLOAT Prob_ParticleFilter1D( FLOAT *x, INT32 Ns , FLOAT *w , FLOAT z );
    //--------------------------OPTICAL_FLOW_SEARCH_FAMILY-------------------------//

	void MotionCornerFilterI (HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img , INT32 imgDimensionX);
	void MotionCornerFilterII(HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev, UCHAR *Gaussian_img , INT32 imgDimensionX , SimpleCarSpec &carSpec , CAN_Info *carState,
		                      int pointNumPrev);

};

#endif