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

#include "../lib/ZFileExtReg.h"

ZFileExtDlg * pThis = NULL;

ZFileExtDlg::ZFileExtDlg()
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
	ExtSetting extSet;

	extSet.m_numIconIndex = 1;
	extSet.m_strExt = "bmp";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 2;
	extSet.m_strExt = "jpg";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 2;
	extSet.m_strExt = "jpeg";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 2;
	extSet.m_strExt = "jpe";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 3;
	extSet.m_strExt = "png";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 4;
	extSet.m_strExt = "psd";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 5;
	extSet.m_strExt = "gif";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "dds";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "tga";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "pcx";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "xpm";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "xbm";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "tif";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "tiff";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "cut";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "ico";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "hdr";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "jng";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "koa";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "mng";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "pcd";
	m_extConnect.push_back(extSet);

	extSet.m_numIconIndex = 0;
	extSet.m_strExt = "ras";
	m_extConnect.push_back(extSet);

}
int CALLBACK ZFileExtDlg::FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
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
	std::vector < ExtSetting >::iterator it, endit = m_extConnect.end();

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

	for ( it = m_extConnect.begin(); it != endit; ++it)
	{
		const ExtSetting & extset = *it;

		SetExtWithProgram("ZViewer", extset.m_strExt, 
			"",	/// ���α׷� Full Path. ����θ� ���� ���α׷��̴�.
			strIconDll.c_str(),	/// ������ ���α׷�
			extset.m_numIconIndex	/// ������ index
		);
	}

	/// explorer �� �������� update ��Ų��.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
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
