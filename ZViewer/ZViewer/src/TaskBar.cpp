/********************************************************************
*
* Created by zelon(Kim, Jinwook - Korea)
* 
*   2008. 6. 2 created
*	FileName : TaskBar.cpp
*
*                                       http://zviewer.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "TaskBar.h"

/// �۾� ǥ������ ���̰� ���ش�.
void TaskBar::ShellTrayShow()
{
	/// �۾� ǥ������ ���̰� ���ش�.
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_SHOW);
	}
}

void TaskBar::ShellTrayHide()
{
	// �۾� ǥ������ ���̰� ���ش�.
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_HIDE);
	}
}

