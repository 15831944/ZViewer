#pragma once

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
	CDesktopWallPaper(const std::string & strBitmapFilename)
	{
		m_strFileName = strBitmapFilename;
		m_eStyle = eDesktopWallPaperStyle_CENTER;
	}

	virtual ~CDesktopWallPaper(){}

	inline void SetWallPaperStyle(eDesktopWallPaperStyle style)
	{
		m_eStyle = style;
	}

	// ������ ����ȭ���� �ٲ۴�.
	void SetDesktopWallPaper();

protected:
	// ����ȭ������ ������ �����̸�. bmp ���� �Ѵ�.
	std::string m_strFileName;
	eDesktopWallPaperStyle m_eStyle;
};