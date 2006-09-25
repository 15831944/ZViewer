/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	CommonFunc.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "CommonDefine.h"

/// OS ������ �Ǵ��� �� ���� enum
enum eOSKind
{
	eOSKind_UNKNOWN,
	eOSKind_98,
	eOSKind_2000,
	eOSKind_XP,
};

const tstring GetOnlyFileName(const tstring & strFullFileName);

class CStringCompareIgnoreCase
{
public:
	bool operator()(const tstring & a, const tstring & b)
	{
		/*
		// ���ڿ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���Ѵ�.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), a.c_str());
		_snprintf(szTempB, sizeof(szTempB), b.c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (strcmp(szTempB, szTempA) > 0);
		*/
		return (_tcscmp(b.c_str(), a.c_str()) > 0);
	}
};

class CStringCompareIgnoreCase_LengthFirst
{
public:
	bool operator()(const tstring & a, tstring & b)
	{
		// ���̸� �켱.
		if ( a.size() < b.size() )
		{
			return true;
		}

		if ( a.size() > b.size() )
		{
			return false;
		}

		/*
		// ���ڿ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���Ѵ�.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), a.c_str());
		_snprintf(szTempB, sizeof(szTempB), b.c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (strcmp(szTempB, szTempA) > 0);
		*/
		return (_tcscmp(b.c_str(), a.c_str()) > 0);

	}
};

void DebugPrintf( const TCHAR *fmt, ... );

bool SetRegistryValue(HKEY hOpenKey, const tstring & strKey,LPCTSTR szValue, const tstring & strData);

/// �ִ� ũ�⸦ ���� �ʴ� ������ �������� ũ�⸦ �����ش�.
RECT GetResizedRectForBigToSmall(const RECT & MaximumSize, const RECT & originalSize);

/// �ִ� ũ�⸦ ���� �ʴ� ������ �������� ũ�⸦ �����ش�.
RECT GetResizedRectForSmallToBig(const RECT & MaximumSize, const RECT & originalSize);

tstring toString(int i);

bool SelectFolder(HWND hWnd, TCHAR * szFolder);

tstring GetFolderNameFromFullFileName(const tstring & strFullFilename);
tstring GetFileNameFromFullFileName(const tstring & strFullFilename);

eOSKind getOSVersion();

/// string �� wstring ���� ��ȯ
std::wstring getWStringFromString(const std::string & str);

/// ���� ���� ������ �ִ� ������ ��´�.
tstring GetProgramFolder();