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
	tstring m_strExt;				// ������ Ȯ����
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
	bool SetExtWithProgram(const tstring & strProgramName, const tstring & strExt, tstring strFullProgramPath = TEXT(""), const tstring & strIcon = TEXT(""), int iIconIndex = 0);

	/// Ȯ���ڸ� ����
	void ExtMapInit();

	void _AddExtSet(const int iIconIndex, const TCHAR * ext);

	std::vector < ExtSetting > m_extConnect;
};