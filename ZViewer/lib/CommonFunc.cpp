#include "stdafx.h"
#include "CommonFunc.h"

void DebugPrintf( const char *fmt, ... )
{
#ifdef _DEBUG
	va_list v;
	char buf[1024*4];
	int len;

	va_start( v, fmt );
	len = wvsprintf( buf, fmt, v );
	va_end( v );

	OutputDebugString( buf );
	OutputDebugString( "\r\n" );
#endif
}

bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData)
{
	if( !hOpenKey || strKey.empty() || !szValue)
	{
		_ASSERTE(!"SetRegistryValue invalid arg");
		return false;
	}

	bool bRetVal = false;
	DWORD dwDisposition;
	HKEY hTempKey = NULL;

	if( ERROR_SUCCESS == ::RegCreateKeyEx(hOpenKey, strKey.c_str(), NULL,
		NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hTempKey, &dwDisposition) )
	{
		// ������ \0 ���� �����ؾ��Ѵٴ���;;
		DWORD	dwBufferLength = (DWORD)strData.size() + 1;

		if( ERROR_SUCCESS == ::RegSetValueEx(hTempKey, (LPTSTR)szValue,
			NULL, REG_SZ, (const BYTE *)strData.c_str(), dwBufferLength) )
		{
			bRetVal = true;
		}
	}

	if( hTempKey )
	{
		::RegCloseKey(hTempKey);
	}

	return bRetVal;
}

// �ִ� ũ�⸦ ���� �ʴ� ������ �������� ũ�⸦ �����ش�.
RECT GetResizedRect(const RECT & MaximumSize, const RECT & originalSize)
{
	if ( originalSize.right <= MaximumSize.right && originalSize.bottom <= MaximumSize.bottom )
	{
		RECT ret = originalSize;
		return ret;
	}

	// ���� ���� ũ�� �� ū ���� ã�´�.
	bool bSetWidth = true;		// ���� ũ�⸦ �������� ���� ���ΰ�?

	double dWidthRate = (double)MaximumSize.right / (double)originalSize.right;
	double dHeightRate = (double)MaximumSize.bottom / (double)originalSize.bottom;

	if ( dHeightRate >=  dWidthRate)
	{
		bSetWidth = true;
	}
	else
	{
		bSetWidth = false;
	}

	// ū ���� MaximumSize �� �ǰ� �ϴ� ��ʸ� ã�´�.
	RECT ret;

	double dRate = 1;
	if ( bSetWidth == true )
	{
		// ���� ũ�Ⱑ �����̴�.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dWidthRate), (int)(originalSize.bottom*dWidthRate));
	}
	else
	{
		// ���� ũ�Ⱑ �����̴�.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dHeightRate), (int)(originalSize.bottom*dHeightRate));
	}


	_ASSERTE(ret.right <= MaximumSize.right);
	_ASSERTE(ret.bottom <= MaximumSize.bottom);

	return ret;
}

std::string toString(int i)
{
	char szTemp[20];
	_ASSERTE( i < 10000000000000000000);

	sprintf(szTemp, "%d", i);

	return std::string(szTemp);
}