
#pragma once


class ZOption
{
public:

	ZOption()
		:	m_bOptionChanged(false)
		,	m_strOptionFilename("option.ini")
	{
		SetDefaultOption();

		LoadFromFile();
	}

	~ZOption()
	{
		SaveToFile();
	}

	//-----------------------------------------------------------
	// �Ʒ��� ���α׷� ���� �Ŀ� �ӽ÷� ���̴� �ɼǵ�. ���Ͽ� �������� �ʴ´�.
	/// ��ü ȭ�� ����ΰ�.
	bool m_bFullScreen;

	/// ȭ�麸�� ū �׸��� ȭ�鿡 �°� ����� ���ΰ�
	bool m_bBigToSmallStretchImage;

	/// ȭ�麸�� ���� �׸��� ȭ�鿡 �°� Ȯ���� ���ΰ�.
	bool m_bSmallToBigStretchImage;

	//// �׸��� ���� ��ġ. �Ϻ� ��ȭ�� ��� ���� ��ܺ��� �����ϱ� ���ؼ�...
	bool m_bRightTopFirstDraw;

private:
	//----------------------------------------------------------
	// �Ʒ��� ���������� ����Ǵ� �ɼǵ�. ���Ͽ� �����Ѵ�.

	/// �ִ� ĳ�õǴ� �޸�
	int m_iMaximumCacheMemory;

	/// �ִ� ĳ�õǴ� �׸� ����
	int m_iMaximumCacheFileNum;

	const std::string m_strOptionFilename;

	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

protected:
	/// ���Ϸκ��� �ɼ��� �о�鿩�� �ٲ� �ɼ��� �ֳ�.
	bool m_bOptionChanged;

};