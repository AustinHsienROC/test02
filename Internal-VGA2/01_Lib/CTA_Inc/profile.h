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
#ifndef _PROFILE_H_

#ifdef _WIN32 
  #define _PROFILE_MVIEW_   (0)   // always define 0 for win32 platform
  #define _PROFILE_MOD_     (0)   // always define 0 for win32 platform
  #define _PROFILE_CTA_     (0)   // always define 0 for win32 platform
#else // K2 platform  
  #define _PROFILE_MVIEW_   (0) // Enable/disable profile multi-view module, both ARM and APEX (to display profile info in debugger's console window)
  #define _PROFILE_MOD_     (0) // Enable/disable profile MOD module, both ARM and APEX (to display profile info in debugger's console window)
  #define _PROFILE_CTA_     (0) // Enable/disable profile CTA module, both ARM and APEX (to display profile info in in debugger's console window)
#endif  

//------------------------------------------------------------------------

#if (_PROFILE_MOD_ || _PROFILE_CTA_ || _PROFILE_MVIEW_)  
    void Profile_Reset();    
    void Profile_PrintReslult();         
#else
    #define Profile_Reset()                   // dummy function
    #define Profile_PrintReslult()            // dummy function
#endif // END #if (_PROFILE_MOD_ || _PROFILE_MVIEW_)

#if _PROFILE_MOD_
    typedef enum { 
        e_APEX_FD1_Phase1 = 0, 
        e_APEX_FD1_Phase2,
        e_APEX_FD5,
        e_APEX_FD6_Phase1,
        e_APEX_FD6_Phase2,
        e_APEX_FD4_Phase1,
        e_APEX_FD4_Phase2,
        e_APEX_FD8,
        e_APEX_HarrisCorner,
        e_APEX_SuppressNonMax,        
        e_MOD_FD7,
        e_MOD_FD9,
        e_MOD_ME2,
        e_MOD_ME4ME5,
        e_MOD_Tracking, 
        e_MOD_UpdateRegionMap,
        e_K2_GetBoundaryPoint,
        e_MOD_GenerateReport,
    } MOD_ProfileFunction_t;    
    
  void Profile_MOD_Start(int idx);
  void Profile_MOD_End(int idx);
#else
  #define Profile_MOD_Start(idx)        // dummy function
  #define Profile_MOD_End(idx)          // dummy function
#endif // END #if _PROFILE_MOD_

#if _PROFILE_CTA_
    typedef enum {        
        e_APEX_CTA_DayProcess = 0,
        e_APEX_CTA_HarrisCorner,
        e_APEX_CTA_CntEdgeQty,
        e_APEX_CTA_CntSobelQty,   
        e_SetPara,
        e_CornersMotionEstimation,
        e_PointTrackPatch, 
        e_MinorBlockUpdate, 
        e_MinorBlockLabeling,
        e_CandidateUpdate,
        e_PointGroupPatch,
        e_SetStartupFlag,
        e_CandAssociation,
        e_EdgeTracking,
        e_UpdateBoundaryInfo,
        e_EliminateCrossover,
        e_GetCandInfo,
        e_DecideMoveDir,
        e_MergeOverlappedCand,
        e_UpdateFlagTHD,
        e_CandidateFeatureUpdate_1, 
        e_Predict_1,
        e_bawl,     
        e_SaveCornerPos,
        e_Update_Data_Before_Next_Frame,
    } CTA_ProfileFunction_t;
    
    void Profile_CTA_Start(int idx);
    void Profile_CTA_End(int idx);
#else
    #define Profile_CTA_Start(idx)        // dummy function
    #define Profile_CTA_End(idx)          // dummy function
#endif // END #if _PROFILE_CTA_

#if _PROFILE_MVIEW_
    typedef enum {        
        e_TopView_Create = 0,
        e_CornerView_Create,
    } MultiView_ProfileFunction_t;
    
    void Profile_MultiView_Start(int idx);
    void Profile_MultiView_End(int idx);
#else
    #define Profile_MultiView_Start(idx)        // dummy function
    #define Profile_MultiView_End(idx)          // dummy function
#endif // END #if _PROFILE_CTA_
 
#endif // END #ifndef _PROFILE_H_
                             

