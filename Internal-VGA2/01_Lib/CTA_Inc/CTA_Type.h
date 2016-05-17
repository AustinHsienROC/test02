/*
===================================================================
Name		: CTA_Type.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function DEFINE the STRUCT and Prototype
                1. HarrisCornerPos
                2. MinorBlock
                3. CandidateRec
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/31	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef __CTA_TYPE_H_
#define __CTA_TYPE_H_

#pragma warning( disable : 4341 )//disable warning of [C4341] for "ENUM_INT8_T"

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

#define UCHAR	unsigned char
#define CHAR	signed char
#define UINT8	unsigned char
#define INT8	CHAR
#define UINT16	unsigned short
#define INT16	signed short
#define UINT32	unsigned int
#define INT32	signed int
#define UINT64	unsigned long
#define INT64	signed long
#define ULONG	unsigned long
#define LONG	signed long
#define FLOAT   float
#define DOUBLE  double
#define BOOL	bool

#ifdef _WIN32
#define ENUM_INT8_T : signed char
#else
#define ENUM_INT8_T
#endif

#ifndef _WIN32  // for K2 platform
  #define assert(a)   { while (0 == (a)) ; }  // to infinite loop
#endif
//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************
typedef enum ENUM_INT8_T{ Invalid = 0 , Startup=1, Valid = 2  } TypeIsValid; //CandidateRec
typedef enum ENUM_INT8_T{ Normal = 0 , SuppressAlarm = 1 , Forbidden=2 } TypeIsSuppress;//
typedef enum ENUM_INT8_T{ No_Tracking = 0 , UpdateMove = 1 , PureTracking=2 } TypeTrackingMode;//
typedef enum ENUM_INT8_T{ dir_Unknown = 0, dir_Static = 0, dir_UpRight=1, dir_UpLeft=2, dir_Left=3, dir_Right=4, dir_Up=5, dir_Down=7 } TypeMoveDir;//
typedef enum ENUM_INT8_T {noBound=0,lowerBound,upperBound,rightBound,leftBound} TypeBoundCondition;
//	2 5 1
// 3 0 4
//	3 7 4
/** 
*@struct HarrisCornerPos save the Harris corner detection() points 
*/
typedef struct _HarrisCornerPos
{
    UINT16  x;		    ///< PositionXi
    UINT16  y;		    ///< PositionYi
} HarrisCornerPos ;

typedef struct _HarrisCornerPos2
{
    CHAR    Diff_x;	    ///< corner point shift of x
    CHAR    Diff_y;	    ///< corner point shift of y
    BOOL    isValid;	///< is feature point has reasonable motion vector
    INT32   X;
    INT32   Y;
    INT32   Diff_X;
    INT32   Diff_Y;
    UINT16  moveCompensation_X; ///< cm
    UINT16  moveCompensation_Y; ///< cm
    UINT16  moveCompensation_x; ///< pixel
    UINT16  moveCompensation_y; ///< pixel

    //INT16 total_points;//valid corner points
    //_HarrisCornerPos operator=(_HarrisCornerPos &corner );
} HarrisCornerPos2 ;

/** 
*@struct MinorBlock save the Important Harris Corner and Edge info in the ROI
*/
typedef struct _MinorBlock
{
    //UINT16 EdgeQuantity;
    UINT16 CornerQuantity;
    UINT16 CornerVector[4]; ///< Right:0 ,Up:1 , Left:2, Down:3
    UCHAR Type;
    //BOOL isValid;			///< Not Used Yet
    UINT16 GroupID;
} MinorBlock;

/** 
*@struct CTACandidate save the Position Info combined from MinorBlocks
*/
typedef struct _CandidateRec
{
    //-------------Label- CandidateUpdate()-------------------------//
    INT16 startxi;           ///< Candidate start point at left-top
    INT16 startyi;           ///< Candidate start point at left-top
    INT16 width;             ///< Candidate width
    INT16 height;            ///< Candidate height
    INT32 size;

    UINT16 EdgeQuantity;     ///< Motion Edge Quantity
    UINT16 CornerQuantity;   ///< (Harris) motion corner amout
    UINT16 CornerVector[4];  ///< motion corner,divide into < Right:0 ,Up:1 , Left:2, Down:3> sum of four components
    FLOAT  CornerMove[2];    ///< Motion Corner AVG movement, < X:0 ,Y:1><X+to the right,Y+to the left>, calculate at each frame, no inheritance, no accumulation // used in edgeTracking()
    TypeIsValid   isValid;	 ///< CNN will change it
    INT16  GroupID;	         ///< starts from 1 
    TypeTrackingMode		isInReviveMode;

    //-------------Features- CandidateFeatureUpdate()-------------------------//
    INT32  RecFeatures[24];  ///< [SampleFeatureNum] specific feature array

    //========CandAssociation()========
    INT16 boundaryCount;		///< how many frames this candidate stays at the boundary
    TypeBoundCondition boundaryType;

    INT16 relatedCandLink;	    ///< CandAssociation() Related to Previous Candidate index and used in Speed Estimation
    BOOL  isRelatedCandValid;	// CandAssociation() Related to Previous Candidate index and used in Speed Estimation
    INT16 retrieveEdgeTimes;    ///< EdgeTrack() 

    //-------------Tracking()-------------------------//
    INT32 appearCounter;         ///< how many frames the obj appears
    INT32 staticCounter;         ///< how many continuous frames  the obj is static
    INT32 BottomLeftCorner [2];  ///< searched corner(X,Y) at bottom left of the obj rect
    INT32 BottomRightCorner[2];  ///< searched corner(X,Y) at bottom right of the obj rect
    INT16 appearPosxy[2];				//the first appear position of the "valid" candidate
    INT16 appearPosXY[2];				//the first appear position of the "valid" candidate, ori: center of car's rear
    INT16 ObjTrackPosxy [2];	     ///< pixel coordinate of tracking point
    INT16 ObjTrackMovexy[2];	     ///< movement of tracking point, calculate at each frame,  no inheritance, no accumulation
//    INT16 ObjTrackMoveXY[2];	     ///< movement of tracking point, calculate at each frame,  no inheritance, no accumulation
    UINT16 ObjTrackSAD;
    TypeMoveDir moveDir;//steady moving direction of the obj
//	BOOL passThrough;//incoming obj is passing through the subject car

    //-------------Estimation()-------------------------//
    INT16 countDown;			///< DelayFlag
    INT16 bondaryLR[2];         ///< pixel (Left and Right boundary from motion edge)
    INT16 PosXYKeepTime;        ///< ObjPosXY exist continuously appears in customer ROI frame( ex. 3 )

    INT32 ObjMovexy[2];		    ///< object movement (pixel), calculate at each frame,  no inheritance, no accumulation
    INT32 ObjPosxy[2];		    ///< pixel (Target Head Position at Img)
    BOOL  isObjPosxyValid;	    ///< 1) Image_xy found

    INT32 ObjMoveXY[2];	        ///< object movement in world coordinate, contains AVG , ObjMoveXY[1] has no value 
    INT32 ObjPosXY[2];		    ///< cm ( Target Head Position at World , contains AVG , Ori: back center of self-car in TopView[Right:+X , Up:+Y] ), ObjPosXY[1] has no value
    BOOL  isObjPosXYValid;	    ///< 2) Image_xy found and then XY is in LUT 

    FLOAT ObjSpeedXY[2];		///< cm/sec(*0.036=km/h) (Right:+ , Left:-) (Vertically Far:+ , Vertically Closed:-), contains AVG, ObjSpeedXY[1] has no value
    FLOAT ObjSpeedWeight;       ///< Speed reliability weight[0.1 - 0.9]
    BOOL  isObjSpeedXYValid;	///< 3) objSpeed found
    FLOAT CandTTC;			    ///< ms

    //-------------bawl()-------------------------//
    BOOL  isAlarm;			    ///< is alarm
} CandidateRec; 

/** 
*@struct HeadLight save the multi info at Night mode
*/
typedef struct _HeadLight
{
    //-----Cluster----------//
    UINT16 startxi;     ///< Lamp start point at left-top
    UINT16 startyi;     ///< Lamp start point at left-top
    UINT16 width;       ///< Lamp width
    UINT16 height;      ///< Lamp height
    UINT16 perimeter;   ///< Lamp perimeter
    UINT16 area;		///< Lamp pixel number
    BOOL   isValid;	    ///< is valid Lamp [ FALSE might set by FilterI,II,...]
    UINT16 GroupID;	    ///< the connected pixel with same GroupID

    //-----Features--------//
    FLOAT  Features[10];///< the specific features selected for machine learning

    //-----Estimation-----//
    UINT16  relatedLamp;	    ///< the association between 2 lamps at (T-1/T) frame
    BOOL    isRelatedLampValid;	///< the association between 2 lamps at (T-1/T) frame
    INT16   countDown;	        ///< "zero" means multiple frames meet (TTC < threshold)		
    FLOAT	ObjPosxy[2];		///< Lamp image coordinate project to the ground
    BOOL	isObjPosxyValid;	///< Lamp image coordinate project to the ground [isValid]
    FLOAT	ObjPosXY[2];		///< Lamp world coordinate(cm) project to the ground
    BOOL	isObjPosXYValid;	///< Lamp world coordinate(cm) project to the ground [isValid]
    FLOAT	ObjSpeedXY[2];		///< Lamp world speed(cm/sec)
    BOOL	isObjSpeedXYValid;	///< Lamp world speed(cm/sec)[isValid]
    FLOAT   TTC;                ///< Time(ms) to collision			
} HeadLight;


typedef struct 
{
    short start_col;    // (x) Should be multiple of 4 if using for detection region
    short start_row;    //(y) Should be multiple of 4 if using for detection region
    short width;        // Should be multiple of 4 if using for detection region 
    short height;       // Should be multiple of 4 if using for detection region

    int startPos;		//=start_row*blk_H_num+start_col
    int size;
} Rect_t;

/** 
    CAN_Info struct save the Car Info from Video File(.YUV) at this Frame
*/
typedef struct _CAN_Info{	
    int XLimit_world;
    int YLimit_world;
    int minRoadWidth;
    int maxRoadWidth;// depends on each country
    int topView_w;
    int topView_h;
    double frameInterval;//sec, time interval 30fps: 0.033; 15 fps: 0.066
    double steerAngle;      //************* degree of steer wheel [Left:Neg Right:Pos]
    double steerAngleLimit;
    double steerAngleAvg;//+-31 deg, resolution: 0.1
    double lastDriveAngle;
    double carSpeed;        //************* km/hr
    double carForwardDis;
    double carSpeedLowLimit;
    double offsetSpeedLimit;
    double accuThreshold;
    double CSetThreshold;
    int *indexArray;//for temp computation
    bool alarmed;
    bool useDriveAngle;
    double refRoadWidth;
    double lastRoadWidth;
    int warningInnerTh;
    int missLineCounter;
    int warningReleaseDis;
    int frameCounter;
    bool crossComplete;
    bool iconAllow;
    bool nightMode;
    bool followLamp;
    int alarmIdx;
    int crossStartDirection;
    //----------------CTA----------------------//
    unsigned int time_stamp;    
    UINT32 timeInterval;        ///< (ms) time difference between 2 frame
}CAN_Info;// CAN_Info; 

//-------------------Weather-------------------------//
typedef enum ENUM_INT8_T 
{
    Day = 1 , 
    Night 
}weatherType_t; 
typedef struct _CTAStatusExport
{
    weatherType_t weatherInfo;

}CTAStatusExport;

typedef struct _TARGET_NUM
{
    INT16 FeaturePtNum;        // number of harris corner point
    INT16 FeaturePtNumPrev;    // number of harris corner point
    INT16 _L_MaxCandNum;       ///< max used obj index at left side in the current frame
    INT16 _R_MaxCandNum;       ///< max used obj index at right side in the current frame
    INT16 _L_CandNum;          ///< ???
    INT16 _R_CandNum;          ///< ???
    INT16 _L_MaxCandNumPrev;   ///< max used obj index at left side in the previous frame
    INT16 _R_MaxCandNumPrev;   ///< max used obj index at right side in the previous frame
    INT16 CrossoverCountDown;
}TARGET_NUM;

typedef struct _CTATimeManagement
{
    INT32          FrameNum;            ///<
    FLOAT          fps;                 ///< (Frame Numbers Per Second)
    FLOAT          fpsPrev;                  ///< (Frame Numbers Per Second)
#ifdef _WIN32  
    LARGE_INTEGER  TicCounter[ 30 ];    ///< function process interval (sec)
    LARGE_INTEGER  cpuFreq;
#endif    
}CTATimeManagement;


#endif //