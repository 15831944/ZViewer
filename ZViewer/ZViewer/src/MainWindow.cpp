/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MainWindow.cpp
 *
 * 2006.11.4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "resource.h"

#include "MainWindow.h"
#include "AboutWindow.h"

#include "../commonSrc/MessageManager.h"

#include "src/ZMain.h"
#include "src/ZResourceManager.h"
#include "src/ZOption.h"

enum
{
	ARROW_MOVEMENT_LENGTH = 100		///< ȭ�麸�� ū �׸��� �� ��, ����Ű�� ������ �� �����̴� ����
};


HMENU g_hPopupMenu;

int CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

CMainWindow::CMainWindow()
{
	SetWndProc();
}


CMainWindow::~CMainWindow()
{
}


void CMainWindow::SetWndProc()
{
	m_wndProc = (WNDPROC)WndProc;
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

/// ����� ��忡���� �۵��ϴ� ����Ű��
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
				MessageBox(hWnd, GetMessage(TEXT("INVALID_IMAGE_FILE")), TEXT("ZViewer"), MB_OK);
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
			ZMain::GetInstance().SetHWND(hWnd);

			InitCommonControls();
			// �������� �������ش�.
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(ZMain::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDI_BIG_MAIN)));

			ZMain::GetInstance().CreateStatusBar();

			// �˾� �޴��� �ҷ����´�.
			HMENU hMenu = LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_POPUP_MENU));
			g_hPopupMenu = GetSubMenu(hMenu, 0);
			ZMain::GetInstance().SetPopupMenu(g_hPopupMenu);

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

			case ID_FILE_COPYTO:
				ZMain::GetInstance().CopyThisFile();
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
				{
					CAboutWindow win;
					win.DoResource(hWnd);
				}
				break;

			case ID_ACCELERATOR_CANCEL_FULLSCREEN:
				// ���� Ǯ ��ũ�� �� ���� Ǯ��ũ���� ����Ѵ�.
				if ( ZOption::GetInstance().IsFullScreen() )
				{
					ZMain::GetInstance().ToggleFullScreen();
				}
				break;

			case ID_MOVE_UNDOIMAGEPOSITION:
				ZMain::GetInstance().Undo();
				ZMain::GetInstance().Draw();
				break;

			case ID_MOVE_REDOIMAGEPOSITION:
				ZMain::GetInstance().Redo();
				ZMain::GetInstance().Draw();
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