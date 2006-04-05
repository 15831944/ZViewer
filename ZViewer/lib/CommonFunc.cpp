/********************************************************************
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

void DebugPrintf( const char *fmt, ... )
{
#ifndef _DEBUG
	return;
#endif

	va_list v;
	char buf[1024*4];
	int len;

	va_start( v, fmt );
	len = wvsprintf( buf, fmt, v );
	va_end( v );

	OutputDebugString( buf );
	OutputDebugString( "\r\n" );

	CLogManager::getInstance().Output(buf);
}

/*
bool StringCompare(const std::string & a, const std::string & b)
{
	// ���ڿ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���Ѵ�.
	char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
	_snprintf(szTempA, sizeof(szTempA), a.c_str());
	_snprintf(szTempB, sizeof(szTempB), b.c_str());
	strlwr(szTempA);
	strlwr(szTempB);

	return (strcmp(szTempB, szTempA) > 0);
}
*/

const std::string GetOnlyFileName(const std::string & strFullFileName)
{
	char szFile[MAX_PATH] = { 0 };
	_splitpath(strFullFileName.c_str(), 0, 0, szFile, 0);

	return szFile;
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

std::string toString(int i)
{
	char szTemp[20];
	_snprintf(szTemp, sizeof(szTemp), "%d", i);

	return std::string(szTemp);
}

bool SelectFolder(HWND hWnd, char * szFolder)
{
	//LPMALLOC pMalloc;
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

	/*
	if ( SHGetMalloc(&pMalloc) != NOERROR )
	{
		return false;
	}

	pMalloc->Free(pidl);
	pMalloc->Release();
	*/

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
			char szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return eOSKind_UNKNOWN;

			lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return eOSKind_UNKNOWN;

			RegCloseKey( hKey );

			if ( lstrcmpi( "WINNT", szProductType) == 0 )
				printf( "Workstation " );
			if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
				printf( "Server " );
			if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
				printf( "Advanced Server " );

			printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
		}

		// Display service pack (if any) and build number.

		if( osvi.dwMajorVersion == 4 && 
			lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
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