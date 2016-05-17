/*
===================================================================
Name		: APEX_CogniVue.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: Apex Process
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/12/29	  | v0.0010  	|First setup                	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef _APEX_CogniVue_H_
#define _APEX_CogniVue_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Def.h"
#ifdef _WIN32

#else   //k2
    #include "oTo_MemMgr.h"
    #include "Utilities.h"
    #include "MemMgr.h"
#endif
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

//******************************************************************************
// T Y P E   D E F I N I T I O N S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   V A R I A B L E S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
void* CTA_MemMgr_Allocate(UINT32 a_unBufSize);

void Apex_Gaussian(UCHAR *dst, UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY);
void Apex_SobelErosion( UCHAR *dst, UCHAR* vSobel_img, UCHAR* hSobel_img, UCHAR *src, INT32 imgDimensionX, INT32 imgDimensionY); 
void Apex_HarrisValue( UINT16 *o_harrisMap, UCHAR *Sobel_img, UCHAR* i_h_value,UCHAR* i_v_value, UCHAR *src, INT32 DimensionX, INT32 DimensionY, INT32 TH);
void Apex_ABSDiff(UCHAR *dst, UCHAR *src1 , UCHAR *src2, INT32 Thd ,int img_width, int img_height);
void Apex_Threshold(UCHAR *g_bin_img, CONST UCHAR *Gaussian_img, INT32 imgDimensionX, INT32 imgDimensionY, UCHAR THD_Binary); 

#endif // END #ifndef _APEX_CogniVue_H_

