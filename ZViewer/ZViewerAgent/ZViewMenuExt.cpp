/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZViewMenuExt.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

// ZViewMenuExt.cpp : CZViewMenuExt�� �����Դϴ�.

#include "stdafx.h"
#include "ZViewMenuExt.h"
#include "../commonSrc/MessageManager.h"
#include "../commonSrc/SaveAs.h"
#include <shlobj.h>

// CZViewMenuExt

const LONG CZViewMenuExt::m_lMaxThumbnailSize = 128;
const LONG CZViewMenuExt::m_l3DBorderWidth    = 1;
const LONG CZViewMenuExt::m_lMenuItemSpacing  = 4;
const LONG CZViewMenuExt::m_lTotalBorderSpace = 2*(m_lMenuItemSpacing+m_l3DBorderWidth);

void CZViewMenuExt::MsgBox(const tstring & strMsg)
{
	MessageBox(HWND_DESKTOP, strMsg.c_str(), TEXT("ZViewerAgent"), MB_OK);
}


/// �ٸ� �̸����� �����Ѵ�.
void CZViewMenuExt::_SaveAS()
{
	CSaveAs::getInstance().setParentHWND(HWND_DESKTOP);

	TCHAR bufferDir[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, bufferDir);

	CSaveAs::getInstance().setDefaultSaveFilename(bufferDir, m_szFile);

	if ( CSaveAs::getInstance().showDialog() )
	{
		ZImage tempImage;
		if ( !tempImage.LoadFromFile(m_szFile) )
		{
			MessageBox(HWND_DESKTOP, GetMessage(TEXT("CANNOT_LOAD_IMAGE_FILE")), TEXT("ZViewer"), MB_OK);
			return;
		}

		tstring strSaveFileName = CSaveAs::getInstance().getSaveFileName();

		if ( false == tempImage.SaveToFile(strSaveFileName, 0) )
		{
			::MessageBox(HWND_DESKTOP, GetMessage(TEXT("CANNOT_SAVE_AS_FILE")), TEXT("ZViewer"), MB_OK);
		}
	}
}

STDMETHODIMP CZViewMenuExt::Initialize (LPCITEMIDLIST pidlFolder, LPDATAOBJECT  pDO, HKEY hkeyProgID )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( GetSystemDefaultLangID() == 0x0412 )
	{
		CMessageManager::getInstance().SetLanguage(eLanguage_KOREAN);
	}

	COleDataObject dataobj;
	HGLOBAL hglobal;
	HDROP hdrop;

    dataobj.Attach ( pDO, FALSE );      // FALSE = don't release IDataObject interface when destroyed

    // Get the first selected file name.  I'll keep this simple and just check
    // the first name to see if it's a .BMP.
    hglobal = dataobj.GetGlobalData ( CF_HDROP );

    if ( NULL == hglobal )
        return E_INVALIDARG;

    hdrop = (HDROP) GlobalLock ( hglobal );

    if ( NULL == hdrop )
        return E_INVALIDARG;

	/// Ž������ ������ ��ư�� ������ �� �̸�����â�� ���� ���ΰ�
	bool bPreviewMenuInsert = false;

	/// ���� Shift Ű�� ������ �޴��� ��� ���̸� �̹��� �̸����⸦ �������� �ʴ´�.
	if ( GetKeyState(VK_LSHIFT) >= 0 )
	{
		/// Get the name of the first selected file.
		if ( DragQueryFile( hdrop, 0, m_szFile, MAX_PATH ) )
		{
			/// Is it's extension is valid 'Image File'
			if ( ZImage::IsValidImageFileExt(m_szFile) )
			{
				if ( m_originalImage.LoadFromFile(m_szFile) )
				{
					bPreviewMenuInsert = true;
					//	MsgBox("Here");

					if ( m_originalImage.GetBPP() == 8 )
					{
						m_b8bit = true;
					}
				}
				else /// �̹��� ������ ���� �� ������.
				{
					//	MsgBox("Here2");
				}
			}
		}
	}

	GlobalUnlock ( hglobal );

    return ( bPreviewMenuInsert ? S_OK : E_FAIL );
}


STDMETHODIMP CZViewMenuExt::QueryContextMenu(HMENU hmenu, UINT uIndex, UINT uidCmdFirst, UINT uidCmdLast, UINT uFlags )
{
    // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
    if ( uFlags & CMF_DEFAULTONLY )
	{
        return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );
	}

	bool bUseOwnerDraw = false;
	HINSTANCE hinstShell;

	UINT uID = uidCmdFirst;

    hinstShell = GetModuleHandle ( _T("shell32") );

	if ( NULL != hinstShell )
	{
		DLLGETVERSIONPROC pProc;

		pProc = (DLLGETVERSIONPROC) GetProcAddress(hinstShell, "DllGetVersion");

		if ( NULL != pProc )
		{
			DLLVERSIONINFO rInfo = { sizeof(DLLVERSIONINFO) };

			if ( SUCCEEDED( pProc ( &rInfo ) ))
			{
				if ( rInfo.dwMajorVersion > 4 ||
					rInfo.dwMinorVersion >= 71 )
				{
					bUseOwnerDraw = true;
				}
			}
		}
	}

	// �̹��� ���Ͽ� ���� ������ �����س��´�.
	TCHAR szImageInfo[256] = { 0 };
	StringCchPrintf(szImageInfo, sizeof(szImageInfo), TEXT("ZViewer %dx%d %dbpp"), m_originalImage.GetWidth(), m_originalImage.GetHeight(), m_originalImage.GetBPP());

	// Store the menu item's ID so we can check against it later when
	// WM_MEASUREITEM/WM_DRAWITEM are sent.
	m_uOurItemID = uidCmdFirst;

	MENUITEMINFO mii;

    mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask  = MIIM_ID | MIIM_TYPE;
	mii.fType  = bUseOwnerDraw ? MFT_OWNERDRAW : MFT_BITMAP;
	mii.wID    = uID++;

	if ( !bUseOwnerDraw )
	{
		// NOTE: This will put the full-sized bitmap in the menu, which is
		// obviously a bit less than optimal.  Compressing the bitmap down
		// to a thumbnail is left as an exercise.
		mii.dwTypeData = (LPTSTR) m_bmp.GetSafeHandle();
	}

	InsertMenuItem ( hmenu, uIndex, TRUE, &mii );
	
	/// ���� �޴��� �����.
	HMENU hSubMenu = CreatePopupMenu();
	{
		InsertMenu( hSubMenu, 0, MF_BYPOSITION | MF_STRING, uID++, GetMessage(TEXT("VIEW_IN_ZVIEWER")));
		InsertMenu( hSubMenu, 1, MF_SEPARATOR, NULL, NULL);
		InsertMenu( hSubMenu, 2, MF_BYPOSITION | MF_STRING, uID++, GetMessage(TEXT("DESKTOP_WALLPAPER_CENTER")));
		InsertMenu( hSubMenu, 3, MF_BYPOSITION | MF_STRING, uID++, GetMessage(TEXT("DESKTOP_WALLPAPER_STRETCH")));
		InsertMenu( hSubMenu, 4, MF_BYPOSITION | MF_STRING, uID++, GetMessage(TEXT("DESKTOP_WALLPAPER_TILE")));
		InsertMenu( hSubMenu, 5, MF_SEPARATOR, NULL, NULL);
		InsertMenu( hSubMenu, 6, MF_BYPOSITION | MF_STRING, uID++, GetMessage(TEXT("DESKTOP_WALLPAPER_CLEAR")));
		InsertMenu( hSubMenu, 7, MF_SEPARATOR, NULL, NULL);
		InsertMenu( hSubMenu, 8, MF_BYPOSITION | MF_STRING, uID++, GetMessage(TEXT("ZVIEWERAGENT_SAVE_AS")));

		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask  = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
		mii.wID    = uID++;
		mii.dwTypeData = szImageInfo;
	}

	InsertMenu( hmenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSubMenu, szImageInfo);


	m_uiMaxMenuID = (uID - uidCmdFirst) - 1;

    // Tell the shell we added 1 top-level menu item.
    return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uID - uidCmdFirst );
}

/// mouseover �� �޴��� ����, �� �Լ����� ���� ���ڿ��� Ž������ '����ǥ����' �� �����ش�.
STDMETHODIMP CZViewMenuExt::GetCommandString (UINT uCmd, UINT uFlags, UINT* puReserved, LPSTR pszName, UINT cchMax)
{
	// ���⼭ uCmd �� �޴��� ���� ������� 0 ���� ������� �´�. ��, ������ �� ���� �޴��� ������ ������.

#ifdef _DEBUG
	TCHAR szTemp[256];
	StringCchPrintf(szTemp, sizeof(szTemp), TEXT("ucmd(%d), uFlags(%d), pszName(%s), cchMax(%d)"), uCmd, uFlags, pszName, cchMax);
	OutputDebugString(szTemp);
	OutputDebugString(TEXT("\r\n"));
#endif

	// �޴��� ���� ������ �Ѿ�� �ȵȴ�.
	if ( uCmd >= m_uiMaxMenuID )
	{
		_ASSERTE(false);
		return E_INVALIDARG;
	}

	static LPCTSTR szHelpString = _T("Select this thumbnail to view the entire picture.");

    USES_CONVERSION;


    // If Explorer is asking for a help string, copy our string into the
    // supplied buffer.
    if ( uFlags & GCS_HELPTEXT )
	{
        if ( uFlags & GCS_UNICODE )
		{
            // We need to cast pszName to a Unicode string, and then use the
            // Unicode string copy API.
            lstrcpynW ( (LPWSTR) pszName, T2CW(szHelpString), cchMax );

			/* �Ʒ� �ڵ带 ���캸�� ����� ������ �� �ִ�.
			TCHAR buffer[256];
            StringCchPrintf( buffer, sizeof(buffer), TEXT("uCmd(%d)"), uCmd);

			lstrcpynW ( (LPWSTR) pszName, T2CW(buffer), cchMax );
			*/
		}
        else
		{
            // Use the ANSI string copy API to return the help string.
            lstrcpynA ( pszName, T2CA(szHelpString), cchMax );
		}
	}

    return S_OK;
}

/// �׸� �� �޴��� Ŭ������ �� � ����� ���������� �����ؼ� �����Ѵ�.
STDMETHODIMP CZViewMenuExt::InvokeCommand( LPCMINVOKECOMMANDINFO pInfo )
{
    // If lpVerb really points to a string, ignore this function call and bail out.
    if ( 0 != HIWORD( pInfo->lpVerb ))
	{
        return E_INVALIDARG;
	}

    // �Ѿ�� �� �ִ� command �� ���� üũ�Ѵ�.
	if ( LOWORD( pInfo->lpVerb ) >= m_uiMaxMenuID)
	{
		_ASSERTE(false);
        return E_INVALIDARG;
	}

    // Open the bitmap in the default paint program.
	//ShellExecute ( pInfo->hwnd, _T("open"), m_szFile, NULL, NULL, SW_SHOWNORMAL );
	//MessageBox(HWND_DESKTOP, "Clisdc1sk!!", "ok", MB_OK);

	switch ( LOWORD( pInfo->lpVerb ) )
	{
	case 0:
	case 1:
		ExecZViewer();
		break;

	case 2:
		// center
		SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_CENTER);
		break;

	case 3:
		// stretch
		SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_STRETCH);
		break;

	case 4:
		// tile
		SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle_TILE);
		break;

	case 5:
		// clear
		CDesktopWallPaper::ClearDesktopWallPaper();
		break;

	case 6:
		_SaveAS();
		break;

	default:
		{
			/// ó������ ���� �޴��̴�.
			_ASSERTE(false);
		}
	}

    return S_OK;
}

STDMETHODIMP CZViewMenuExt::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // res is a dummy LRESULT variable.  It's not used (IContextMenu2::HandleMenuMsg()
    // doesn't provide a way to return values), but it's here so that the code 
    // in MenuMessageHandler() can be the same regardless of which interface it
    // gets called through (IContextMenu2 or 3).

	LRESULT res;

    return MenuMessageHandler ( uMsg, wParam, lParam, &res );
}

STDMETHODIMP CZViewMenuExt::HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // For messages that have no return value, pResult is NULL.  This is
    // _very_ mean on the part of MS since it forces us to check whether
    // pResult is valid before using it.  You'd think that a pointer to
    // a "return value" would always be valid, but alas no.
    // If it is NULL, I create a dummy LRESULT variable, so the code in
    // MenuMessageHandler() will always have a valid pResult pointer.

    if ( NULL == pResult )
	{
		LRESULT res;
		return MenuMessageHandler ( uMsg, wParam, lParam, &res );
	}
    else
	{
		return MenuMessageHandler ( uMsg, wParam, lParam, pResult );
	}
}

STDMETHODIMP CZViewMenuExt::MenuMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
    switch ( uMsg )
	{
        case WM_MEASUREITEM:
            return OnMeasureItem ( (MEASUREITEMSTRUCT*) lParam, pResult );
			break;

        case WM_DRAWITEM:
            return OnDrawItem ( (DRAWITEMSTRUCT*) lParam, pResult );
	}

    return S_OK;
}


STDMETHODIMP CZViewMenuExt::OnMeasureItem ( MEASUREITEMSTRUCT* pmis, LRESULT* pResult )
{
    // Check that we're getting called for our own menu item.
	if ( m_uOurItemID != pmis->itemID )
	{
		return S_OK;
	}

	LONG   lThumbWidth;
	LONG   lThumbHeight;

	m_lBmpWidth = m_originalImage.GetWidth();
	m_lBmpHeight = m_originalImage.GetHeight();

	// ������� ũ�⸦ ����Ѵ�.
	RECT original = { 0, 0, m_lBmpWidth, m_lBmpHeight };
	RECT max = { 0, 0, m_lMaxThumbnailSize, m_lMaxThumbnailSize };

	RECT result = GetResizedRectForBigToSmall(max, original);

	lThumbWidth = result.right;
	lThumbHeight = result.bottom;

    // Calculate the size of the menu item, which is the size of the thumbnail +
    // the border and padding (which provides some space at the edges of the item).
    m_lItemWidth = lThumbWidth + m_lTotalBorderSpace;
    m_lItemHeight = lThumbHeight + m_lTotalBorderSpace;


    // Store the size of the item in the MEASUREITEMSTRUCT.
    pmis->itemWidth = m_lItemWidth;
    pmis->itemHeight = m_lItemHeight;

    *pResult = TRUE;            // we handled the message

    return S_OK;
}

STDMETHODIMP CZViewMenuExt::OnDrawItem(DRAWITEMSTRUCT * pdis, LRESULT * pResult )
{
    // Check that we're getting called for our own menu item.
    if ( m_uOurItemID != pdis->itemID )
	{
		return S_OK;
	}

	CDC*  pdcMenu = CDC::FromHandle ( pdis->hDC );
	CRect rcItem ( pdis->rcItem );  // RECT of our menu item
	CRect rcDraw;                   // RECT in which we'll be drawing

    // rcDraw will first be set to the RECT that we set in WM_MEASUREITEM.
    // It will get deflated as we go along.
    rcDraw.left = (rcItem.right + rcItem.left - m_lItemWidth) / 2;
    rcDraw.top = (rcItem.top + rcItem.bottom - m_lItemHeight) / 2;
    rcDraw.right = rcDraw.left + m_lItemWidth;
    rcDraw.bottom = rcDraw.top + m_lItemHeight;

    // Shrink rcDraw to account for the padding space around
    // the thumbnail.
    rcDraw.DeflateRect ( m_lMenuItemSpacing, m_lMenuItemSpacing );

   // �޴��� ����� ĥ�Ѵ�.
    if ( pdis->itemState & ODS_SELECTED )
	{
		// ���콺�� over �Ǿ� ������(���õǾ�������)
        pdcMenu->FillSolidRect ( rcItem, GetSysColor ( COLOR_HIGHLIGHT ));
	}
	else
	{
        pdcMenu->FillSolidRect ( rcItem, GetSysColor ( COLOR_3DFACE ));
	}

	// Draw the sunken 3D border.
	for ( unsigned int i = 1; i <= m_l3DBorderWidth; i++ )
	{
		pdcMenu->Draw3dRect ( rcDraw, GetSysColor ( COLOR_3DDKSHADOW ),
			GetSysColor ( COLOR_3DHILIGHT ));

		rcDraw.DeflateRect ( 1, 1 );
	}

	// Rescale �� �ʿ��ϸ� ���ش�.
	if ( rcDraw.Width() < m_originalImage.GetWidth()
		|| rcDraw.Height() < m_originalImage.GetHeight() )
	{
		/*
		if ( m_b8bit )	// 8bit �÷��� 32�� �ٲ� �Ŀ� rescale ����� �Ѵ�... ����.. �� �̷���;;
		{
			if ( !m_originalImage.ConvertTo32Bit() )
			{
#ifdef _DEBUG
				MsgBox("Can't convert to 32bit");
#endif
			}
		}
		*/

		m_originalImage.Resize(rcDraw.Width(), rcDraw.Height());
	}

	// ������ �׸���.
	int r = StretchDIBits(*pdcMenu,
		rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(),
		0, 0, rcDraw.Width(), rcDraw.Height(),
		m_originalImage.GetData(),		//FreeImage_GetBits(m_toDraw),
		m_originalImage.GetBitmapInfo(),		//FreeImage_GetInfo(m_toDraw),
		0, 0x00cc0020);

    *pResult = TRUE;            // we handled the message

    return S_OK;
}

/// ������ dll �� �ִ� ������ ZViewer �� �����Ѵ�.
void CZViewMenuExt::ExecZViewer()
{
	TCHAR command[FILENAME_MAX] = { 0 };

	tstring strProgramFolder = GetProgramFolder();

	StringCchPrintf(command, sizeof(command), TEXT("%s\\ZViewer.exe %s"), strProgramFolder.c_str(), m_szFile);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags =STARTF_USESHOWWINDOW;
	si.wShowWindow =SW_SHOW;
	ZeroMemory( &pi, sizeof(pi) );

	// ZViewer �� �����Ų��.
	CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

void CZViewMenuExt::SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style)
{
	// ���纸�� �ִ� ������ ������ ������ �����Ѵ�.
	TCHAR szSystemFolder[_MAX_PATH] = { 0 };

	if ( E_FAIL == SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, szSystemFolder) )
	{
		_ASSERTE(false);
		return;
	}

	tstring strSaveFileName = szSystemFolder;
	strSaveFileName += TEXT("\\zviewer_bg.bmp");

	ZImage tempImage;
	if ( !tempImage.LoadFromFile(m_szFile) )
	{
		MessageBox(HWND_DESKTOP, GetMessage(TEXT("CANNOT_LOAD_IMAGE_FILE")), TEXT("ZViewer"), MB_OK);
		return;
	}

	if ( false == tempImage.SaveToFile(strSaveFileName, BMP_DEFAULT) )
	{
		_ASSERTE(false);
		return;
	}


	CDesktopWallPaper::SetDesktopWallPaper(strSaveFileName, style);
}