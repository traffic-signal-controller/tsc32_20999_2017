#ifndef _FLASHMAC_H_

const int MAX_FRAME_LEN = 4;     			 //ÿ����spi���������ݳ���
const int MAX_DEFAULT_TEMPERATURE    = 50;   //����¶�
const int MIN_DEFAULT_TEMPERATURE    = -10;  //����¶�
const int DROP_NORMAL_TEMPERATURE    = 30;   //���µ�Ԥ�����¶�
const int UP_NORMAL_TEMPERATURE      = 0;    //���ȵ�Ԥ�����¶�


//���Ȳ�������
enum
{
	FAN_HOT  = 0 , //����
	FAN_COLD = 1 , //ɢ��
	FAN_NULL = 2 , //�޲���
	FAN_OTHER    
};

class CFlashMac
{
public:
	static CFlashMac* CreateInstance();	
	//void SetHardwareFlash();
	void SetHardwareFlash(bool isflash);//MOD 2013 0808 1640
	bool GetHardwareFlash();
	void WriteAndRead();
	void SetWriteData(Byte* pTx);
	void ResolveReadData(Byte* pTx);
	void SetMaxMinTemperature(int iMaxTpt,int iMinTpt);
	void SendRecordBoardMsg(Byte ucType);

	
	void FlashHeartBeat();					   //�������������  ADD: 0604 1646
	void RecvFlashCan(SCanFrame sRecvCanTmp) ; //������CAN���ݽ��մ����� ADD: 2013 0712 1530

	void FlashCfgSet(); 				       //�������������ú��� ADD: 0604 17 58
	void FlashCfgGet(); 					   //���������û�ȡ���� ADD:0605 08 47

	void FlashForceStart(Byte ucType);  	   // ������ǿ�ƻ���   ADD:0605 08 49
	void FlashForceEnd();   				   //��ɽ���˳�ǿ�ƻ���  ADD:0605 08 50
	void FlashGetVer();

	bool m_bGetHardwareFlash;  //��ȡ Ӳ������
	bool m_bGetColdFan;        //��ȡ ɢ�ȷ���
	bool m_bGetHotFan;         //��ȡ ���ȷ���

	bool m_bGetForDoor;        //ǰ�Ŵ�
	bool m_bGetAfterDoor;      //���Ŵ�

	bool m_bRecordSts;      //��һ�μ�¼������ͨ��״̬

	bool m_bPowerType;         //�������� true:������ false:̫����
	int  m_iVoltage;           //��ѹ
	int  m_iTemperature;       //�¶�

	/****	���û���������������    ********/
	Byte m_ucSetDutyCycle;  //ռ�ձ�
	Byte m_ucSetFlashRate;  //����Ƶ��
	Byte m_ucSetSyType;   //����ͬ����ʽ
	Byte m_ucSetFlashStatus ; //��������
		
	
	/****	��ȡ����������������	********/
	Byte m_ucGetDutyCycle;	//ռ�ձ�
	Byte m_ucGetFlashRate;	//����Ƶ��
	Byte m_ucGetSyType;   //����ͬ����ʽ
	Byte m_ucFlashStatus ; //��������
	Byte m_ucFlashVer[5] ; //�������汾

private:
	CFlashMac();
	~CFlashMac();

	Byte m_ucErrCheckCnt;   	//�������ܵ�У�����Ĵ���
	Byte m_ucRightCnt;      	//�������ܵ���ȷ���ݵĴ���

	bool m_bSetHardwareFlash;  //���� Ӳ������
	Byte m_ucSetFanSts;        //����״̬
	Byte m_ucLastSetFanSts;    //�����ϴε�״̬

	Byte  m_ucSetColdCnt;        //���� ɢ�ȷ��ȵĴ��� ����
	Byte  m_ucSetHotCnt;         //���� ���ȷ��ȵĴ��� ����
	Byte  m_ucSetNullFanCnt;     //���Ȳ�������

	int  m_iDevFd;             //�豸fd
	int  m_iMaxTemperature;    //����¶�
	int  m_iMinTemperature;    //����¶�

	STscConfig* m_pTscCfg;
};

#endif  //FlashMac
