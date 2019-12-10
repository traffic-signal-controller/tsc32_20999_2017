#ifndef _TSCMSGQUEUE_H_
#define _TSCMSGQUEUE_H_

#include "ace/Message_Queue.h"
#include "ace/Synch.h"

#include "ComStruct.h"
#include "Log.h"

/*
*�źŻ����Ŀ�����Ϣ����
*����ʵ������
*/
class CTscMsgQueue 
{
public:
	static CTscMsgQueue* CreateInstance();
	void DealData();
	int SendMessage(SThreadMsg* pMsg,int iLen);
	void WriteEventLog(Byte uEvtType, Byte* pEvtValue);

/*
public:
	Byte m_ucLogMaxId;   //��־����������
	Byte m_ucLogMinId;   //��־���������С��id
*/

private:
	CTscMsgQueue();
	~CTscMsgQueue();

	CLogger m_logger;
	ACE_Message_Queue<ACE_MT_SYNCH>* m_pMsgQue;
};

#endif  //_TSCMSGQUEUE_H_

