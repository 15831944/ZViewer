/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	ZHistory.h
*
*                                       http://www.wimy.com
*********************************************************************/

/*

 �� Ŭ������ ����ڰ� Ű�� �߸� �����Ͽ� ���ڱ� ������ �̹��� ���Ϸ� �Ѿ�� ���� ����
 �̹��� ������ �� ������ ����� �Ͽ� �ٽ� �ε����� �ǵ����� �ϱ� ���� Ŭ�����̴�.
*/

#pragma once

#include <list>

class ZHistory
{
public:

	ZHistory();

	std::deque < int > m_indexVector;

	void push_lastImageIndex(int iIndex);

	int Undo();
	int Redo();

	bool CanUndo() const
	{
		if ( m_indexVector.size() <= 0 ) return false;
		if ( m_iCurrentIndex <= 0 ) return false;

		return true;
	}

	bool CanRedo() const
	{
		if ( m_indexVector.size() <= 0 ) return false;
		if ( m_iCurrentIndex >= m_indexVector.size() ) return false;

		return true;
	}

protected:
	size_t m_iCurrentIndex;			// ���� Undo/Redo �� ��ġ
};
