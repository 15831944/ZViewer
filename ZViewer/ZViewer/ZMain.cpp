#include "StdAfx.h"
#include ".\zmain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZCacheImage.h"
#include "src/ZResourceManager.h"
#include "../lib/DesktopWallPaper.h"

#include <ShlObj.h>

#include "resource.h"

using namespace std;

ZMain & ZMain::GetInstance()
{
	static ZMain aInstance;
	return aInstance;
}

ZMain::ZMain(void)
:	m_hMainDlg(NULL)
	
{
	SetProgramFolder();

	if ( m_bannerImage.load((m_strProgramFolder + "\\banner.jpg").c_str()) == false )
	{
		_ASSERTE(!"can't load banner.");
	}
	
	// ��� �ٿ�ε带 �����Ѵ�.
	//StartBannerDownload();

}

ZMain::~ZMain(void)
{
}

int ZMain::GetLogCacheHitRate()
{
	return ZCacheImage::GetInstance().GetLogCacheHitRate();
}

long ZMain::GetCachedKByte()
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
		/*
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
		0, (LPSECURITY_ATTRIBUTES) NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		(HANDLE) NULL);
		*/
		OpenFile(ofn.lpstrFile);
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

std::string ZMain::GetFolderNameFromFullFileName(const std::string & strFullFilename)
{
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	_splitpath(strFullFilename.c_str(), szDrive, szDir, 0, 0);

	std::string strFolder = szDrive;
	strFolder += szDir;

	return strFolder;
}

void ZMain::Draw(bool bEraseBg)
{
	//return;
	if ( m_currentImage.IsValid() == FALSE ) return;
	if ( m_hMainDlg == NULL ) return;

	RECT rt;
	GetClientRect(m_hMainDlg, &rt);
	if ( m_option.m_bFullScreen == false ) rt.bottom -= STATUSBAR_HEIGHT;

	HDC mainDC = GetDC(m_hMainDlg);

	if ( m_vFile.size() <= 0 )
	{
		SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
		Rectangle(mainDC, 0, 0, rt.right, rt.bottom);

		ReleaseDC(m_hMainDlg, mainDC);
		return;
	}

	// �׸��� �׷����� ������ ȭ���� ��ġ
	int iDrawX = 0;			
	int iDrawY = 0;

	// �׸��� �׷����� �����ϴ� �׸��� ��ġ
	int iDrawPartX = 0;
	int iDrawPartY = 0;

	bool bWidthBig = false;
	bool bHeightBig = false;

	if ( m_currentImage.GetWidth() < rt.right )
	{
		// ���� ���̰� ȭ�麸�� ���� ���� �߾ӿ� ����
		iDrawX = (rt.right/2) - (m_currentImage.GetWidth()/2);
	}
	else
	{
		// ���� ���̰� ȭ�麸�� ũ�� Ŭ����
		iDrawPartX = m_iShowingX;
		bWidthBig = true;
	}

	if ( m_currentImage.GetHeight() < rt.bottom )
	{
		// ���� ���̰� ȭ�麸�� ���� ���� �߾ӿ� ����
		iDrawY = (rt.bottom/2) - (m_currentImage.GetHeight()/2);
	}
	else
	{
		// ���� ���̰� ȭ�麸�� ũ�� Ŭ����
		iDrawPartY = m_iShowingY;
		bHeightBig = true;
	}


	/// �׸��� ȭ�麸�� ū�� Ȯ��
	if ( bWidthBig || bHeightBig )
	{
		/// �׸��� ȭ�麸�� ũ��...

		if ( m_option.m_bBigToSmallStretchImage )
		{
			/// ū �׸��� ��ҽ��Ѽ� �׸���.

			RECT toRect;
			SetRect(&toRect, 0, 0, m_currentImage.GetWidth(), m_currentImage.GetHeight());
			toRect = GetResizedRect(rt, toRect);

			ZImage stretchedImage(m_currentImage);

			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			iDrawX = (rt.right/2) - (toRect.right/2);
			iDrawY = (rt.bottom/2) - (toRect.bottom/2);

			if ( bEraseBg )	// ����� ������ �ϸ� �����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, rt.right, rt.bottom);
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

			int x = m_currentImage.GetWidth();
			int y = m_currentImage.GetHeight();
			HBITMAP hbmScreen = CreateCompatibleBitmap(mainDC,
				x, 
				y); 

			SelectObject(memDC, hbmScreen);


			// �޸𸮿� ��ü �׸��� �׸���.
			int r = StretchDIBits(memDC,
				0, 0, 
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				0, 0,
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				m_currentImage.GetData(),
				m_currentImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);

			if ( bEraseBg )	// ����� ������ �ϸ� �����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, rt.right, rt.bottom);
			}

			// �޸𸮰��� ȭ�鿡 �׸���.
			BOOL b = BitBlt(mainDC, 
				iDrawX, iDrawY,			// �׸� ȭ���� x, y ��ǥ. ȭ�鿡 �� �� ���� 0, 0 �̾���Ѵ�.
				m_currentImage.GetWidth(), rt.bottom,		// �׷��� ȭ���� ����, ���� ����. 
				memDC, 
				iDrawPartX, iDrawPartY,			// �׷��� �̹��� ������ ���� x,y ��ǥ
				SRCCOPY);

			DebugPrintf("rt.bottom : %d, PartX : %d, iDrawPartY : %d", rt.bottom, iDrawPartX, iDrawPartY);

			DeleteObject(hbmScreen);
			DeleteDC(memDC);
		}
	}
	else	/// ȭ���� �׸����� ������...
	{
		if ( m_option.m_bSmallToBigStretchImage )
		{
			/// ���� �׸��� ȭ�鿡 �°� Ȯ���ؼ� �׸���.
		}
		else
		{
			if ( bEraseBg )	// ����� ������ �ϸ� �����.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, rt.right, rt.bottom);
			}

			/// ���� �׸��� ȭ�� ����� �׸���.
			int r = StretchDIBits(mainDC,
				iDrawX, iDrawY, 
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				0, 0,
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				m_currentImage.GetData(),
				m_currentImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);
		}
	}
	ReleaseDC(m_hMainDlg, mainDC);

	// ���콺 Ŀ�� ���
	if ( m_currentImage.GetWidth() > rt.right ||
		m_currentImage.GetHeight() > rt.bottom
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

	if ( false == m_option.m_bFullScreen )
	{
		PostMessage(m_hStatus, WM_PAINT, 0, 0);
	}
}

bool StringCompare(const std::string & a, const std::string & b)
{
#pragma message("TODO : ���ڿ� �񱳸� �� �� ��� �ҹ��ڷ� �ٲ㼭 ���ؾ���")
	return (strcmp(b.c_str(), a.c_str()) > 0);
}

const std::string RemoveExt(const std::string & strFullFileName)
{
	char szFile[MAX_PATH] = { 0 };
	_splitpath(strFullFileName.c_str(), 0, 0, szFile, 0);

	return szFile;
}

bool FilenameCompare(const std::string & a, const std::string & b)
{
	return (strcmp(RemoveExt(b).c_str(), RemoveExt(a).c_str()) > 0);
}

void ZMain::ZFindFile(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive)
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
			if (wfd.cFileName[0]!='.' && bFindRecursive == true)
			{
				wsprintf(newpath,"%s%s%s\\*.*",drive,dir,wfd.cFileName);
				ZFindFile(newpath, foundStorage, bFindRecursive);
			}
		}
		else
		{
			wsprintf(fname,"%s%s%s",drive,dir,wfd.cFileName);

			if ( ZImage::IsValidImageFileExt(wfd.cFileName) )
			{
				foundStorage.push_back(fname);
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
				wsprintf(fname,"%s%s%s",drive,dir,wfd.cFileName);

				foundStorage.push_back(fname);

				if ( bFindRecursive == true )
				{
					wsprintf(newpath,"%s%s%s\\*.*",drive,dir,wfd.cFileName);
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

	m_vFile.clear();
	ZFindFile(strToFindFolder.c_str(), m_vFile, false);

	// ���� ������ �����Ѵ�.
	sort(m_vFile.begin(), m_vFile.end(), FilenameCompare);

	// Cache Thread �� �����Ѵ�.
	ZCacheImage::GetInstance().SetImageVector(m_vFile);

	if ( m_strCurrentFilename.empty() && !m_vFile.empty())
	{
		m_strCurrentFilename = m_vFile[0];
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
	sort(vFolders.begin(), vFolders.end(), StringCompare);

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

void ZMain::OpenFolder(const std::string strFolder)
{
	// Ư�� ������ ���� ���ϵ��� �˻��ؼ� ���� �� ù��° ������ ����.

	std::string strTemp = strFolder;
	strTemp += "\\*.*";

	vector < std::string > vFiles;
	ZFindFile(strTemp.c_str(), vFiles, false);

	sort(vFiles.begin(), vFiles.end(), FilenameCompare);

	if ( vFiles.size() == 0 )
	{
		std::string strMsg = strFolder;
		strMsg += " folder has no image file.";
		MessageBox(m_hMainDlg, strMsg.c_str(), "ZViewer", MB_OK);
		return;
	}
	else
	{
		OpenFile(vFiles[0]);
	}
}


void ZMain::OpenFile(const string & strFilename)
{
	m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
	RescanFolder();

	// ��ĵ�� ���� �� ���� ������ ã�´�.
	std::vector<std::string>::iterator it = find(m_vFile.begin(), m_vFile.end(), strFilename);

	if ( m_vFile.size() <= 0 )
	{
		_ASSERTE(!"size of scanned file list is 0.");

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
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex];
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
	ShowWindow(m_hStatus, SW_HIDE);
	SetMenu(m_hMainDlg, NULL);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

void ZMain::FormShow()
{
	LONG style = GetWindowLong(m_hMainDlg, GWL_STYLE);
	style |= WS_CAPTION;
	style |= WS_THICKFRAME;
	ShowWindow(m_hStatus, SW_SHOW);
	SetMenu(m_hMainDlg, m_hMainMenu);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

void ZMain::ToggleFullScreen()
{
	static RECT lastPosition;

	if ( m_option.m_bFullScreen )	// ���� Ǯ��ũ���̸� ���� ȭ������ ���ư���.
	{
		m_option.m_bFullScreen = !m_option.m_bFullScreen;

		ShellTrayShow();	// �������� �۾� ǥ������ �����ش�.

		FormShow();	// �޴�, ���� ǥ���ٵ��� �����ش�.

		SetWindowPos(m_hMainDlg, HWND_TOP, lastPosition.left, lastPosition.top, lastPosition.right - lastPosition.left, lastPosition.bottom - lastPosition.top, SWP_SHOWWINDOW);
	}
	else	// ���� Ǯ��ũ���� �ƴϸ� Ǯ��ũ������ �����.
	{
		m_option.m_bFullScreen = !m_option.m_bFullScreen;
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

	CheckMenuItem(m_hMainMenu, ID_VIEW_FULLSCREEN, m_option.m_bFullScreen ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_FULLSCREEN, m_option.m_bFullScreen ? MF_CHECKED : MF_UNCHECKED);
}

void ZMain::ToggleBigToScreenStretch()
{
	m_option.m_bBigToSmallStretchImage = !m_option.m_bBigToSmallStretchImage;

	CheckMenuItem(m_hMainMenu, ID_VIEW_BIGTOSCREENSTRETCH , m_option.m_bBigToSmallStretchImage ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_BIGTOSCREENSTRETCH, m_option.m_bBigToSmallStretchImage ? MF_CHECKED : MF_UNCHECKED);

	Draw();
}

void ZMain::SetStatusBarText()
{
	char szTemp[256];

	if ( m_vFile.size() == 0 || m_strCurrentFilename.empty() )
	{
		// File Index
		sprintf(szTemp, "No File");
		SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

		// �ػ� ����
		sprintf(szTemp, "");
		SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM)szTemp);

		// �̹��� ������
		sprintf(szTemp, "");
		SendMessage(m_hStatus, SB_SETTEXT, 2, (LPARAM)szTemp);

		// �ӽ÷� http://wimy.com
		sprintf(szTemp, "http://wimy.com");
		SendMessage(m_hStatus, SB_SETTEXT, 3, (LPARAM)szTemp);

		// �ε��ð�
		sprintf(szTemp, "");
		SendMessage(m_hStatus, SB_SETTEXT, 4, (LPARAM)szTemp);

		// ���ϸ�
		sprintf(szTemp, "No File");
		SendMessage(m_hStatus, SB_SETTEXT, 5, (LPARAM)szTemp);
	}
	else
	{
		// File Index
		sprintf(szTemp, "%d/%d", m_iCurretFileIndex+1, m_vFile.size());
		SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

		// �ػ� ����
		sprintf(szTemp, "%dx%dx%dbpp", m_currentImage.GetWidth(), m_currentImage.GetHeight(), m_currentImage.GetBPP());
		SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM)szTemp);

		// image size
		long imageSize = m_currentImage.GetImageSize();

		if ( imageSize > 1024 )
		{
			sprintf(szTemp, "%dKByte", m_currentImage.GetImageSize()/1024);
		}
		else
		{
			sprintf(szTemp, "%dByte", m_currentImage.GetImageSize());
		}
		SendMessage(m_hStatus, SB_SETTEXT, 2, (LPARAM)szTemp);



		// �ӽ÷� http://wimy.com
		sprintf(szTemp, "http://wimy.com");
		SendMessage(m_hStatus, SB_SETTEXT, 3, (LPARAM)szTemp);

		// �ε��ð�
		sprintf(szTemp, "%.3fsec", (float)(m_dwLoadingTime / 1000.0));
		SendMessage(m_hStatus, SB_SETTEXT, 4, (LPARAM)szTemp);

		// ���ϸ�
		char szFilename[MAX_PATH], szFileExt[MAX_PATH];
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFilename, szFileExt);

		sprintf(szTemp, "%s%s", szFilename, szFileExt);
		SendMessage(m_hStatus, SB_SETTEXT, 5, (LPARAM)szTemp);

	}
}

void ZMain::SetTitle()
{
	char szTemp[MAX_PATH+256];
	if ( m_strCurrentFilename.empty() )
	{
		sprintf(szTemp, "ZViewer v%s", g_strVersion.c_str());
	}
	else
	{
		char szFileName[MAX_PATH] = { 0 };
		char szFileExt[MAX_PATH] = { 0 };
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFileName, szFileExt);

		//sprintf(szTemp, "%s%s - ZViewer for rubi v%s", szFileName, szFileExt, g_strVersion.c_str() );
		sprintf(szTemp, "%s%s - for rubi :D", szFileName, szFileExt);
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

		DebugPrintf("Cache Hit!!!!!!!!!!!!!\r\n");

		ZCacheImage::GetInstance().LogCacheHit();
	}
	else
	{
		// ĳ�ÿ��� ã�� �� ������ ���� �о���̰�, ĳ�ÿ� �߰��Ѵ�.

		bool bLoadOK = false;

		for ( int i=0; i<10; ++i)
		{
			bLoadOK = m_currentImage.LoadFromFile(m_strCurrentFilename);
			if ( bLoadOK || i >= 5) break;

			DebugPrintf("Direct Load failed. sleep");

			Sleep(100);
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
			if ( !ZCacheImage::GetInstance().hasCachedData(m_strCurrentFilename, m_iCurretFileIndex))
			{
				ZCacheImage::GetInstance().AddCacheData(m_strCurrentFilename, m_currentImage);
			}
			
			DebugPrintf("Cache miss. Add to cache.");
			ZCacheImage::GetInstance().LogCacheMiss();
		}


	}
	m_dwLoadingTime = GetTickCount() - start;
	SetTitle();	// ���ϸ��� ������ Ÿ��Ʋ�ٿ� ���´�.
	SetStatusBarText();

	m_iShowingX = 0;
	m_iShowingY = 0;

	if ( m_option.m_bRightTopFirstDraw )	// ���� ��ܺ��� �����ؾ��ϸ�
	{
		RECT rt;
		GetClientRect(m_hMainDlg, &rt);

		if ( m_currentImage.GetWidth() > rt.right )
		{
			m_iShowingX = m_currentImage.GetWidth() - rt.right;
		}
	}

}

void ZMain::OnDrag(int x, int y)
{
	if ( m_option.m_bBigToSmallStretchImage )
	{
		/// ū �׸��� ȭ�鿡 �°� ��Ʈ��Ī�� ���� �巡�״� ���� �ʾƵ� �ȴ�.
		return;
	}

	RECT rt;
	GetClientRect(m_hMainDlg, &rt);

	if ( m_option.m_bFullScreen == false ) rt.bottom -= STATUSBAR_HEIGHT;

	bool bDraw = false;
	if ( (m_iShowingX + x) >= 0 ) 
	{
		if ( m_iShowingX + x + rt.right >= m_currentImage.GetWidth())
		{
			x = m_currentImage.GetWidth() - rt.right - m_iShowingX;// - 1;
		}

		bDraw = true;
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
		bDraw = true;
		m_iShowingY += y;
	}
	else
	{
		m_iShowingY = 0;
	}

	if ( bDraw ) Draw(false);
}

void ZMain::ShellTrayShow()
{
	// �۾� ǥ������ ���̰� ���ش�.
	HWND h = FindWindow("Shell_TrayWnd", "");

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_SHOW);
	}
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

void ZMain::OnFocusLose()
{
	DebugPrintf("OnFocusLose()");

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
	DebugPrintf("OnFocusGet()");
	if ( m_option.m_bFullScreen )
	{
		DebugPrintf("OnFocusGet() at fullscreen");
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		ShellTrayHide();
	}
}

void ZMain::StartBannerDownload()
{
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(0, 0, BannerThreadFunc, this, 0, &dwThreadID);

	if ( hThread == INVALID_HANDLE_VALUE )
	{
		_ASSERTE(!"Can't make thread");
	}
	CloseHandle(hThread);
}

DWORD WINAPI ZMain::BannerThreadFunc(LPVOID p)
{
	/*
	fipInternetIO i;
	if ( i.downloadFile("http://www.wimy.com/images/zviewerbanner.jpg") )
	{
		fipImage k;
		k.loadFromHandle(&i, (fi_handle)&i, 0);
		if ( k.save((ZMain::GetInstance().m_strProgramFolder + "\\banner.jpg").c_str(), JPEG_QUALITYSUPERB) == FALSE )
		{
			_ASSERTE(!"Can't banner save");
		}
	}
	else
	{
//		_ASSERTE(!"Can't download");
	}
	*/

	return 0;
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
		m_strCurrentFilename = m_vFile[m_iCurretFileIndex];
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
	m_option.m_bRightTopFirstDraw = !m_option.m_bRightTopFirstDraw;

	CheckMenuItem(m_hMainMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, m_option.m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, m_option.m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
}

void ZMain::ShowFileExtDlg()
{
	ZFileExtDlg::GetInstance().ShowDlg();
}

void ZMain::DeleteThisFile()
{
	/// TODO : LoadString() �� �̿��ؼ� ���ҽ����� �ҷ������� �����ؾ���.
	int iRet = MessageBox(m_hMainDlg, ZResourceManager::GetInstance().GetString(IDS_DELETE_THIS_FILE).c_str(), "ZViewer", MB_YESNO);

	if ( iRet == IDYES )
	{
		if ( 0 == unlink(m_strCurrentFilename.c_str()) )
		{
			// ���� ������ ������ �����ΰ�?
			if ( m_vFile.size() <= 1 )
			{
				m_iCurretFileIndex = 0;
				m_strCurrentFilename = "";
				
				m_vFile.clear();

				SetTitle();
				SetStatusBarText();
				Draw(true);


			}
			else
			{
				if ( ((int)m_vFile.size() - 1) > m_iCurretFileIndex )	// ���� �׸��� �ִ�.
				{
					// for ���� ���鼭 ���� ���� ã�Ƴ��´�.
					vector<std::string>::iterator it, endit = m_vFile.end();
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
					vector<std::string>::iterator it, endit = m_vFile.end();
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
		else
		{
			MessageBox(m_hMainDlg, "Can't delete this file!", "ZViewer", MB_OK);
		}
	}
}

/*
void ZMain::LoadLanguage()
{
	ZINIOption opt;
	opt.LoadFromFile("language/korean.ini");

	// �޴��� �����Ѵ�.
	ModifyMenu(m_hPopupMenu, ID_DELETETHISFILE, MF_BYCOMMAND, ID_DELETETHISFILE, opt.GetValue("MenuFileOpen").c_str());
	//SetDlgItemText(m_hPopupMenu, ID_DELETETHISFILE, opt.GetValue("MenuDeleteThisFile").c_str());
}
*/

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

	std::string strSaveFileName = szSystemFolder;
	strSaveFileName += "\\rubi_bg.bmp";

	if ( FALSE == m_currentImage.SaveToFile(strSaveFileName, BMP_DEFAULT) )
	{
		_ASSERTE(false);
		return;
	}


	CDesktopWallPaper wallPaper(strSaveFileName);
	wallPaper.SetWallPaperStyle(style);
	wallPaper.SetDesktopWallPaper();
}

void ZMain::ClearDesktopWallPaper()
{
	CDesktopWallPaper::ClearDesktopWallPaper();
}