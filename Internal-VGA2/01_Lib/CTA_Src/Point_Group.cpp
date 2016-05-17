//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#ifdef _WIN32
  #include "stdafx.h"
#else // K2 platform
  #include "GlobalTypes.h"
#endif
#include <string.h>
#include "Point_Group.h"

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
Point_Group* Point_Group::instance = NULL;

//******************************************************************************
// I N T E R N A L    V A R I A B L E S
//******************************************************************************
static UCHAR  LabelArr  [ MAXCandidateNum ];
static UINT16 EqualLabel[  MAXCandidateNum * MAXCandidateNum ]; // 256x256/8/8
static UINT16 sortDecendent[4];

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************
static UINT16 Labeling_TwoPass( UCHAR *inSrc , UINT16 Width, UINT16 Height);
static UINT16 Labeling_SeedFilling( UCHAR *inSrc , UINT16 Width, UINT16 Height);
static UINT16 isolatedBlock( UINT16 posX, UINT16 posY, UCHAR *inArr, UINT16 *NeighborGId);
static BOOL   ArrPushInOreder( UINT16 *inPtr  , UINT16 inLength , UINT16 inData);
static BOOL   ArrFindGroup( UINT16 &outMinGroupID ,UINT16 *inPtr , UINT16 inHeight , UINT16 inWidth , UINT16 inGroupID, UINT16 &Dbg_x, UINT16 &Dbg_y);


/**
* @brief		 Two Pass Label Algorithm
* @param[in,out] inSrc	Output/Input Arr
* @return				MAX GroupNum
*/
static UINT16 Labeling_TwoPass( UCHAR *inSrc , UINT16 Width, UINT16 Height)
{

    UINT16 EqualLabelHeight = (Width * Height);
    UINT16 EqualLabelWidth  = (Width * Height);
    UINT16 EqualLabelIdx_V  = 0;
    UCHAR  GroupIdx         = 1;

    /// Initial Equal Label
    memset( EqualLabel , 0 , sizeof(EqualLabel) );

    //1. First pass
    UINT16 NeighborId[8] ={ 0 };
    for ( int vert = 0 ; vert <  Height ; ++vert)
    {
        for ( int hori = 0 ; hori < Width ; ++hori)
        {
            if( 255 <= inSrc[ hori+vert*(Width) ] )
            {
                UINT16 NeighborCount = isolatedBlock( hori , vert , inSrc , NeighborId );
                if(  0 >= NeighborCount  )
                {
                    inSrc[ hori+vert*(Width) ] = GroupIdx;
                    GroupIdx++;
                }
                else
                {
                    /// �NNeighbors�����̤p�Ƚᤩ��B(x,y):
                    inSrc[ hori + vert * (ROISegBlockNum_H) ] = NeighborId[0];

                    /// �O��Neighbors���U�ӭȡ]label�^�������۵����Y. //(��í�w)
                    if( NeighborCount > 1 )
                    {
                        
                        /// Find the proper Vert
                        UINT16 EqualRowNum = 0 ;
                        BOOL isFind        = FALSE ;

                        for ( int i1 = 0 ; i1 <=EqualLabelIdx_V; ++i1 )
                        {
                            for ( int i2 = 0 ; EqualLabel[ i2+i1*(EqualLabelWidth) ] >0; ++i2 )
                            {
                                if( EqualLabel[ i2+i1*(EqualLabelWidth) ] ==  NeighborId[0])
                                {
                                    EqualRowNum = i1;
                                    isFind = TRUE;
                                    i1 = EqualLabelIdx_V+1; /// break loop
                                    break;
                                }
                            }
                        }
                        
                        if( FALSE == isFind )
                        {
                            EqualRowNum = EqualLabelIdx_V;
                            EqualLabelIdx_V++;
                        }

                        /// addin proper hori
                        for ( int i1 = 0 ; NeighborId[ i1 ] > 0  ; ++i1)
                        {
                            ArrPushInOreder( &EqualLabel[EqualRowNum*(EqualLabelWidth)]  , EqualLabelWidth , NeighborId[i1] );
                        }
                    }
                }
            } //end of if( 255 <= inSrc[ hori+vert*(Width) ] )
        }// end of ( hori < Width )
    }// end of ( vert < Height )

//delete [] NeighborId; NeighborId = NULL;
//debug info
//   UCHAR debArr[256] ={0};
//    memcpy(debArr,inSrc,256*sizeof(UCHAR));

    ///2. second pass
    for ( int vert = 0 ; vert <  Height ; ++vert)
    {
        for ( int hori = 0 ; hori < Width ; ++hori)
        {
            if( 1 < inSrc[ hori+vert*(Width) ] )
            {
                UINT16 MinGroupID =0;
                UINT16 Dbg_x; 
                UINT16 Dbg_y;
                UINT16 tmpID = (UINT16)(inSrc[ hori+vert*(Width)]);
                BOOL isSuccess = ArrFindGroup( MinGroupID ,EqualLabel , EqualLabelHeight , EqualLabelWidth ,tmpID , Dbg_x, Dbg_y);
                
                if( TRUE == isSuccess)
                {
                    inSrc[ hori+vert*(Width) ] = MinGroupID;
                }
            }
        }
    }

    //delete [] EqualLabel; EqualLabel = NULL;

    /// the MAX Label Number at final (Get rid of the temporary label)
    UINT16 LabelNum = 0;
    for ( int i1 = 0 ; i1 < (Height * Width) ; ++i1)
        LabelNum =	MAX(LabelNum,inSrc[i1]); 

    return LabelNum;
}

typedef struct _SeedFillingUnit
{
    UINT16 x;
    UINT16 y;
}SeedFillingUnit;

SeedFillingUnit SeedFillingStack[256];

/**
* @brief		 �N inSrc �� (in_x,in_y) �P��|�I =255���I��J SeedFillingStack[Idx_push/Idx_push+1/..]��
* @param[in,out] inSrc	Output/Input Arr
*/
static void SeedFilling_PushNeighbors( UCHAR *inSrc , UINT16 &Width, UINT16 &Height ,
                                        SeedFillingUnit *SeedFillingStack ,INT16 &Idx_push , INT16 in_x, INT16 in_y)
{
    const INT16 ConnectedNeighbor = 4; ///< 4: 4-Connected Neighbor / 8: 8-Connected-Neighbor

    ///1) Initial Variable
    INT16 tmpHori;
    INT16 tmpVert;
    INT16 Pos[16];
    Pos[0]  = in_x-1;	Pos[1]  = in_y+0;
    Pos[2]  = in_x+0;	Pos[3]  = in_y-1;
    Pos[4]  = in_x+1;	Pos[5]  = in_y+0;
    Pos[6]  = in_x+0;	Pos[7]  = in_y+1;

    Pos[8]  = in_x-1;	Pos[9]  = in_y+1;
    Pos[10] = in_x-1;	Pos[11] = in_y-1;
    Pos[12] = in_x+1;	Pos[13] = in_y-1;
    Pos[14] = in_x+1;	Pos[15] = in_y+1;

    ///2) Push into SeedFillingStack
    for ( INT16 i1 = 0 ; i1 < (ConnectedNeighbor*2) ; i1+=2)
    {
        tmpHori = Pos[i1];
        tmpVert = Pos[i1+1];

        if( (tmpHori < 0) || (tmpHori>=Width) || (tmpVert < 0) ||(tmpVert>=Height) ){} //Out of boundary
        else if( 255 == inSrc[ tmpHori + tmpVert* Width ] )
        {
            SeedFillingStack[Idx_push].x = tmpHori;
            SeedFillingStack[Idx_push].y = tmpVert;
            Idx_push++;
        }
    }
}
/**
* @brief		 Seed-filling connected component (Label) 4- or 8-connected Algorithm
* @param[in,out] inSrc	Output/Input Arr
* @return				GroupNum ex.5 means 1-5 valid group
*/
static UINT16 Labeling_SeedFilling( UCHAR *inSrc , UINT16 Width, UINT16 Height)
{
    /*
    �]1�^���y�Ϲ��A������e�����IB(x,y) == 1�G
            a�B�NB(x,y)�@���ؤl�]������m�^�A�ýᤩ��@��label�A�M��(push)�N�Ӻؤl�۾F���Ҧ��e�����������J�̤��F
            b�B(pop)�u�X�̳������A�ᤩ��ۦP��label�A�M��A�N�P�Ӵ̳������۾F���Ҧ��e�����������J�̤��F
            c�B����b�B�J�A����̬��šF
            ���ɡA�K���F�Ϲ�B�����@�ӳs�q�ϰ�A�Ӱϰ줺�������ȳQ�аO��label�F
    �]2�^���Ʋġ]1�^�B�A���챽�y�����F
            ���y������A�N�i�H�o��Ϲ�B���Ҧ����s�q�ϰ�F
    */
    
    /// 1) Algorithm
    INT32 GroupID  = 1;		///< Group ID assign
    INT16 Idx_push = 0;		///< the SeedFillingStack Index
    for (INT16 vert = 0 ; vert < Height ;++vert)
    {
        for (INT16 hori = 0 ; hori < Width ;++hori)
        {
            if( 255 <= inSrc[ hori + vert* Width ] ) // Pixel(x,y) == 255�G
            {
                ///1.1 Initial
                INT16 tmp_x;
                INT16 tmp_y;
                memset( SeedFillingStack,0,sizeof(SeedFillingStack) ); //1024 Bytes
                SeedFillingStack[0].x = hori;
                SeedFillingStack[0].y = vert;
                Idx_push              = 1;

                SeedFilling_PushNeighbors(	inSrc , Width, Height ,
                                            SeedFillingStack , Idx_push ,  
                                            SeedFillingStack[0].x, SeedFillingStack[0].y);

                //1.2 iterative pop-push
                while ( Idx_push > 0)
                {
                    /// POP a SeedFillingUnit
                    tmp_x = SeedFillingStack[Idx_push-1].x ;
                    tmp_y = SeedFillingStack[Idx_push-1].y ;

                    inSrc[ tmp_x + tmp_y* Width ]  = GroupID ;
                    SeedFillingStack[Idx_push-1].x = 0;
                    SeedFillingStack[Idx_push-1].y = 0;

                    Idx_push--;

                    /// PUSH Neighbors
                    SeedFilling_PushNeighbors(	inSrc , Width, Height ,
                                                SeedFillingStack ,Idx_push ,  
                                                tmp_x, tmp_y);
                }
                
                GroupID++; // New Group Label
            } // end of if( 255 <= inSrc[ hori + vert* Width ] ) // Pixel(x,y) == 255�G
        } // end of hori < Width
    } // end of vert < Height

    return (GroupID-1); //GroupNum ex.5 means 1-5 valid group
}
/**
* @brief	  Array insertion in order w/o repeat Number (Verified)
* @param[out] inPtr			Output/Input Ordered-Array Ptr
* @param[in]  inPushIdx		Input default pushed position
* @param[in]  inLength		Input Array Length
* @param[in]  inData		Input Data.
* @return	  isSuccess
*/
static BOOL ArrPushInOreder( UINT16 *inPtr  , UINT16 inLength , UINT16 inData)
{
    /// Find the Last Position(inPushIdx)
    UINT16 inPushIdx = 999;
    for ( UINT16 i1 = 0 ; i1< inLength ; ++i1)
    {
        if( 0 == inPtr[i1]  )
        {
            inPushIdx = i1;
            break;
        }
    }
    if( inPushIdx > (inLength-1))
        return FALSE;

    inPtr[inPushIdx] = inData;
    UINT16 tmp01;
    for ( int i1 = 0; i1 < inPushIdx ; ++i1)
    {
        if( inPtr[inPushIdx] == inPtr[i1] )
        {
            inPtr[inPushIdx] = 0;
            return TRUE ; //Repeat Num DO NOT add.
        }
        else if( inPtr[inPushIdx]< inPtr[i1] )
        {
            /// SWAP
            tmp01 = inPtr[i1];
            inPtr[i1] = inPtr[inPushIdx];
            inPtr[inPushIdx] = tmp01;
        }
    }
    return TRUE;
}

/**
* @brief	  IsolatedBlock is check the 4-special-Neighbor(LEFT,UpLeft,Up,UpRight) GroupID 
* @param[in]  inCTAMinorBlock	Input UCHAR arr ptr
* @param[out] NeighborGId		Output Neighbor ID in increasely ordered
* @return	  count the Neighbor groups number
*/
static UINT16 isolatedBlock( UINT16 posX, UINT16 posY, UCHAR *inArr, UINT16 *NeighborGId)
{
    memset(NeighborGId,0,8*sizeof(UINT16));

    INT16 tmpY , tmpX;
    //UINT16 tmpGID;
    UINT16 count = 0;

    /// 4 Point
    tmpY = posY;
    tmpX = posX-1;

    if( (tmpX<0) || (tmpX>=ROISegBlockNum_H) ||(tmpY<0) || (tmpY>=ROISegBlockNum_V) ){}
    else
    {
        if( ( 0 != inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ] ) )
        {
            BOOL isRepeat = FALSE;
            for ( int i1 = 0 ; i1<4;++i1)
            {
                if( NeighborGId[i1] == inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ] )
                    isRepeat = TRUE;
            }
            if( FALSE == isRepeat)
            {
                NeighborGId[ count ] = inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ];
                count++;
            }
        }
    }

    tmpY = posY-1;
    tmpX = posX-1;
    if( (tmpX<0) || (tmpX>=ROISegBlockNum_H) ||(tmpY<0) || (tmpY>=ROISegBlockNum_V) ){}
    else
    {
        if(  0 !=  inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ] )
        {
            BOOL isRepeat = FALSE;
            for ( int i1 = 0 ; i1<4;++i1)
            {
                if( NeighborGId[i1] == inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ] )
                    isRepeat = TRUE;
            }
            if( FALSE == isRepeat)
            {
                NeighborGId[ count ] = inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ];
                count++;
            }
        }
    }

    tmpY = posY-1;
    tmpX = posX;
    if( (tmpX<0) || (tmpX>=ROISegBlockNum_H) ||(tmpY<0) || (tmpY>=ROISegBlockNum_V) ){}
    else
    {
        if( ( 0 != inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ]  ) )
        {
            BOOL isRepeat = FALSE;
            for ( int i1 = 0 ; i1<4;++i1)
            {
                if( NeighborGId[i1] == inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ] )
                    isRepeat = TRUE;
            }
            if( FALSE == isRepeat)
            {
                NeighborGId[ count ] = inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ];
                count++;
            }
        }
    }

    tmpY = posY-1;
    tmpX = posX+1;
    if( (tmpX<0) || (tmpX>=ROISegBlockNum_H) ||(tmpY<0) || (tmpY>=ROISegBlockNum_V) ){}
    else
    {
        if( ( 0 != inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ]  ) )
        {
            BOOL isRepeat = FALSE;
            for ( int i1 = 0 ; i1<4;++i1)
            {
                if( NeighborGId[i1] == inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ] )
                    isRepeat = TRUE;
            }
            if( FALSE == isRepeat)
            {
                NeighborGId[ count ] = inArr[ tmpX+ tmpY*(ROISegBlockNum_H) ];
                count++;
            }
        }
    }

    /// sort NeighborGId
    UINT16 tmp01;
    for ( int i1 = 0 ; i1 < (count) ; ++i1)
    {
        for ( int i2 = 1 ; i2 < (count) ; ++i2)
        {
            if(NeighborGId[i1] > NeighborGId[i2])
            {
                /// swap
                tmp01 = NeighborGId[i2];
                NeighborGId[i2] = NeighborGId[i1];
                NeighborGId[i1] = tmp01;
            }
        }
    }

    return count;

}

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

/**
* @brief	  Statistic of [ MV of corners/ MotionEdge ] into MinorBlock[256] and calculate its Type/isValid
* @param[out] CTAMinorBlock		Output Statistic Info
* @param[in]  Region			Input ROI of image(startx,starty,endx,endy)
* @param[in]  g_img_FrameDiff	Input MotionEdge
* @param[in]  harrisCornersPrev	Input HarrisCorner & MotionEstimation
* @param[in]  imgDimensionX		Input Image Size
* @return	  GroupNumIdx = BlockType = #MinorBlockType1 [!= #(MinorBlockType1 + MinorBlockType2)] 
*/
UINT16 Point_Group::MinorBlockUpdate(MinorBlock CTAMinorBlock[] , UINT16 CTAMinorBlock_EdgeQuantity[],
                                     const INT32 *Region, UCHAR *g_img_FrameDiff, HarrisCornerPos *harrisCornersPrev, HarrisCornerPos2 *harrisCorners2Prev,
								     INT32 imgDimensionX, INT32 imgDimensionY , const DOUBLE THD[3], int pointNumberPrev)
{
    if( 0 >= pointNumberPrev) return 0;
    
    ///0) Clear 
    memset(&CTAMinorBlock[0] , 0 , MAXCandidateNum * sizeof(CTAMinorBlock[0]) );
    
    ///1)Update EdgeQuantity at g_img_FrameDiff
//    UCHAR pixel;
    UINT16 BlockSizeV = ROISegBlockV ; //(Region[3] - Region[1]) / ROISegBlockNum_V;
    UINT16 BlockSizeH = ROISegBlockH ; //(Region[2] - Region[0]) / ROISegBlockNum_H;

    INT16 ShiftV = 4 ; 
    INT16 ShiftH = 4 ; 
    INT32  half_width = imgDimensionX >> 1;   


    FLOAT fBlockSizeV = 1.0/BlockSizeV;
    FLOAT fBlockSizeH = 1.0/BlockSizeH;

    /// Add the EdgeQuantity in proper CTAMinorBlock
    INT16 idxV,idxH;
    UINT32 vOffset;
#if 0  // remove to APEX
    for(int vert = Region[1] ; vert < Region[3] ; ++vert)
    {
        vOffset=vert * imgDimensionX;
        for(int hori = Region[0] ; hori < Region[2] ; ++hori)
        {
            pixel = g_img_FrameDiff[ hori + vOffset ];
            if ( pixel>=255 )
            {
                idxV = (vert - Region[1] )*fBlockSizeV;
                idxV = MIN( idxV , (ROISegBlockNum-1) );
                idxH = (hori - Region[0] )*fBlockSizeH;
                if( (idxH < 0) || (idxH >= ROISegBlockNum) )
                {
                    continue;
                }

                CTAMinorBlock[idxH +( idxV * ROISegBlockNum ) ].EdgeQuantity += 1;
            }
        }
    }
#endif
    ///2) Update CornerQuantity / CornerVector
    idxH = 0;
    idxV = 0;
    INT32 MaxPtNum = MIN(pointNumberPrev, MAXSizeHarrisCorner);
    for( UINT32 i1 = 0 ; i1 < MaxPtNum; ++i1)
    {
        if( TRUE == harrisCorners2Prev[i1].isValid )  // Motioned Harris Corners
        {
            if( (harrisCornersPrev[i1].x<Region[0]) || (harrisCornersPrev[i1].x>=Region[2]) )
				continue;//out o range at the other side

            idxV = ( ( harrisCornersPrev[i1].y - Region[1] )>>ShiftV ); 
            idxH = ( ( harrisCornersPrev[i1].x - Region[0] )>>ShiftH );
            
            vOffset = idxV * ROISegBlockNum_H;

            ++CTAMinorBlock[idxH +vOffset ].CornerQuantity;
            if( harrisCorners2Prev[i1].Diff_x >0 )
            {
                CTAMinorBlock[ idxH + vOffset ].CornerVector[0] += harrisCorners2Prev[i1].Diff_x ;
            }
            else
            {
                CTAMinorBlock[ idxH + vOffset ].CornerVector[2] += (-harrisCorners2Prev[i1].Diff_x) ;
            }
            if( harrisCorners2Prev[i1].Diff_y >0 )
            {
                CTAMinorBlock[ idxH + vOffset ].CornerVector[1] += harrisCorners2Prev[i1].Diff_y ;
            }
            else
            {
                CTAMinorBlock[ idxH + vOffset ].CornerVector[3] += (-harrisCorners2Prev[i1].Diff_y) ;
            }      
        }// end of TRUE == harrisCorners2Prev[i1].isValid
    } // end of i1 < MaxPtNum

    ///3) Update MinorBlock Type
    INT32 THD_MotionEdgeQ     = THD[0]; //10;
    INT32 THD_MotionVectorQ   = THD[1]; //3;
    FLOAT THD_MotionVectorDir = THD[2]; //(FLOAT)0.9; /// �̤j��Ӥ�V���>90%

    BOOL isEdgeQPass              = FALSE;
    BOOL isMotionQPass            = FALSE;
    BOOL isMotionDirPass          = FALSE;
    
    UINT16 GroupNumIdx = 1;
    UINT16 MVDir[4] ;
    UINT16 numerator;
    UINT16 denumerator;
    for ( int vert = 0 ; vert < ROISegBlockNum_V ; ++vert)
    {
        vOffset = vert * ROISegBlockNum_H ;
        for ( int hori = 0 ; hori < ROISegBlockNum_H ; ++hori)
        {
            isEdgeQPass   = ( CTAMinorBlock_EdgeQuantity[ hori +vOffset ]   >= THD_MotionEdgeQ  )? TRUE: FALSE;
            isMotionQPass = ( CTAMinorBlock[ hori +vOffset ].CornerQuantity >= THD_MotionVectorQ)? TRUE: FALSE;

            MVDir[0] = CTAMinorBlock[ hori +vOffset ].CornerVector[0];
            MVDir[1] = CTAMinorBlock[ hori +vOffset ].CornerVector[1];
            MVDir[2] = CTAMinorBlock[ hori +vOffset ].CornerVector[2];
            MVDir[3] = CTAMinorBlock[ hori +vOffset ].CornerVector[3];

            /// Sort the list decedent order
            UINT16 tmpSort;
            for ( UINT8 i1 = 0; i1 < 4 ; ++i1)
                sortDecendent[i1] = MVDir[i1];
            for ( UINT8 i1 = 0; i1 < 3 ; ++i1)
            {
                for ( UINT8 i2 = i1+1; i2 < 4 ; ++i2)
                {
                    if(sortDecendent[i1] < sortDecendent[i2]) // <: decendent >:ascendent
                    {
                        tmpSort = sortDecendent[i1];
                        sortDecendent[i1] = sortDecendent[i2];
                        sortDecendent[i2] = tmpSort;
                    }
                }
            }

            numerator   = sortDecendent[0] + sortDecendent[1];
            denumerator = MVDir[0] + MVDir[1] + MVDir[2] + MVDir[3];

            if( 0 == denumerator )
                isMotionDirPass = FALSE ;
            else
            {
                if(numerator  < (THD_MotionVectorDir * denumerator) )                  // �̤j��Ӥ�V���>90%
                {
                    isMotionDirPass = FALSE ;
                }
                else if( (Region[0] < (imgDimensionX>>1) ) && (MVDir[0] < MVDir[2]) )  // LEFT ROI & move left
                {
                    isMotionDirPass = FALSE ;
                }
                else if( (Region[0] > (imgDimensionX>>1) ) && (MVDir[0] > MVDir[2]) )  // RIGHT ROI & move RIGHT
                {
                    isMotionDirPass = FALSE ;
                }
                else
                {
                    isMotionDirPass = TRUE ;
                }
            }

            /// CTAMinorBlock [ Type / GroupNum ] Assign
            CTAMinorBlock[ hori +vOffset ].Type = (!isEdgeQPass)?MinorBlockTypeNoise:
                                                                    ( (!isMotionQPass)?  MinorBlockType2:
                                                                    ( (!isMotionDirPass)?MinorBlockTypeNoise:
                                                                    MinorBlockType1 ));
            

            if ( MinorBlockType1 == CTAMinorBlock[ hori +vOffset ].Type)
            {
                CTAMinorBlock[ hori +vOffset ].GroupID = GroupNumIdx;
                GroupNumIdx++;
            }
            if ( MinorBlockType2 == CTAMinorBlock[ hori +vOffset ].Type)
            {
            }
        } // end of hori
    } // end of vert
    return (GroupNumIdx-1); 
}


/**
* @brief	 MinorBlockLabeling Using Two-Pass labeling Alg (8-connected)
* @param[in] inCTAMinorBlock	Input MinorBlock with correct GroupID
* @param[in] inGroupIdx			Input CandidateRec at last Frame
* @return	 the Two-Pass Label MAX GroupNum ( != Total Group Num)
*/
UINT16 Point_Group::MinorBlockLabeling( MinorBlock inCTAMinorBlock[] )
{
    INT32 cntNum       = ROISegBlockNum_V * ROISegBlockNum_H;
    UCHAR *pArr        = LabelArr;
    MinorBlock *pBlock = inCTAMinorBlock;

    for ( INT32 cnt1 = 0 ; cnt1 < cntNum ; ++cnt1 )
    {
        *(pArr++) = ( (MinorBlockType1 == pBlock->Type)||(MinorBlockType2 == pBlock->Type) )? 255:0;
        ++pBlock;
    }

    UINT16 GroupNum = Labeling_SeedFilling( LabelArr , ROISegBlockNum_H, ROISegBlockNum_V);
    //UINT16 GroupNum = Labeling_TwoPass( LabelArr , ROISegBlockNum, ROISegBlockNum); /// warning: not implement bug-free.


    pArr   = LabelArr;
    pBlock = inCTAMinorBlock;
    for ( INT32 i1 = 0 ; i1 < cntNum ; ++i1)
    {
        pBlock->GroupID = *(pArr++);
        ++pBlock;
    }

    return GroupNum;
}

/**
* @brief	  CandidateRec Update info[GroupID/...] according to inCTAMinorBlock and the Previous CandidateRec
* @param[out] outCTACandidate		Output CandidateRec
* @param[in]  inCTAMinorBlock		Input MinorBlock with correct GroupID
* @param[in]  inCTACandidatePrev	Input CandidateRec at last Frame
* @param[in]  ImageWidth			Input Image Width for isLeft decision
* @param[in]  THD_CandidateUpdate	Input THD_MotionEdgeQ ,THD_MotionVectorQ, THD_MotionVectorInnerDir[�V�����ʶq>THD*�V�~���ʶq]
* @return	  Valid candidates count number after Group & Filter I
* @Note       Eric:group into white block
*/
UINT16 Point_Group::CandidateUpdate( CandidateRec outCTACandidate[] , MinorBlock inCTAMinorBlock[] , UINT16 inCTAMinorBlock_EdgeQuantity[], INT16 LabelCount , const INT32 *Region , INT32 ImageWidth ,const FLOAT THD_CandidateUpdate[3], CAN_Info *i_carState )
{

    INT16  ShiftV = 4;  //(INT32)((Region[3]-Region[1])/ROISegBlockNum_V);
    INT16  ShiftH = 4;  //(INT32)((Region[2]-Region[0])/ROISegBlockNum_H);

    /// ini
    for ( INT32 i1  = 0 ; i1 < LabelCount ;++i1)
    {
        outCTACandidate[ i1 ].startxi = 0x7FFF;
        outCTACandidate[ i1 ].startyi = 0x7FFF;
        outCTACandidate[ i1 ].GroupID = i1+1;
        outCTACandidate[ i1 ].isValid = Valid;
    }

    /// re-arrange
    INT16 label;
    INT16 Idx1;
    UINT16 vOffset;
    for ( INT32 vert = 0 ; vert < ROISegBlockNum_V ; ++vert)
        {
            vOffset = vert * ROISegBlockNum_H ;
        for ( INT32 hori = 0 ; hori < ROISegBlockNum_H ; ++hori)
            {
            label = inCTAMinorBlock[ hori + vOffset ].GroupID;
            if( 0 == label ) continue;

            Idx1 = label - 1;
            outCTACandidate[ Idx1 ].startxi = MIN( (outCTACandidate[ Idx1 ].startxi), (Region[0] + (hori<<ShiftH )));
            outCTACandidate[ Idx1 ].startyi = MIN( (outCTACandidate[ Idx1 ].startyi), (Region[1] + (vert<<ShiftV )));
            outCTACandidate[ Idx1 ].width   = MAX( (outCTACandidate[ Idx1 ].width),   (Region[0] + ((hori+1)<<ShiftH) - outCTACandidate[ Idx1 ].startxi ) );
            outCTACandidate[ Idx1 ].height  = MAX( (outCTACandidate[ Idx1 ].height),  (Region[1] + ((vert+1)<<ShiftV) - outCTACandidate[ Idx1 ].startyi ) );
                }
            }

    for ( INT32 i1 = 0 ; i1 < LabelCount ;++i1)
    {
        outCTACandidate[ i1 ].size =  outCTACandidate[ i1 ].width * outCTACandidate[ i1 ].height;

        INT32 startx = ((outCTACandidate[ i1 ].startxi - Region[0]) >> ShiftH) ;
        INT32 starty = ((outCTACandidate[ i1 ].startyi - Region[1]) >> ShiftV ) ;
        INT32 endx   = startx + (outCTACandidate[ i1 ].width >> ShiftH );
        INT32 endy   = starty + (outCTACandidate[ i1 ].height>> ShiftV );

        for ( INT32 vert = starty ; vert < endy ;++vert)
        {
            vOffset = vert * ROISegBlockNum_H ;
            for ( INT32 hori = startx ; hori < endx ; ++hori)
            {
                outCTACandidate[ i1 ].EdgeQuantity   += inCTAMinorBlock_EdgeQuantity[hori+vOffset];
                outCTACandidate[ i1 ].CornerQuantity += inCTAMinorBlock[hori+vOffset].CornerQuantity;
                for(INT8 i2 = 0; i2 < 4 ; ++i2)
                    outCTACandidate[ i1 ].CornerVector[i2] += inCTAMinorBlock[hori+vOffset].CornerVector[i2];
            }
        }
    }
    
//     return LabelCount; //if return here , it means no Filter I

    /// Do Filter I [Let isValid = False]
    INT32 CanCountFiltered         = LabelCount;
    INT32 THD_MotionEdgeQ          = THD_CandidateUpdate[0]; ///< MotionEdgeQ: number of motion edge number
    INT32 THD_MotionVectorQ        = THD_CandidateUpdate[1]; ///< MotionVectorQ: number of motion vector(no matter what direction)
    FLOAT THD_MotionVectorInnerDir = THD_CandidateUpdate[2]; ///< �V�����ʪ��ƶq> 5*�V�~���ʪ��ƶq
    DOUBLE mySpeed                 = i_carState->carSpeed;
    DOUBLE myAngle                 = i_carState->steerAngle;

    for ( INT32 i1 = 0 ; i1 < LabelCount ;++i1)
    {
        if( Invalid == outCTACandidate[ i1 ].isValid ) 
            continue;

        INT32 mySize    = outCTACandidate[ i1 ].size;
        INT32 myStartx  = outCTACandidate[ i1 ].startxi;
        INT32 myStarty  = outCTACandidate[ i1 ].startyi;
        INT32 myWidh    = outCTACandidate[ i1 ].width;
        INT32 myHeight  = outCTACandidate[ i1 ].height;
        #pragma region <==Filter_[EdgeQuantity , CornerQuantity]
        if( outCTACandidate[ i1 ].EdgeQuantity * THD_MotionVectorQ + outCTACandidate[ i1 ].CornerQuantity*THD_MotionEdgeQ <THD_MotionVectorQ*THD_MotionEdgeQ )
        {
            outCTACandidate[ i1 ].isValid = Invalid;
            --CanCountFiltered ;
            continue;
        }
        #pragma endregion

        #pragma region <==Filter_[CornerVector]====
        UCHAR innerIdx      = ( outCTACandidate[ i1 ].startxi < (ImageWidth>>1) )? (0):(2);
        UCHAR outterIdx     = ( outCTACandidate[ i1 ].startxi < (ImageWidth>>1) )? (2):(0);
        FLOAT IOVectorRatio = (FLOAT)outCTACandidate[ i1 ].CornerVector[innerIdx]/(MAX( (1.0/(THD_MotionVectorInnerDir)),(FLOAT)outCTACandidate[ i1 ].CornerVector[outterIdx]));
        
        if( IOVectorRatio < THD_MotionVectorInnerDir )
        {
			if(IOVectorRatio>0.4 &&outCTACandidate[ i1 ].EdgeQuantity>800&& outCTACandidate[ i1 ].CornerVector[innerIdx]>1)
			{
			}
			else
			{
				outCTACandidate[ i1 ].isValid = Invalid;
                --CanCountFiltered ;
				continue;
			}
        }
        if( IOVectorRatio < THD_MotionVectorInnerDir && (outCTACandidate[ i1 ].EdgeQuantity>1000)
            && (outCTACandidate[ i1 ].CornerVector[innerIdx] + outCTACandidate[ i1 ].CornerVector[outterIdx]) >10 )
        {
            outCTACandidate[ i1 ].isValid = Invalid;
            --CanCountFiltered ;
            continue;   
        }
        if( (outCTACandidate[ i1 ].CornerVector[innerIdx] + outCTACandidate[ i1 ].CornerVector[outterIdx]) < 4 ) // enough direction
        {
            outCTACandidate[ i1 ].isValid = Invalid;
            --CanCountFiltered;
            continue;
        }
        #pragma endregion

        #pragma region <==Filter_[ShiftingBackGround]====
        INT32 sizeAtBG;
        INT32 posyBG = (( (RegionL[3]-RegionL[1])/4 + RegionL[1]) );
        if( myStarty > posyBG){sizeAtBG = 0;}
        else
        {
            INT32 tmp2 = MIN( posyBG , (myStarty+myHeight) );
            sizeAtBG= (myWidh)*(tmp2 - myStarty);
        }
        if( ( (ABS(mySpeed*myAngle)) >= 280 ) && 
            (mySize>=4096) && 
            (sizeAtBG >= 3328) )
        {
            outCTACandidate[ i1 ].isValid = Invalid;
            --CanCountFiltered ;
            continue;
        }
        #pragma endregion
    }
    return CanCountFiltered; // ex.2 means 2 rectangles after Filtered
}
/**
* @brief		Equal-Labeled Array Finding (Verified)
* @param[out]	outMinGroupID	Output the minimal GroupID
* @param[in]	inPtr			Input Ordered-Array Ptr
* @param[in]	inHeight		Input Ordered-Array Height
* @param[in]	inWidth			Input Ordered-Array Width
* @param[in]	inGroupID		Input the searched ID
* @param[in]	Dbg_x			Input The position_x for DEBUG
* @param[in]	Dbg_y			Input The position_y for DEBUG
* @return		isSuccess
*/
static BOOL ArrFindGroup( UINT16 &outMinGroupID ,UINT16 *inPtr , UINT16 inHeight , UINT16 inWidth , UINT16 inGroupID, UINT16 &Dbg_x, UINT16 &Dbg_y)
{
    UINT16 idx01;
    for ( int i1 = 0 ; i1 < inHeight ;++i1)
    {
        if( 0 == inPtr[ i1*(inWidth)] ) //Find Fail
            return FALSE; 
        for (int i2 = 0 ; i2 < inWidth ;++i2)
        {
            idx01 = inPtr[ i2 + i1*(inWidth)];
            if( idx01 == inGroupID)
            {
                outMinGroupID = inPtr[ 0 + i1*(inWidth)];
                Dbg_x = i2;
                Dbg_y = i1;
                return TRUE; 
            }
            else if( idx01 == 0 ) //EOL
            {
                break;
            }
        }
    }
    return FALSE; // Find Fail.
}

/**
* @brief            Cluster(FilterI) the binary image(Light) into HeadLight information with detailed statistic.
* @param[in]        g_Binar_img     Binary Image 
* @param[out]       lampL           the lamp structure
* @param[out]       _L_Num          the lamp number
* @param[in]        RegionL         the ROI[4](startx,starty,endx,endy)
*/
void Point_Group::ClusterIntoHeadlight( UCHAR *Binar_img, INT32 imgDimensionX, INT32 imgDimensionY, HeadLight lampL[MAX_HeadLightNum] , HeadLight lampR[MAX_HeadLightNum] , INT16 &_L_Num , INT16 &_R_Num,const INT32 RegionL[4], const INT32 RegionR[4] )
{
    UINT16 labelCount = 0;

    ///1) Remove the Light out of boundary(RegionL/R) , May optimize in the future
    INT32 startx = RegionL[0];
    INT32 starty = RegionL[1];
    INT32 endx   = RegionR[2];
    INT32 endy   = RegionR[3];

    INT32 tmpIdx1;
    for ( INT32 vert = starty ; vert < endy ;++vert)
    {
        tmpIdx1 = vert * imgDimensionX;
        for ( INT32 hori = startx ; hori < endx ;++hori)
        {
            if(Binar_img[ hori + tmpIdx1 ] > 0 )
            {
                if( hori==0 || hori==(SrcImg_W-1) || vert == 0 || vert ==(SrcImg_H-1) ) 
                {
                    Binar_img[ hori + tmpIdx1 ] = 0;
                    continue;
                }
                if( hori>=RegionL[0] && hori<RegionL[2] && vert >=RegionL[1] && vert <RegionL[3]) // in RegionL
                {
                    continue;
                }
                if( hori>=RegionR[0] && hori<RegionR[2] && vert >=RegionR[1] && vert <RegionR[3]) // in RegionR
                {
                    continue;
                }
                Binar_img[ hori + tmpIdx1 ] = 0; // out of boundary
            }
        } //end of hori < imgDimensionX
    } //end of vert < imgDimensionY 

    ///2) connected pixel (ie.bwlabel)
    labelCount = Labeling_SeedFilling( Binar_img , imgDimensionX, imgDimensionY); ///<ex. 5 means 1-5 groupID

    ///3) Cluster to HeadLightL/R and Cal [width/height/perimeter/area/GroupID/isValid]
    ///   �@�k: ������Cluster to HeadLightL , �A�h����HeadLightR , ���kLabelNum�� = labelCount
    INT32  tmpIdx2, tmpLabel;
    UINT16 labelL = 0, labelR = 0;
    BOOL   isEdge;
    UINT16 labelxMin[MAX_HeadLightNum];
    UINT16 labelxMax[MAX_HeadLightNum] = {0};
    UINT16 labelyMin[MAX_HeadLightNum];
    UINT16 labelyMax[MAX_HeadLightNum] = {0};
    memset( labelxMin , SrcImg_W , MAX_HeadLightNum * sizeof(UINT16) );
    memset( labelyMin , SrcImg_W , MAX_HeadLightNum * sizeof(UINT16) );

    for ( INT32 vert = starty ; vert < endy ;++vert)
    {
        tmpIdx2 = vert * imgDimensionX;
        for ( INT32 hori = startx ; hori < endx ;++hori)
        {
            tmpLabel = Binar_img[ hori + tmpIdx2 ];
            if( tmpLabel > 0 )
            {
                /// lampL[ labelL-1 ] information update
                //3.1/2/3/4) startxi/startyi/width/height
                labelxMin[tmpLabel-1] = ( labelxMin[tmpLabel-1]>hori )?(hori):(labelxMin[tmpLabel-1]);
                labelxMax[tmpLabel-1] = ( labelxMax[tmpLabel-1]>hori )?(labelxMax[tmpLabel-1]):(hori);
                labelyMin[tmpLabel-1] = ( labelyMin[tmpLabel-1]>vert )?(vert):(labelyMin[tmpLabel-1]);
                labelyMax[tmpLabel-1] = ( labelyMax[tmpLabel-1]>vert )?(labelyMax[tmpLabel-1]):(vert);

                //3.5) perimeter
                isEdge = FALSE;
                if( (0 == Binar_img[ hori+0 + (vert-1) * imgDimensionX ])||
                    (0 == Binar_img[ hori+1 + (vert-1) * imgDimensionX ])||
                    (0 == Binar_img[ hori+1 + (vert+0) * imgDimensionX ])||
                    (0 == Binar_img[ hori+1 + (vert+1) * imgDimensionX ])||
                    (0 == Binar_img[ hori+0 + (vert+1) * imgDimensionX ])||
                    (0 == Binar_img[ hori-1 + (vert+1) * imgDimensionX ])||
                    (0 == Binar_img[ hori-1 + (vert+0) * imgDimensionX ])||
                    (0 == Binar_img[ hori-1 + (vert-1) * imgDimensionX ]))
                {
                    isEdge = TRUE;
                }
                lampL[ tmpLabel-1 ].perimeter = (isEdge == TRUE)? ( lampL[ tmpLabel-1 ].perimeter+1 ):(lampL[ tmpLabel-1 ].perimeter);
                //3.6) area;
                lampL[ tmpLabel-1 ].area ++;
                //3.7) isValid
                lampL[ tmpLabel-1 ].isValid = TRUE;
                //3.8) GroupID
                lampL[ tmpLabel-1 ].GroupID = tmpLabel; ///< it will be updated when transfer to lampR

            }
        } // end of hori < imgDimensionX
    }// end of vert < imgDimensionY

    ///3.9) Update startxi/startyi/width/height
    for (INT32 i1 = 0 ; i1 < labelCount ;++i1)
    {
        lampL[ i1 ].startxi = labelxMin[ i1 ];
        lampL[ i1 ].startyi = labelyMin[ i1 ];
        lampL[ i1 ].width   = labelxMax[ i1 ] - labelxMin[ i1 ]+1;
        lampL[ i1 ].height  = labelyMax[ i1 ] - labelyMin[ i1 ]+1;
    }

    ///3.10) separate into  HeadLight L / R
    for (INT32 i1 = 0 ; i1 < labelCount ;++i1)
    {
        if( FALSE == lampL[ i1 ].isValid ) continue;
        if( lampL[ i1 ].startxi > (imgDimensionX>>1) ) //SWAP
        {
            //3.10.1) copy from HeadLightL to HeadLightR
            lampR[i1].startxi   = lampL[ i1 ].startxi;
            lampR[i1].startyi   = lampL[ i1 ].startyi;
            lampR[i1].width     = lampL[ i1 ].width;
            lampR[i1].height    = lampL[ i1 ].height;
            lampR[i1].perimeter = lampL[ i1 ].perimeter;
            lampR[i1].area      = lampL[ i1 ].area;
            lampR[i1].GroupID   = lampL[ i1 ].GroupID; 
            lampR[i1].isValid   = TRUE;

            //3.10.2) disable Left
            lampL[i1].isValid = FALSE;
        }
    }
    _L_Num = _R_Num = labelCount; //ex. 5 means 0-4 isValid , ����i��1,3 �k��0,2,4
    return ;
}

/**
* @brief            CTA Patches at night mode
*/
void Point_Group::HeadlightPatch()
{
    ///TODO: patches
    return;
}

Point_Group* Point_Group::getInstance()
{
    if(!instance)
        instance = new Point_Group;
    return instance;
}

Point_Group::Point_Group(void)
{
}
Point_Group::~Point_Group(void)
{
}

void Point_Group::ResetInstance()
{
    delete instance; 
    instance = NULL;
}