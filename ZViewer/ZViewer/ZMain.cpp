/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZMain.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "StdAfx.h"
#include ".\zmain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "../lib/DesktopWallPaper.h"
#include "src/MoveToDlg.h"
#include "src/SaveAs.h"
#include "src/ZOption.h"

#include <ShlObj.h>
#include <cstdio>

#include "resource.h"

using namespace std;

ZMain & ZMain::GetInstance()
{
	static ZMain aInstance;
	return aInstance;
}


ZMain::ZMain(void)
:	m_hMainDlg(NULL)
,	m_sortOrder(eFileSortOrder_FILENAME)
,	m_osKind(eOSKind_UNKNOWN)
{
	m_bLastCacheHit = false;
	SetProgramFolder();

	_SetOSVersion();

	_ASSERTE(m_osKind != eOSKind_UNKNOWN);
}

ZMain::~ZMain(void)
{
}



/// Timer �� �޾��� ��
void ZMain::onTimer()
{
	if ( ZCacheImage::GetInstance().isCachingNow() )
	{
		DebugPrintf("now caching...");
	}
	showCacheStatus();
}


BOOL ZMain::_SetOSVersion()
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
			return FALSE;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			m_osKind = eOSKind_XP;
			printf ("Microsoft Windows Server&nbsp;2003 family, ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			m_osKind = eOSKind_XP;
			printf ("Microsoft Windows XP ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			m_osKind = eOSKind_2000;
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
				return FALSE;

			lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return FALSE;

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
			m_osKind = eOSKind_98;
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
	return TRUE; 
}


int ZMain::GetLogCacheHitRate() const
{
	return ZCacheImage::GetInstance().GetLogCacheHitRate();
}

long ZMain::GetCachedKByte() const
{
	return ZCacheImage::GetInstance().GetCachedKByte();
}

void ZMain::SetHWND(HWND hWnd)
{
	m_hMainDlg = hWnd;
	InitOpenFileDialog();
}

void ZMain::InitOpenFileDialog()
{
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hMainDlg;
	ofn.lpstrFile = szFile;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "ImageFiles(jpg,gif,png,bmp,psd,tga,tif,ico)\0*.jpg;*.jpeg;*.gif;*.png;*.bmp;*.psd,*.tga;*.tif;*.ico\0All(*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = m_strCurrentFolder.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void ZMain::OpenFileDialog()
{
	// ���� ������ ������ �ش�.
	ofn.lpstrInitialDir = m_strCurrentFolder.c_str();

	//m_bOpeningFileDialog = true;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn)==TRUE) 
	{
		OpenFile(ofn.lpstrFile);
	}
}

/// ���� ������ �ٸ� �������� �����ϴ� ���� ���̾�α׸� ����.
void ZMain::SaveFileDialog()
{
	CSaveAs::getInstance().setParentHWND(m_hMainDlg);
	CSaveAs::getInstance().setDefaultSaveFilename(m_strCurrentFolder, m_strCurrentFilename);
	
	if ( CSaveAs::getInstance().showDialog() )
	{
		std::string strSaveFilename = CSaveAs::getInstance().getSaveFileName();

		if ( false == m_currentImage.SaveToFile(strSaveFilename, 0) )
		{
			MessageBox(m_hMainDlg, "unsupported file type", "ZViewer", MB_OK);
		}
	}
}

void ZMain::OnInit()
{
	if ( m_strInitArg.empty() )
	{
		// ���� ���ڰ� ������ ���α׷� ������ ���� �����̴�.
		m_strCurrentFolder = m_strProgramFolder;
		RescanFolder();

		//m_currentImage = m_bannerImage;
	}
	else
	{
		// ���� ���ڰ� ������ �� ������ �����ش�.
		OpenFile(m_strInitArg);
	}
}

void ZMain::Draw(bool bEraseBg)
{
	if ( m_currentImage.IsValid() == FALSE ) return;
	if ( m_hMainDlg == NULL ) return;

	RECT currentScreenRect;
	getCurrentScreenRect(currentScreenRect);

	HDC mainDC = GetDC(m_hMainDlg);

	if ( m_vFile.size() <= 0 )
	{
		SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
		Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);

		ReleaseDC(m_hMainDlg, mainDC);
		return;
	}

	// �׸��� �׷����� ������ ȭ���� ��ġ
	int iDrawX = 0;			
	int iDrawY = 0;

	// �׸��� �׷����� �����ϴ� �׸��� ��ġ
	int iDrawPartX = 0;
	int iDrawPartY = 0;

	bool bNeedClipping = false;

	const WORD currentImageWidth = m_currentImage.GetWidth();
	const WORD currentImageHeight = m_currentImage.GetHeight();

	if ( currentImageWidth <= currentScreenRect.right )
	{
		// ���� ���̰� ȭ�麸�� ���� ���� �߾ӿ� ����
		iDrawX = (currentScreenRect.right/2) - (currentImageWidth/2);
	}
	else
	{
		// ���� ���̰� ȭ�麸�� ũ�� Ŭ����
		iDrawPartX = m_iShowingX;
		bNeedClipping = true;
	}

	if ( currentImageHeight <= currentScreenRect.bottom )
	{
		// ���� ���̰� ȭ�麸�� ���� ���� �߾ӿ� ����
		iDrawY = (currentScreenRect.bottom/2) - (currentImageHeight/2);
	}
	else
	{
		// ���� ���̰� ȭ�麸�� ũ�� Ŭ����
		iDrawPartY = m_iShowingY;
		bNeedClipping = true;
	}

#pragma message("TODO : ȭ���� �������� �� Rectangle �� ������ �ʰ�, ȭ�� ũ�⸸ŭ memoryDC �� ���� �� �˰� ĥ�ϰ�, �׸��� �ű� �׸���, �� memDC �� �׸���")
#pragma message("TODO : memDC �� �ٽ� ���õǾ���ϴ����� flag �� �����ؼ� ȭ���� ��ũ���� ��, ���� �ε巴�� ����")

	/// �׸��� ȭ�麸�� ū�� Ȯ��
	if ( bNeedClipping )
	{
		DebugPrintf("!!!!!!!!! clipping on draw......");

		/// �׸��� ȭ�麸�� ũ��...

		if ( ZOption::GetInstance().m_bBigToSmallStretchImage )
		{
			/// ū �׸��� ��ҽ��Ѽ� �׸���.

			RECT toRect;
			SetRect(&toRect, 0, 0, currentImageWidth, currentImageHeight);
			toRect = GetResizedRectForBigToSmall(currentScreenRect, toRect);

			ZImage stretchedImage(m_currentImage);

			/*
			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			*/

			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			iDrawX = (currentScreenRect.right - toRect.right) / 2;
			iDrawY = (currentScreenRect.bottom - toRect.bottom) / 2;

			if ( bEraseBg )	// ����� ������ �ϸ� �����. ���ο� �׸��� �׸��� ������ �׷��� �������� ����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			int r = StretchDIBits(mainDC,
				iDrawX, iDrawY, 
				stretchedImage.GetWidth(), stretchedImage.GetHeight(),
				0, 0,
				stretchedImage.GetWidth(), stretchedImage.GetHeight(),
				stretchedImage.GetData(),
				stretchedImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);
		}
		else
		{
			/// ū �׸��� ��ũ���� �����ϰ� Ŭ�����ؼ� �׸���.
			HDC memDC = CreateCompatibleDC(mainDC);

			HBITMAP hbmScreen = CreateCompatibleBitmap(mainDC, currentImageWidth, currentImageHeight); 

			SelectObject(memDC, hbmScreen);

			// �޸𸮿� ��ü �׸��� �׸���.
			if ( m_currentImage.isTransparent() )
			{
				DebugPrintf("Drawing transparent image...");
				fipWinImage tempImage;
				tempImage = m_currentImage.getFipImage();

				tempImage.rescale(currentImageWidth, currentImageHeight, FILTER_BOX);
				RECT rt;
				rt.left = 0;
				rt.top = 0;
				rt.right = currentImageWidth;
				rt.bottom = currentImageHeight;

				tempImage.drawEx(memDC, rt, TRUE);
			}
			else
			{
				int r = StretchDIBits(memDC,
					0, 0, 
					currentImageWidth, currentImageHeight,
					0, 0,
					currentImageWidth, currentImageHeight,
					m_currentImage.GetData(),
					m_currentImage.GetBitmapInfo(),
					DIB_RGB_COLORS, SRCCOPY);
			}

			if ( bEraseBg )	// ����� ������ �ϸ� �����. ���ο� �׸��� �׸��� ������ �׷��� �������� ����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			// �޸𸮰��� ȭ�鿡 �׸���.
			BOOL b = BitBlt(mainDC, 
				iDrawX, iDrawY,			// �׸� ȭ���� x, y ��ǥ. ȭ�鿡 �� �� ���� 0, 0 �̾���Ѵ�.
				m_currentImage.GetWidth(), currentScreenRect.bottom,		// �׷��� ȭ���� ����, ���� ����. 
				memDC, 
				iDrawPartX, iDrawPartY,			// �׷��� �̹��� ������ ���� x,y ��ǥ
				SRCCOPY);

			//DebugPrintf("rt.bottom : %d, PartX : %d, iDrawPartY : %d", currentScreenRect.bottom, iDrawPartX, iDrawPartY);

			DeleteObject(hbmScreen);
			DeleteDC(memDC);
		}
	}
	else	/// image is smaller than screen
	{
		if ( currentImageWidth < currentScreenRect.right && currentImageHeight < currentScreenRect.bottom 
			&& ZOption::GetInstance().m_bSmallToBigStretchImage ) ///< if option is on
		{
			DebugPrintf("!!!!!!!!!!!! stretching on draw...");

			/// ���� �׸��� ȭ�鿡 �°� Ȯ���ؼ� �׸���.
			RECT originalImageRect;
			SetRect(&originalImageRect, 0, 0, currentImageWidth, currentImageHeight);
			RECT toRect = GetResizedRectForSmallToBig(currentScreenRect, originalImageRect);

			ZImage stretchedImage(m_currentImage);

			/*
			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			*/

			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			_ASSERTE(toRect.right == stretchedImage.GetWidth());
			_ASSERTE(toRect.bottom == stretchedImage.GetHeight());

			int iDrawPointX = (currentScreenRect.right - toRect.right) / 2;
			int iDrawPointY = (currentScreenRect.bottom - toRect.bottom) / 2;
			
			if ( bEraseBg )	// ����� ������ �ϸ� �����. ���ο� �׸��� �׸��� ������ �׷��� �������� ����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			if ( stretchedImage.isTransparent() )
			{
				DebugPrintf("drawing trans.....");
//				stretchedImage.Resize(currentImageWidth, currentImageHeight);

				fipWinImage temp;
				temp = stretchedImage.getFipImage();

				RECT rt;
				rt.left = iDrawPointX;
				rt.top = iDrawPointY;
				rt.right = toRect.right;
				rt.bottom = toRect.bottom;

				temp.drawEx(mainDC, rt, TRUE);
			}
			else
			{
				int r = StretchDIBits(mainDC,
					iDrawPointX, iDrawPointY, 
					toRect.right, toRect.bottom,
					0, 0,
					toRect.right, toRect.bottom,
					stretchedImage.GetData(),
					stretchedImage.GetBitmapInfo(),
					DIB_RGB_COLORS, SRCCOPY);
			}

		}
		else
		{
			if ( bEraseBg )	// ����� ������ �ϸ� �����. ���ο� �׸��� �׸��� ������ �׷��� �������� ����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			if ( m_currentImage.isTransparent() )
			{
				DebugPrintf("Drawing transparent image...");
				fipWinImage tempImage;
				tempImage = m_currentImage.getFipImage();
				
				/// rescale �� �ϸ�, ������ ��濡 üũ���̰� �׷�����... �� �̷���;;;
				tempImage.rescale(currentImageWidth, currentImageHeight, FILTER_BOX);
				RECT rt;
				rt.left = iDrawX;
				rt.top = iDrawY;
				rt.right = rt.left + currentImageWidth;
				rt.bottom = rt.top + currentImageHeight;
				tempImage.drawEx(mainDC, rt, TRUE);
			}
			else
			{
				/// ���� �׸��� ȭ�� ����� �׸���.
				int r = StretchDIBits(mainDC,
					iDrawX, iDrawY, 
					currentImageWidth, currentImageHeight,
					0, 0,
					currentImageWidth, currentImageHeight,
					m_currentImage.GetData(),
					m_currentImage.GetBitmapInfo(),
					DIB_RGB_COLORS, SRCCOPY);
			}

		}
	}
	ReleaseDC(m_hMainDlg, mainDC);

	// ���콺 Ŀ�� ���
	if ( currentImageWidth > currentScreenRect.right ||
		currentImageHeight > currentScreenRect.bottom
		)
	{
		// ���콺 Ŀ���� hand ��
		m_bHandCursor = true;
	}
	else
	{
		// ���콺 Ŀ���� �������
		m_bHandCursor = false;
	}
	PostMessage(m_hMainDlg, WM_SETCURSOR, 0, 0);

	if ( false == ZOption::GetInstance().IsFullScreen() )
	{
		PostMessage(m_hStatusBar, WM_PAINT, 0, 0);
	}
}

void ZMain::ZFindFile(const char *path, std::vector< FileData > & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;

	char fname[_MAX_FNAME] = { 0 };
	BOOL bResult=TRUE;
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		_splitpath(path,drive,dir,NULL,NULL);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' && bFindRecursive == true)
			{
				_snprintf(newpath, sizeof(newpath), "%s%s%s\\*.*",drive,dir,wfd.cFileName);
				ZFindFile(newpath, foundStorage, bFindRecursive);
			}
		}
		else
		{
			_snprintf(fname, sizeof(fname), "%s%s%s",drive,dir,wfd.cFileName);

			if ( ZImage::IsValidImageFileExt(wfd.cFileName) )
			{
				FileData aData;
				aData.m_timeModified = wfd.ftLastWriteTime;
				aData.m_strFileName = fname;
				aData.m_nFileSize = wfd.nFileSizeLow;
				foundStorage.push_back(aData);
			}
		}
		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}

void ZMain::ZFindFolders(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	//memset(&wfd, 0, sizeof(wfd));
	char fname[MAX_PATH] = { 0 };
	BOOL bResult=TRUE;
	char drive[_MAX_DRIVE] = { 0 };
	char dir[MAX_PATH] = { 0 };
	char newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		_splitpath(path,drive,dir,NULL,NULL);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' )
			{
				_snprintf(fname, sizeof(fname), "%s%s%s",drive,dir,wfd.cFileName);

				foundStorage.push_back(fname);

				if ( bFindRecursive == true )
				{
					_snprintf(newpath, sizeof(newpath), "%s%s%s\\*.*",drive,dir,wfd.cFileName);
					ZFindFolders(newpath, foundStorage, bFindRecursive);
				}
			}
		}
		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}


void ZMain::RescanFolder()
{
	std::string strToFindFolder = m_strCurrentFolder;

	strToFindFolder += "*.*";

	_GetFileListAndSort(strToFindFolder, m_vFile);

	// Cache Thread �� �����Ѵ�.
	ZCacheImage::GetInstance().SetImageVector(m_vFile);

	if ( m_strCurrentFilename.empty() && !m_vFile.empty())
	{
		m_strCurrentFilename = m_vFile[0].m_strFileName;
		m_iCurretFileIndex = 0;

		LoadCurrent();
	}
}

void ZMain::SetProgramFolder()
{
	char szGetFileName[FILENAME_MAX] = { 0 };
	DWORD ret = GetModuleFileName(GetModuleHandle(NULL), szGetFileName, FILENAME_MAX);

	if ( ret == 0 )
	{
		_ASSERTE(!"Can't get module folder");
	}
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	_splitpath(szGetFileName, szDrive, szDir, 0, 0);

	m_strProgramFolder = szDrive;
	m_strProgramFolder += szDir;
}

bool ZMain::GetNeighborFolders(std::vector < std::string > & vFolders)
{
	std::string strParentFolder;

	// ���� ������ ���� ������ �˻��Ѵ�.
	{
		// ���� �������� �����ʺ��� 2��°�� \ �� ��´�.
		size_t pos = m_strCurrentFolder.find_last_of("\\");

		if ( pos == m_strCurrentFolder.npos )
		{
			MessageBox(m_hMainDlg, "Can't find parent folder.", "ZViewer", MB_OK);
			return false;
		}

		std::string strParentFolder = m_strCurrentFolder.substr(0, pos);

		pos = strParentFolder.find_last_of("\\");

		if ( pos == strParentFolder.npos )
		{
			MessageBox(m_hMainDlg, "Can't find parent folder.", "ZViewer", MB_OK);
			return false;
		}

		strParentFolder = strParentFolder.substr(0, pos);

		strParentFolder += "\\*.*";
		// ���� ������ ���� �������� ��´�.
		ZFindFolders(strParentFolder.c_str(), vFolders, false);

		if ( vFolders.size() <= 0 )
		{
			_ASSERTE(vFolders.size() > 0 );
			return false;
		}
	}

	// ���� ������ ���� �������� �����Ѵ�.
	if ( m_osKind == eOSKind_XP )
	{
		sort(vFolders.begin(), vFolders.end(), CStringCompareIgnoreCase_LengthFirst());
	}
	else
	{
		sort(vFolders.begin(), vFolders.end(), CStringCompareIgnoreCase());
	}

	return true;
}

void ZMain::NextFolder()
{
	std::vector < std::string > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// ���� ������ ���� ���� �� ���� ������ ���� ������ ������ ���� ������ �˻��� ��, ó�� �̹����� ����.
	{
		int iFoundIndex = -1;
		// ���� ������ index �� �O�´�.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + "\\" == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		_ASSERTE(iFoundIndex != -1);

		if ( (iFoundIndex + 1) >= (int)vFolders.size() )
		{
			// ������ �����̴�.
			MessageBox(m_hMainDlg, "Here is the last folder.", "ZViewer", MB_OK);
			return;
		}
		else
		{
			OpenFolder(vFolders[iFoundIndex+1]);
		}
	}
}


void ZMain::PrevFolder()
{
	std::vector < std::string > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// ���� ������ ���� ���� �� ���� ������ ���� ������ ������ ���� ������ �˻��� ��, ó�� �̹����� ����.
	{
		int iFoundIndex = -1;
		// ���� ������ index �� �O�´�.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + "\\" == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		_ASSERTE(iFoundIndex != -1);

		if ( (iFoundIndex-1 < 0 ) )
		{
			// ������ �����̴�.
			MessageBox(m_hMainDlg, "Here is the first folder.", "ZViewer", MB_OK);
			return;
		}
		else
		{
			OpenFolder(vFolders[iFoundIndex-1]);
		}
	}
}

void ZMain::_GetFileListAndSort(const std::string & strFolderPathAndWildCard, FileListVector & vFileList)
{
	vFileList.resize(0);
	ZFindFile(strFolderPathAndWildCard.c_str(), vFileList, false);

	// ���� ������ �����Ѵ�.
	switch ( m_sortOrder )
	{
	case eFileSortOrder_FILENAME:

		if ( m_osKind == eOSKind_XP )
		{
			sort(vFileList.begin(), vFileList.end(), CFileDataSort_OnlyFilenameCompare_XP());
		}
		else
		{
			sort(vFileList.begin(), vFileList.end(), CFileDataSort_OnlyFilenameCompare());
		}
		break;

	case eFileSortOrder_FILESIZE:
		sort(vFileList.begin(), vFileList.end(), CFileDataSort_FileSize());
		break;

	case eFileSortOrder_LAST_MODIFY_TIME:
		sort(vFileList.begin(), vFileList.end(), CFileDataSort_LastModifiedTime());
		break;

	default:
		_ASSERTE(false);

	}
}

void ZMain::OpenFolder(const std::string strFolder)
{
	// Ư�� ������ ���� ���ϵ��� �˻��ؼ� ���� �� ù��° ������ ����.

	std::string strTemp = strFolder;
	strTemp += "\\*.*";

	vector < FileData > vFiles;

	_GetFileListAndSort(strTemp, vFiles);

	if ( vFiles.size() == 0 )
	{
		std::string strMsg = strFolder;
		strMsg += " folder has no image file.";
		MessageBox(m_hMainDlg, strMsg.c_str(), "ZViewer", MB_OK);
		return;
	}
	else
	{
		OpenFile(vFiles[0].m_strFileName);
	}
}


void ZMain::OpenFile(const string & strFilename)
{
	m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
	RescanFolder();

	// ��ĵ�� ���� �� ���� ������ ã�´�.
	std::vector< FileData >::const_iterator it, endit = m_vFile.end();

	for ( it = m_vFile.begin(); it != endit; ++it)
	{
		if ( it->m_strFileName == strFilename )
		{
			break;
		}
	}

	if ( m_vFile.empty() )
	{
		_ASSERTE(!"size of scanned file list is 0. Check folder name or path!!");

		return;
	}

	_ASSERTE(it != m_vFile.end());	// �� ������ �������� ����.

	if ( it != m_vFile.end() )
	{
		m_iCurretFileIndex = (int)(it - m_vFile.begin());//i;
		m_strCurrentFilename = strFilename;

		LoadCurrent();

		Draw();
	}
}


bool ZMain::MoveIndex(int iIndex)
{
	if ( m_vFile.empty() ) return false;

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_vFile.size() ) iIndex = (int)m_vFile.size() - 1;

	if ( m_iCurretFileIndex == iIndex ) return false;

	m_iCurretFileIndex = iIndex;
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex].m_strFileName;
	LoadCurrent();

	return true;
}

bool ZMain::FirstImage()
{
	// ������ ��ġ�� History �� �����س��´�.
	m_history.push_lastImageIndex(m_iCurretFileIndex);
	return MoveIndex(0);
}

bool ZMain::LastImage()
{
	// ������ ��ġ�� History �� �����س��´�.
	m_history.push_lastImageIndex(m_iCurretFileIndex);
	return MoveIndex((int)m_vFile.size() - 1);
}

void ZMain::OnChangeCurrentSize(int iWidth, int iHeight)
{
	m_iCurrentScreenWidth = iWidth;
	m_iCurrentScreenHeight = iHeight;

	if ( m_iShowingX + iWidth > m_currentImage.GetWidth() )
	{
		m_iShowingX -= (m_iShowingX + iWidth - m_currentImage.GetWidth());

		if ( m_iShowingX < 0 ) m_iShowingX = 0;
	}
	if ( m_iShowingY + iHeight > m_currentImage.GetHeight() )
	{
		m_iShowingY -= (m_iShowingY + iHeight - m_currentImage.GetHeight());
		if ( m_iShowingY < 0 ) m_iShowingY = 0;
	}
}

void ZMain::FormHide()
{
	LONG style = GetWindowLong(m_hMainDlg, GWL_STYLE);
	style &= ~WS_CAPTION;
	style &= ~WS_THICKFRAME;
	ShowWindow(m_hStatusBar, SW_HIDE);
	SetMenu(m_hMainDlg, NULL);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

void ZMain::FormShow()
{
	LONG style = GetWindowLong(m_hMainDlg, GWL_STYLE);
	style |= WS_CAPTION;
	style |= WS_THICKFRAME;
	ShowWindow(m_hStatusBar, SW_SHOW);
	SetMenu(m_hMainDlg, m_hMainMenu);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

/// Cache status �� ���� ǥ���ٿ� ǥ���Ѵ�.
void ZMain::showCacheStatus()
{
	// �ػ� ����
	//_snprintf(szTemp, sizeof(szTemp), "");
	//SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

	static bool bLastActionIsCache = false;

	bool bNowActionIsCache = ZCacheImage::GetInstance().isCachingNow();

	if ( bLastActionIsCache != bNowActionIsCache )
	{
		bLastActionIsCache = bNowActionIsCache;
		
		if ( bNowActionIsCache )
		{
			PostMessage(m_hStatusBar, SB_SETTEXT, 5, (LPARAM)"Caching");
		}
		else
		{
			PostMessage(m_hStatusBar, SB_SETTEXT, 5, (LPARAM)"Cached");
		}

		/*
		HDC hDC = ::GetDC(m_hStatusBar);

		enum
		{
			eCacheShowWindowSize = 20
		};
		RECT drawRect;
		drawRect.left  = 503;
		drawRect.right = drawRect.left + eCacheShowWindowSize;
		drawRect.top = 3;
		drawRect.bottom = 19;

		static int percentage = 0;

		if ( ZCacheImage::GetInstance().isCachingNow() )
		{
			++percentage;

			if ( percentage > 100 ) percentage = 0;

			drawRect.right = drawRect.left + (eCacheShowWindowSize * ( percentage / 100.0 ));
		}

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = RGB(100, 200, 200);
		logBrush.lbHatch = 0;
		HBRUSH hBrush = CreateBrushIndirect(&logBrush);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		Rectangle(hDC, drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
		*/
	}
}


void ZMain::ToggleFullScreen()
{
	static RECT lastPosition;

	if ( ZOption::GetInstance().IsFullScreen() )	// ���� Ǯ��ũ���̸� ���� ȭ������ ���ư���.
	{
		ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());

		ShellTrayShow();	// �������� �۾� ǥ������ �����ش�.

		FormShow();	// �޴�, ���� ǥ���ٵ��� �����ش�.

		SetWindowPos(m_hMainDlg, HWND_TOP, lastPosition.left, lastPosition.top, lastPosition.right - lastPosition.left, lastPosition.bottom - lastPosition.top, SWP_SHOWWINDOW);
	}
	else	// ���� Ǯ��ũ���� �ƴϸ� Ǯ��ũ������ �����.
	{
		ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());
		// ���� ũ�⸦ ����Ѵ�.
#pragma message("TODO: Maximized �Ǿ��� �� ó��")
		GetWindowRect(m_hMainDlg, &lastPosition);

		FormHide();// �޴�, ���� ǥ���ٵ��� �����.

		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);

		// �۾� ǥ������ �����ش�.
		ShellTrayHide();

		// ��Ŀ���� ������ ������� ���ư����ϹǷ� Ǯ����´�.
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		//MoveWindow(m_hMainDlg, )
		//m_iRestoreX = 
	}

	CheckMenuItem(m_hMainMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);

	m_iShowingX = 0;
	m_iShowingY = 0;
}


void ZMain::ToggleSmallToScreenStretch()
{
	ZOption::GetInstance().m_bSmallToBigStretchImage = ! ZOption::GetInstance().m_bSmallToBigStretchImage;

	CheckMenuItem(m_hMainMenu, ID_VIEW_SMALLTOSCREENSTRETCH, ZOption::GetInstance().m_bSmallToBigStretchImage ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_SMALLTOSCREENSTRETCH, ZOption::GetInstance().m_bSmallToBigStretchImage ? MF_CHECKED : MF_UNCHECKED);

	ZCacheImage::GetInstance().clearCache();
	ZCacheImage::GetInstance().setCacheEvent();

	LoadCurrent();
	Draw();

}

void ZMain::ToggleBigToScreenStretch()
{
	ZOption::GetInstance().m_bBigToSmallStretchImage = !ZOption::GetInstance().m_bBigToSmallStretchImage;

	CheckMenuItem(m_hMainMenu, ID_VIEW_BIGTOSCREENSTRETCH , ZOption::GetInstance().m_bBigToSmallStretchImage ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_BIGTOSCREENSTRETCH, ZOption::GetInstance().m_bBigToSmallStretchImage ? MF_CHECKED : MF_UNCHECKED);

	ZCacheImage::GetInstance().clearCache();
	ZCacheImage::GetInstance().setCacheEvent();

	LoadCurrent();
	Draw();

}

void ZMain::SetStatusBarText()
{
	char szTemp[256];

	if ( m_vFile.size() == 0 || m_strCurrentFilename.empty() ) // ���� �ִ� ������ ������
	{
		// File Index
		_snprintf(szTemp, sizeof(szTemp), "No File");
		SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)szTemp);

		// �ػ� ����
		_snprintf(szTemp, sizeof(szTemp), "");
		SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

		// �̹��� ������
		_snprintf(szTemp, sizeof(szTemp), "");
		SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)szTemp);

		// �ӽ÷� http://wimy.com
		_snprintf(szTemp, sizeof(szTemp), "http://wimy.com");
		SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)szTemp);

		// �ε��ð�
		_snprintf(szTemp, sizeof(szTemp), "");
		SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)szTemp);

		// ���ϸ�
		_snprintf(szTemp, sizeof(szTemp), "No File");

		showCacheStatus(); ///< 5

		SendMessage(m_hStatusBar, SB_SETTEXT, 6, (LPARAM)szTemp);
	}
	else
	{
		// File Index
		_snprintf(szTemp, sizeof(szTemp), "%d/%d", m_iCurretFileIndex+1, m_vFile.size());
		SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)szTemp);

		// �ػ� ����
		_snprintf(szTemp, sizeof(szTemp), "%dx%dx%dbpp", m_currentImage.GetOriginalWidth(), m_currentImage.GetOriginalHeight(), m_currentImage.GetBPP());
		SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

		// image size
		long imageSize = m_vFile[m_iCurretFileIndex].m_nFileSize;

		if ( imageSize > 1024 )
		{
			if ( imageSize/1024 > 1024 )
			{
				_snprintf(szTemp, sizeof(szTemp), "%.2fMByte", imageSize/1024/1024.0f);
			}
			else
			{
				_snprintf(szTemp, sizeof(szTemp), "%dKByte", imageSize/1024);
			}
		}
		else
		{
			_snprintf(szTemp, sizeof(szTemp), "%dByte", imageSize);
		}
		SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)szTemp);

		// �ӽ÷� http://wimy.com
		_snprintf(szTemp, sizeof(szTemp), "http://wimy.com");
		SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)szTemp);

		// �ε��ð�
		if ( m_bLastCacheHit )
		{
			_snprintf(szTemp, sizeof(szTemp), "%.3fsec [C]", (float)(m_dwLoadingTime / 1000.0));
		}
		else
		{
			_snprintf(szTemp, sizeof(szTemp), "%.3fsec [N]", (float)(m_dwLoadingTime / 1000.0));
		}
		SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)szTemp);

		// ���ϸ�
		char szFilename[MAX_PATH], szFileExt[MAX_PATH];
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFilename, szFileExt);

		showCacheStatus(); ///< 5

		_snprintf(szTemp, sizeof(szTemp), "%s%s", szFilename, szFileExt);
		SendMessage(m_hStatusBar, SB_SETTEXT, 6, (LPARAM)szTemp);
	}
}

void ZMain::SetTitle()
{
	char szTemp[MAX_PATH+256];

	if ( m_strCurrentFilename.empty() )	// ���纸�� �ִ� ���ϸ��� ������
	{
		_snprintf(szTemp, sizeof(szTemp), "ZViewer v%s", g_strVersion.c_str());
	}
	else // ���纸�� �ִ� ���ϸ��� ������
	{
		char szFileName[MAX_PATH] = { 0 };
		char szFileExt[MAX_PATH] = { 0 };
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFileName, szFileExt);

		_snprintf(szTemp, sizeof(szTemp), "%s%s - %s", szFileName, szFileExt, m_strCurrentFilename.c_str());
	}
	SetWindowText(m_hMainDlg, szTemp);
}

void ZMain::LoadCurrent()
{
	static bool bFirst = true;

	if ( bFirst )
	{
		bFirst = false;

		ZCacheImage::GetInstance().SetImageVector(m_vFile);
		ZCacheImage::GetInstance().StartThread();
	}

	DWORD start = GetTickCount();
	
	if ( ZCacheImage::GetInstance().hasCachedData(m_strCurrentFilename, m_iCurretFileIndex) )
	{
		{
			ZCacheImage::GetInstance().getCachedData(m_strCurrentFilename, m_currentImage);
		}

		DebugPrintf("Cache Hit!!!!!!!!!!!!!");

		m_bLastCacheHit = true;

		ZCacheImage::GetInstance().LogCacheHit();
	}
	else
	{
		// ĳ�ÿ��� ã�� �� ������ ���� �о���̰�, ĳ�ÿ� �߰��Ѵ�.
		DebugPrintf("Can't find in cache. load now...");

		bool bLoadOK = false;

		for ( unsigned int i=0; i<10; ++i)
		{
			bLoadOK = m_currentImage.LoadFromFile(m_strCurrentFilename);
			if ( bLoadOK || i >= 5) break;

			DebugPrintf("Direct Load failed. sleep");

			Sleep(10);
		}

		if ( bLoadOK == false )
		{
			_ASSERTE(!"Can't load image");

			string strErrorFilename = m_strProgramFolder;
			strErrorFilename += "LoadError.png";
			if ( !m_currentImage.LoadFromFile(strErrorFilename) )
			{
				// ���� �� ǥ���ϴ� ������ �о������ �� ������
				MessageBox(m_hMainDlg, "Can't load Error Image file.", "ZViewer", MB_OK);
			}
		}
		else
		{
			DebugPrintf("Cache miss.");
			m_bLastCacheHit = false;
			ZCacheImage::GetInstance().LogCacheMiss();
		}
	}
	m_dwLoadingTime = GetTickCount() - start;
	SetTitle();	// ���ϸ��� ������ Ÿ��Ʋ�ٿ� ���´�.
	SetStatusBarText();

	m_iShowingX = 0;
	m_iShowingY = 0;

	if ( ZOption::GetInstance().m_bRightTopFirstDraw )	// ���� ��ܺ��� �����ؾ��ϸ�
	{
		RECT rt;
		getCurrentScreenRect(rt);

		if ( m_currentImage.GetWidth() > rt.right )
		{
			m_iShowingX = m_currentImage.GetWidth() - rt.right;
		}
	}
}

void ZMain::OnDrag(int x, int y)
{
	if ( !m_currentImage.IsValid()) return;

	if ( ZOption::GetInstance().m_bBigToSmallStretchImage )
	{
		/// ū �׸��� ȭ�鿡 �°� ��Ʈ��Ī�� ���� �巡�״� ���� �ʾƵ� �ȴ�.
		return;
	}

	RECT rt;
	getCurrentScreenRect(rt);

	int iNowShowingX = m_iShowingX;
	int iNowShowingY = m_iShowingY;

	if ( (m_iShowingX + x) >= 0 ) 
	{
		if ( m_iShowingX + x + rt.right >= m_currentImage.GetWidth())
		{
			x = m_currentImage.GetWidth() - rt.right - m_iShowingX;// - 1;
		}
		m_iShowingX += x;
	}
	else
	{
		m_iShowingX = 0;
	}

	if ( ( m_iShowingY + y )  >= 0 ) 
	{
		if ( m_iShowingY + y + rt.bottom >= m_currentImage.GetHeight())
		{
			y = m_currentImage.GetHeight() - rt.bottom - m_iShowingY;// - 1;
		}
		m_iShowingY += y;
	}
	else
	{
		m_iShowingY = 0;
	}

	if ( m_iShowingX != iNowShowingX || m_iShowingY != iNowShowingY )
	{
		Draw(false);
	}
}

/// �۾� ǥ������ ���̰� ���ش�.
void ZMain::ShellTrayShow()
{
	/// �۾� ǥ������ ���̰� ���ش�.
	HWND h = FindWindow("Shell_TrayWnd", "");

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_SHOW);
	}
}


void ZMain::ChangeFileSort(eFileSortOrder sortOrder)
{
	m_sortOrder = sortOrder;
	ReLoadFileList();
}

void ZMain::ReLoadFileList()
{
	std::string strFileName = m_strCurrentFilename;
	RescanFolder();

	OpenFile(strFileName);
}

void ZMain::ShellTrayHide()
{
	// �۾� ǥ������ ���̰� ���ش�.
	HWND h = FindWindow("Shell_TrayWnd", "");

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_HIDE);
	}
}


void ZMain::_ProcAfterRemoveThisFile()
{
	// ���� ������ ������ �����ΰ�?
	if ( m_vFile.size() <= 1 )
	{
		m_iCurretFileIndex = 0;
		m_strCurrentFilename = "";

		m_vFile.resize(0);

		SetTitle();
		SetStatusBarText();
		Draw(true);


	}
	else
	{
		if ( ((int)m_vFile.size() - 1) > m_iCurretFileIndex )	// ���� �׸��� �ִ�.
		{
			// for ���� ���鼭 ���� ���� ã�Ƴ��´�.
			vector< FileData >::iterator it, endit = m_vFile.end();
			int i = 0;
			bool bFound = false;
			for ( it = m_vFile.begin(); it != endit; ++it)
			{
				if ( i == m_iCurretFileIndex)
				{
					bFound = true;
					break;
				}
				++i;
			}
			if ( !bFound )
			{
				_ASSERTE(!"Can't find the file");
				return;
			}
			NextImage();

			m_vFile.erase(it);

			// �������Ƿ� ���� �ε����� 1���δ�.
			m_iCurretFileIndex -= 1;

			SetTitle();
			SetStatusBarText();

			Draw(true);
		}
		else
		{
			// ����� 1�� �ƴϰ�, �������� �������Ƿ� ���� ���� �ִ�.
			// for ���� ���鼭 ���� ���� ã�Ƴ��´�.
			vector< FileData >::iterator it, endit = m_vFile.end();
			int i = 0;

			bool bFound = false;
			for ( it = m_vFile.begin(); it != endit; ++it)
			{
				if ( i == m_iCurretFileIndex)
				{
					bFound = true;
					break;
				}
				++i;
			}

			if ( !bFound )
			{
				_ASSERTE(!"Can't find the file");
				return;
			}
			PrevImage();

			m_vFile.erase(it);
			SetTitle();
			SetStatusBarText();
			Draw(true);
		}


	}
}


void ZMain::OnFocusLose()
{
	//DebugPrintf("OnFocusLose()");

	ShellTrayShow();
	/*
//	if ( m_bOpeningFileDialog )
	{
		m_bOpeningFileDialog = false;
	}
//	else
	{
	}
	*/
}

void ZMain::OnFocusGet()
{
	//DebugPrintf("OnFocusGet()");
	if ( ZOption::GetInstance().IsFullScreen() )
	{
		DebugPrintf("OnFocusGet() at fullscreen");
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		ShellTrayHide();
	}
}

void ZMain::Undo()
{
	if ( m_history.CanUndo() )
	{
		int iLast = m_history.Undo();

		// Ȥ�ó� ������ ����� �ȵ�
		if ( iLast < 0 || iLast >= (int)m_vFile.size() )
		{
			_ASSERTE(!"Over range...");
			return;
		}

		if ( m_vFile.empty() ) return;

		m_iCurretFileIndex = iLast;
		m_strCurrentFilename = m_vFile[m_iCurretFileIndex].m_strFileName;
		LoadCurrent();
	}
}

void ZMain::Redo()
{
	if ( m_history.CanRedo() )
	{
		m_history.Redo();
	}
}

void ZMain::OnRightTopFirstDraw()
{
	ZOption::GetInstance().m_bRightTopFirstDraw = !ZOption::GetInstance().m_bRightTopFirstDraw;

	CheckMenuItem(m_hMainMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
}

void ZMain::ShowFileExtDlg()
{
	ZFileExtDlg::GetInstance().ShowDlg();
}

void ZMain::DeleteThisFile()
{
	char szDeleteMsg[256];

	_snprintf(szDeleteMsg, sizeof(szDeleteMsg), ZResourceManager::GetInstance().GetString(IDS_DELETE_THIS_FILE).c_str(), GetFileNameFromFullFileName(m_strCurrentFilename).c_str());
	int iRet = MessageBox(m_hMainDlg, szDeleteMsg, "ZViewer", MB_YESNO);

	if ( iRet == IDYES )
	{
		if ( 0 == unlink(m_strCurrentFilename.c_str()) )
		{
			_ProcAfterRemoveThisFile();
		}
		else
		{
			MessageBox(m_hMainDlg, "Can't delete this file!", "ZViewer", MB_OK);
		}
	}
}


void ZMain::MoveThisFile()
{

	CMoveToDlg aDlg;
	
	if ( !aDlg.DoModal() )
	{
		return;
	}

	std::string strFolder = aDlg.GetMoveToFolder();

	std::string filename = GetFileNameFromFullFileName(m_strCurrentFilename);
	std::string strToFileName = aDlg.GetMoveToFolder();
	strToFileName += "\\";
	strToFileName += filename;

	// �Űܰ� ������ ���� ������ �ִ��� Ȯ���Ѵ�.
	if ( 0 != _access(aDlg.GetMoveToFolder().c_str(), 00) )
	{
		MessageBox(m_hMainDlg, "Wrong folder name", "ZViewer", MB_OK);
		return;
	}

	// ���� ������ �����ϴ��� Ȯ���Ѵ�.
	if ( 0 == _access(strToFileName.c_str(), 00) )
	{
		// �̹� �����ϸ�
		if ( IDNO == MessageBox(m_hMainDlg, "There is a file same name. Overwrite?", "ZViewer", MB_YESNO) )
		{
			return;
		}

	}
	
	MoveFileEx(m_strCurrentFilename.c_str(), strToFileName.c_str(), MOVEFILE_REPLACE_EXISTING);
	_ProcAfterRemoveThisFile();
}

void ZMain::Rotate(bool bClockWise)
{
	if ( m_currentImage.IsValid() )
	{
		if ( bClockWise )
		{
			m_currentImage.Rotate(-90);
		}
		else
		{
			m_currentImage.Rotate(90);
		}
		Draw(true);
	}
}

void ZMain::SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style)
{
	// ���纸�� �ִ� ������ ������ ������ �����Ѵ�.
	char szSystemFolder[_MAX_PATH] = { 0 };

	if ( E_FAIL == SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, szSystemFolder) )
	{
		_ASSERTE(false);
		return;
	}

	char szFileName[MAX_PATH] = { 0 };
	_splitpath(m_vFile[m_iCurretFileIndex].m_strFileName.c_str(), 0, 0, szFileName, 0);

	std::string strSaveFileName = szSystemFolder;
	strSaveFileName += "\\zviewer_bg_";
	strSaveFileName += szFileName;
	strSaveFileName += ".bmp";

	if ( FALSE == m_currentImage.SaveToFile(strSaveFileName, BMP_DEFAULT) )
	{
		_ASSERTE(false);
		return;
	}

	CDesktopWallPaper::SetDesktopWallPaper(strSaveFileName, style);
}


/// ���� ȭ���� �׸� �� �ִ� ������ ũ�⸦ �޾ƿ´�.
void ZMain::getCurrentScreenRect(RECT & rect)
{
	_ASSERTE(m_hMainDlg);
	GetClientRect(m_hMainDlg, &rect);
	if ( ZOption::GetInstance().IsFullScreen() == false ) rect.bottom -= STATUSBAR_HEIGHT;
}

/// On Window is resized
void ZMain::OnWindowResized()
{
	RECT rt;
	getCurrentScreenRect(rt);

	OnChangeCurrentSize(rt.right, rt.bottom);

	if ( ZOption::GetInstance().m_bBigToSmallStretchImage || ZOption::GetInstance().m_bSmallToBigStretchImage )
	{
		ZCacheImage::GetInstance().clearCache();
		ZCacheImage::GetInstance().setCacheEvent();
		LoadCurrent();
	}
}