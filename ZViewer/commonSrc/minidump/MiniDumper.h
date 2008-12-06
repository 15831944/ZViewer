
#include <windows.h>
#include "dbghelp.h"

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

class MiniDumper
{
public:
	MiniDumper(const TCHAR * szDumpFilename = TEXT("Minidump.dmp"), const TCHAR * szDumpMsg = TEXT("Oops! Crash!"));
	~MiniDumper();

//private:///< ���� ��Ȳ���� �ִ��� �Լ� ȣ���� ���̱� ���� private �� ���� ����
	TCHAR m_szDumpFilename[FILENAME_MAX];
	TCHAR m_szDumpMsg[FILENAME_MAX];
	static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
};
