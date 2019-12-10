#ifndef _TSCTIMER_H_
#define _TSCTIMER_H_

#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "Define.h"
/*
*�źŻ����ƶ�ʱ�����緢�������¼���
*/
class CTscTimer : public ACE_Event_Handler
{
public: 
	CTscTimer(Byte ucMaxTick);
	~CTscTimer();
	virtual int handle_timeout(const ACE_Time_Value &tCurrentTime, const void * = 0); //��ʱ���ص�����
	void ChooseDecTime();
private:
	bool m_bWatchdog;
	Byte m_ucTick;
	Byte m_ucMaxTick;
};

#endif //_CTSCTIMER_H_

