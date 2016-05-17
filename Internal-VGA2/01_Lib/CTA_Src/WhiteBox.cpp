//******************************************************************************
//  I N C L U D E     F I L E S
//******************************************************************************
#ifdef _WIN32
  #include "stdafx.h"
#endif
#include <string.h>
#include "WhiteBox.h"


//******************************************************************************
// S T A T I C   V A R I A B L E S
//******************************************************************************
#if defined(_WIN32)
    static UINT32 IdxSaveImg = 1000001 ; ///< saved jpgfileName
#endif
//******************************************************************************
// G L O B A L   V A R I A B L E S
//******************************************************************************

//******************************************************************************
// I N T E R N A L   F U N C T I O N S
//******************************************************************************

/**
*@brief		Car Info display [carSpeed / steerAngle] & All the previous-frame Data Check
*@param[in] FrameHd			Car Info from Firmware.
*/
void verify_SrcImg_Header( UCHAR *dst_Y, INT32 dst_width, INT32 dst_height, UCHAR *src_Y, INT32 src_width, INT32 src_height, CAN_Info *carState,CTATimeManagement &i_TimeM, TARGET_NUM &i_target, UCHAR *i_GaussPrev, UCHAR *Sobel_imgPrev, UCHAR *g_harrisMapPrev, HarrisCornerPos *cornerPrev, CandidateRec *CandRecPrevL, CandidateRec *CandRecPrevR,
                          int pointNumPrev)
{
#if defined(_WIN32) &&( ON == OSD_verify_SrcImg_Header)
    
    OSD_SetColor(100, 122, 244);
    OSD_Draw_Text( (CHAR*)"carSpeed"   , dst_Y , dst_width, dst_height, dst_width/2-20, dst_height-88, 0);
    OSD_Draw_INT32( (INT32)carState->carSpeed  , dst_Y, dst_width, dst_height, dst_width/2+10-20, dst_height-65, 3 ,12);
    OSD_Draw_Text( (CHAR*)"steerAngle" , dst_Y , dst_width, dst_height, dst_width/2-20, dst_height-55, 0);
    OSD_Draw_INT32( (INT32)carState->steerAngle, dst_Y, dst_width, dst_height, dst_width/2+10-20, dst_height-35, 3 ,12);
    
#endif
#if defined(SYNC_ACROSS_PLATFORM)
    #ifdef DATA_PATTERN_FOUNTAINHEAD 
    LoadFrameHead( GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_carState.DAT"), carState);
    LoadTimeManagement(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_TimeManagement.DAT"),&i_TimeM);
    LoadTargetNum( GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_TargetNum.DAT"),&i_target);

    LoadBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_src_Y.RAW" ),             src_Y,               src_width*src_height);
    LoadBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_Gaussian_imgPrev.RAW"),   i_GaussPrev,         src_width*src_height);
    LoadBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_Sobel_imgPrev.RAW"),      Sobel_imgPrev,       src_width*src_height);
    LoadBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_harrisMapPrev.RAW"),      g_harrisMapPrev,     src_width*src_height);

    LoadCorners(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_cornerPrev.RAW"),cornerPrev, MAXSizeHarrisCorner);
    LoadCandidateRec(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_CandRecPrevL.RAW"),CandRecPrevL, MAXCandidateNum);
    LoadCandidateRec(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_CandRecPrevR.RAW"),CandRecPrevR, MAXCandidateNum);

    /** for debug (Verified)
    DumpFrameHead(GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH, "00_carState.DAT"), carState);
    DumpTimeManagement( GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH, "00_TimeManagement.DAT"),&i_TimeM,true ); 
    DumpTargetNum(GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH, "00_TargetNum.DAT"),&i_target);

    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_src_Y.RAW"),              src_Y,              src_width*src_height,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_Gaussian_imgPrev.RAW"),   i_GaussPrev,        src_width*src_height,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_Sobel_imgPrev.RAW"),      Sobel_imgPrev,      src_width*src_height,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_harrisMapPrev.RAW"),      g_harrisMapPrev,    src_width*src_height,true);

    DumpCorners(GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_cornerPrev.RAW"),cornerPrev, MAXSizeHarrisCorner);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_CandRecPrevL.RAW"),CandRecPrevL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_OUTPUT_PATH,"00_CandRecPrevR.RAW"),CandRecPrevR, MAXCandidateNum);
    */
    #else
    DumpFrameHead(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_carState.DAT"), carState);
    DumpTimeManagement( GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_TimeManagement.DAT"),&i_TimeM,true ); 
    DumpTargetNum(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH, "00_TargetNum.DAT"),&i_target);

    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_src_Y.RAW"),              src_Y,              src_width*src_height,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_Gaussian_imgPrev.RAW"),   i_GaussPrev,        src_width*src_height,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_Sobel_imgPrev.RAW"),      Sobel_imgPrev,      src_width*src_height,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_harrisMapPrev.RAW"),      g_harrisMapPrev,    src_width*src_height,true);

    DumpCorners(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_cornerPrev.RAW"),cornerPrev, pointNumPrev);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_CandRecPrevL.RAW"),CandRecPrevL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_INPUT_PATH,"00_CandRecPrevR.RAW"),CandRecPrevR, MAXCandidateNum);

    #endif
#endif

}

/**
*@brief		show the Weather Type at canvas(Yellow: Day / Black:Night)
*/
void verify_WeatherType( UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY, weatherType_t isDay )
{
#if defined(_WIN32) &&( ON == OSD_verify_WeatherType)
    (Day == isDay)? (OSD_Ycc422::OSD_SetColor(244, 244, 5)) : (OSD_Ycc422::OSD_SetColor(0, 0, 0));
    OSD_Ycc422::OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,imgDimensionX-20,0,imgDimensionX,20,1,TRUE,1.0);
#endif

#if defined(_DUMP_OUTPUT_)
    INT32 tmp1 = (INT32)isDay;
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "02_weatherType.RAW" ),  &tmp1, 1);
#endif

}
/**
*@brief		Corners Information
*@param[in] harrisMap			Harris-corner Information Map
*@param[in] pointNumber			Harris-corner total number
*/
void verify_GaussianImg_HarrisCorner( UCHAR *dst_Y , UCHAR *Gaussian_img, UCHAR *harrisMap, UCHAR *sobel, UCHAR *vsobel, UCHAR *hsobel, UINT16 *harrisValue, 
                                      UINT16 *VSobelQty16x16, UINT16 *HSobelQty16x16,
                                      INT32 pointNumber , INT32 imgDimensionX , INT32 imgDimensionY )
{
#if defined(_WIN32) &&( ON == OSD_verify_GaussianImg_HarrisCorner)
    #if ( ON == OPENCV_EN ) 
        /// save image in opencv
        //OPENCV->UpdateCurGrayImg(Gaussian_img,imgDimensionX,imgDimensionY);

        // Frame Point Tracking.
        //OpencvPlug::getInstance()->GoodFeatureToTrack();

        // Frame Diff of Sobel
        //OpencvPlug::getInstance()->FrameDiff();
    #endif

    ///1) Show Harris Corner Number
	OSD_SetColor( 50, 222, 1 );
    OSD_Draw_Text( (CHAR*)"Corners" , dst_Y, imgDimensionX, imgDimensionY, 58, 0, 0);
    OSD_Draw_INT32( pointNumber , dst_Y, imgDimensionX, imgDimensionY,  65, 24, 3 ,12);

    /// Draw Harris corner point
    //INT32 Y_value    = 210;//147; ///< Harris corner Color_Y
    //INT32 CbCr_value = 50;//122; ///< Harris corner Color_CbCr
    //CopyHalfImgToOutput(dst_Y, imgDimensionX, imgDimensionY, harrisMap, imgDimensionX, imgDimensionY, 0, Y_value, CbCr_value, harrisMap);
    /// �i�H�NHarris Corner ø�s Rec
    INT32 drawWidth = 2;  
	OSD_SetColor(0, 250, 12);
    for(int row = 0; row<imgDimensionY; row++)
        for (int col=0; col<imgDimensionX; col++)
        {
            if(*( harrisMap + row * imgDimensionX + col )>=1)
            {
                OSD_Draw_Rec(dst_Y,imgDimensionX,imgDimensionY , col,row,col+(drawWidth>>1),row+(drawWidth>>1),1,TRUE,1);
            }
        }

    //[P.S.] Dump (UCHAR*)Sobel to Dst(YCbCr) 
    //CopyImgToDst(dst_Y,g_hSobel_img,imgDimensionX,imgDimensionY);
#endif

#if defined(_DUMP_OUTPUT_)
    //DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_Gaussian_img.RAW" ), Gaussian_img , imgDimensionX*imgDimensionY,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_harrisMap.RAW" ),    harrisMap    , imgDimensionX*imgDimensionY,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_sobel.RAW" ),        sobel        , imgDimensionX*imgDimensionY,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_vsobel.RAW" ),       vsobel       , imgDimensionX*imgDimensionY,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_hsobel.RAW" ),       hsobel       , imgDimensionX*imgDimensionY,true);
    DumpBitMap2(GetFullPathFileName(C_MOD_DUMP_PATH, "01_VSobelQty16x16.RAW"),  VSobelQty16x16, MAXCandidateNum*2);
    DumpBitMap2(GetFullPathFileName(C_MOD_DUMP_PATH, "01_HSobelQty16x16.RAW"),  HSobelQty16x16, MAXCandidateNum*2);
  #ifdef _WIN32
    DumpBitMap2(GetFullPathFileName(C_MOD_DUMP_PATH, "01_harrisValue.RAW" ),  harrisValue  , imgDimensionX*imgDimensionY,true);
  #endif
#endif
}

/**
*@brief		1. RegionL/R Draw the motion corner ROI
*@n			2. Draw the motion corner
*@n			3. Plot the motion corner Number
*@param[in] harrisCornersPrev	Harris-corner Queue
*@param[in] RegionL				motion corner detect ROI
*/
void verify_MotionCorner(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                         HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev,
                         const INT32  RegionL[] ,const INT32  RegionR[],
                         int pointNumPrev)
{
#if defined(_WIN32) &&( ON == OSD_verify_MotionCorner)

    ///0) ��ܶ©�
    // UCHAR BlackCanvas[WVGA_W*WVGA_H]={0};
    // CopyImgToDst(dst_Y,BlackCanvas,imgDimensionX,imgDimensionY); ///<�N g_img_FrameDiff ø�s�X �h �e��PLOT���N�B�\//

    /* TODO: Not show now
    ///2) Draw �����y���v
    int countMVL = 0;
    int countMVR = 0;
    OSD_SetColor(255, 0 , 0 );
    for( UINT32 i1 = 0 ; i1 < MAXSizeHarrisCorner; ++i1)
    {
        if( harrisCornersPrev[i1].isValid ) 
        {
int deb01 = harrisCornersPrev[i1].x;
int deb02 = harrisCornersPrev[i1].y;
int deb03 = harrisCornersPrev[i1].Diff_X;
int deb04 = harrisCornersPrev[i1].Diff_Y;
int deb05 = harrisCornersPrev[i1].moveCompensation_X;
int deb06 = harrisCornersPrev[i1].moveCompensation_Y;
int deb07 = harrisCornersPrev[i1].moveCompensation_x;
int deb08 = harrisCornersPrev[i1].moveCompensation_y;

OSD_SetColor(255, 0 , 0 );
OSD_Draw_Rec(dst_Y,imgDimensionX,imgDimensionY,542,274,542+5,274+5,1,true);

OSD_SetColor(255, 0 , 0 );

            if( harrisCornersPrev[i1].moveCompensation_X ==0 &&  harrisCornersPrev[i1].moveCompensation_Y ==0 )
                continue;
            OSD_Draw_Line(dst_Y , imgDimensionX, imgDimensionY, 
                         harrisCornersPrev[i1].x,harrisCornersPrev[i1].y,
                         harrisCornersPrev[i1].moveCompensation_x, harrisCornersPrev[i1].moveCompensation_y ,1);
            
            if( harrisCornersPrev[i1].x <= (imgDimensionX>>1) )
                countMVL ++ ;
            else
                countMVR ++ ;
        }
    }//end of ( i1 < MAXSizeHarrisCorner )
    */

    ///3) Draw Motion Estimation Lines
    int countMVL = 0;
    int countMVR = 0;
    countMVL = 0;
    countMVR = 0;
    OSD_SetColor(255, 255, 0);
    for( UINT32 i1 = 0 ; i1 < MAXSizeHarrisCorner; ++i1)
    {
        if( harrisCorners2Prev[i1].isValid ) 
        {
            /* Draw Rec
            OSD_Draw_Rec(dst_Y , imgDimensionX, imgDimensionY,
                harrisCornersPrev[i1].x-1,harrisCornersPrev[i1].y-1,
                harrisCornersPrev[i1].x+1, harrisCornersPrev[i1].y+1,1,TRUE);
            */
            
            OSD_Draw_Line(dst_Y , imgDimensionX, imgDimensionY, 
                         harrisCornersPrev[i1].x,harrisCornersPrev[i1].y,
                         harrisCornersPrev[i1].x+harrisCorners2Prev[i1].Diff_x, harrisCornersPrev[i1].y+harrisCorners2Prev[i1].Diff_y ,1);
            
            if( harrisCornersPrev[i1].x <= (imgDimensionX>>1) )
                countMVL ++ ;
            else
                countMVR ++ ;
        }
    }//end of ( i1 < MAXSizeHarrisCorner )

    ///4) Plot Motion Corner Number
    OSD_SetColor(255, 255, 0);
    OSD_Draw_Text( (CHAR*)"MotionCorner" , dst_Y, imgDimensionX, imgDimensionY, 0  , 35, 0);
    OSD_Draw_Text( (CHAR*)"MotionCorner" , dst_Y, imgDimensionX, imgDimensionY, imgDimensionX-100 , 35, 0);
    OSD_Draw_INT32( countMVL   , dst_Y, imgDimensionX, imgDimensionY, 10 , 60, 3, 12);
    OSD_Draw_INT32( countMVR   , dst_Y, imgDimensionX, imgDimensionY, imgDimensionX-100, 60, 3, 12);

    ///4) Debug for Motion vector statistic
/*
    INT32  VectorL[4]  = {0}; ///< Right:0 ,Up:1 , Left:2, Down:3
    INT32  VectorR[4]  = {0}; ///< Right:0 ,Up:1 , Left:2, Down:3
    INT32  VectorL_1[4]= {0};
    for( UINT32 i1 = 0 ; i1 < MAXSizeHarrisCorner; ++i1)
    {
        if( harrisCornersPrev[i1].isValid && 
            (harrisCornersPrev[i1].x<RegionL[2] &&(harrisCornersPrev[i1].x>RegionL[0]))&&
            (harrisCornersPrev[i1].y<RegionL[3] &&(harrisCornersPrev[i1].y>RegionL[1]))) 
        {
            VectorL[0] += (harrisCornersPrev[i1].Diff_x > 0)?(harrisCornersPrev[i1].Diff_x):0;
            VectorL[1] += (harrisCornersPrev[i1].Diff_y < 0)?(-harrisCornersPrev[i1].Diff_y):0;
            VectorL[2] += (harrisCornersPrev[i1].Diff_x < 0)?(-harrisCornersPrev[i1].Diff_x):0;
            VectorL[3] += (harrisCornersPrev[i1].Diff_y > 0)?(harrisCornersPrev[i1].Diff_y):0;

        }
        if( harrisCornersPrev[i1].isValid && 
            (harrisCornersPrev[i1].x<(RegionL[2]>>1) &&(harrisCornersPrev[i1].x>RegionL[0]))&&
            (harrisCornersPrev[i1].y<(RegionL[3]>>1) &&(harrisCornersPrev[i1].y>RegionL[1]))) 
        {
            VectorL_1[0] += (harrisCornersPrev[i1].Diff_x > 0)?(harrisCornersPrev[i1].Diff_x):0;
            VectorL_1[1] += (harrisCornersPrev[i1].Diff_y < 0)?(-harrisCornersPrev[i1].Diff_y):0;
            VectorL_1[2] += (harrisCornersPrev[i1].Diff_x < 0)?(-harrisCornersPrev[i1].Diff_x):0;
            VectorL_1[3] += (harrisCornersPrev[i1].Diff_y > 0)?(harrisCornersPrev[i1].Diff_y):0;

        }
        if( harrisCornersPrev[i1].isValid && 
            (harrisCornersPrev[i1].x<RegionR[2] &&(harrisCornersPrev[i1].x>=RegionR[0]))&&
            (harrisCornersPrev[i1].y<RegionR[3] &&(harrisCornersPrev[i1].y>=RegionR[1]))) 
        {
            VectorR[0] += (harrisCornersPrev[i1].Diff_x > 0)?(harrisCornersPrev[i1].Diff_x):0;
            VectorR[1] += (harrisCornersPrev[i1].Diff_y < 0)?(-harrisCornersPrev[i1].Diff_y):0;
            VectorR[2] += (harrisCornersPrev[i1].Diff_x < 0)?(-harrisCornersPrev[i1].Diff_x):0;
            VectorR[3] += (harrisCornersPrev[i1].Diff_y > 0)?(harrisCornersPrev[i1].Diff_y):0;

        }
    }//end of ( i1 < MAXSizeHarrisCorner )
    OSD_SetColor(255, 255, 0);
    OSD_Draw_INT32(VectorL[0], dst_Y , imgDimensionX , imgDimensionY , RegionL[2],RegionL[3]+10,1,10);
    OSD_Draw_INT32(VectorL[1], dst_Y , imgDimensionX , imgDimensionY , RegionL[2],RegionL[3]+25,1,10);
    OSD_Draw_INT32(VectorL[2], dst_Y , imgDimensionX , imgDimensionY , RegionL[2],RegionL[3]+40,1,10);
    OSD_Draw_INT32(VectorL[3], dst_Y , imgDimensionX , imgDimensionY , RegionL[2],RegionL[3]+55,1,10);
    OSD_Draw_INT32(VectorR[0], dst_Y , imgDimensionX , imgDimensionY , RegionR[0],RegionR[3]+10,1,10);
    OSD_Draw_INT32(VectorR[1], dst_Y , imgDimensionX , imgDimensionY , RegionR[0],RegionR[3]+25,1,10);
    OSD_Draw_INT32(VectorR[2], dst_Y , imgDimensionX , imgDimensionY , RegionR[0],RegionR[3]+40,1,10);
    OSD_Draw_INT32(VectorR[3], dst_Y , imgDimensionX , imgDimensionY , RegionR[0],RegionR[3]+55,1,10);

    OSD_Draw_INT32(VectorL_1[0], dst_Y , imgDimensionX , imgDimensionY , RegionL[2]-25,RegionL[3]+10,1,10);
    OSD_Draw_INT32(VectorL_1[1], dst_Y , imgDimensionX , imgDimensionY , RegionL[2]-25,RegionL[3]+25,1,10);
    OSD_Draw_INT32(VectorL_1[2], dst_Y , imgDimensionX , imgDimensionY , RegionL[2]-25,RegionL[3]+40,1,10);
    OSD_Draw_INT32(VectorL_1[3], dst_Y , imgDimensionX , imgDimensionY , RegionL[2]-25,RegionL[3]+55,1,10);
*/    

#endif

#if defined(_DUMP_OUTPUT_)
    DumpCorners(GetFullPathFileName(C_MOD_DUMP_PATH, "03_harrisValue.RAW" ),  harrisCornersPrev  ,  pointNumPrev);
    DumpCorners2(GetFullPathFileName(C_MOD_DUMP_PATH, "03_harrisValue2.RAW" ),  harrisCorners2Prev  , pointNumPrev);
#endif
}


/**
*@brief		Show the [Frame Difference of 2 Sobel images] on [dst_Y] 
*@param[in] g_img_FrameDiff			Frame Difference of 2 Sobel images
*@Note		Close it Generally
*/
void verify_FLT_ABSDiff( UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , UCHAR *g_img_FrameDiff , UCHAR *sobel, UCHAR *sobelPrev )
{
#if defined(_WIN32) &&( ON == OSD_verify_FLT_ABSDiff)
    //[P.S.] Dump g_img_FrameDiff to Dst 
    // CopyImgToDst(dst_Y,g_img_FrameDiff,imgDimensionX,imgDimensionY); ///<�N g_img_FrameDiff ø�s�X �h �e��PLOT���N�B�\//
    // OPENCV->ShowImg(g_img_FrameDiff,imgDimensionX,imgDimensionY);

    ///1) Show FrameDiff plot ���I
    OSD_SetColor(155 , 55 , 255);
    INT32 tmpIdx;
    for (INT32 vert = 0 ; vert < imgDimensionY ; ++vert)
    {
        tmpIdx = vert * imgDimensionX;
        for ( INT32 hori = 0 ; hori < imgDimensionX ;++hori)
        {
            if (g_img_FrameDiff[tmpIdx + hori] >=255)
            {
                OSD_Draw_Pixel(dst_Y,imgDimensionX,imgDimensionY,hori,vert,1.0);
            }
        }
    }

    #if ( ON == DBG_TXT)
    //FILE *fptr;
    //errno_t err=fopen_s(&fptr,"c:\\test\\CTA_output.txt","a");
    //fprintf(fptr,"idx(FrameNum)=%d, TH(Harris)=%d, PointNum=%d \n", FrameNum, TH, pointNumber);
    //fclose(fptr);
    #endif
#endif

#if defined(_DUMP_OUTPUT_)
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_img_FrameDiff.RAW" ),g_img_FrameDiff,  imgDimensionX*imgDimensionY,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_img_sobel_by_FrameDiff.RAW" ),sobel,            imgDimensionX*imgDimensionY,true);
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_img_sobelPrev_by_FrameDiff.RAW" ),sobelPrev,        imgDimensionX*imgDimensionY,true);

#endif

}
/**
*@brief		Draw the MinorBlock Type[1/2]
*@param[in] BlockSizeV			MinorBlock Size
*@param[in] CTAMinorBlockL		CTAMinorBlock information
*@param[in] RegionL				motion corner detect ROI
*/
void verify_MinorBlockUpdate(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                            INT16 BlockSizeV , INT16 BlockSizeH, MinorBlock *CTAMinorBlockL, MinorBlock *CTAMinorBlockR, const INT32 RegionL[] , const INT32 RegionR[])
{
#if defined(_WIN32) &&( ON == OSD_verify_MinorBlockUpdate)
    /// �ŦǤp�� Shows the MinorBlocks Types
    FLOAT opaque = 0.3;
    for ( int vert = 0 ; vert < ROISegBlockNum_V ; ++vert)
    {
        for ( int hori = 0 ; hori < ROISegBlockNum_H ; ++hori)
        {
            // Left
            if ( (CTAMinorBlockL[ hori+vert*ROISegBlockNum_H ].Type == MinorBlockType1)  ) // Satisfied all condition 
            {
                OSD_Ycc422::OSD_SetColor(12, 133, 234); // BLUE
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX , imgDimensionY , 
                    RegionL[0]+BlockSizeH*hori, RegionL[1]+BlockSizeV*vert,\
                    RegionL[0]+BlockSizeH*(hori+1)-1,RegionL[1]+BlockSizeV*(vert+1)-1 ,1, FALSE , 0.9);
            }
            else if ( (CTAMinorBlockL[ hori+vert*ROISegBlockNum_H ].Type == MinorBlockType2)  ) // Only Satisfied THD_MotionEdgeQ
            {
                OSD_Ycc422::OSD_SetColor(144, 144, 145); // GRAY
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    RegionL[0]+BlockSizeH*hori, RegionL[1]+BlockSizeV*vert,\
                    RegionL[0]+BlockSizeH*(hori+1)-1,RegionL[1]+BlockSizeV*(vert+1)-1 ,1, FALSE , opaque);
            }

            /// Right
            if ( (CTAMinorBlockR[ hori+vert*ROISegBlockNum_H ].Type == MinorBlockType1)  ) // Satisfied all condition 
            {
                OSD_Ycc422::OSD_SetColor(12, 133, 234);// BLUE
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    RegionR[0]+BlockSizeH*hori, RegionR[1]+BlockSizeV*vert,\
                    RegionR[0]+BlockSizeH*(hori+1)-1,RegionR[1]+BlockSizeV*(vert+1)-1 ,1, FALSE , 0.9);
            }
            else if ( (CTAMinorBlockR[ hori+vert*ROISegBlockNum_H ].Type == MinorBlockType2)  ) // Only Satisfied THD_MotionEdgeQ
            {
                OSD_Ycc422::OSD_SetColor(144, 144, 145);// GRAY
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX , imgDimensionY , 
                    RegionR[0]+BlockSizeH*hori, RegionR[1]+BlockSizeV*vert,\
                    RegionR[0]+BlockSizeH*(hori+1)-1,RegionR[1]+BlockSizeV*(vert+1)-1 ,1, FALSE , opaque);
            }
        }
    } //end of vert
#endif

#if defined(SYNC_ACROSS_PLATFORM)

    DumpMinorBlock(GetFullPathFileName(C_MOD_DUMP_PATH, "04_MinorBlockL.DAT" ),  CTAMinorBlockL, MAXCandidateNum);
    DumpMinorBlock(GetFullPathFileName(C_MOD_DUMP_PATH, "04_MinorBlockR.DAT" ),  CTAMinorBlockR, MAXCandidateNum);

#endif
}

/**
*@brief		Plot the label Number [ MinorBlocks unite into Candidate ]
*@param[in] LabelCountL			Candidate count
*/
void verify_Labeling(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                     INT16 BlockSizeH , INT16 BlockSizeV,
                     const INT32 RegionL[] , const INT32 RegionR[],
                     UINT16 LabelCountL , UINT16 LabelCountR,
                     MinorBlock *CTAMinorBlockL , MinorBlock *CTAMinorBlockR,
                     UINT16 *CTAMinorBlockL_EdgeQuantity, UINT16 *CTAMinorBlockR_EdgeQuantity)
{
#if defined(_WIN32) &&( ON == OSD_verify_Labeling)

    /// 1) CTAMinorBlockL/R �̦P�s�դ��C�� (�i�����)
    /*
    INT32 tmplabel;
    for ( INT32 vert = 0; vert < ROISegBlockNum ;++vert)
    {
        for (INT32 hori = 0; hori < ROISegBlockNum ;++hori)
        {
            tmplabel = CTAMinorBlockL[hori + vert*ROISegBlockNum].GroupID;
            if( 0 != tmplabel  )
            {
                INT32 graylevel = tmplabel * 20 + 50;
                OSD_SetColor(graylevel, graylevel*1.1, graylevel*1.2);
                OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,
                    RegionL[0]+hori*BlockSizeH,
                    RegionL[1]+vert*BlockSizeV,
                    RegionL[0]+(hori+1)*BlockSizeH,
                    RegionL[1]+(vert+1)*BlockSizeV,3,true);
            }
            tmplabel = CTAMinorBlockR[hori + vert*ROISegBlockNum].GroupID;
            if( 0 != tmplabel  )
            {
                INT32 graylevel = tmplabel * 20 + 50;
                OSD_SetColor(graylevel,graylevel*1.1,graylevel*1.2);
                OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,
                    RegionR[0]+hori*BlockSizeH,
                    RegionR[1]+vert*BlockSizeV,
                    RegionR[0]+(hori+1)*BlockSizeH,
                    RegionR[1]+(vert+1)*BlockSizeV,3,true);
            }
        }
    }
    */
    /// 2) �p�� CTAMinorBlockL/R �� �㦳�h�ָs�աC( LabelCountL/R�O Maximun Label Idx )
    INT32 countL = 0;
    INT32 countR = 0;
    ///Left
    for( INT32 i1 = 1; i1 <= LabelCountL ; ++i1 )
    {
        for ( INT32 vert = 0; vert < ROISegBlockNum_V ;++vert)
        {
            for (INT32 hori = 0; hori < ROISegBlockNum_H ;++hori)
            {
                if( i1 == CTAMinorBlockL[hori + vert*ROISegBlockNum_H].GroupID  )
                {
                    countL++;
                    vert = ROISegBlockNum_V;
                    hori = ROISegBlockNum_H;
                }
            }
        }
    }
    ///Right
    for( INT32 i1 = 1; i1 <= LabelCountR ; ++i1 )
    {
        for ( INT32 vert = 0; vert < ROISegBlockNum_V ;++vert)
        {
            for (INT32 hori = 0; hori < ROISegBlockNum_H ;++hori)
            {
                if( i1 == CTAMinorBlockR[hori + vert*ROISegBlockNum_H].GroupID  )
                {
                    countR++;
                    vert = ROISegBlockNum_V;
                    hori = ROISegBlockNum_H;
                }
            }
        }
    }

    ///3) Label �϶� Numbers (�Ĥ@���ؿ�ϰ�ƥ�).
    OSD_Ycc422::OSD_SetColor(230, 0, 230);
    OSD_Ycc422::OSD_Draw_Text( (CONST char*)"Obj" , dst_Y, imgDimensionX, imgDimensionY, 0, 70, 0);
    OSD_Ycc422::OSD_Draw_Text( (CONST char*)"Obj" , dst_Y, imgDimensionX, imgDimensionY, imgDimensionX-140, 70, 0);
    OSD_Ycc422::OSD_Draw_INT32( countL , dst_Y , imgDimensionX, imgDimensionY,  10, 90, 3 ,12);
    OSD_Ycc422::OSD_Draw_INT32( countR , dst_Y , imgDimensionX, imgDimensionY, imgDimensionX-130, 90, 3 ,12);

    #if ( ON == DBG_TXT)
    //FILE *fptr;
    //errno_t err=fopen_s(&fptr,"c:\\test\\CTA_output.txt","a");
    //fprintf(fptr,"idx(FrameNum)=%d, TH(Harris)=%d, PointNum=%d \n", FrameNum, TH, pointNumber);
    //fclose(fptr);
    #endif
#endif  

#if defined(_DUMP_OUTPUT_)

    DumpMinorBlock(GetFullPathFileName(C_MOD_DUMP_PATH, "05_MinorBlockL.DAT" ),  CTAMinorBlockL, MAXCandidateNum);
    DumpMinorBlock(GetFullPathFileName(C_MOD_DUMP_PATH, "05_MinorBlockR.DAT" ),  CTAMinorBlockR, MAXCandidateNum);
    DumpBitMap2(GetFullPathFileName(C_MOD_DUMP_PATH, "05_MinorBlockL_EdgeQuantity.DAT" ),  CTAMinorBlockL_EdgeQuantity, MAXCandidateNum);
    DumpBitMap2(GetFullPathFileName(C_MOD_DUMP_PATH, "05_MinorBlockR_EdgeQuantity.DAT" ),  CTAMinorBlockR_EdgeQuantity, MAXCandidateNum);

    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "05_MinorBlockLLabelNum.DAT" ),  &LabelCountL,1);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "05_MinorBlockRLabelNum.DAT" ),  &LabelCountR,1);

#endif
}

/**
*@brief		Plot the Candidate Number after Filter I.
*@param[in] CandidateCountL			Candidate count after Filter I.
*/
void verify_CandidateUpdate(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
    UINT16 CandidateCountL , UINT16 CandidateCountR , CandidateRec *CTACandidateL , CandidateRec *CTACandidateR )
{
#if defined(_WIN32) &&( ON == OSD_verify_CandidateUpdate)
    ///1) Candidate Number
    OSD_SetColor(255, 13, 255);
    OSD_Draw_Text( (CHAR*)"FilterI" , dst_Y , imgDimensionX, imgDimensionY, 38, 70, 0);
    OSD_Draw_Text( (CHAR*)"FilterI" , dst_Y , imgDimensionX, imgDimensionY, imgDimensionX-110, 70, 0);
    OSD_Draw_INT32( CandidateCountL , dst_Y , imgDimensionX, imgDimensionY,  49, 90, 3 ,12);
    OSD_Draw_INT32( CandidateCountR , dst_Y , imgDimensionX, imgDimensionY, imgDimensionX-100, 90, 3 ,12);
    
    /// BLUE Candidate Rectangles
    OSD_SetColor(0, 25, 255); ///< BLUE
    for ( int i1 = 0 ; i1< MAXCandidateNum ; ++i1)
    {
        /// Left
        if( Invalid != CTACandidateL[i1].isValid )
        {	
            OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                CTACandidateL[i1].startxi , CTACandidateL[i1].startyi,\
                CTACandidateL[i1].startxi+CTACandidateL[i1].width ,CTACandidateL[i1].startyi+CTACandidateL[i1].height ,3,FALSE);
        }

        /// Right
        if( Invalid != CTACandidateR[i1].isValid )
        {	
            OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                CTACandidateR[i1].startxi , CTACandidateR[i1].startyi,\
                CTACandidateR[i1].startxi+CTACandidateR[i1].width ,CTACandidateR[i1].startyi+CTACandidateR[i1].height ,3,FALSE);
        }
    }


#endif


#if defined(_DUMP_OUTPUT_) 

    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "06_CTACandidateL.DAT" ),  CTACandidateL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "06_CTACandidateR.DAT" ),  CTACandidateR, MAXCandidateNum);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "06_CandidateCountL.DAT" ),  &CandidateCountL,1);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "06_CandidateCountR.DAT" ),  &CandidateCountR,1);

#endif
}

/**
*@brief		OFFLINE -machine learn DATA_Export(Image & features)
*@param[in] CandidateCountL			Candidate count after Filter I.
*/
void verify_CandidateFeatureUpdate( UCHAR *Gaussian_img , INT32 imgDimensionX , INT32 imgDimensionY , CandidateRec *CTACandidateL , CandidateRec *CTACandidateR, CONST INT32 FrameNo )
{
#if defined(_WIN32) &&( ON == OSD_verify_CandidateFeatureUpdate)
    using namespace std;
    ///OFFLINE_DATA_Export
    #if( ON == OFFLINE_TRAIN_DUMPIMG)
        #if( OFF == OPENCV_EN )
        assert(!"Need OpenCV ENABLE");
        #endif
    
    ///1) Initial PC File
    std::ofstream fout;
    {
        string fpath  = VideoFilePath;
        /// Extract FileName (ex. 105_20130801_00001)
        INT32 posh = fpath.rfind("\\");
        INT32 pose = fpath.rfind(".");
        INT32 len = fpath.length();
        string fname = fpath.substr(posh+1,pose-posh-1);
        string fname2 = "C:\\CTA_OFFLINE_TRAIN_DATA\\";
        fname2 += fname; fname2+="_Features.csv";  //ex.C:\CTA_OFFLINE_TRAIN_DATA\102_20150127_16_44_41_624_Features.csv

        fout.open(fname2.c_str(), std::ios::app);
    }
    if(!fout){assert(!"open file fail");}

    ///2) Save Img & RecFeatures to PC File
    for ( int i1 = 0 ; i1 < MAXCandidateNum ; ++i1)
    {
        if( Valid == CTACandidateL[i1].isValid )
        {
            ///2.1) save image
            string fpath = VideoFilePath;
            /// Extract FileName (ex. 105_20130801_00001)
            INT32 posh = fpath.rfind("\\");
            INT32 pose = fpath.rfind(".");
            INT32 len = fpath.length();
            string fname = fpath.substr(posh+1,pose-posh-1);

            /// Get save filename
            char   Idx[30];
            sprintf( Idx ,"%d",IdxSaveImg);
            fname += "_";
            fname += Idx;
            sprintf( Idx ,"%d",FrameNo);
            fname += "_";
            fname += Idx;
            fname += ".jpg"; //ex. 105_20130801_00001_1000001.jpg
            OPENCV->SaveImg(Gaussian_img,imgDimensionX,imgDimensionY, 
                CTACandidateL[i1].startxi , CTACandidateL[i1].startyi , CTACandidateL[i1].width , CTACandidateL[i1].height ,
                fname.c_str());

            ///2.2) save RecFeatures.
            fout<<fname<<",";
            for ( INT32 i2 = 0 ; i2 < 24 ; ++i2)
            {
                FLOAT data01 = CTACandidateL[i1].RecFeatures[ i2 ];
                fout<<data01<<","; /// 100001.jpg
            }
            fout<<endl;
            IdxSaveImg++;
        }
        if( Valid == CTACandidateR[i1].isValid )
        {
            ///2.1) save image
            string fpath = VideoFilePath;
            /// Extract FileName (ex. 105_20130801_00001)
            INT32 posh = fpath.rfind("\\");
            INT32 pose = fpath.rfind(".");
            INT32 len = fpath.length();
            string fname = fpath.substr(posh+1,pose-posh-1);

            /// Get save filename
            char   Idx[30];
            sprintf( Idx ,"%d",IdxSaveImg);
            fname += "_";
            fname += Idx;
            sprintf( Idx ,"%d",FrameNo);
            fname += "_";
            fname += Idx;
            fname += ".jpg"; //ex. 105_20130801_00001_1000001_233.jpg

            OPENCV->SaveImg(Gaussian_img,imgDimensionX,imgDimensionY, 
                CTACandidateR[i1].startxi , CTACandidateR[i1].startyi ,
                CTACandidateR[i1].width, CTACandidateR[i1].height ,
                fname.c_str());

            /// 2.2) save RecFeatures
            fout<<fname<<","; //100001.jpg
            for ( INT32 i2 = 0 ; i2 < 24 ; ++i2)
            {
                FLOAT data01 = CTACandidateR[i1].RecFeatures[ i2 ];
                fout<<data01<<","; /// 100001.jpg
            }
            fout<<endl;
            IdxSaveImg++;
        }
    }
    #endif
#endif


#if defined(_DUMP_OUTPUT_) 

    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "07_CTACandidateL.DAT" ),  CTACandidateL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "07_CTACandidateR.DAT" ),  CTACandidateR, MAXCandidateNum);

#endif
}
/**
*@brief		Draw "White" Rectangle of Valid Candidate after Filter II.
*@param[in] CandidateCountL			Candidate count after Filter II.
*/
void verify_CNNPredict(UCHAR *dst_Y ,INT32 imgDimensionX,INT32 imgDimensionY, UINT16 CandidateCountL, UINT16 CandidateCountR,
                              CandidateRec *CTACandidateL , CandidateRec *CTACandidateR )
{
#if defined(_WIN32) &&( ON == OSD_verify_CNNPredict)
    /// Candidate Number after FilterII
    OSD_Ycc422::OSD_SetColor(0xFF, 152, 203);
    OSD_Ycc422::OSD_Draw_Text( (const char*)"II"      , dst_Y , imgDimensionX, imgDimensionY, 109, 70, 0); ///< FilterII
    OSD_Ycc422::OSD_Draw_Text( (const char*)"II"      , dst_Y , imgDimensionX, imgDimensionY, imgDimensionX-40, 70, 0); ///< FilterII
    OSD_Ycc422::OSD_Draw_INT32( CandidateCountL , dst_Y , imgDimensionX, imgDimensionY,  120, 90, 3 ,12); //PredictCanValidL??
    OSD_Ycc422::OSD_Draw_INT32( CandidateCountR , dst_Y , imgDimensionX, imgDimensionY, imgDimensionX-30, 90, 3 ,12); //PredictCanValidR??

    /// White Candidate Rectangles after ( Filter II )
    /*
    OSD_SetColor(255, 255, 255);
    for ( int i1 = 0 ; i1< ROISegBlockNum*ROISegBlockNum ; ++i1)
    {
        /// Left
        if( TRUE == CTACandidateL[i1].isValid )
        {	
            OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                CTACandidateL[i1].startxi , CTACandidateL[i1].startyi,\
                CTACandidateL[i1].startxi+CTACandidateL[i1].width ,CTACandidateL[i1].startyi+CTACandidateL[i1].height ,1,FALSE);
        }

        /// Right
        if( TRUE == CTACandidateR[i1].isValid )
        {	
            OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                CTACandidateR[i1].startxi , CTACandidateR[i1].startyi,\
                CTACandidateR[i1].startxi+CTACandidateR[i1].width ,CTACandidateR[i1].startyi+CTACandidateR[i1].height ,1,FALSE);
        }
    }
    */
#endif

#if defined(_DUMP_OUTPUT_)

    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "08_CTACandidateL.DAT" ),  CTACandidateL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "08_CTACandidateR.DAT" ),  CTACandidateR, MAXCandidateNum);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "08_CandidateCountL.DAT" ),  &CandidateCountL,1);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "08_CandidateCountR.DAT" ),  &CandidateCountR,1);

#endif
}

/**
*@brief		1.Plot Dynamic Max Flag
*@n			2.Plot ObjInfo [Flag/Posx/PosX/Speed/TTC]
*@param[in] CountDownFlagAdpL		Dynamic Max Flag
*/
void verify_Estimation( UCHAR * dst_Y,INT32 imgDimensionX,INT32 imgDimensionY, const INT32 *RegionL, const INT32 *RegionR, CandidateRec *CTACandidateL , CandidateRec *CTACandidateR , CandidateRec *CTACandidatePrevL, CandidateRec *CTACandidatePrevR,INT16 CountDownFlagAdpL , INT16 CountDownFlagAdpR,
                        TARGET_NUM *pTargetNum)
{
#if defined(_WIN32) &&( ON == OSD_verify_Estimation)
    ///------------------------Adaptive_MaxCountDownFlag------------------------///
    ///1) Dynamic Max Flag
    /* Not Show
    OSD_SetColor(255, 0 , 0);
    OSD_Draw_INT32( CountDownFlagAdpL , dst_Y , imgDimensionX, imgDimensionY,
        RegionL[2],
        RegionL[3]+3,
        1 , 10 ); 
    OSD_Draw_INT32( CountDownFlagAdpR , dst_Y , imgDimensionX, imgDimensionY,
        RegionR[0]-10,
        RegionR[3]+3,
        1 , 10 ); 
    */
    ///------------------------ObjInfo------------------------------------------///
    /// RULLER
    #if defined(_WIN32) &&( ON == OSD_RULLER)
    OSD_SetColor(0,0,255);
    OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,
                0,imgDimensionY*1/5.5, (imgDimensionX),imgDimensionY*1/5.5,1,true ); 
    OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,
                (imgDimensionX>>1),0, (imgDimensionX>>1),imgDimensionY,1,true ); 
    OSD_SetColor(255,0xFF,0);
    for(int i1 = 0 ; i1 < imgDimensionX ; i1+= 100)
        OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY,i1-2,imgDimensionY*1/5.5, i1+2,imgDimensionY*1/5.5,1,true ); 
    for(int i1 = 0 ; i1 < imgDimensionY ; i1+= 100)
        OSD_Draw_Rec(dst_Y,imgDimensionX, imgDimensionY, (imgDimensionX>>1),i1-2, (imgDimensionX>>1),i1+2,1,true ); 

    #endif
    
    /// draw Candidate Info(from top to toe is countDown,PosXY,SpeedXY,TTC)+ Motioncorner at right-bottom side of ObjRec
    CONST UINT16 InfoWordSize = 7;
    
    /// Black Canvas
    OSD_Ycc422::OSD_SetColor(0, 0, 0);
    OSD_Ycc422::OSD_Draw_Rec( dst_Y,imgDimensionX, imgDimensionY,0,imgDimensionY*4/5, (imgDimensionX>>1)*4/5,imgDimensionY,1,true ); 
    OSD_Ycc422::OSD_Draw_Rec( dst_Y,imgDimensionX, imgDimensionY,(imgDimensionX>>1)*6/5,imgDimensionY*4/5, imgDimensionX, imgDimensionY,1,true ); 

    for ( INT16 i1 = 0 ; i1< MAXCandidateNum ; ++i1)
    {
#pragma region LeftSide
        /// Left
        if( CTACandidateL[i1].isValid!=Invalid )//draw UI at <Startup> flag
        {	
            /// Position (Draw_Rec xiyi ) & (Draw_INT32 Xw(cm))
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
#if UI_DRAW_CAR_HEAD == ON    /// car head( ObjPosxy )                 
            /// car head( ObjPosxy )
            OSD_Ycc422::OSD_SetColor(255, 8, 8);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY,CTACandidateL[i1].ObjPosxy[0]-1,CTACandidateL[i1].ObjPosxy[1]-2,
                CTACandidateL[i1].ObjPosxy[0]+1,CTACandidateL[i1].ObjPosxy[1]+1,3,TRUE);
#endif            
#if UI_DRAW_TRACK_PT==ON	//TrackPoint
            if(CTACandidateL[i1].isInReviveMode==PureTracking)
                OSD_Ycc422::OSD_SetColor(255, 128, 8);//TrackPoint(ObjTrackPosxy)
            else
                OSD_Ycc422::OSD_SetColor(201,119, 255);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY,CTACandidateL[i1].ObjTrackPosxy[0],CTACandidateL[i1].ObjTrackPosxy[1]-1,
                CTACandidateL[i1].ObjTrackPosxy[0]+1,CTACandidateL[i1].ObjTrackPosxy[1]+1,3,TRUE);
#endif
            /// ( PosXYKeepTime )
            OSD_Ycc422::OSD_Draw_INT32( CTACandidateL[i1].PosXYKeepTime , dst_Y , imgDimensionX , imgDimensionY,
                CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*InfoWordSize,
                CTACandidateL[i1].startyi+CTACandidateL[i1].height+5,
                1 , InfoWordSize ); 
        }

        if( CTACandidateL[i1].isValid!=Invalid )//also draw UI at <Startup> flag
        {
            /// Plot Xw( ObjPosXY[0] )
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( CTACandidateL[i1].ObjPosXY[0] , dst_Y , imgDimensionX, imgDimensionY ,
                CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*InfoWordSize,
                CTACandidateL[i1].startyi+CTACandidateL[i1].height+5+InfoWordSize+3,
                1 , InfoWordSize ); 

            /// Speed(km/hr)
            FLOAT speedKmHr = SPEED_CONST_KMH * CTACandidateL[i1].ObjSpeedXY[0];
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*InfoWordSize,
                            CTACandidateL[i1].startyi+CTACandidateL[i1].height+5+2*(InfoWordSize+3),
                            1 , InfoWordSize ); 

            /// TTC
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( CTACandidateL[i1].CandTTC , dst_Y , imgDimensionX, imgDimensionY,
                            CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*InfoWordSize,
                            CTACandidateL[i1].startyi+CTACandidateL[i1].height+5+3*(InfoWordSize+3),
                            1 , InfoWordSize ); 
/* ����ܤ覡
            /// ��ܤU��°ϰ�
            CONST INT32 WordSizeAtBot = 18;
            // POS(m)
            OSD_SetColor(255, 255, 0);
            INT32 PosX =  CTACandidateL[i1].ObjPosXY[0]/100;
            OSD_Draw_INT32( PosX , dst_Y , imgDimensionX, imgDimensionY ,
                            CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*WordSizeAtBot,
                            SrcImg_H-85,
                            3 , WordSizeAtBot ); 

            /// Speed(km/hr)
            OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*WordSizeAtBot,
                            SrcImg_H-60,
                            3 , WordSizeAtBot ); 

            /// TTC (sec/10)
            INT32 ttc = CTACandidateL[i1].CandTTC/100;
            OSD_Draw_INT32( ttc , dst_Y , imgDimensionX, imgDimensionY,
                            CTACandidateL[i1].startxi+CTACandidateL[i1].width-2*WordSizeAtBot,
                            SrcImg_H-36,
                            3 , WordSizeAtBot ); 
*/
            /* more detail
            /// Motion Corners
            OSD_Draw_INT32( CTACandidateL[i1].CornerQuantity , dst_Y , imgDimensionX , imgDimensionY,
                CTACandidateL[i1].startxi,
                CTACandidateL[i1].startyi+CTACandidateL[i1].height+5,
                1 , InfoWordSize ); 
            */
        }
#pragma endregion

#pragma region  RightSide
        /// Right
        if( CTACandidateR[i1].isValid!=Invalid )//also draw UI at <Startup> flag
        {	
            /// Position (Draw_Rec xiyi ) & (Draw_INT32 Xw(cm))		
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
#if UI_DRAW_CAR_HEAD==ON   /// car head            
            OSD_Ycc422::OSD_SetColor(255, 8, 8);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY,CTACandidateR[i1].ObjPosxy[0]-1,CTACandidateR[i1].ObjPosxy[1]-2,
                CTACandidateR[i1].ObjPosxy[0]+1,CTACandidateR[i1].ObjPosxy[1]+1,3,TRUE);
#endif
#if UI_DRAW_TRACK_PT==ON		//TrackPoint
            if(CTACandidateR[i1].isInReviveMode==PureTracking)
                OSD_Ycc422::OSD_SetColor(255, 128, 0);
            else
                OSD_Ycc422::OSD_SetColor(201, 119, 255);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY,CTACandidateR[i1].ObjTrackPosxy[0],CTACandidateR[i1].ObjTrackPosxy[1]-1,
                CTACandidateR[i1].ObjTrackPosxy[0]+1,CTACandidateR[i1].ObjTrackPosxy[1]+1,3,TRUE);
#endif
            /// PosXYKeepTime
            OSD_Ycc422::OSD_Draw_INT32( CTACandidateR[i1].PosXYKeepTime , dst_Y , imgDimensionX , imgDimensionY ,
                            CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*InfoWordSize,
                            CTACandidateR[i1].startyi+CTACandidateR[i1].height+5,
                            1 , InfoWordSize ); 
        }

        if( CTACandidateR[i1].isValid!=Invalid )//also draw UI at <Startup> flag
        {
            /// Plot Xw ( ObjPosXY[0] )
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( CTACandidateR[i1].ObjPosXY[0] , dst_Y , imgDimensionX , imgDimensionY,
                CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*InfoWordSize,
                CTACandidateR[i1].startyi+CTACandidateR[i1].height+5+InfoWordSize+3,
                1 , InfoWordSize ); 

            /// Speed(km/hr)
            FLOAT speedKmHr = SPEED_CONST_KMH * CTACandidateR[i1].ObjSpeedXY[0];
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY , // +:closed -:away
                CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*InfoWordSize,
                CTACandidateR[i1].startyi+CTACandidateR[i1].height+5+2*(InfoWordSize+3),
                1 , InfoWordSize ); 

            /// TTC
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( CTACandidateR[i1].CandTTC , dst_Y , imgDimensionX , imgDimensionY ,
                CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*InfoWordSize,
                CTACandidateR[i1].startyi+CTACandidateR[i1].height+5+3*(InfoWordSize+3),
                1 , InfoWordSize ); 

/* ����ܤ覡
            /// ��ܤU��°ϰ�
            const INT32 WordSizeAtBot = 18;
            // POS(m)
            OSD_SetColor(255, 255, 0);
            INT32 PosX =  CTACandidateR[i1].ObjPosXY[0]/100;
            OSD_Draw_INT32( PosX , dst_Y , imgDimensionX, imgDimensionY ,
                            CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*WordSizeAtBot,
                            SrcImg_H-85,
                            3 , WordSizeAtBot ); 

            /// Speed(km/hr)
            OSD_SetColor(255, 255, 0);
            OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*WordSizeAtBot,
                            SrcImg_H-60,
                            3 , WordSizeAtBot ); 

            /// TTC (sec/10)
            INT32 ttc = CTACandidateR[i1].CandTTC/100;
            OSD_SetColor(255, 255, 0);
            OSD_Draw_INT32( ttc , dst_Y , imgDimensionX, imgDimensionY,
                            CTACandidateR[i1].startxi+CTACandidateR[i1].width-2*WordSizeAtBot,
                            SrcImg_H-36,
                            3 , WordSizeAtBot ); 
*/
            /* more detail
            /// Motion Corners
            OSD_Draw_INT32( CTACandidateR[i1].CornerQuantity , dst_Y , imgDimensionX , imgDimensionY,
                CTACandidateR[i1].startxi,
                CTACandidateR[i1].startyi+CTACandidateR[i1].height+5,
                1 , InfoWordSize ); 
            */
        }
#pragma endregion
    }

    /// display bottom UI black zone  
    INT32 idxL[3] = {((SrcImg_W>>1)-120) ,((SrcImg_W>>1)-190),((SrcImg_W>>1)-260)};
    INT32 idxR[3] = {((SrcImg_W>>1)+70) ,((SrcImg_W>>1)+140),((SrcImg_W>>1)+210)};
    INT32 tmpIdx = 0;
    for ( INT16 i1 = 0 ; i1< MAXCandidateNum ; ++i1)
    {
        /// Left
        if( CTACandidateL[i1].isValid==Valid )
        {	
            CONST INT32 WordSizeAtBot = 14;
           
            FLOAT speedKmHr = SPEED_CONST_KMH * CTACandidateL[i1].ObjSpeedXY[0];
            // POS(m)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            INT32 PosX =  CTACandidateL[i1].ObjPosXY[0]/100;
            OSD_Ycc422::OSD_Draw_INT32( PosX , dst_Y , imgDimensionX, imgDimensionY ,
                            idxL[ tmpIdx ],
                            SrcImg_H-85,
                            3 , WordSizeAtBot ); 

            /// Speed(km/hr)
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            idxL[ tmpIdx ],
                            SrcImg_H-60,
                            3 , WordSizeAtBot ); 

            /// TTC (sec/10)
            INT32 ttc = CTACandidateL[i1].CandTTC/100;
            OSD_Ycc422::OSD_Draw_INT32( ttc , dst_Y , imgDimensionX, imgDimensionY,
                            idxL[ tmpIdx ],
                            SrcImg_H-36,
                            3 , WordSizeAtBot ); 

           ++tmpIdx;
        }
        if(tmpIdx>2)
            break;
    }
    tmpIdx = 0;
    for ( INT16 i1 = 0 ; i1< MAXCandidateNum ; ++i1)
    {
        /// Right
        if( CTACandidateR[i1].isValid==Valid )
        {	
            const INT32 WordSizeAtBot = 14;

            // POS(m)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            INT32 PosX =  CTACandidateR[i1].ObjPosXY[0]/100;

            OSD_Ycc422::OSD_Draw_INT32( PosX , dst_Y , imgDimensionX, imgDimensionY ,
                            idxR[tmpIdx],
                            SrcImg_H-85,
                            3 , WordSizeAtBot ); 

            /// Speed(km/hr)
            FLOAT speedKmHr = SPEED_CONST_KMH * CTACandidateR[i1].ObjSpeedXY[0];
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( (-1)*speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                idxR[tmpIdx],
                SrcImg_H-60,
                3 , WordSizeAtBot ); 

            /// TTC (sec/10)
            INT32 ttc = CTACandidateR[i1].CandTTC/100;
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( ttc , dst_Y , imgDimensionX, imgDimensionY,
                idxR[tmpIdx],
                SrcImg_H-36,
                3 , WordSizeAtBot ); 
            ++tmpIdx;
        }
        if(tmpIdx>2)
            break;
    }
    

#endif

#if defined(_DUMP_OUTPUT_) 

    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "09_CTACandidateL.DAT" ),  CTACandidateL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "09_CTACandidateR.DAT" ),  CTACandidateR, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "09_CTACandidatePrevL.DAT" ),  CTACandidatePrevL, MAXCandidateNum);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "09_CTACandidatePrevR.DAT" ),  CTACandidatePrevR, MAXCandidateNum);

#endif

}

/**
*@brief		Show Image/Audio Warning Info
*@param[in] RegionL			Image processing ROI
*@param[in] CTACandidateL	Objects Info.
*@param[in] isAlarmL		is CTADog bowwow
*/
void verify_CTADog( UCHAR *dst_Y , INT32 imgDimensionX,INT32 imgDimensionY, const INT32 *RegionL, const INT32 *RegionR, CandidateRec *CTACandidateL , CandidateRec *CTACandidateR , INT32 FrameNum )
{
#if defined(_WIN32) &&( ON == OSD_verify_CTADog)

    /// UI display for Customer.

    #pragma region "ImageWarning - Rectangle"
    ///1) ImageWarning - Rectangle
    //========================================================================
    //  White    Rec: Pass 2 Filters -[Ref: verify_CNNPredict]: it should be a reasonable candidate
    //  Red      Rec: isAlarm == TRUE
    //========================================================================
    CONST INT32 AddSize = 0;
    for ( int i1 = 0 ; i1 < MAXCandidateNum ; ++i1)
    {
        /// Left
        if( Valid == CTACandidateL[i1].isValid )
        {	
            /// Draw White Rec Pass 2 Filters 
            OSD_Ycc422::OSD_SetColor(255, 255, 255);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                CTACandidateL[i1].startxi-AddSize , CTACandidateL[i1].startyi,\
                CTACandidateL[i1].startxi+CTACandidateL[i1].width+AddSize , CTACandidateL[i1].startyi+CTACandidateL[i1].height ,1,FALSE);

            /// Draw Red Rec
            if( ( TRUE == CTACandidateL[i1].isAlarm ) )
            {
                OSD_Ycc422::OSD_SetColor(0xFF, 0, 0);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    CTACandidateL[i1].startxi , CTACandidateL[i1].startyi,\
                    CTACandidateL[i1].startxi+CTACandidateL[i1].width ,CTACandidateL[i1].startyi+CTACandidateL[i1].height ,1,FALSE);
            }
        }

        /// Right
        if( Valid == CTACandidateR[i1].isValid )
        {	
            /// Draw White Rec Pass 2 Filters 
            OSD_Ycc422::OSD_SetColor(255, 255, 255);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                CTACandidateR[i1].startxi-AddSize , CTACandidateR[i1].startyi,\
                CTACandidateR[i1].startxi+CTACandidateR[i1].width+AddSize , CTACandidateR[i1].startyi+CTACandidateR[i1].height ,1,FALSE);

            /// Draw Red Rec
            if( ( TRUE == CTACandidateR[i1].isAlarm ) )
            {
                OSD_Ycc422::OSD_SetColor(0xFF, 0, 0);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    CTACandidateR[i1].startxi , CTACandidateR[i1].startyi,\
                    CTACandidateR[i1].startxi+CTACandidateR[i1].width ,CTACandidateR[i1].startyi+CTACandidateR[i1].height ,1,FALSE);
            }
        }
    } // end of MAXCandidateNum
    #pragma endregion

    #pragma region "ImageWarning - FrontLine"
    OSD_Ycc422::OSD_SetColor(25,250,255);
    ForEach(i1, 0, MAXCandidateNum)
    {
        /// Left
        if((Valid == CTACandidateL[i1].isValid)
          &&(CTACandidateL[i1].isObjPosxyValid)
          &&(CTACandidateL[i1].isObjPosXYValid))
        {	
            INT32 tmpxi,tmpyi,tmpxi2,tmpyi2;

            INT32 iv1 = 50; //cm
            for ( INT32 Yw = 599 ; Yw > 1 ; --Yw)
            {
                iv1 = (iv1 <= (-50))? 50 : (iv1 - 1);
                if(iv1 < 0) continue;

                WModel->PosWorldToImg( INV_XwShiftToBackCenter(CTACandidateL[i1].ObjPosXY[0])/CTA_TV_COMPRESS ,Yw/CTA_TV_COMPRESS, tmpxi,tmpyi );
                WModel->PosWorldToImg( INV_XwShiftToBackCenter(CTACandidateL[i1].ObjPosXY[0])/CTA_TV_COMPRESS ,(Yw-1)/CTA_TV_COMPRESS, tmpxi2,tmpyi2 );
                OSD_Ycc422::OSD_Draw_Line(dst_Y,imgDimensionX,imgDimensionY, tmpxi, tmpyi,tmpxi2,tmpyi2,3);
            }
        }

        /// Right
        if((Valid == CTACandidateR[i1].isValid)
            &&(CTACandidateR[i1].isObjPosxyValid)
            &&(CTACandidateR[i1].isObjPosXYValid))
        {	
            INT32 tmpxi,tmpyi,tmpxi2,tmpyi2;

            INT32 iv1 = 50; //cm
            for ( INT32 Yw = 599 ; Yw > 1 ; --Yw)
            {
                iv1 = (iv1 <= (-50))? 50 : (iv1 - 1);
                if(iv1 < 0) continue;

                WModel->PosWorldToImg( INV_XwShiftToBackCenter(CTACandidateR[i1].ObjPosXY[0])/CTA_TV_COMPRESS ,Yw/CTA_TV_COMPRESS, tmpxi,tmpyi );
                WModel->PosWorldToImg( INV_XwShiftToBackCenter(CTACandidateR[i1].ObjPosXY[0])/CTA_TV_COMPRESS ,(Yw-1)/CTA_TV_COMPRESS, tmpxi2,tmpyi2 );
                OSD_Ycc422::OSD_Draw_Line(dst_Y,imgDimensionX,imgDimensionY, tmpxi, tmpyi,tmpxi2,tmpyi2,3);
            }
        }
    } // end of MAXCandidateNum
    #pragma endregion

    ///2) AudioWarning - BIG RED ROI
    UCHAR alarmState = CTA_GetResult_Alarm();
    OSD_Ycc422::OSD_SetColor(255, 0, 0);
    if( alarmState & 0x02 ) // left
    {
        OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
            RegionL[0] , RegionL[1],RegionL[2] ,RegionL[3],5,FALSE);
        /// XML File
        EventResultL[FrameNum] = TRUE;
    }
    if( alarmState & 0x01 ) // Right
    {
        OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
            RegionR[0] , RegionR[1],RegionR[2] ,RegionR[3],5,FALSE);
        /// XML File
        EventResultR[FrameNum] = TRUE;
    }

#endif
}


/**
*@brief		Harris corner Maps
*@param[in] g_harrisMap			Harris-corner Information Map
*@param[in] pointNumber			Harris-corner total number
*/
void verify_CornersMap( UCHAR *dst_Y , UCHAR *harrisMap, INT32 pointNumber , INT32 imgDimensionX , INT32 imgDimensionY )
{
#if defined(_WIN32) &&( ON == OSD_verify_CornersMap)

    ///1) Show Harris Corner Number(GREEN point)
    OSD_Ycc422::OSD_SetColor(50, 222, 1);
    OSD_Ycc422::OSD_Draw_Text( (const char*)"Corners"    , dst_Y, imgDimensionX, imgDimensionY, 58, 0, 0);
    OSD_Ycc422::OSD_Draw_INT32( pointNumber , dst_Y, imgDimensionX, imgDimensionY, 65, 24, 3 ,12);

    ///2) Draw Harris corner point
    OSD_Ycc422::OSD_SetColor(0, 250, 12);
    for(int row=0; row<imgDimensionY; row++)
        for (int col=0; col<imgDimensionX; col++)
        {
            if(*(harrisMap + row*imgDimensionX +col)>=1)
            {
                OSD_Ycc422::OSD_Draw_Rec(dst_Y,imgDimensionX,imgDimensionY , col-1,row-1,col+1,row+1,1,TRUE,1);
            }
        }
    
    //[P.S.] Dump g_img_FrameDiff to Dst 
    //CopyImgToDst(dst_Y,g_img_FrameDiff,imgDimensionX,imgDimensionY); ///<�N g_img_FrameDiff ø�s�X �h �e��PLOT���N�B�\//

#endif
}
char* verify_DumpTrackingInfo( char* FrameReport,int& FrameReportIdx,CONST CandidateRec* CandidateL,CONST CandidateRec* CandidateR,CONST TARGET_NUM *targetNum)
{
    INT32 i;
    INT16 L_MaxCandNum=targetNum->_L_MaxCandNum;
    INT16 R_MaxCandNum=targetNum->_R_MaxCandNum;

    FrameReportIdx=0;
    memset(FrameReport,0,sizeof(FrameReport));
    
    FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"Crossover=%d\r\n", targetNum->CrossoverCountDown);

    for(i=0; i<L_MaxCandNum; i++)
    {
        if(CandidateL[i].isValid==Invalid)
            continue;
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"<L%d>",i);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"startxi=%d, ",CandidateL[i].startxi);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"width=%d,\r\n",CandidateL[i].width);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"isValid=%d, ",CandidateL[i].isValid);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"MDir=%d,\r\n",CandidateL[i].moveDir);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"AppearC=%d, ",CandidateL[i].appearCounter);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"BoundC=%d,\r\n",CandidateL[i].boundaryCount);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"CornerMv=(%3.1f,%3.1f),\r\n",CandidateL[i].CornerMove[0],CandidateL[i].CornerMove[1]);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"TrkPt(%d,%d)Mv(%d,%d)Sad(%d),\r\n",CandidateL[i].ObjTrackPosxy[0],CandidateL[i].ObjTrackPosxy[1],(int)CandidateL[i].ObjTrackMovexy[0],(int)CandidateL[i].ObjTrackMovexy[1],(int)CandidateL[i].ObjTrackSAD);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"Movxy=(%d,%d), \r\n",(int)CandidateL[i].ObjMovexy[0],(int)CandidateL[i].ObjMovexy[1]);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"Speed=%5.1f\r\n",CandidateL[i].ObjSpeedXY[0]*SPEED_CONST_KMH);
    }

    for(i=0; i<R_MaxCandNum; i++)
    {
        if(CandidateR[i].isValid==Invalid)
            continue;
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"<R%d>",i);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"startxi=%d, ",CandidateR[i].startxi);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"width=%d,\r\n",CandidateR[i].width);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"isValid=%d, ",CandidateR[i].isValid);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"MDir=%d,\r\n",CandidateR[i].moveDir);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"AppearC=%d, ",CandidateR[i].appearCounter);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"BoundC=%d,\r\n",CandidateR[i].boundaryCount);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"CornerMv=(%3.1f,%3.1f),\r\n",CandidateR[i].CornerMove[0],CandidateR[i].CornerMove[1]);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"TrkPt(%d,%d)Mv(%d,%d)Sad(%d),\r\n",CandidateR[i].ObjTrackPosxy[0],CandidateR[i].ObjTrackPosxy[1],(int)CandidateR[i].ObjTrackMovexy[0],(int)CandidateR[i].ObjTrackMovexy[1],(int)CandidateR[i].ObjTrackSAD);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"Movxy=(%d,%d), \r\n",(int)CandidateR[i].ObjMovexy[0],(int)CandidateR[i].ObjMovexy[1]);
        FrameReportIdx+=sprintf(FrameReport+FrameReportIdx,"Speed=%5.1f\r\n",CandidateR[i].ObjSpeedXY[0]*SPEED_CONST_KMH);
    }
    return FrameReport;
}

/**
*@brief		[Optional] TXT Information Export
*@param[in] Gaussian_img	    Src Image
*/
void verify_Update_Data_Before_Next_Frame(  UCHAR *Gaussian_img,UCHAR *Gaussian_imgPrev,
                                            UCHAR *Sobel_img, UCHAR *Sobel_imgPrev,
                                            HarrisCornerPos *harrisCorners , HarrisCornerPos *harrisCornersPrev,
                                            HarrisCornerPos2 *harrisCorners2 , HarrisCornerPos2 *harrisCorners2Prev,
                                            CandidateRec *CTACandidateL, CandidateRec *CTACandidatePrevL,
                                            CandidateRec *CTACandidateR, CandidateRec *CTACandidatePrevR,
                                            INT32 &FrameNum ,INT32 imgDimensionX,INT32 imgDimensionY,
                                            TARGET_NUM *pTargetNum)
{
#if defined(_WIN32) &&( ON == OSD_verify_Update_Data_Before_Next_Frame)
    #if ( ON == DBG_TXT_no)
    FILE *fptr;
    errno_t err=fopen_s(&fptr,"c:\\test\\CTA_output.txt","a");
    for ( std::vector<HarrisCornerPos>::iterator it = harrisCorners.begin() ; it != harrisCorners.end() ; it++)
        fprintf(fptr,"idx(FrameNum)=%d, (%d, %d) \n", FrameNum, it->x, it->y);
    fclose(fptr);
    #endif
#endif

#if defined(_DUMP_OUTPUT_)

    DumpBitMap(GetFullPathFileName(C_MOD_DUMP_PATH, "10_Gaussian_imgPrev.DAT" ),        Gaussian_imgPrev,   imgDimensionX*imgDimensionY);
    DumpBitMap(GetFullPathFileName(C_MOD_DUMP_PATH, "10_Sobel_imgPrev.DAT" ),           Sobel_imgPrev,      imgDimensionX*imgDimensionY);
    DumpCorners(GetFullPathFileName(C_MOD_DUMP_PATH, "10_harrisCornersPrev.DAT" ),      harrisCornersPrev,  pTargetNum->FeaturePtNumPrev);
    DumpCorners2(GetFullPathFileName(C_MOD_DUMP_PATH, "10_harrisCorners2Prev.DAT" ),     harrisCorners2Prev,  pTargetNum->FeaturePtNumPrev);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "10_CTACandidatePrevL.DAT" ), CTACandidatePrevL,  pTargetNum->_L_MaxCandNumPrev);
    DumpCandidateRec(GetFullPathFileName(C_MOD_DUMP_PATH, "10_CTACandidatePrevR.DAT" ), CTACandidatePrevR,  pTargetNum->_R_MaxCandNumPrev);
    
#endif
}
/**
*@brief		show 1) HeadLight Number 2) lamp rectangle
*@param[in] Gaussian_img	    Src Image
*/
void verify_APEX01_Night( UCHAR *dst_Y , INT32 imgDimensionX, INT32 imgDimensionY, UCHAR *g_Binar_img )
{

#if defined(_WIN32) &&( ON == OSD_verify_APEX01_Night)
    //1) Draw TargetPixel YELLOW NIGHT
    UINT16 startx = RegionL[0];
    UINT16 starty = RegionL[1];
    UINT16 endx   = RegionR[2];
    UINT16 endy   = RegionR[3];
    OSD_Ycc422::OSD_SetColor(255, 255, 0);
    INT32 offsetV ;
    for (INT32 vert = starty; vert < endy ;++vert)
    {
        offsetV = vert * imgDimensionX;
        for (INT32 hori = startx; hori < endx ;++hori)
        {
            if( 0 < g_Binar_img[ offsetV + hori ])
                OSD_Ycc422::OSD_Draw_Pixel(dst_Y,imgDimensionX,imgDimensionY , hori , vert );
        }
    }
#endif

#if defined(_DUMP_OUTPUT_) 
    DumpBitMap (GetFullPathFileName(C_MOD_DUMP_PATH, "01_Binar_img.RAW" ), g_Binar_img , imgDimensionX*imgDimensionY,true);
#endif
}
/**
*@brief		show 1) HeadLight Number 2) lamp rectangle
*/
void verify_ClusterIntoHeadlight(	UCHAR * dst_Y,INT32 imgDimensionX,INT32 imgDimensionY, 
                                    const INT32 RegionL[4], const INT32 RegionR[4],
                                    HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum],
                                    UINT16 count1L , UINT16 count1R)
{
    #if defined(_WIN32) &&( ON == OSD_verify_ClusterIntoHeadlight)
    
    /// GREEN rect & digit NIGHT
    //1) PLOT lampL/R Number
    OSD_Ycc422::OSD_SetColor(0, 255, 0);
    OSD_Ycc422::OSD_Draw_Text((const char*)"Light" , dst_Y , imgDimensionX , imgDimensionY,0,30,0);
    OSD_Ycc422::OSD_Draw_Text((const char*)"Light" , dst_Y , imgDimensionX , imgDimensionY,SrcImg_W-70,30,0);

    INT32 countL = 0;
    INT32 countR = 0;
    for ( INT32 i1 = 0 ; i1< MAX_HeadLightNum ; ++i1)
    {
        countL = (TRUE == lampL[ i1 ].isValid )?(countL+1):(countL);
        countR = (TRUE == lampR[ i1 ].isValid )?(countR+1):(countR);
    }
    OSD_Ycc422::OSD_Draw_INT32( countL , dst_Y , imgDimensionX , imgDimensionY,
        10,
        50,
        1 , 11 ); 
    OSD_Ycc422::OSD_Draw_INT32( countR , dst_Y , imgDimensionX , imgDimensionY,
        SrcImg_W-50,
        50, 1 , 11 ); 
    
    /* no GREEN rect NIGHT
    //2) PLOT lampL/R Rectangle.
    OSD_SetColor(0, 255, 0);
    for ( INT32 i1 = 0 ; i1< MAX_HeadLightNum ; ++i1)
    {
        /// Left
        if( TRUE == lampL[i1].isValid )
        {	
            OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY,
                lampL[i1].startxi-1,
                lampL[i1].startyi-1,
                lampL[i1].startxi+lampL[i1].width+2,
                lampL[i1].startyi+lampL[i1].height+2,1,FALSE);
        }
        if( TRUE == lampR[i1].isValid )
        {	
            OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY,
                lampR[i1].startxi-1,
                lampR[i1].startyi-1,
                lampR[i1].startxi+lampR[i1].width+2,
                lampR[i1].startyi+lampR[i1].height+2,1,FALSE);
        }
    }
    */
    #endif

#if defined(_DUMP_OUTPUT_)
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "53_lampL.DAT" ),  lampL  , MAX_HeadLightNum);
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "53_lampR.DAT" ),  lampR  , MAX_HeadLightNum);
#endif
}

#if defined(_WIN32) && ( ON == OSD_verify_HeadLightFeatureUpdate_1 )
    static UCHAR tmp_YCbCr_Img[ SrcImg_H * SrcImg_W * 2];   ///< only for Night mode save image
    static UCHAR tmp_Y_Img    [ SrcImg_H * SrcImg_W ];      ///< only for Night mode save image
#endif
/**
*@brief		Export 1) data 2) images
*@param[in] 
*/
void verify_HeadLightFeatureUpdate_1( UCHAR *dst_Y, INT32 imgDimensionX ,INT32 imgDimensionY, const INT32 RegionL[4] , const INT32 RegionR[4], HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] )
{
#if defined(_WIN32) && ( ON == OSD_verify_HeadLightFeatureUpdate_1 )
    using namespace std;
    /// OFFLINE_DATA_Export
    #if( ON == OFFLINE_TRAIN_DUMPIMG_NIGHT)
        #if( OFF == OPENCV_EN )
            assert(!"Need OpenCV ENABLE");
        #endif

    ///1) Initial PC File
    std::ofstream fout;
    fout.open("C:\\CTA_OFFLINE_TRAIN_DATA\\FeatureEx_Night.csv",std::ios::app);
    if(!fout){  assert(!"open file fail");  }

    ///2) Save Img & Features to PC File
    CONST INT32 FeatureNumNight = 5;
    for ( int i1 = 0 ; i1 < MAX_HeadLightNum ; ++i1)
    {
        if( TRUE == lampL[i1].isValid )
        {
            ///2.1) save image
            string fpath = VideoFilePath;
            /// Extract FileName (ex. 105_20130801_00001)
            INT32 posh   = fpath.rfind("\\");
            INT32 pose   = fpath.rfind(".");
            INT32 len    = fpath.length();
            string fname = fpath.substr(posh+1,pose-posh-1);

            /// Get save filename
            CHAR   Idx[30];
            sprintf( Idx ,"%d",IdxSaveImg);
            fname += "_";
            fname += Idx;
            fname += ".jpg"; //ex. 105_20130801_00001_1000001_FrameNum.jpg

            /// save original gray image.
            /// ����x�s��i�Ƕ��v���P�ؿ�O�Ϫ��覡�i�H���M�����ĥΤH�u����
            CONST INT32 scaleVal = 1;
            INT32 startx         = MAX(lampL[i1].startxi - scaleVal * lampL[i1].width , 1);
            INT32 endx           = MIN(lampL[i1].startxi + lampL[i1].width + scaleVal * lampL[i1].width  , (imgDimensionX-1));
            INT32 width          = endx-startx;
            INT32 starty         = MAX(lampL[i1].startyi - scaleVal * lampL[i1].height , 1);
            INT32 endy           = MIN(lampL[i1].startyi + lampL[i1].height + scaleVal * lampL[i1].height  , (imgDimensionY-1));
            INT32 height         = endy - starty;
            
            OSD_Ycc422::OSD_SetColor(255,255,255);
            memcpy( tmp_YCbCr_Img , dst_Y , imgDimensionY * imgDimensionX * 2);
            OSD_Ycc422::OSD_Draw_Rec( tmp_YCbCr_Img , imgDimensionX, imgDimensionY ,startx , starty, endx, endy, 3, FALSE );

            OSD_Ycc422::Extract_Y_From_YCbYCr( tmp_Y_Img, tmp_YCbCr_Img, imgDimensionX, imgDimensionY );
            OPENCV->SaveImg( tmp_Y_Img ,imgDimensionX,imgDimensionY, 
                            0 , 0 , (imgDimensionX>>1) , imgDimensionY-1 ,
                            fname.c_str());

            ///2.2) save RecFeatures_Night
            fout<<fname<<",";
            for ( INT32 i2 = 0 ; i2 < FeatureNumNight ; ++i2)
            {
                FLOAT data01 =lampL[i1].Features[ i2 ];
                fout<<data01<<",";
            }
            fout<<endl;
            IdxSaveImg++;
        }
        if( TRUE == lampR[i1].isValid )
        {
            ///2.1) save image
            string fpath = VideoFilePath;
            /// Extract FileName (ex. 105_20130801_00001)
            INT32 posh = fpath.rfind("\\");
            INT32 pose = fpath.rfind(".");
            INT32 len = fpath.length();
            string fname = fpath.substr(posh+1,pose-posh-1);

            /// Get save filename
            CHAR   Idx[30];
            sprintf( Idx ,"%d",IdxSaveImg);
            fname += "_";
            fname += Idx;
            fname += ".jpg"; //ex. 105_20130801_00001_1000001_FrameNum.jpg

            /// save original gray image.
            /// ����x�s��i�Ƕ��v���P�ؿ�O�Ϫ��覡�i�H���M�����ĥΤH�u����
            CONST INT32 scaleVal = 1;
            INT32 startx         = MAX(lampR[i1].startxi - scaleVal * lampR[i1].width , 1);
            INT32 endx           = MIN(lampR[i1].startxi + lampR[i1].width + scaleVal * lampR[i1].width  , (imgDimensionX-1));
            INT32 width          = endx-startx;
            INT32 starty         = MAX(lampR[i1].startyi - scaleVal * lampR[i1].height , 1);
            INT32 endy           = MIN(lampR[i1].startyi + lampR[i1].height + scaleVal * lampR[i1].height  , (imgDimensionY-1));
            INT32 height         = endy-starty;

            OSD_Ycc422::OSD_SetColor(255,255,255);
            memcpy( tmp_YCbCr_Img , dst_Y , imgDimensionY * imgDimensionX * 2);
            OSD_Ycc422::OSD_Draw_Rec(tmp_YCbCr_Img , imgDimensionX, imgDimensionY ,startx,starty,endx,endy,3,FALSE );

            OSD_Ycc422::Extract_Y_From_YCbYCr( tmp_Y_Img, tmp_YCbCr_Img, imgDimensionX, imgDimensionY );
            OPENCV->SaveImg( tmp_Y_Img ,imgDimensionX,imgDimensionY, 
                (imgDimensionX>>1)  , 0 , (imgDimensionX>>1)-1 , imgDimensionY-1 ,
                fname.c_str());

            /// 2.2) save RecFeatures
            fout<<fname<<","; //100001.jpg
            for ( INT32 i2 = 0 ; i2 < FeatureNumNight ; ++i2)
            {
                FLOAT data01 = lampR[i1].Features[ i2 ];
                fout<<data01<<","; /// 100001.jpg
            }
            fout<<endl;
            IdxSaveImg++;
        }
    }

    #endif
#endif

#if defined(_DUMP_OUTPUT_)
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "54_lampL.DAT" ),  lampL  , MAX_HeadLightNum);
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "54_lampR.DAT" ),  lampR  , MAX_HeadLightNum);
#endif

}

/**
*@brief		Plot Valid Candidate after Filter II.
*/
void verify_HeadLightPredict_1( UCHAR *dst_Y , INT32 imgDimensionX ,INT32 imgDimensionY,
                                const INT32 RegionL[4] , const INT32 RegionR[4],
                                HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] ,
                                UINT16 count2L , UINT16 count2R)
{

#if defined(_WIN32) && ( ON == OSD_verify_HeadLightPredict_1 )

    /// Candidate Number after FilterII
    OSD_Ycc422::OSD_SetColor(255, 255, 255);
    OSD_Ycc422::OSD_Draw_Text( (const char*)"LAMP" , dst_Y, imgDimensionX, imgDimensionY, 0, 70, 0); ///< FilterII
    OSD_Ycc422::OSD_Draw_Text( (const char*)"LAMP" , dst_Y, imgDimensionX, imgDimensionY, 690, 70, 0); ///< FilterII
    OSD_Ycc422::OSD_Draw_INT32( count2L , dst_Y , imgDimensionX, imgDimensionY,  10, 90, 3 ,12); 
    OSD_Ycc422::OSD_Draw_INT32( count2R , dst_Y , imgDimensionX, imgDimensionY, 700, 90, 3 ,12); 

/*   removed to the verify_CTADog_Night()
    /// (ø�s�ծ�)White HeadLight Rectangles after ( Filter II )
    OSD_SetColor(255, 255, 255);
    for ( INT32 i1 = 0 ; i1< MAX_HeadLightNum ; ++i1)
    {
        /// Left
        if( TRUE == lampL[i1].isValid )
        {	
            OSD_Draw_Rec(   dst_Y, imgDimensionX, imgDimensionY, 
                            lampL[i1].startxi , lampL[i1].startyi,\
                            lampL[i1].startxi+lampL[i1].width ,lampL[i1].startyi+lampL[i1].height ,1,FALSE);
        }

        /// Right
        if( TRUE == lampR[i1].isValid )
        {	
            OSD_Draw_Rec(   dst_Y, imgDimensionX, imgDimensionY, 
                            lampR[i1].startxi , lampR[i1].startyi,\
                            lampR[i1].startxi + lampR[i1].width ,lampR[i1].startyi+lampR[i1].height ,1,FALSE);
        }
    }
*/
#endif

#if defined(_DUMP_OUTPUT_)
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "55_lampL.DAT" ),  lampL  , MAX_HeadLightNum);
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "55_lampR.DAT" ),  lampR  , MAX_HeadLightNum);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "55_lampNumL.DAT" ),  &count2L , 1);
    DumpValue(GetFullPathFileName(C_MOD_DUMP_PATH, "55_lampNumR.DAT" ),  &count2R , 1);
#endif
}

/**
*@brief		1.Plot/draw ObjInfo [/Posx/PosX/Speed/TTC]
*@param[in] CountDownFlagAdpL_Night		Dynamic Max Flag
*/
void verify_Estimation_Night( UCHAR *dst_Y , INT32 imgDimensionX, INT32 imgDimensionY, const INT32 RegionL[4],const INT32 RegionR[4], HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum],HeadLight lampPrevL[], HeadLight lampPrevR[] )
{
#if defined(_WIN32) && ( ON == OSD_verify_Estimation_Night )
    
    ///------------------------LampInfo------------------------------------------///
    /// ø�s Lamp
    //  1) ø�I: Posxi : ���I
    //  2) �T��: [countDown/ ���I / Speed(km/hr) / TTC]
    
    CONST UINT16 InfoWordSize = 7;
    OSD_Ycc422::OSD_SetColor(255, 255, 0);
    for ( int i1 = 0 ; i1< ROISegBlockNum_H * ROISegBlockNum_V ; ++i1)
    {
        /// Left
        if( TRUE == lampL[i1].isValid )
        {	
            /// countDown
            OSD_Ycc422::OSD_Draw_INT32( lampL[i1].countDown , dst_Y , imgDimensionX , imgDimensionY,
                lampL[i1].startxi+lampL[i1].width-2*InfoWordSize,
                lampL[i1].startyi+lampL[i1].height+5,
                1 , InfoWordSize ); 

            /// Position (Draw_INT32 Xw(cm))
            /// Plot Xw
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( lampL[i1].ObjPosXY[0] , dst_Y , imgDimensionX, imgDimensionY ,
                lampL[i1].startxi+lampL[i1].width-2*InfoWordSize,
                lampL[i1].startyi+lampL[i1].height+5+InfoWordSize+3,
                1 , InfoWordSize ); 

            /// draw Xw Line
            if( lampL[i1].isObjPosxyValid && lampL[i1].isObjPosXYValid)
            {
                /// draw CarHead point �C�I
                INT32 tmpx,tmpy;
                INT32 tmpX,tmpY;
                tmpX  =  lampL[i1].ObjPosXY[0] + ((CTA_TV_WIDTH)>>1)*CTA_TV_COMPRESS;
                tmpY  =  (CTA_TV_HEIGHT*CTA_TV_COMPRESS) - lampL[i1].ObjPosXY[1];
                WModel->PosWorldToImg(tmpX/CTA_TV_COMPRESS , tmpY/CTA_TV_COMPRESS , tmpx , tmpy);
                OSD_Ycc422::OSD_SetColor(25,250,255);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX,imgDimensionY, tmpx-2, tmpy-2,tmpx+2,tmpy+2,1,TRUE);

                /// draw line �C�u��
                OSD_Ycc422::OSD_SetColor( 25,250,255 );
                tmpY = CTA_TV_HEIGHT*CTA_TV_COMPRESS;
                for ( INT32 Yw = tmpY ; Yw>0 ; --Yw)
                {
                    WModel->PosWorldToImg( tmpX/CTA_TV_COMPRESS, Yw/CTA_TV_COMPRESS , tmpx , tmpy );
                    OSD_Ycc422::OSD_Draw_Pixel(dst_Y , imgDimensionX , imgDimensionY , tmpx, tmpy);
                }
            }

            /// Speed(km/hr)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            FLOAT speedKmHr = (TRUE == lampL[i1].isObjSpeedXYValid)? (SPEED_CONST_KMH * lampL[i1].ObjSpeedXY[0]):0;
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            lampL[i1].startxi+lampL[i1].width-2*InfoWordSize,
                            lampL[i1].startyi+lampL[i1].height+5+2*(InfoWordSize+3),
                            1 , InfoWordSize ); 

            /// TTC(ms)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            FLOAT showTTC = ( (TRUE==lampL[i1].isObjSpeedXYValid) &&(TRUE==lampL[i1].isObjPosXYValid))?lampL[i1].TTC:(0);
            if( showTTC<2000 && showTTC>0 )
                OSD_Ycc422::OSD_SetColor(255, 0, 0);
            OSD_Ycc422::OSD_Draw_INT32( showTTC , dst_Y , imgDimensionX, imgDimensionY,
                            lampL[i1].startxi+lampL[i1].width-2*InfoWordSize,
                            lampL[i1].startyi+lampL[i1].height+5+3*(InfoWordSize+3),
                            1 , InfoWordSize ); 

            /// ��ܤU��°ϰ�
            const INT32 WordSizeAtBot = 18;
            // POS(m)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            INT32 PosX =  lampL[i1].ObjPosXY[0]/100;
            OSD_Ycc422::OSD_Draw_INT32( PosX , dst_Y , imgDimensionX, imgDimensionY ,
                            lampL[i1].startxi+lampL[i1].width-2*WordSizeAtBot,
                            SrcImg_H-85,
                            3 , WordSizeAtBot ); 

            /// Speed(km/hr)
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            lampL[i1].startxi+lampL[i1].width-2*WordSizeAtBot,
                            SrcImg_H-60,
                            3 , WordSizeAtBot ); 

            /// TTC (ms)
            if( showTTC<2000 && showTTC>0 )
                OSD_Ycc422::OSD_SetColor(255, 0, 0);
            OSD_Ycc422::OSD_Draw_INT32( showTTC , dst_Y , imgDimensionX, imgDimensionY,
                            lampL[i1].startxi+lampL[i1].width-2*WordSizeAtBot,
                            SrcImg_H-36,
                            3 , WordSizeAtBot ); 
        }

        /// Right
        if( TRUE == lampR[i1].isValid )
        {	
            
            /// countDown
            OSD_Ycc422::OSD_Draw_INT32( lampR[i1].countDown , dst_Y , imgDimensionX , imgDimensionY,
                lampR[i1].startxi+lampR[i1].width-2*InfoWordSize,
                lampR[i1].startyi+lampR[i1].height+5,
                1 , InfoWordSize ); 
                
            /// Position (Draw_INT32 Xw(cm))
            /// Plot Xw
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            OSD_Ycc422::OSD_Draw_INT32( lampR[i1].ObjPosXY[0] , dst_Y , imgDimensionX, imgDimensionY ,
                            lampR[i1].startxi+lampR[i1].width-2*InfoWordSize,
                            lampR[i1].startyi+lampR[i1].height+5+InfoWordSize+3,
                            1 , InfoWordSize ); 
            
            /// draw Xw Line
            if( lampR[i1].isObjPosxyValid && lampR[i1].isObjPosXYValid)
            {
                /// draw CarHead point �C�I
                OSD_Ycc422::OSD_SetColor( 25,250,255 );
                INT32 tmpx,tmpy;
                INT32 tmpX,tmpY;
                tmpX  =  lampR[i1].ObjPosXY[0] + ((CTA_TV_WIDTH)>>1)*CTA_TV_COMPRESS;
                tmpY  =  (CTA_TV_HEIGHT*CTA_TV_COMPRESS) - lampR[i1].ObjPosXY[1];
                WModel->PosWorldToImg(tmpX/CTA_TV_COMPRESS , tmpY/CTA_TV_COMPRESS , tmpx , tmpy);
                OSD_Ycc422::OSD_SetColor(25,250,255);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX,imgDimensionY, tmpx-2, tmpy-2,tmpx+2,tmpy+2,1,TRUE);
                
                /// draw line �C�u��
                OSD_Ycc422::OSD_SetColor( 25,250,255 );
                tmpY = CTA_TV_HEIGHT*CTA_TV_COMPRESS;
                for ( INT32 Yw = tmpY ; Yw>0 ; --Yw)
                {
                    WModel->PosWorldToImg( tmpX/CTA_TV_COMPRESS, Yw/CTA_TV_COMPRESS , tmpx , tmpy );
                    OSD_Ycc422::OSD_Draw_Pixel(dst_Y , imgDimensionX , imgDimensionY , tmpx, tmpy);
                }
            }

            /// Speed(km/hr)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            FLOAT speedKmHr = (TRUE == lampR[i1].isObjSpeedXYValid)? (SPEED_CONST_KMH * lampR[i1].ObjSpeedXY[0]):0;
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                lampR[i1].startxi+lampR[i1].width-2*InfoWordSize,
                lampR[i1].startyi+lampR[i1].height+5+2*(InfoWordSize+3),
                1 , InfoWordSize ); 

            /// TTC
            FLOAT showTTC = ( (TRUE==lampR[i1].isObjSpeedXYValid) &&(TRUE==lampR[i1].isObjPosXYValid))?lampR[i1].TTC:(0);
            if( showTTC<2000 && showTTC>0 )
                OSD_Ycc422::OSD_SetColor(255, 0, 0);
            OSD_Ycc422::OSD_Draw_INT32( showTTC , dst_Y , imgDimensionX, imgDimensionY,
                            lampR[i1].startxi+lampR[i1].width-2*InfoWordSize,
                            lampR[i1].startyi+lampR[i1].height+5+3*(InfoWordSize+3),
                            1 , InfoWordSize ); 

            /// ��ܤU��°ϰ�
            const INT32 WordSizeAtBot = 18;
            // POS(m)
            OSD_Ycc422::OSD_SetColor(255, 255, 0);
            INT32 PosX =  lampR[i1].ObjPosXY[0]/100;
            OSD_Ycc422::OSD_Draw_INT32( PosX , dst_Y , imgDimensionX, imgDimensionY ,
                            lampR[i1].startxi+lampR[i1].width-2*WordSizeAtBot,
                            SrcImg_H-85,
                            3 , WordSizeAtBot ); 

            /// Speed(km/hr)
            OSD_Ycc422::OSD_Draw_INT32( speedKmHr , dst_Y , imgDimensionX , imgDimensionY,
                            lampR[i1].startxi+lampR[i1].width-2*WordSizeAtBot,
                            SrcImg_H-60,
                            3 , WordSizeAtBot ); 

            /// TTC (ms)
            if( showTTC<2000 && showTTC>0 )
                OSD_Ycc422::OSD_SetColor(255, 0, 0);
            OSD_Ycc422::OSD_Draw_INT32( showTTC , dst_Y , imgDimensionX, imgDimensionY,
                            lampR[i1].startxi+lampR[i1].width-2*WordSizeAtBot,
                            SrcImg_H-36,
                            3 , WordSizeAtBot ); 
        }
    }

#endif

#if defined(_DUMP_OUTPUT_)
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "56_lampL.DAT" ),  lampL          , MAX_HeadLightNum);
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "56_lampR.DAT" ),  lampR          , MAX_HeadLightNum);
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "56_lampPrevL.DAT" ),  lampPrevL  , MAX_HeadLightNum);
    DumpHeadLight(GetFullPathFileName(C_MOD_DUMP_PATH, "56_lampPrevR.DAT" ),  lampPrevR  , MAX_HeadLightNum);
#endif
}

/**
*@brief		Show Image/Audio Warning Info
*@n         1)ImageWarning - ø�sRectangle /��/�� ��
*@param[in] RegionL			Image processing ROI
*@param[in] lampL	        HeadLight Info.
*@param[in] isAlarmL		is CTADog bowwow
*/
void verify_CTADog_Night( UCHAR *dst_Y ,INT32 imgDimensionX,INT32 imgDimensionY,const INT32 RegionL[4],const INT32 RegionR[4], HeadLight lampL[MAX_HeadLightNum] ,HeadLight lampR[MAX_HeadLightNum] , UCHAR AlarmState , INT32 FrameNum )
{
#if defined(_WIN32) && ( ON == OSD_verify_CTADog_Night )
    
    ///1) ImageWarning - ø�sRectangle /��/�� ��
    //========================================================================
    //  White    Rec: Pass 2 Filters -[Ref: verify_CNNPredict]: it should be a reasonable candidate
    //  Yellow   Rec: 0 < TTC < THD(2sec) catched
    //  Red      Rec: steady detected (countdown flag = 0):  TTC alarm, 
    //========================================================================

    CONST INT32 AddSize = 0;
    for ( int i1 = 0 ; i1< MAXCandidateNum ; ++i1)
    {
        /// Left
        if( (TRUE == lampL[i1].isValid) )
        {	
            /// Draw White Rec Pass 2 Filters 
            OSD_Ycc422::OSD_SetColor(255, 255, 255);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    lampL[i1].startxi -AddSize , lampL[i1].startyi,\
                    lampL[i1].startxi+lampL[i1].width +AddSize , lampL[i1].startyi+lampL[i1].height ,1,FALSE);
            

            /// Draw Yellow Rec when 0<TTC<2
            if( ( lampL[i1].TTC <= CTATTC) && ( 0 < lampL[i1].TTC ) )
            {
                OSD_Ycc422::OSD_SetColor(255, 255, 0);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    lampL[i1].startxi - AddSize , lampL[i1].startyi,\
                    lampL[i1].startxi + lampL[i1].width + AddSize , lampL[i1].startyi+lampL[i1].height ,1,FALSE);
            }

            /// Draw Red Rec when countdown==0
            if( 0 == lampL[i1].countDown  )
            {
                OSD_Ycc422::OSD_SetColor(255, 0, 0);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    lampL[i1].startxi - AddSize , lampL[i1].startyi,\
                    lampL[i1].startxi + lampL[i1].width + AddSize , lampL[i1].startyi+lampL[i1].height ,1,FALSE);
            }
        }
        /// Right
        if( (TRUE == lampR[i1].isValid) )
        {	
            /// Draw White Rec Pass 2 Filters 
            OSD_Ycc422::OSD_SetColor(255, 255, 255);
            OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                lampR[i1].startxi - AddSize , lampR[i1].startyi,\
                lampR[i1].startxi + lampR[i1].width + AddSize , lampR[i1].startyi+lampR[i1].height ,1,FALSE);

            /// Draw Yellow Rec when TTC < 2
            if( (lampR[i1].TTC <= CTATTC) &&(0<lampR[i1].TTC) )
            {
                OSD_Ycc422::OSD_SetColor(255, 255, 0);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    lampR[i1].startxi - AddSize , lampR[i1].startyi,\
                    lampR[i1].startxi + lampR[i1].width + AddSize , lampR[i1].startyi+lampR[i1].height ,1,FALSE);

            }
            /// Draw Red Rec when (0 == CTACandidateR[i1].countDown )
            if( (0 == lampR[i1].countDown ) )
            {
                OSD_Ycc422::OSD_SetColor(255, 0, 0);
                OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
                    lampR[i1].startxi - AddSize , lampR[i1].startyi,\
                    lampR[i1].startxi + lampR[i1].width + AddSize , lampR[i1].startyi+lampR[i1].height ,1,FALSE);
            }
        }
    } // end of MAXCandidateNum


    ///2) AudioWarning - ø�s�j ROI ���خ�
    OSD_Ycc422::OSD_SetColor(255, 0, 0);
    if(AlarmState & 0x02)
    {
        OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
            RegionL[0] , RegionL[1],RegionL[2] ,RegionL[3],5,FALSE);
        /// XML File
        EventResultL[FrameNum] = TRUE;
    }
    if(AlarmState & 0x01)
    {
        OSD_Ycc422::OSD_Draw_Rec(dst_Y, imgDimensionX, imgDimensionY, 
            RegionR[0] , RegionR[1],RegionR[2] ,RegionR[3],5,FALSE);
        /// XML File
        EventResultR[FrameNum] = TRUE;

        /* Warning : Not Ready to draw circle
        OSD_Draw_Line(dst_Y, imgDimensionX, imgDimensionY,RegionR[0]+25,RegionR[1],RegionR[2]-25,RegionR[1],5);
        BOOL DrawPartial[] = {0,1,0,0};
        OSD_Draw_Circle(dst_Y, imgDimensionX, imgDimensionY,RegionR[0]+35,RegionR[1]+35,37,5,DrawPartial);
        */
    }

#endif
}

/**
*@brief		[Optional] TXT Information Export
*/
void verify_Update_Data_Before_Next_Frame_Night( UCHAR *Gaussian_img, UCHAR *Gaussian_imgPrev , UCHAR *g_bin_img, UCHAR *g_bin_PrevImg, HeadLight lampL[MAX_HeadLightNum],HeadLight lampPrevL[MAX_HeadLightNum], HeadLight lampR[MAX_HeadLightNum],HeadLight lampPrevR[MAX_HeadLightNum], INT32 &FrameNum, INT32 imgDimensionX, INT32 imgDimensionY )
{
#if defined(_WIN32) && ( ON == verify_Update_Data_Before_Next_Frame_Night )

#endif

#if defined(_DUMP_OUTPUT_)

    DumpBitMap( GetFullPathFileName(C_MOD_DUMP_PATH, "57_Gaussian_imgPrev.RAW" ),   Gaussian_imgPrev,   imgDimensionX*imgDimensionY);
    DumpBitMap( GetFullPathFileName(C_MOD_DUMP_PATH, "57_g_bin_PrevImg.RAW" ),      g_bin_PrevImg,      imgDimensionX*imgDimensionY);
    DumpHeadLight( GetFullPathFileName(C_MOD_DUMP_PATH, "57_lampPrevL.RAW" ),       lampPrevL,  MAX_HeadLightNum);
    DumpHeadLight( GetFullPathFileName(C_MOD_DUMP_PATH, "57_lampPrevR.RAW" ),       lampPrevR,  MAX_HeadLightNum);
#endif
}

/**
*@brief		PLOT the Time consumption in PC simulation
*/
void verify_TimeCost( UCHAR *dst_Y , INT32 imgDimensionX ,INT32 imgDimensionY,CTATimeManagement &inCTATime , INT32 inIdx )
{
#if defined(_WIN32) &&( ON == OSD_verify_TimeCost)
    //INT32 Len = sizeof(inCTATime.TicCounter)/sizeof(LARGE_INTEGER);
    DOUBLE PING = (DOUBLE)(inCTATime.TicCounter[inIdx-1].QuadPart - inCTATime.TicCounter[ 0 ].QuadPart)/
                  (DOUBLE)(inCTATime.cpuFreq.QuadPart);     ///< sec

    OSD_Ycc422::OSD_SetColor( 0xFF, 0xFF, 0);
    OSD_Ycc422::OSD_Draw_Text ( (const char*)"PING" , dst_Y, imgDimensionX, imgDimensionY, 500 , 0, 0);
    OSD_Ycc422::OSD_Draw_INT32( (INT32)(PING*1000) , dst_Y , imgDimensionX, imgDimensionY,  555, 5, 3 ,12);//ms 

    /// Slice time interval(Debug)
    CONST INT32 Len = 30; //sizeof(inCTATime.TicCounter)/sizeof(LARGE_INTEGER);
    DOUBLE PING2[Len] = {0};
    for ( INT32 i1 = 0 ; i1 < (Len-1) ; ++i1)
    {
        PING2[ i1 ] = (DOUBLE)(inCTATime.TicCounter[ i1+1 ].QuadPart - inCTATime.TicCounter[ i1 ].QuadPart)/
                      (DOUBLE)(inCTATime.cpuFreq.QuadPart) * 1000; ///< ms
    }
#endif
}


#ifdef _USE_CTA_OWN_IO_FUNCTION_
/**
*@brief		Dump image
*/
void DumpBitMap(char* filename,unsigned char* bitmap[],int start_idx,int end_idx,int size,bool reset)
{
    FILE* fp;
    int i;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(941);
#endif

    for(i=start_idx; i<=end_idx; i++)
    {
        count=0;
        count=fwrite(bitmap[i],sizeof(unsigned char),size,fp);
#ifdef ERROR_CODE
        if(count!=size)
            ErrorExit(940);
#endif
    }
    fclose(fp);
}

/**
*@brief		Dump image
*/
void DumpBitMap(char* filename,unsigned char* bitmap,int size,bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(960);
#endif
    count=0;
    count=fwrite(bitmap,sizeof(unsigned char),size,fp);
#ifdef ERROR_CODE
    if(count!=size)
        ErrorExit(939);
#endif

    fclose(fp);
}

void DumpBitMap2(char* filename, unsigned short* bitmap,int size, bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(960);
#endif
    count=0;
    count=fwrite(bitmap,sizeof(unsigned short),size,fp);
#ifdef ERROR_CODE
    if(count!=(size*sizeof(unsigned short)))
        ErrorExit(939);
#endif

    fclose(fp);
}
/**
*@brief		Dump Frame Header[speed steerAngle]
*/
void DumpFrameHead( char* filename,CAN_Info *head,bool reset/*=true*/ )
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(953);
#endif

    count=0;
    //count=fwrite(head,sizeof(CAN_Info),1,fp);

    count += fwrite(&head->carSpeed,sizeof(head->carSpeed),1,fp);
    count += fwrite(&head->steerAngle,sizeof(head->steerAngle),1,fp);
    count += fwrite(&head->time_stamp,sizeof(head->time_stamp),1,fp);
    count += fwrite(&head->timeInterval,sizeof(head->timeInterval),1,fp);
        
#ifdef ERROR_CODE
        if(count!=1)
            ErrorExit(952);
#endif
    fclose(fp);
}
#endif // END #ifdef _USE_CTA_OWN_IO_FUNCTION_

void DumpTimeManagement(char* filename,CTATimeManagement *TimeMa, bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(953);
#endif

    count=0;
    count=fwrite(TimeMa,sizeof(CTATimeManagement),1,fp);

#ifdef ERROR_CODE
    if(0 == count)
        ErrorExit(952);
#endif
    fclose(fp);

}
/**
*@brief		Dump Car Info
*/
void DumpCarInfo( char* filename,SimpleCarSpec *carSpec,bool reset )
{
    FILE* fp;
    int count=0;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(935);
#endif

    count=fwrite(carSpec,sizeof(SimpleCarSpec),1,fp);

#ifdef ERROR_CODE
    if(count!=1)
        ErrorExit(934);
#endif
    fclose(fp);
}


/**
*@brief		Dump Img2World LUT
*/
void DumpLUT_Img2World( char* filename,CTA_CaliResult_t *LUT,bool reset/*=true*/ )
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(LUT,1,sizeof(CTA_CaliResult_t),fp);

#ifdef ERROR_CODE
    if(count!=sizeof(CTA_CaliResult_t))
        ErrorExit(932);
#endif

    fclose(fp);
}

/**
*@brief		Dump TARGET_NUM
*/
void DumpTargetNum(char* filename,TARGET_NUM* i_target,bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(i_target,1,sizeof(TARGET_NUM),fp);

#ifdef ERROR_CODE
    if(count!=sizeof(CTA_CaliResult_t))
        ErrorExit(932);
#endif

    fclose(fp);

}

/**
*@brief		Dump HarrisCornerPos
*/
void DumpCorners( char* filename,HarrisCornerPos *i_corners,int cornerNum,bool reset/*=true*/ )
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(i_corners, cornerNum ,sizeof(HarrisCornerPos),fp);

#ifdef ERROR_CODE
    if(count!=(sizeof(HarrisCornerPos)*cornerNum))
        ErrorExit(932);
#endif

    fclose(fp);
}

/**
*@brief		Dump HarrisCornerPos2
*/
void DumpCorners2( char* filename, HarrisCornerPos2 *i_corners2, int cornerNum, bool reset/*=true*/ )
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(i_corners2, cornerNum, sizeof(HarrisCornerPos2), fp);

#ifdef ERROR_CODE
    if(count!=(sizeof(HarrisCornerPos2)*cornerNum))
        ErrorExit(932);
#endif

    fclose(fp);
}


/**
*@brief		Dump MinorBlock
*/
void DumpMinorBlock(char* filename,MinorBlock *i_blocks,int blockNum,bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(i_blocks, blockNum ,sizeof(MinorBlock),fp);

#ifdef ERROR_CODE
    if(count!=(sizeof(MinorBlock)*blockNum))
        ErrorExit(932);
#endif
    fclose(fp);
}
/**
*@brief		Dump CandidateRec
*/
void DumpCandidateRec( char* filename,CandidateRec *i_candRec, int CandNum,bool reset/*=true*/ )
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(i_candRec, CandNum,sizeof(CandidateRec),fp);

#ifdef ERROR_CODE
    if(count!=(CandNum * sizeof(CandidateRec) ) )
        ErrorExit(932);
#endif
    fclose(fp);
}

void DumpHeadLight(char* filename,HeadLight *i_lamp,int lampNum,bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(i_lamp, lampNum,sizeof(HeadLight),fp);

#ifdef ERROR_CODE
    if(count!=(CandNum * sizeof(CandidateRec) ) )
        ErrorExit(932);
#endif
    fclose(fp);

}
/**
*@brief		Dump World2Img LUT
*/
void DumpLUT_World2Img(char* filename,const unsigned int* LUT,bool reset)
{
    FILE* fp;
    int count;

    if(reset)
        fp=fopen(filename,"wb");
    else
        fp=fopen(filename,"ab+");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif
    count=fwrite(LUT,1, (CTA_TV_WIDTH*CTA_TV_HEIGHT*sizeof(unsigned int)) ,fp);

#ifdef ERROR_CODE
    if(count!=(CTA_TV_WIDTH*CTA_TV_HEIGHT*sizeof(unsigned int)))
        ErrorExit(932);
#endif

    fclose(fp);
}

#ifdef _USE_CTA_OWN_IO_FUNCTION_
/**
*@brief		Load partial image
*/
fpos_t LoadBitMap(char* filename,unsigned char* bitmap[],int start_idx,int end_idx,int size,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;
    int i;
    int total_count=0;

    fp=fopen(filename,"rb");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(959);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    for(i=start_idx; i<=end_idx; i++)
        total_count+=fread(bitmap[i],sizeof(unsigned char),size,fp);

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}
/**
*@brief		 Load image
* @param[in] size	     [Range : 1 ~ imageWidth * imageHeight]
*/
fpos_t LoadBitMap(char* filename,unsigned char* bitmap,int size,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;
    int count;

    fp=fopen(filename,"rb");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(959);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    count=fread(bitmap,sizeof(unsigned char),size,fp);

    if(count!=size)
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9591);
        else if(ferror(fp))
            ErrorExit(9592);
        else
            ErrorExit(9593);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

fpos_t LoadBitMap2(char* filename,unsigned short* bitmap,int size,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;
    int count;

    fp=fopen(filename,"rb");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(959);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    count = fread( bitmap,sizeof(unsigned short),size,fp );

    if(count!=(size*sizeof(unsigned short)))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9591);
        else if(ferror(fp))
            ErrorExit(9592);
        else
            ErrorExit(9593);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

#endif // END #ifdef _USE_CTA_OWN_IO_FUNCTION_

/**
*@brief		 Load partial Frame Header
* @param[in] size	     [Range : 1 ~ imageWidth * imageHeight]
*/
fpos_t LoadFrameHead( char* filename,CAN_Info* head,fpos_t start_pos/*=0*/ )
{
    FILE* fp;
    fpos_t end_pos;
    int count(0);

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(947);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    count += fread(&head->carSpeed,sizeof(head->carSpeed),1,fp);
    count += fread(&head->steerAngle,sizeof(head->steerAngle),1,fp);
    count += fread(&head->time_stamp,sizeof(head->time_stamp),1,fp);
    count += fread(&head->timeInterval,sizeof(head->timeInterval),1,fp);
    if(0 == count)
    {
#ifdef ERROR_CODE
            if(feof(fp))
                ErrorExit(9471);
            else if(ferror(fp))
                ErrorExit(9472);
            else
                ErrorExit(9473);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

/**
*@brief     Load Time management
*/
fpos_t LoadTimeManagement(char* filename, CTATimeManagement *TimeMa, fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp = fopen( filename, "rb" );
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread( TimeMa, sizeof(CTATimeManagement),1,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	

}
/**
*@brief		 Load whole LoadCarInfo
*/
fpos_t LoadCarInfo(char* filename,SimpleCarSpec *carSpec,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread(carSpec,sizeof(SimpleCarSpec),1,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

/**
*@brief		 Load whole TARGET_NUM
*/
fpos_t LoadTargetNum( char* filename,TARGET_NUM* i_target, fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread(i_target,sizeof(TARGET_NUM),1,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}
/**
*@brief    Load HarrisCornerPos
*/
fpos_t LoadCorners(char* filename,HarrisCornerPos *i_corners, int cornerNum,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread(i_corners,sizeof(HarrisCornerPos),cornerNum,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

/**
*@brief    Load MinorBlock
*/
fpos_t LoadMinorBlock(char* filename,MinorBlock *i_blocks,int blockNum,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread(i_blocks,sizeof(MinorBlock),blockNum,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;
}
/**
*@brief    Load CandidateRec
*/
fpos_t LoadCandidateRec(char* filename,CandidateRec *i_candRec, int candNum, fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread(i_candRec,sizeof(CandidateRec),candNum,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}


fpos_t LoadHeadLight(char* filename,HeadLight *i_lamp,int lampNum,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(933);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    if(!fread(i_lamp,sizeof(HeadLight),lampNum,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9331);
        else if(ferror(fp))
            ErrorExit(9332);
        else
            ErrorExit(9333);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	

}

/**
*@brief    Load Img2World LUT
*/
fpos_t LoadLUT_Img2World(char* filename, CTA_CaliResult_t *LUT,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;
    int count;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(931);
#endif
    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    count = fread(LUT,1,sizeof(CTA_CaliResult_t),fp);

    if( count != sizeof(CTA_CaliResult_t) )
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9311);
        else if(ferror(fp))
            ErrorExit(9312);
        else
            ErrorExit(9313);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

/**
*@brief	   Load World2Img LUT
*/
fpos_t LoadLUT_World2Img(char* filename,unsigned int* LUT,fpos_t start_pos)
{
    FILE* fp;
    fpos_t end_pos;
    int count;

    fp=fopen(filename,"rb");

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(931);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    count=fread(LUT,1,(CTA_TV_WIDTH*CTA_TV_HEIGHT*sizeof(unsigned int)),fp);

    if(count!=(CTA_TV_WIDTH*CTA_TV_HEIGHT*sizeof(unsigned int)))
    {
#ifdef ERROR_CODE
        if(feof(fp))
            ErrorExit(9311);
        else if(ferror(fp))
            ErrorExit(9312);
        else
            ErrorExit(9313);
#endif
    }

    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

#ifdef _USE_CTA_OWN_IO_FUNCTION_

static char g_strDebugFileName[300];
char * GetFullPathFileName( const char* a_strPath, const char * a_strFileName)
{
    if ( (NULL==a_strPath) || (NULL==a_strFileName) )
        return NULL;

    sprintf(g_strDebugFileName, "%s%s", a_strPath,a_strFileName);

    return g_strDebugFileName;
}

#endif // END #ifdef #ifdef _USE_CTA_OWN_IO_FUNCTION_