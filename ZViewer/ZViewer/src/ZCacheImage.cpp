/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	ZCacheImage.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "ZCacheImage.h"
#include "ZOption.h"
#include "ZMain.h"
#include "../../commonSrc/MessageManager.h"

using namespace std;


ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bCacheGoOn(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lCacheSize(0)
,	m_numImageVectorSize(0)
,	m_lastActionDirection(eLastActionDirection_FORWARD)
{
	m_bNowCaching = false;
	m_hThread = INVALID_HANDLE_VALUE;
}

ZCacheImage::~ZCacheImage()
{
	m_bCacheGoOn = false;

	m_hCacheEvent.setEvent();

	if ( m_hThread != INVALID_HANDLE_VALUE )
	{
		/// ĳ�� �����尡 �����ϱ� ��ٸ���.
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);

		m_hThread = INVALID_HANDLE_VALUE;
	}

	DebugPrintf(TEXT("Cached Thread ended."));
}

void ZCacheImage::SetImageVector(const std::vector < FileData > & v)
{
	CLockObjUtil lock(m_cacheLock);

	m_numImageVectorSize = v.size();

	m_cacheData.SetNewFileList(v, m_numImageVectorSize);

	m_cacheData.Clear();
	m_lCacheSize = 0;

	DebugPrintf(TEXT("imageVecSize : %d"), m_numImageVectorSize);
}

void ZCacheImage::StartThread()
{
	if ( ZOption::GetInstance().IsUseCache() )
	{
		DWORD dwThreadID;
		m_hThread = CreateThread(0, 0, ThreadFuncProxy, this, 0, &dwThreadID);

		// Cache �� �����ϴ� �������
		if ( SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
		{
			assert(!"Can't SetThreadPriority!");
		}
	}
}

// ���� ĳ�õǾ� �ִ� ���ϵ��� output ������� �ѷ��ش�.
void ZCacheImage::ShowCachedImageToOutputWindow()
{
#ifndef _DEBUG
	return; // ������ ��忡���� �׳� ����
#endif

	CLockObjUtil lock(m_cacheLock);

	m_cacheData.PrintCachedData();

}

DWORD ZCacheImage::ThreadFuncProxy(LPVOID )
{
	ZCacheImage::GetInstance().ThreadFunc();
	return 0;
}


bool ZCacheImage::_CacheIndex(int iIndex)
{
	/*
	// �ִ� ĳ�� ũ�⸦ �Ѿ����� �� �̻� ĳ������ �ʴ´�.
	if ( m_iCurrentIndex != iIndex &&
		((m_lCacheSize / 1024 / 1024) > ZOption::GetInstance().GetMaxCacheMemoryMB()) )
	{
		return false;
	}
	*/

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_numImageVectorSize ) iIndex = (int)m_numImageVectorSize - 1;

	// �̹� ĳ�õǾ� �ִ��� ã�´�.
	bool bFound = false;
	tstring strFileName;

	strFileName = m_cacheData.GetFilenameFromIndex(iIndex); ;//m_imageIndex2FilenameMap[iIndex];

	if ( strFileName.length() <= 0 ) return false;

	{
		CLockObjUtil lock(m_cacheLock);
		if ( m_cacheData.HasCachedData(strFileName) )
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// ĳ�õǾ� ���� ������ �о���δ�.
	{
		ZImage cacheReadyImage;

		m_vBuffer.resize(0);
		HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_POSIX_SEMANTICS,  NULL);

		bool bLoadOK = false;

		if ( INVALID_HANDLE_VALUE == hFile )
		{
			assert(false);
			bLoadOK = false;
		}
		else
		{
			enum
			{
				readBufferSize = 2048
			};
			
			BYTE readBuffer[readBufferSize];

			DWORD dwReadBytes;
			BOOL bReadOK = TRUE;

			DWORD dwStart = GetTickCount();

			while ( bReadOK )
			{
				if ( m_bNewChange)
				{
					CloseHandle(hFile);
					DebugPrintf(TEXT("---------------------------- stop readfile"));
					return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
				}
				bReadOK = ReadFile(hFile, readBuffer, readBufferSize, &dwReadBytes,  NULL);
				if ( m_bNewChange)
				{
					CloseHandle(hFile);
					DebugPrintf(TEXT("---------------------------- stop readfile"));
					return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
				}
				if ( FALSE == bReadOK )
				{
					assert(false);
					bLoadOK = false;
					break;
				}
				else
				{
					if ( dwReadBytes <= 0 )	///< ������ ������ �о���.
					{
						break;
					}
					else /// 
					{
						m_vBuffer.resize(m_vBuffer.size() + dwReadBytes);

						memcpy((&(m_vBuffer[0])) + m_vBuffer.size() - dwReadBytes, readBuffer, dwReadBytes);
					}
				}
			}
			DWORD dwEnd = GetTickCount();
			DebugPrintf(TEXT("----- readfile(%s) time(%d)"), strFileName.c_str(), dwEnd - dwStart);
			CloseHandle(hFile);	///< ���Ͽ��� �бⰡ ������ ������ �ݾ��ش�.

			/// todo: �Ʒ� ������ ��� ������ �ٲٸ� �� ��������
			fipMemoryIO mem(&m_vBuffer[0], (DWORD)m_vBuffer.size());
			
			DebugPrintf(TEXT("----- start decode(%s)"), strFileName.c_str());
			TIMECHECK_START("decode time");
			bLoadOK = cacheReadyImage.LoadFromMemory(mem, strFileName);
			DebugPrintf(TEXT("----- end of decode(%s)"), strFileName.c_str());
			TIMECHECK_END();
		}

		if ( bLoadOK == false )
		{
			assert(!"Can't load image");

			tstring strErrorFilename = GetProgramFolder();
			strErrorFilename += TEXT("LoadError.png");
			if ( !cacheReadyImage.LoadFromFile(strErrorFilename) )
			{
				MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);

				// ���� �� ǥ���ϴ� ������ �о������ �� ������
				//ShowMessageBox(GetMessage(TEXT("CANNOT_LOAD_ERROR_IMAGE_FILE")));

				HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
				cacheReadyImage.CopyFromBitmap(hBitmap);
			}
		}


		if ( bLoadOK )
		{
			/*
			{/// Debug Code
				//if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

				AddCacheData(strFileName, temp);

				if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
				*/

				/*
				WORD tempWidth, tempHeight;
				long tempImageSize = cacheReayImage.GetImageSize();

				tempWidth = cacheReayImage.GetWidth();
				tempHeight = cacheReayImage.GetHeight();

				temp.Resize(10, 10);

				tempWidth = cacheReayImage.GetWidth();
				tempHeight = cacheReayImage.GetHeight();
				tempImageSize = cacheReayImage.GetImageSize();
			}
			*/

#if 0
			if ( ZOption::GetInstance().IsBigToSmallStretchImage() )
			{
				RECT screenRect = { 0 };
				if ( false == ZMain::GetInstance().getCurrentScreenRect(screenRect) ) return false;

				RECT imageRect = { 0 };
				imageRect.right = cacheReayImage.GetWidth();
				imageRect.bottom = cacheReayImage.GetHeight();

				if ( imageRect.right > screenRect.right || imageRect.bottom > screenRect.bottom )
				{
					RECT newRect = GetResizedRectForBigToSmall(screenRect, imageRect);

					if ( newRect.right != imageRect.right || newRect.bottom != imageRect.bottom )
					{
						DebugPrintf(TEXT("Resizing Cache..."));
						cacheReayImage.Resize((WORD)newRect.right, (WORD)newRect.bottom);
					}
				}
			}

			if ( ZOption::GetInstance().IsSmallToBigStretchImage() )
			{
				RECT screenRect;
				if ( false == ZMain::GetInstance().getCurrentScreenRect(screenRect) ) return false;

				/*
				if ( screenRect.right > 10 )
				{
					--screenRect.right;
				}
				if ( screenRect.bottom > 10 )
				{
					--screenRect.bottom;
				}
				*/

				RECT imageRect = { 0 };
				imageRect.right = cacheReayImage.GetWidth();
				imageRect.bottom = cacheReayImage.GetHeight();

				if ( imageRect.right < screenRect.right && imageRect.bottom < screenRect.bottom )
				{
					RECT newRect = GetResizedRectForSmallToBig(screenRect, imageRect);

					if ( newRect.right != imageRect.right || newRect.bottom != imageRect.bottom )
					{
						DebugPrintf(TEXT("Resizing Cache..."));
						cacheReayImage.Resize((WORD)newRect.right, (WORD)newRect.bottom);
					}
				}
			}
#endif
			const long CachedImageSize = cacheReadyImage.GetImageSize();

			if ( iIndex == m_iCurrentIndex )	///< ���� ���� �̹����� ������ �ִ´�.
			{
				AddCacheData(strFileName, cacheReadyImage, true);
				return true;
			}
			// ���� �̹����� ���� ������ ������
			else if ( (m_lCacheSize + CachedImageSize) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
			{
				int iTemp = 100;
				int iFarthestIndex = -1;

				do
				{
					// ĳ�õǾ� �ִ� �͵� �� ���� ���� index ���� �հ��� ã�´�.
					
					{
						CLockObjUtil lock(m_cacheLock);
						iFarthestIndex = m_cacheData.GetFarthestIndexFromCurrentIndex(m_iCurrentIndex);
					}
					assert(iFarthestIndex >= 0 );

					size_t nCachedFarthestDiff = abs(iFarthestIndex - m_iCurrentIndex);
					size_t nToCacheDiff = abs(iIndex - m_iCurrentIndex );

					if ( nCachedFarthestDiff < nToCacheDiff )
					{
						// ĳ�� �ߴ� �� �� ���� �ָ��ִ� ���� �̹��ź��� ���� ������ ���̻� ĳ������ �ʴ´�
						return false;
					}
					else if ( nCachedFarthestDiff == nToCacheDiff )
					{
						// ĳ���ߴ� �Ŷ� �̹��� ĳ���� ���� ������ ��ġ�� ������

						if ( m_lastActionDirection == eLastActionDirection_FORWARD )
						{
							// ������ ���� ���̸� ���� �ָ��ִ� ���� prev �̸� �����(������ ���� ���� ���� next image �� �켱������ ����)
							if ( iFarthestIndex >= iIndex )
							{
								// ĳ�õǾ� �ִ� ���� ����� �ʴ´�.
								return false;
							}
						}
						else
						{
							// �ڷ� ���� ���̸� ���� �ָ��ִ� ���� next �̸� �����.
							if ( iFarthestIndex <= iIndex )
							{
								return false;
							}
						}
					}

					//  ���� ���� �� ������ ������ ���� �� ���� Ŭ�����ϰ�, ���� ���� ĳ���� ���� ���� ���� ��Ȳ�̴�.

					// ���� �� ���� clear �Ѵ�.
					{
						CLockObjUtil lock(m_cacheLock);
						m_cacheData.ClearFarthestDataFromCurrent(iFarthestIndex, m_lCacheSize);
					}

					// ���� ��� ���� �뷮�� Ȯ�������� �ٽ� �� �̹����� ���� �� �ִ� �� ĳ�ø� üũ�Ѵ�.
					if ( (m_lCacheSize + CachedImageSize) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB() )
					{
						AddCacheData(strFileName, cacheReadyImage);
						return true;
					}
					else
					{
						DebugPrintf(TEXT("There is no vacant space"));
					}

					// ������ ���ѷ����� �����ϱ� ���� 100���� ������.
					--iTemp;
				} while( iTemp > 0 );

				assert(iTemp >= 0 );
			}
			else
			{
				AddCacheData(strFileName, cacheReadyImage);
				if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
			}
		}
	}
	return true;
}

void ZCacheImage::ThreadFunc()
{
	m_bNewChange = false;
	CacheMapIterator it, endit;
	int iPos = 0;
	int i = 0;

	while ( m_bCacheGoOn ) // thread loop
	{
		m_bNowCaching = true;
		iPos = 0;
		assert((int)m_numImageVectorSize == (int)m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_numImageVectorSize);

#ifdef _DEBUG

		{
			CLockObjUtil lock(m_cacheLock);
			if ( m_cacheData.IsEmpty() )
			{
				assert(m_lCacheSize == 0);
			}
		}
#endif

		for ( i=0; i<ZOption::GetInstance().m_iMaxCacheImageNum/2; ++i)
		{
			if ( false == m_bCacheGoOn ) break; ///< ���α׷��� ����Ǿ����� for �� ������.
			if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

			/// �׻� ���� �̹����� ���� ĳ���Ѵ�.
			_CacheIndex(m_iCurrentIndex);
			if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

			/// ���纸�� �ִ� ���⿡ ���� ����� �̹����� ���� ĳ���� ������ �Ǵ��Ѵ�.

			if ( m_lastActionDirection == eLastActionDirection_FORWARD )
			{
				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
				if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
				if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
			}
			else if ( m_lastActionDirection == eLastActionDirection_BACKWARD )
			{
				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
				if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
				if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
			}
			else
			{
				assert(false);
			}

			++iPos;
		}

		//DebugPrintf("wait event");

		m_bNowCaching = false;
		m_hCacheEvent.wait();
		m_bNewChange = false;

		DebugPrintf(TEXT("Recv event"));
	}
}

bool ZCacheImage::hasCachedData(const tstring & strFilename, int iIndex)
{
	if ( false == ZOption::GetInstance().IsUseCache() ) return false;	///< ���� ĳ���� ���� ������ ���°Ŵ�.

	// index �� üũ�Ѵ�.
	m_iCurrentIndex = iIndex;
	m_strCurrentFileName = strFilename;

	m_bNewChange = true;

	m_hCacheEvent.setEvent();

	{
		CLockObjUtil lock(m_cacheLock);

		if ( m_cacheData.HasCachedData(strFilename) ) return true;
	}
	return false;
}

void ZCacheImage::getCachedData(const tstring & strFilename, ZImage & image)
{
	CLockObjUtil lock(m_cacheLock);

	m_cacheData.GetCachedData(strFilename, image);
}


void ZCacheImage::AddCacheData(const tstring & strFilename, ZImage & image, bool bForceCache)
{
	if ( false == image.IsValid() )
	{
		assert(false);
		return;
	}

	long imageSize = image.GetImageSize();

	CLockObjUtil lock(m_cacheLock);

	/// �̹� ĳ�õǾ� ������ ĳ������ �ʴ´�.
	if ( m_cacheData.HasCachedData(strFilename) ) return;

	if ( false == bForceCache )
	{
		/// �뷮�� üũ�ؼ� �� �̹����� ĳ������ �� ������ �Ѿ���� ĳ������ �ʴ´�.
		if ( (m_lCacheSize + imageSize) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() ) return;
	}

	if ( false == m_bCacheGoOn ) return;

#ifndef _DEBUG
	try
	{
#endif
		DWORD dwStart = GetTickCount();

		m_cacheData.InsertData(strFilename, image);
		m_lCacheSize += imageSize;

		DebugPrintf(TEXT("%s added to cache"), strFilename.c_str());
		DWORD dwEnd = GetTickCount();
		DebugPrintf(TEXT("Cache insert time : %d filename(%s)"), dwEnd - dwStart, strFilename.c_str());

#ifndef _DEBUG
	}
	catch ( ... )
	{
		return;
	}
#endif
}

/// ���� ������ ĳ���Ǿ����� üũ�ؼ� �����ش�.
bool ZCacheImage::IsNextFileCached() const
{
	CLockObjUtil lock(m_cacheLock);

	int iNextIndex = m_iCurrentIndex;

	if ( m_lastActionDirection == eLastActionDirection_FORWARD )
	{
		++iNextIndex;
	}
	else
	{
		--iNextIndex;
	}

	iNextIndex = ZMain::GetInstance().GetCalculatedMovedIndex(iNextIndex);

	return m_cacheData.HasCachedData(iNextIndex);
}

/// ���� ĳ�� ������ ����� �ֿܼ� �����ش�. ������� ����
void ZCacheImage::debugShowCacheInfo()
{
	RECT rt;
	if ( false == ZMain::GetInstance().getCurrentScreenRect(rt) )
	{
		DebugPrintf(TEXT("CurrentScreenSize : Cannot getCurrentScreenRect"));
		return;
	}
	DebugPrintf(TEXT("CurrentScreenSize : %d, %d"), rt.right, rt.bottom);

	CLockObjUtil lock(m_cacheLock);
	m_cacheData.ShowCacheInfo();
}


void ZCacheImage::clearCache()
{
	CLockObjUtil lock(m_cacheLock);
	m_cacheData.Clear();
	m_lCacheSize = 0;
	DebugPrintf(TEXT("Clear cache data"));
}


long ZCacheImage::GetCachedKByte() const
{
	return (m_lCacheSize/1024);
}
