/*
===================================================================
Name		: CTA_CustomData.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: DEFINE the STRUCT and Prototype of identical car spec 
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/09	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef __CTA_CUSTOMDATA_H
#define __CTA_CUSTOMDATA_H

#include "CustomData.h"

typedef struct
{
    unsigned int RearBumper_pixel;				  //(pixel)保桿在TopView上高度   [CUT_REAR_BUMPER]
    unsigned int CarWidth_cm;					  //(cm)車寬					 [CAR_WIDTH_CM]
    unsigned int WheelBase_cm;					  //(cm)軸距		 	 		 [WHEEL_BASE_CM]
    unsigned int RearTireToBumper_cm;		      //(cm)後輪軸到車尾距離	  	 [REAR_TIRE_TO_BUMPER_CM]
    double SteerOverFrontTireRatio;				  //(ratio), 角傳動比			 [STEER_OVER_FRONT_TIRE]
}SimpleCarSpec ;

#endif
