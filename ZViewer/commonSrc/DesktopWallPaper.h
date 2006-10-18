/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	DesktopWallPaper.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once


/// �������� ����ȭ�������� �������� ���� �����ִ� Ŭ����
class CDesktopWallPaper
{
public:
	enum eDesktopWallPaperStyle
	{
		eDesktopWallPaperStyle_CENTER,
		eDesktopWallPaperStyle_TILE,
		eDesktopWallPaperStyle_STRETCH
	};

public:
	/// ������ ����ȭ���� �ٲ۴�.
	static void SetDesktopWallPaper(const tstring & strBitmapFileName, eDesktopWallPaperStyle style);

	/// ����ȭ���� ���ȭ���� �����.
	static void ClearDesktopWallPaper();

protected:
	/// ����ȭ������ ������ �����̸�. bmp ���� �Ѵ�.
	tstring m_strFileName;
	eDesktopWallPaperStyle m_eStyle;
};