#ifndef _LOG_H_
#define _LOG_H_

#include "ace/OS.h"
#include "ace/Task.h"
#include "ace/Method_Request.h"
#include "ace/Activation_Queue.h"
#include "ace/Auto_Ptr.h"

#include "ComStruct.h"

/*
*��־��¼ ��ȡ�������ģʽ 
*��ֹ��¼��־����ʱ�����Ӱ�쵽�����߳�
*/
class CLogger:public ACE_Task<ACE_MT_SYNCH>
{
public:
	CLogger();
	int svc();
	void WriteEventLog(Byte ucEvtType,Uint uiEvtValue);
	void WriteEventLogActive(Byte ucEvtType,Uint uiEvtValue);
	void SetMaxMinId(Uint ucMaxId,Uint ucMinId);

public:
	Uint m_uiLogMaxId;   //��ǰ��־�������id
	Uint m_uiLogMinId;   //��ǰ��־��С����id
private:
	ACE_Activation_Queue m_queCmd;    //�������
};

class CLogCmd:public ACE_Method_Request
{
public:
	CLogCmd(CLogger *pLog,Byte ucEvtType,Uint uiEvtValue);
	int call();

private:
	Byte m_ucEvtType;   //�¼�����
	Uint m_uiEvtValue;  //�¼�ֵ
	CLogger* m_pLog;   
};

#endif  //_LOG_H_
