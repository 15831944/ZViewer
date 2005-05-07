/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtDlg.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

enum eFileExt
{
	eFileExt_BMP = 1,
	eFileExt_JPG = 2,
	eFileExt_GIF = 5,
	eFileExt_PNG = 3,
	eFileExt_PSD = 4,
	eFileExt_MAX
};

enum
{
	eFileExt_ALL = 0
};

class ZFileExtDlg
{
	ZFileExtDlg();

public:
	static ZFileExtDlg & GetInstance();

	static int CALLBACK FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

	void ShowDlg();


	/// ���� �ý��ۿ� ������ ���� Ȯ���� ���� ���¸� �����Ѵ�. true �� ����� ���̴�.
	bool m_bFileExt[eFileExt_MAX];

	/// ���� �ý��ۿ� ������ ���� Ȯ���� ���� ���¸� ���´�.
	void LoadExtEnv(HWND hwnd);

	/// ���� ��������� �ý��ۿ� ����.
	void SaveExtEnv();

private:
	/// ����� ���� �ֳ�?
	bool m_bChanged;

	/// ������ ���� ������ �����ϴ� �Լ�
	bool SetExtWithProgram(const std::string & strProgramName, const std::string & strExt, std::string strFullProgramPath = "", const std::string & strIcon = "", int iIconIndex = 0);

	/// Ȯ���ڸ� ����
	void ExtMapInit();
	typedef std::map< eFileExt, std::string> extMapType;
	extMapType	m_extMap;
};