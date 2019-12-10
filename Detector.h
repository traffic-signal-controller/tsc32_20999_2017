#ifndef _DETECTOR_H_
#define _DETECTOR_H_

#include "ComStruct.h"
#include "ManaKernel.h"
#include "ComDef.h"

/*
	�������������״̬
*/
enum
{
	DET_HEAD_VEHSTS     = 0x02 , //���ذ�������������16��ͨ���ĳ������״̬
	DET_HEAD_SPEED0104  = 0x03 , //���ذ�������������1-4�������Ȧ��ƽ���ٶ�
	DET_HEAD_SPEED0508  = 0x04 , //���ذ�������������5-8�������Ȧ��ƽ���ٶ�
	
	DET_HEAD_SEN0108_GET = 0x05 , //���ذ�������������1-8ͨ��������ļ��������
	DET_HEAD_SEN0916_GET = 0x06 , //���ذ�������������9-16ͨ��������ļ��������
	
	DET_HEAD_COIL0104_GET = 0x07 , //���ذ�������������1-4�������Ȧ�İ����
	DET_HEAD_COIL0508_GET = 0x08 , //���ذ�������������5-8�������Ȧ�İ����
	
	DET_HEAD_DISTAN0104_GET = 0x09 , //���ذ�������������1-4�������Ȧ����Ȧ����
	DET_HEAD_DISTAN0508_GET = 0x0a , //���ذ�������������5-8�������Ȧ����Ȧ����

	DET_HEAD_SEN0108_SET = 0x0b ,  //���ذ巢��1 - 8ͨ�����������ø������
	DET_HEAD_SEN0916_SET = 0x0c ,  //���ذ巢��9 - 16ͨ�����������ø������

	DET_HEAD_COIL0104_SET = 0x0d , //���ذ巢��1 �C 4�������Ȧ�İ����ݸ��������
	DET_HEAD_COIL0508_SET = 0x0e , //���ذ巢��5 �C 8�������Ȧ�İ����ݸ��������
	
	DET_HEAD_COILALLOW_SET= 0x1b,  //���ذ������Ƿ�������Ȧ�󶨡�  ADD 2013 0816 1420

	DET_HEAD_DISTAN0104_SET = 0x0f , //���ذ巢��1 �C 4�������Ȧ����Ȧ������������
	DET_HEAD_DISTAN0508_SET = 0x10 , //���ذ巢��5 �C 8�������Ȧ����Ȧ������������

	DET_HEAD_STS = 0x11 , //���ذ�������������16��ͨ���ļ��������״̬   //������������·����·�ȵ���Ϣ	
	
	DET_HEAD_SENDATA0107_GET = 0x12 ,         //��ʾ���ذ�������������1-7������������ֵ   //ADD: 2013 08 05 1600 
	DET_HEAD_SENDATA0814_GET = 0x13 ,         //��ʾ���ذ�������������8-14������������ֵ
	DET_HEAD_SENDATA1516_GET = 0x14 ,         //��ʾ���ذ�������������15-16������������ֵ

	DET_HEAD_SENDATA0107_SET = 0x15 ,         //��ʾ���ذ����ü��������1-7������������ֵ
	DET_HEAD_SENDATA0814_SET = 0x16 ,         //��ʾ���ذ����ü��������8-14������������ֵ
	DET_HEAD_SENDATA1516_SET = 0x17 ,         //��ʾ���ذ����ü��������15-16������������ֵ

	DET_HEAD_FRENCY_GET = 0x18 , 		    //��ʾ���ذ�������������16��ͨ������Ƶ������
	DET_HEAD_FRENCY_SET = 0x19 , 		      //��ʾ���ذ巢�͸������16��ͨ������Ƶ�����á�

	DET_HEAD_WORK_SET = 0x1a , 		    //��ʾ���ذ����ü����������ʽ��
	DET_HEAD_VER      =0xff  ,             		   //��ʾ������汾 ADD:20141201
	DET_HEAD_ID       =0x0                  		  //��ʾ�������ID��
	  
};

class CDetector
{
public:
	static CDetector* CreateInstance();
	
	void SelectDetectorBoardCfg(int *pDetCfg);
	bool CheckDetector();	
	bool SelectBrekonCardStatus(Byte ucBoardIndex, Byte ucAddress);
	void SearchAllStatus(bool chkcar,bool chkdecstatus);
	void SearchSpeed(Byte ucBoardIndex, Byte ucAddress, Byte ucRecAddress);
	void GetOccupy();
	int GetActiveDetSum();
	void GetDetStatus(SDetectorSts* pDetStatus);
	void GetDetData(SDetectorData* pDetData);
	void GetDetAlarm(SDetectorAlarm* pDetAlarm);
	bool IsDetError();
	int GetDetBoardType();
	bool HaveDetBoard();
	void IsVehileHaveCar();
	bool IsHaveCarPhaseGrp(Uint uiPhase,Byte& ucPhaseIndex , SPhaseDetector* pPhaseDet);

	//���ذ�����ȫ�����������16��ͨ���ĳ������״̬
	void GetAllVehSts(Byte QueryType,Byte ucBdindex);  //ADD: 2013 0710 10 54

	//���ذ����󵥸����������16��ͨ���ĳ������״̬
	void GetVehSts(Byte ucBoardAddr,Byte QueryType);   //ADD: 2013 0710 10 54

	//���ذ����������м�����Ĺ���״̬ //ADD: 2013 1114 0930
	void GetAllWorkSts(); 
	void GetAllDecVer(); //��ѯ���г����ӿڰ�������ADD 20150202 1427

	void RecvDetCan(Byte ucBoardAddr,SCanFrame sRecvCanTmp);// �����CAN���߽��ջ���������   //ADD: 2013 0710 10 54
	Byte GetDecBoardIndex(Byte DecBoardHexAddr) ;  //���ؼ������ӿڰ��ַ����ADD: 20150202
/*
#ifndef WINDOWS
	void GetHaveCarTime(time_t* pTime);
#endif
*/
	//void SendDetLink(Byte ucBoardIndex);
	Byte GetDecAddr(Byte ucBoardIndex);
	void SendDecIsLink(Byte ucBoardIndex,Byte IsAllowLink);
	void GetDecVars(Byte ucBoardIndex,Byte GetType);
	void SendDecWorkType(Byte ucBoardIndex);
	void SendDecFrency(Byte ucBoardIndex);
	void SendDecSenData(Byte ucBoardIndex,Byte ucSetType) ; //ADD  20130816 1600
	void SendDetLink(Byte ucBoardIndex,Byte SetType);
	void SearchDetLink(Byte ucBoardIndex);

	//void SendRoadDistance(Byte ucBoardIndex);
	void SendRoadDistance(Byte ucBoardIndex,Byte SetType);
	void SearchRoadDistance(Byte ucBoardIndex);

	void SearchRoadSpeed(Byte ucBoardIndex);

	//void SendDelicacy(Byte ucBoardIndex);
	void SendDelicacy(Byte ucBoardIndex,Byte SetType);
	void SearchDelicacy(Byte ucBoardIndex);

	void GetAdaptInfo(int* pDetTimeLen , int* pTotalStat);
	void SetStatCycle(Byte ucCycle);

	void SendRecordBoardMsg(Byte ucDetIndex,Byte ucType);
	void PrintDetInfo(char* pFileName,int iFileLine,Byte ucBoardIndex,char* sErrSrc,int iPrintCnt,Byte* ucRecvBuf);
public:
	
	Byte m_ucActiveBoard1; 	/* 1  - 16 ��ļ������*/
	Byte m_ucActiveBoard2;   //17 - 32 ��ļ������
	Byte m_ucActiveBoard3;	//33 - 48 ��ļ������
	Byte m_ucActiveBoard4 ; //65-96	��ļ������
	bool m_bErrFlag[MAX_DETECTOR];       //�����־

	bool m_bRecordSts[MAX_DET_BOARD];    //��һ�μ�¼�����ͨ��״̬

	Byte m_ucDetError[MAX_DETECTOR];     //����״̬      32 - 47:��1������      48 - 63:��2������
	Byte m_ucLastDetError[MAX_DETECTOR]; //�ϴι���״̬   32 - 47:��1������      48 - 63:��2������
	Byte m_ucDetErrTime[MAX_DETECTOR]; //���ϴ���      32 - 47:��1������      48 - 63:��2������

	int m_iAdapDetTimeLen[MAX_DETECTOR];    //�г�ʱ���ͳ��  100ms/��λ ��������Ӧ����
	int m_iAdapTotalStat[MAX_DETECTOR];     //����ͳ��   /�� ��������Ӧ����

	Byte m_ucRoadSpeed[MAX_DET_BOARD][8];     //������ƽ���ٶ� 

	Byte m_ucSetRoadDis[MAX_DET_BOARD][8];    //���õĳ�������
	Byte m_ucGetRoadDis[MAX_DET_BOARD][8];    //��ȡ���ĳ�������

	Byte m_ucSetDetDelicacy[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD];  //���ü�������������
	Byte m_ucGetDetDelicacy[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD];  //��ȡ���ļ�������������

	Byte m_sSetLookLink[MAX_DET_BOARD][8];  //���õ���Ȧ��Ӧ��ϵ
	Byte m_sGetLookLink[MAX_DET_BOARD][8];  //��ȡ������Ȧ��Ӧ��ϵ
	
	Byte m_ucSetFrency[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD] ;
	Byte m_ucGetFrency[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD] ;
	Byte m_iChkType ;
	Byte m_ucSetSensibility[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD] ; //ADD 2013 0816 1530
	Byte m_ucGetSensibility[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD] ; //ADD 2013 0816 1530
	Byte m_ucDecBoardVer[MAX_DET_BOARD][5]; //ADD:20141201  ������忨 �ĳ���汾
	Byte m_ucDecBoardId[MAX_DET_BOARD][4]; //ADD 20150112 ������忨ID
	VehicleStat m_ucDecCarsAnaly[MAX_DETECTOR];
	
private:
	CDetector();
	~CDetector();

	Byte m_ucNoCnt[MAX_DET_BOARD];          //����û�н������ݵĴ���
	Byte m_ucErrAddrCnt[MAX_DET_BOARD];    //�������ܵ������ַ�Ĵ���
	Byte m_ucErrCheckCnt[MAX_DET_BOARD];   //�������ܵ�У�����Ĵ���
	Byte m_ucRightCnt[MAX_DET_BOARD];      //�������ܵ���ȷ���ݵĴ���

	int m_iDevFd;
	int m_iTotalDistance;             //ͳ�Ƽ�� 	
	
	int m_iDetCfg[MAX_DET_BOARD];      // 0-������ 1-��һ������� 17-��17�������
	int m_iBoardErr[MAX_DET_BOARD];    //true:�� false:����
	int m_iLastDetSts[MAX_DETECTOR];  //�ϴγ���״̬
	int m_iDetStatus[MAX_DETECTOR];/* //1:�г� 0:�޳�  0 - 15:��1�������ӿڰ� 16 - 31:��2�������ӿڰ�*/
	int m_iDetTimeLen[MAX_DETECTOR];  //�г�ʱ���ͳ��  100ms/��λ
	int m_iDetOccupy[MAX_DETECTOR];   //ռ����
	Byte m_ucTotalStat[MAX_DETECTOR];  //����ͳ��   /��
	Byte m_iDetSpeedAvg[MAX_DETECTOR]; //�����ٶ�
	Byte m_ucDetSts[MAX_DET_BOARD][MAX_DETECTOR_PER_BOARD]; //�г��޳���־ ADD: 2013 0710 1050
	//int m_iLastDetTimeLen[MAX_DETECTOR];  //�ϸ�ͳ�����ڵ��г�ʱ���ͳ��  100ms/��λ
	
	STscConfig* m_pTscCfg;

	ACE_Thread_Mutex  m_sMutex;
};

#endif //_DETECTOR_H_
