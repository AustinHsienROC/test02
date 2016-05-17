/**

	Copyright (C) 2014 By oToBrite Electronics Corp.

	@file 	FilesInfo.h
	@brief  This function implement Information get from specific Folder

	@version	1nd
	@author		Austin
	@date		2014/08/09
	@n  * Implement Functions.
*/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef _FILEINFO_H_
#define _FILEINFO_H_
//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************

#include <windows.h> //GetModuleFileName
#include <string>
#include <vector>
#include <algorithm>    // std::find
#include <locale.h>

//#include <iostream> //wcout
#include "CTA_Def.h"

//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

#if( ON == PC_FILE_INFO ) 
//------------------------------------

#define MAXFileFolderLen 240
//------------------------------------

//******************************************************************************
// C L A S S   B O D Y S
//******************************************************************************

using namespace std; 

class FilesInfo
{
private:
	wstring FolderName;
	wstring FileType;
	vector<wstring> FilesList;

public:
	FilesInfo(void);
	FilesInfo(wstring path);
	~FilesInfo(void);

	void	SetFolderName(wstring path);
	void	SetFileType(wstring FileType);
	
	wstring GetCurFilePath();
	wstring GetCurFileType();
	INT32	GetFilesList(vector<wstring> &list);

	void	UpdateFilesList(void);
	BOOL	isFileNameExist(wstring str1, INT32 &index);


	//-----------------char//Wchar//string//wstring_Format_Transform------------------------
	static wstring str2wstr(string str);					// string <-> wstring
	static string  wstr2str(wstring wstr);					// wstring<-> string
	static void    str2char(string str, CHAR cha1[240]);	// string <-> char
	static string  char2str(CHAR *cha1);					// char   <-> string
	static void    char2wchar(CHAR *cha1,wchar_t *wcha1);	// char   <-> wchar
	static void	   wchar2char(wchar_t *wcha1,CHAR *cha1);	// wchar   <-> char
	static wstring wchar2wstr(wchar_t *wcha1);			    // wchar   <-> wstring
	static void	   wstr2wchar(wstring wstr,wchar_t *wcha1);	// wstring <-> wchar

};
#endif
#endif