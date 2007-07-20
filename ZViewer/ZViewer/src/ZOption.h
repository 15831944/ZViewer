/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include "CommonFunc.h"
#include "OptionFile.h"

class StringToValue
{
	enum eValueType
	{
		eValueType_INT,
		eValueType_BOOL,
	};

public:
	StringToValue(tstring & val, int * iValue)
	{
		_Init(val, iValue, eValueType_INT);
	}

	StringToValue(tstring & val, bool * bValue)
	{
		_Init(val, bValue, eValueType_BOOL);
	}


	void InsertValueToMap(iniMap & mapData)
	{
		switch ( m_valueType )
		{
		case eValueType_BOOL:
			{
				bool * pData = (bool*)m_pData;
				mapData[m_str] = (*pData) ? TEXT("true") : TEXT("false");

			}
			break;

		case eValueType_INT:
			{
				int * pData = (int*)m_pData;
				mapData[m_str] = toString(*pData);
			}
			break;
		default:
			_ASSERTE(false);
		}
	}

	void InsertMapToValue(iniMap & mapData)
	{
		switch ( m_valueType )
		{
		case eValueType_BOOL:
			{
				iniMap::const_iterator it = mapData.find(m_str);

				if ( it == mapData.end() )
				{
					return;
				}

				const tstring value = it->second;

				bool * pData = (bool*)m_pData;
				*pData = (it->second == TEXT("true"));

			}
			break;

		case eValueType_INT:
			{
				iniMap::const_iterator it = mapData.find(m_str);

				if ( it == mapData.end() )
				{
					return;
				}

				const tstring value = it->second;

				int * pData = (int*)m_pData;
				*pData = _tstoi(value.c_str());
			}
			break;
		default:
			_ASSERTE(false);
		}
	}

	const tstring & getString() const { return m_str; }
protected:

	void _Init(tstring & str, void * pData, eValueType valueType)
	{
		m_str = str;
		m_valueType = valueType;
		m_pData = pData;
	}

	void * m_pData;
	tstring m_str;
	eValueType m_valueType;
};


class ZOption
{
	ZOption();

	void _InsertSaveOptionSetting(TCHAR * str, bool * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

	void _InsertSaveOptionSetting(TCHAR * str, int * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

public:
	static ZOption & GetInstance();

	~ZOption()
	{
		SaveToFile();
	}

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}
	void SetFullScreen(bool bFullScreen) { m_bFullScreen = bFullScreen; }

	int GetMaxCacheMemoryMB() const { return m_iMaximumCacheMemoryMB; }
	void SetMaxCacheMemoryMB(const int iMB) { m_iMaximumCacheMemoryMB = iMB; }

	const bool IsLoopImages() const { return m_bLoopImages; }
	void ToggleLoopImages() { m_bLoopImages = !m_bLoopImages; }

	const bool IsBigToSmallStretchImage() const { return m_bBigToSmallStretchImage; }
	void ToggleBigToSmallStretchImage() { m_bBigToSmallStretchImage = !m_bBigToSmallStretchImage; }

	const bool IsSmallToBigStretchImage() const { return m_bSmallToBigStretchImage; }
	void ToggleSmallToBigStretchImage() { m_bSmallToBigStretchImage = !m_bSmallToBigStretchImage; }

	const bool IsUseCache() const { return m_bUseCache; }
	//-----------------------------------------------------------
	// �Ʒ��� ���α׷� ���� �Ŀ� �ӽ÷� ���̴� �ɼǵ�. ���Ͽ� �������� �ʴ´�.

	//// �׸��� ���� ��ġ. �Ϻ� ��ȭ�� ��� ���� ��ܺ��� �����ϱ� ���ؼ�...
	bool m_bRightTopFirstDraw;

	/// �� ȭ�鿡 2���� �׸��� ������ ���ΰ�.
	bool m_bTwoInSaveView;

	/// �ִ� ĳ���� �̹��� ����. ���⼭ ���ݹ�ŭ ����, ���������� ����.
	int m_iMaxCacheImageNum;

protected:
	/// ��ü ȭ�� ����ΰ�.m_bOptionChanged(false)
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// �Ʒ��� ���������� ����Ǵ� �ɼǵ�. ���Ͽ� �����Ѵ�.

	/// ĳ�� �ý����� �����ΰ�... �޸𸮰� ���� ��ǻ�Ϳ����� �� �� �ֵ��� �ϱ� ���ؼ�
	bool m_bUseCache;

	/// ������ �׸� ���Ͽ��� �������� ���� ó�� �׸� ���Ϸ� �� ���ΰ�?
	bool m_bLoopImages;

	/// ȭ�麸�� ū �׸��� ȭ�鿡 �°� ����� ���ΰ�
	bool m_bBigToSmallStretchImage;

	/// ȭ�麸�� ���� �׸��� ȭ�鿡 �°� Ȯ���� ���ΰ�.
	bool m_bSmallToBigStretchImage;

	/// ĳ�õǴ� �ִ� �޸𸮷�
	int m_iMaximumCacheMemoryMB;

	tstring m_strOptionFilename;

	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

	void SetSaveOptions();
protected:
	/// ���Ϸκ��� �ɼ��� �о�鿩�� �ٲ� �ɼ��� �ֳ�.
	bool m_bOptionChanged;

	std::vector < StringToValue > m_saveOptions;

};

