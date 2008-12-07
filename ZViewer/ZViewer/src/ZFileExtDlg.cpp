/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtDlg.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "ZFileExtDlg.h"
#include "ZMain.h"
#include "ZResourceManager.h"
#include <ShlObj.h>

#include "ZFileExtReg.h"

#include "../commonSrc/ExtInfoManager.h"

ZFileExtDlg * pThis = NULL;

ZFileExtDlg::ZFileExtDlg()
{
	pThis = this;
}

ZFileExtDlg & ZFileExtDlg::GetInstance()
{
	static ZFileExtDlg a;
	return a;
}

void ZFileExtDlg::ShowDlg()
{
	INT_PTR iRet = DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_FILE_EXT_DIALOG), ZMain::GetInstance().GetHWND(), FileExtDlgProc);

	if ( iRet == IDOK )
	{
		SaveExtEnv();
	}
}

int CALLBACK ZFileExtDlg::FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM /*lParam*/)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			//pThis->LoadExtEnv(hWnd);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				EndDialog(hWnd, IDOK);
				break;

			case IDCANCEL:
				EndDialog(hWnd, IDCANCEL);
				break;
			}
		}
		break;

	case WM_DESTROY:
		EndDialog(hWnd, 0);
		return 0;
	}
	return FALSE;
}

/*
void ZFileExtDlg::LoadExtEnv(HWND hwnd)
{
	/// Registry ���� �����Ǿ� �ִ� ������ �޾ƿ´�.
	/// TODO: registry ���� �о�´�.

	/// �о�� ��� checkbox �� �����Ѵ�.
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_BMP)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_GIF)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_PNG)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_PSD)), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(GetDlgItem(hwnd, (IDC_CHECK_JPG)), BM_SETCHECK, BST_CHECKED, 0);
	//SendMessage(GetDlgItem(hwnd, MAKEINTRESOURCE(IDC_CHECK_BMP)), BM_SETCHECK, BST_CHECKED, 0);
}
*/

void ZFileExtDlg::SaveExtEnv()
{
	std::vector < ExtSetting >::iterator it, endit = ExtInfoManager::GetInstance().m_extConnect.end();

	tstring strProgramFolder(GetProgramFolder());

	tstring strIconDll = strProgramFolder;
	strIconDll += TEXT("ZViewerIcons.dll");

	for ( it = ExtInfoManager::GetInstance().m_extConnect.begin(); it != endit; ++it)
	{
		const ExtSetting & extset = *it;

		if ( false == SetExtWithProgram(TEXT("ZViewer"), extset.m_strExt, 
			TEXT(""),	/// ���α׷� Full Path. ����θ� ���� ���α׷��̴�.
			strIconDll.c_str(),	/// ������ ���α׷�
			extset.m_numIconIndex	/// ������ index
		) )
		{
			assert(false);
		}
	}

	/// explorer �� �������� update ��Ų��.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

bool ZFileExtDlg::SetExtWithProgram(const tstring & strProgramName, const tstring & strExt, tstring strFullProgramPath, const tstring & strIcon, int iIconIndex)
{
	ZFileExtReg fileExtReg;


	if ( strFullProgramPath.empty() )
	{
		// get full file path to program executable file
		TCHAR szProgPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath) / sizeof(TCHAR));

		strFullProgramPath = szProgPath;
	}

	tstring strTempText;

	fileExtReg.m_strExtension = strExt.c_str();

	// ���α׷����� ���ڸ� �Ѱ��� ���� full path �� �����.
	strTempText  = strFullProgramPath;
	strTempText += TEXT(" \"%1\"");
	fileExtReg.m_strShellOpenCommand = strTempText.c_str();
	fileExtReg.m_strDocumentShellOpenCommand = strTempText.c_str();

	// ������Ʈ���� ����� ���� ���α׷��� �̸��� Ȯ���ڸ� ���Ѵ�.
	tstring strClassName = strProgramName;
	strClassName += TEXT(".");
	strClassName += strExt;
	fileExtReg.m_strDocumentClassName = strClassName.c_str();

	// Ȯ���ڿ� �´� �⺻ �������� �����Ѵ�.
	if ( strIcon.size() <= 0 )
	{
		// ������ ���α׷��� �������� ������ ���� ���α׷��� ù��° �������� ����.
		strTempText  = strFullProgramPath;
		strTempText += TEXT(",0");
	}
	else
	{
		// ������ ���α׷��� ���������� icon ��ȣ�� ����.
		if ( iIconIndex < 0 )
		{
			assert(iIconIndex >= 0 );
			iIconIndex = 0;
		}

		strTempText = strIcon;
		strTempText += TEXT(",");

		TCHAR szTemp[COMMON_BUFFER_SIZE] = { 0 };
		SPrintf(szTemp, COMMON_BUFFER_SIZE, TEXT("%d"), iIconIndex);
		strTempText += szTemp;
	}
	fileExtReg.m_strDocumentDefaultIcon = strTempText.c_str();

	// ������ ������ ������Ʈ���� ����Ѵ�.
	fileExtReg.SetRegistries();

	return true;
}
