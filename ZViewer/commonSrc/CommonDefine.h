/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	CommonDefine.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include <string>
#include <windows.h>

/// �Ϲ������� ���澲�� ������ ũ��
const int COMMON_BUFFER_SIZE = 512;

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#define		EXIF_ROTATION_0		"top, left side"
#define		EXIF_ROTATION_90	"right side, top"
#define		EXIF_ROTATION_180	"bottom, right side"
#define		EXIF_ROTATION_270	"left side, bottom"


const tstring g_strVersion = TEXT("0.7.0alpha1");
const tstring g_strHomepage = TEXT("http://zviewer.wimy.com");
struct TagData
{
	std::string m_strKey;
	std::string m_strValue;
};

struct FileData
{
	tstring m_strFileName;	// ���ϸ�
	_FILETIME m_timeModified;	// �ֱ� ������ ��¥
	DWORD m_nFileSize;			// ����ũ��
};

struct ExtSetting
{
	unsigned int m_numIconIndex;		// icon dll ������ index
	tstring m_strExt;				// ������ Ȯ����
};


class CFileDataSort_OnlyFilenameCompare
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		/*
		// ���ϸ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���Ѵ�.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), GetOnlyFileName(a.m_strFileName).c_str());
		_snprintf(szTempB, sizeof(szTempB), GetOnlyFileName(b.m_strFileName).c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (strcmp(szTempB, szTempA) > 0);
		*/
		return (_tcsicmp(b.m_strFileName.c_str(), a.m_strFileName.c_str()) > 0);
	}
};

class CFileDataSort_OnlyFilenameCompare_XP
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		/*
		// ���ϸ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���Ѵ�.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), GetOnlyFileName(a.m_strFileName).c_str());
		_snprintf(szTempB, sizeof(szTempB), GetOnlyFileName(b.m_strFileName).c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (StrCmp(szTempB, szTempA) > 0);
		*/

		return ( _tcsicmp(b.m_strFileName.c_str(), a.m_strFileName.c_str()) > 0);
	}
};

class CFileDataSort_FileSize
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		return a.m_nFileSize > b.m_nFileSize;
	}
};

class CFileDataSort_LastModifiedTime
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		// �����ִ� �ε�ȣ�� > �̸� �ֱٰ��� �տ�, < �̸� �ֱٰ��� �ڿ� ����.
		if ( a.m_timeModified.dwHighDateTime == b.m_timeModified.dwHighDateTime)
		{
			return a.m_timeModified.dwLowDateTime >= b.m_timeModified.dwLowDateTime;
		}
		return a.m_timeModified.dwHighDateTime >= b.m_timeModified.dwHighDateTime;
	}
};
