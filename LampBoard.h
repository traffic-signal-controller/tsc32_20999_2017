#ifndef __LAMPBOARD__H__
#define __LAMPBOARD__H__
#include "ComDef.h"
#include "ComStruct.h"
#include <ace/Thread_Mutex.h>
#include <ace/OS.h>
#include "ManaKernel.h"
#define LAMPBOARD_DEBUG
/*
*�ƿذ���Ϣ����Լ���״̬��ȡ��
*/
class CLampBoard 
{
public:
	static CLampBoard* CreateInstance();
	void ReviseLampInfo(Byte ucType,Byte pLampInfo[MAX_LAMP]);
	void SetLamp(Byte* pLampOn,Byte* pLampFlash);
	void SetSeriousFlash(bool isflash);
	bool IsFlash() ;	
	//��ɫ���� 
	void SendLamp();
	void SendSingleLamp(Byte ucLampBoardId ,Byte ucFlashBreak);
	//�ƿذ��·��������ݣ�����ݼ�⿪�أ����̳�ͻ��⿪��
	void SendCfg();
	void SendSingleCfg(Byte ucLampBoardId);
	//���ݼ�����ݣ����̳�ͻ�������
	void CheckLight();
	void CheckSingleLight(Byte ucLampBoardId);
	//���ݵ������������
	void CheckLampElect(Byte ucLampBoardId,Byte ucType);
	void CheckSingleElect(Byte ucLampBoardId);
	void CheckElect();
	//�ƿذ�����¶ȼ����¶�ֵ
	void CheckTemp();
	void CheckSingleTemp(Byte ucLampBoardId);
	void RecvLampCan(Byte ucBoardAddr,SCanFrame sRecvCanTmp); //ADD: 2013 0712 CAN���յ��ݼ������
	Byte GetLampBoardAddr(Byte LampBoardIndex); //��ȡ������CAN��ַADD:20141226
	Byte GetLampBoardVer(Byte LampBoardIndex); //��ȡ���������汾ADD20150113
	
	void GetLamp(Byte* pLampOn,Byte* pLampFlash);//��ȡ������ɫADD:20150313
	
	void SetLampChannelColor(Byte ColorType,Byte CountDownTime); //ADD:20150806
private:
	CLampBoard();
	~CLampBoard();
public:
	CManaKernel * pManakernel ;
	bool m_bRecordSts[MAX_LAMP/12];  //�����ƿذ��ͨ��״̬
	bool IsChkLight ;
	Byte m_ucCheckCfg[MAX_LAMP_BOARD];        //bit 0 1 ���Ƿ��������𻵼��,bit 2 3�����Ƿ������̳�ͻ���  //ADD: 2013 0712 1111
	
	Byte m_ucLampBoardVer[MAX_LAMP_BOARD][5]; //����������汾
private:
	//input para
	Byte m_ucLampBoardError[MAX_LAMP_BOARD] ;
	Byte m_ucLampOn[MAX_LAMP];         //��������Ӷ�Ӧ ����1 ��0
	Byte m_ucLampFlash[MAX_LAMP];
	bool m_bSeriousFlash;	
	Byte m_ucLampOnCfg[MAX_LAMP_BOARD][3] ;   // ����ÿ���ÿ��ͨ���ĵ�����������˸״�����·����ƿذ�����ñ������ƥ�� 3�ֽڵ�ÿ��λ��ʾһ��ͨ��
	bool m_bLampErrFlag[MAX_LAMP];
	//output para
	//Byte m_ucLampSts[MAX_LAMP];      //������״̬
	Byte m_ucLampStas[MAX_LAMP] ;     // �����ƿذ������״̬   //ADD: 2013 0712 1111
	Byte m_ucLampConflic[MAX_LAMP_BOARD][4] ; // �ƿذ������źŵƵĺ��̳�ͻ��� ADD:20130802 1350 
	Byte m_ucChannelSts[(MAX_LAMP+3)/4]; //����ͨ��״̬
	Ushort m_usLampElect[MAX_LAMP_BOARD][8];  //�����Ƶĵ���
	int iLampBoardTemp[MAX_LAMP_BOARD]; //�����ƿذ忨�İ����¶�
	
	ACE_Thread_Mutex  m_mutexLamp;
};

#endif   //__LAMPBOARD__H__
