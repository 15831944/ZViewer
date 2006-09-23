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


class ZOption
{
	ZOption();

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

	//-----------------------------------------------------------
	// �Ʒ��� ���α׷� ���� �Ŀ� �ӽ÷� ���̴� �ɼǵ�. ���Ͽ� �������� �ʴ´�.

	/// ȭ�麸�� ū �׸��� ȭ�鿡 �°� ����� ���ΰ�
	bool m_bBigToSmallStretchImage;

	/// ȭ�麸�� ���� �׸��� ȭ�鿡 �°� Ȯ���� ���ΰ�.
	bool m_bSmallToBigStretchImage;

	//// �׸��� ���� ��ġ. �Ϻ� ��ȭ�� ��� ���� ��ܺ��� �����ϱ� ���ؼ�...
	bool m_bRightTopFirstDraw;

	/// �� ȭ�鿡 2���� �׸��� ������ ���ΰ�.
	bool m_bTwoInSaveView;

	/// ĳ�� �ý����� �����ΰ�... �޸𸮰� ���� ��ǻ�Ϳ����� �� �� �ֵ��� �ϱ� ���ؼ�
	bool m_bUseCache;

	/// �ִ� ĳ���� �̹��� ����. ���⼭ ���ݹ�ŭ ����, ���������� ����.
	int m_iMaxCacheImageNum;

	/// ������ �׸� ���Ͽ��� �������� ���� ó�� �׸� ���Ϸ� �� ���ΰ�?
	bool m_bLoopImages;

protected:
	/// ��ü ȭ�� ����ΰ�.m_bOptionChanged(false)
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// �Ʒ��� ���������� ����Ǵ� �ɼǵ�. ���Ͽ� �����Ѵ�.

	/// �ִ� ĳ�õǴ� �޸�
	int m_iMaximumCacheMemory;

	/// �ִ� ĳ�õǴ� �׸� ����
	int m_iMaximumCacheFileNum;

	tstring m_strOptionFilename;

	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

protected:
	/// ���Ϸκ��� �ɼ��� �о�鿩�� �ٲ� �ɼ��� �ֳ�.
	bool m_bOptionChanged;

};