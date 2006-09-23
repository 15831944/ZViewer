/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	MoveToDlg.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

/// ���� �̵��� �� �� �����ִ� ���̾�α׸� ���� Ŭ����
class CMoveToDlg
{
public:
	CMoveToDlg();
	~CMoveToDlg();

	/// ���̾�α׸� ȭ�鿡 ����.
	bool DoModal();

	const tstring & GetMoveToFolder() const { return m_strMoveToFolder; }

	static int CALLBACK MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

	void SetFolder(const tstring & strFolder);

protected:

	HWND m_hWnd;
	void OnBrowserButton();

	tstring m_strMoveToFolder;
};