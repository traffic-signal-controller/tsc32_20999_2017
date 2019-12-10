#ifndef  __COMDEF209992017__H__
#define  __COMDEF209992017__H__
#include "Define.h"
#include "ace/OS.h"
#include "Configure.h"

#define MIX_GB25280FRAME_LEN 0x10       //����Э����̳���16�ֽ�

typedef struct GbDataNode
{
    Byte NodeData;
    GbDataNode *pNext;
}*pList;

/*
    ����GB0999-2017Э��ͨ�ų�ʱ����
*/
enum
{
    TSC_COMMUICATION_TIMEOUT1= 0x32,         //��ʱ5��
    TSC_COMMUICATION_TIMEOUT2= 0x64,         //��ʱ10��        
    TSC_COMMUICATION_TIMEOUT3= 0xC8        //��ʱ20��
};


/*
    ����GB0999-2017Э��ͨ������
*/
enum
{
    TSC_COMMUICATION_TCP= 0x1,         //TCP��ʽͨ��
    TSC_COMMUICATION_UDP= 0x2,         //UDP��ʽͨ��        
    TSC_COMMUICATION_RS232= 0x3        //���ڷ�ʽͨ��
};


/*****************GBT25280Э������־����*************************************************/
enum
{
    OBJECT_ONLINE_GB25280                         = 0x1,     //����
    OBJECT_TRAFFIC_FLOW_GB25280          =0x2,      //��ͨ����Ϣ
    OBJECT_WORKSTATUS_GB25280              =0x3,      //�źŻ�����״̬
    OBJECT_LAMPCOLOR_GB25280                =0x4,      //��ɫ״̬
    OBJECT_CURRENTTIME_GB25280             =0x5,      //��ǰʱ��
    OBJECT_LANPGROUP_GB25280                =0x6,      //�źŵ���
    OBJECT_PHASE_GB25280                           =0x7,   //�ź���λ
    OBJECT_SIGNALPATTERN_GB25280         =0x8,      //�ź���ʱ����
    OBJECT_PLAN_GB25280                            =0x9,      //�������ȼƻ�
    OBJECT_WORKTYPE_GB25280                  =0xA,     //������ʽ
    OBJECT_TSCERROR_GB25280                   =0xB,      //�źŻ�����
    OBJECT_TSCVERSION_GB25280               =0xC,      //�źŻ��汾
    OBJECT_TSCPARAVER_GB25280              =0xD,       //���������汾
    OBJECT_TSCIC_GB25280                       =0xE,       //�źŻ�ʶ����
    OBJECT_REMOTECTRL_GB25280             =0xF,       //�źŻ�Զ�̿���
    OBJECT_DETECTOR_GB25280            =0x10,     //�����
    OBJECT_DETECTOR_OTHER                     =0xFF       // δ֪ ���ڹ����쳣����Ӧ��
};



/*********************************GBT20999-2017Э������֡����ö��**************************/
enum
{
    TYPE_QUERY                =0x10,  //��ѯ
    TYPE_QUERY_REPLY          =0x20,  //��ѯӦ��    
    TYPE_QUERY_ERROR_REPLY    =0x21,  //��ѯ����ظ�
    TYPE_SET                  =0x30,  //����
    TYPE_SET_REPLY            =0x40,  //����Ӧ��
    TYPE_SET_ERROR_REPLY      =0x41,  //���ó���ظ�    
    TYPE_BRAODCAST            =0x50,  //�㲥    
    TYPE_TRAP                 =0x60,  //�����ϱ�    
    TYPE_HEART_SEARCH         =0x70,  //������ѯ    
    TYPE_HEART_REPLY          =0x80  //����Ӧ��
};

/*********************************GBT25280-2016��¼Э�����ݱ�ͷ����***********************/
enum
{
    GB25280TABLE_VERSION           =0x10,  //�����ݰ汾
    GB25280TABLE_SENDFLAG        = 0x10,  //�����ݷ��ͱ��
    GB25280TABLE_RECVFLAG        = 0x20  //�����ݽ��ձ��
};

/*********************************GB0999-2017����ֵ״̬******************************************/
enum
{
    LIGHT_TYPE_VEHICLE     = 0x1,          //����������
    LIGHT_TYPE_NONVEHICLE  = 0x2,          //�ǻ���������
    LIGHT_TYPE_PEDESTRIAN  = 0x3,          //���˵���
    LIGHT_TYPE_ROAD        = 0x4          //��������
   
};


/*********************************GB0999-2017��������******************************************/
enum
{
    STATUS_BADVALUE     = 0x10,          //ֵ����
    STATUS_WRONGLENGTH  = 0x11,          //ֵ���ȴ���  
    STATUS_OVERFLOW     = 0x12,          //ֵԽ��    
    STATUS_READONLY     = 0x20,          //ֵֻ��
    STATUS_NULL         = 0x30,          //ֵ������
    STATUS_ERROR        = 0x40,          //ֵһ�����
    CONTROL_FAIL        = 0x50          //����ʧ��
};

/*********************************GB0999-2017����״̬******************************************/
enum
{
    LIGHT_STATUS_OFF            = 0x1,           //���
    LIGHT_STATUS_RED            = 0x10,          //���
    LIGHT_STATUS_REDFLASH       = 0x11,          //����        
    LIGHT_STATUS_RED_FASTFLASH  = 0x12,          //�����
    LIGHT_STATUS_GREEN          = 0x20,          //�̵�
    LIGHT_STATUS_GREENFLASH     = 0x21,          //����        
    LIGHT_STATUS_GREEN_FASTFLASH  = 0x22,        //�̿���
    LIGHT_STATUS_YELLOW         = 0x30,          //�Ƶ�
    LIGHT_STATUS_YELLOWFLASH    = 0x31,          //����        
    LIGHT_STATUS_YELLOW_FASTFLASH  = 0x32,       //�ƿ���    
    LIGHT_STATUS_REDYELLOW      = 0x40           //��Ƶ�
   
};

/*********************************GB0999-2017���������******************************************/
enum
{
    DETECTOR_COIL            = 0x1,          //��Ȧ
    DETECTOR_VIDEO           = 0x2,          //��Ƶ
    DETECTOR_GEOMAGNETIC     = 0x3,          //�ش�        
    DETECTOR_MICROWAVE       = 0x4,          //΢�������    
    DETECTOR_ULTRASONIC      = 0x5,          //�����������    
    DETECTOR_INFRARED        = 0x6          //�����߼����
   
};

/*********************************GB0999-2017��λ�׶γ�������******************************************/
enum
{
    PHASE_STAGE_STATUS_NOTOFWAY         = 0x10,          //��λ�׶�δ����    
    PHASE_STAGE_STATUS_ONTHEWAY         = 0x20,         //��λ�׶����ڷ���      
    PHASE_STAGE_STATUS_TRANSITON        = 0x30          //��λ�׶ι���  
};

/*********************************GB0999-2017��λ�׶�״̬******************************************/
enum
{
    PHASE_STAGE_TYPE_FIX            = 0x10,          //��λ�׶ι̶�����    
    PHASE_STAGE_TYPE_DEMAND         = 0x20          //��λ�׶ΰ��������   
};

/*********************************GB0999-2017����ģʽ******************************************/
enum
{
    MODE_CENTER_CONTROL               = 0x10,          //���Ŀ���ģʽ
    MODE_CENTER_TIMETABLE_CONTROL     = 0x11,          //�����ռƻ�����    
    MODE_CENTER_OPTIMIZATION_CONTROL  = 0x12,          //�����Ż�����   
    MODE_CENTER_COORDINATION_CONTROL  = 0x13,          //����Э������    
    MODE_CENTER_ADAPTIVE_CONTROL      = 0x14,          //��������Ӧ����
    MODE_CENTER_MANUAL_CONTROL        = 0x15,          //�����ֶ�����
    MODE_LOCAL_CONTROL                = 0x20,          //���ؿ���ģʽ
    MODE_LOCAL_FIXCYCLE_CONTROL       = 0x21,          //���ض����ڿ���    
    MODE_LOCAL_VA_CONTROL             = 0x22,          //���ظ�Ӧ����   
    MODE_LOCAL_COORDINATION_CONTROL   = 0x23,          //����Э������    
    MODE_LOCAL_ADAPTIVE_CONTROL       = 0x24,          //��������Ӧ����
    MODE_LOCAL_MANUAL_CONTROL         = 0x25,          //�����ֶ�����
    MODE_SPECIAL_CONTROL              = 0x30,          //�������ģʽ
    MODE_LOCAL_FLASH_CONTROL          = 0x31,          //��������    
    MODE_LOCAL_ALLRED_CONTROL         = 0x32,          //ȫ�����   
    MODE_LOCAL_ALLOFF_CONTROL         = 0x33           //�صƿ���  
};



/*********************************GB0999-2017��������******************************************/
enum
{
    TYPE_ALARM_LIGHT        = 0x10,          //�źŵƱ���
    TYPE_ALARM_DETECTOR     = 0x30,          //���������
    TYPE_ALARM_DEVICE       = 0x40,          //�豸���ϱ���
    TYPE_ALARM_ENVIRONMENT  = 0x60           //���������쳣����
   
};

/*********************************GB0999-2017��������******************************************/
enum
{
    TYPE_FAULT_GREENCONFICT       = 0x10,          //�̳�ͻ����
    TYPE_FAULT_GREENREDCONFICT    = 0x11,          //���̳�ͻ����
    TYPE_FAULT_REDLIGHT           = 0x20,          //��ƹ���
    TYPE_FAULT_YELLOWLIGHT        = 0x21,          //�Ƶƹ���
    TYPE_FAULT_GREENLIGHT         = 0x22,          //�̵ƹ���
    TYPE_FAULT_COMMUNICATION      = 0x30,          //ͨ�Ź���
    TYPE_FAULT_SELF               = 0x40,          //�Լ����
    TYPE_FAULT_DETECTOR           = 0x41,          //���������
    TYPE_FAULT_REALY              = 0x42,          //�̵�������
    TYPE_FAULT_MEMORY             = 0x43,          //�洢������
    TYPE_FAULT_CLOCK              = 0x44,          //ʱ�ӹ���
    TYPE_FAULT_MOTHERBOARD        = 0x45,          //�������
    TYPE_FAULT_PHASEBOARD         = 0x46,          //��λ�����
    TYPE_FAULT_DETECTORBOARD      = 0x47,          //����������
    TYPE_FAULT_CONFIG             = 0x50,          //���ù���
    TYPE_FAULT_RESPONSE           = 0x70           //������Ӧ����
   
};

/*********************************GB0999-2017���϶���******************************************/
enum
{
    SWTICH_NULL                   = 0x0,           //�޹��϶���
    SWTICH_TO_FLASH               = 0x10,          //�л�������    
    SWTICH_TO_OFF                 = 0x20,          //�л����ص�
    SWTICH_TO_RED                 = 0x30,          //�л���ȫ��
    SWTICH_TO_LOCAL_FIXCYCLE      = 0x40,          //�л������ض�����
    SWTICH_TO_LOCAL_COORDINATION  = 0x50,          //�л�������Э��
    SWTICH_TO_LOCAL_VA            = 0x60           //�л������ظ�Ӧ
}; 


/*********************************GB0999-2017Э������ֵ******************************************/
enum
{
    ORDER_FLASH         = 0x1,         //����
    ORDER_RED           = 0x2,         //ȫ��    
    ORDER_ON            = 0x3,         //����
    ORDER_OFF           = 0x4,         //�ص�
    ORDER_RESET         = 0x5,         //����
    ORDER_CANCEL        = 0x0          //ȡ������
}; 


/*********************************GB0999-2017Э�������ඨ��******************************************/
enum
{
    DATACLASS_DEVICE_INFO        = 0x1,         //�豸��Ϣ    
    DATACLASS_BASIC_INFO         = 0x2,         //������Ϣ
    DATACLASS_LIGHT_INFO         = 0x3,         //������Ϣ
    DATACLASS_PHASE_INFO         = 0x4,         //��λ��Ϣ
    DATACLASS_DETECTOR_INFO      = 0x5,         //�������Ϣ
    DATACLASS_PHASESTAGE_INFO    = 0x6,         //��λ�׶���Ϣ
    DATACLASS_PHASESECURY_INFO   = 0x7,         //��λ��ȫ��Ϣ
    DATACLASS_EMERGENCYPRIORITY  = 0x8,         //��������
    DATACLASS_PLAN_INFO          = 0x9,         //������Ϣ
    DATACLASS_TRANSITIONCONSTRAIN  = 0xA,       //����Լ��
    DATACLASS_DAILYPLAN          = 0xB,         //�ռƻ�
    DATACLASS_SCHEDULETABLE      = 0xC,         //���ȱ�
    DATACLASS_RUNSTATUS          = 0xD,         //����״̬
    DATACLASS_TRAFFICDATA        = 0xE,         //��ͨ����
    DATACLASS_ALARMDATA          = 0xF,         //��������
    DATACLASS_FAULTDATA          = 0x10,        //��������
    DATACLASS_CENTERCONTROL      = 0x11,        //���Ŀ���
    DATACLASS_PIPLECOMMAND       = 0x12         //�ܵ�����
}; 

/************************************�źŻ�GB25280-2016��¼Э���źŻ����ýṹ�嶨��*/
typedef struct SGB25280TscConfig
{
    Byte  TscCommunicationStatus ;//�źŻ�ͨ��״̬
    int   TscAreaCode ;                      //�źŻ������
    int TscCrossCode ;                      //�źŻ�·�ں�
    int iPort ;                                     //ͨ�Ŷ˿�
    bool bGetQueryResponse ;       //�Ƿ��յ���λ�˷��͵�������ѯӦ��
    bool bUpTrafficFlow ;                //�Ƿ�����ͨ�����ϴ�
    bool bUpTscVerUp ;                  //�Ƿ����źŻ��汾�����ϴ�
    bool bUpTscError ;                    //�Ƿ����źŻ����������ϴ�
    bool bUpLampColor;                //�Ƿ����źŻ���ɫ״̬�����ϴ�
    bool bUpTscWorkStatus ;         //�Ƿ����źŻ�����״̬�����ϴ�
    ACE_TString  TscClientIp ;        //��λ��IPv4��ַ
    ACE_TString  IdentifyCode;       //����ָ�����ָ����֤��
} sGB25280TscConfig;
;
#endif
