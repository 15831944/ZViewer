/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	stdafx.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

/// _WIN32_WINNT means NT 4.0. If you want for above Win2000, use 0x0500. Check http://a.tk.co.kr/75
#define _WIN32_WINNT 0x0501

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

#include "freeimage/FreeImagePlus.h"

#include "CommonDefine.h"
