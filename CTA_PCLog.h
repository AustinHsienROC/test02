/*
===================================================================
Name		: CTA_PCLog.h
Author		: Austin
Version		: v0.0010
Copyright	: Copyright (C) 2014 By oToBrite Electronics Corp.
Description	: This function implement CTA XML Log Application

==================================================================
History
--------------+-------------+-----------------------------------------+------------
DATE		  |VERSION      |DESCRIPTION  							  | Author
--------------+-------------+-----------------------------------------+------------
2014/09/05	  | v0.0010  	|Implement Functions.          	    	  | Austin
--------------+-------------+-----------------------------------------+------------
*/

#ifndef _CTA_PCLog_H_
#define _CTA_PCLog_H_

//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "CTA_Type.h"
#include "CTA_Def.h"

#ifdef _WIN32
    #include <string>
    #include "tinyxml.h"
    #include <time.h>
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
    #define MAX_ClipNum  4000    ///<Log��clipNum������
    #define Max_FrameLen 6000    ///<����ɮ�Frame length ������ 4000= 5*60(sec)*20(fps)

    #define CTALog CTA_PCLog::getInstance()

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************


class CTA_PCLog
{
private:
    static CTA_PCLog *instance;
    std::string FileName;

public:
    void ExportCTALog_ALARM(CONST char *ClipName , BOOL *EventResultL ,BOOL *EventResultR, UINT32 EventLen );

    CTA_PCLog(void);
    ~CTA_PCLog(void);
    static CTA_PCLog* getInstance();
    static void ResetInstance();
};
#endif
#endif