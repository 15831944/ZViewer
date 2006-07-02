/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZImage.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include "FreeImagePlus.h"
#include "CommonFunc.h"

/// FreeImagePlus �� Ŭ������ fipWinImage �� �����Ͽ� �Լ� ���� ���� ���� �ϴ� Ŭ����
class ZImage
{
public:
	static bool StartupLibrary(){ return true; }
	static bool CleanupLibrary(){ return true; }
	static bool IsValidImageFileExt(const char * szFilename);
	static const char * GetLibraryVersion()
	{
		return FreeImage_GetVersion();
	}

	void clear()
	{
		m_image.clear();
	}

	WORD GetWidth() { return m_image.getWidth(); }
	WORD GetHeight() { return m_image.getHeight(); }

	WORD GetOriginalWidth() { return m_originalWidth; }
	WORD GetOriginalHeight() { return m_originalHeight; }

	WORD GetBPP() { return m_image.getBitsPerPixel(); }
	BYTE * GetData() { return m_image.accessPixels(); }
	BITMAPINFO * GetBitmapInfo() { return m_image.getInfo(); }
	bool IsValid() { return (m_image.isValid() == TRUE); }
	bool LoadFromFile(const std::string & strFilename)
	{
		//DebugPrintf("LoadFromFile : %s", strFilename.c_str());

		try
		{
			if ( m_image.load(strFilename.c_str()) == TRUE )
			{
				m_originalWidth = m_image.getWidth();
				m_originalHeight = m_image.getHeight();
				m_originalSize = m_image.getImageSize();
				return true;
			}
		}
		catch ( ... )
		{
#pragma message("TODO : �� �κп� exception �� �߻��ϴ� ������ ã�ƾ���...")
			_ASSERTE(false);
			return false;
		}
		return false;
	}

	bool SaveToFile(const std::string & strFilename, int iFlag)
	{
		return ( TRUE == m_image.save(strFilename.c_str(), iFlag));
	}

	void Rotate(double dAngle) { m_image.rotate(dAngle); }

	bool Resize(WORD newWidth, WORD newHeight)
	{
		return ( m_image.rescale(newWidth, newHeight, FILTER_BOX) == TRUE );
	}

	bool ConvertTo32Bit() { return ( TRUE == m_image.convertTo32Bits() ); }

	long GetImageSize()
	{
		return m_image.getImageSize();
	}

	long GetOriginalImageSize()
	{
		return m_originalSize;
	}

	bool isTransparent()
	{
		return (m_image.isTransparent() == TRUE);
	}

	const fipWinImage & getFipImage()
	{
		return m_image;
	}

private:
	fipWinImage m_image;

	/// Image's original width - before resizing
	WORD m_originalWidth;

	/// Image's original height - before resizing
	WORD m_originalHeight;

	/// Image's original size - before resizing
	long m_originalSize;
};