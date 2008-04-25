I/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	CommonFunc.cpp
*
*                                       http://www.wimy.com
*********************************************************************/
#include "stdafx.h"
#include "CommonFunc.h"
#include <shlobj.h>
#include <io.h>
#include "LogManager.h"
#include <strsafe.h>

void DebugPrintf( const TCHAR *fmt, ... )
{
#ifndef _DEBUG
	return;
#endif

	va_list v;
	TCHAR buf[1024*4];
	int len;

	va_start( v, fmt );
	len = StringCbVPrintf( buf, sizeof(buf) / sizeof(TCHAR), fmt, v );
	va_end( v );

	OutputDebugString( buf );
	OutputDebugString( TEXT("\r\n") );

	CLogManager::getInstance().Output(buf);
}

const tstring GetOnlyFileName(const tstring & strFullFileName)
{
	TCHAR szFile[FILENAME_MAX] = { 0 };
	_wsplitpath_s(strFullFileName.c_str(), NULL,0 , NULL, 0, szFile,FILENAME_MAX, NULL, 0);

	return szFile;
}

bool SetRegistryValue(HKEY hOpenKey, const tstring & strKey,LPCTSTR szValue, const tstring & strData)
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
		DWORD	dwBufferLength = (DWORD)(strData.size() + 1) * sizeof(TCHAR);

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
RECT GetResizedRectForBigToSmall(const RECT & MaximumSize, const RECT & originalSize)
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

RECT GetResizedRectForSmallToBig(const RECT & MaximumSize, const RECT & originalSize)
{
	if ( originalSize.right > MaximumSize.right && originalSize.bottom > MaximumSize.bottom )
	{
		return GetResizedRectForBigToSmall(MaximumSize, originalSize);
	}

	// ���� ���� ũ�� �� ū ���� ã�´�.
	bool bSetWidth = true;		// ���� ũ�⸦ �������� ���� ���ΰ�?

	double dWidthRate = (double)originalSize.right / (double)MaximumSize.right;
	double dHeightRate = (double)originalSize.bottom / (double)MaximumSize.bottom;

	if ( dHeightRate <=  dWidthRate)
	{
		bSetWidth = true;
	}
	else
	{
		bSetWidth = false;
	}

	RECT ret;

	if ( bSetWidth == true )
	{
		// ���� ũ�Ⱑ �����̴�.
		SetRect(&ret, 0, 0, (int)(MaximumSize.right), (int)(MaximumSize.right * originalSize.bottom/originalSize.right));
	}
	else
	{
		// ���� ũ�Ⱑ �����̴�.
		SetRect(&ret, 0, 0, (int)(originalSize.right * MaximumSize.bottom / originalSize.bottom), (int)(MaximumSize.bottom));
	}


	_ASSERTE(ret.right <= MaximumSize.right);
	_ASSERTE(ret.bottom <= MaximumSize.bottom);

	return ret;
}

tstring toString(int i)
{
	const int szSize = 20;
	TCHAR szTemp[szSize];
	StringCchPrintf(szTemp, szSize, TEXT("%d"), i);

	return tstring(szTemp);
}

/// ������ �����ϴ� ���̾�α׸� ����.
bool SelectFolder(HWND hWnd, TCHAR * szFolder)
{
	LPITEMIDLIST pidl;
	BROWSEINFO bi;

	bi.hwndOwner = hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("Select folder");
	bi.ulFlags = 0;
	bi.lpfn = 0;
	bi.lParam = 0;

	pidl = SHBrowseForFolder(&bi);

	if ( pidl == NULL )
	{
		return false;
	}

	SHGetPathFromIDList(pidl, szFolder);

	return true;
}

/// ����̺�� ������� ���ϸ����� �̷���� ���ڿ��� �ָ�, ����̺�� ����������� ��ȯ�Ѵ�.
tstring GetFolderNameFromFullFileName(const tstring & strFullFilename)
{
	TCHAR szDrive[_MAX_DRIVE] = { 0 };
	TCHAR szDir[_MAX_DIR] = { 0 };
	_tsplitpath_s(strFullFilename.c_str(), szDrive,_MAX_DRIVE, szDir,_MAX_DIR, NULL,0, NULL,0);

	tstring strFolder = szDrive;
	strFolder += szDir;

	return strFolder;
}

/// ����̺�� ������� ���ϸ����� �� ���ڿ��� �ָ�, ���ϸ� �ش�.
tstring GetFileNameFromFullFileName(const tstring & strFullFilename)
{
	TCHAR szFileName[FILENAME_MAX] = { 0 };
	TCHAR szFileExt[MAX_PATH] = { 0 };
	_tsplitpath_s(strFullFilename.c_str(), NULL,0, NULL,0, szFileName,FILENAME_MAX, szFileExt,MAX_PATH);

	tstring strFilename = szFileName;
	strFilename += szFileExt;

	return strFilename;
}

/// ���� ���� ���� OS �� ������ ��ȯ�Ѵ�.
eOSKind getOSVersion()
{
	const int BUFSIZE = 1024;

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return eOSKind_UNKNOWN;
	}

	eOSKind retKind = eOSKind_UNKNOWN;

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			retKind = eOSKind_XP;
			printf ("Microsoft Windows Server&nbsp;2003 family, ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			retKind = eOSKind_XP;
			printf ("Microsoft Windows XP ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			retKind = eOSKind_2000;
			printf ("Microsoft Windows 2000 ");
		}

		if ( osvi.dwMajorVersion <= 4 )
			printf("Microsoft Windows NT ");

		// Test for specific product on Windows NT 4.0 SP6 and later.
		if( bOsVersionInfoEx )
		{
			// Test for the workstation type.
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				if( osvi.dwMajorVersion == 4 )
					printf ( "Workstation 4.0 " );
				else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					printf ( "Home Edition " );
				else
					printf ( "Professional " );
			}

			// Test for the server type.
			else if ( osvi.wProductType == VER_NT_SERVER )
			{
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						printf ( "Datacenter Edition " );
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						printf ( "Enterprise Edition " );
					else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
						printf ( "Web Edition " );
					else
						printf ( "Standard Edition " );
				}

				else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						printf ( "Datacenter Server " );
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						printf ( "Advanced Server " );
					else
						printf ( "Server " );
				}

				else  // Windows NT 4.0 
				{
					if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						printf ("Server 4.0, Enterprise Edition " );
					else
						printf ( "Server 4.0 " );
				}
			}
		}
		else  // Test for specific product on Windows NT 4.0 SP5 and earlier
		{
			HKEY hKey;
			TCHAR szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return eOSKind_UNKNOWN;

			lRet = RegQueryValueEx( hKey, TEXT("ProductType"), NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return eOSKind_UNKNOWN;

			RegCloseKey( hKey );

			if ( lstrcmpi( TEXT("WINNT"), szProductType) == 0 )
				printf( "Workstation " );
			if ( lstrcmpi( TEXT("LANMANNT"), szProductType) == 0 )
				printf( "Server " );
			if ( lstrcmpi( TEXT("SERVERNT"), szProductType) == 0 )
				printf( "Advanced Server " );

			printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
		}

		// Display service pack (if any) and build number.

		if( osvi.dwMajorVersion == 4 && 
			lstrcmpi( osvi.szCSDVersion, TEXT("Service Pack 6") ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"),
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet == ERROR_SUCCESS )
				printf( "Service Pack 6a (Build %d)\n", osvi.dwBuildNumber & 0xFFFF );         
			else // Windows NT 4.0 prior to SP6a
			{
				printf( "%s (Build %d)\n",
					osvi.szCSDVersion,
					osvi.dwBuildNumber & 0xFFFF);
			}

			RegCloseKey( hKey );
		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			printf( "%s (Build %d)\n",
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}


		break;

		// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			printf ("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				printf("OSR2 " );
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			retKind = eOSKind_98;
			printf ("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				printf("SE " );
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			printf ("Microsoft Windows Millennium Edition\n");
		} 
		break;

	case VER_PLATFORM_WIN32s:

		printf ("Microsoft Win32s\n");
		break;
	}
	return retKind; 
}


/// string �� wstring ���� ��ȯ
std::wstring getWStringFromString(const std::string & str)
{
	WCHAR buff[256] = { 0 };
	if ( 0 == MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, str.c_str(), (int)str.size(), buff, 256) )
	{
		return L"failedConvert";
	}
	return std::wstring(buff);
}


/// ���� ���� ������ �ִ� ������ ��´�.
tstring GetProgramFolder()
{
	tstring retString;

	TCHAR szGetFileName[FILENAME_MAX] = { 0 };

	/// ZViewer, ZViewerAgent �Ѵ� FreeImage.dll �� ���Ƿ� �� dll �� �ִ� ������ ã�´�.
	DWORD ret = GetModuleFileName(GetModuleHandle(TEXT("FreeImage.dll")), szGetFileName, FILENAME_MAX);

	if ( ret == 0 )
	{
		_ASSERTE(!"Can't get module folder");
	}
	TCHAR szDrive[_MAX_DRIVE] = { 0 };
	TCHAR szDir[_MAX_DIR] = { 0 };
	_tsplitpath_s(szGetFileName, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, NULL,0, NULL,0);

	retString = szDrive;
	retString += szDir;

	return retString;
}