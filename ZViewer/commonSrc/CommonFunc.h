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

#ifdef _MSC_VER
#include <strsafe.h>
#endif
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

/// int �� tstring type ���� �����ش�.
tstring toString(int i);

/// ������ �����ϴ� ���̾�α׸� ����.
bool SelectFolder(HWND hWnd, TCHAR * szFolder);

/// ����̺�� ������� ���ϸ����� �̷���� ���ڿ��� �ָ�, ����̺�� ����������� ��ȯ�Ѵ�.
tstring GetFolderNameFromFullFileName(const tstring & strFullFilename);

/// ����̺�� ������� ���ϸ����� �� ���ڿ��� �ָ�, ���ϸ� �ش�.
tstring GetFileNameFromFullFileName(const tstring & strFullFilename);

/// ���� ���� ���� OS �� ������ ��ȯ�Ѵ�.
eOSKind getOSVersion();

/// string �� wstring ���� ��ȯ
std::wstring getWStringFromString(const std::string & str);

/// ���� ���� ������ �ִ� ������ ��´�.
tstring GetProgramFolder();

/// �����Ϸ� ������ �°� �Լ� ����
void SplitPath(const TCHAR * path, TCHAR * drive, size_t driveNumberOfElements, TCHAR * dir, size_t dirNumberOfElements,
				TCHAR * fname, size_t nameNumberOfElements, TCHAR * ext, size_t extNumberOfElements);

#ifdef _MSC_VER
#define SPrintf	StringCchPrintf
#else
#define SPrintf _snwprintf
#endif
