#ifndef _GBTDB_H_
#define _GBTDB_H_

#pragma once

#include <ace/Thread_Mutex.h>
#include <ace/OS.h>
#include "sqlite3.h"
#include "ComDef.h"

namespace GBT_DB
{
#define CONST_MODULE          "ucMaxModule"
#define CONST_PLAN            "ucMaxPlan" 
#define CONST_SCHEDULE        "ucMaxSchedule" 
#define CONST_SCHEDULEEVT     "ucMaxScheduleEvt" 
#define CONST_EVENTTYPE       "ucMaxEventType"
#define CONST_EVENTLOG        "ucMaxEventLog"
#define CONST_DETECTOR        "ucMaxDetector"
#define CONST_PHASE           "ucMaxPhase"
#define CONST_CHANNEL         "ucMaxChannel" 
#define CONST_PATTERN         "ucMaxPattern"
#define CONST_STAGEPATTERN    "ucMaxStagePattern"
#define CONST_STAGE           "ucMaxStage"
#define CONST_OVAERLAPPHASE   "ucMaxOverlapPhase"

#define SYS_DEVICEID        "usDeviceId"
#define SYS_SYNCHTIME       "ucSynchTime"
#define SYS_SYNCHFLAG       "usSynchFlag"
#define SYS_ZONE            "lZone"
#define SYS_DETDATA_SEQNO   "ucDetDataSeqNo"
#define SYS_DETDATA_CYCLE   "ucDetDataCycle"
#define SYS_DETPULSE_SEQNO  "ucDetPulseSeqNo"
#define SYS_DETPULSE_CYCLE  "ucDetPulseCycle"
#define SYS_FLSTIME_START   "ucFlsTimeWhenStart"
#define SYS_REDTIME_START   "ucAllRedTimeWhenStart"
#define SYS_REMOTECTRL_FLAG "ucRemoteCtrlFlag"
#define SYS_FLASH_FREQ      "ucFlashFreq"
#define SYS_BRTTIME_BEGIN   "ulBrtCtrlBgnTime"
#define SYS_BRTTIME_END     "ulBrtCtrlEndTime"
#define SYS_GLOBAL_CYCLE    "ucGlobalCycle"
#define SYS_PHASE_OFFSET    "ucCoorPhaseOffset"
#define SYS_DEGRADE_MODE    "ucDegradeMode"
#define SYS_CTRL_OPTFLAG    "ucCtrlMasterOptFlag"
#define SYS_SIGDEV_COUNT    "ucSigDevCount"
#define SYS_BASE_ADDR       "usBaseAddr"
#define SYS_DOWNLOAD_FLAG   "ucDownloadFlag"

#if defined(__GNUC__) 
#define PACKED  __attribute__((packed)) 
#else 
#pragma pack(1) 
#define PACKED 
#endif   

const Ushort SMALL_BUF_SIZE     = 256;
const Ushort LARGE_BUF_SIZE     = 1024;

const Ushort MAX_STRING_LEN     = 255;
const Ushort MAX_MODULE_COUNT   = 16;
const Ushort MAX_PLAN_COUNT     = 40;
const Ushort MAX_STAGE_COUNT    = 16;   /*���������׶���*/
const Ushort MAX_PHASE_COUNT    = 32;   /*�����λ��*/
const Byte  MAX_STAGEFACT_CNT  = 30;   //ͳ����Ϣ 30��



/*
	ģ������ö��
*/
enum
{
    MODULE_OTHER        = 1,        //��������
    MODULE_HARDWARE     = 2,        //Ӳ��ģ��
    MODULE_SOFTWARE     = 3        //���ģ��
};




 
enum
{
    TBL_SYSTEM,			 //ϵͳ��
    TBL_CONSTANT,        //������
    TBL_MODULE,			 //ģ���
    TBL_PLAN,			 //ʱ�����ȱ�
    TBL_SCHEDULE,  		 //ʱ�α�
    TBL_PHASE,			 //��λ��
    TBL_COLLISION,       //��λ��ͻ��
    TBL_DETECTOR,        //�������
    TBL_CHANNEL,         //ͨ����
    TBL_PATTERN,         //������
    TBL_STAGEPATTERN,    //�׶���ʱ��
    TBL_OVERLAPPHASE,    //������λ��
    TBL_EVENTTYPE,       //�¼����ͱ�
    TBL_EVENTLOG,        //�¼���־��
	TBL_VEHICLESTAT,     //����ͳ�� �Ӵ����¾�Ϊ��չ��
	TBL_CNTDOWNDEV,      //����ʱ����
	TBL_SPECFUN,         //���⹦�����ñ�
	TBL_DET_EXTEND,      //�������չ��
	TBL_STAGE_FACTTIME,  //��¼�����׶ε�ʱ��
	TBL_ADAPT_STAGETIME, //����Ӧ�������ɵĸ����׶�ʱ��
	TBL_PHASE2DIREC,     //��λ�뷽���Ӧ��
	TBL_ADAPT_PARA,      //����Ӧ���Ʋ���
   	TBL_LAMP_CHECK,      //����ͨ������
    TABLE_COUNT          //�źż�ϵͳ������
};

/*
	ϵͳ����Ϣ�ṹ��
*/
typedef struct
{
    int     iTblId;       //���
    int     iFieldCount;  //���ֶ���
    char*   sTblName;     //����
    char**  sFieldName;   //���ֶ���
}PACKED TableDesc;

/*
	���ݿ����ģ����
*/
template <class TD, class TC>
class DbArray
{
public:
    DbArray(void)      				//Ĭ�Ϲ��캯��
    {
        m_uCount = 0; 
        m_pData = NULL;
    };
    ~DbArray(void)                  //��������
    {
        if (NULL != m_pData)
        {
            ACE_OS::free(m_pData);
            
        }
        m_pData     = NULL;
        m_uCount    = 0;
    };
    DbArray(const TD* pData, TC uCount)   //���ι��캯��
    {
        m_uCount   = 0;
        m_pData    = NULL;
        
        if (0 == uCount || NULL == pData)
        {
            return;
        }
        
        m_pData = (TD*)ACE_OS::malloc(uCount*sizeof(TD));

        if (NULL != pData)
        {
            m_uCount = uCount;
            ACE_OS::memcpy(m_pData, pData, uCount*sizeof(TD));
        }
    };
    
    DbArray(DbArray& dbArray)               //���ƹ��캯��
    {
        m_uCount   = 0;
        m_pData    = NULL;
        
        TC  uCount;
        TD* pData;

        pData = GetData(uCount);
        if (0 == uCount || NULL == pData)
        {
            return;
        }
        
        m_pData = (TD*)ACE_OS::malloc(uCount*sizeof(TD));

        if (NULL != pData)
        {
            m_uCount = uCount;
            ACE_OS::memcpy(m_pData, pData, uCount*sizeof(TD));
        }
    };
    
    
    TD* GetData(TC& uCount)    //�������ݻ�ȡ����
    {
        uCount = m_uCount;
        return m_pData;
    };
    
    
    TD* GetData()              //�޲����ݻ�ȡ����
    {
        return m_pData;
    }
    TC GetCount()
    {
        return m_uCount;
    };
    
    
    bool SetData(const TD* pData, TC uCount)   //�������ú���
    {
        if (NULL != m_pData)
        {
            ACE_OS::free(m_pData);
        }
        
        m_pData = NULL;
        m_uCount = 0;

        if (0 == uCount || NULL == pData)
        {
            return true;
        }
        
        m_pData = (TD*)ACE_OS::malloc(uCount*sizeof(TD));

        if (NULL != m_pData)
        {
            m_uCount = uCount;
            ACE_OS::memcpy(m_pData, pData, uCount*sizeof(TD));
        }
        else
        {
            return false;
        }

        return true;
    };
    
    
    bool SetString(const char* sString)     //�ַ������ú���
    {
        if (1 != sizeof(TD))
        {
            return false;
        }

        TC   uCount;
        uCount = (TC)(ACE_OS::strlen(sString) + 1);
        if (!AllocData(uCount))
        {
            return false;
        }

        ACE_OS::strcpy(m_pData, sString);
        return true;
    };
    
    
    bool AllocData(TC uCount)                   //�ռ���亯��
    {
        if (NULL != m_pData)
        {
            ACE_OS::free(m_pData);
        }
        
        m_pData = NULL;
        m_uCount = 0;

        if (0 == uCount)
        {
            return true;
        }
        
        m_pData = (TD*)ACE_OS::malloc(uCount*sizeof(TD));

        if (NULL != m_pData)
        {
            m_uCount = uCount;
            ACE_OS::memset(m_pData, 0, sizeof(TD)*uCount);
        }
        else
        {
            return false;
        }

        return true;
    };


    DbArray &operator = (DbArray& dbArray)    //���������
    {
        if (NULL != m_pData)
        {
            ACE_OS::free(m_pData);
        }
        
        m_pData = NULL;
        m_uCount = 0;

        
        TC  uCount;
        TD* pData = dbArray.GetData(uCount);

        if (0 == uCount || NULL == pData)
        {
            return *this;
        }

        m_pData = (TD*)ACE_OS::malloc(uCount*sizeof(TD));

        if (NULL != m_pData)
        {
            m_uCount = uCount;
            ACE_OS::memcpy(m_pData, pData, uCount*sizeof(TD));
        }

        return *this;
    };
 private:
    TC      m_uCount;
    TD*     m_pData;
};

typedef DbArray<Char, Ushort>       DbString;      //�������

typedef DbArray<Byte, Byte>         DbByteArray;   //�������


/*
����:����������ض���� 0x81-0x84
˵��:C2.2    ������ʵ�ִ�����豸���͵����ù��ܵĹ��ö���
*/
typedef struct
{
    Ushort usDevicId;       /*0x81 �����豸ʶ�����*/
    Byte  ucMaxModule;     /*0x82 ����ģ�����������*/
    Ushort usSynchTime;     /*0x83 ����ͬ��ʱ��*/
    Ushort usSynchFlag;     /*0x84 ����ͬ����־*/
}PACKED TblCommonPara;

/*
����:ģ������� 0x86
˵��:C2.3
    ���������Ӳ�����̵���Ϣ��ÿ������ģ����ͺš��汾��
    �Լ�ģ��ʹӲ����Ʒ���������Ʒ�Ķ�����Ϣ
*/
typedef struct
{
    Byte       ucModuleId;                 /*ģ�������,1-16*/
    DbString    strDevNode;                 /*ģ���豸�ڵ�*/
    DbString    strCompany;                 /*ģ��������*/
    DbString    strModel;                   /*ģ���ͺ�*/
    DbString    strVersion;                 /*ģ��汾*/
    Byte       ucType;                     /*ģ������,��ö��enModuleType*/
}Module;
typedef DbArray<Module, Byte>     TblModule;  //����ģ�����ݱ�����

/*
����:����ʱ����� 0x86-0x8C
˵��:C3.1
    ������ʵ�ִ�����豸���͵�ʱ����ع��ܵĹ��ö���
*/
typedef struct
{
    Ulong       ulUtc;                      /*0x86 UTCʱ��,��1970-01-01 00:00:00���������*/
    Long        lZone;                      /*0x87 ʱ��,��UTCʱ�����������*/
    Ulong       ulLocal;                    /*0x88 ����ʱ��*/
    Byte       ucMaxPlanCnt;               /*0x89 ���ȼƻ�����������*/
    Byte       ucMaxScheduleCnt;           /*0x8A ʱ�α�������*/
    Byte       ucMaxEvtCount;              /*0x8B ʱ�α����ʱ����(�¼�)*/
    Byte       ucActiveSheduleNo;          /*0x8C �ʱ�α�ı��*/
}PACKED TblTimePara;

/*
����:ʱ�����ȱ� 0x8D
˵��:C3.2
*/
typedef struct
{
    Byte       ucId;                       /*���ȼƻ���,1-40*/
    Ushort      usMonthFlag;                /*�����£�bit1��bit12�ֱ��Ӧ1��12����*/
    Byte       ucWeekFlag;                 /*�����죬bit1��bit7��Ӧ��һ������*/
    Ulong       ulDayFlag;                  /*�����죬bit1��bit7��Ӧ1����31��*/
    Byte       ucScheduleId;               /*ʱ�α��*/
}PACKED Plan;
typedef DbArray<Plan, Byte>       TblPlan; //����ʱ�����ȱ�����

/*
����:ʱ�α�   0x8E
˵��:C3.3
*/
typedef struct
{
    Byte       ucScheduleId;               /*ʱ�α��*/
    Byte       ucEvtId;                    /*ʱ���¼���*/
    Byte       ucBgnHour;                  /*��ʼִ�е�������*/
    Byte       ucBgnMinute;                /*��ʼִ�е�������*/
    Byte       ucCtrlMode;                 /*���Ʒ�ʽ����ö��enControlMode*/
    Byte       ucPatternNo;                /*��ʱ������*/
    Byte       ucAuxOut;                   /*�����������*/
    Byte       ucSpecialOut;               /*���⹦�����*/
}PACKED Schedule;
typedef DbArray<Schedule, Ushort>       TblSchedule;  //����ʱ�α�����


/*
����:�¼�������
˵��:C4.1
*/
typedef struct
{
    Byte   ucMaxEvtType;                   /*0x8F �¼����ͱ���������*/
    Byte   ucMaxLogLine;                   /*0x90 �¼���־����������*/
}PACKED TblEventPara;


/*
����:�¼����ͱ� 0x91
˵��:C4.2

*/
typedef struct
{
    Byte       ucEvtTypeId;                /*�¼����ͱ��*/
    Ulong       ulClearTime;                /*�¼����͵����ʱ��*/
    DbString    strEvtDesc;                 /*�¼���������*/
    Byte       ucLogCount;                 /*�����¼����¼���־���е�����*/
}EventType;
typedef DbArray<EventType, Byte>   TblEventType;  //������־���ʹ�����



/*
����:�¼���־�� 0x92
˵��:C4.3
*/
typedef struct
{
    Byte       ucEventId;                  /*�¼����*/
    Byte       ucEvtType;                  /*�¼�����*/
    Ulong       ulHappenTime;               /*�¼�������ʱ��*/
    Ulong       ulEvtValue;                 /*�¼�ֵ*/
}PACKED EventLog;
typedef DbArray<EventLog, unsigned int>     TblEventLog; //������־������



typedef struct
{
	Ulong       ulId;                       /*���*/
	Byte       ucDetId;                    /*�����id*/
	Ulong       ulCarTotal;                 /*�����������Ŷӳ���*/
	Byte       ucOccupancy;                /*ռ����*/
	Ulong       ulAddtime;                 /*��¼��ӵ�ʱ��*/
}PACKED VehicleStat;
typedef DbArray<VehicleStat, Ulong>    TblVehicleStat;  //���峵��ͳ����

/*
����:��λ��ز�����
˵��:C5.1
*/
typedef struct
{
    Byte       ucMaxPhaseCount;            /*0x93 ��λ����������*/
    Byte       ucMaxPhaseGrp;              /*0x94 ��λ״̬����������*/
}PACKED PhasePara;


/*
����:��λ��ز�����  0x95
˵��:C5.2
*/
typedef struct
{
    Byte       ucPhaseId;                  /*��λ��*/
    Byte       ucPedestrianGreen;          /*�����̵�ʱ��*/
    Byte       ucPedestrianClear;          /*�������ʱ��*/
    Byte       ucMinGreen;                 /*��С�̵�ʱ��*/
    Byte       ucGreenDelayUnit;           /*��λ�̵��ӳ�ʱ��*/
    Byte       ucMaxGreen1;                /*����̵�ʱ��1*/
    Byte       ucMaxGreen2;                /*����̵�ʱ��2*/
    Byte       ucFixGreen;                 /*������λ�̶��̵�ʱ��*/
    Byte       ucGreenFlash;               /*����ʱ��*/
    Byte       ucPhaseTypeFlag;            /*��λ����*/
    Byte       ucPhaseOption;              /*��λѡ���*/
    Byte       ucExtend;                   /*��չ�ֶ�*/
}PACKED Phase;
typedef DbArray<Phase, Byte>   TblPhase; //������λ����������


/*
����:��λ���״̬�� 0x96
˵��:C5.3
*/
typedef struct
{
    Byte       ucId;                       /*����*/
    Byte       ucRedOut;                   /*��λ�ĺ�����״̬*/
    Byte       ucYellowOut;                /*��λ�ĻƵ����״̬*/
    Byte       ucGreenOut;                 /*��λ���̵����״̬*/
}PACKED PhaseSignal;

typedef DbArray<PhaseSignal, Byte>   TblPhaseSignal; //������λ���״̬������


/*
����:��λ��ͻ�� 0x97
˵��:C5.4

*/
typedef struct
{
    Byte    ucPhaseId;         /*��λ��*/
    Uint32  usCollisionFlag ;    /*��ͻ��λ*/
}PACKED Collision;
typedef DbArray<Collision, Byte>   TblCollision;



/*
����:���������
˵��:C6.1
*/
typedef struct
{
    Byte       ucMaxDetCount;          /*0x98 ��������������*/
    Byte       uMaxDetGrpCount;        /*0x99 �����״̬��������*/
    Byte       ucDataSeqNo;            /*0x9A �����������ˮ��*/
    Byte       ucDataCycle;            /*0x9B ���ݲɼ�����*/
    Byte       uDetCount;              /*0x9C �������ĸ���*/
    Byte       ucPulseSeqNo;           /*0x9D ����������ˮ��*/
    Byte       ucPulseCycle;           /*0x9E �������ݲɼ�����*/
}PACKED DetectorPara;


/*
����:����������� 0x9F
˵��:C6.2
*/
typedef struct
{
    Byte       ucDetectorId;       /*�����ID*/
    Byte       ucPhaseId;          /*�������Ӧ����λ*/
    Byte       ucDetFlag;          /*��������ͱ�ʶ*/
    Byte       ucDirect;           /*���������*/
    Byte       ucValidTime;        /*�������Чʱ��*/
    Byte       ucOptionFlag;       /*�����ѡ�����*/
    Ushort      usSaturationFlow;   /*��������*/
    Byte       ucSaturationOccupy; /*����ռ����*/
}PACKED Detector;
typedef DbArray<Detector, Byte>   TblDetector;  //���峵�������������


/*
*������ ����ʱ���ñ�
*/
typedef struct
{
	Byte       ucDevId;     
	Ushort      usPhase;  
	Byte       ucOverlapPhase;       
	Byte       ucMode;       
}PACKED CntDownDev;
typedef DbArray<CntDownDev, Byte>   TblCntDownDev; //���嵹��ʱ�豸��

/*
����:������ �������չ��
*/
typedef struct
{
	Byte       ucId;          //�������
	Byte       ucSensi;       //������
	Byte       ucGrpNo;       //��������
	Byte       ucPro;         //Ȩֵ
	Byte       ucOcuDefault;  //ռ����Ĭ��ֵ
	Ushort      usCarFlow;     //������Ĭ��ֵ
	Byte       ucFrency ;      //��Ȧ��Ƶ��  ADD:20120807 10 12
	Byte       ucGrpData ;     //��Ȧ��ֵ
	Ushort     ucGrpDistns ;   //��Ȧ�󶨾���
}PACKED DetExtend;

typedef DbArray<DetExtend, Byte>  TblDetExtend;  //����������չ����

/*
����:������ ����Ӧ����ֵ��
*/
typedef struct
{
	Byte       ucType;         //����Ӧ����
	Byte       ucFirstPro;     //��һ�����ڵ�Ȩֵ
	Byte       ucSecPro;       //�ڶ������ڵ�Ȩֵ
	Byte       ucThirdPro;     //���������ڵ�Ȩֵ
	Byte       ucOcuPro;       //ռ����Ȩֵ
	Byte       ucCarFlowPro;   //������Ȩֵ
	Byte       ucSmoothPara;   //ƽ��ƽ��������
}PACKED AdaptPara;

typedef DbArray<AdaptPara, Byte>  TblAdaptPara;  //��������Ӧ��

/*
����:�����״̬�� 0xA0
˵��:C6.3
*/
typedef struct
{
    Byte       ucId;               /*�����״̬���к�*/
    Byte       ucStatus;           /*�����״̬*/
    Byte       ucAlarm;            /*���������*/
}PACKED DetectorStatus;
typedef DbArray<DetectorStatus, Byte>  TblDetectorSts; //��������״̬������

/*
����:��������ݲɼ��� 0xA1
˵��:C6.4
*/
typedef struct
{
    Byte       ucDectorId;         /*��������*/
    Byte       ucVehCount;         /*�����������ͳ�������*/
    Byte       ucLargeCount;       /*���ͳ�������*/
    Byte       ucSmallCount;       /*С�ͳ�������*/
    Byte       ucOccupy;           /*ռ����*/
    Byte       ucVelocity;         /*�ٶ�*/
    Byte       ucVehLeng;          /*������*/
}PACKED DetectorData;
typedef DbArray<DetectorData, Byte>  TblDetectorData;

/*
����:��������ϱ����� 0xA2
˵��:C6.5
*/
typedef struct
{

    Byte       ucDetectorId;           /*��������*/
    Byte       ucDetAlmFlag;           /*���������״̬*/
    Byte       ucLoopAlmFlag;          /*��Ȧ����״̬*/
}PACKED DetAlmSts;
typedef DbArray<DetAlmSts, Byte>  TblDetAlmSts;

/*
����:��Ԫ��ز�����
˵��:C7.1
*/
typedef struct
{
    Byte       ucFlsTimeWhenStart;     /*0xA3 ����ʱ������ʱ��*/
    Byte       ucAllRedTimeWhenStart;  /*0xA4 ����ʱ��ȫ��ʱ��*/
    Byte       ucCtrlSts;              /*0xA5 ��ǰ�źŻ��Ŀ���״̬*/
    Byte       ucFlashCtrlMode;        /*0xA6 ��ǰ���������ģʽ*/
    Byte       ucAlarm2;               /*0xA7 �źŻ�����2*/
    Byte       ucAlarm1;               /*0xA8 �źŻ�����1*/
    Byte       ucAlmSts;               /*0xA9 �źŻ�����ժҪ*/
    Byte       ucRemoteCtrlFlag;       /*0xAA Զ�̿��Ʊ�ʶ*/
    Byte       ucFlashFreq;            /*0xAB ����Ƶ��*/
    Ulong       ulBrtCtrlStrTime;       /*0xAC �Զȿ��ƿ�ʼʱ��*/
    Ulong       ulBrtCtrlEndTime;       /*0xAD �Ҷȿ��ƽ���ʱ��*/
}PACKED TblCtrlUnitPara;

/*
����:�ƿض˿ڲ���
˵��:C8.1
*/
typedef struct
{
    Byte       ucMaxChannel;           /*0xAE�źŻ�֧�ֵ����ͨ����*/
    Byte       ucChnGrpCount;          /*0xAFͨ������*/
}PACKED TblChannelPara;

/*
����:�ƿض˿ڲ��� 0xB0
˵��:C8.2
*/
typedef struct
{
    Byte       ucChannelId;            /*ͨ����*/
    Byte       ucCtrlSrc;              /*ͨ�����Ƶ��ź�Դ������λ��*/
    Byte       ucAutoFlsCtrlFlag;      /*�Զ�����Ŀ���ģʽʱ��ͨ��״̬*/
    Byte       ucCtrlType;             /*ͨ����������*/
}PACKED Channel;
typedef DbArray<Channel, Byte>   TblChannel;

/*
����:ͨ�����״̬�� 0xB1
˵��:C8.3
*/
typedef struct
{
    Byte   ucId;                       /*ͨ��״̬���кţ�1��2*/
    Byte   ucRedSts;                   /*���״̬*/
    Byte   ucYellowSts;                /*�Ƶ�״̬*/
    Byte   ucGreenSts;                 /*�̵�״̬*/
}PACKED ChannelStatus;
typedef DbArray<ChannelStatus, Byte>   TblChannelSts;

/*
����:���Ʋ�����
˵��:C9.1
*/
typedef struct
{
    Byte   ucPatternCount;             /*0xB2 ��ʱ������*/
    Byte   ucMaxStgPatternCount;       /*0xB3 ���׶���ʱ���С*/
    Byte   ucMaxStageCount;            /*0xB4 ���׶���*/
    Byte   ucManuCtrlFlag;             /*0xB5 �ֶ����Ʒ�����־*/
    Byte   ucSysCtrlFlag;              /*0xB6 ϵͳ���Ʒ�����ʶ*/
    Byte   ucCtrlMode;                 /*0xB7 ���Ʒ�ʽ*/
    Byte   ucGlobalCycle;              /*0xB8 ��������ʱ��*/
    Byte   ucPhaseOffset;              /*0xB9 Э����λ��*/
    Byte   ucStageSts;                 /*0xBA �׶�״̬*/
    Byte   ucStepCmd;                  /*0xBB ����ָ��*/
    Byte   ucDegradeMode;              /*0xBC ����ģʽ*/
    Byte   ucDegradePattern[14];       /*0xBD ������׼������*/
    Byte   ucStageTime[MAX_STAGE_COUNT];/*0xBE ��ǰ�������׶ε�ʱ��*/
    Byte   ucGreenTime[MAX_STAGE_COUNT];/*0xBF ��ǰ���������ؼ���λ�ĵ��̵�ʱ��*/
}PACKED CtrlPara;


/*
����:��ʱ������
˵��:C9.2
*/
typedef struct
{
    Byte   ucPatternId;                 /*������*/
    Byte   ucCycleTime;                 /*����ʱ��*/
    Byte   ucOffset;                    /*��λ��*/
    Byte   ucCoorPhase;                /*Э����λ*/
    Byte   ucStagePatternId;           /*�׶���ʱ���*/
}PACKED Pattern;
typedef DbArray<Pattern, Byte>  TblPattern;


/*
����:�׶���ʱ��
˵��:C9.3
*/
typedef struct
{
    Byte   ucStagePatternId;       /*�׶���ʱ������*/
    Byte   ucStageNo;              /*�׶κ�*/
    Uint32  usAllowPhase;           /*������λ*/
    Byte   ucGreenTime;            /*�׶��̵�ʱ��*/
    Byte   ucYellowTime;           /*�׶��̻Ƶ�ʱ��*/
    Byte   ucRedTime;              /*�׶κ��ʱ��*/
    Byte   ucOption;               /*�׶�ѡ�����*/
}PACKED StagePattern;
typedef DbArray<StagePattern, Ushort>  TblStagePattern;


/*
������λ��ز���
����:������λ��
˵��:C13.1
*/
typedef struct
{
    Byte ucOverlapPhaseId;          /*������λ���*/
    Byte ucOperateType;            /*��������*/
    Byte ucIncldPhaseCnt;          /*������λ�ĸ���*/
    Byte ucIncldPhase[MAX_PHASE_COUNT];/*������λ*/
    Byte ucAdjustPhaseCnt;             /*������λ�ĸ���*/
    Byte ucAdjustPhase[MAX_PHASE_COUNT];/*������λ*/
    Byte ucTailGreen;                  /*β���̵�*/
    Byte ucTailYellow;                  /*β���Ƶ�*/
    Byte ucTailRed;                    /*β�����*/
}PACKED OverlapPhase;
typedef DbArray<OverlapPhase, Byte>  TblOverlapPhase;

/*
*�źŻ����⹦�����ñ�
*/
typedef struct
{
	Byte ucFunType ;  //��������
	Byte ucValue   ;  //ֵ ��ͬ�Ĺ������Ͷ�Ӧ��ͬ��ֵ
}PACKED SpecFun;
typedef DbArray<SpecFun, Byte>  TblSpecFun;

/*
*�źŻ��Ľ׶��̵�ʱ�� ����Ӧ����
*/
typedef struct
{
	Ulong      uiAddTime;        //��¼��ʱ��
	Byte      ucStageCnt;       //�׶θ���
	DbString   sStageGreenTime;  //�����׶ε��̵�ʱ��
}StageFactTime;
typedef DbArray<StageFactTime, Ulong> TblStageFactTime;

/*
*����Ӧ�������ɵĽ׶��̵�ʱ��
*/
typedef struct
{
	Byte      ucId;             //��id
	Byte      ucWeekType;       //����
	Byte      ucHour;           //ʱ
	Byte      ucMin;            //��
	Byte      ucStageCnt;       //�׶θ���
	DbString   sStageGreenTime;  //�����׶ε��̵�ʱ��
}AdaptStageTime;
typedef DbArray<AdaptStageTime, Byte> TblAdaptStageTime;

/*
*��������λ��Ӧ��
*/
typedef struct 
{
	Byte ucId;
	Byte ucPhase;
	Byte ucOverlapPhase;
	Byte ucRoadCnt;
}PhaseToDirec;
typedef DbArray<PhaseToDirec, Byte> TblPhaseToDirec;

/*
ͨ�����ݼ�����ñ�  ADD:2013 0801 0901

*/
typedef struct
{	
	Byte ucSubChannelId;		//ͨ������·��	 Ŀǰ4�����ù�48·	1---48
	Byte ucIsCheck; 			//�Ƿ���Ҫ���е��ݼ�� 0-����Ҫ 1-��Ҫ
}PACKED ChannelChk;
typedef DbArray<ChannelChk,Byte>TblChannelChk;



#undef PACKED 
#if !defined(__GNUC__) 
#pragma pack() 
#endif

/*
	���ݿ���ϴ�����
*/
class CSqliteRst
{
public:
    CSqliteRst();
    ~CSqliteRst();
    void Clear();
public:
    char**  m_ppResult;  //���ݿ���ѯ�����
    int     m_iRow;      //������
    int     m_iColum;    //������
};

class CGbtTscDb
{
public:
    CGbtTscDb();
    ~CGbtTscDb();

    /*��ʼ�����ݿ�*/
    bool InitDb(const char* pDbPath);
    /*�ر����ݿ�*/
    void CloseDb();
    /*����Ĭ������*/
	void InitDefaultData(); //ADD:20141209
/***********************ģ�������***************************/
    /*��ѯȫ��*/
    bool QueryModule(TblModule& tblModule);

   /*����ģ���ID��ѯ*/
    bool QueryModule(Byte uModuleId, Module& sModule);

    /*���������̲�ѯ*/
    bool QueryModule(const char* sCompany, TblModule& tblModule);

    /*���ģ���*/
    bool AddModule(Byte uModuleId, Module& sModule);

    /*�޸�ģ���*/
    bool ModModule(Byte uModuleId, Module& sModule);

    /*ɾ��ģ���*/
    bool DelModule(Byte uModuleId);

    /*���ģ���*/
    bool IsModuleValid(Byte ucModuleId, Module& sModule);

/***********************����Ӧ���ƵĽ׶��̵����ݴ�����***************************/
	/*��ѯȫ���׶��̵Ʊ�*/
	bool QueryStageFactTime(TblStageFactTime& tblStageFactTime);

	/*��ѯ�ض�ʱ�����Ľ׶��̵Ʊ�*/
	bool QueryStageFactTime(TblStageFactTime& tblStageFactTime,Ulong ulStartTime,Ulong ulEndTime);

	/*��ӽ׶��̵Ʊ�*/
	bool AddStageFactTime(StageFactTime& sStageFactTime);

	/*ɾ���׶��̵Ʊ�*/
	bool DeleteStageFactTime(Ulong ulAddTime);

/*********************����Ӧ��ѧϰ���ɵĽ׶�ʱ�������***********************/
	/*��ѯȫ������Ӧ�׶��̵Ʊ�*/
	bool QueryAdaptStageTime(TblAdaptStageTime& tblAdaptStageTime);

	/*�������ڲ�ѯȫ���׶��̵Ʊ�*/
	bool QueryAdaptStageTime(TblAdaptStageTime& tblAdaptStageTime,Byte ucWeekType);

	/*����ucid��ѯ��¼*/
	bool QueryAdaptStageTime(Byte ucId, AdaptStageTime& sAdaptStageTime);

	/*����ucid�޸ļ�¼*/
	bool ModAdaptStageTime(AdaptStageTime& sAdaptStageTime);

	/*�������Ӧ�׶��̵Ʊ�*/
	bool AddAdaptStageTime(AdaptStageTime& sAdaptStageTime);

	/*ɾ��ȫ������Ӧ�׶��̵Ʊ�*/
	bool DeleteAdaptStageTime();

/*****************���ȼƻ�������*****************************/
    /*��ѯʱ�����ȱ���ȼƻ���*/
    bool QueryPlan(TblPlan& tblPlan);
    
    /*���ݼƻ��Ų�ѯ���ȼƻ�*/
    bool QueryPlan(Byte uPlanId, Plan& sPlan);
    
    /*���ʱ�����ȱ�,���ȼƻ���*/
    bool AddPlan(Byte ucPlanId, /*���ȼƻ���*/
                 Plan& sPlan);    /*���ȼƻ���*/

    /*ɾ��ʱ�����ȱ�*/
    bool DelPlan(Byte ucPlanId);

	bool DelPlan();

    /*�޸�ʱ�����ȱ�*/
    bool ModPlan(Byte ucPlanId, Plan& sPlan);

    /*���ʱ�����ȱ�*/
    bool IsPlanValid(Byte ucPlanId,Plan& sPlan);

/*************************ʱ�α�**********************************/
    /*��ѯʱ�α�*/
    bool QuerySchedule(TblSchedule& tblSchedule);

    /*����ʱ�κŲ�ѯʱ�α�*/
    bool QuerySchedule(Byte uScheduleId, TblSchedule& tblSchedule);


    /*����ʱ�κź��¼��Ų�ѯʱ�α�*/
    bool QuerySchedule(Byte uScheduleId, Byte uEvtId, Schedule& sSchedule);
    
    /*���ʱ�α�*/
    bool AddSchedule(Byte uScheduleId, Byte uEvtId, Schedule& sSchedule);

    /*�޸�ʱ�α�*/
    bool ModSchedule(Byte uScheduleId, Byte uEvtId, Schedule& sSchedule);

    /*ɾ��ʱ�α�*/
    bool DelSchedule(Byte uScheduleId, Byte uEvtId);

    /*����ʱ�κ�ɾ��ʱ�α�*/
    bool DelSchedule(Byte uScheduleId);

     /*ɾ��ʱ�α�*/
    bool DelSchedule();

    bool IsScheduleValid(Byte uScheduleId, Byte uEvtId, Schedule& sSchedule);
    
/*****************��λ������*****************************/
    /*��ѯ���е���λ*/
    bool QueryPhase(TblPhase& tblPhase);

    /*������λ�Ų�ѯ*/
    bool QueryPhase(Byte uPhaseId, Phase& sPhase);
    
    /*�����λ*/
    bool AddPhase(Byte uPhaseId, Phase& sPhase);  
    
    /*�޸���λ��*/
    bool ModPhase(Byte uPhaseId, Phase& sPhase);

    /*ɾ����λ��*/
    bool DelPhase(Byte uPhaseId);
	bool DelPhase();
    bool IsPhaseValid(Byte uPhaseId, Phase& sPhase);
    
/*********************��λ��ͻ������******************************************/
    /*��ѯ���е���λ��ͻ*/
    bool QueryCollision(TblCollision& tblCollision);

    /*������λ�Ų�ѯ*/
    bool QueryCollision(Byte uPhaseId, Collision& sCollision);
    
    /*�����λ��ͻ*/
    bool AddCollision(Byte uPhaseId, Collision& sCollision);  
    
    /*�޸���λ��ͻ��*/
    bool ModCollision(Byte uPhaseId, Collision& sCollision);

    /*ɾ����λ��ͻ��*/
    bool DelCollision(Byte uPhaseId);

    bool IsCollisionValid(Byte uPhaseId, Collision& sCollision);
/**********************���������������*************************************/
    /*��ѯ���������*/
    bool QueryDetPara(DetectorPara& detPara);

public:
/******************************�����������*************************************/
    /*��ѯ���м����*/
    bool QueryDetector(TblDetector& tblDetector);

    /*���ݼ�����Ų�ѯ*/
    bool QueryDetector(Byte uDetectorId, Detector& sDetector);
    
    /*��Ӽ����*/
    bool AddDetector(Byte uDetectorId, Detector& sDetector);  
    
    /*�޸ļ������*/
    bool ModDetector(Byte uDetectorId, Detector& sDetector);

    /*ɾ���������*/
    bool DelDetector(Byte uDetectorId);

	bool DelDetector();

    bool IsDetectorValid(Byte uDetectorId, Detector& sDetector);

/********************************����ʱ���ñ�����************************************/
	bool QueryCntDownDev(Byte ucDevId, CntDownDev& sCntDownDev);

	bool QueryCntDownDev(TblCntDownDev& tblCntDownDev);

	bool AddCntDownDev(CntDownDev& sCntDownDev);

	bool DelCntDownDev(Byte ucDevId);

	bool ModCntDownDev(Byte ucDevId, CntDownDev& sCntDownDev);

/*******************************���⹦�ܿ������ñ�����******************************/
	bool QuerySpecFun(SpecFun& sSpecFun);

	bool QuerySpecFun(TblSpecFun& tblSpecFun);

	bool AddSpecFun(Byte ucFunType,Byte ucValue);

	bool ModSpecFun(Byte ucFunType,Byte ucValue);

/*******************************�������չ������***********************************/
	bool QueryDetExtend(DetExtend& sDetExtend);

	bool QueryDetExtend(Byte uDetId, DetExtend& sDetExtend);

	bool QueryDetExtend(TblDetExtend& tblDetExtend);

	bool AddDetExtend(DetExtend sDetExtend);

	bool ModDetExtend(Byte uDetExtendId, DetExtend& sDetExtend);

/*****************************����Ӧ����������**********************************/
	bool QueryAdaptPara(TblAdaptPara& tblAdaptPara);
	
	bool QueryAdaptPara(AdaptPara& sAdaptPara);

	bool AddAdaptPara(AdaptPara sAdaptPara);

	bool ModAdaptPara(AdaptPara& sAdaptPara);

/********************************ͨ��������*****************************************/
    /*��ѯ���е�ͨ��*/
    bool QueryChannel(TblChannel& tblChannel);

    /*����ͨ���Ų�ѯ*/
    bool QueryChannel(Byte uChannelId, Channel& sChannel);
    
    /*���ͨ��*/
    bool AddChannel(Byte uChannelId, Channel& sChannel);  

    /*�޸�ͨ����*/
    bool ModChannel(Byte uChannelId, Channel& sChannel);

    /*ɾ����λ��ͻ��*/
    bool DelChannel(Byte uChannelId);

	bool DelChannel();

    bool IsChannelValid(Byte uChannelId, Channel& sChannel);
/********************************��ʱ����������*****************************************/
    /*��ѯ���е���ʱ����*/
    bool QueryPattern(TblPattern& tblPattern);

    /*���ݷ����Ų�ѯ*/
    bool QueryPattern(Byte uPatternId, Pattern& sPattern);
    
    /*�����ʱ����*/
    bool AddPattern(Byte uPatternId, Pattern& sPattern);  
    
    /*�޸���ʱ������*/
    bool ModPattern(Byte uPatternId, Pattern& sPattern);

    /*ɾ����λ��ͻ��*/
    bool DelPattern(Byte uPatternId);
	bool DelPattern();
    bool IsPatternValid(Byte uPatternId, Pattern& sPattern);  

/********************************�׶���ʱ������*****************************************/
    /*��ѯ���еĽ׶���ʱ*/
    bool QueryStagePattern(TblStagePattern& tblStage);

    /*���ݽ׶���ʱ�Ų�ѯ*/
    bool QueryStagePattern(Byte uStagePatternId, TblStagePattern& tblStage);

    /*���ݽ׶���ʱ�źͽ׶κŲ�ѯ*/
    bool QueryStagePattern(Byte uStagePatternId, Byte uStageNo, StagePattern& sStage);
    
    /*��ӽ׶���ʱ����*/
    bool AddStagePattern(Byte uStagePatternId, Byte uStageNo, StagePattern& sStage);  
    
    /*�޸Ľ׶���ʱ��*/
    bool ModStagePattern(Byte uStagePatternId, Byte uStageNo, StagePattern& sStage);

    /*ɾ���׶���ʱ��*/
    bool DelStagePattern(Byte uStagePatternId);

     /*ɾ���׶���ʱ��*/
    bool DelStagePattern(Byte uStagePatternId, Byte uStageNo);

	bool DelStagePattern();

    bool IsStagePatternValid(Byte uStagePatternId, Byte uStageNo, StagePattern& sStage);
    
    
/********************************������λ������*****************************************/
    /*��ѯ���еĸ�����λ*/
    bool QueryOverlapPhase(TblOverlapPhase& tblFollow);

    /*������λ�Ų�ѯ*/
    bool QueryOverlapPhase(Byte uOverlapPhase, OverlapPhase& sOverlapPhase);
    
    /*��Ӹ�����λ*/
    bool AddOverlapPhase(Byte uOverlapPhaseId, OverlapPhase& sOverlapPhase);  
    
    /*�޸ĸ�����λ*/
    bool ModOverlapPhase(Byte uOverlapPhaseId, OverlapPhase& sOverlapPhase);

    /*ɾ��������λ��*/
    bool DelOverlapPhase(Byte uOverlapPhaseId);

	bool DelOverlapPhase();

    bool IsOverlapPhaseValid(Byte uOverlapPhaseId, OverlapPhase& sOverlapPhase);
    
/********************************�¼����ͱ�����*****************************************/
    /*��ѯ�¼����ͱ�*/
    bool QueryEventType(TblEventType& tblEvtType);

    /*�����¼����ͺŲ�ѯ*/
    bool QueryEventType(Byte uEvtTypeId, EventType& sEvtType);
    
    /*����¼����ͱ�*/
    bool AddEventType(Byte uEvtTypeId, EventType& sEvtType);  
    
    /*�޸��¼����ͱ�*/
    bool ModEventType(Byte uEvtTypeId, EventType& sEvtType);

    /*ɾ���¼����ͱ�*/
    bool DelEventType(Byte uEvtTypeId);

    /*ɾ���¼����ͱ�*/
    bool DelEventType();
    
    
/********************************�¼���־������*****************************************/
    /*��ѯ�¼���־��*/
    bool QueryEventLog(TblEventLog& tblLog);

    /*�����¼����ͺŲ�ѯ*/
    bool QueryEventLog(Byte uEvtTypeId, TblEventLog& tblLog);

    /*������־ID��ѯ*/
    bool QueryEventLog(Byte uEvtLogId, EventLog& sEvtLog);

	/*��ȡ��־�����������Լ�ʱ����С��id*/
	bool GetLogMaxMin(Uint32* pMaxId, Uint32* pMinId);
    
    /*����¼���־*/
    bool AddEventLog(Uint32* pMaxId, Uint32* pMinId, Byte uEvtType, Ulong uEvtValue);

    /*�����¼�����ɾ���¼���־��*/
    bool DelEventLog(Byte uEvtTypeId);

    /*ɾ����־*/
    bool DelEventLog();
	 /*ɾ����־*/
   bool DelEventLog(Byte uEvtTypeId ,Uint uStartTime ,Uint uEndTime) ;

/******************************ͨ�����ݼ�����ñ�******************************/

/*��ѯͨ�����ݼ�����ñ�*/
bool QueryChannelChk(TblChannelChk & tblChanChk);//(TblEventType& tblEvtType)

/*ɾ��ͨ�����ݼ�����ñ�*/
bool DelChannelChk();

/*���ͨ�����ݼ�����ñ�*/
bool AddChannelChk(Byte uChanChkId, ChannelChk& sChannelChk);

/*�޸�ͨ�����ݼ�����ñ�*/
bool ModChannelChk(Byte uChanChkId, ChannelChk& sChannelChk);

/******************************************************************************/

/******************************����ͳ�Ʊ�����***************************************/
    bool AddVehicleStat(Byte ucDetId , Ulong ulCarTotal , Byte ucOccupancy);

	bool DelVehicleStat(Ulong ulId);

	bool QueryVehileStat(VehicleStat& sVehicleStat);

	bool QueryVehicleStat(TblVehicleStat& tblVehicleStat);

/****************************��λ�뷽���Ӧ������*******************************/
	bool QueryPhaseToDirec(TblPhaseToDirec& tblPhaseToDirec);

	bool QueryPhaseToDirec(Byte ucId, PhaseToDirec& sPhaseToDirec);

	bool AddPhaseToDirec(Byte ucId, PhaseToDirec& sPhaseToDirec);

	bool ModPhaseToDirec(Byte ucId, PhaseToDirec& sPhaseToDirec);

	bool DelPhaseToDirec();

   /*��ȡ�����������*/
    bool GetCommonPara(Ushort& usDeviceId, Byte& ucMaxModule, Byte& usSynchTime, Ushort& usSynchFlag);
   
    /*�޸Ĺ������ò���*/
    bool SetCommonPara(Ushort* usDeviceId, Byte* ucMaxModule, Byte* usSynchTime, Ushort* usSynchFlag);

    /*��ȡ����ʱ�����*/
    bool GetCommTimePara(Long& lZone, Byte& ucMaxPlanCnt, Byte& ucMaxScheduleCnt, Byte& ucMaxEvtCount);
    
    /*�޸Ĺ���ʱ�����*/
    bool SetCommTimePara(Long* lZone, Byte* ucMaxPlanCnt, Byte* ucMaxScheduleCnt, Byte* ucMaxEvtCount);
    
    /*��ȡ�¼�����*/
    bool GetEventPara(Byte& ucMaxEvtType, Byte& ucMaxLogCount);
    
    /*�����¼�����*/
    bool SetEventPara(Byte* ucMaxEvtType, Byte* ucMaxLogCount);

    /*��ȡ��λ����*/
    bool GetPhasePara(Byte& ucMaxPhaseCount);

    /*������λ����*/
    bool SetPhasePara(Byte& ucMaxPhaseCount);

    /*��ȡ�ƿز���*/
    bool GetChannelPara(Byte& ucMaxChannel);

    /*���õƿز���*/
    bool SetChannelPara(Byte& ucMaxChannel);   
    
   /*��ȡ���Ƶ�Ԫ��ز���*/
    bool GetUnitCtrlPara(Byte& ucFlsTimeWhenStart, Byte& ucAllRedTimeWhenStart, Byte& ucRemoteCtrlFlag, Byte& ucFlashFreq);
    
    /*���ÿ��Ƶ�Ԫ��ز���*/
    bool SetUnitCtrlPara(Byte* ucFlsTimeWhenStart, Byte* ucAllRedTimeWhenStart, Byte* ucRemoteCtrlFlag, Byte* ucFlashFreq);
  
    /*��ȡ��ʱ��������*/
    bool GetPatternPara(Byte& ucMaxPatternCount, Byte& ucMaxStgPatternCount, Byte& ucMaxStageCount);
    
    /*������ʱ��������*/
    bool SetPatternPara(Byte* ucMaxPatternCount, Byte* ucMaxStgPatternCount, Byte* ucMaxStageCount);
  
    /*��ȡ��������ʱ��*/
    bool GetGlobalCycle(Byte& ucGlobalCycle);
   
    /*���ù�������ʱ��*/
    bool SetGlobalCycle(Byte ucGlobalCycle);

    /*��ȡЭ����λ��*/
    bool GetCoorPhaseOffset(Byte& ucPhaseOffset);

    /*����Э����λ��*/
    bool SetCoorPhaseOffset(Byte& ucPhaseOffset);
    
    /*��ȡ��������*/
    bool GetDegradeCfg(Byte& ucDegradeMode, Byte& ucDegradePattern);

    /*��ȡ������׼����*/
    bool GetDegradePattern(DbByteArray& tblDegrade);

    /*���ݷ����Ż�ȡ*/
    bool GetDegradePattern(Byte ucDegradeMode, Byte& ucDegradePattern);

    /*���ý�������*/
    bool SetDegradeMode(Byte ucDegradeMode);

    /*���ý�����׼����*/
    bool SetDegradePattern(DbByteArray& tblDegrade);

    /*���ý�����׼����*/
    bool SetDegradePattern(Byte ucDegradeMode, Byte ucDegradePattern);

    /*��ȡ������������*/
    bool GetCtrlMasterOptFlag(Byte& ucCtrlOptFlag);
    
    /*���ÿ�����������*/
    bool SetCtrlMasterOptFlag(Byte ucCtrlOptFlag);
    
     /*��ȡ��·���źŻ�����*/
    bool GetMultiCtrlPara(Ushort& usBaseAddr, Byte& ucDevCount);
    
    /*���ö�·���źŻ�����*/
    bool SetMultiCtrlPara(Ushort* usBaseAddr, Byte* ucDevCount);
    
    /*��ȡ���������֤�ַ���*/
    bool GetEypSerial(char * SysEypSerial);   //201310101344
    /*���ñ��������֤�ַ���*/
    bool SetEypSerial(char *passwd);  //201310101350

     /*��ѯϵͳ��*/
    bool GetSystemData(const char* sField, Ulong& ulData);

    /*����ϵͳ��*/
    bool SetSystemData(const char* sField, Ulong ulData);

    /*��ѯ*/
    bool GetFieldData(const char* sTable, const char* sField, Ulong& ulData);

    /*����*/
    bool SetFieldData(const char* sTable, const char* sField, Ulong ulData);

    /*��ѯ*/
    bool GetFieldSignData(const char* sTable, const char* sField, Long& lData);

    /*����*/
    bool SetFieldSignData(const char* sTable, const char* sField, Long lData);

private:
    /*��ѯ*/
    bool Query(const char* sSql, CSqliteRst& sRst);
    /*ɾ�����޸ĵ�*/
    bool ExecuteCmd(const char* sSql);
    /*���Ƿ����*/
    bool IsTableExist(const char* sTable);
    void AddDefault();

    /*��Ӽ��������*/
    bool AddDetPara(DetectorPara& detPara);

    /*���ϵͳ����*/
    bool IsSysParaValid(const char* sField, Ulong& ulValue);

    bool IsSysParaValid(const char* sField, Long& lValue);
private:
    sqlite3*            m_pSqlite;
    ACE_Thread_Mutex    m_mutexDb;
};
extern TableDesc* m_gTableDesc;
extern CGbtTscDb* m_gTscDb;

int ExchangeData(int iOptType,
                Byte bObjId, 
                Byte uIdxFst, 
                Byte uIdxSnd, 
                Byte uSubId, 
                Byte* pData, 
                int uDataSize,
                Byte& uErrorSts,
                Byte& uErrorIdx);

Byte GetFieldsValue(const char* sConstant, const char uSplit, Byte* sValue, Byte uValSize);

#define UNPACKET_ULONG(sValueSeq, ulValue) \
ulValue = ((Ulong)((sValueSeq)[0])<<24)|((Ulong)((sValueSeq)[1])<<16)|((Ulong)((sValueSeq)[2])<<8)|(sValueSeq)[3]

#define UNPACKET_USHORT(sValueSeq, usValue) \
usValue = ((Ushort)((sValueSeq)[0])<<8)|(sValueSeq)[1]

#define UNPACKET_UCHAR(sValueSeq, ucValue) \
ucValue = sValueSeq[0]

#define PACKET_ULONG(sValueSeq, ulValue) \
(sValueSeq)[0] = (Byte)((ulValue>>24)&0xFF);\
(sValueSeq)[1] = (Byte)((ulValue>>16)&0xFF);\
(sValueSeq)[2] = (Byte)((ulValue>>8)&0xFF);\
(sValueSeq)[3] = (Byte)(ulValue&0xFF);

#define PACKET_USHORT(sValueSeq, usValue) \
(sValueSeq)[0] = (Byte)((usValue>>8)&0xFF);\
(sValueSeq)[1] = (Byte)(usValue&0xFF);

#define PACKET_UCHAR(sValueSeq, ucValue) \
(sValueSeq)[0] = (Byte)(ucValue&0xFF);

#define PACKET_DATA(VALSIZE, SEQ, DATA)\
    switch (VALSIZE)\
    {\
    case 4:\
        PACKET_ULONG(SEQ, DATA);\
        break;\
    case 2:\
        PACKET_USHORT(SEQ, DATA);\
        break;\
    default:\
        PACKET_UCHAR(SEQ, DATA);\
    }

#define UNPACKET_DATA(VALSIZE, SEQ, DATA)\
    switch (VALSIZE)\
    {\
    case 4:\
        UNPACKET_ULONG(SEQ, DATA);\
        break;\
    case 2:\
        UNPACKET_USHORT(SEQ, DATA);\
        break;\
    default:\
        UNPACKET_UCHAR(SEQ, DATA);\
    }
}
#endif

