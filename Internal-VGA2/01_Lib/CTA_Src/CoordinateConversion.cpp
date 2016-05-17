//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
  #include "stdafx.h"
#else // K2 platform
  #include "GlobalTypes.h"
#endif

#include "CoordinateConversion.h"

//******************************************************************************
// I N T E R N A L   V A R I A B L E S
//******************************************************************************
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
WorldModel* WorldModel::instance = NULL;

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

/**
* @brief  �@�ɮy��(���I:�̥���� ���(5cm) ) �d�� �v���y��(���I ���W�� ���j���(1pixel) ) 
* @n	  [CTA_TopView1152x120_lut.hex]
*/
BOOL WorldModel::PosWorldToImg( CONST INT32 PosXw,CONST INT32 PosYw, INT32 &Posxi, INT32 &Posyi )
{
    if(PosXw <0 || PosXw>=CTA_TV_WIDTH || PosYw<0 || PosYw>=CTA_TV_HEIGHT)
    {
        Posxi = Posyi = 0;
        return FALSE;
    }
    if( NULL == pCTA_LUT_TOPVIEW ) return FALSE;
    dataBuf = pCTA_LUT_TOPVIEW[ PosXw + PosYw * CTA_TV_WIDTH ];
    Posxi   = ( dataBuf & (0x0000FFFF) );
    Posyi   = ( dataBuf >> 16		   );
	return TRUE;
}

/**
* @brief  �v���y��(���I�e�����W��  ���j���(1pixel) ) �d�� �@�ɮy��(���I:�̥���� ���(1cm) ) //
* @n	  [CTA_calib.hex]
* 20150302: world dimension=2880*2 cm in width, 600cm in depth
* can call XwShiftToBackCenter()/YwShiftToBackCenter() to shift to car center
*/
BOOL WorldModel::PosImgToWorld( INT32 &PosXw, INT32 &PosYw,CONST INT32 Posxi,CONST INT32 Posyi )
{
    if( NULL == pCTA_Cali ) return FALSE;
    
    if( Posxi < (pCTA_Cali->ROI_start_x + pCTA_Cali->ROI_width) && ( Posxi >=(pCTA_Cali->ROI_start_x) )&&
        Posyi < (pCTA_Cali->ROI_start_y + pCTA_Cali->ROI_height)&& ( Posyi >=(pCTA_Cali->ROI_start_y) )) // in ROI
    {
        PosXw = ( pCTA_Cali->TopView_forward_LUT[ (Posxi - pCTA_Cali->ROI_start_x) + (Posyi - pCTA_Cali->ROI_start_y) * (pCTA_Cali->ROI_width)] & (0x0000FFFF) ); ///< 0xFFFFFFFF if OUT OF BOUNDARY
        PosYw = ( pCTA_Cali->TopView_forward_LUT[ (Posxi - pCTA_Cali->ROI_start_x) + (Posyi - pCTA_Cali->ROI_start_y) * (pCTA_Cali->ROI_width)] >> 16 );

       /* old version
        UINT16 start_x = Posxi - CTA_Cali.ROI_start_x ;
        UINT16 start_y = Posyi - CTA_Cali.ROI_start_y ;
        UINT16 width   = CTA_Cali.ROI_width ;
        UINT16 height  = CTA_Cali.ROI_height ;

        PosXw = ( CTA_Cali.TopView_forward_LUT[ start_x + start_y * width] & (0x0000FFFF) ); ///< 0xFFFFFFFF if OUT OF BOUNDARY
        PosYw = ( CTA_Cali.TopView_forward_LUT[ start_x + start_y * width] >> 16 );
        */
		return TRUE;
    }
    else // OUT OF BOUNDARY
    {
        PosXw = 0xFFFF;
        PosYw = 0xFFFF;
		return FALSE;
    }
}


/**
* @brief	test Drive to verification the LUT
*/
void WorldModel::testDriveCC01()
{
    
    #pragma region [ESTIMATION Img2World]
    INT32 tmpXw3,tmpYw3;
    int min1 = 99999;
    int max1 = 0;
    for ( int i1 = 0; i1 < SrcImg_H; ++i1)
    {
        PosImgToWorld( tmpXw3 ,tmpYw3, (SrcImg_W>>1),i1 );
        if(tmpYw3!= 65535)
        {
            if( tmpYw3>max1 )
                max1 = tmpYw3;
            if( tmpYw3<min1 )
                min1 = tmpYw3;
        }
    }
    /// The Max Yw in Img2World LUT (ex. 599)
    max1;
    /// The Min Yw in Img2World LUT (ex.16)
    min1;

    INT32 minXw = 99999; ///< (cm)
    INT32 maxXw = 0;     ///< (cm)
    for ( int i1 = 0; i1 < SrcImg_W; ++i1)
    {
        PosImgToWorld( tmpXw3 ,tmpYw3, i1, 250);
        if(tmpXw3 != 65535)
        {
            if( tmpXw3>maxXw )
                maxXw = tmpXw3;
            if( tmpXw3<minXw )
                minXw = tmpXw3;
        }
    }
    /// The Max Xw in Img2World LUT (ex. 5960)
    maxXw;
    /// The Min Xw in Img2World LUT (ex.0)
    minXw;
    #pragma endregion

    #pragma region [ESTIMATION World2Img]
    INT32 tmpXw4 = CTA_TV_WIDTH*CTA_TV_COMPRESS-1; ///(cm) ori: top-Left [0 ~ CTA_TV_WIDTH*CTA_TV_COMPRESS ]
    INT32 tmpYw4 = 0;                              ///(cm) ori: top-Left [0 ~ CTA_TV_HEIGHT*CTA_TV_COMPRESS ]
    INT32 tmpxi4,tmpyi4;
    PosWorldToImg(tmpXw4/CTA_TV_COMPRESS , tmpYw4/CTA_TV_COMPRESS , tmpxi4,tmpyi4);
    if( (0!= tmpxi4) &&( 0 != tmpyi4) )
    {
        /// correct POSx/y
        tmpxi4; // ex. 55
        tmpyi4; // ex. 222
    }
    #pragma endregion

}
WorldModel* WorldModel::getInstance()
{
    if(!instance)
        instance = new WorldModel;
    return instance;
}

WorldModel::WorldModel(void)
{
    pCTA_Cali        = NULL;
    pCTA_LUT_TOPVIEW = NULL;
}
WorldModel::~WorldModel(void)
{
}

void WorldModel::ResetInstance()
{
    delete instance; 
    instance = NULL; 
}
