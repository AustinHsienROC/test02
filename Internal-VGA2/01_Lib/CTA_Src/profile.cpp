/******************************************************************************
 *  (C) Copyright oToBrite Electronic Inc.. 2015 All right reserved.
 *
 *  Confidential Information
 *
 *  All parts of the oToBrite Program Source are protected by copyright law
 *  and all rights are reserved.
 *  This documentation may not, in whole or in part, be copied, photocopied,
 *  reproduced, translated, or reduced to any electronic medium or machine
 *  readable form without prior consent, in writing, from oToBrite.
 ******************************************************************************/
 
 #include "profile.h"
 
#if (_PROFILE_MOD_ || _PROFILE_CTA_ || _PROFILE_MVIEW_)
#include <stdio.h>        // for printf()
#include <string.h>       // for memset()
#include "Utilities.h"

#if _PROFILE_MOD_    
    char *strMOD_Function[] = {   
        "APEX_FD1_Phase1", 
        "APEX_FD1_Phase2",
        "APEX_FD5",
        "APEX_FD6_Phase1",
        "APEX_FD6_Phase2",
        "APEX_FD4_Phase1",
        "APEX_FD4_Phase2",
        "APEX_FD8",
        "APEX_HarrisCorner",
        "APEX_SuppressNonMax",        
        "MOD_FD7",
        "MOD_FD9",
        "MOD_ME2",
        "MOD_ME4ME5",
        "MOD_Tracking", 
        "MOD_UpdateRegionMap",
        "K2_GetBoundaryPoint",
        "MOD_GenerateReport",
    };
    
    #define MOD_FUN_NUM     (sizeof(strMOD_Function)/sizeof(char*))
    
    static uint32 tick_mod[MOD_FUN_NUM], tm_mod[MOD_FUN_NUM];

    void Profile_Reset_MOD();
    void Profile_Title_MOD();
    void Profile_Result_MOD();
#else
    #define Profile_Reset_MOD()   // dummy function
    #define Profile_Title_MOD()   // dummy function
    #define Profile_Result_MOD()  // dummy function
#endif // END #if _PROFILE_MOD_

#if _PROFILE_CTA_            
    char *strCTA_Function[] = {    
        "APEX_CTA_DayProcess",
        "APEX_CTA_HarrisCorner",
        "APEX_CTA_CntEdgeQty",
        "APEX_CTA_CntSobelQty",        
        "SetPara",
        "CornersMotionEstimation",
        "PointTrackPatch",
        "MinorBlockUpdate",
        "MinorBlockLabeling",
        "CandidateUpdate",
        "PointGroupPatch",
        "SetStartupFlag",
        "CandAssociation",
        "EdgeTracking",
        "UpdateBoundaryInfo",
        "EliminateCrossover",
        "GetCandInfo",
        "DecideMoveDir",
        "MergeOverlappedCand",
        "UpdateFlagTHD",
        "CandidateFeatureUpdate_1", 
        "Predict_1",
        "bawl",
        "SaveCornerPos",
        "Update_Data_Before_Next_Frame",
    };

    #define CTA_FUN_NUM     (sizeof(strCTA_Function)/sizeof(char*))
    
    static uint32 tick_cta[CTA_FUN_NUM], tm_cta[CTA_FUN_NUM];
        
    void Profile_Reset_CTA();
    void Profile_Title_CTA();
    void Profile_Result_CTA();
#else
    #define Profile_Reset_CTA()   // dummy function
    #define Profile_Title_CTA()   // dummy function
    #define Profile_Result_CTA()  // dummy function
#endif // END #if _PROFILE_CTA_    

#if _PROFILE_MVIEW_    
    char *strMultiView_Function[] = {    
            "TopView_Create",
            "_CornerView_Create",            
            };

    #define MVIEW_FUN_NUM     (sizeof(strMultiView_Function)/sizeof(strMultiView_Function[0]))
    
    static uint32 tick_mview[MVIEW_FUN_NUM], tm_mview[MVIEW_FUN_NUM];
        
    void Profile_Reset_MultiView();
    void Profile_Title_MultiView();
    void Profile_Result_MultiView();
#else
    #define Profile_Reset_MultiView()   // dummy function
    #define Profile_Title_MultiView()   // dummy function
    #define Profile_Result_MultiView()  // dummy function
#endif // END #if _PROFILE_MVIEW_

static int profile_idx = 0;     // frame index for profile
//////////////////////////////////////////////////////////////////////////
void Profile_Reset() 
{
    Profile_Reset_MOD();
    Profile_Reset_CTA();
    Profile_Reset_MultiView();
}

void Profile_PrintReslult()
{
    if (0 == profile_idx) {
        printf("Profile record\n");
        printf("frame_idx, ");
        Profile_Title_MOD();
        Profile_Title_CTA();
        Profile_Title_MultiView();
        printf("\n");
    }
    
    printf("%d, ", profile_idx);
    Profile_Result_MOD();
    Profile_Result_CTA();
    Profile_Result_MultiView();
    printf("\n");
    
    profile_idx++;                    
}
#endif // END #if (_PROFILE_MOD_ || _PROFILE_CTA_ || _PROFILE_MVIEW_)

//////////////////////////////////////////////////////////////////////////  
#if _PROFILE_MOD_

void Profile_MOD_Start(int idx)
{
    tick_mod[idx] = GetSystemTimeUS();
}

void Profile_MOD_End(int idx)
{
    tm_mod[idx] = GetTimeLength(tick_mod[idx], GetSystemTimeUS());
}

void Profile_Reset_MOD()
{
    memset(tick_mod, 0, sizeof(tick_mod));
    memset(tm_mod, 0, sizeof(tm_mod));
}

void Profile_Title_MOD()
{
    int i;
    
    for (i=0; i<MOD_FUN_NUM; i++) {
         printf("%s, ", strMOD_Function[i]);
    }
}

void Profile_Result_MOD()
{
    int i;    
    
    for (i=0; i<MOD_FUN_NUM; i++) {
         printf("%3u.%03u, ", tm_mod[i]/1000, tm_mod[i]%1000);
    }
}

#endif // END #if _PROFILE_MOD_

//////////////////////////////////////////////////////////////////////////  
#if _PROFILE_CTA_

void Profile_CTA_Start(int idx)
{
    tick_cta[idx] = GetSystemTimeUS();
}

void Profile_CTA_End(int idx)
{
    tm_cta[idx] = GetTimeLength(tick_cta[idx], GetSystemTimeUS());
}

void Profile_Reset_CTA()
{
    memset(tick_cta, 0, sizeof(tick_cta));
    memset(tm_cta, 0, sizeof(tm_cta));
}

void Profile_Title_CTA()
{
    int i;
    
    for (i=0; i<CTA_FUN_NUM; i++) {
         printf("%s, ", strCTA_Function[i]);
    }
}

void Profile_Result_CTA()
{
    int i;    
    
    for (i=0; i<CTA_FUN_NUM; i++) {
         printf("%3u.%03u, ", tm_cta[i]/1000, tm_cta[i]%1000);
    }
}

#endif // END #if _PROFILE_CTA_

//////////////////////////////////////////////////////////////////////////
#if _PROFILE_MVIEW_

void Profile_MultiView_Start(int idx)
{
    tick_mview[idx] = GetSystemTimeUS();
}

void Profile_MultiView_End(int idx)
{
    tm_mview[idx] = GetTimeLength(tick_mview[idx], GetSystemTimeUS());
}

void Profile_Reset_MultiView()
{
    memset(tick_mview, 0, sizeof(tick_mview));
    memset(tm_mview, 0, sizeof(tm_mview));
}

void Profile_Title_MultiView()
{
    int i;
    
    for (i=0; i<MVIEW_FUN_NUM; i++) {
         printf("%s, ", strMultiView_Function[i]);
    }
}

void Profile_Result_MultiView()
{
    int i;    
    
    for (i=0; i<MVIEW_FUN_NUM; i++) {
         printf("%3u.%03u, ", tm_mview[i]/1000, tm_mview[i]%1000);
    }
}

#endif // END #if _PROFILE_CTA_    