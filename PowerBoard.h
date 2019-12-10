#ifndef _POWERBOARD_H_
#define _POWERBOARD_H_

#include "ComDef.h"
#include <ace/Thread_Mutex.h>
#include <ace/OS.h>

#define POWERBOARD_DEBUG

class CPowerBoard
{
public:
	static CPowerBoard* CreateInstance();

public:
	//��ѹ���ݣ���ѹ״̬
	void CheckVoltage();

	//���ذ������Դģ�鷢����������
	void GetPowerBoardCfg();

	//���ذ巢�͸���Դģ����������
	void SetPowerBoardCfg();
	void SetPowerCfgData(Byte m_ucWarnHighVol,Byte m_ucWarnLowVol,Byte m_ucVolPlan,Byte m_ucDogCfg);
	//�㲥���� 
	void HeartBeat();
	//���յ�Դ���Can���ݲ�����
	void RecvPowerCan(Byte ucBoardAddr,SCanFrame sRecvCanTmp);
	
	int GetStongVoltage() ;//��ȡ�ߵ�ѹADD��201309281540
	void GetPowerVer(Byte PowerBdindex); //��ȡ��Դ��汾
	static Byte iHeartBeat ;
private:
	CPowerBoard();
	~CPowerBoard();

public:
	//input
	Byte m_iSetWarnHighVol;  //��ѹԤ����ѹ(����ֵ)
	Byte m_iSetWarnLowVol;   //��ѹԤ����ѹ(����ֵ)
	Byte m_ucSetStongHighVolPlan;  //ǿ���ѹԤ��
	Byte m_ucSetStongLowVolPlan;   //ǿ���ѹԤ��
	Byte m_ucSetWeakHighVolPlan;   //�����ѹԤ��
	Byte m_ucSetWeakLowVolPlan;    //�����ѹԤ��

	//output
	int m_iStongVoltage;  //ǿ���ѹ
	int m_iWeakVoltage;   //�����ѹ
	int m_iBusVoltage;    //���ߵ�ѹ

	int m_iGetWarnHighVol;  //��ѹԤ����ѹ(����ֵ)
	int m_iGetWarnLowVol;   //��ѹԤ����ѹ(����ֵ)
	Byte m_ucGetStongHighVolPlan;  //ǿ���ѹԤ��
	Byte m_ucGetStongLowVolPlan;   //ǿ���ѹԤ��
	Byte m_ucGetWeakHighVolPlan;   //�����ѹԤ��
	Byte m_ucGetWeakLowVolPlan;    //�����ѹԤ��
	Byte m_ucSetWatchCfg ;         //��Դ�忴�Ź����� //ADD:201404021301
	Byte m_ucPowerBoardVer[MAX_POWERBOARD][5];

	ACE_Thread_Mutex  m_mutexVoltage;

};

#endif //_POWERBOARD_H_

