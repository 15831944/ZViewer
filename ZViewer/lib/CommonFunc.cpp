#include "stdafx.h"
#include "CommonFunc.h"
#include <shlobj.h>
#include <io.h>

void DebugPrintf( const char *fmt, ... )
{
#ifdef _DEBUG
	va_list v;
	char buf[1024*4];
	int len;

	va_start( v, fmt );
	len = wvsprintf( buf, fmt, v );
	va_end( v );

	OutputDebugString( buf );
	OutputDebugString( "\r\n" );
#endif
}

bool StringCompare(const std::string & a, const std::string & b)
{
#pragma message("TODO : ���ڿ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���ؾ���")
	return (strcmp(b.c_str(), a.c_str()) > 0);
}

const std::string GetOnlyFileName(const std::string & strFullFileName)
{
	char szFile[MAX_PATH] = { 0 };
	_splitpath(strFullFileName.c_str(), 0, 0, szFile, 0);

	return szFile;
}

bool FilenameCompare(const std::string & a, const std::string & b)
{
	return (strcmp(GetOnlyFileName(b).c_str(), GetOnlyFileName(a).c_str()) > 0);
}


bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData)
{
	if( !hOpenKey || strKey.empty() || !szValue)
	{
		_ASSERTE(!"SetRegistryValue invalid arg");
		return false;
	}

	bool bRetVal = false;
	DWORD dwDisposition;
	HKEY hTempKey = NULL;

	if( ERROR_SUCCESS == ::RegCreateKeyEx(hOpenKey, strKey.c_str(), NULL,
		NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hTempKey, &dwDisposition) )
	{
		// ������ \0 ���� �����ؾ��Ѵٴ���;;
		DWORD	dwBufferLength = (DWORD)strData.size() + 1;

		if( ERROR_SUCCESS == ::RegSetValueEx(hTempKey, (LPTSTR)szValue,
			NULL, REG_SZ, (const BYTE *)strData.c_str(), dwBufferLength) )
		{
			bRetVal = true;
		}
	}

	if( hTempKey )
	{
		::RegCloseKey(hTempKey);
	}

	return bRetVal;
}

// �ִ� ũ�⸦ ���� �ʴ� ������ �������� ũ�⸦ �����ش�.
RECT GetResizedRect(const RECT & MaximumSize, const RECT & originalSize)
{
	if ( originalSize.right <= MaximumSize.right && originalSize.bottom <= MaximumSize.bottom )
	{
		RECT ret = originalSize;
		return ret;
	}

	// ���� ���� ũ�� �� ū ���� ã�´�.
	bool bSetWidth = true;		// ���� ũ�⸦ �������� ���� ���ΰ�?

	double dWidthRate = (double)MaximumSize.right / (double)originalSize.right;
	double dHeightRate = (double)MaximumSize.bottom / (double)originalSize.bottom;

	if ( dHeightRate >=  dWidthRate)
	{
		bSetWidth = true;
	}
	else
	{
		bSetWidth = false;
	}

	// ū ���� MaximumSize �� �ǰ� �ϴ� ��ʸ� ã�´�.
	RECT ret;

	double dRate = 1;
	if ( bSetWidth == true )
	{
		// ���� ũ�Ⱑ �����̴�.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dWidthRate), (int)(originalSize.bottom*dWidthRate));
	}
	else
	{
		// ���� ũ�Ⱑ �����̴�.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dHeightRate), (int)(originalSize.bottom*dHeightRate));
	}


	_ASSERTE(ret.right <= MaximumSize.right);
	_ASSERTE(ret.bottom <= MaximumSize.bottom);

	return ret;
}

std::string toString(int i)
{
	char szTemp[20];
	_snprintf(szTemp, sizeof(szTemp), "%d", i);

	return std::string(szTemp);
}

bool SelectFolder(HWND hWnd, char * szFolder)
{
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl;
	BROWSEINFO bi;

	bi.hwndOwner = hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = "Select folder";
	bi.ulFlags = 0;
	bi.lpfn = 0;
	bi.lParam = 0;

	pidl = SHBrowseForFolder(&bi);

	if ( pidl == NULL )
	{
		return false;
	}

	SHGetPathFromIDList(pidl, szFolder);

	if ( SHGetMalloc(&pMalloc) != NOERROR )
	{
		return false;
	}

	pMalloc->Free(pidl);
	pMalloc->Release();

	return true;
}

std::string GetFolderNameFromFullFileName(const std::string & strFullFilename)
{
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	_splitpath(strFullFilename.c_str(), szDrive, szDir, 0, 0);

	std::string strFolder = szDrive;
	strFolder += szDir;

	return strFolder;
}

std::string GetFileNameFromFullFileName(const std::string & strFullFilename)
{
	char szFileName[MAX_PATH] = { 0 };
	char szFileExt[MAX_PATH] = { 0 };
	_splitpath(strFullFilename.c_str(), 0, 0, szFileName, szFileExt);

	std::string strFilename = szFileName;
	strFilename += szFileExt;

	return strFilename;
}

