
#ifndef WIRELESSBUTTONS_H_
#define WIRELESSBUTTONS_H_

#include "ComDef.h"	
#include "ManaKernel.h"

class CWirelessBtnTimer : public ACE_Event_Handler
{
public: 
	CWirelessBtnTimer();
	~CWirelessBtnTimer();
	static CWirelessBtnTimer* CreateInstance();
	virtual int handle_timeout(const ACE_Time_Value &tCurrentTime, const void * = 0); //��ʱ���ص�����
	void Resettimeout();  //�������߰����ֿس�ʱ����
private:
	Uint m_uctimeout ;
	Uint m_ucSystemSetTimeOut ; //���ݿ�Ԥ����ĳ�ʱʱ��
};

class CWirelessBtn 
{
public:
	CWirelessBtn();
	virtual ~CWirelessBtn();
	static CWirelessBtn* CreateInstance();	
	void RecvMacCan(SCanFrame sRecvCanTmp);  //ADD:201410211450
	void BackToAuto();
	void EntryWirelessManul();
	void HandleSpecialDirec(Uint *RecvDircData); //ADD:20141105
	bool SetCurrenStepFlash(Byte LampColor); //ADD:201411071644
	void HandWirelessBtnMsg(); //ADD:201411171521
	bool GetbHandleWirelessBtnMsg(); //ADD:201411171550
	void StartWirelessBtnTimer() ; //ADD:201411180800
	void CloseWirelessBtn() ;//ADD:201411180800
private:	
	Byte m_ucLastManualSts;  //֮ǰ���ֶ�״̬	
	Byte m_ucdirectype ;     //��һ���򱣴���� ȡֵ��Χ0-3 ����������
	SCanFrame sWirelessBtnCanMsg;      //�߳������ֿذ�����Ϣ����
	CManaKernel * pManaKernel ;
	bool bFirstOperate ;  //�Ƿ�ս����ֿ�
	bool bTransitSetp ;   //�Ƿ�ǰ���ڹ��ɲ�״̬,���簴��
	Uint RecvoldDirec ;  //�ϴ����ⷽ�򰴼�ֵ(�ϱ��� �ϱ�ֱ�ҵȵ�)
	Byte m_ucAllowLampStatus ; //��ǰ������λ��ɫ״̬	
	bool bHandleWirelessBtbMsg ; //ADD:20141117  �Ƿ���Ҫ�̴߳��������ֿذ�ť��Ϣ
	ActiveTimer m_tActiveWirelessBtnTimer; //ADD:201411180800
	long m_lWirelessBtnTimerId ; //ADD:201411180800
	CWirelessBtnTimer* m_pWirelessBtnTimer; //ADD:201411180800
};



#endif /* MANUAL_H_ */
