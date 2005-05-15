/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	Main.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "ZMain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "src/ZCacheImage.h"

int CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

HMENU g_hPopupMenu;


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance ,LPSTR lpszCmdParam,int nCmdShow)
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

	if ( strcmp(lpszCmdParam, "/fileext") == 0 )	
	{
		int iRet = MessageBox(HWND_DESKTOP, ZResourceManager::GetInstance().GetString(IDS_ASSOCIATE_FILE_EXTS).c_str(), "ZViewer", MB_YESNO);

		if ( iRet == IDYES )
		{
			ZFileExtDlg::GetInstance().SaveExtEnv();
		}
		return 0;
	}
	else if ( strcmp(lpszCmdParam, "/freezvieweragent") == 0 )	// uninstall �� �� ZViewerAgent �� unload �Ѵ�.
	{
		CoFreeUnusedLibraries();
		return 0;
	}

	std::string strCmdString;

	// ������ ���� ���� ����ǥ�� �ѷ��μ� �ش�. �׷��� ����ǥ�� �����Ѵ�.
	if ( strlen(lpszCmdParam) > 0 )
	{
		// ���� ����ǥ�� �����ϰ� ������(����ȭ�鿡�� ������ ��)
		size_t iLen = strlen(lpszCmdParam);
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
	char szTemp[MAX_PATH] = { 0 };
	GetLongPathName(strCmdString.c_str(), szTemp, MAX_PATH);

	strCmdString = szTemp;

	std::string strInitArg = strCmdString;


	//MessageBox(HWND_DESKTOP, strInitArg.c_str(), "sf", MB_OK);

#ifdef _DEBUG
	strInitArg = "C:\\_Samples\\19028-1.jpg";
	//strInitArg = "C:\\A.bmp";
#endif
	ZImage::StartupLibrary();

	ZMain::GetInstance().setInitArg(strInitArg);

	ZMain::GetInstance().SetInstance(hInstance);


	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	char lpszClass[256] = "ZViewer";

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
		lpszClass,		// Window Title
		WS_OVERLAPPEDWINDOW,// | WS_EX_ACCEPTFILES,
		iXPosition,		// �⺻ x ��ġ
		iYPosition,		// �⺻ y ��ġ
		iWidth,				// width
		iHeight,			// height
		NULL,
		hMenu,	// MainMenu
		hInstance,NULL);

	// Ž���⿡���� Drag&Drop �� �����ϰ� �Ѵ�.
	DragAcceptFiles(hWnd, TRUE);

	ShowWindow(hWnd,nCmdShow);

	// ����Ű ����. ���⼭ ��ȯ�� �ڵ��� ���α׷��� ����� �� �ڵ������� close ��.
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCELERATOR));

	while(GetMessage(&Message,0,0,0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &Message))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	//
	if ( hLang )
	{
		if ( FALSE == FreeLibrary(hLang) )
		{
			_ASSERTE(!"Can't free language dll.");
		}
	}

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

			}
		}
		break;

	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;

			UINT iFileNum = 0;

			iFileNum = DragQueryFile(hDrop, 0xffffffff, 0, 0);

			if ( iFileNum <= 0 ) break;

			char szFileName[MAX_PATH] = { 0 };
			DragQueryFile(hDrop, 0, szFileName, MAX_PATH);

			if ( ZImage::IsValidImageFileExt(szFileName) )
			{
				ZMain::GetInstance().OpenFile(szFileName);
			}
			else
			{
				MessageBox(hWnd, "Invalid image file", "ZViewer", MB_OK);
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
			HWND hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "Status line", hWnd, 0);
			ZMain::GetInstance().SetStatusHandle(hStatusBar);

			// StatusBar �� split �Ѵ�.
			int SBPart[6] =
			{
				70,		/// %d/%d ���纸�� �ִ� �̹��� ������ index number
				200,	/// %dx%dx%dbpp �ػ󵵿� color depth, image size
				300,		/// image size
				420,	/// temp banner http://www.wimy.com
				500,	/// ������ �о���̴µ� �ɸ� �ð�
				2000,	/// ���ϸ�ǥ��
			};
			SendMessage(hStatusBar, SB_SETPARTS, 6, (LPARAM)SBPart);

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
			if ( ZMain::GetInstance().IsFullScreen() )
			{
				ZMain::GetInstance().ShellTrayShow();
			}
			PostQuitMessage(0);
		}
		break;

	case WM_SETCURSOR:
		{
			RECT rt;
			GetClientRect(hWnd, &rt);
			if ( ZMain::GetInstance().IsFullScreen() == false ) rt.bottom -= STATUSBAR_HEIGHT;	// StatusBar �� ���� ����.

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);

			if ( PtInRect(&rt, pt) )
			{
				if ( ZMain::GetInstance().m_bHandCursor )
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

			RECT rt;
			GetClientRect(hWnd, &rt);
			if ( ZMain::GetInstance().IsFullScreen() == false ) rt.bottom -= STATUSBAR_HEIGHT;

			ZMain::GetInstance().OnChangeCurrentSize(rt.right, rt.bottom);
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
		ZMain::GetInstance().m_bHandCursor = false;
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

			case ID_MAINMENU_FILE_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;

			case ID_MAINMENU_FILE_OPEN:
				ZMain::GetInstance().OpenFileDialog();
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
				ZMain::GetInstance().ChangeFilrSort(eFileSortOrder_FILENAME);
				break;

			case ID_SORT_FILESIZE:
				ZMain::GetInstance().ChangeFilrSort(eFileSortOrder_FILESIZE);
				break;

			case ID_SORT_FILEDATE:
				ZMain::GetInstance().ChangeFilrSort(eFileSortOrder_LAST_MODIFY_TIME);
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
					if ( ZMain::GetInstance().IsFullScreen() )
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
				ZMain::GetInstance().OnDrag(100, 0);
				break;

			case ID_ACCELERATOR_LEFT:
				ZMain::GetInstance().OnDrag(-100, 0);
				break;

			case ID_ACCELERATOR_UP:
				ZMain::GetInstance().OnDrag(0, -100);
				break;

			case ID_ACCELERATOR_DOWN:
				ZMain::GetInstance().OnDrag(0, 100);
				break;

			case ID_VIEW_RIGHTTOPFIRSTDRAW:
				ZMain::GetInstance().OnRightTopFirstDraw();
				break;

			case ID_OPTION_FILE_EXT:
				ZMain::GetInstance().ShowFileExtDlg();
				break;

			case ID_VIEW_BIGTOSCREENSTRETCH:
			case ID_POPUPMENU_BIGTOSCREENSTRETCH:
				ZMain::GetInstance().ToggleBigToScreenStretch();
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
				ZMain::GetInstance().ClearDesktopWallPaper();
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

		DebugPrintf("Recv WM_PAINT");
		return 0;
	case WM_DESTROY:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
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

			char szTemp[MAX_PATH];
			_snprintf(szTemp, sizeof(szTemp), "CacheHitRate : %d%%", ZMain::GetInstance().GetLogCacheHitRate());
			SetDlgItemText(hWnd, IDC_STATIC_HITRATE, szTemp);

			NUMBERFMT nFmt = { 0, 0, 3, ".", ",", 1 };

			TCHAR szOUT[20];
			_snprintf(szTemp, sizeof(szTemp), "%d",ZMain::GetInstance().GetCachedKByte());
			::GetNumberFormat(NULL, NULL, szTemp, &nFmt, szOUT, 20);

			_snprintf(szTemp, sizeof(szTemp), "CachedMemory : %sKB, Num of Cached Image : %d", szOUT, ZCacheImage::GetInstance().GetNumOfCacheImage());
			SetDlgItemText(hWnd, IDC_STATIC_CACHE_MEMORY, szTemp);

			_snprintf(szTemp, sizeof(szTemp), "ProgramPath : %s", ZMain::GetInstance().GetProgramFolder().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_PROGRAM_PATH, szTemp);

			_snprintf(szTemp, sizeof(szTemp), "Library Version : %s", ZImage::GetLibraryVersion());
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