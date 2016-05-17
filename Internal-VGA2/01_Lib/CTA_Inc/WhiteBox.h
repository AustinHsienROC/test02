/*
===================================================================
Name		: WhiteBox.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement ALL CTA Verification process

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/08/17	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
2015/1/7	  | v0.0020  	|Keep Data in Sync Across Platforms and Devices | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef _WHITEBOX_H_
#define _WHITEBOX_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#if( ON == PC_FILE_INFO )
    #include <fstream>
#endif
#include <stdio.h>          ///< FILE
#include <stdlib.h>

#ifdef _WIN32 
    #include "OpencvPlug.h"		///< Need Before "CTA_Type.h" due to VARIABLE File
    #include "FilesInfo.h"		
    #include "OSD_Ycc422.h"
#endif

#include "CTA_Def.h"
#include "CTA_CustomData.h"
#include "CTAApi.h"
#include "Point_Group.h"	///< #define MinorBlockType1/2/Noise
#include "Estimation.h"		///< #define CTATTC

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
#if defined( SYNC_ACROSS_PLATFORM) && defined(_WIN32)
#define C_MOD_DUMP_INPUT_PATH	".\\DumpData\\SystemInput\\"
#define C_MOD_DUMP_OUTPUT_PATH	".\\DumpData\\SystemOutput\\"
#define C_MOD_DUMP_PATH			".\\DumpData\\"
#else 
    #define C_MOD_DUMP_INPUT_PATH	"C:\\DumpData\\SystemInput\\"
    //#define C_MOD_DUMP_OUTPUT_PATH	"C:\\DumpData\\SystemOutput\\"
    #define C_MOD_DUMP_PATH			"C:\\DumpData\\"
#endif 


//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
void verify_SrcImg_Header( UCHAR *dst_Y, INT32 dst_width, INT32 dst_height, UCHAR *src_Y, INT32 src_width, INT32 src_height, CAN_Info *carState,CTATimeManagement &i_TimeM, TARGET_NUM &i_target, UCHAR *i_GaussPrev, UCHAR *Sobel_imgPrev, UCHAR *g_harrisMapPrev, HarrisCornerPos *cornerPrev, CandidateRec *CandRecPrevL, CandidateRec *CandRecPrevR,
                           int pointNumPrev);

void verify_WeatherType( UCHAR *dst_Y , INT32 imgDimensionX,INT32 imgDimensionY, weatherType_t isDay );
char* verify_DumpTrackingInfo( char* FrameReport,int& FrameReportIdx,CONST CandidateRec* CandidateL,CONST CandidateRec* CandidateR,CONST TARGET_NUM *targetNum);

#pragma region ----------------DayMode----------------
void verify_GaussianImg_HarrisCorner( UCHAR *dst_Y , UCHAR *Gaussian_img, UCHAR *harrisMap, UCHAR *sobel, UCHAR *vsobel, UCHAR *hsobel, UINT16 *harrisValue, 
                                      UINT16 *VSobelQty16x16, UINT16 *HSobelQty16x16,
                                      INT32 pointNumber , INT32 imgDimensionX , INT32 imgDimensionY );

void verify_MotionCorner(	UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                            HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev,
                            const INT32  RegionL[] ,
                            const INT32  RegionR[],
                            int pointNumPrev);

void verify_FLT_ABSDiff(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , UCHAR *g_img_FrameDiff , UCHAR *sobel, UCHAR *sobelPrev);

void verify_MinorBlockUpdate(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                             INT16 BlockSizeV , INT16 BlockSizeH, 
                             MinorBlock *CTAMinorBlockL, MinorBlock *CTAMinorBlockR, 
                             const INT32 RegionL[] , const INT32 RegionR[]);

void verify_Labeling(	UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                        INT16 BlockSizeH , INT16 BlockSizeV,
                        const INT32 RegionL[] , const INT32 RegionR[],
                        UINT16 LabelCountL , UINT16 LabelCountR,
                        MinorBlock *CTAMinorBlockL , MinorBlock *CTAMinorBlockR,
                        UINT16 *CTAMinorBlockL_EdgeQuantity, UINT16 *CTAMinorBlockR_EdgeQuantity);

void verify_CandidateUpdate(UCHAR *dst_Y , INT32 imgDimensionX , INT32 imgDimensionY , 
                            UINT16 CandidateCountL , UINT16 CandidateCountR , 
                            CandidateRec *CTACandidateL , CandidateRec *CTACandidateR );

void verify_CandidateFeatureUpdate( UCHAR *Gaussian_img , INT32 imgDimensionX , INT32 imgDimensionY , CandidateRec *CTACandidateL , CandidateRec *CTACandidateR, CONST INT32 FrameNo );

void verify_CNNPredict(	UCHAR *dst_Y ,INT32 imgDimensionX,INT32 imgDimensionY, UINT16 CandidateCountL, UINT16 CandidateCountR,
                        CandidateRec *CTACandidateL , CandidateRec *CTACandidateR );

void verify_Estimation( UCHAR * dst_Y,INT32 imgDimensionX,INT32 imgDimensionY, const INT32 *RegionL, const INT32 *RegionR, CandidateRec *CTACandidateL , CandidateRec *CTACandidateR , CandidateRec *CTACandidatePrevL, CandidateRec *CTACandidatePrevR,INT16 CountDownFlagAdpL , INT16 CountDownFlagAdpR, 
                        TARGET_NUM *pTargetNum);

void verify_CTADog( UCHAR *dst_Y , INT32 imgDimensionX,INT32 imgDimensionY, const INT32 *RegionL, const INT32 *RegionR, CandidateRec *CTACandidateL , CandidateRec *CTACandidateR , INT32 FrameNum);
void verify_CornersMap( UCHAR *dst_Y , UCHAR *harrisMap, INT32 pointNumber , INT32 imgDimensionX , INT32 imgDimensionY);

void verify_Update_Data_Before_Next_Frame(  UCHAR *Gaussian_img,UCHAR *Gaussian_imgPrev,
                                            UCHAR *Sobel_img, UCHAR *Sobel_imgPrev,
                                            HarrisCornerPos *harrisCorners , HarrisCornerPos *harrisCornersPrev,
                                            HarrisCornerPos2 *harrisCorners2 , HarrisCornerPos2 *harrisCorners2Prev,
                                            CandidateRec *CTACandidateL, CandidateRec *CTACandidatePrevL,
                                            CandidateRec *CTACandidateR, CandidateRec *CTACandidatePrevR,
                                            INT32 &FrameNum ,INT32 imgDimensionX,INT32 imgDimensionY,
                                            TARGET_NUM *pTargetNum);
#pragma endregion ----------------DayMode----------------

#pragma region    ----------------NightMode----------------
void verify_APEX01_Night( UCHAR *dst_Y , INT32 imgDimensionX, INT32 imgDimensionY, UCHAR *g_Binar_img );
void verify_ClusterIntoHeadlight(	UCHAR * dst_Y,INT32 imgDimensionX,INT32 imgDimensionY, 
                                    const INT32 RegionL[4], const INT32 RegionR[4],
                                    HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum],
                                    UINT16 count1L , UINT16 count1R);

void verify_HeadLightFeatureUpdate_1( UCHAR *dst_Y, INT32 imgDimensionX ,INT32 imgDimensionY, const INT32 RegionL[4] , const INT32 RegionR[4], HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum]);

void verify_HeadLightPredict_1( UCHAR *dst_Y , INT32 imgDimensionX ,INT32 imgDimensionY,
                                const INT32 RegionL[4] , const INT32 RegionR[4],
                                HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] ,
                                UINT16 count2L , UINT16 count2R);
void verify_Update_Data_Before_Next_Frame_Night( UCHAR *Gaussian_img, UCHAR *Gaussian_imgPrev , UCHAR *g_bin_img, UCHAR *g_bin_PrevImg, HeadLight lampL[MAX_HeadLightNum],HeadLight lampPrevL[MAX_HeadLightNum], HeadLight lampR[MAX_HeadLightNum],HeadLight lampPrevR[MAX_HeadLightNum], INT32 &FrameNum, INT32 imgDimensionX, INT32 imgDimensionY);
void verify_Estimation_Night(UCHAR *dst_Y , INT32 imgDimensionX, INT32 imgDimensionY, const INT32 RegionL[4],const INT32 RegionR[4], HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum],HeadLight lampPrevL[],  HeadLight lampPrevR[]);
void verify_CTADog_Night( UCHAR *dst_Y ,INT32 imgDimensionX,INT32 imgDimensionY,const INT32 RegionL[4],const INT32 RegionR[4], HeadLight lampL[MAX_HeadLightNum] ,HeadLight lampR[MAX_HeadLightNum] , UCHAR AlarmState , INT32 FrameNum);
#pragma endregion ----------------NightMode----------------

void verify_TimeCost( UCHAR *dst_Y , INT32 imgDimensionX ,INT32 imgDimensionY,CTATimeManagement &inCTATime , INT32 inIdx);



#ifdef SYNC_ACROSS_PLATFORM
char *GetFullPathFileName( const char* a_strPath, const char * a_strFileName);

void DumpBitMap(char* filename,unsigned char* bitmap[],int start_idx,int end_idx,int size,bool reset=true);
void DumpBitMap(char* filename,unsigned char* bitmap,int size,bool reset=true);
void DumpBitMap2(char* filename,unsigned short* bitmap,int size,bool reset=true);
void DumpFrameHead(char* filename,CAN_Info *head,bool reset=true);
void DumpTimeManagement(char* filename,CTATimeManagement *TimeMa, bool reset=true);
void DumpCarInfo(char* filename,SimpleCarSpec* carSpec,bool reset=true);
void DumpTargetNum(char* filename,TARGET_NUM* i_target,bool reset=true);
void DumpCorners(char* filename,HarrisCornerPos *i_corners,int cornerNum,bool reset=true);
void DumpCorners2( char* filename, HarrisCornerPos2 *i_corners2, int cornerNum, bool reset =true);
void DumpMinorBlock(char* filename,MinorBlock *i_blocks,int blockNum,bool reset=true);
void DumpCandidateRec(char* filename,CandidateRec *i_candRec, int CandNum,bool reset=true);
void DumpLUT_Img2World(char* filename,CTA_CaliResult_t *LUT,bool reset=true);
void DumpLUT_World2Img(char* filename,const unsigned int* LUT,bool reset=true);
void DumpHeadLight(char* filename,HeadLight *i_lamp,int lampNum,bool reset=true);



fpos_t LoadBitMap(char* filename,unsigned char* bitmap[],int start_idx,int end_idx,int size,fpos_t start_pos=0);
fpos_t LoadBitMap(char* filename,unsigned char* bitmap,int size,fpos_t start_pos=0);
fpos_t LoadBitMap2(char* filename,unsigned short* bitmap,int size,fpos_t start_pos=0);
fpos_t LoadFrameHead(char* filename,CAN_Info* head,fpos_t start_pos=0);
fpos_t LoadTimeManagement(char* filename,CTATimeManagement *TimeMa,fpos_t start_pos=0);
fpos_t LoadCarInfo(char* filename,SimpleCarSpec* carSpec,fpos_t start_pos=0);
fpos_t LoadTargetNum(char* filename,TARGET_NUM* i_target,fpos_t start_pos=0);
fpos_t LoadCorners(char* filename,HarrisCornerPos *i_corners, int cornerNum,fpos_t start_pos=0);
fpos_t LoadMinorBlock(char* filename,MinorBlock *i_blocks,int blockNum,fpos_t start_pos=0);
fpos_t LoadCandidateRec(char* filename,CandidateRec *i_candRec, int candNum, fpos_t start_pos=0);
fpos_t LoadLUT_Img2World(char* filename, CTA_CaliResult_t *LUT, fpos_t start_pos=0);
fpos_t LoadLUT_World2Img(char* filename,unsigned int* LUT,fpos_t start_pos=0);
fpos_t LoadHeadLight(char* filename,HeadLight *i_lamp,int lampNum,fpos_t start_pos=0);

/**
*@brief		Export single Value with any data type
*/
template <class T>
void DumpValue(char* filename,T* map,int count,bool reset = true)
{
    int wcount;

    /// Binary write a new File or append at last
    FILE* fp = (reset)?( fopen(filename,"wb") ):( fopen(filename,"ab+") );

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(945);
#endif
    wcount=fwrite(map,sizeof(T),count,fp);
    //////////////////////////////////////
    //work around for 32-bit value in K2	//
    if(sizeof(T)==4)										//
        fwrite(map,sizeof(T),count,fp);			//
    /////////////////////////////////////
#ifdef ERROR_CODE
    if(wcount!=count)
        ErrorExit(944);
#endif
    fclose(fp);
}
/**
*@brief		Export Array Value with any data type
*/
template <class T>
void DumpValues(char* filename,T* map[],int start_idx,int end_idx,bool reset = true)
{
    int i;
    int count;
    FILE* fp = (reset)?( fopen(filename,"wb") ):( fopen(filename,"ab+") );

#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(953);
#endif

    for(i=start_idx; i<=end_idx; i++)
    {
        count=0;
        count=fwrite(map[i],sizeof(T),1,fp);
#ifdef ERROR_CODE
        if(count!=1)
            ErrorExit(952);
#endif
    }
    fclose(fp);
}

/**
*@brief		Load Single Value with any data type
*/
template <class T>
fpos_t LoadValue(char* filename,T* map,int size,fpos_t start_pos=0)
{
    FILE* fp;
    fpos_t end_pos;
    int count;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(943);
#endif
    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);
    count=0;
    count=fread(map,sizeof(T),size,fp);

    //////////////////////////////////////
    //work around for 32-bit value in K2	//
    if(sizeof(T)==4)										//
        fread(map,sizeof(T),size,fp);				//
    /////////////////////////////////////

#ifdef ERROR_CODE
    if(count!=size)
    {
        if(feof(fp))
            ErrorExit(9431);
        else if(ferror(fp))
            ErrorExit(9432);
        else
            ErrorExit(9433);
    }
#endif
    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}

/**
*@brief		Load Array Value with any data type
*/
template <class T>
fpos_t LoadValues(char* filename,T* map[],int start_idx,int end_idx,fpos_t start_pos=0)
{
    FILE* fp;
    fpos_t end_pos;
    int i;

    fp=fopen(filename,"rb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(942);
#endif

    if(start_pos!=0)//continue reading
        fsetpos(fp,&start_pos);

    for(i=start_idx; i<=end_idx; i++)
    {
        if(!fread(map[i],sizeof(T),1,fp))
    {
#ifdef ERROR_CODE
        if(feof(fp))
                ErrorExit(9421);
        else if(ferror(fp))
                ErrorExit(9422);
        else
                ErrorExit(9423);
#endif
        }
    }
    fgetpos(fp,&end_pos);
    fclose(fp);
    return end_pos;	
}
#endif // end of SYNC_ACROSS_PLATFORM

#ifdef _WIN32

/**
*@brief		dump image with region: rectangle from (s_x,s_y) to (e_x,e_y) 
*@param[in] binary		[true: either 0 or 255 in value][false: gray level [0,255] in value]
*/
template <class T>
void DumpImageROI(char* filename,int s_x,int s_y,int e_x,int e_y,int img_width,T* img,bool binary=false)
{
    FILE* fp;
    int i,j;
    int s_idx=s_y*img_width+s_x;
    int idx;
    int value;

    fp=fopen(filename,"wb");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(974);
#endif
    for(j=s_y; j<e_y; j++,s_idx+=img_width)
        for(i=s_x,idx=s_idx; i<e_x; i++,idx++)
        {
            value=img[idx];
            if(binary)
            {
                if(value>0) value=255;
                else value=0;
            }
            fputc(value,fp);
        }
    fclose(fp);
}

/**
*@brief		dump image with region: dump value in text file
*@param[in] binary		[true: either 0 or 255 in value][false: gray level [0,255] in value]
*/
template <class T>
void DumpValueToText(char* filename,int s_x,int s_y,int e_x,int e_y,int img_width,T* img,bool binary=false)
{
    FILE* fp;
    int i,j;
    int s_idx=s_y*img_width+s_x;
    int idx;
    int value;

    fp=fopen(filename,"w");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(974);
#endif
    fprintf(fp,"idx ",0);
    for(i=s_x; i<e_x; i++)
        fprintf(fp,"[%d] ",i);

    for(j=s_y; j<e_y; j++,s_idx+=img_width)
    {
        fprintf(fp,"\n[%d] ",j);
        for(i=s_x,idx=s_idx; i<e_x; i++,idx++)
        {
            value=img[idx];
            if(binary)
            {
                if(value>0) value=255;
                else value=0;
            }
            //fput(value,fp);
            fprintf(fp,"%d ",value);
        }
    }
    fclose(fp);
}

//K2 does not support itoa
#ifdef _WIN32

/**
*@brief		UNKNOWN
*/
template <class T>
void DumpVerticalHistogram(char* filename,int s_x,int s_y,int e_x,int e_y,int img_width,T* img,bool binary=false)
{
    FILE* fp;
    static int file_idx=0;
    int i,j;
    int s_idx=s_y*img_width+s_x;
    int idx;//image idx
    int value;
    char f_idx[3];
    char filenameA[30];

    itoa(file_idx++,f_idx,10);
    strncpy(filenameA,filename,strlen(filename)-4);
    filenameA[strlen(filename)-4]='\0';
    strcat(filenameA,"_");
    strcat(filenameA,f_idx);
    strcat(filenameA,filename+strlen(filename)-4);

    fp=fopen(filenameA,"w");
#ifdef ERROR_CODE
    if(fp==NULL)
        ErrorExit(974);
#endif
    fprintf(fp,"STR=(%d,%d) ",s_x,s_y);

    for(j=s_y; j<e_y; j++,s_idx+=img_width)
    {
//		fprintf(fp,"\n[%d] ",j);
        for(i=s_x,idx=s_idx; i<e_x; i++,idx++)
        {
            value=img[idx];
            if(binary)
            {
                if(value>0) value=255;
                else value=0;
            }
            //fput(value,fp);
            fprintf(fp,"%d ",value);
        }
    }
    fclose(fp);
}
#endif // END #ifdef _WIN32

/**
*@brief		UNKNOWN PURPOSE
*/
template <class T>
void CopyDataROI(T* dst, const T* src, const short img_width,const short img_height, const Rect_t* roi_1,const Rect_t* roi_2=NULL)
{
    int i,j;
    int idx,s_idx;//start_idx

    memset(dst,0,sizeof(T)*img_width*img_height);

    s_idx=roi_1->start_row*img_width+roi_1->start_col;
    for(j=roi_1->start_row; j<roi_1->height; j++,s_idx+=img_width)
    for(idx=s_idx, i=roi_1->start_col; i<roi_1->width; i++, idx++)
        dst[idx]=src[idx];

    if(roi_2)
    {
        s_idx=roi_2->start_row*img_width+roi_2->start_col;
        for(j=roi_2->start_row; j<roi_2->height; j++,s_idx+=img_width)
        for(idx=s_idx, i=roi_2->start_col; i<roi_2->width; i++, idx++)
            dst[idx]=src[idx];
    }
}
#endif//_WIN32

#endif//_WHITEBOX_H_