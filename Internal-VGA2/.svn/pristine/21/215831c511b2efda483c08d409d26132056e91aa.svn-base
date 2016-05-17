/*
===================================================================
Name		: CoordinateConversion.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement Coordinate Conversion from 
              World model <-> image 2D model <-> Camera model
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/09	  | v0.0010  	|First setup                	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef _CoordinateConversion_H_
#define _CoordinateConversion_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************

#include "CTA_Type.h"
#include "CTA_Def.h"


//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

#define CTA_TV_WIDTH    (576*2)     // CTA top-view width  (5cm/pixel)
#define CTA_TV_HEIGHT   (  120)     // CTA top-view height (5cm/pixel)
#define CTA_TV_COMPRESS  5			// CTA top-view is compress 1cm/pixel


//-------------FORMULAR-------------------------------//
//change from [LUT world coordinate] to [subject car back world coordinate] 
#define XwShiftToBackCenter(tmpXw)  (tmpXw - ((CTA_TV_WIDTH)>>1)*CTA_TV_COMPRESS)     //(cm) tmpXw is LUT value 
#define YwShiftToBackCenter(tmpYw)  (CTA_TV_HEIGHT*CTA_TV_COMPRESS - tmpYw)			  //(cm) tmpYw is LUT value 

//change from [subject car back world coordinate] to [LUT world coordinate]
#define INV_XwShiftToBackCenter(tmpXw)  (tmpXw + ((CTA_TV_WIDTH)>>1)*CTA_TV_COMPRESS) //(cm) tmpXw is LUT value 
#define INV_YwShiftToBackCenter(tmpYw)  (CTA_TV_HEIGHT*CTA_TV_COMPRESS - tmpYw)			//(cm) tmpYw is LUT value 


#define WModel WorldModel::getInstance()
//******************************************************************************
// T Y P E   D E F I N I T I O N S
//******************************************************************************

/**
* @struct	CTA_CaliResult_t Calibration Result [ Img to TopView ] LUT 
* @brief	TopView_forward_LUT save the UINT32 data of mapping (MSB16:Y / LSB16:X)
* @param	ROI_start_x				refer to DesignDoc(the image pos the LUT ROI start )
* @param	ROI_width				refer to DesignDoc
* @param	start_x_lut				refer to DesignDoc(the start of purple at row[i])
* @param	end_x_lut				refer to DesignDoc(the end   of purple at row[i])
* @param	TopView_forward_LUT		refer to DesignDoc(the sufficient memory of LUT ROI [640,360 make sure enough buffer ])
*/
#define SRC_WIDTH       ( SrcImg_W)   // original view image width , in pixels
#define SRC_HEIGHT      ( SrcImg_H)   // original view image height, in pixels 
typedef struct {
    unsigned int version;
    unsigned short ROI_start_x;
    unsigned short ROI_start_y;
    unsigned short ROI_width;
    unsigned short ROI_height;
    unsigned short start_x_lut[SRC_HEIGHT];
    unsigned short end_x_lut[SRC_HEIGHT];
    unsigned int TopView_forward_LUT[SRC_WIDTH*SRC_HEIGHT];   
} CTA_CaliResult_t;

//******************************************************************************
// E X T E R N A L   V A R I A B L E S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
/**
* @class  WorldModel "CoordinateConversion.h"
* @brief  <Img><-><World> 2 LUTs
*/
class WorldModel
{
private:
    UINT32 dataBuf;
    static WorldModel *instance;
public:
    CTA_CaliResult_t *pCTA_Cali;
    UINT32           *pCTA_LUT_TOPVIEW;

public:
    WorldModel(void);
    ~WorldModel(void);

    static WorldModel* getInstance();
    static void ResetInstance();

	BOOL PosWorldToImg( CONST INT32 PosXw,CONST INT32 PosYw, INT32 &Posxi, INT32 &Posyi );
	BOOL PosImgToWorld( INT32 &PosXw, INT32 &PosYw,CONST INT32 Posxi,CONST INT32 Posyi );

    void testDriveCC01();
};

#endif // END #ifndef _CoordinateConversion_H_

