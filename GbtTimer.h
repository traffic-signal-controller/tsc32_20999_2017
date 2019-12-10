#ifndef _CGBTTIMER_H_
#define _CGBTTIMER_H_

#include "ComStruct.h"
#include "GbtMsgQueue.h"

const int MAX_REPORT_NUM = 4;   //��������ϱ���Ϣ

/*
*gbt�����ϱ���ʱ��
*/
class CGbtTimer : public ACE_Event_Handler
{
public:
	static CGbtTimer* CreateInstance();
	virtual int handle_timeout(const ACE_Time_Value &tCurrentTime, const void * = 0); //��ʱ���ص�����

#ifdef GBT_TCP
	void TailorReport(Byte ucGbtArrIndex , Uint uBufCnt , Byte* pBuf);
	void SendErrMsg(Byte ucGbtArrIndex);
	bool CreateReport(Byte ucGbtArrIndex,Byte ucObjectCmd , Ushort usCycle);
#else
	void TailorReport(ACE_INET_Addr& addrClient , Uint uBufCnt , Byte* pBuf);
	void SendErrMsg(ACE_INET_Addr& addrClient);
	bool CreateReport(ACE_INET_Addr& addrClient,Byte ucObjectCmd , Ushort usCycle);
#endif

private:
	CGbtTimer();
	~CGbtTimer();

private:
	Byte m_ucTick;
	Byte m_ucMaxTick;
	ACE_Thread_Mutex  m_sMutex;
	SReportSelf m_sReportInfo[MAX_REPORT_NUM];
};

#endif //_CGBTTIMER_H_

