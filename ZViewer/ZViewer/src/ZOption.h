
#pragma once

#include "ZINIOption.h"

class ZOption
{
public:

	ZOption()
	{
		SetDefaultOption();
	}

	/// ��ü ȭ�� ����ΰ�.
	bool m_bFullScreen;

	/// ȭ�麸�� ū �׸��� ȭ�鿡 �°� ����� ���ΰ�
	bool m_bBigToSmallStretchImage;

	/// ȭ�麸�� ���� �׸��� ȭ�鿡 �°� Ȯ���� ���ΰ�.
	bool m_bSmallToBigStretchImage;

	//// �׸��� ���� ��ġ. �Ϻ� ��ȭ�� ��� ���� ������ �����ϱ� ���ؼ�...
	bool m_bRightTopFirstDraw;

	void SetDefaultOption();
};