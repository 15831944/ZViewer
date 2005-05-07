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

struct ExtSetting
{
	unsigned int m_numIconIndex;		// icon dll ������ index
	std::string m_strExt;				// ������ Ȯ����
};

class ZFileExtDlg
{
	ZFileExtDlg();

public:
	static ZFileExtDlg & GetInstance();

	static int CALLBACK FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

	void ShowDlg();


	/// ���� �ý��ۿ� ������ ���� Ȯ���� ���� ���¸� �����Ѵ�. true �� ����� ���̴�.
//	bool m_bFileExt[eFileExt_MAX];

	/// ���� �ý��ۿ� ������ ���� Ȯ���� ���� ���¸� ���´�.
//	void LoadExtEnv(HWND hwnd);

	/// ���� ��������� �ý��ۿ� ����.
	void SaveExtEnv();

private:
	/// ����� ���� �ֳ�?
//	bool m_bChanged;

	/// ������ ���� ������ �����ϴ� �Լ�
	bool SetExtWithProgram(const std::string & strProgramName, const std::string & strExt, std::string strFullProgramPath = "", const std::string & strIcon = "", int iIconIndex = 0);

	/// Ȯ���ڸ� ����
	void ExtMapInit();
	std::vector < ExtSetting > m_extConnect;
//	typedef std::map< eFileExt, std::string> extMapType;
//	extMapType	m_extMap;
};