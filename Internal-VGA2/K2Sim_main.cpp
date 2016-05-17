// K2Sim_main.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "windows.h"
#include "avisynth.h"
//#include "SmartPark.h"
#include "CTAApi.h"


class Invert : public GenericVideoFilter {

	public: 
	//Invert(PClip _child) : GenericVideoFilter(_child) {} 
	Invert(PClip _child);
	~Invert();        

	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env); 
};

Invert::Invert(PClip _child) : GenericVideoFilter(_child) 
{    
	// vi.width  is the width of the video frame
	// vi.height is the height of the video frame
    CTA_WorkBuf_Allocate();
    //old version
	//oTo_WorkBuf_Allocate4AVISyn(vi.width, vi.height); 
	vi.width = 640+420;
}

Invert::~Invert()
{
    CTA_Release();
    // old version
	//oTo_WorkBuf_Release(); // release work buffer for oTo_SmartPark
}

PVideoFrame __stdcall Invert::GetFrame(int n, IScriptEnvironment* env) {

	PVideoFrame src = child->GetFrame(n, env); 
	PVideoFrame dst = env->NewVideoFrame(vi); 

	const unsigned char* srcpY = src->GetReadPtr(PLANAR_Y); 
	const unsigned char* srcpV = src->GetReadPtr(PLANAR_V); 
	const unsigned char* srcpU = src->GetReadPtr(PLANAR_U); 

	unsigned char* dstpY = dst->GetWritePtr(PLANAR_Y); 
	unsigned char* dstpV = dst->GetWritePtr(PLANAR_V); 
	unsigned char* dstpU = dst->GetWritePtr(PLANAR_U); 

	const int src_pitchY  = src->GetPitch(PLANAR_Y); 
	const int src_pitchUV = src->GetPitch(PLANAR_V); 
	const int dst_pitchY  = dst->GetPitch(PLANAR_Y); 
	const int dst_pitchUV = dst->GetPitch(PLANAR_U); 

	const int row_sizeY  = dst->GetRowSize(PLANAR_Y); // Could also be PLANAR_Y_ALIGNED which would return a mod16 rowsize 
	const int row_sizeUV = dst->GetRowSize(PLANAR_U); // Could also be PLANAR_U_ALIGNED which would return a mod8 rowsize 

	const int heightY  = dst->GetHeight(PLANAR_Y); 
	const int heightUV = dst->GetHeight(PLANAR_U); 

	bool ret_code;
	VideoInfo info;
	info = GetVideoInfo();

	ret_code = info.IsRGB();
	ret_code = info.IsRGB24();
	ret_code = info.IsRGB32();
	ret_code = info.IsYUV();
	ret_code = info.IsYUY2();
	ret_code = info.IsYV12();
	ret_code = info.IsPlanar();
	ret_code = info.IsFieldBased();

	//oTo_LDWS(dstpY, dstpU, dstpV, (unsigned char *)srcpY, (unsigned char *)srcpU, (unsigned char *)srcpV, 
	//		 vi.width, vi.height);
    CTA_Process(dstpY, (unsigned char *)srcpY, NULL);
    // old version
	//	oTo_ImgProcess2(dstpY, vi.width*2, vi.height, (unsigned char *)srcpY // YCbCr 422
	//		 ,640*2, vi.height,1,20,NULL);

	return dst; 

}

AVSValue __cdecl Create_Invert(AVSValue args, void* user_data, IScriptEnvironment* env) {

	return new Invert(args[0].AsClip()); 

}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env) {

	env->AddFunction("Invert", "c", Create_Invert, 0); 
	return "`Invert' sample plugin"; 

}