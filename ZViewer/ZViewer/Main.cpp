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

	Created by zelon (Kim, Jinwook in Korea)

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
#include "src/ZMain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "src/ZCacheImage.h"
#include "src/MainWindow.h"

#include "../commonSrc/LogManager.h"
#include "../commonSrc/MessageManager.h"

#ifdef _DEBUG
#include "vld/vld.h"
#endif



int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance ,LPTSTR lpszCmdParam,int nCmdShow)
{
	// �⺻���� ������� ������Ʈ�� �ִ� �����̴�.
	ZResourceManager::GetInstance().SetHandleInstance(hInstance);

	HINSTANCE hLang = NULL;

	// debug mode ������ �׻� ������(����� �׽�Ʈ�� ���ؼ�)
#ifndef _DEBUG
	if ( GetSystemDefaultLangID() == 0x0412 )
	{
		CMessageManager::getInstance().SetLanguage(eLanguage_KOREAN);

		hLang = LoadLibrary(TEXT("language/korean.dll"));

		if ( hLang )
		{
			ZResourceManager::GetInstance().SetHandleInstance(hLang);
		}
		else
		{
			assert(hLang != NULL);
		}
	}
#endif

	/// ���� Ȯ���ڸ� �����϶�� �Ÿ�
	if ( _tcscmp(lpszCmdParam, TEXT("/fileext")) == 0 )	
	{
		int iRet = MessageBox(HWND_DESKTOP, GetMessage(TEXT("REG_FILE_TYPE")), TEXT("ZViewer"), MB_YESNO);

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
	if ( strInitArg.empty() )
	{
		strInitArg = TEXT("..\\SampleImages\\zviewer_jpg.jpg");
	}
	//strInitArg = "C:\\A.bmp";
#endif
	ZImage::StartupLibrary();

	ZMain::GetInstance().SetInitArg(strInitArg);
	ZMain::GetInstance().SetInstance(hInstance);

	CMainWindow aWindow;

	HWND hWnd = aWindow.Create(hInstance, HWND_DESKTOP, nCmdShow);

	/// Add Timer
	enum
	{
		eTimerValue = 9153
	};
	UINT_PTR timerPtr = SetTimer(hWnd, eTimerValue, 100, NULL);

	if ( timerPtr == 0 )
	{
		MessageBox(hWnd, GetMessage(TEXT("")), TEXT("ZViewer"), MB_OK);
		return 0;
	}

	WPARAM wParam = aWindow.MsgProc();

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
			assert(!"Can't free language dll.");
		}
	}

	ZCacheImage::GetInstance().CleanUp();
	CLogManager::getInstance().CleanUp();
	ZImage::CleanupLibrary();

	return (int)wParam;
}
