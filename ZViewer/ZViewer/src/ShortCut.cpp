﻿
#include "stdafx.h"

#include "ShortCut.h"

#include "../commonSrc/CommonFunc.h"
#include "../commonSrc/ZOption.h"
#include "resource.h"
#include "src/ZMain.h"

using namespace std;

#define VK_PAGE_UP		VK_PRIOR
#define VK_PAGE_DOWN	VK_NEXT
#define VK_X			(0x58)

const bool ShortCut::ShortCutData::isAllKeyMatched(const WPARAM pressedKey) const
{
	if (pressedKey != m_key)
	{
		return false;
	}

	return this->isModifierKeyMatched();
}

const bool ShortCut::ShortCutData::isModifierKeyMatched() const
{
	bool bNeedControl = false;
	bool bNeedShift = false;

	bool modifierOK = true;

	for (const auto & modifierKey : m_modifierKeys)
	{
		if (modifierKey == VK_CONTROL) bNeedControl = true;
		if (modifierKey == VK_SHIFT) bNeedShift = true;
	}

	if (bNeedControl != IsPressedVirtualKey(VK_CONTROL))
	{
		modifierOK = false;
	}

	if (bNeedShift != IsPressedVirtualKey(VK_SHIFT))
	{
		modifierOK = false;
	}

	return modifierOK;
}

ShortCut& ShortCut::GetInstance(void)
{
	static ShortCut inst;
	return inst;
}

ShortCut::ShortCut()
{
	initializeShortCutData();
}

void ShortCut::insertShortCutData(unsigned short key, int sendid)
{
	ShortCutData data;
	data.m_key = key;
	data.m_sendID = sendid;

	m_shortcutData.push_back(data);
}

void ShortCut::insertShortCutData(unsigned short modifier, unsigned short key, int sendid)
{
	ShortCutData data;
	data.m_modifierKeys.push_back(modifier);
	data.m_key = key;
	data.m_sendID = sendid;

	m_shortcutData.push_back(data);
}

void ShortCut::insertShortCutData(unsigned short modifier1, unsigned short modifier2, unsigned short key, int sendid)
{
	ShortCutData data;
	data.m_modifierKeys.push_back(modifier1);
	data.m_modifierKeys.push_back(modifier2);
	data.m_key = key;
	data.m_sendID = sendid;

	m_shortcutData.push_back(data);
}

HACCEL ShortCut::MakeAccelTable()
{
	vector < ACCEL > accels;

#pragma message("check http://msdn.microsoft.com/en-us/library/windows/desktop/ms646337(v=vs.85).aspx#editable_acc and make below for-loop")
	for ( size_t i=0; i<m_shortcutData.size(); ++i )
	{
		ACCEL aAccel = { 0, };
		aAccel.fVirt = FALT | FVIRTKEY;
		aAccel.key = VK_X;
		aAccel.cmd = ID_MAINMENU_FILE_EXIT;

		accels.push_back(aAccel);
	} 

	/// returned handle must be destroyed
	return CreateAcceleratorTable(&accels[0], accels.size());
}

ShortCutResult ShortCut::DoShortCut(const WPARAM pressedKey)
{
	DebugPrintf(TEXT("DoShortCut"));

	for ( auto it = m_shortcutData.begin(); it != m_shortcutData.end(); ++it )
	{
		ShortCutData & data = (*it);
		
		{
			if (data.isAllKeyMatched(pressedKey) == true)
			{
				PostMessage(ZMain::GetInstance().GetHWND(), WM_COMMAND, data.m_sendID, NULL);
			}
		}
	}

	switch ( pressedKey )
	{
	case VK_ESCAPE:
		{
			if ( ZOption::GetInstance().IsFullScreen() )	// 현재 풀스크린이면 원래 화면으로 돌아간다.
			{
				ZMain::GetInstance().ToggleFullScreen();
			}
			else
			{
				ZMain::GetInstance().CloseProgram();
			}
		}
		break;

	default:
		return ShortCutResult_NOT_EXECUTED;
	}
	return ShortCutResult_EXECUTED;
}

void ShortCut::initializeShortCutData(void)
{
	m_shortcutData.reserve(50);

	insertShortCutData(VK_DOWN, ID_ACCELERATOR_DOWN);
    insertShortCutData(VK_LEFT, ID_ACCELERATOR_LEFT);
    insertShortCutData(VK_RIGHT, ID_ACCELERATOR_RIGHT);
    insertShortCutData(VK_UP, ID_ACCELERATOR_UP);

	insertShortCutData(VK_CONTROL, 'C', ID_COPY_TO_CLIPBOARD);

	insertShortCutData(VK_SHIFT, VK_DELETE, ID_DELETETHISFILE);
    insertShortCutData(VK_DELETE, ID_DELETETHISFILE_RECYCLEBIN);

	insertShortCutData(VK_CONTROL, VK_SHIFT, 'C', ID_FILE_COPYTO);
    insertShortCutData(VK_CONTROL, VK_SHIFT, 'M', ID_FILE_MOVETO);

    insertShortCutData(VK_F1, ID_HELP_ABOUT);
    insertShortCutData(VK_F5, ID_FILE_RESCAN_FOLDER);
    insertShortCutData(VK_F9, ID_SHOW_CACHED_FILENAME);

    insertShortCutData(VK_CONTROL, 'O', ID_MAINMENU_FILE_OPEN);
	insertShortCutData(VK_CONTROL, 'S', ID_FILE_SAVE_AS_NEW_EXT);

	insertShortCutData(VK_CONTROL, 'W', ID_MAINMENU_FILE_EXIT);

	insertShortCutData(VK_HOME, ID_MOVE_FIRSTIMAGE);
    insertShortCutData(VK_END, ID_MOVE_LASTIMAGE);

    insertShortCutData(VK_CONTROL, VK_LEFT, ID_MOVE_PREV_JUMP);
    insertShortCutData(VK_CONTROL, VK_RIGHT, ID_MOVE_NEXT_JUMP);

	insertShortCutData(VK_PAGE_UP, ID_MOVE_PREVIMAGE);
	insertShortCutData('E', ID_MOVE_PREVIMAGE);

    insertShortCutData(VK_PAGE_DOWN, ID_MOVE_NEXTIMAGE);
    insertShortCutData('R', ID_MOVE_NEXTIMAGE);
    insertShortCutData(VK_SPACE, ID_MOVE_NEXTIMAGE);

    insertShortCutData(VK_CONTROL, VK_PAGE_UP, ID_MOVE_PREVFOLDER);
    insertShortCutData(VK_CONTROL, 'E', ID_MOVE_PREVFOLDER);
    insertShortCutData(VK_CONTROL, VK_PAGE_DOWN, ID_MOVE_NEXTFOLDER);
    insertShortCutData(VK_CONTROL, 'R', ID_MOVE_NEXTFOLDER);

    insertShortCutData(VK_SHIFT, VK_BACK, ID_MOVE_REDOIMAGEPOSITION);
    insertShortCutData(VK_BACK, ID_MOVE_UNDOIMAGEPOSITION);

	insertShortCutData(VK_RETURN, ID_VIEW_FULLSCREEN);

    insertShortCutData(/*']'*/ VK_OEM_6, ID_VIEW_ROTATECLOCKWISE);
    insertShortCutData(/*'['*/ VK_OEM_4, ID_VIEW_ROTATECOUNTERCLOCKWISE);
}
