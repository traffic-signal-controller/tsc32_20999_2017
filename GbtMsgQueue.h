#ifndef _GBTMSGQUEUE_H_
#define _GBTMSGQUEUE_H_

#include "ace/Message_Queue.h"
#include "ace/Synch.h"
#include "ace/SOCK_Dgram.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ComDef.h"
#include "ComStruct.h"

/*****************GBTЭ������־���ͺ�����Э���־�����Ǹ�ö��********************/
enum 
{
	//����������ض���
	OBJECT_DEV_PARA     = 0x81,  //�����豸ʶ�����
	OBJECT_MAX_MODULE         ,  //����ģ����������
	OBJECT_COM_SYN_TIME       ,  //����ͬ��ʱ��
	OBJECT_SYN_FLAG           ,  //����ͬ����־
	OBJECT_MODULE_TABLE       ,  //ģ���

	//����ʱ�����
	OBJECT_UTC_TIME     = 0x86,  //����ʱ��
	OBJECT_STANDAR_TIMEZONE   ,  //��׼ʱ��
	OBJECT_LOCAL_TIME         ,  //����ʱ��
	OBJECT_MAX_TIMEGROUP      ,  //ʱ�����ȱ��������
	OBJECT_MAX_SCHEDULE       ,  //ʱ�α��������
	OBJECT_MAX_SUBSCHEDULE    ,  //ʱ�α�����ʱ����
	OBJECT_ACTIVESCHEDULE_NO  ,  //�ʱ�α�ı��
	OBJECT_TIMEGROUP_TABLE    ,  //ʱ�����ȱ�
	OBJECT_SCHEDULE_TABLE     ,  //ʱ�α�

	//�ϱ�����
	OBJECT_MAX_EVENTTYPE    = 0x8F,  //�¼����ͱ��������
	OBJECT_MAX_EVENTLOG           ,  //�¼���־����������
	OBJECT_EVENTTYPE_TABLE        ,  //�¼����ͱ����
	OBJECT_EVENTLOG_TEBLE         ,  //�¼���־�����

	//��λ����
	OBJECT_MAX_PHASE      = 0x93  ,  //��λ��������� 
	OBJECT_MAX_PHASESTS           ,  //��λ״̬���������
	OBJECT_PHASE_TABLE            ,  //��λ������
	OBJECT_PHASESTATUS_TABLE      ,  //��λ���״̬��
	OBJECT_PHASECONFLICT_TABLE    ,  //��λ��ͻ��

	//���������
	OBJECT_MAX_DETECTOR    = 0x98,  //����������������
	OBJECT_MAX_DETECTORSTS       ,  //�����״̬����������
	OBJECT_DETECTORDATA_ID       ,  //���������ˮ��
	OBJECT_DATA_CYCLE            ,  //���ݲɼ�����
	OBJECT_ACTIVEDETECTOR_NUM    ,  //����������
	OBJECT_PULSEDATA_ID          ,  //����������ˮ��
	OBJECT_PULSEDATA_GETCYCLE    ,  //�������ݲɼ�����
	OBJECT_DETECTORDEF_TABLE     ,  //������������������
	OBJECT_DETECTORSTS_TABLE     ,  //�����״̬��
	OBJECT_DETECTORDATA_TABLE    ,  //��ͨ������ݱ�
	OBJECT_DETECTORWARN_TABLE    ,  //������������������

	//��Ԫ����
	OBJECT_START_FLASH  = 0xA3   ,  //����ʱ���������ʱ��
	OBJECT_START_ALLRED          ,  //����ʱ��ȫ�����ʱ��
	OBJECT_CURTSC_CTRL           ,  //��ǰ���źŻ�����״̬
	OBJECT_CURTSC_FLASHCTRL      ,  //��ǰ���������ģʽ
	OBJECT_TSC_WARN2             ,  //�źŻ�����2
	OBJECT_TSC_WARN1             ,  //�źŻ�����1
	OBJECT_TSC_WARN_SUMMARY      ,  //�źŻ�����ժҪ
	OBJECT_ALLOW_FUN             ,  //����Զ�̿���ʵ�弤���źŻ���ĳЩ����
	OBJECT_FLASH_FRE             ,  //����Ƶ��
	OBJECT_SHINE_STARTTIME       ,  //�Զȿ��ƿ���ʱ��
	OBJECT_SHINE_DOWM_TIME       ,  //�Զȿ��ƹر�ʱ��

	//�ƿض˿ڲ���
	OBJECT_MAX_CHANNEL     = 0xAE,  //�źŻ�֧�ֵ����ͨ������
	OBJECT_MAX_CHANNELSTS        ,  //ͨ��״̬����
	OBJECT_CHANNEL_TABLE         ,  //ͨ��������
	OBJECT_CHANNELSTATUS_TABLE   ,  //ͨ�����״̬��

	//���Ʋ���
	OBJECT_TIMEPATTERN_NUM = 0xB2,  //��ʱ������
	OBJECT_MAX_SCHEDULETIME      ,  //���׶���ʱ����
	OBJECT_MAX_SUBSCHEDULETIME   ,  //���׶���
	OBJECT_SWITCH_MANUALCONTROL  ,  //�ֶ����Ʒ���
	OBJECT_SWITCH_SYSTEMCONTROL  ,  //ϵͳ���Ʒ���
	OBJECT_SWITCH_CONTROL        ,  //���Ʒ�ʽ
	OBJECT_COMCYCLETIME          ,  //��������ʱ��
	OBJECT_ADJUST_PHASEGAP       ,  //Э����λ��
	OBJECT_SWITCH_STAGE          ,  //�׶�״̬
	OBJECT_GOSTEP                ,  //����ָ��
	OBJECT_DEMOTION_MODE         ,  //����ģʽ
	OBJECT_DEMOTION_PATTERN      ,  //������׼������
	OBJECT_CURPATTERN_SCHTIMES   ,  //��ǰ�������׶�ʱ��
	OBJECT_CURPATTERN_GREENTIMES ,  //��ǰ�������ؼ���λ�̵�ʱ��
	OBJECT_TIMEPATTERN_TABLE     ,  //��ʱ������
	OBJECT_SCHEDULETIME_TABLE    ,  //�׶���ʱ��

	//���ر�־����
	OBJECT_DOWNLOAD_FLAG   = 0xC2,  //��λ�����ز����Ŀ�ʼ������־

	//������������
	OBJECT_CONTROLTSC_PARA = 0xC3,  //��������ѡ�����
	OBJECT_TSCADDRESS            ,  //�źŻ�����ַ
	OBJECT_CROSSNUM              ,  //·������

	//������λ����
	OBJECT_MAX_OVERLAPPHASE = 0xC6,  //������λ���������
	OBJECT_MAX_OVERLAPPHASESTS    ,  //������λ״̬���������
	OBJECT_OVERLAPPHASE_TABLE     ,  //������λ��
	OBJECT_OVERLAPPHASE_STATUS    ,  //������λ״̬��

	//��������־
         OBJECT_WATCH_PARA      				 = 0xf5,   //������Ͳ��� �¶� ��ѹ ��
	OBJECT_IP              						 = 0xf6,   //�źŻ���ip
	OBJECT_SET_REPORTSELF  				 = 0xf7,   //���������ϱ�
	OBJECT_EXT_TSC_STATUS   				 = 0xf8,   //״̬���ͱ����
	OBJECT_MODULE_STATUS  			          = 0xf9 ,   //ģ��״̬
	OBJECT_CNTDOWN_DEV     				 = 0xf1,   //����ʱ�豸��    ADD:2013071 1034
	OBJECT_PHASETODIRECT				          = 0xfa,   //��λ�뷽���Ӧ��
	OBJECT_ADAPTPARA       					 = 0xfb,   //����Ӧ����ֵ
	OBJECT_DET_EXTENTED     				 = 0xfc,   //�������չ��
	OBJECT_ADAPT_STAGE      				 = 0xfd,
	OBJECT_CONNECT_DEV     				 = 0xf0,   //����豸����һ���ֽڱ�ʾ����ʱ�豸����ȡֵΪ0-32��0��ʾû�е���ʱ�豸���ڶ����ֽڱ�ʾ�ɱ��־�豸����ȡֵΪ0-16��0��ʾû�пɱ��־���ʾ��Ӽ��������ȡֵΪ0-48��0��ʾû����Ӽ����
	//OBJECT_CNTDOWN_STATS  			 = 0xf2,   	//����ʱ״̬��
	OBJECT_COMMAND_SIGNAL  			          = 0xf2 ,    //��λ��ָ�������λ�׶��л��ͷ���
	OBJECT_CHANNEL_CHK    			          = 0xff,   //ͨ�����ݼ�����ñ� ADD?20130801 1121
	OBJECT_YWFLASH_CFG   				          = 0xe1,   //����������
	OBJECT_DET_EXTCFG       					 = 0xe2,   //�������չ����
	OBJECT_LAMPBOARD_CFG    				 = 0xe3,   //�ƿذ���ݼ��ͺ��̳�ͻ�������
	OBJECT_PSCBTN_NUM 	    				 = 0xee,    //ģ��8λ���˰�ť���� ADD:2013 0829 1540
	OBJECT_TMPPATTERN_CFG   				 = 0xef,    //12������ʱ��Ϸ�����Ĭ��60��
	OBJECT_SYSFUNC_CFG    			          = 0xe4,      //ϵͳ������������
	OBJECT_SENDCLIENT_CNTDOWN			 = 0xe6 ,
	OBJECT_POWERBOARD_CFG    			 = 0xe7,   //��Դ������
	OBJECT_GSM_CFG           					 = 0xe8 ,  //GSM����
	OBJECT_BUTTONPHASE_CFG 			          = 0xe9    //ģ�����߰�����ť
};
/*****************GBTЭ������־���ͺ�����Э���־�����Ǹ�ö��********************/

class CGbtMsgQueue
{
public:
	static CGbtMsgQueue* CreateInstance();
	static void* RunGbtRecv(void* arg);	
	int  SendGbtMsg(SThreadMsg* pMsg,int iLen);
	void DealData();	
#ifdef GBT_TCP
	SGbtDealData* GetGbtDealDataPoint();
	Byte GetDealDataIndex(bool bReportSelf , ACE_SOCK_Stream& sockStreamRemote);
#else
	Byte GetDealDataIndex(bool bReportSelf ,ACE_INET_Addr& addrRemote);
#endif
	bool SendTscCommand(Byte ucObjType,Byte ucValue);
	void SetCmuAndCtrl(Byte* pBuf,int& iRecvIndex);
	void SetCmuAndCtrl(Byte* pBuf,int& iRecvIndex , Byte ucSubId);
	void GetCmuAndCtrl(Byte* pBuf,int& iSendIndex);
	void GetCmuAndCtrl(Byte* pBuf,int& iSendIndex , Byte ucSubId);
	void ReworkNetPara(Byte* cIp ,Byte* cMask,Byte* cGateWay);
	void GetNetPara(Byte* pIp , Byte* pMask , Byte* pGateway);
	void InterceptStr(char* pBuf, char* pstr , Byte* pData , Byte ucCnt);
	void GetNetParaByAce(Byte* pip,char* phost_name);  //add 2014 03 20   lurenhua ͨ��ACE��ȡ��IP��ַ������ȡ�ö��
	
	int GetCtrlStatus(unsigned int uiWorkStatus,unsigned int uiCtrl);
	bool GetSystemShellRst(const char* shellcmd , char * cshellrst ,Byte datasize); //ADD:201411041430
	
	void GotoSendToHost(Byte ucDealDataIndex);
public:
	ACE_INET_Addr m_addrLocal;
#ifdef GBT_TCP
	ACE_SOCK_Acceptor m_acceptor;   //tcp
#else
	ACE_SOCK_Dgram m_sockLocal;  //udp
#endif
	int iPort   ;               //MOD:201309250830	
	SGbtDealData  m_sGbtDealData[MAX_CLIENT_NUM];
private:
	CGbtMsgQueue();
	~CGbtMsgQueue();

	int  CheckMsg(Byte ucDealDataIndex,Uint iBufLen,Byte* pBuf);
	void FirstRecv(Byte ucDealDataIndex,Uint iBufLen,Byte* pBuf);
	Byte GetSendOperateType(Byte ucRecvOptType);
	void DealRecvBuf(Byte ucDealDataIndex);
 	void CleanDealData(Byte ucDealDataIndex);
 	void SendToHost(Byte ucDealDataIndex);
  	bool IsSingleObject(Byte ucObjectFlag);
  	bool IsGetTscStatusObject(Byte ucObjectFlag);
	bool IsSendTscCommand(Byte ucObjectFlag);
	bool IsOtherObject(Byte ucObjectFlag);
	bool IsExtendObject(Byte ucObjectFlag);
	//bool IsDoubleIndexObject(Byte ucObjectFlag);
 	void PackTscStatus(Byte ucDealDataIndex,void* pValue);
	void PackOtherObject(Byte ucDealDataIndex);
	void PackExtendObject(Byte ucDealDataIndex);
	Byte ToObjectCurTscCtrl(unsigned int uiCtrl);
 	Byte ToObjectControlSwitch(unsigned int uiWorkSts,unsigned int uiCtrl);
 	
 	void SelfReport(unsigned int uiDataLen,Byte* pDataBuf);
	int GbtCtrl2TscCtrl(Byte ucctrl);
	int GetSysCtrlStatus(unsigned int uiWorkStatus,unsigned int uiCtrl);
	int GetManualCtrlStatus(unsigned int uiWorkStatus,unsigned int uiCtrl);

	void TscCopyFile(char* fpSrc, char* fpDest);
	void UpdateNetPara(Byte* pIp);  //add: 2014 03 20   lurenhua zlg m3352���İ�IP��ַ�޸ĺ���
	
	void GetWatchPara(Byte* pBuf,int *iSendIndex);	
	void GetModuleStatus(Byte* pBuf,int *iSendIndex ,Byte subId,Byte ucQueryType,Byte ucBdindex);
	void PackTscExStatus(Byte ucDealDataIndex,void* pValue);
	void GetFlashCfg(Byte* pBuf,int *iSendIndex) ;  //ADD: 2013 0808 10 30
	void SetFlashCtrl(Byte* pBuf,int& iRecvIndex); //ADD 2013 0808 1430
	void  GetPowerCfg(Byte* pBuf,int *iSendIndex ,Byte ucQueryType); //ADD:201404021520	
	void  SetPowerCfg(Byte* pBuf,int& iRecvIndex) ;//ADD:201404021520

	void GetDetCfg(Byte* pBuf,Byte ucBdIndex,Byte ucQueryType,int *iSendIndex) ; //ADD 20130821 1130
	void SetDetCfg(Byte* pBuf,int& iRecvIndex) ; //ADD 2013 0821 1624
	void SetLampBdtCfg(Byte* pBuf,int& iRecvIndex);  //ADD 2013 0822 1606
	void SetPscNum(Byte* pBuf,int& iRecvIndex);  //ADD 20130829 1600
	void SetTmpPattern(Byte* pBuf,int& iRecvIndex) ; //ADD 20131016 1700
	void SetSysFunc(Byte* pBuf,int& iRecvIndex); //ADD 20131019 1400
	void PrintIpList();

	void GotoMsgError(Byte ucDealDataIndex,Byte ucErrorSts,Byte ucErrorIdx);
	void GotoDealRecvbuf(Byte ucDealDataIndex);
	void SetSmsFunc(Byte* pBuf,int& iRecvIndex ,int iRecvBufLen); //ADD 201406041030
	void SetCommandSignal(Byte* pBuf,int& iRecvIndex) ;  //ADD 201409231002
	void SetButtonPhase(Byte* pBuf,int& iRecvIndex);        //ADD 201410181052
	
	void GetSysFuncCfg(Byte* pBuf,Byte ucQueryType,int *iSendIndex) ; //ADD 201601141650

	//Byte m_ucAddrNum;
	ACE_Message_Queue<ACE_MT_SYNCH>* m_pMsgQue;
      
	ACE_Thread_Mutex  m_sMutex;
	//ACE_INET_Addr m_AddrArr[MAX_CLIENT_NUM];

	int m_iManualCtrlNo;
	int m_iSystemCtrlNo;
};

#endif  //_GBTMSGQUEUE_H_

