// ZViewerAgent.cpp : DLL ���������� �����Դϴ�.

#include "stdafx.h"
#include "resource.h"
#include "ZViewerAgent.h"

class CZViewerAgentModule : public CAtlDllModuleT< CZViewerAgentModule >
{
public :
	DECLARE_LIBID(LIBID_ZViewerAgentLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ZVIEWERAGENT, "{3081C080-FDE6-4B38-807E-C4EA3CE53C12}")
};

CZViewerAgentModule _AtlModule;

class CZViewerAgentApp : public CWinApp
{
public:

// ������
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CZViewerAgentApp, CWinApp)
END_MESSAGE_MAP()

CZViewerAgentApp theApp;

BOOL CZViewerAgentApp::InitInstance()
{
    return CWinApp::InitInstance();
}

int CZViewerAgentApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}


// DLL�� OLE�� ���� ��ε�� �� �ִ��� �����ϴ� �� ���˴ϴ�.
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}


// Ŭ���� ���͸��� ��ȯ�Ͽ� ��û�� ������ ��ü�� ����ϴ�.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - �ý��� ������Ʈ���� �׸��� �߰��մϴ�.
STDAPI DllRegisterServer(void)
{
    // ��ü, ���� ���̺귯�� �� ���� ���̺귯���� ��� �ִ� ��� �������̽��� ����մϴ�.
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - �ý��� ������Ʈ������ �׸��� �����մϴ�.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}
