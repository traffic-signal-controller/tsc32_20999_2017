#ifndef _LAMP_H_
#define _LAMP_H_

#include "ComDef.h"
#include "ComStruct.h"

#define LAMP_BOARD_START_ADDR 5  /*��ʼ�ƿذ��λ��*/
#define LAMP_CPP "Lamp.cpp"

/*
*���Ƶƾ���Ϣ�������
*������Ŀ����ʵ��
*/
class CLamp 
{
public:
	static CLamp* CreateInstance();
	void SetFlashFre(Byte ucFlashFre);
	void SetLamp(Byte* pLampOn,Byte* pLampFlash);
	void SendLamp();
	void SendToCom1();
	void RecvFromCom1(int iBoardIndex);
	//void SendLampStsToCom2();
	void GetLampBoardExit(bool* bLampBoardExit);
	void IsGreenConfict();
	void SetOverCycle();
	void SetSeriousFlash();
	void SendRecordBoardMsg(Byte ucBoardIndex,Byte ucType);
	void PrintLampInfo(char* pFileName,int iFileLine,Byte ucBoardIndex,char* sErrSrc,int iPrintCnt,Byte* ucRecvBuf);
	bool CheckSendFrame(Byte* ucSendFrame);

	bool m_bRecordSts[MAX_LAMP_BOARD];      //��һ�μ�¼�ƿذ��ͨ��״̬

private:
	CLamp();
	~CLamp();

#ifdef WINDOWS
	void SimulateSend();
#endif
	
	bool LampGreenAlwaysOn();
	bool LampAllRedOff();
	//bool InConflictPhase();
	void ReviseLampInfo(Byte ucType,Byte* pLampInfo);
	
	bool m_bSeriousFlash;
	bool m_firstSend;   //��һ�η���
	bool m_bLampBoardExit[MAX_LAMP_BOARD];  //������ĵƿذ��Ƿ����

	Byte m_ucNoCnt[MAX_LAMP_BOARD];         //����û�н������ݵĴ���
	Byte m_ucErrAddrCnt[MAX_LAMP_BOARD];    //�������ܵ������ַ�Ĵ���
	Byte m_ucErrCheckCnt[MAX_LAMP_BOARD];   //�������ܵ�У�����Ĵ���
	Byte m_ucRightCnt[MAX_LAMP_BOARD];      //�������ܵ���ȷ���ݵĴ���

	Byte m_ucOverCnt;     //��������һ�����ں�ſ�ʼ�����ݹ���״̬
	Byte m_tick;
	Byte m_ucFlashFre;  //����Ƶ�� 1:0.5HZ 2:1HZ 4:2HZ
	Byte m_ucLampOn[MAX_LAMP];         //��������Ӷ�Ӧ ����1 ��0
	Byte m_ucLampFlash[MAX_LAMP];
	Byte m_ucLampError[MAX_LAMP];            //�ƾ߼��״̬
	Byte m_ucLastLampError[MAX_LAMP];        //�ϴεƾ߼��״̬
	Byte m_ucLampErrTime[MAX_LAMP];          //�������
	Byte m_ucLampBoardError[MAX_LAMP_BOARD]; //�ƿذ���״̬
	bool m_bLampErrFlag[MAX_LAMP];           //���ݴ����־
	bool m_bLampGreenFlag[MAX_LAMP];         //�̳�ͻ��¼

	Byte m_ucLampStatus[3*MAX_LAMP_BOARD];  //3λ��ʾһ��壬����ο�ͨ��Э��

	Byte m_ucSendFrame[3+MAX_LAMP/8];  //�������� addr+len+data+sum 

	int  m_iSerial1Fd;

	int m_iGreenConflict;

//#ifdef CALLNOTEFAULT
	int  m_iSerial2Fd;
//#endif

#ifdef WINDOWS
	unsigned short m_usLampSendData[MAX_LAMP_BOARD];
#endif
	ACE_Thread_Mutex  m_mMutex; 
	ACE_Thread_Mutex  m_mutexLamp;

	STscConfig* m_pTscCfg;

};

#endif //_LAMP_H_

