// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

/// if Visual Studio .NET 2005, disable 6011 warning because of STL warning
#if _MSC_VER == 1400
#pragma warning(disable:6011)
#endif

#include <windows.h>
#include <cassert>
#include <CommCtrl.h>
#include <WindowsX.h>
#include <Shlwapi.h>

// STL Headers
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <tchar.h>

#ifdef _MSC_VER
#include <strsafe.h>
#endif

#include <cassert>

#include <shlobj.h>
#include <io.h>

#include "../lib/freeimage/FreeImagePlus.h"

#include "../commonSrc/CommonDefine.h"
