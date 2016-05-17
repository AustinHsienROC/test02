/*
===================================================================
Name		: OSD_Ycc422.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement OSD_Plot main process.

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2015/03/16	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/


#ifndef _OSD_Ycc422_H
#define _OSD_Ycc422_H
#ifdef WIN32    // PC

#include "CTA_Type.h"
#include "CTA_Def.h"

class OSD_Ycc422
{
public:

    //---------------------Pencil_Draw---------------------
    static void Rgb2Ycc(UCHAR R, UCHAR G, UCHAR B, UCHAR *Y, UCHAR *Cb, UCHAR *Cr);
    static void OSD_SetColor(UCHAR R, UCHAR G, UCHAR B);

    static void OSD_Draw_Pixel(UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 x, UINT32 y , FLOAT alpha = 1.0);
    static void OSD_Draw_Line (UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 start_x, INT32 start_y, INT32 end_x, INT32 end_y, UINT32 thickness, FLOAT alpha = 1.0);
    static void OSD_Draw_Rec  (UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 start_x, INT32 start_y, INT32 end_x, INT32 end_y, UINT32 thickness, BOOL isFilled, FLOAT alpha = 1.0);
    static void OSD_Draw_Digit(UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 start_x, INT32 start_y, UCHAR digitNum, UINT32 thickness ,UINT32 size);
 
    static void OSD_Draw_INT32(INT32 value , UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 start_x, INT32 start_y, UINT32 thickness ,UINT32 size);
    static void OSD_Draw_Text (CONST char *str , UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 start_x, INT32 start_y,UINT32 size);
    static void OSD_Draw_Circle(UCHAR *o_ycc422, UINT32 dst_width, UINT32 dst_height, INT32 center_x, INT32 center_y, INT32 radius, UINT32 thickness, BOOL DrawPartial[4]);

    //---------------------ImgPaste---------------------
    static void ImgPaste_Y(UCHAR *o_ycc422, UCHAR *Y , UINT32 inWIDTH, UINT32 inHEIGHT);
    static void ImgPaste_rgb444(UCHAR *o_ycc422, UCHAR *r, UCHAR *g, UCHAR *b , UINT32 inWIDTH, UINT32 inHEIGHT);

    //---------------------ImgExtract/Transform---------------------
    static void Extract_Y(UCHAR *o_Y, UCHAR *i_ycc422, INT32 img_width, INT32 img_height);
    static void Extract_CC(UCHAR *o_Cb, UCHAR *o_Cr, UCHAR *i_ycc422, INT32 img_width, INT32 img_height);
    static void Format_rgb444(UCHAR *o_R, UCHAR *o_G, UCHAR *o_B, UCHAR *i_ycc422, INT32 img_width, INT32 img_height);

};

#endif
#endif