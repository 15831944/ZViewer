﻿/********************************************************************
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
#include "../commonSrc/DesktopWallPaper.h"
#include "src/SelectToFolderDlg.h"
#include "../commonSrc/SaveAs.h"
#include "../commonSrc/ZOption.h"
#include "../commonSrc/CommonFunc.h"
#include "../commonSrc/ExtInfoManager.h"
#include "MessageManager.h"
#include "TaskBar.h"

#include <ShlObj.h>
#include <cstdio>

#include "src/ExifDlg.h"

#include "resource.h"

using namespace std;

enum
{
	eTimerValue = 9153
};

ZMain & ZMain::GetInstance()
{
	static ZMain aInstance;
	return aInstance;
}


ZMain::ZMain(void)
:	m_hMainDlg(NULL)
,	m_sortOrder(eFileSortOrder_FILENAME)
,	m_osKind(eOSKind_UNKNOWN)
,	m_alpha(255)
{
	memset( &m_lastPosition, 0, sizeof( m_lastPosition ) );
	m_hBufferDC = NULL;
//	m_bLastCacheHit = false;

	m_bCurrentImageLoaded = false;
	m_pCurrentImage = NULL;
	m_bgBrush = CreateSolidBrush(RGB(128,128,128));
	m_hEraseRgb = CreateRectRgn(0,0,1,1);

	if ( INVALID_HANDLE_VALUE == m_bgBrush )
	{
		assert(false);
	}
}

ZMain::~ZMain(void)
{
	if ( NULL != m_hBufferDC )
	{
		DebugPrintf(TEXT("Before delete bufferDC"));
		BOOL bRet = DeleteDC(m_hBufferDC);

		DebugPrintf(TEXT("after delete bufferDC"));

		if ( bRet == FALSE )
		{
			assert(bRet);
		}

		m_hBufferDC = NULL;
	}

	if ( m_bgBrush != INVALID_HANDLE_VALUE )
	{
		DeleteObject(m_bgBrush);
	}
}


/// 버퍼로 쓰이는 DC 를 릴리즈한다.
void ZMain::_releaseBufferDC()
{
	if ( m_hBufferDC != NULL )
	{
		BOOL bRet = DeleteDC(m_hBufferDC);

		if ( bRet == FALSE )
		{
			assert(bRet);
		}
		m_hBufferDC = NULL;
	}
}

/// Timer 를 받았을 때
void ZMain::onTimer()
{
	if ( ZOption::GetInstance().m_bSlideMode )
	{/// check slidemode
		DWORD dwNow = GetTickCount();

		if ( dwNow - ZOption::GetInstance().m_dwLastSlidedTime > ZOption::GetInstance().m_iSlideModePeriodMiliSeconds )
		{
			NextImage();
			Draw();
			ZOption::GetInstance().m_dwLastSlidedTime = dwNow;
		}
	}

	if ( ZCacheImage::GetInstance().isCachingNow() )
	{
		//DebugPrintf("now cache status...");
	}

	if ( false == m_bCurrentImageLoaded )
	{
		LoadCurrent();
	}

	ShowCacheStatus();
}


/// ZViewer 전용 메시지 박스
int ZMain::ShowMessageBox(const TCHAR * msg, UINT button)
{
	return ::MessageBox(m_hMainDlg, msg, TEXT("ZViewer"), button);
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
	ofn.lpstrFilter = ExtInfoManager::GetInstance().GetFileDlgFilter();

	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = m_strCurrentFolder.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLEHOOK | OFN_EXPLORER;
	ofn.lpfnHook = (LPOFNHOOKPROC)CenterOFNHookProc;
}

void ZMain::OpenFileDialog()
{
	// 현재 폴더를 세팅해 준다.
	ofn.lpstrInitialDir = m_strCurrentFolder.c_str();

	//m_bOpeningFileDialog = true;

	// Display the Open dialog box.
	if (GetOpenFileName(&ofn)==TRUE)
	{
		OpenFile(ofn.lpstrFile);
	}
}

/// 현재 파일을 다른 형식으로 저장하는 파일 다이얼로그를 연다.
void ZMain::SaveFileDialog()
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		return;
	}
	CSaveAs::getInstance().setParentHWND(m_hMainDlg);
	CSaveAs::getInstance().setDefaultSaveFilename(m_strCurrentFolder, m_strCurrentFilename);

	if ( true == CSaveAs::getInstance().showDialog() )
	{
		tstring strSaveFilename = CSaveAs::getInstance().getSaveFileName();

		if ( false == m_pCurrentImage->SaveToFile(strSaveFilename, 0) )
		{
			ShowMessageBox(GetMessage(TEXT("CANNOT_SAVE_AS_FILE")));
			ShowMessageBox(GetMessage(TEXT("CANNOT_SAVE_AS_FILE_TRANS_ERROR")));
		}
	}
}

void ZMain::OnInit()
{
	/// 불러온 최종 옵션을 점검하여 메뉴 중 체크표시할 것들을 표시한다.
	SetCheckMenus();

	if ( m_strInitArg.empty() )
	{
		RescanFolder();
	}
	else
	{
		// 시작 인자가 있으면 그 파일을 보여준다.
		OpenFile(m_strInitArg);
	}
}

/// 현재 보고 있는 파일의 Exif 정보를 보여준다.
void ZMain::ShowExif()
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		assert(m_pCurrentImage);
		return;
	}
	ExifDlg aDlg;
	aDlg.Create(m_hMainInstance, m_hMainDlg, SW_SHOW, TEXT("ExifDlg"), NULL);
	aDlg.MakeExifMap(*m_pCurrentImage);
	aDlg.DoResource(m_hMainDlg);
}

void ZMain::Draw(HDC toDrawDC, bool bEraseBg)
{
	TIMECHECK_START("StartDraw");

	HDC mainDC;

	if ( NULL == toDrawDC )
	{
		mainDC = GetDC(m_hShowWindow);
	}
	else
	{
		/// 무효화된 부분만 그릴 때의 DC 를 얻어오기 위해
		mainDC = toDrawDC;
	}

	if ( NULL == mainDC )
	{
		assert(mainDC != NULL);
		return;
	}

	RECT currentScreenRect;
	if ( false == getCurrentScreenRect(currentScreenRect) )
	{
		assert(false);
		return;
	}

	/// 파일이 하나도 없을 때는 배경만 지우고 바로 리턴한다.
	if ( m_vFile.size() <= 0 )
	{
		_eraseBackground(mainDC, currentScreenRect.right, currentScreenRect.bottom);

		if ( NULL == toDrawDC )
		{
			ReleaseDC(m_hMainDlg, mainDC);
		}

		return;
	}

	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		//assert(m_pCurrentImage);
		return;
	}

	DebugPrintf(TEXT("ZMain::Draw()"));
	if ( m_pCurrentImage->IsValid() == FALSE ) return;
	if ( m_hMainDlg == NULL ) return;

	/// ShowWindow 부분만 그리도록 클리핑 영역을 설정한다.
	HRGN hrgn = CreateRectRgn(currentScreenRect.left, currentScreenRect.top, currentScreenRect.right, currentScreenRect.bottom);
	SelectClipRgn(mainDC, hrgn);

	const int zoomedImageWidth = (int)(m_pCurrentImage->GetWidth() * m_fCurrentZoomRate);
	const int zoomedImageHeight = (int)(m_pCurrentImage->GetHeight() * m_fCurrentZoomRate);

	/// 그릴 그림이 현재 화면보다, 가로 & 세로 모두 크면 배경을 지우지 않아도 됨.
	if ( zoomedImageWidth >= currentScreenRect.right && zoomedImageHeight >= currentScreenRect.bottom )
	{
		bEraseBg = false;
	}

	if ( zoomedImageWidth <= currentScreenRect.right )
	{
		// 가로 길이가 화면보다 작을 때는 중앙에 오게
		m_iShowingX = -((currentScreenRect.right/2) - (zoomedImageWidth/2));
	}

	if ( zoomedImageHeight <= currentScreenRect.bottom )
	{
		// 세로 길이가 화면보다 작을 때는 중앙에 오게
		m_iShowingY = -((currentScreenRect.bottom/2) - (zoomedImageHeight/2));
	}

	RECT drawRect = { -m_iShowingX, -m_iShowingY, zoomedImageWidth-m_iShowingX, zoomedImageHeight-m_iShowingY };

	if ( bEraseBg )	// 배경을 지워야 하면 지운다. 그림이 그려지는 부분을 제외하고 지운다.
	{
		HRGN hScreenRGN = CreateRectRgn(currentScreenRect.left, currentScreenRect.top, currentScreenRect.right, currentScreenRect.bottom);
		HRGN hDrawRGN = CreateRectRgn(drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
		CombineRgn(m_hEraseRgb, hScreenRGN, hDrawRGN, RGN_DIFF);

		FillRgn(mainDC, m_hEraseRgb, m_bgBrush);

		DeleteObject(hScreenRGN);
		DeleteObject(hDrawRGN);
	}

	/// 추후 확대/축소 후의 화면 위치등을 위해서
	m_fCenterX = (float)((currentScreenRect.right/2.0f) - (float)drawRect.left) / (float)(drawRect.right-drawRect.left);
	m_fCenterY = (float)((currentScreenRect.bottom/2.0f) - (float)drawRect.top) / (float)(drawRect.bottom-drawRect.top);

	//DebugPrintf(TEXT("center(%f,%f) on Draw()"), m_fCenterX, m_fCenterY);

#ifdef _DEBUG
//	DWORD dwStart = GetTickCount();
#endif
	m_pCurrentImage->Draw(mainDC, drawRect);
#ifdef _DEBUG
//	DWORD dwDiff = GetTickCount() - dwStart;
	//DebugPrintf(TEXT("freeImagePlus.Draw spend time : %d"), dwDiff);
#endif

	ReleaseDC(m_hMainDlg, mainDC);

	// 마우스 커서 모양
	if ( zoomedImageWidth > currentScreenRect.right || zoomedImageHeight > currentScreenRect.bottom )
	{
		// 마우스 커서를 hand 로
		SetHandCursor(true);
	}
	else
	{
		// 마우스 커서를 원래대로
		SetHandCursor(false);
	}
	PostMessage(m_hMainDlg, WM_SETCURSOR, 0, 0);

	// 풀 스크린이 아닐 때는 상태 표시줄을 다시 그린다.
	if ( false == ZOption::GetInstance().IsFullScreen() )
	{
		PostMessage(m_hStatusBar, WM_PAINT, 0, 0);
	}

	DeleteObject(hrgn);
	if ( NULL == toDrawDC )
	{
		ReleaseDC(m_hMainDlg, mainDC);
	}
	TIMECHECK_END();
}

void ZMain::FindFile(const TCHAR * path, std::vector< FileData > & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;

	BOOL bResult=TRUE;
	TCHAR drive[_MAX_DRIVE] = { 0 };
	TCHAR dir[_MAX_DIR] = { 0 };
	TCHAR fname[_MAX_FNAME] = { 0 };
	TCHAR newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		SplitPath(path, drive,_MAX_DRIVE ,dir,_MAX_DIR, NULL,0 ,NULL,0);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' && bFindRecursive == true)
			{
				SPrintf(newpath, MAX_PATH, L"%s%s%s\\*.*",drive,dir,wfd.cFileName);
				FindFile(newpath, foundStorage, bFindRecursive);
			}
		}
		else
		{
			SPrintf(fname, _MAX_FNAME, L"%s%s%s",drive,dir,wfd.cFileName);

			if ( ExtInfoManager::GetInstance().IsValidImageFileExt(wfd.cFileName) )
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

void ZMain::FindFolders(const TCHAR *path, std::vector<tstring> & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	//memset(&wfd, 0, sizeof(wfd));
	TCHAR fname[MAX_PATH] = { 0 };
	BOOL bResult=TRUE;
	TCHAR drive[_MAX_DRIVE] = { 0 };
	TCHAR dir[_MAX_DIR] = { 0 };
	TCHAR newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		SplitPath(path, drive,_MAX_DRIVE, dir,_MAX_DIR, NULL,0, NULL,0);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' )
			{
				SPrintf(fname, MAX_PATH, L"%s%s%s",drive,dir,wfd.cFileName);

				foundStorage.push_back(fname);

				if ( bFindRecursive == true )
				{
					SPrintf(newpath, MAX_PATH, L"%s%s%s\\*.*",drive,dir,wfd.cFileName);
					FindFolders(newpath, foundStorage, bFindRecursive);
				}
			}
		}
		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}


void ZMain::RescanFolder()
{
	tstring strToFindFolder = m_strCurrentFolder;

	strToFindFolder += TEXT("*.*");

	TIMECHECK_START("--- rescan folder");
	_GetFileListAndSort(strToFindFolder, m_vFile);
	TIMECHECK_END();

	// Cache Thread 에 전달한다.
	ZCacheImage::GetInstance().SetImageVector(m_vFile);

	if ( m_strCurrentFilename.empty() && !m_vFile.empty())
	{
		m_strCurrentFilename = m_vFile[0].m_strFileName;
		m_iCurretFileIndex = 0;

		LoadCurrent();
	}
}

bool ZMain::GetNeighborFolders(std::vector < tstring > & vFolders)
{
	// 현재 폴더의 상위 폴더를 검색한다.
	{
		// 현재 폴더에서 오른쪽부터 2번째의 \ 를 얻는다.
		size_t pos = m_strCurrentFolder.find_last_of(TEXT("\\"));

		if ( pos == m_strCurrentFolder.npos )
		{
			ShowMessageBox(GetMessage(TEXT("CANNOT_FIND_PARENT_DIRECTORY")));
			return false;
		}

		tstring strParentFolder = m_strCurrentFolder.substr(0, pos);

		pos = strParentFolder.find_last_of(TEXT("\\"));

		if ( pos == strParentFolder.npos )
		{
			ShowMessageBox(GetMessage(TEXT("CANNOT_FIND_PARENT_DIRECTORY")));
			return false;
		}

		strParentFolder = strParentFolder.substr(0, pos);

		strParentFolder += TEXT("\\*.*");
		// 상위 폴더의 하위 폴더들을 얻는다.
		FindFolders(strParentFolder.c_str(), vFolders, false);

		if ( vFolders.size() <= 0 )
		{
			assert(vFolders.size() > 0 );
			return false;
		}
	}

	// 상위 폴더의 하위 폴더들을 정렬한다.
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
	std::vector < tstring > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
	{
		int iFoundIndex = -1;
		// 현재 폴더의 index 를 찿는다.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + TEXT("\\") == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		assert(iFoundIndex != -1);

		if ( (iFoundIndex + 1) >= (int)vFolders.size() )
		{
			// 마지막 폴더이다.
			ShowMessageBox(GetMessage(TEXT("LAST_DIRECTORY")));
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
	std::vector < tstring > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
	{
		int iFoundIndex = -1;
		// 현재 폴더의 index 를 찿는다.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + TEXT("\\") == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		assert(iFoundIndex != -1);

		if ( (iFoundIndex-1 < 0 ) )
		{
			// 마지막 폴더이다.
			ShowMessageBox(GetMessage(TEXT("FIRST_FOLDER")));
			return;
		}
		else
		{
			OpenFolder(vFolders[iFoundIndex-1]);
		}
	}
}

void ZMain::_GetFileListAndSort(const tstring & strFolderPathAndWildCard, FileListVector & vFileList)
{
	vFileList.resize(0);
	FindFile(strFolderPathAndWildCard.c_str(), vFileList, false);

	// 얻은 파일을 정렬한다.
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
		assert(false);

	}
}

void ZMain::OpenFolder(const tstring & strFolder)
{
	// 특정 폴더의 하위 파일들을 검색해서 정렬 후 첫번째 파일을 연다.

	tstring strTemp = strFolder;
	strTemp += TEXT("\\*.*");

	vector < FileData > vFiles;

	_GetFileListAndSort(strTemp, vFiles);

	if ( vFiles.size() == 0 )
	{
		TCHAR msg[COMMON_BUFFER_SIZE];
		SPrintf(msg, COMMON_BUFFER_SIZE, GetMessage(TEXT("THIS_DIRECTORY_HAS_NO_IMAGE")), strFolder.c_str());
		::MessageBox(m_hMainDlg, msg, TEXT("ZViewer"), MB_OK);
		return;
	}
	else
	{
		OpenFile(vFiles[0].m_strFileName);
	}
}


void ZMain::OpenFile(const tstring & strFilename)
{
	m_bCurrentImageLoaded = false;
	m_pCurrentImage = NULL;

	m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
	RescanFolder();

	// 스캔한 파일 중 현재 파일을 찾는다.
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
		assert(!"size of scanned file list is 0. Check folder name or path!!");

		return;
	}

	assert(it != m_vFile.end());	// 그 파일이 없을리가 없다.

	if ( it != m_vFile.end() )
	{
		m_iCurretFileIndex = (int)(it - m_vFile.begin());//i;
		m_strCurrentFilename = strFilename;

		LoadCurrent();
	}
}


bool ZMain::MoveIndex(int iIndex)
{
	if ( m_vFile.empty() ) return false;

	iIndex = GetCalculatedMovedIndex(iIndex);

	if ( m_iCurretFileIndex == iIndex ) return false;

	m_iCurretFileIndex = iIndex;
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex].m_strFileName;
	m_bCurrentImageLoaded = false;
	m_pCurrentImage = NULL;
	LoadCurrent();

	return true;
}

/// 새로 이동할 인덱스 번호를 주면 지금 상황에 맞는 인덱스 번호를 돌려준다.
int ZMain::GetCalculatedMovedIndex(int iIndex)
{
	if ( iIndex < 0 )
	{
		if ( ZOption::GetInstance().IsLoopImages() )
		{
			iIndex = (int)(m_vFile.size() - ((-1*iIndex) % m_vFile.size()));
		}
		else
		{
			iIndex = 0;
		}
	}

	if ( iIndex >= (int)m_vFile.size() )
	{
		if ( ZOption::GetInstance().IsLoopImages() || ZOption::GetInstance().m_bSlideMode )
		{
			iIndex = (int)( iIndex % m_vFile.size() );
		}
		else
		{
			iIndex = (int)m_vFile.size() - 1;
		}
	}
	return iIndex;
}

bool ZMain::FirstImage()
{
	// 현재의 위치를 History 에 저장해놓는다.
	m_history.push_lastImageIndex(m_iCurretFileIndex);
	return MoveIndex(0);
}

bool ZMain::LastImage()
{
	// 현재의 위치를 History 에 저장해놓는다.
	m_history.push_lastImageIndex(m_iCurretFileIndex);
	return MoveIndex((int)m_vFile.size() - 1);
}

void ZMain::OnChangeCurrentSize(int iWidth, int iHeight)
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
//		assert(m_pCurrentImage);
		return;
	}

	if ( m_iShowingX + iWidth > m_pCurrentImage->GetWidth() )
	{
		m_iShowingX -= (m_iShowingX + iWidth - m_pCurrentImage->GetWidth());

		if ( m_iShowingX < 0 ) m_iShowingX = 0;
	}
	if ( m_iShowingY + iHeight > m_pCurrentImage->GetHeight() )
	{
		m_iShowingY -= (m_iShowingY + iHeight - m_pCurrentImage->GetHeight());
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

void ZMain::StartTimer()
{
	/// Add Timer
	m_timerPtr = SetTimer(m_hMainDlg, eTimerValue, 100, NULL);

	if ( m_timerPtr == 0 )
	{
		MessageBox(m_hMainDlg, GetMessage(TEXT("CANNOT_MAKE_TIMER")), TEXT("ZViewer"), MB_OK);
		return;
	}
}

void ZMain::StopTimer()
{
	/// KillTimer
	if ( m_timerPtr != 0 )
	{
		BOOL bRet = KillTimer(m_hMainDlg, eTimerValue);

		if ( FALSE == bRet )
		{
			assert(bRet);
		}
		else
		{
			m_timerPtr = 0;
		}
	}
}

/// Cache status 를 상태 표시줄에 표시한다.
void ZMain::ShowCacheStatus()
{
	static bool bLastActionIsCache = false;

	bool bNowActionIsCache = ZCacheImage::GetInstance().isCachingNow();

	if ( bLastActionIsCache != bNowActionIsCache )
	{
		bLastActionIsCache = bNowActionIsCache;

		static tstring strStatusMsg=TEXT("...");	///< PostMessage() 로 호출하므로, 메모리가 없어지지 않게 하기 위해 static

		if ( m_vFile.empty() )
		{
			strStatusMsg = TEXT("");
		}
		else if ( false == bNowActionIsCache ) ///< 캐쉬가 끝났으면
		{
			strStatusMsg = TEXT("Cached");
		}
		else///< 아직 캐쉬 중이면
		{
			if ( ZCacheImage::GetInstance().IsNextFileCached() )	///< 다음 파일의 캐쉬가 끝났으면
			{
				strStatusMsg = TEXT("Caching files");
			}
			else ///< 아직 다음 파일도 캐쉬가 안 끝났으면,
			{
				strStatusMsg = TEXT("Caching next");
			}
		}
		PostMessage(m_hStatusBar, SB_SETTEXT, 6, (LPARAM)strStatusMsg.c_str());

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
	if ( ZOption::GetInstance().IsFullScreen() )	// 현재 풀스크린이면 원래 화면으로 돌아간다.
	{
		ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());

		TaskBar::ShellTrayShow();	// 숨겨졌던 작업 표시줄을 보여준다.

		FormShow();	// 메뉴, 상태 표시줄등을 보여준다.

		SetWindowPos(m_hMainDlg, HWND_TOP, m_lastPosition.left, m_lastPosition.top, m_lastPosition.right - m_lastPosition.left, m_lastPosition.bottom - m_lastPosition.top, SWP_SHOWWINDOW);
	}
	else	// 현재 풀스크린이 아니면 풀스크린으로 만든다.
	{
		ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());
		// 현재 크기를 기억한다.
		GetWindowRect(m_hMainDlg, &m_lastPosition);

		FormHide();// 메뉴, 상태 표시줄등을 숨긴다.

		const int screenWidth = ::GetSystemMetrics( SM_CXSCREEN );
		const int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		// 화면 듀얼 모니터 처리를 해줌. 처음 위치에 따라서 full screen 후 화면의 위치를 바꿔준다.(아래 소스는 듀얼 모니터이고, 양쪽의 해상도가 같고, 기본 모니터가 왼쪽의 모니터일 때만 적용된다)
		const int screenX = int(m_lastPosition.left / screenWidth) * screenWidth;
		const int screenY = int(m_lastPosition.top / screenHeight) * screenHeight;

		SetWindowPos(m_hMainDlg, HWND_TOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, screenX, screenY, screenWidth, screenHeight, TRUE);


		/* patch 적용하기 전의 소스
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);
		*/

		// 작업 표시줄을 가려준다.
		TaskBar::ShellTrayHide();

		// 포커스를 잃으면 원래대로 돌아가야하므로 풀어놓는다.
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);
		/* patch 적용하기 전의 소스
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		*/

		//MoveWindow(m_hMainDlg, )
		//m_iRestoreX =
	}

	SetCheckMenus();

	m_iShowingX = 0;
	m_iShowingY = 0;
}


void ZMain::ToggleSmallToScreenStretch()
{
	ZOption::GetInstance().ToggleSmallToBigStretchImage();

	SetCheckMenus();

	LoadCurrent();
	Draw();
}

void ZMain::ToggleBigToScreenStretch()
{
	ZOption::GetInstance().ToggleBigToSmallStretchImage();

	SetCheckMenus();

	LoadCurrent();
	Draw();
}

void ZMain::ToggleLoopImage()
{
	ZOption::GetInstance().ToggleLoopImages();

	SetCheckMenus();
}

void ZMain::SetStatusBarText()
{
	TCHAR szTemp[COMMON_BUFFER_SIZE];
	tstring strTemp;

	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false || m_vFile.size() == 0 || m_strCurrentFilename.empty() ) // 보고 있는 파일이 없으면
	{
		// File Index
		SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)_T("No File"));

		// 해상도 정보
		SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)TEXT(""));

		// 이미지 사이즈
		SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)_T(""));

		// 임시로 http://wimy.com
		SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)TEXT(""));

		// 임시로 http://wimy.com
		SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)TEXT("http://wimy.com"));

		// 로딩시간
		SendMessage(m_hStatusBar, SB_SETTEXT, 5, (LPARAM)TEXT(""));

		// 캐시 중인가
		if ( m_vFile.size() == 0 )
		{
			SendMessage(m_hStatusBar, SB_SETTEXT, 6, (LPARAM)TEXT(""));
		}
		else
		{
			ShowCacheStatus(); ///< 6
		}

		// 파일명
		SendMessage(m_hStatusBar, SB_SETTEXT, 7, (LPARAM)TEXT("No File"));
	}
	else
	{
		// File Index
		SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%d/%d"), m_iCurretFileIndex+1, m_vFile.size());
		SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%dx%dx%dbpp"), m_pCurrentImage->GetOriginalWidth(), m_pCurrentImage->GetOriginalHeight(), m_pCurrentImage->GetBPP());
		SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

		// image size
		long imageSize = m_vFile[m_iCurretFileIndex].m_nFileSize;

		if ( imageSize > 1024 )
		{
			if ( imageSize/1024 > 1024 )
			{
				SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%.2fMByte"), imageSize/1024/1024.0f);
			}
			else
			{
				SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%dKByte"), imageSize/1024);
			}
		}
		else
		{
			SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%dByte"), imageSize);
		}
		SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)szTemp);

		// 이미지 배율
		SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%d%%"), (int)(m_fCurrentZoomRate*100.0f));
		//SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%f"), m_fCurrentZoomRate);
		SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 임시로 http://wimy.com
		SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)TEXT("http://wimy.com"));

		// 로딩시간
		SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%.3fsec"), (float)(m_dwLoadingTime / 1000.0));
		SendMessage(m_hStatusBar, SB_SETTEXT, 5, (LPARAM)szTemp);

		ShowCacheStatus(); ///< 6

		// 파일명
		TCHAR szFilename[MAX_PATH], szFileExt[MAX_PATH];
		SplitPath(m_strCurrentFilename.c_str(), NULL,0, NULL,0, szFilename,MAX_PATH, szFileExt,MAX_PATH);
		SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%s%s"), szFilename, szFileExt);
		SendMessage(m_hStatusBar, SB_SETTEXT, 7, (LPARAM)szTemp);
	}
}

void ZMain::SetTitle()
{
	TCHAR szTemp[MAX_PATH+256];

	if ( m_strCurrentFilename.empty() )	// 현재보고 있는 파일명이 없으면
	{
		SPrintf(szTemp, MAX_PATH+256, TEXT("ZViewer v%s"), g_strVersion.c_str());
	}
	else // 현재보고 있는 파일명이 있으면
	{
		TCHAR szFileName[FILENAME_MAX] = { 0 };
		TCHAR szFileExt[MAX_PATH] = { 0 };
		SplitPath(m_strCurrentFilename.c_str(), NULL,0, NULL,0, szFileName,FILENAME_MAX, szFileExt,MAX_PATH);

		SPrintf(szTemp, MAX_PATH+256, TEXT("%s%s - %s"), szFileName, szFileExt, m_strCurrentFilename.c_str());
	}
	BOOL bRet = SetWindowText(m_hMainDlg, szTemp);

	if ( FALSE == bRet )
	{
		assert(!"Can't SetWindowText");
	}
}

void ZMain::LoadCurrent()
{
	/// 파일 목록이 하나도 없으면 로딩을 시도하지 않는다.
	if ( m_vFile.empty() ) return;

	static bool bFirst = true;

	if ( bFirst )
	{
		bFirst = false;

		ZCacheImage::GetInstance().SetImageVector(m_vFile);
		ZCacheImage::GetInstance().StartThread();
	}

	DWORD start = GetTickCount();

	_releaseBufferDC();

	static tstring lastCheckFileName;

	/// 캐시된 데이터에 있으면
	if ( ZCacheImage::GetInstance().hasCachedData(m_strCurrentFilename, m_iCurretFileIndex) )
	{
		{/// 캐시된 데이터를 읽어온다.
			ZCacheImage::GetInstance().GetCachedData(m_strCurrentFilename, m_pCurrentImage);

			assert(m_pCurrentImage);
			assert(m_pCurrentImage->IsValid());

			m_bCurrentImageLoaded = true;
		}

		DebugPrintf(TEXT("Cache Hit!!!!!!!!!!!!!"));

		ZCacheImage::GetInstance().LogCacheHit();

		m_dwLoadingTime = GetTickCount() - start;
		SetTitle();			///< 파일명을 윈도우 타이틀바에 적는다.
		SetStatusBarText();	///< 상태 표시줄 내용을 설정한다.

		m_iShowingX = 0;
		m_iShowingY = 0;

		if ( ZOption::GetInstance().m_bRightTopFirstDraw )	// 우측 상단부터 시작해야하면
		{
			RECT rt;
			getCurrentScreenRect(rt);

			if ( m_pCurrentImage->GetWidth() > rt.right )
			{
				m_iShowingX = m_pCurrentImage->GetWidth() - rt.right;
			}
		}

		/// 기본적으로 그림의 zoom rate 를 설정한다.
		m_fCurrentZoomRate = 1.0f;
		RECT currentScreenRect;
		if ( false == getCurrentScreenRect(currentScreenRect) )
		{
			assert(false);
		}
		else
		{
			if ( ZOption::GetInstance().IsBigToSmallStretchImage() || ZOption::GetInstance().IsSmallToBigStretchImage() )
			{
				RECT toRect;
				SetRect(&toRect, 0, 0, m_pCurrentImage->GetWidth(), m_pCurrentImage->GetHeight());

				if ( ZOption::GetInstance().IsBigToSmallStretchImage() )
				{
					if ( m_pCurrentImage->GetWidth() > (currentScreenRect.right - currentScreenRect.left) || m_pCurrentImage->GetHeight() > (currentScreenRect.bottom - currentScreenRect.top) )
					{
						toRect = GetResizedRectForBigToSmall(currentScreenRect, toRect);
					}
				}

				if ( ZOption::GetInstance().IsSmallToBigStretchImage() )
				{
					if ( m_pCurrentImage->GetWidth() < (currentScreenRect.right - currentScreenRect.left) && m_pCurrentImage->GetHeight() < (currentScreenRect.bottom - currentScreenRect.top) )
					{
						toRect = GetResizedRectForSmallToBig(currentScreenRect, toRect);
					}
				}
				m_fCurrentZoomRate = (float)(toRect.right - toRect.left) / (float)(m_pCurrentImage->GetWidth());
			}
		}

		Draw();
	}
	else
	{
		// 캐시에서 찾을 수 없으면 지금 읽어들이고, 캐시에 추가한다.
		DebugPrintf(TEXT("Can't find in cache. load now..."));

/*
		else
		{
			DebugPrintf(TEXT("Cache miss."));
			m_bLastCacheHit = false;
			ZCacheImage::GetInstance().LogCacheMiss();
		}
		*/

		//m_currentImage = m_loadingImage;
		//m_fCurrentZoomRate = 1.0f;
		//Draw();
	}
}

void ZMain::OnDrag(int x, int y)
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		//assert(m_pCurrentImage);
		return;
	}

	//	DebugPrintf(TEXT("Drag (%d,%d)"), x, y);
	if ( !m_pCurrentImage->IsValid()) return;

	RECT rt;
	getCurrentScreenRect(rt);

	int iNowShowingX = m_iShowingX;
	int iNowShowingY = m_iShowingY;

	int iZoomedWidth = (int)(m_pCurrentImage->GetWidth() * m_fCurrentZoomRate);
	int iZoomedHeight = (int)(m_pCurrentImage->GetHeight() * m_fCurrentZoomRate);

	/// 현재 그림이 화면보다 작거나 딱 맞으면 drag 를 하지 않는다.
	if ( iZoomedWidth <= rt.right && iZoomedHeight <= rt.bottom )
	{
		return;
	}

	m_iShowingX += x;

	if ( m_iShowingX <= 0 )
	{
		m_iShowingX = 0;
	}
	if ( m_iShowingX + rt.right >= iZoomedWidth )
	{
		m_iShowingX = iZoomedWidth - rt.right;
	}

	m_iShowingY += y;
	if ( m_iShowingY <= 0 )
	{
		m_iShowingY = 0;
	}
	if ( m_iShowingY + rt.bottom >= iZoomedHeight )
	{
		m_iShowingY = iZoomedHeight - rt.bottom;
	}

	//DebugPrintf(TEXT("ShowingX(%d), ShowingY(%d)"), m_iShowingX, m_iShowingY);
	/// 보여주는 X 좌표나 Y 좌표 둘 중 하나라도 변경되었으면, 드래그된 것이므로 다시 그린다.
	if ( m_iShowingX != iNowShowingX || m_iShowingY != iNowShowingY )
	{
		/// 여기서 false 인 이유는, 드래그를 할 수 있으면, 배경을 다시 칠할 필요가 없기 때문이다.
		Draw(NULL, false);
	}
}


void ZMain::ChangeFileSort(eFileSortOrder sortOrder)
{
	m_sortOrder = sortOrder;
	ReLoadFileList();
}

void ZMain::ReLoadFileList()
{
	tstring strFileName = m_strCurrentFilename;
	RescanFolder();

	OpenFile(strFileName);
}

/// 현재 파일이 지워졌을 때 후의 처리. 파일 삭제, 이동 후에 불리는 함수이다.
void ZMain::_ProcAfterRemoveThisFile()
{
	m_bCurrentImageLoaded = false;
	m_pCurrentImage = NULL;
	m_strCurrentFilename.resize(0);

	// 현재 파일이 마지막 파일인가?
	if ( m_vFile.size() <= 1 )
	{
		m_iCurretFileIndex = 0;
		m_strCurrentFilename = TEXT("");

		m_vFile.resize(0);

		// Cache Thread 에 전달한다.
		ZCacheImage::GetInstance().SetImageVector(m_vFile);

		SetTitle();
		SetStatusBarText();
		Draw(NULL, true);
	}
	else
	{
		tstring strNextFilename;

		assert(m_vFile.size() > 1);
		assert(m_iCurretFileIndex >= 0);

		if ( m_iCurretFileIndex == (int)(m_vFile.size() - 1) )	///< 마지막 파일이면 이전 파일이고,
		{
			strNextFilename = m_vFile[m_iCurretFileIndex-1].m_strFileName;
		}
		else/// 마지막 파일이 아니면 다음 파일을 보여준다.
		{
			strNextFilename = m_vFile[m_iCurretFileIndex+1].m_strFileName;
		}
		ZMain::GetInstance().OpenFile(strNextFilename);
	}
	ZCacheImage::GetInstance().SetImageVector(m_vFile);
}


void ZMain::OnFocusLose()
{
	//DebugPrintf("OnFocusLose()");

	TaskBar::ShellTrayShow();
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
		DebugPrintf(TEXT("OnFocusGet() at fullscreen"));

		const int screenWidth = ::GetSystemMetrics( SM_CXSCREEN );
		const int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		// 화면 듀얼 모니터 처리를 해줌.
		const int screenX = int(m_lastPosition.left / screenWidth) * screenWidth;
		const int screenY = int(m_lastPosition.top / screenHeight) * screenHeight;

		SetWindowPos(m_hMainDlg, HWND_TOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, screenX, screenY, screenWidth, screenHeight, TRUE);
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, screenX, screenY, screenWidth, screenHeight, SWP_NOMOVE|SWP_NOSIZE);

		/*
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		*/
		TaskBar::ShellTrayHide();
	}
}

void ZMain::Undo()
{
	if ( m_history.CanUndo() )
	{
		int iLast = m_history.Undo();

		// 혹시나 범위를 벗어나면 안됨
		if ( iLast < 0 || iLast >= (int)m_vFile.size() )
		{
			assert(!"Over range...");
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

	SetCheckMenus();
}

void ZMain::ShowFileExtDlg()
{
	ZFileExtDlg::GetInstance().ShowDlg();
}

void ZMain::DeleteThisFile()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() || m_bCurrentImageLoaded == false )
	{
		return;
	}

	TCHAR szDeleteMsg[COMMON_BUFFER_SIZE];

	SPrintf(szDeleteMsg, COMMON_BUFFER_SIZE, GetMessage(TEXT("DELETE_THIS_FILE")), GetFileNameFromFullFileName(m_strCurrentFilename).c_str());
	int iRet = ::MessageBox(m_hMainDlg, szDeleteMsg, TEXT("ZViewer"), MB_YESNO);

	if ( iRet == IDYES )
	{
		if ( 0 == _tunlink(m_strCurrentFilename.c_str()) )
		{
			DebugPrintf(TEXT("-- %s deleted --"), m_strCurrentFilename.c_str());
			_ProcAfterRemoveThisFile();
		}
		else
		{
			ShowMessageBox(GetMessage(TEXT("CANNOT_DELETE_THIS_FILE")));
		}
	}
}

void ZMain::DeleteThisFileToRecycleBin()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() || m_bCurrentImageLoaded == false )
	{
		return;
	}

	SHFILEOPSTRUCT fo;
	fo.hwnd = m_hMainDlg;
	fo.wFunc = FO_DELETE;

	TCHAR szFilename[_MAX_PATH];
	SPrintf(szFilename, _MAX_PATH, _T("%s"), m_strCurrentFilename.c_str());
	szFilename[m_strCurrentFilename.size()+1] = 0;

	fo.pFrom = szFilename;
	fo.pTo = NULL;
	fo.fFlags = FOF_ALLOWUNDO;

	if ( 0 == SHFileOperation(&fo))
	{
		if ( fo.fAnyOperationsAborted == FALSE )
		{
			DebugPrintf(TEXT("-- %s deleted --"), m_strCurrentFilename.c_str());
			_ProcAfterRemoveThisFile();
		}
	}
	else
	{
		ShowMessageBox(GetMessage(TEXT("CANNOT_DELETE_THIS_FILE")));
	}
}


void ZMain::MoveThisFile()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() )
	{
		return;
	}

	CSelectToFolderDlg aDlg;

	if ( !aDlg.DoModal() )
	{
		return;
	}

	tstring strFolder = aDlg.GetMoveToFolder();

	tstring filename = GetFileNameFromFullFileName(m_strCurrentFilename);
	tstring strToFileName = aDlg.GetMoveToFolder();

	if ( strToFileName.size() <= 2 )
	{
		ShowMessageBox(GetMessage(TEXT("MOVE_DESTINATION_IS_TOO_SHORT")));
		return;
	}

	strToFileName += TEXT("\\");
	strToFileName += filename;

	// 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
	if ( 0 != _taccess(aDlg.GetMoveToFolder().c_str(), 00) )
	{
		ShowMessageBox(GetMessage(TEXT("WRONG_DIRECTORY_NAME")));
		return;
	}

	// 같은 파일이 존재하는지 확인한다.
	if ( 0 == _taccess(strToFileName.c_str(), 00) )
	{
		// 이미 존재하면
		if ( IDNO == ShowMessageBox(GetMessage(TEXT("ASK_OVERWRITE_FILE")), MB_YESNO) )
		{
			return;
		}
	}

	if ( FALSE == MoveFileEx(m_strCurrentFilename.c_str(), strToFileName.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) )
	{
		ShowMessageBox(GetMessage(TEXT("CANNOT_MOVE_FILE")));
	}
	_ProcAfterRemoveThisFile();
}


/// 현재 파일을 복사한다.
void ZMain::CopyThisFile()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() )
	{
		return;
	}

	CSelectToFolderDlg aDlg;

	if ( !aDlg.DoModal() )
	{
		return;
	}

	tstring strFolder = aDlg.GetMoveToFolder();

	tstring filename = GetFileNameFromFullFileName(m_strCurrentFilename);
	tstring strToFileName = aDlg.GetMoveToFolder();

	if ( strToFileName.size() <= 2 )
	{
		ShowMessageBox(GetMessage(TEXT("MOVE_DESTINATION_IS_TOO_SHORT")));
		return;
	}

	strToFileName += TEXT("\\");
	strToFileName += filename;

	// 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
	if ( 0 != _taccess(aDlg.GetMoveToFolder().c_str(), 00) )
	{
		ShowMessageBox(GetMessage(TEXT("WRONG_DIRECTORY_NAME")));
		return;
	}

	// 같은 파일이 존재하는지 확인한다.
	if ( 0 == _taccess(strToFileName.c_str(), 00) )
	{
		// 이미 존재하면
		if ( IDNO == ShowMessageBox(GetMessage(TEXT("ASK_OVERWRITE_FILE")), MB_YESNO) )
		{
			return;
		}
	}

	if ( FALSE == CopyFile(m_strCurrentFilename.c_str(), strToFileName.c_str(), FALSE) )
	{
		ShowMessageBox(GetMessage(TEXT("CANNOT_COPY_FILE")));
	}
}


void ZMain::Rotate(bool bClockWise)
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		assert(m_pCurrentImage);
		return;
	}

	if ( m_pCurrentImage->IsValid() )
	{
		if ( bClockWise )
		{
			m_pCurrentImage->Rotate(-90);
		}
		else
		{
			m_pCurrentImage->Rotate(90);
		}

		m_iShowingX = 0;
		m_iShowingY = 0;

		_releaseBufferDC();

		Draw(NULL, true);
	}
}

void ZMain::DecreaseOpacity()
{
	if ( m_alpha < 20 )
	{
		m_alpha = 20;
	}
	m_alpha -= 10;

	SetWindowLong ( m_hMainDlg, GWL_EXSTYLE, GetWindowLong(m_hMainDlg, GWL_EXSTYLE) | WS_EX_LAYERED );

	SetLayeredWindowAttributes(m_hMainDlg, 0, m_alpha, LWA_ALPHA);
}

void ZMain::IncreaseOpacity()
{
	if ( m_alpha >= 245 )
	{
		m_alpha = 245;
	}
	m_alpha += 10;

	SetWindowLong ( m_hMainDlg, GWL_EXSTYLE, GetWindowLong(m_hMainDlg, GWL_EXSTYLE) | WS_EX_LAYERED );
	SetLayeredWindowAttributes(m_hMainDlg, 0, m_alpha, LWA_ALPHA);
}


void ZMain::SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style)
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		assert(m_pCurrentImage);
		return;
	}

	// 현재보고 있는 파일을 윈도우 폴더에 저장한다.
	TCHAR szSystemFolder[_MAX_PATH] = { 0 };

	if ( E_FAIL == SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, szSystemFolder) )
	{
		assert(false);
		return;
	}

	TCHAR szFileName[FILENAME_MAX] = { 0 };
	SplitPath(m_vFile[m_iCurretFileIndex].m_strFileName.c_str(), NULL,0, NULL,0, szFileName,FILENAME_MAX, NULL,0);

	tstring strSaveFileName = szSystemFolder;
	strSaveFileName += TEXT("\\zviewer_bg_");
	strSaveFileName += szFileName;
	strSaveFileName += TEXT(".bmp");

	if ( FALSE == m_pCurrentImage->SaveToFile(strSaveFileName, BMP_DEFAULT) )
	{
		assert(false);
		return;
	}

	CDesktopWallPaper::SetDesktopWallPaper(strSaveFileName, style);
}


/// 현재 화면의 그릴 수 있는 영역의 크기를 받아온다.
bool ZMain::getCurrentScreenRect(RECT & rect)
{
	if ( NULL == m_hShowWindow )
	{
		assert(m_hShowWindow);
		return false;
	}

	if ( FALSE == GetClientRect(m_hShowWindow, &rect) ) return false;
	return true;
}

/// On Window is resized
void ZMain::OnWindowResized()
{
	RECT rt;
	getCurrentScreenRect(rt);

	OnChangeCurrentSize(rt.right, rt.bottom);
}

/// 마우스 휠을 돌렸을 때. delta 는 돌린 방향과 돌린 정도, bControlPushed 는 Control 키가 눌려졌으면 true
void ZMain::OnMouseWheel(const short delta, bool bControlPushed)
{
	if ( bControlPushed )
	{
		if ( delta < 0 )
		{
			ZMain::GetInstance().ZoomOut();
		}
		else
		{
			ZMain::GetInstance().ZoomIn();
		}
	}
	else
	{
		if ( delta < 0 )
		{
			ZMain::GetInstance().NextImage();
		}
		else
		{
			ZMain::GetInstance().PrevImage();
		}
	}
}

/// 상태 표시 윈도우를 만든다.
void ZMain::CreateStatusBar()
{
	// StatusBar 를 생성한다.
	m_hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, TEXT("No Image"), m_hMainDlg, 0);

	// StatusBar 를 split 한다. 아래의 숫자는 크기가 아니라 절대 위치라는 것을 명심!!!!!!!
	enum
	{
		STATUS_SPLIT_NUM = 8
	};

	int SBPart[STATUS_SPLIT_NUM] =
	{
		70,		///< %d/%d 현재보고 있는 이미지 파일의 index number
		200,	///< %dx%dx%dbpp 해상도와 color depth, image size
		300,	///< image size
		350,	///< zoom
		470,	///< temp banner http://www.wimy.com
		550,	///< 파일을 읽어들이는데 걸린 시간
		633,	///< cache status
		1910,	///< 파일명표시
	};
	SendMessage(m_hStatusBar, SB_SETPARTS, STATUS_SPLIT_NUM, (LPARAM)SBPart);
}

/// 메뉴 중 체크표시 되는 것을 확인하여 설정해준다.
void ZMain::SetCheckMenus()
{
	/// start of checking main menu
	CheckMenuItem(m_hMainMenu, ID_OPTION_VIEWLOOP, ZOption::GetInstance().IsLoopImages() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hMainMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hMainMenu, ID_VIEW_SMALLTOSCREENSTRETCH, ZOption::GetInstance().IsSmallToBigStretchImage() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hMainMenu, ID_VIEW_BIGTOSCREENSTRETCH , ZOption::GetInstance().IsBigToSmallStretchImage() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hMainMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);

	CheckMenuItem(m_hMainMenu, ID_AUTOROTATION, ZOption::GetInstance().IsUseAutoRotation() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hMainMenu, ID_VIEW_ALWAYSONTOP, ZOption::GetInstance().m_bAlwaysOnTop ? MF_CHECKED : MF_UNCHECKED);
	/// end of checking main menu



	/// start of checking popup menu
	CheckMenuItem(m_hPopupMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_SMALLTOSCREENSTRETCH, ZOption::GetInstance().IsSmallToBigStretchImage() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_BIGTOSCREENSTRETCH, ZOption::GetInstance().IsBigToSmallStretchImage() ? MF_CHECKED : MF_UNCHECKED);
	/// end of checking popup menu
}

void ZMain::StartSlideMode()
{
	ZOption::GetInstance().m_bSlideMode = true;
	ZOption::GetInstance().m_dwLastSlidedTime = GetTickCount();
}

void ZMain::ToggleAutoRotation()
{
	ZOption::GetInstance().SetAutoRotation(!ZOption::GetInstance().IsUseAutoRotation());
	SetCheckMenus();
}

void ZMain::ToggleAlwaysOnTop()
{
	ZOption::GetInstance().m_bAlwaysOnTop = !ZOption::GetInstance().m_bAlwaysOnTop;

	if ( ZOption::GetInstance().m_bAlwaysOnTop )
	{
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else
	{
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	}
	SetCheckMenus();
}

/// 배경을 지운다.
void ZMain::_eraseBackground(HDC mainDC, LONG right, LONG bottom)
{
	SelectObject(mainDC, m_bgBrush);
	Rectangle(mainDC, 0, 0, right, bottom);
}

/// Zoom in & out 전의 센터 위치로 그림을 드래그함. ZoomOut, ZoomIn 중에 호출됨
void ZMain::_PositionPreviousCenter()
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		assert(m_pCurrentImage);
		return;
	}

	RECT cr;
	getCurrentScreenRect(cr);
	int moveX = (int)(m_pCurrentImage->GetWidth() * m_fCurrentZoomRate * m_fCenterX - (cr.right / 2));
	int moveY = (int)(m_pCurrentImage->GetHeight() * m_fCurrentZoomRate * m_fCenterY - (cr.bottom / 2));
	OnDrag(moveX, moveY);
}

void ZMain::ZoomIn()
{
	double fBeforeModify = m_fCurrentZoomRate;

	if ( m_fCurrentZoomRate < 1.9999 )
	{
		m_fCurrentZoomRate += 0.1;
	}
	else
	{
		m_fCurrentZoomRate += 1.0;
	}

	if ( m_fCurrentZoomRate >= 20.0 )
	{
		m_fCurrentZoomRate = fBeforeModify;
	}

	/// 실수 연산이 딱 떨어지지 않기 때문에 처리해준다.
	m_fCurrentZoomRate = ((int)((m_fCurrentZoomRate * 10.0)+0.5) / 10.0f);

	if ( fBeforeModify != m_fCurrentZoomRate )
	{
		m_iShowingX = 0;
		m_iShowingY = 0;

		SetStatusBarText();
		_PositionPreviousCenter(); ///< 이전 배율에서의 화면 위치로 드래그해서 Zoom 할 때 자연스럽게 보여준다.
		Draw();
	}
}

void ZMain::ZoomOut()
{
	double fBeforeModify = m_fCurrentZoomRate;

	if ( m_fCurrentZoomRate <= 2.000001 )
	{
		m_fCurrentZoomRate -= 0.1;
	}
	else
	{
		m_fCurrentZoomRate -= 1.0;
	}

	if ( m_fCurrentZoomRate < 0.1 )
	{
		m_fCurrentZoomRate = fBeforeModify;
	}

	/// 실수 연산이 딱 떨어지지 않기 때문에 처리해준다.
	m_fCurrentZoomRate = ((int)((m_fCurrentZoomRate * 10.0)+0.5) / 10.0f);

	if ( fBeforeModify != m_fCurrentZoomRate )
	{
		m_iShowingX = 0;
		m_iShowingY = 0;

		SetStatusBarText();
		_PositionPreviousCenter(); ///< 이전 배율에서의 화면 위치로 드래그해서 Zoom 할 때 자연스럽게 보여준다.
		Draw();
	}
}

void ZMain::ZoomNone()
{
	if ( m_fCurrentZoomRate == 1.0 ) return;

	m_fCurrentZoomRate = 1.0;
	m_iShowingX = 0;
	m_iShowingY = 0;

	SetStatusBarText();
	Draw();
}

/// 현재보는 이미지를 클립보드에 복사한다.
void ZMain::CopyToClipboard()
{
	if ( NULL == m_pCurrentImage || m_bCurrentImageLoaded == false )
	{
		assert(m_pCurrentImage);
		return;
	}

	if ( m_pCurrentImage->IsValid() )
	{
		m_pCurrentImage->CopyToClipboard(m_hMainDlg);
	}
	else
	{
		assert(false);
	}
}

/// 그림을 보여줄 윈도우를 만든다.
void ZMain::CreateShowWindow()
{
	m_hShowWindow = CreateWindow(TEXT("static"), TEXT(""), WS_CHILD | WS_VISIBLE , 0,0,100,100,
	m_hMainDlg, (HMENU)-1, ZResourceManager::GetInstance().GetHInstance(), NULL);

	AdjustShowWindowScreen();

	assert(m_hShowWindow != INVALID_HANDLE_VALUE);
}


/// 현재 크기에 맞는 ShowWindow 크기를 정한다.
void ZMain::AdjustShowWindowScreen()
{
	RECT rect;
	GetClientRect(m_hMainDlg, &rect);

	if ( false == ZOption::GetInstance().IsFullScreen() ) /// 풀 스크린이 아닐 때는 작업표시줄의 영역만큼은 빼줘야함
	{
		enum
		{
			STATUSBAR_HEIGHT = 20
		};

		rect.bottom -= STATUSBAR_HEIGHT;
	}
	SetWindowPos(m_hShowWindow, HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_NOMOVE);
}
