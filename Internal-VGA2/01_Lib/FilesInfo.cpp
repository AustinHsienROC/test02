//******************************************************************************
// I N C L U D E   F I L E S
//******************************************************************************
#include "StdAfx.h"
#include "FilesInfo.h"

#if( ON == PC_FILE_INFO  )
//******************************************************************************
// M A C R O   D E F I N I T I O N S
//******************************************************************************

//******************************************************************************
// S T A T I C   F U N C T I O N   P R O T O T Y P E S
//******************************************************************************

void FilesInfo::char2wchar(CHAR *cha1,wchar_t *wcha1)
{
	//strlen的結果要在運行的時候才能計算出來，是用來計算字串的長度，不是類型占記憶體的大小(sizeof)。
	mbstowcs(wcha1, cha1, strlen(cha1)); 
}
void FilesInfo::wchar2char(wchar_t *wcha1,CHAR *cha1)
{
	wcstombs(cha1, wcha1, wcslen(wcha1));
}
/**
* @brief  // string <-> char
* [Limit to 240 Len]
*/
void FilesInfo::str2char(string str , CHAR cha1[240]) 
{
	strcpy(cha1,str.c_str());
}
void FilesInfo::wstr2wchar(wstring wstr,wchar_t *wcha1)
{
	wcscpy(wcha1,wstr.c_str());
}

/**
* @brief // char   <-> string
*/
string FilesInfo::char2str(CHAR *cha1)
{
	return string(cha1);
}
wstring FilesInfo::wchar2wstr(wchar_t *wcha1)
{
	return wstring(wcha1);
}

/**
* @brief // wstring<-> string
*/
string FilesInfo::wstr2str(wstring wstr)		 // wstring<-> string
{
	unsigned len = wstr.size() * 4 ;
	setlocale(LC_CTYPE, "" );
	char *p = new  char [len];
	wcstombs(p,wstr.c_str(),len);
	string str1(p);
	delete[] p;
	return str1;
}
/**
* @brief string <-> wstring
*/
wstring FilesInfo::str2wstr(string str)
{
	unsigned len = str.size() * 2 ; 
	setlocale(LC_CTYPE, "" );      
	wchar_t *p = new wchar_t[len]; 
	mbstowcs(p,str.c_str(),len); 
	std::wstring str1(p);
	delete[] p; 
	return str1;
}
//******************************************************************************
// F U N C T I O N   B O D Y S
//******************************************************************************


/**
* @brief set FilesList as the File List in current Folder
*/
void FilesInfo::UpdateFilesList(void)
{
	//3) find the File(filename) under specific folder.
	//FolderName ///< specific Folder
	wstring FilePathType = FolderName;
	FilePathType += L"*.";
	FilePathType += FileType; ///< "....\\*.*"

	WIN32_FIND_DATA findFileData;
	HANDLE hfind = FindFirstFile( FilePathType.c_str() , &findFileData );
	if(hfind != INVALID_HANDLE_VALUE)
	{
		//comboBox1->Items->Clear();
		FilesList.clear();
		while(1)
		{
			if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //the Folder
			{}else // the file
			{
				//1) PRINT File List
				//wcout<<findFileData.cFileName<<endl;
				//2) put into checkbox1
				FilesList.push_back(findFileData.cFileName);

			}
			if(!FindNextFile(hfind,&findFileData))break;
		}
		//comboBox1->SelectedIndex = 0;
		FindClose(hfind);
	}
}
/**
* @brief set the current path as default to FolderName
* @return	Length of list
*/
INT32 FilesInfo::GetFilesList(vector<wstring> &list)
{
	list.clear();

	for ( std::vector<wstring>::iterator it = FilesList.begin() ; it != FilesList.end() ; it++)
	{
		list.push_back( (*it) );
	}
	return list.size(); //ex.5 = item[0-4]
}

BOOL FilesInfo::isFileNameExist(wstring str1,INT32 &index)
{
	std::vector<wstring>::iterator it;

	// iterator to vector element:
	it = find (FilesList.begin(), FilesList.end(), str1);

	if(it != FilesList.end()) // Find successful
	{
		index = distance(FilesList.begin(), it) ;
		return true;
	}
	
	return false;
}
/**
* @brief set the current path as default to FolderName
*/
FilesInfo::FilesInfo(void)
{
	//1) Get Current Path(strModulePath)
	wchar_t curPath[MAXFileFolderLen];
	GetModuleFileName(NULL,curPath,MAXFileFolderLen); ///< c:\\.....\\AustinTemplate.exe
	FolderName = wchar2wstr(curPath);
	
	//2) Remove the "AustinTemplate.exe"
	int found  = FolderName.rfind(L"\\");
	FolderName = FolderName.substr(0,found+1);

	FileType = L"*";
}
FilesInfo::FilesInfo(wstring path)
{
	this->FolderName = path;
	FileType = L"*";
}
FilesInfo::~FilesInfo()
{
}
void FilesInfo::SetFolderName(wstring path)
{
	this->FolderName = path;
}
void FilesInfo::SetFileType(wstring FileType){ this->FileType = FileType;}
wstring FilesInfo::GetCurFilePath()
{
	return FolderName;
}
wstring FilesInfo::GetCurFileType()
{
	return FileType;
}
#endif