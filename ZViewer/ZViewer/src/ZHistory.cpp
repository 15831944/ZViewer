
#include "stdafx.h"
#include "ZHistory.h"

ZHistory::ZHistory()
: m_iCurrentIndex(0)
{

}

void ZHistory::push_lastImageIndex(int iIndex)
{
	// ���� ��ġ ������ Redo �� �����.
	m_indexVector.resize(m_iCurrentIndex);

	// �������� �ִ´�.
	m_indexVector.push_back(iIndex);

	// index ��ġ�� �����Ѵ�.
	m_iCurrentIndex = m_indexVector.size();

}

int ZHistory::Undo()
{
	_ASSERTE(CanUndo());

	// Undo/Redo ��ġ�� �����Ѵ�.
	--m_iCurrentIndex;

	// ���������� �ô� ���� �����ش�.
	int iRet = m_indexVector[m_iCurrentIndex];


	return iRet;
}

int ZHistory::Redo()
{
	_ASSERTE(CanRedo());

	// Undo �� �ǵ�����.

	++m_iCurrentIndex;

	return m_indexVector[m_iCurrentIndex];
}