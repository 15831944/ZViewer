#pragma once

#include "src/ZHistory.h"
#include "src/ZOption.h"
#include "../lib/DesktopWallPaper.h"
#include "../lib/ZImage.h"

class ZMain
{
public:
	static ZMain & GetInstance();
	~ZMain(void);

	void setInitArg(const std::string & strArg) { m_strInitArg = strArg; }
	void SetProgramFolder();
	void RescanFolder();			// ���ϸ���� �ٽ� �о���δ�.
	void OnInit();
	void Draw(bool bEraseBg = true);

	// ���纸�� �ִ� �̹����� ������ ����ȭ���� ������� �����Ѵ�.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);
	
	// ����ȭ�� �̹����� ���ش�.
	void ClearDesktopWallPaper();

	bool GetNeighborFolders(std::vector < std::string > & vFolders);
	void NextFolder();
	void PrevFolder();

	// Ư�� ��ġ�� �̹��� ���Ϸ� �ǳʶڴ�.
	bool MoveIndex(int iIndex);

	// ���� ��ġ���� ������ �̵��Ѵ�.
	bool MoveRelateIndex(int iRelateIndex)
	{
		return MoveIndex(m_iCurretFileIndex + iRelateIndex);
	}

	bool NextImage() { return MoveRelateIndex(+1); }
	bool PrevImage() { return MoveRelateIndex(-1); }

	bool FirstImage();
	bool LastImage();

	void Rotate(bool bClockWise);


	HWND GetHWND() const { return m_hMainDlg; }
	void SetHWND(HWND hWnd);
	void SetMainMenu(HMENU hMenu)
	{
		m_hMainMenu = hMenu;
	}
	void SetPopupMenu(HMENU hMenu)
	{
		m_hPopupMenu = hMenu;
	}

	long GetCachedKByte();

	void SetStatusHandle(HWND hWnd) { m_hStatus = hWnd; }
	HWND GetStatusHandle() const { return m_hStatus; }

	void ToggleFullScreen();

	void ToggleBigToScreenStretch();

	int GetLogCacheHitRate();

	void DeleteThisFile();

	const std::string & GetProgramFolder() const	// ���α׷� ���� ������ �ִ� ������ �����´�.
	{
		return m_strProgramFolder;
	}

	/// ������ ��� ini �� �о�ͼ� ȭ���� �����Ѵ�.
	//void LoadLanguage();

	void SetInstance(HINSTANCE hInstance) { m_hMainInstance = hInstance; }
	HINSTANCE GetHInstance() const { return m_hMainInstance; }
	void OnChangeCurrentSize(int iWidth, int iHeight);
	void OnDrag(int x, int y);

	void OnFocusLose();
	void OnFocusGet();

	void OnRightTopFirstDraw();

	inline bool IsFullScreen() const { return m_option.m_bFullScreen; }

	/// Ư�� ������ ����.
	void OpenFile(const std::string & strFilename);

	/// Ư�� ������ ù��° ������ ����.
	void OpenFolder(const std::string strFolder);
	void OpenFileDialog();

	void Undo();
	void Redo();

	bool m_bHandCursor;

	void ShowFileExtDlg();

	void ShellTrayShow();

private:
	ZMain(void);

	void LoadCurrent();
	void SetStatusBarText();
	void SetTitle();

	void InitOpenFileDialog();

	void StartBannerDownload();
	static DWORD WINAPI BannerThreadFunc(LPVOID p);

	void ShellTrayHide();

	void FormShow();
	void FormHide();

	std::string m_strInitArg;		// ���α׷� ���� ����.
	std::string m_strProgramFolder;	// ���α׷� ���� ������ �ִ� ����

	std::string m_strCurrentFolder;	// ���� ����
	std::string m_strCurrentFilename;	// ���� �����ְ� �ִ� �����̸�
	std::vector<std::string> m_vFile;
	int m_iCurretFileIndex;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;

	fipImage m_bannerImage;

	/// ���� �����ְ� �ִ� �̹���
	ZImage m_currentImage;
	DWORD m_dwLoadingTime;				// ���� �̹����� �ε��ϴ� �� �ɸ� �ð�

	HWND m_hMainDlg;
	HWND m_hStatus;
	HINSTANCE m_hMainInstance;

	HMENU m_hMainMenu;
	HMENU m_hPopupMenu;

	ZOption m_option;

	int m_iShowingX;			// �׸� �� ��� ��� �����ϳ�.
	int m_iShowingY;			// �׸� �� ��� ��� �����ϳ�.

	void ZFindFile(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive = false);
	void ZFindFolders(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive = false);
	std::string GetFolderNameFromFullFileName(const std::string & strFullFilename);

	// For Open File Dialog
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[MAX_PATH];       // buffer for file name
	//bool m_bOpeningFileDialog;	// ��üȭ���� �� ���� ���̾�α׸� ���� �����Ÿ��� ���� ����

	// For Undo/Redo
	ZHistory m_history;

};
