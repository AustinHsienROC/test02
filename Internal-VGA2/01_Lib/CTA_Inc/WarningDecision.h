/*
===================================================================
Name		: WarningDecision.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: WarningDecision functions make the correct Warning Alarm and identify the warning candidate.
              1. Audio:     Warning Alarm
              2. Visual:    Candidate Rectangle.
==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/07/30	  | v0.0010  	|First setup functions        	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/
#ifndef _WarningDecision_H_
#define _WarningDecision_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Type.h"
#include "CTA_Def.h"

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
#define CTADog WarningDecision::getInstance()

//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   V A R I A B L E   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// E X T E R N A L   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************
class WarningDecision
{
private:
    static WarningDecision *instance;

public:
    WarningDecision(void);
    ~WarningDecision(void);

    static WarningDecision* getInstance();
    static void ResetInstance();

    void bawl( CandidateRec CTACandidateL[], CandidateRec CTACandidateR[], INT32 MaxCandNumL, INT32 MaxCandNumR, UCHAR &i_CTAAlarmState, UCHAR &i_CTAAlarmTrigger, INT16 i_CTACustROI[4] );
    void WarningDecisionPatch();

    void bowwow_Night(HeadLight lampL[], HeadLight lampR[], INT16 maxLampNumL, INT16 maxLampNumR, UCHAR &AlarmState);
};

#endif //end of _WarningDecision_H_