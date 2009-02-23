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
	m_cacheData.SetImageVector(v);
}

void ZCacheImage::StartThread()
{
	DWORD dwThreadID;
	m_hThread = CreateThread(0, 0, ThreadFuncProxy, this, 0, &dwThreadID);

	// Cache �� �����ϴ� �������
	if ( SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
	{
		assert(!"Can't SetThreadPriority!");
	}
}

// ���� ĳ�õǾ� �ִ� ���ϵ��� output ������� �ѷ��ش�.
void ZCacheImage::ShowCachedImageToOutputWindow()
{
#ifndef _DEBUG
	return; // ������ ��忡���� �׳� ����
#else
	m_cacheData.PrintCachedData();
#endif
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

	if ( iIndex < 0 )///< ĳ�� �Ŵ������� ���� ������ �ڸ� ĳ���Ϸ��� �õ��ϱ� ������ �� if ���� ������ �ȴ�. assert ��Ȳ�� �ƴϴ�.
	{
		iIndex = 0;
	}
	if ( iIndex >= (int)m_cacheData.GetImageVectorSize() )///< ĳ�� �Ŵ������� ���� ������ ���� ĳ���Ϸ��� �õ��ϱ� ������ �� if ���� ������ �ȴ�. assert ��Ȳ�� �ƴϴ�.
	{
		iIndex = (int)(m_cacheData.GetImageVectorSize()) - 1;
	}

	/// �� ��Ȳ�� ���� ���� ����� �ϳ��� ���� ��Ȳ�̴�.
	if ( iIndex < 0 ) return false;

	// �̹� ĳ�õǾ� �ִ��� ã�´�.
	bool bFound = false;
	tstring strFileName;

	strFileName = m_cacheData.GetFilenameFromIndex(iIndex); ;//m_imageIndex2FilenameMap[iIndex];

	if ( strFileName.size() <= 0 ) return false;

	if ( strFileName.length() <= 0 ) return false;

	{
		if ( m_cacheData.HasCachedData(strFileName) )
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// ĳ�õǾ� ���� ������ �о���δ�.
	{
		ZImage * pCacheReadyImage = new ZImage();

		m_vBuffer.resize(0);
		HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_POSIX_SEMANTICS,  NULL);

		bool bLoadOK = false;

		if ( INVALID_HANDLE_VALUE == hFile )
		{
			//assert(false);
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
				if ( m_bNewChange && m_iCurrentIndex != iIndex )	///< ���ο� �׸����� �Ѿ�µ� ���� �ε����� �ƴϸ�
				{
					CloseHandle(hFile);
					DebugPrintf(TEXT("---------------------------- stop readfile"));
					delete pCacheReadyImage;
					return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
				}
				bReadOK = ReadFile(hFile, readBuffer, readBufferSize, &dwReadBytes,  NULL);
				if ( m_bNewChange && m_iCurrentIndex != iIndex )	///< ���ο� �׸����� �Ѿ�µ� ���� �ε����� �ƴϸ�
				{
					CloseHandle(hFile);
					DebugPrintf(TEXT("---------------------------- stop readfile"));
					delete pCacheReadyImage;
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

			if ( m_vBuffer.size() < 5 )
			{
				/// ���� ũ�Ⱑ �ʹ� ����
				bLoadOK = false;
			}
			else
			{
				DWORD dwEnd = GetTickCount();
				DebugPrintf(TEXT("----- readfile(%s) time(%d)"), strFileName.c_str(), dwEnd - dwStart);
				CloseHandle(hFile);	///< ���Ͽ��� �бⰡ ������ ������ �ݾ��ش�.

				assert(m_vBuffer.size() > 0);
				/// todo: �Ʒ� ������ ��� ������ �ٲٸ� �� ��������
				fipMemoryIO mem(&m_vBuffer[0], (DWORD)m_vBuffer.size());

				DebugPrintf(TEXT("----- start decode(%s)"), strFileName.c_str());
				TIMECHECK_START("decode time");
				bLoadOK = pCacheReadyImage->LoadFromMemory(mem, strFileName);
				DebugPrintf(TEXT("----- end of decode(%s)"), strFileName.c_str());
				TIMECHECK_END();

				/// �ɼǿ� ���� �ڵ� ȸ���� ��Ų��.
				if ( ZOption::GetInstance().IsUseAutoRotation() )
				{
					pCacheReadyImage->AutoRotate();
				}
			}
		}

		if ( bLoadOK == false )
		{
			assert(!"Can't load image");

			tstring strErrorFilename = GetProgramFolder();
			strErrorFilename += TEXT("LoadError.png");
			if ( pCacheReadyImage->LoadFromFile(strErrorFilename) )
			{
				bLoadOK = true; ///< error image �� load ok
			}
			else
			{
				MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);

				// ���� �� ǥ���ϴ� ������ �о������ �� ������
				//ShowMessageBox(GetMessage(TEXT("CANNOT_LOAD_ERROR_IMAGE_FILE")));

				HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
				pCacheReadyImage->CopyFromBitmap(hBitmap);
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
			const long CachedImageSize = pCacheReadyImage->GetImageSize();

			if ( iIndex == m_iCurrentIndex )	///< ���� ���� �̹����� ������ �ִ´�.
			{
				AddCacheData(strFileName, pCacheReadyImage, true);
				return true;
			}
			// ���� �̹����� ���� ������ ������
			else if ( (m_cacheData.GetCachedTotalSize() + CachedImageSize) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
			{
				int iTemp = 100;
				int iFarthestIndex = -1;

				do
				{
					// ĳ�õǾ� �ִ� �͵� �� ���� ���� index ���� �հ��� ã�´�.
					{
						iFarthestIndex = m_cacheData.GetFarthestIndexFromCurrentIndex(m_iCurrentIndex);
					}
					assert(iFarthestIndex >= 0 );

					size_t nCachedFarthestDiff = abs(iFarthestIndex - m_iCurrentIndex);
					size_t nToCacheDiff = abs(iIndex - m_iCurrentIndex );

					if ( nCachedFarthestDiff < nToCacheDiff )
					{
						// ĳ�� �ߴ� �� �� ���� �ָ��ִ� ���� �̹��ź��� ���� ������ ���̻� ĳ������ �ʴ´�
						delete pCacheReadyImage;
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
								delete pCacheReadyImage;
								return false;
							}
						}
						else
						{
							// �ڷ� ���� ���̸� ���� �ָ��ִ� ���� next �̸� �����.
							if ( iFarthestIndex <= iIndex )
							{
								delete pCacheReadyImage;
								return false;
							}
						}
					}

					//  ���� ���� �� ������ ������ ���� �� ���� Ŭ�����ϰ�, ���� ���� ĳ���� ���� ���� ���� ��Ȳ�̴�.

					// ���� �� ���� clear �Ѵ�.
					{
						m_cacheData.ClearFarthestDataFromCurrent(iFarthestIndex);
					}

					// ���� ��� ���� �뷮�� Ȯ�������� �ٽ� �� �̹����� ���� �� �ִ� �� ĳ�ø� üũ�Ѵ�.
					if ( (m_cacheData.GetCachedTotalSize() + CachedImageSize) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB() )
					{
						AddCacheData(strFileName, pCacheReadyImage);
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
				AddCacheData(strFileName, pCacheReadyImage);
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
		assert((int)m_cacheData.GetImageVectorSize() == (int)m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_cacheData.GetIndex2FilenameMapSize());
		assert(m_iCurrentIndex <= (int)m_cacheData.GetImageVectorSize());

#ifdef _DEBUG

		{
			if ( m_cacheData.IsEmpty() )
			{
				assert(m_cacheData.GetCachedTotalSize() == 0);
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
	// index �� üũ�Ѵ�.
	m_iCurrentIndex = iIndex;
	m_strCurrentFileName = strFilename;

	m_bNewChange = true;

	m_hCacheEvent.setEvent();

	{
		if ( m_cacheData.HasCachedData(strFilename) ) return true;
	}
	return false;
}

void ZCacheImage::GetCachedData(const tstring & strFilename, ZImage * & pImage)
{
	m_cacheData.GetCachedData(strFilename, pImage);

	assert(pImage);
}


void ZCacheImage::AddCacheData(const tstring & strFilename, ZImage * pImage, bool bForceCache)
{
	if ( false == pImage->IsValid() )
	{
		assert(false);
		return;
	}

	if ( false == m_bCacheGoOn ) return;

	m_cacheData.InsertData(strFilename, pImage, bForceCache);
}

/// ���� ������ ĳ���Ǿ����� üũ�ؼ� �����ش�.
bool ZCacheImage::IsNextFileCached() const
{
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

	m_cacheData.ShowCacheInfo();
}


void ZCacheImage::clearCache()
{
	m_cacheData.ClearCachedImageData();
	DebugPrintf(TEXT("Clear cache data"));
}


long ZCacheImage::GetCachedKByte() const
{
	return (m_cacheData.GetCachedTotalSize()/1024);
}
