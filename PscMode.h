#ifndef _PSCMODE_H_
#define _PSCMODE_H_

#include "ComStruct.h"
#include "ManaKernel.h"
#include "ComDef.h"
#include "time.h"

#define CPSCMODE_FILE "CPscMode.cpp"

const int MAX_PUSHS = 8;   //�������˰�ť��

/***********************************************************************
CROSS1 running:
Push Button               Condiction match
STANDYSTATUS------------->WAITPGSTATUS----------------->PGENABLESTATUS
Time is over
---------->STANDYSTATUS

CROSS2 running:
Push Button              Condiction Match
STANDYSTATUS------------>WAITPGSTATUS----------------->PGENABLESTATUS
Time is over                Condiction Match
----------->NXTWAITPGSTATUS----------------->NXTPGENABLESTATUS

*************************************************************************/	

/*
*�źŻ�pscģʽ
*/
class CPscMode
{
public:
	static CPscMode* CreateInstance();
	void DealButton();
	void DecTime();
	void GoNextStep();
	void PscSwitchStatus();
	void InitPara();
	void DealPSC();
	Byte m_ucBtnNum  ;
private:
	CPscMode();
	~CPscMode();

	bool m_bStartCntCown;         //�Ƿ�ʼ����ʱ
	bool m_bStep1ToStep2;         //����1������2 true:����++ false������--
	bool m_bCrossStage1First;     //���˽׶���ǰ
	bool m_bBoxPush[MAX_PUSHS];  //������ť״̬
	Byte m_ucPscStatus;    //PSC ����״̬
	Byte m_ucCurStep;      //�źŻ���ǰ������
	Byte m_ucStandStep;    //��׼������
	Byte m_ucCrossStep1;   //����ͨ��������1
	Byte m_ucCrossStep2;   //����ͨ��������2
	time_t m_ucNextTime ;   //���ڼ�¼����ͨ�н���ʱ�䣬���ڼ����������˰�ť��Ч���ʱ�䡣
	Byte m_psc_intervaltime ;
	CManaKernel* m_pWorkParaManager;
};

#endif //_PSCMODE_H_
