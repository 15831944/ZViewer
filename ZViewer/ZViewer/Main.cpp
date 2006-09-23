/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	Main.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

/**
	@mainpage ZViewer's Document

	ZViewer �� GPL �� ���� �����Ǵ� �̹��� ��� ���α׷��Դϴ�.

	Created by zelon (JKim, Jinwook in Korea)

	Project Homepage : http://kldp.net/projects/zviewer/

	Createor Homepage : http://www.wimy.com


	�ֿ� ����� ������ �����ϴ�.

	- �پ��� Ȯ���ڸ� ���� �̹��� ������ �� �� �ֽ��ϴ�.

	- Ž���⿡�� ������ ��ư�� ���� �̸����Ⱑ �����մϴ�.

	- �̹��� ���ϵ��� ĳ���صξ� ���� �̹��� ���Ⱑ �����մϴ�.

	- ���� ũ�Ⱑ �۾� ���� �ε��˴ϴ�.

	- �̹��� ������ �ٸ� �������� ��ȯ�� �����մϴ�.

	- ȭ�� ũ��� �°� Ȯ��/����Ͽ� �̹����� �� �� �ֽ��ϴ�.

*/

#include "stdafx.h"
#include "resource.h"
#include "src/ZMain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "src/ZCacheImage.h"
#include "src/ZOption.h"
#include "../lib/LogManager.h"

#ifdef _DEBUG
#include "vld/vld.h"
#endif

int CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

HMENU g_hPopupMenu;

enum
{
	ARROW_MOVEMENT_LENGTH = 100
};


int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance ,LPTSTR lpszCmdParam,int nCmdShow)
{
	// �⺻���� ������� ������Ʈ�� �ִ� �����̴�.
	ZResourceManager::GetInstance().SetHandleInstance(hInstance);

	HINSTANCE hLang = NULL;

	// debug mode ������ �׻� ������(����� �׽�Ʈ�� ���ؼ�)
#ifndef _DEBUG
	if ( GetSystemDefaultLangID() == 0x0412 )
	{
		hLang = LoadLibrary("language/korean.dll");

		if ( hLang )
		{
			ZResourceManager::GetInstance().SetHandleInstance(hLang);
		}
		else
		{
			_ASSERTE(hLang != NULL);
		}
	}
#endif

	/// ���� Ȯ���ڸ� �����϶�� �Ÿ�

//	MessageBox(HWND_DESKTOP, strInitArg.c_str(), "asdf", MB_OK);

	if ( _tcscmp(lpszCmdParam, TEXT("/fileext")) == 0 )	
	{
		int iRet = MessageBox(HWND_DESKTOP, ZResourceManager::GetInstance().GetString(IDS_ASSOCIATE_FILE_EXTS).c_str(), TEXT("ZViewer"), MB_YESNO);

		if ( iRet == IDYES )
		{
			ZFileExtDlg::GetInstance().SaveExtEnv();
		}
		return 0;
	}
	else if ( _tcscmp(lpszCmdParam, TEXT("/freezvieweragent")) == 0 )	// uninstall �� �� ZViewerAgent �� unload �Ѵ�.
	{
		CoFreeUnusedLibraries();
		return 0;
	}

	tstring strCmdString;

	// ������ ���� ���� ����ǥ�� �ѷ��μ� �ش�. �׷��� ����ǥ�� �����Ѵ�.
	if ( _tcslen(lpszCmdParam) > 0 )
	{
		// ���� ����ǥ�� �����ϰ� ������(����ȭ�鿡�� ������ ��)
		size_t iLen = _tcslen(lpszCmdParam);
		for ( unsigned int i=0; i<iLen; ++i)
		{
			if ( lpszCmdParam[i] == '\"')
			{
				continue;
			}
			strCmdString.push_back(lpszCmdParam[i]);
		}
	}

	// ~1 �������� �� ���ϸ��� �´ٸ�, ���� �� �н��� ��´�.
	TCHAR szTemp[MAX_PATH] = { 0 };
	GetLongPathName(strCmdString.c_str(), szTemp, MAX_PATH);

	strCmdString = szTemp;

	tstring strInitArg = strCmdString;


	//MessageBox(HWND_DESKTOP, strInitArg.c_str(), "sf", MB_OK);

#ifdef _DEBUG
	strInitArg = TEXT("Z:\\(__�����ڷ�__)\\�ܾ���� ������\\19028-1.jpg");
	//strInitArg = "C:\\A.bmp";
#endif
	ZImage::StartupLibrary();

	ZMain::GetInstance().SetInitArg(strInitArg);

	ZMain::GetInstance().SetInstance(hInstance);


	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	TCHAR lpszClass[256] = TEXT("ZViewer");

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIG_MAIN));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	int iWidth = 1000;
	int iHeight = 700;
	int iXPosition = (GetSystemMetrics(SM_CXSCREEN)/2) - (iWidth/2);
	int iYPosition = (GetSystemMetrics(SM_CYSCREEN)/2) - ( iHeight/2) ;

	HMENU hMenu = (HMENU)LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_MAIN_MENU));

	ZMain::GetInstance().SetMainMenu(hMenu);

	hWnd=CreateWindow(
		lpszClass,
		lpszClass,		///< Window Title
		WS_OVERLAPPEDWINDOW,///< | WS_EX_ACCEPTFILES,
		iXPosition,		///< �⺻ x ��ġ
		iYPosition,		///< �⺻ y ��ġ
		iWidth,			///< width
		iHeight,		///< height
		NULL,
		hMenu,			///< MainMenu
		hInstance,NULL);

	// Ž���⿡���� Drag&Drop �� �����ϰ� �Ѵ�.
	DragAcceptFiles(hWnd, TRUE);

	ShowWindow(hWnd,nCmdShow);

	// ����Ű ����. ���⼭ ��ȯ�� �ڵ��� ���α׷��� ����� �� �ڵ������� close ��.
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCELERATOR));

	/// Add Timer
	enum
	{
		eTimerValue = 9153
	};
	UINT_PTR timerPtr = SetTimer(hWnd, eTimerValue, 100, NULL);

	if ( timerPtr == 0 )
	{
		MessageBox(hWnd, TEXT("Can't make timer"), TEXT("ZViewer"), MB_OK);
		return 0;
	}


	while(GetMessage(&Message,0,0,0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &Message))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	/*
	/// KillTimer
	if ( timerPtr != 0 )
	{
		BOOL bRet = KillTimer(hWnd, eTimerValue);

		_ASSERTE(bRet);
	}
	*/

	//
	if ( hLang )
	{
		if ( FALSE == FreeLibrary(hLang) )
		{
			_ASSERTE(!"Can't free language dll.");
		}
	}

	ZCacheImage::GetInstance().CleanUp();
	CLogManager::getInstance().CleanUp();
	ZImage::CleanupLibrary();

	return (int)Message.wParam;
}

int CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static bool m_bCapture = false;
	static int lastX;
	static int lastY;

	switch(iMessage)
	{
	case WM_CHAR:
		{
			switch ( wParam )
			{
			case 'A':
			case 'a':
			case '7':
				ZMain::GetInstance().OnDrag(-5000, -5000);
				break;

			case 'S':
			case 's':
			case '9':
				ZMain::GetInstance().OnDrag(5000, -5000);
				break;

			case 'Z':
			case 'z':
			case '1':
				ZMain::GetInstance().OnDrag(-5000, 5000);
				break;

			case 'X':
			case 'x':
			case '3':
				ZMain::GetInstance().OnDrag(5000, 5000);
				break;

#ifdef _DEBUG
			case '`':
				ZCacheImage::GetInstance().debugShowCacheInfo();
				break;

			case '~':
				{
					DWORD dwStart = GetTickCount();
					ZCacheImage::GetInstance().clearCache();
					DWORD dwClearTime = GetTickCount() - dwStart;

					DebugPrintf(TEXT("clear spend time : %d"), dwClearTime);
				}
				break;

			case '0':
				{
					fipMultiPage image;
					image.open("C:\\_Samples\\ljyzzno_2.gif", FALSE, TRUE);

					int iGetPageCount = image.getPageCount();

					FIBITMAP * pBitmap = FreeImage_Clone(image.lockPage(0));

					fipImage a;
					a = pBitmap;


					int k = 0;
					++k;
				}
				break;
#endif
			}
		}
		break;

	case WM_DROPFILES:///< Ž���⿡�� �巡�� �� ������� ������ ��
		{
			HDROP hDrop = (HDROP)wParam;

			UINT iFileNum = 0;

			iFileNum = DragQueryFile(hDrop, 0xffffffff, 0, 0);

			if ( iFileNum <= 0 ) break;

			TCHAR szFileName[MAX_PATH] = { 0 };
			DragQueryFile(hDrop, 0, szFileName, MAX_PATH);

			if ( ZImage::IsValidImageFileExt(szFileName) )
			{
				ZMain::GetInstance().OpenFile(szFileName);
			}
			else
			{
				MessageBox(hWnd, TEXT("Invalid image file"), TEXT("ZViewer"), MB_OK);
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		ZMain::GetInstance().OpenFileDialog();
		break;

	case WM_LBUTTONDOWN:
		{
			m_bCapture = true;
			SetCapture(hWnd);

			lastX = LOWORD(lParam);
			lastY = HIWORD(lParam);
		}
		break;

	case WM_LBUTTONUP:
		{
			m_bCapture = false;
			ReleaseCapture();
		}
		break;

	case WM_MOUSEMOVE:
		{
			if ( m_bCapture )
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				int diffX = x - lastX;
				int diffY = y - lastY;

				ZMain::GetInstance().OnDrag(-diffX, -diffY);
				lastX = x;
				lastY = y;
			}
		}
		break;

	case WM_CREATE:
		{
			InitCommonControls();
			// �������� �������ش�.
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDI_BIG_MAIN)));

			// StatusBar �� �����Ѵ�.
			HWND hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, TEXT("Status line"), hWnd, 0);
			ZMain::GetInstance().SetStatusHandle(hStatusBar);

			// StatusBar �� split �Ѵ�. �Ʒ��� ���ڴ� ũ�Ⱑ �ƴ϶� ���� ��ġ��� ���� ���!!!!!!!
			int SBPart[7] =
			{
				70,		/// %d/%d ���纸�� �ִ� �̹��� ������ index number
				200,	/// %dx%dx%dbpp �ػ󵵿� color depth, image size
				300,		/// image size
				420,	/// temp banner http://www.wimy.com
				500,	/// ������ �о���̴µ� �ɸ� �ð�
				553,		/// cache status
				1860,	/// ���ϸ�ǥ��
			};
			SendMessage(hStatusBar, SB_SETPARTS, 7, (LPARAM)SBPart);

			// �˾� �޴��� �ҷ����´�.
			HMENU hMenu = LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_POPUP_MENU));
			g_hPopupMenu = GetSubMenu(hMenu, 0);
			ZMain::GetInstance().SetPopupMenu(g_hPopupMenu);

			ZMain::GetInstance().SetHWND(hWnd);
			ZMain::GetInstance().OnInit();
			return TRUE;
		}
		break;

	case WM_CLOSE:
		{
			if ( ZOption::GetInstance().IsFullScreen() )
			{
				ZMain::GetInstance().ShellTrayShow();
			}
			PostQuitMessage(0);
		}
		break;

	case WM_SETCURSOR:
		{
			RECT rt;
			ZMain::GetInstance().getCurrentScreenRect(rt);

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);

			if ( PtInRect(&rt, pt) )
			{
				if ( ZMain::GetInstance().IsHandCursor() )
				{
					if ( HIWORD(lParam) == 513 )	// ���콺 ���� ��ư�� ������ ������
					{
						SetCursor(LoadCursor(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDC_MOVE_HAND_CAPTURE_CURSOR)));
					}
					else
					{
						SetCursor(LoadCursor(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDC_MOVE_HAND_CURSOR)));
					}

					//DebugPrintf("LoadWait");
				}
				else
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
					//DebugPrintf("LoadArrow");
				}
				return 0;
			}

		}
		break;

	case WM_MOUSEWHEEL:		// WM_MOUSEWHEEL
		{
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			//DebugPrintf("Wheel Delta : %d", zDelta);

#pragma message("ToDo : ���� �ް��� ���ư� ���� �������� �̹����� �پ�Ѱ�")

			bool bDraw = false;
			if ( zDelta < 0 )
			{
				bDraw = ZMain::GetInstance().NextImage();
			}
			else
			{
				bDraw = ZMain::GetInstance().PrevImage();
			}

			// �̹����� �ѱ��� �ʾ����� ���� �׸��� �ʴ´�.
			if ( bDraw ) ZMain::GetInstance().Draw();
		}
		break;

	case WM_SIZE:
		{
			SendMessage(ZMain::GetInstance().GetStatusHandle(), WM_SIZE, wParam, lParam);
			ZMain::GetInstance().OnWindowResized();
		}
		break;

	case WM_MBUTTONDOWN:
		ZMain::GetInstance().ToggleFullScreen();
		break;

	case WM_SETFOCUS:
		ZMain::GetInstance().OnFocusGet();
		break;

	case WM_KILLFOCUS:
		ZMain::GetInstance().OnFocusLose();
		break;

	case WM_CONTEXTMENU:
		ZMain::GetInstance().SetHandCursor(false);
		TrackPopupMenu(g_hPopupMenu, TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), 0, hWnd, NULL);
		break;

	case WM_COMMAND:
		{
			int wMid = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);

			switch ( wParam )
			{
			case IDOK:
			case ID_MAINMENU_FILE_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}

			switch ( wMid )
			{
				/////////////////////////////////////////////
				// Main Menu

			case ID_SHOW_CACHED_FILENAME:
				ZCacheImage::GetInstance().ShowCachedImageToOutputWindow();
				break;

			case ID_MAINMENU_FILE_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;

			case ID_MAINMENU_FILE_OPEN:
				ZMain::GetInstance().OpenFileDialog();
				break;

			case ID_FILE_SAVE_AS_NEW_EXT:
				ZMain::GetInstance().SaveFileDialog();
				break;

			case ID_FILE_MOVETO:
				ZMain::GetInstance().MoveThisFile();
				break;

			case ID_MOVE_NEXTFOLDER:
				ZMain::GetInstance().NextFolder();
				break;

			case ID_MOVE_PREVFOLDER:
				ZMain::GetInstance().PrevFolder();
				break;

			case ID_MOVE_NEXTIMAGE:
				{
					if ( ZMain::GetInstance().NextImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_PREVIMAGE:
				{
					if ( ZMain::GetInstance().PrevImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_NEXT_JUMP:
				{
					if ( ZMain::GetInstance().MoveRelateIndex(+10) )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_PREV_JUMP:
				{
					if ( ZMain::GetInstance().MoveRelateIndex(-10) )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;
			case ID_MOVE_FIRSTIMAGE:
				{
					if ( ZMain::GetInstance().FirstImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_MOVE_LASTIMAGE:
				{
					if ( ZMain::GetInstance().LastImage() )
					{
						ZMain::GetInstance().Draw();
					}
				}
				break;

			case ID_VIEW_FULLSCREEN:
				ZMain::GetInstance().ToggleFullScreen();
				break;

			case ID_SORT_FILENAME:
				ZMain::GetInstance().ChangeFileSort(eFileSortOrder_FILENAME);
				break;

			case ID_SORT_FILESIZE:
				ZMain::GetInstance().ChangeFileSort(eFileSortOrder_FILESIZE);
				break;

			case ID_SORT_FILEDATE:
				ZMain::GetInstance().ChangeFileSort(eFileSortOrder_LAST_MODIFY_TIME);
				break;

			case ID_FILE_RESCAN_FOLDER:
				ZMain::GetInstance().ReLoadFileList();
				break;

			case ID_HELP_ABOUT:
				DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_DIALOGHELP), hWnd, AboutWndProc);
				break;

			case ID_ACCELERATOR_CANCEL_FULLSCREEN:
				{
					// ���� Ǯ ��ũ�� �� ���� Ǯ��ũ���� ����Ѵ�.
					if ( ZOption::GetInstance().IsFullScreen() )
					{
						ZMain::GetInstance().ToggleFullScreen();
					}
				}
				break;

			case ID_MOVE_UNDOIMAGEPOSITION:
				{
					ZMain::GetInstance().Undo();
					ZMain::GetInstance().Draw();
				}
				break;

			case ID_MOVE_REDOIMAGEPOSITION:
				{
					ZMain::GetInstance().Redo();
					ZMain::GetInstance().Draw();
				}
				break;

			case ID_ACCELERATOR_RIGHT:
				ZMain::GetInstance().OnDrag(ARROW_MOVEMENT_LENGTH, 0);
				break;

			case ID_ACCELERATOR_LEFT:
				ZMain::GetInstance().OnDrag(-ARROW_MOVEMENT_LENGTH, 0);
				break;

			case ID_ACCELERATOR_UP:
				ZMain::GetInstance().OnDrag(0, -ARROW_MOVEMENT_LENGTH);
				break;

			case ID_ACCELERATOR_DOWN:
				ZMain::GetInstance().OnDrag(0, ARROW_MOVEMENT_LENGTH);
				break;

			case ID_VIEW_RIGHTTOPFIRSTDRAW:
				ZMain::GetInstance().OnRightTopFirstDraw();
				break;

			case ID_OPTION_VIEWLOOP:
				ZMain::GetInstance().ToggleLoopImage();
				break;

			case ID_OPTION_FILE_EXT:
				ZMain::GetInstance().ShowFileExtDlg();
				break;

			case ID_VIEW_BIGTOSCREENSTRETCH:
			case ID_POPUPMENU_BIGTOSCREENSTRETCH:
				ZMain::GetInstance().ToggleBigToScreenStretch();
				break;

			case ID_VIEW_SMALLTOSCREENSTRETCH:
			case ID_POPUPMENU_SMALLTOSCREENSTRETCH:
				ZMain::GetInstance().ToggleSmallToScreenStretch();
				break;

			case ID_VIEW_ROTATECLOCKWISE:
				ZMain::GetInstance().Rotate(true);
				break;

			case ID_VIEW_ROTATECOUNTERCLOCKWISE:
				ZMain::GetInstance().Rotate(false);
				break;

			case ID_DELETETHISFILE:
				ZMain::GetInstance().DeleteThisFile();
				break;

			case ID_SETDESKTOPWALLPAPER_CENTER:
				ZMain::GetInstance().SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_CENTER);
				break;

			case ID_SETDESKTOPWALLPAPER_TILE:
				ZMain::GetInstance().SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_TILE);
				break;

			case ID_SETDESKTOPWALLPAPER_STRETCH:
				ZMain::GetInstance().SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_STRETCH);
				break;

			case ID_SETDESKTOPWALLPAPER_CLEAR:
				CDesktopWallPaper::ClearDesktopWallPaper();
				break;
				// End of Main Menu
				/////////////////////////////////////////////

			}
		}
		break;

	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		hdc=BeginPaint(hWnd, &ps);

		ZMain::GetInstance().Draw();
		EndPaint(hWnd, &ps);

		SendMessage(ZMain::GetInstance().GetStatusHandle(), WM_PAINT, wParam, lParam);

		DebugPrintf(TEXT("Recv WM_PAINT"));
		return 0;
	case WM_DESTROY:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return 0;

	case WM_TIMER:
		ZMain::GetInstance().onTimer();
		return 0;
	}
	return (int)(DefWindowProc(hWnd,iMessage,wParam,lParam));

}

int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemText(hWnd, IDC_STATIC_VERSION, g_strVersion.c_str());

			TCHAR szTemp[MAX_PATH];
			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("CacheHitRate : %d%%"), ZMain::GetInstance().GetLogCacheHitRate());
			SetDlgItemText(hWnd, IDC_STATIC_HITRATE, szTemp);

			NUMBERFMT nFmt = { 0, 0, 3, TEXT("."), TEXT(","), 1 };

			TCHAR szOUT[20];
			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%d"),ZMain::GetInstance().GetCachedKByte());
			::GetNumberFormat(NULL, NULL, szTemp, &nFmt, szOUT, 20);

			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("CachedMemory : %sKB, Num of Cached Image : %d"), szOUT, ZCacheImage::GetInstance().GetNumOfCacheImage());
			SetDlgItemText(hWnd, IDC_STATIC_CACHE_MEMORY, szTemp);

			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("ProgramPath : %s"), ZMain::GetInstance().GetProgramFolder().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_PROGRAM_PATH, szTemp);

			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("Library Version : %s"), ZImage::GetLibraryVersion());
			SetDlgItemText(hWnd, IDC_STATIC_LIBRARY_VERSION, szTemp);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				EndDialog(hWnd, 0);
				break;
			}
		}
		break;
	
	}
	return FALSE;
}