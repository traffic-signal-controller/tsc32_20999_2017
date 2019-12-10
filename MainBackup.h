#ifndef MAINBACKUP_H_
#define MAINBACKUP_H_

#include "ComDef.h"
#include "SerialCtrl.h"
#include "IoOperate.h"
#include "Cdkey.h"
#include "ComStruct.h"

enum
{
	MAINBACKUP_NONE 			= 	0 ,  		//������
	MAINBACKUP_READ_ID 		= 	1 ,  		//���İ��ȡ�豸ID����  ����
	MAINBACKUP_WRITE_ID		=	2,			//���İ�д���豸ID����
	MAINBACKUP_READ_MANUAL	=	3,			//���İ��ȡ�ֿ�״̬
	MAINBACKUP_READ_LED		=	4,			//���İ��ȡָʾ��״̬
	MAINBACKUP_WRITE_LED		=	5,			//���İ�д��ָʾ��״̬
	MAINBACKUP_HEART			=	6,			//������ÿ500ms����һ�Σ���Ϊ���İ���������
	MAINBACKUP_LAMP			=	7			//���İ��·��׶β��������ݵ�Ƭ���������64������������ʾ���ǲ�����ע����ϵ�ɫ���ݺͽ�����ɫ���ݣ�

};

enum
{
	MAINBACKUP_RECEVIE_NONE 			= 	0 ,  		//������
	MAINBACKUP_RECEVIE_ID 			= 	1 ,  		//���İ��ȡ�豸ID����  ����
	MAINBACKUP_RECEVIE_WRITE_ID_TF	=	2,			//���İ�д���豸ID����  �ɹ�ʧ��   8���ֽ�
	MAINBACKUP_RECEVIE_READ_MANUAL	=	3,			//���İ��ȡ�ֿ�״̬
	MAINBACKUP_RECEVIE_READ_LED		=	4,			//����ָʾ�Ƶ�״̬  1���ֽ�
	MAINBACKUP_RECEVIE_WRITE_LED_TF	=	5,			//ָʾ��״̬д��ɹ���ʧ�� ��1���ֽ�
	MAINBACKUP_RECEVIE_SETP			=	6,			//��Ƭ����ǰ������
	MAINBACKUP_RECEVIE_SETP_TABLE	=	7			//��Ƭ��ȷ���յ��Ľ׶β����� 13���ֽ�

};


enum
{
	MAINBACKUP_LED_MODE_GREEN	= 0,
	MAINBACKUP_LED_MODE_YELLOW	=1,
	MAINBACKUP_LED_MODE_RED		=2
	
};

enum 
{
	MAINBACKUP_TSCP_TSC = 0,
	MAINBACKUP_TSCP_PSC = 1
};

enum 
{
	MAINBACKUP_AUTO_SELF = 0,
	MAINBACKUP_AUTO_MANUAL = 1
};

enum
{
	MAINBACKUP_MANUAL_SELF 			= 0,	//��������
	MAINBACKUP_MANUAL_MANUAL 		= 1,	//�ֶ�����
	MAINBACKUP_MANUAL_NEXT_STEP		=3,		//��һ��
	MAINBACKUP_MANUAL_YELLOW_FLASH	=5,		//����
	MAINBACKUP_MANUAL_ALL_RED		=9,		//ȫ��
	MAINBACKUP_MANUAL_NEXT_PHASE	=17,	//��һ��λ
	MAINBACKUP_MANUAL_NEXT_DIREC	=33	//��һ����

};
#define MANUAL_TO_AUTO_TIME 10  //�ֿ�״̬����10���Ӻ󣬽��Լ��л�����������

/*
�ֿ�״̬
*/
enum
{
	MAC_CTRL_NOTHING    = 0x00 , //����ԭ������
	MAC_CTRL_ALLOFF     = 0x01 , //�ص�
	MAC_CTRL_ALLRED     = 0x02 , //ȫ��
	MAC_CTRL_FLASH      = 0x03 , //����
	MAC_CTRL_NEXT_PHASE = 0x04 , //��һ��λ
	MAC_CTRL_NEXT_DIR   = 0x05 , //��һ���� 
	MAC_CTRL_NEXT_STEP  = 0x06 , //��һ��
	MAC_CTRL_OTHER      = 0x07 , //����
};

enum
{
	LAMP_OFF_ALL			=0,		//�ص�
	LAMP_RED			=1,		//���
	LAMP_YELLOW		=2,//�Ƶ�
	LAMP_GREEN			=3,//�̵�
	LAMP_RED_FLASH	=4,//����
	LAMP_YELLOW_FLASH	=5,//����
	LAMP_GREEN_FLASH	=6,//����
	LAMP_OTHER			=7//������Ԥ��
};

class MainBackup 
{
public:
	MainBackup();
	virtual ~MainBackup();
	static MainBackup* CreateInstance();
	void OpenDev();
	bool SendBackup(Byte *pByte ,int iSize);
	bool RecevieBackup(Byte *pByte ,int iSize);
	void OperateBackup(Byte *pbytes, Uint pLen,Byte *rsBytes, Uint rsLen);
	void OperateManual(Ushort mbs);
	void DoManual();
	static void* Recevie(void* arg);
	void HeartBeat();
	void ReadLED(Byte &status);
	void ReadID(Byte *pByte);
	void WriteIDTF(Byte *pByte);
	void WriteLEDTF(Byte &writeLED);
	void CurrentSetp(Byte &setp);
	void SetpTable(Byte *setpTable);
	void DoReadId();
	void DoWriteId();
	void DoReadLED();
	void DoWriteLED(Byte ucByte);
	void DoSendStep(STscRunData *m_pRunData);
	void SendOneStep(Uint i);
	void SendStep();

	
	void RecvMainBackCan(SCanFrame sRecvCanTmp); //ADD:20150310
	void GetMainBackVer(); //ADD:20150310	
	Byte m_ucMainBackVer[5]; // ���ݵ�Ƭ���汾ADD:20150310
	bool bSendStep;
private:
	
	ACE_Thread_Mutex  m_sMutex;
	int m_iSerial3fd;
	Byte m_ucLastManualSts;  //֮ǰ���ֶ�״̬
	STscRunData* pRunData;
	
};


#endif /* MAINBACKUP_H_ */

