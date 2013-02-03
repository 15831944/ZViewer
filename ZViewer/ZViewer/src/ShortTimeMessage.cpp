﻿/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * ShortTimeMessage.cpp
 *
 * 2006. 3. 13 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "ShortTimeMessage.h"

using namespace std::chrono;

CShortTimeMessage & CShortTimeMessage::getInstance()
{
    static CShortTimeMessage inst;
    return inst;
}


CShortTimeMessage::CShortTimeMessage()
{
}


CShortTimeMessage::~CShortTimeMessage()
{
}


void CShortTimeMessage::setMessage(const std::string & strMsg, DWORD dwShowTimeInMilliSec)
{
	m_strMessage = strMsg;

	m_hideTick = system_clock::now();
	m_hideTick += milliseconds(dwShowTimeInMilliSec);
}

/// 타이머가 호출되었을 때
void CShortTimeMessage::onTimer()
{
	long long remainTimeToShow = duration_cast<milliseconds>(m_hideTick - system_clock::now()).count();

	/// 보여줄 메시지의 시간이 지나지 않았으면 화면에 그린다.
	if ( remainTimeToShow > 0 )
	{
		//ZMain::GetInstance().drawShortMessage(m_strMessage);
	}
	else /// 보여줄 시간이 지났으면 그리지 않는다.............. 이전 메시지가 화면에 있으면 지워야한다..
	{
		//ZMain::GetInstance().drawShortMessage("");`
	}
}