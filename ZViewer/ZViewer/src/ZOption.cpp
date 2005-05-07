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
#include "OptionFile.h"
#include "CommonFunc.h"

void ZOption::SetDefaultOption()
{
	m_bFullScreen = false;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
}

void ZOption::LoadFromFile()
{
	iniMap data;

	COptionFile::LoadFromFile(m_strOptionFilename, data);
}

void ZOption::SaveToFile()
{
	iniMap data;

	// �����ؾ��ϴ� �ɼ� �� ����� ���� ������
	if ( m_bOptionChanged )
	{
		data["maximumcachememory"] = toString(m_iMaximumCacheMemory);
		data["maximumcachefilenum"] = toString(m_iMaximumCacheFileNum);
	}

	COptionFile::SaveToFile(m_strOptionFilename, data);
}

