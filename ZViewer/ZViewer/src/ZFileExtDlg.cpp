
#include "stdafx.h"
#include "resource.h"
#include "ZFileExtDlg.h"
#include "../ZMain.h"
#include "ZResourceManager.h"
#include <ShlObj.h>

#include "../lib/ZFileExtReg.h"

ZFileExtDlg * pThis = NULL;

ZFileExtDlg::ZFileExtDlg()
:	m_bChanged(true)
{
	pThis = this;

	ExtMapInit();
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

void ZFileExtDlg::ExtMapInit()
{
	m_extMap[eFileExt_BMP] = "bmp";
	m_extMap[eFileExt_GIF] = "gif";
	m_extMap[eFileExt_JPG] = "jpg";
	m_extMap[eFileExt_PNG] = "png";
	m_extMap[eFileExt_PSD] = "psd";
}
int CALLBACK ZFileExtDlg::FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			pThis->LoadExtEnv(hWnd);
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

void ZFileExtDlg::SaveExtEnv()
{
	if ( m_bChanged )
	{
		extMapType::iterator it, endit = m_extMap.end();

		std::string strProgramFolder;

		{
			char szGetFileName[FILENAME_MAX] = { 0 };
			DWORD ret = GetModuleFileName(GetModuleHandle(NULL), szGetFileName, FILENAME_MAX);

			if ( ret == 0 )
			{
				_ASSERTE(!"Can't get module folder");
				return;
			}
			char szDrive[_MAX_DRIVE] = { 0 };
			char szDir[_MAX_DIR] = { 0 };
			_splitpath(szGetFileName, szDrive, szDir, 0, 0);

			strProgramFolder = szDrive;
			strProgramFolder += szDir;
		}

		std::string strIconDll = strProgramFolder;
		strIconDll += "ZViewerIcons.dll";

		std::string strExt;
		for ( it = m_extMap.begin(); it != endit; ++it)
		{
			strExt = it->second;

			SetExtWithProgram("ZViewer", strExt, 
				"",	/// ���α׷� Full Path. ����θ� ���� ���α׷��̴�.
				strIconDll.c_str(),	/// ������ ���α׷�
				it->first	/// ������ index
			);
		}

		/// explorer �� �������� update ��Ų��.
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	}
}

bool ZFileExtDlg::SetExtWithProgram(const std::string & strProgramName, const std::string & strExt, std::string strFullProgramPath, const std::string & strIcon, int iIconIndex)
{
	ZFileExtReg fileExtReg;


	if ( strFullProgramPath.size() == 0 )
	{
		// get full file path to program executable file
		char szProgPath[MAX_PATH];
		::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath));

		strFullProgramPath = szProgPath;
	}

	std::string strTempText;

	fileExtReg.m_strExtension = strExt.c_str();

	// ���α׷����� ���ڸ� �Ѱ��� ���� full path �� �����.
	strTempText  = strFullProgramPath;
	strTempText += " \"%1\"";
	fileExtReg.m_strShellOpenCommand = strTempText.c_str();
	fileExtReg.m_strDocumentShellOpenCommand = strTempText.c_str();

	// ������Ʈ���� ����� ���� ���α׷��� �̸��� Ȯ���ڸ� ���Ѵ�.
	std::string strClassName = strProgramName;
	strClassName += ".";
	strClassName += strExt;
	fileExtReg.m_strDocumentClassName = strClassName.c_str();

	// Ȯ���ڿ� �´� �⺻ �������� �����Ѵ�.
	if ( strIcon.size() <= 0 )
	{
		// ������ ���α׷��� �������� ������ ���� ���α׷��� ù��° �������� ����.
		strTempText  = strFullProgramPath;
		strTempText += ",0";
	}
	else
	{
		// ������ ���α׷��� ���������� icon ��ȣ�� ����.
		if ( iIconIndex < 0 )
		{
			_ASSERTE(iIconIndex >= 0 );
			iIconIndex = 0;
		}

		strTempText = strIcon;
		strTempText += ",";

		char szTemp[256];
		_snprintf(szTemp, sizeof(szTemp), "%d", iIconIndex);
		strTempText += szTemp;
	}
	fileExtReg.m_strDocumentDefaultIcon = strTempText.c_str();

	// ������ ������ ������Ʈ���� ����Ѵ�.
	fileExtReg.SetRegistries();

	return true;
}
