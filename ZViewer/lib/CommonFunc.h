#pragma once

void DebugPrintf( const char *fmt, ... );

bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData);

// �ִ� ũ�⸦ ���� �ʴ� ������ �������� ũ�⸦ �����ش�.
RECT GetResizedRect(const RECT & MaximumSize, const RECT & originalSize);

std::string toString(int i);