/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "ZOption.h"


ZOption & ZOption::GetInstance()
{
	static ZOption inst;
	return inst;
}


ZOption::ZOption()
{
	/// �Ʒ� ���� ZViewerAgent ������ true �̴�
	m_bDontSaveInstance = false;
}

void ZOption::LoadOption()
{
	TCHAR buffer[256];

	/// C:\Documents and Settings\USERID\Local Settings\Application Data �� ��ġ�� ���´�.
	if ( S_OK != SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer) )
	{
		m_strOptionFilename = GetProgramFolder();
	}
	else
	{
		m_strOptionFilename = buffer;
	}

	m_strOptionFilename += TEXT("\\zviewer.ini");
	m_bOptionChanged = false;

	/// �⺻���� �ɼ��� �����صд�.
	SetDefaultOption();

	SetSaveOptions();

	/// �⺻���� �ɼǿ��� ���Ͽ��� �ҷ��� ������ ������.
	LoadFromFile();
}

/// �� ������� �� �̸����� ����ǰ�, �ҷ��� �´ٶ�� ����
void ZOption::SetSaveOptions()
{
	_InsertSaveOptionSetting(L"maximumcachememoryMB", &m_iMaximumCacheMemoryMB);
	_InsertSaveOptionSetting(L"maximumcachefilenum", &m_iMaxCacheImageNum);

	_InsertSaveOptionSetting(L"loop_view", &m_bLoopImages);

	_InsertSaveOptionSetting(L"stretch_small_to_big", &m_bSmallToBigStretchImage);
	_InsertSaveOptionSetting(L"stretch_big_to_small", &m_bBigToSmallStretchImage);

	_InsertSaveOptionSetting(L"use_open_cmd_shell", &m_bUseOpenCMDInShell);
	_InsertSaveOptionSetting(L"use_preview_shell", &m_bUsePreviewInShell);

	_InsertSaveOptionSetting(L"use_debug", &m_bUseDebug);

	_InsertSaveOptionSetting(L"use_auto_rotation", &m_bUseAutoRotation);
}

/// �⺻���� �ɼ��� �����صд�. ��ġ �� ó�� ����Ǿ��� �� �� ���� �������� zviewer.ini ������ ���������.
void ZOption::SetDefaultOption()
{
	m_bUsePreviewInShell = true;
	m_bUseOpenCMDInShell = false;
	m_bAlwaysOnTop = false;
	m_bSlideMode = false;
	m_iSlideModePeriodMiliSeconds = 5000;	///< Default slide mode period is 5 seconds
	m_bLoopImages = false;
	m_bUseAutoRotation = true;
	m_bFullScreen = false;
	m_bUseDebug = true;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
	m_iMaximumCacheMemoryMB = 50;

	m_iMaxCacheImageNum = 50;

#ifdef _DEBUG
	m_iMaxCacheImageNum = 10;
	m_iSlideModePeriodMiliSeconds = 1000;
	m_bUseOpenCMDInShell = true;
	m_bUseDebug = true;
#endif
}

void ZOption::LoadFromFile()
{
	iniMap data;

	/// ���Ϸκ��� ���� �ҷ����Ⱑ �������� ���� ������ �Ѵ�.
	if ( COptionFile::LoadFromFile(m_strOptionFilename, data) )
	{
		for ( size_t i=0; i<m_saveOptions.size(); ++i )
		{
			if ( data.count(m_saveOptions[i].getString()) > 0)
			{
				m_saveOptions[i].InsertMapToValue(data);
			}
		}
	}
}

void ZOption::SaveToFile()
{
	/// ZViewerAgent ���� ��ĥ ���� ������ ���� �ʱ� ����
	if ( m_bDontSaveInstance ) return;

	iniMap data;

	m_bOptionChanged = true;

	// �����ؾ��ϴ� �ɼ� �� ����� ���� ������
	if ( m_bOptionChanged )
	{
		for ( size_t i=0; i<m_saveOptions.size(); ++i )
		{
			m_saveOptions[i].InsertValueToMap(data);
		}
		COptionFile::SaveToFile(m_strOptionFilename, data);
	}
}