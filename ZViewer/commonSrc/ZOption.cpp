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
#include "ZMain.h"


ZOption & ZOption::GetInstance()
{
	static ZOption inst;
	return inst;
}


ZOption::ZOption()
{
	TCHAR buffer[256];
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

	/// �ҷ��� ���� �ɼ��� �����Ͽ� �޴� �� üũǥ���� �͵��� ǥ���Ѵ�.
	ZMain::GetInstance().SetCheckMenus();
}

void ZOption::SetSaveOptions()
{
	_InsertSaveOptionSetting(L"maximumcachememoryMB", &m_iMaximumCacheMemoryMB);
	_InsertSaveOptionSetting(L"maximumcachefilenum", &m_iMaxCacheImageNum);

	_InsertSaveOptionSetting(L"use_cache", &m_bUseCache);
	_InsertSaveOptionSetting(L"loop_view", &m_bLoopImages);

	_InsertSaveOptionSetting(L"stretch_small_to_big", &m_bSmallToBigStretchImage);
	_InsertSaveOptionSetting(L"stretch_big_to_small", &m_bBigToSmallStretchImage);
}

/// �⺻���� �ɼ��� �����صд�.
void ZOption::SetDefaultOption()
{
	m_bAlwaysOnTop = false;
	m_bSlideMode = false;
	m_iSlideModePeriodMiliSeconds = 5000;	///< Default slide mode period is 5 seconds
	m_bLoopImages = false;
	m_bUseCache = true;
	m_bFullScreen = false;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
	m_iMaximumCacheMemoryMB = 50;

	m_iMaxCacheImageNum = 50;

#ifdef _DEBUG
	m_iMaxCacheImageNum = 10;
	m_iSlideModePeriodMiliSeconds = 1000;
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