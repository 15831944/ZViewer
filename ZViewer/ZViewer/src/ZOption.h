/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include "CommonFunc.h"
#include "OptionFile.h"
#include "StringToValue.h"

class ZOption
{
	ZOption();

	void _InsertSaveOptionSetting(TCHAR * str, bool * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

	void _InsertSaveOptionSetting(TCHAR * str, int * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

public:
	static ZOption & GetInstance();

	~ZOption()
	{
		SaveToFile();
	}

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}
	void SetFullScreen(bool bFullScreen) { m_bFullScreen = bFullScreen; }

	int GetMaxCacheMemoryMB() const { return m_iMaximumCacheMemoryMB; }
	void SetMaxCacheMemoryMB(const int iMB) { m_iMaximumCacheMemoryMB = iMB; }

	const bool IsLoopImages() const { return m_bLoopImages; }
	void ToggleLoopImages() { m_bLoopImages = !m_bLoopImages; }

	const bool IsBigToSmallStretchImage() const { return m_bBigToSmallStretchImage; }
	void ToggleBigToSmallStretchImage() { m_bBigToSmallStretchImage = !m_bBigToSmallStretchImage; }

	const bool IsSmallToBigStretchImage() const { return m_bSmallToBigStretchImage; }
	void ToggleSmallToBigStretchImage() { m_bSmallToBigStretchImage = !m_bSmallToBigStretchImage; }

	const bool IsUseCache() const { return m_bUseCache; }
	//-----------------------------------------------------------
	// �Ʒ��� ���α׷� ���� �Ŀ� �ӽ÷� ���̴� �ɼǵ�. ���Ͽ� �������� �ʴ´�.

	//// �׸��� ���� ��ġ. �Ϻ� ��ȭ�� ��� ���� ��ܺ��� �����ϱ� ���ؼ�...
	bool m_bRightTopFirstDraw;

	/// �� ȭ�鿡 2���� �׸��� ������ ���ΰ�.
	bool m_bTwoInSaveView;

	/// �ִ� ĳ���� �̹��� ����. ���⼭ ���ݹ�ŭ ����, ���������� ����.
	int m_iMaxCacheImageNum;

	/// �����̵� ����ΰ�
	bool m_bSlideMode;

	/// �����̵� ����� �ð�(mili seconds)
	int m_iSlideModePeriodMiliSeconds;

	DWORD m_dwLastSlidedTime;

protected:
	/// ��ü ȭ�� ����ΰ�.m_bOptionChanged(false)
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// �Ʒ��� ���������� ����Ǵ� �ɼǵ�. ���Ͽ� �����Ѵ�.

	/// ĳ�� �ý����� �����ΰ�... �޸𸮰� ���� ��ǻ�Ϳ����� �� �� �ֵ��� �ϱ� ���ؼ�
	bool m_bUseCache;

	/// ������ �׸� ���Ͽ��� �������� ���� ó�� �׸� ���Ϸ� �� ���ΰ�?
	bool m_bLoopImages;

	/// ȭ�麸�� ū �׸��� ȭ�鿡 �°� ����� ���ΰ�
	bool m_bBigToSmallStretchImage;

	/// ȭ�麸�� ���� �׸��� ȭ�鿡 �°� Ȯ���� ���ΰ�.
	bool m_bSmallToBigStretchImage;

	/// ĳ�õǴ� �ִ� �޸𸮷�
	int m_iMaximumCacheMemoryMB;

	tstring m_strOptionFilename;

	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

	void SetSaveOptions();
protected:
	/// ���Ϸκ��� �ɼ��� �о�鿩�� �ٲ� �ɼ��� �ֳ�.
	bool m_bOptionChanged;

	std::vector < StringToValue > m_saveOptions;

};

