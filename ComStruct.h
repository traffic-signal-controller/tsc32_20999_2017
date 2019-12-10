#ifndef _COMSTRUCT_H_
#define _COMSTRUCT_H_

#include "ComDef.h"
#include "ace/INET_Addr.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "ace/Synch.h"
#include "GbtDb.h"

#ifndef  ActiveTimer
typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> ActiveTimer;  //������ʱ����
#endif

#ifndef TscAceDebug
#define TscAceDebug(X) \
/*	do { \
int __ace_error = ACE_Log_Msg::last_error_adapter (); \
ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
ace___->log X; \
} while (0)
*/
#endif
#if defined(__GNUC__) 
#define PACKED  __attribute__((packed)) 
#else 
#pragma pack(1) 
#define PACKED 
#endif


/*
	GBT��Ϣ�ṹ��
*/
struct SGbtFrame
{
	int iIndex;              //��Ϣ�ֽ�����
	int iBufLen;             //��Ϣ����
	Byte ucBuf[MAX_BUF_LEN]; //��Ϣ��������
}
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
;

/*
	GBT��Ϣ����ṹ��
*/
struct SGbtDealData
{	
	int            iObjNum;        //������
	bool           bIsDeal;        //��Ϣ�Ƿ��Ѿ�����
	bool           bReportSelf;    //�Ƿ��Զ��ϱ�
#ifdef GBT_TCP
	ACE_SOCK_Stream SockStreamClient;  //TCP�ͻ��˵�ַ
#else
	ACE_INET_Addr  AddrClient;     //UDP�ͻ��˵�ַ
#endif
	SGbtFrame      sRecvFrame;     //���յ�GBT��Ϣ�ṹ��
	SGbtFrame      sSendFrame;     //�����͵�GBT��Ϣ�ṹ��
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
;

/*
*	�����̼߳���Ϣ���н�������Ϣ���ݽṹ
*/
struct SThreadMsg
{
	Ulong ulType;        //��Ϣ����
	Ulong ulThreadPid;   //���͸���Ϣ�Ľ���
	Byte  ucMsgOpt;      //��Ϣ�ĸ��Ӳ���
	Uint  uiMsgDataLen;  //��Ϣ���ݿ鳤��
	void* pDataBuf;      //���ݿ�洢ָ��
}PACKED;

/*
	ʱ�����Ȳ�����
*/
struct STimeGroup
{
	Byte   ucId;                            //���ȼƻ���
	Ushort usMonth;                 //������        b0-b11   ÿλ1��     1����ִ��
	Byte   ucDayWithWeek;     //������(����)  b0-b6    ÿλÿ��1��  b0���� b1����1
	Ulong  uiDayWithMonth;  //������(����)  b0-b30   ÿλÿ��1��
	Byte   ucScheduleId;          //ʱ�α��      0��ʾ��Ч
}
PACKED;

/*
	ʱ�α������
*/
struct SSchedule 
{
	Byte ucId;              //ʱ�α��
	Byte ucEventId;         //ʱ���¼���
	Byte ucHour;            //����
	Byte ucMin;             //����
	Byte ucCtrl;            //���Ʒ�ʽ
	Byte ucTimePatternId;   //��ʱ������
	Byte ucAuxOut;          //�����������
	Byte ucSpecialOut;      //���⹦�����
}
PACKED
;

/*
	��ʱ��������
*/
struct STimePattern
{
	Byte ucId;
	Byte ucCycleLen;         //����ʱ��
	Byte ucPhaseOffset;      //��λ��
	Byte ucAdjustPhaseGap;   //Э����λ��
	Byte ucScheduleTimeId;   //��Ӧ�׶���ʱ���
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
	�׶���ʱ����
*/
struct SScheduleTime
{
	Byte   ucId;
	Byte   ucScheduleId;  //�׶κ�
	Uint32 usAllowPhase;  //������λ
	Byte   ucGreenTime;   //�׶��̵�ʱ��
	Byte   ucYellowTime;  //�׶λƵ�ʱ��
	Byte   ucRedTime;
	Byte   ucOption;      //�׶�ѡ��
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
	��λ����
*/
struct SPhase
{
	Byte ucId;               //��λ��(����)
	Byte ucPedestrianGreen;  //�����̵�����
	Byte ucPedestrianClear;  //�������
	Byte ucMinGreen;         //��С��
	Byte ucGreenDelayUnit;   //��λ�̵��ӳ�ʱ��
	Byte ucMaxGreen1;        //����̵�ʱ��1
	Byte ucMaxGreen2;        //����̵�ʱ��2
	Byte ucFixedGreen;       //������λ�̶��̵�ʱ��
	Byte ucGreenFlash;       //����ʱ��
	Byte ucType;             //��λ����
	Byte ucOption;           //��λѡ��
	Byte ucExtend;           //��չ�ֶ�
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
	������λ�����
*/
struct SOverlapPhase
{
	Byte ucId;
	Byte ucOperateType;             //��������
	Byte ucIncludePhaseLen;         //������λ����
	Byte ucIncludePhase[MAX_PHASE]; //������λ ÿ���ֽ�1����λ��
	Byte ucCorrectPhaseLen;         //������λ����
	Byte ucCorrectPhase[MAX_PHASE]; //������λ
	Byte ucTailGreen;               //β���̵�
	Byte ucTailYellow;   
	Byte ucTailRed;  
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
	ͨ������
*/
struct SChannel
{
	Byte ucId;
	Byte ucSourcePhase; //ͨ�������ź�Դ����λ��
	Byte ucFlashAuto;   //�Զ���������
	Byte ucType;        //����
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
*	��Ԫ����
*/
struct SUnit
{
	Byte ucStartFlashTime;     //����ʱ���������ʱ��
	Byte ucStartAllRedTime;    //����ʱ��ȫ�����ʱ��
	Byte ucCtrl;               //��ǰ���źŻ�����״̬
	Byte ucFlashCtrl;          //��ǰ���������ģʽ
	Byte ucWarn2;              //�źŻ�����
	Byte ucWarn1;              //�źŻ�����
	Byte ucWarnSmary;          //�źŻ�����ժҪ
	Byte ucAllowFun;           //����Զ�̿���ʵ�弤���źŻ���ĳЩ����
	Byte ucFlashFre;           //����Ƶ��
	Uint uiShineStartTime;     //�Զȿ��ƿ���ʱ��
	Uint uiShineDownTime;      //�Զȿ��ƹر�ʱ��
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
	ÿ·���ݼ���������  ADD:20130731 
*/
struct SChannelCheck
{
	Byte ucSubChannelId;		//ͨ������·��   Ŀǰ4�����ù�48·  1---48
	Byte ucIsCheck; 			//�Ƿ���Ҫ���е��ݼ�� 0-����Ҫ 1-��Ҫ
	
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
*��������λ�Ķ�Ӧ��ϵ
*/

struct SPhaseToDirec
{
	Byte ucId;     //�����
	Byte ucPhase;  //��λ��
	Byte ucOverlapPhase; //������λ��
	Byte ucRoadCnt;   //������
}
PACKED
;

/*
	�źŻ�����������Ϣ
*/
struct STscConfig 
{
	int                   iDetCfg[MAX_DET_BOARD];           //����������ʼ���� 0 1 17
	STimeGroup            sTimeGroup[MAX_TIMEGROUP];        //ʱ�����ȱ�
	SSchedule             sSchedule[MAX_SCHEDULE_PER_DAY];  //һ���ʱ�α�
	STimePattern          sTimePattern[MAX_TIMEPATTERN];    //��ʱ������
	SScheduleTime         sScheduleTime[MAX_SCHEDULETIME_TYPE][MAX_SON_SCHEDULE];  //�׶���ʱ��
	SPhase                sPhase[MAX_PHASE];                   //��λ��
	GBT_DB::Collision sPhaseConflict[MAX_CONFLICT_PHASE];  //��ͻ��λ��
	SOverlapPhase         sOverlapPhase[MAX_OVERLAP_PHASE];    //������λ��
	SChannel              sChannel[MAX_CHANNEL];               //ͨ����
	SUnit                 sUnit;                               //��Ԫ����
	GBT_DB::Detector  sDetector[MAX_DETECTOR];             //���������
	GBT_DB::SpecFun   sSpecFun[FUN_COUNT];                 //���ܶ����
	GBT_DB::DetExtend sDetExtend[MAX_DETECTOR];            //�������չ����
	//ACE_Thread_Mutex  mMutex;  
	SChannelCheck         sChannelChk[MAX_LAMP];         //ͨ�����ݼ������   
	SPhaseToDirec         sPhaseToDirec[MAX_DREC] ;     //��������λ����   
	Byte DegradeMode ; 									//����ģʽ 201310191100
	Byte DegradePattern ; 						  	    //������׼���� 201310191400
	GBT_DB::CntDownDev sCntDownDev[MAX_CNTDOWNDEV];     //����ʱ������
	Byte cGbType ;             // ����Э������    20170501 1120
	//Uint DirecButtonPhase[4] ;    //���򰴼���λ���ñ�������
 }
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
*ÿ����������صƾ������Ϣ
*/
struct SStepInfo
{
	Byte ucLampOn[MAX_LAMP];      //�����ƾ��������0����1���� ֱ���������豸ryg��Ӧ
	Byte ucLampFlash[MAX_LAMP];   //�����ƾ��������
	Byte ucStepLen;               //ÿ��������������ʱ��
	Byte ucPhaseColor[MAX_PHASE]; //ѡ����λ�ڴ˲�����ʾ����ɫ
	Byte ucOverlapPhaseColor[MAX_OVERLAP_PHASE];  //ѡ�и�����λ��λ�ڴ˲�����ʾ����ɫ
	Uint uiAllowPhase;    //������λ 1λ1����λ
	Uint uiOverlapPhase;  //������λ 1λ1����λ
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
*ÿ����λ�Ĳ�����Ϣ
*/
struct SPhaseStep
{
	bool bIsAllowPhase;   //1��������λ ������λ
	Byte ucPhaseId;       //��λ�ţ�������λ���id
	Byte ucStepTime[4];   //�� ���� �� �� �ĳ���ʱ��  0:Ϊû�иò�
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
	�źŻ���̬���ݶ���
*/
struct STscRunData 
{
	bool              bStartFlash;      //����ʱ�Ļ���  true:��  false:����
	Byte              ucWorkMode;       //�źŻ�����ģʽ
	Uint			  uiWorkStatus;     //����״̬
	Uint              uiOldWorkStatus;  //�ϴεĹ���״̬
	Uint              uiCtrl;           //���Ʒ�ʽ
	Uint              uiOldCtrl;        //�ϴεĿ��Ʒ�ʽ
	Uint              uiUtcsHeartBeat;  //utcs���� ����
	Byte              ucScheduleId;     //ʱ�α��
	Uint              uiScheduleCtrl;   //ʱ�α���Ŀ��Ʒ�ʽ
	Byte              ucTimePatternId;  //��ʱ������
	Byte              ucScheduleTimeId; //�׶���ʱ���
	Byte			  ucStepNo;         //��ǰ������
	Byte              ucLockPhase;      //����������λ��
	bool              bOldLock;         //��¼�ϴ��Ƿ�Ϊ������λ
	Byte              ucStepNum;        //��ǰ���ڵĲ�������
	Byte              ucStepTime;       //��ǰ��������
	Byte              ucElapseTime;     //��ǰ�������е�ʱ��
	//Byte              ucStageElapseTime;//��ǰ�׶������е�ʱ��
	Byte              ucRunTime;        //��ǰ���������е�ʱ��
	Byte              ucCycle;          //����ʱ��
	SScheduleTime     sScheduleTime[MAX_SON_SCHEDULE];        //��ǰ�����ӽ׶εļ���
	Byte              ucStageIncludeSteps[MAX_SON_SCHEDULE];  //��ǰ�����׶εĸ��԰����Ĳ�����
	Byte              ucStageCount;     //��ǰ�׶�����
	Byte              ucStageNo;        //��ǰ�׶κ�
	bool              bNeedUpdate;      //��������̬��̬�����Ƿ���Ҫ����
	SStepInfo         sStageStepInfo[MAX_STEP];   //����������Ϣ
	//ACE_Thread_Mutex  mMutex;                     //������
	bool              bIsChkLght ;
	bool              b8cndtown ;   //ADD:20131107
	Byte            ucManualType ;//ADD:20141021 ��ǰ�ֿ�����	
	Byte			  flashType ;   //ADD:20141106 ��ϵͳ���ڻ���״̬ʱ���������
	Byte              ucPlanId ;    //ADD:20150310 ʱ����
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;



/*
	��λ״̬�������
*/
struct SPhaseSts 
{
	Byte ucId;     //���
	Byte ucRed;    //������״̬  b0-b7 1 ���ź� 0 ���ź�
	Byte ucYellow; //�Ƶ�
	Byte ucGreen;  //�̵�
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
	ͨ��״̬�������
*/
struct SChannelSts 
{
	Byte ucId;     
	Byte ucRed;     //������״̬ 1 � 0���
	Byte ucYellow;
	Byte ucGreen;
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
	������λ״̬����
*/
struct SOverlapPhaseSts 
{
	Byte ucId;
	Byte ucRed;      //������״̬��־ ÿһλ1����λ
	Byte ucYellow;
	Byte ucGreen;
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
	�����״̬����
*/
struct SDetectorSts 
{
	Byte ucId;
	Byte ucStatus;  //״̬
	Byte ucAlarm;   //����
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
	��ͨ������ݲ���
*/
struct SDetectorData 
{
	Byte ucId;
	Byte ucVolume;       //������
	Byte ucLongVolume;   //���ͳ�����
	Byte ucSmallVolume;  //С�ͳ�����
	Byte ucOccupancy;    //ռ����
	Byte ucVelocity;     //�ٶ�
	Byte ucVehLen;       //������
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
	����������澯
*/
struct SDetectorAlarm
{
	Byte ucId;
	Byte ucDetAlarm;   //���������״̬
	Byte ucCoilAlarm;  //��Ӧ��Ȧ����״̬
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

//�źŻ�״̬
struct STscStatus
{
	bool bStartFlash;       //����ʱ�Ļ���
	Uint uiWorkStatus;      //����״̬
	Uint uiCtrl;            //���Ʒ�ʽ
	Byte ucStepNo;          //��ǰ����
	Byte ucStageNo;         //��ǰ�׶�
	Byte ucActiveSchNo;     //��ǰ���ʱ�α��
	Byte ucTscAlarm2;       //�źŻ�����2
	Byte ucTscAlarm1;       //�źŻ�����1
	Byte ucTscAlarmSummary; //�źŻ�����ժҪ

	Byte ucCurStageLen[16]; //��ǰ�������׶�ʱ��
	Byte ucCurKeyGreen[16]; //��ǰ�������ؼ���λ�̵�ʱ��
	
	Byte ucActiveDetCnt;             //����������
	SDetectorSts  sDetSts[8];        //�����״̬
	SDetectorData sDetData[48];      //��ͨ���������
	SDetectorAlarm    sDetAlarm[48]; //���������

	SPhaseSts  sPhaseSts[MAX_PHASE/8];       //��λ״̬���
	SOverlapPhaseSts  sOverlapPhaseSts;      //������λ״̬���
	SChannelSts  sChannelSts[MAX_CHANNEL/8]; //ͨ��״̬���
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
*��λ�������Ķ�Ӧ��ϵ
*/
struct SPhaseDetector 
{
	int iRoadwayCnt;                 //������ �� �������
	int iDetectorId[MAX_DETECTOR];   //��Ӧ�ļ����id
	int iDetectorCarNumbers[MAX_DETECTOR] ; //��Ӧ�����������
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;

/*
*�¹���GAT508-2014 32����ַ����ʱ��Ϣ
*/
struct SNewGBCntDownTimeInfo
{
	Byte CntDownColor;               //����ʱ�豸��ɫ
	Byte CntDownTime;                //����ʱ�豸ʱ��
}
PACKED
;




/*
*Can���ݽṹ
*/
struct SCanFrame 
{
	Ulong ulCanId;     //canid 4�ֽ�32bit ��Чλ29λ 11������ID��18����չID
	Byte pCanData[8];  //can�������8���ֽ�
	Byte ucCanDataLen; //can���ݵ���Ч����
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;


/*
* Detctor Loop
*
*/
struct SLoopLink
{
	Byte ucForDetId;    //1-16
	Byte ucBackDetId;   //1-16
}
/*
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
*/
PACKED
;
/*
*��ǰ�������ƴ��ݲ���
*/
struct SPreAnalysisParaData 
{
	Byte DevId ; //�豸��	
	bool    IsUsed ;
	bool    IsOffline ; //�Ƿ�����
	Ushort CarLength ; //�����Ŷӳ���
	Ushort CarNumber ; //������Ŀ
	Byte   CarDirecLamp ; //������������
	Byte   PhaseId ;	
};

struct VehicleStat
{
	Ulong       ulId;                       /*���*/
	Byte       ucDetId;                    /*�����id*/
	Ulong       ulCarTotal;                 /*�����������Ŷӳ���*/
	Byte       ucOccupancy;                /*ռ����*/
	Ulong       ulAddtime;                 /*��¼��ӵ�ʱ��*/
	bool        bRecordQueueLength ;       //��¼�Ŷӳ���
}PACKED ;

/*Send CntDownNum to client*/
struct SendCntDownNum
{
	bool bSend ;
	bool bUsed ;
	ACE_INET_Addr addClient;  //�ͻ��˵�ַ
};

//#pragma pack(pop)
#undef PACKED 
#if !defined(__GNUC__) 
#pragma pack() 
#endif

/*
*�����ϱ�
*/
struct SReportSelf
{
	Byte   ucTick;            //�ۼƴ���
	Ushort usCycle;           //���� ��λ100ms
	Byte   ucCmd;             //������
#ifdef GBT_TCP
	Byte   ucGbtDealDataIndex;  //������±�
#else
	ACE_INET_Addr addClient;  //�ͻ��˵�ַ
#endif
};




#endif  //_COMSTRUCT_H_
