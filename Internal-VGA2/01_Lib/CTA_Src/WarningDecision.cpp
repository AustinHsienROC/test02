#ifdef _WIN32
  #include "stdafx.h"
#else // K2 platform
  #include "GlobalTypes.h"
#endif
#include "WarningDecision.h"

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************
WarningDecision* WarningDecision::instance = NULL;

//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************

/**
* @brief	 Warning Decision
* @n         Alarm Rising trigger.
* @n         12 frames hold on
* @param[in]  CTACandidateL		Input all CandidateL at current frame
* @param[in]  MaxCandNumL		CandidateL numbers
* @param[out] isAlarm		    The result([0:No alarm / 1:Right Alrm  /2:LeftAlarm / 3: Both side alarm])
*/
void WarningDecision::bawl( CandidateRec CTACandidateL[], CandidateRec CTACandidateR[], INT32 MaxCandNumL, INT32 MaxCandNumR, UCHAR &i_CTAAlarmState, UCHAR &i_CTAAlarmTrigger, INT16 i_CTACustROI[4] )
{
    const  INT16 TRIG_LATENCY = 12;
    static INT16 latencyCntL  = 0;
    static INT16 latencyCntR  = 0;
    latencyCntL = MAX(0, latencyCntL - 1);
    latencyCntR = MAX(0, latencyCntR - 1);

    BOOL isLAlarmPrev = (0x00 == (i_CTAAlarmState & 0x02))?FALSE:TRUE;
    BOOL isRAlarmPrev = (0x00 == (i_CTAAlarmState & 0x01))?FALSE:TRUE;

    i_CTAAlarmState = 0x00;
    for(INT16 i1 = 0 ; i1 < MaxCandNumL ; ++i1) // Left
    {
        if( (Valid == CTACandidateL[i1].isValid) 
            && (TRUE == CTACandidateL[i1].isObjPosXYValid)
            && (CTACandidateL[i1].ObjPosXY[0] > -i_CTACustROI[1])
            && (CTACandidateL[i1].ObjPosXY[0] < -i_CTACustROI[0])
            && (CTACandidateL[i1].PosXYKeepTime >=2 ))
        {
            CTACandidateL[i1].isAlarm = TRUE;
            i_CTAAlarmState |= 0x02;
        }
        else
        {
            CTACandidateL[i1].isAlarm = FALSE;
        }
    }// end of i1 < MaxCandNumL
    for(INT16 i1 = 0 ; i1 < MaxCandNumR ; ++i1)
    {
        if( (Valid == CTACandidateR[i1].isValid)  
            && (TRUE == CTACandidateR[i1].isObjPosXYValid)
            && (CTACandidateR[i1].ObjPosXY[0] < i_CTACustROI[1])
            && (CTACandidateR[i1].ObjPosXY[0] > i_CTACustROI[0])
            && (CTACandidateR[i1].PosXYKeepTime >=2 ))
        {
            CTACandidateR[i1].isAlarm = TRUE;
            i_CTAAlarmState |= 0x01;
        }
        else
        {
            CTACandidateR[i1].isAlarm = FALSE;
        }
    }// end of i1 < MaxCandNumR

    /// i_CTAAlarmTrigger Cal
    i_CTAAlarmTrigger = 0x00;
    if( (FALSE == isLAlarmPrev) 
        &&(i_CTAAlarmState & 0x02)
        &&(0 == latencyCntL))
    {
        i_CTAAlarmTrigger |= 0x02 ;
        latencyCntL = TRIG_LATENCY;
    }
    if( (FALSE == isRAlarmPrev) 
        &&(i_CTAAlarmState & 0x01)
        &&(0 == latencyCntR))
    {
        i_CTAAlarmTrigger |= 0x01 ;
        latencyCntR = TRIG_LATENCY;
    }
    /*note: Old TTC version
    /// Voice Alarm
    for ( INT16 i1 = 0 ; i1 < MaxCandNumL ; ++i1) 
    {
        if( CTACandidateL[i1].isValid!=Valid ) continue;
        if( 0 >= CTACandidateL[i1].countDown )
        {
            i_CTAAlarmState |= 0x02;
            break;
        }
    }

    for ( INT16 i1 = 0 ; i1 < MaxCandNumR ; ++i1) 
    {
        if( CTACandidateR[i1].isValid!=Valid ) continue;
        if( 0 >= CTACandidateR[i1].countDown )
        {
            i_CTAAlarmState |= 0x01;
            break;
        }
    }
    */
}
/**
* @brief	 [NightMode]Warning Decision
* @param[in] lamp		Input lamp at current frame
* @return	 isWarninig Event Trigger
*/
void WarningDecision::bowwow_Night( HeadLight lampL[], HeadLight lampR[], INT16 maxLampNumL, INT16 maxLampNumR, UCHAR &AlarmState )
{
    AlarmState = 0x00;
    /// Voice Alarm
    for ( INT16 i1 = 0 ; i1 < maxLampNumL ; ++i1) // CTACandidate Total Num
    {
        if( FALSE == lampL[i1].isValid ) continue;
        if( 0 >= lampL[i1].countDown )
        {
            AlarmState |= 0x02;
            break;
        }
    }
    for ( INT16 i1 = 0 ; i1 < maxLampNumR ; ++i1) 
    {
        if( FALSE == lampR[i1].isValid ) continue;
        if( 0 >= lampR[i1].countDown )
        {
            AlarmState |= 0x01;
            break;
        }
    }

}
void WarningDecision::WarningDecisionPatch()
{
    return;
}
WarningDecision* WarningDecision::getInstance()
{
    if(!instance)
        instance = new WarningDecision;
    return instance;
}

WarningDecision::WarningDecision(void)
{
}


WarningDecision::~WarningDecision(void)
{
}

void WarningDecision::ResetInstance()
{
    delete instance; 
    instance = NULL;
}
