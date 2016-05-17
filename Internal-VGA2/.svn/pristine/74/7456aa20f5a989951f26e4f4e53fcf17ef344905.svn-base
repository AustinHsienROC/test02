#ifdef _WIN32
#include "stdafx.h"

#include "ML_ImgFeaturesDescriptors.h"
#include <math.h>       /* exp */
#include <string.h>

#include "Filter2D.h"

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
ImgFeaturesDescriptors* ImgFeaturesDescriptors::instance = NULL;

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************


typedef enum ENUM_INT8_T{ LBP =1 ,UniformLBP} LBPAlgType;
/**
* @brief     Extract the LBP at the specific windows , 81 means point = 8, radius =1 
* @param[in] img	        Source Image
* @param[in] startx	        Windows start position
* @param[in] windowWidth	Windows width
* @param[out]outLBP	        the result LBP features[LBP:size = 256 , UniformLBP:size = 59]
* @return    NULL
* @ref          http://en.wikipedia.org/wiki/Local_binary_patterns / http://cs229.stanford.edu/proj2008/Jo-FaceDetectionUsingLBPfeatures.pdf / http://blog.sina.com.cn/s/blog_4bdbec750101ekuh.html / http://baike.baidu.com/view/1099358.htm
*/
void ImgFeaturesDescriptors::GetFeature_LBP81( UCHAR *img, INT32 ImgWidth , INT32 ImgHeight , INT32 startx,INT32 starty,INT32 windowWidth, INT32 windowHeight,INT32 *outLBP )
{
    
    LBPAlgType choice = LBP; //{ LBP =1 ,UniformLBP}

    if(LBP == choice)
    {
        UCHAR  LBPValue;
        INT32  pixelValue;
        /// initial
        memset( outLBP ,0, 256);

        /// cal LBP histogram
        for(int vert = starty ; vert<=(starty+windowHeight) ; ++vert)
        {
            for(int hori = startx ;hori<=(startx+windowWidth) ; ++hori)
            {
                LBPValue  =  0 ;
                pixelValue=  img[ (hori+0) + (vert+0)*ImgWidth ];
                LBPValue += ( img[ (hori-1) + (vert-1)*ImgWidth ]>pixelValue )?(1 ):(0);
                LBPValue += ( img[ (hori+0) + (vert-1)*ImgWidth ]>pixelValue )?(2 ):(0);
                LBPValue += ( img[ (hori+1) + (vert-1)*ImgWidth ]>pixelValue )?(4 ):(0);
                LBPValue += ( img[ (hori+1) + (vert+0)*ImgWidth ]>pixelValue )?(8 ):(0);
                LBPValue += ( img[ (hori+1) + (vert+1)*ImgWidth ]>pixelValue )?(16):(0);
                LBPValue += ( img[ (hori+0) + (vert+1)*ImgWidth ]>pixelValue )?(32):(0);
                LBPValue += ( img[ (hori-1) + (vert+1)*ImgWidth ]>pixelValue )?(64):(0);
                LBPValue += ( img[ (hori-1) + (vert+1)*ImgWidth ]>pixelValue )?(128):(0);
                
                outLBP[ LBPValue ] += 1 ;
            }// end of [hori<=(startx+windowWidth)]
        }// end of [vert<=(starty+windowHeight)]
        
    }
    else if( UniformLBP == choice)
    {
        /// initial
        UCHAR  LBPValue;
        INT32  pixelValue;
        UCHAR UnoformLBPList[] ={ 0,1 ,2, 3,4, 6,7,8,  12,14, 15,  16,24, 28, 30,31, 32, 48,  56, 60, 62, 63,64, 96,112,120,  124,  126,  127,  128, 129, 131,  135,  143,  159,  191,  192,  193,  195,  199, 207, 223,  224,  225,  227,  231,  239,  240,  241,  243, 247, 248,  249,  251,  252,  253,  254,  255};
        memset( outLBP ,0, sizeof(UnoformLBPList)+1 ); //59

        /// cal LBP histogram
        for(int vert = starty ; vert<=(starty+windowHeight) ; ++vert)
        {
            for(int hori = startx ;hori<=(startx+windowWidth) ; ++hori)
            {
                LBPValue  = 0;
                pixelValue=   img[ (hori+0) + (vert+0)*ImgWidth ];
                LBPValue += ( img[ (hori-1) + (vert-1)*ImgWidth ]>pixelValue )?(1 ):(0);
                LBPValue += ( img[ (hori+0) + (vert-1)*ImgWidth ]>pixelValue )?(2 ):(0);
                LBPValue += ( img[ (hori+1) + (vert-1)*ImgWidth ]>pixelValue )?(4 ):(0);
                LBPValue += ( img[ (hori+1) + (vert+0)*ImgWidth ]>pixelValue )?(8 ):(0);
                LBPValue += ( img[ (hori+1) + (vert+1)*ImgWidth ]>pixelValue )?(16):(0);
                LBPValue += ( img[ (hori+0) + (vert+1)*ImgWidth ]>pixelValue )?(32):(0);
                LBPValue += ( img[ (hori-1) + (vert+1)*ImgWidth ]>pixelValue )?(64):(0);
                LBPValue += ( img[ (hori-1) + (vert+1)*ImgWidth ]>pixelValue )?(128):(0);

                bool  isFind = false;
                int   idx1;
                for ( idx1 = 0; idx1 <sizeof(UnoformLBPList); ++idx1 )
                {
                    if( LBPValue == UnoformLBPList[ idx1 ] )
                    {
                        isFind = true;
                        break;
                    }
                }
                outLBP[ (isFind)?(UnoformLBPList[idx1]):(sizeof(UnoformLBPList)) ] += 1 ; // isFind in the List , if not outLBP[58]++;
            }// end of [hori<=(startx+windowWidth)]
        }// end of [vert<=(starty+windowHeight)]

    }
}

/**
* @brief     Score of Symmetry at the window
* @param[in] img	        Source Image
* @param[in] startx	        Windows start position
* @param[in] windowWidth	Windows width
* @return    Score of Symmetry[ -1(Bad) ~ 1(Good) ]
* @ref       Leeuwen(2001)
*/
FLOAT ImgFeaturesDescriptors::GetFeature_Symmetry( UCHAR *SrcImg, INT32 ImgWidth , INT32 ImgHeight , INT32 centerx,INT32 starty,INT32 windowHalfWidth, INT32 windowHeight )
{
    /* test drive
        centerx         = 100;
        windowHalfWidth = 5;
        starty          = 50;
        windowHeight    = 60;
        表示 50~60間 每一行計算symmetry平均分數。而每一行symmetry為 (100+1~5) (100-1~5)的範圍計算even/odd function
    */
    /// initialize
    int halfWindow = windowHalfWidth;
    int x          = 0 ;
    int retNum     = 0;
    float *G_en    = new float[ halfWindow*2 ]; 
    float *G_odd   = new float[ halfWindow*2 ]; 

    for(int vert = starty ; vert < (starty+windowHeight) ; ++vert)
    {
        int hori = centerx;

        /// cal mean value
        float mean   = 0;
        for(int idx1 = 1 ;idx1<= halfWindow ; ++idx1)
        {
            mean += SrcImg[ idx1+(hori) + vert*ImgWidth];
            mean += SrcImg[-idx1+(hori) + vert*ImgWidth];
        }
        mean /= (halfWindow*2);

        /// cal Gen , Godd 
        for(int idx1 = 1 ;idx1 <= halfWindow ; ++idx1)
        {
            G_en [ idx1 ]  = (float)(SrcImg[ idx1+(hori) + vert*ImgWidth] + SrcImg[ -idx1+(hori) + vert*ImgWidth])/2;
            G_en [ idx1 ] -= mean;
            G_odd[ idx1 ]  = (float)(SrcImg[ idx1+(hori) + vert*ImgWidth] - SrcImg[ -idx1+(hori) + vert*ImgWidth])/2;
        }

        /// cal Score at this row
        float score1 = 0 , score2 = 0;
        for(int idx1 = 1 ;idx1<= halfWindow ; ++idx1)
        {
            score1 += G_en [ idx1 ] * G_en [ idx1 ];
            score2 += G_odd[ idx1 ] * G_odd[ idx1 ];
        }
        retNum += (score1-score2)/(score1+score2);
    }
    retNum /= windowHeight ;

    delete [] G_en ; G_en  = NULL;
    delete [] G_odd; G_odd = NULL;
    return retNum;
}

/**
* @brief     find in the 8x8 pixels 
* @param[in] img	     Source Image with fixed size(48*64)
* @param[in] sobelMag	 sobel magnitude Image with fixed size(48*64)
* @param[in] startX      start point(left-Top)
* @param[out]tmpCellHist 9 bins histogram
*/
static void cellhisto(UCHAR *img , UCHAR *sobelMag , CHAR *sobelTheta, INT32 tmpCellHist[9],INT32 startX,INT32 startY)
{
    memset(tmpCellHist, 0, sizeof(INT32)*9);
    int tmp1;
    for (int vertC = 0 ; vertC < 8 ; ++vertC)
    {
        for ( int horiC = 0 ; horiC < 8 ; ++horiC)
        {
            tmp1 = ((INT16)sobelTheta[ (startX+ horiC)+(startY+vertC)*64 ]+90)/20 ;   ///< 0-8 9bins

            tmpCellHist[MIN(tmp1,8)] += sobelMag[ (startX+ horiC)+(startY+vertC)*64 ];///< cell histo weight
        }
    }
}
/**
* @brief     Get HOG descriptors of the specific image size (not verified completely)
* @param[in] img	        Source Image with fixed size(48*64)
* @param[out] HOGDescriptor	HOG 432 features 
* @ref       http://blog.sina.com.cn/s/blog_60e6e3d50101bkpn.html / http://blog.sina.com.cn/s/blog_60e6e3d50101bier.html / http://blog.csdn.net/to_xidianhph_youth/article/details/10580495
*/
void ImgFeaturesDescriptors::HOG_48_64( UCHAR *img , FLOAT HOGDescriptor[432] )
{
    /************************************************************************/
    /* img size  = 48 * 64
       cell size = 8 * 8 pixels
       block size = 2*2 cells
       non-overlapped
       L1-norm normalize
       9 bins of Sobel theta(-pi/2~pi/2)
       9*12*4 = 432 feature array.
    */
    /************************************************************************/
    ///1) initialize
    UCHAR sobelMag    [48*64];
    CHAR  sobelTheta  [48*64];
    INT32 tmpCellHist [9];    ///< cell(8x8pixels)   histogram
    INT32 tmpBlockHist[9*4]; ///< 2x2 cells histogram
    FLOAT tmpNormBlockHist[9*4]; ///< 2x2 cells histogram with normalized

    memset(HOGDescriptor,0, sizeof(FLOAT)*432);

    ///2) Histogram equalize
    ImgFilter->HistogramEq(img ,64 , 48);

    ///3) Sobel Mag / theta
    ImgFilter->SobelEdge(sobelMag , sobelTheta , img , 64 , 48);

    ///4) HOG features
    int tmp1;
    int posX , posY;
    int norm1;
    for ( int vertB = 0 ; vertB < 3 ; ++vertB) // block
    {
        for ( int horiB = 0 ; horiB < 4 ; ++horiB)
        {
            tmp1 = vertB * 4 + horiB;         ///< 0-11
            
            ///4.1) cal 2x2 cells histogram
            for ( int vertC = 0 ; vertC < 2 ; ++vertC)// cell
            {
                for ( int horiC = 0 ; horiC < 2 ; ++horiC)
                {
                    posX = (horiB<<4) + (horiC<<3);
                    posY = (vertB<<4) + (vertC<<3);
                    cellhisto(img , sobelMag , sobelTheta, tmpCellHist ,posX,posY); ///< tmpCellHist
                    memcpy( tmpBlockHist+((vertC<<1)+ horiC)*9, tmpCellHist,sizeof(INT32)*9); //copy 9*4 histogram
                }
            } //end of [vertC < 1]

            ///4.2) Block normalized L1-norm
            norm1 = 0;
            for ( int i1 = 0  ; i1<36 ; ++i1)
                norm1 += tmpBlockHist[i1];
            int ada = norm1;
            for ( int i1 = 0 ; i1<36 ; ++i1)
            {
                tmpNormBlockHist[i1] = (FLOAT)tmpBlockHist[i1]/((FLOAT)norm1+0.001);
            }
            ///4.3) descriptors gen
            memcpy( HOGDescriptor+tmp1*36 ,tmpNormBlockHist ,sizeof(FLOAT)*36 );
        }
    }// end of [vertB < 3];
    return;
}
ImgFeaturesDescriptors* ImgFeaturesDescriptors::getInstance()
{
    if(!instance)
        instance = new ImgFeaturesDescriptors;
    return instance;
}
ImgFeaturesDescriptors::ImgFeaturesDescriptors(void)
{
}
ImgFeaturesDescriptors::~ImgFeaturesDescriptors(void)
{
}

void ImgFeaturesDescriptors::ResetInstance()
{
    delete instance; 
    instance = NULL;
}

#endif // #ifdef _WIN32
