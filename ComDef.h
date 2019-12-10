#ifndef __COMDEF__H__
#define __COMDEF__H__

#include "Define.h"

/*
Can�������� ռ��IDʶ��� bit26-bit28
*/
enum
{
	CAN_MSG_TYPE_000 = 0, //���ʵʱ��  ��������ʹ��
	CAN_MSG_TYPE_001 = 1, //��ɫ���̳�ͻ�����ݼ��
	CAN_MSG_TYPE_010 = 2, //��ɫ���̳�ͻ�����ݼ��
	CAN_MSG_TYPE_011 = 3, //��ɫ���̳�ͻ�����ݼ��
	CAN_MSG_TYPE_100 = 4, //������ݡ��ֿ����ݺ͵�Դ������
	CAN_MSG_TYPE_101 = 5, //�������ݵķ��ͣ�����������״̬����������ٰ�
	CAN_MSG_TYPE_110 = 6, //û��ʵʱ��Ҫ��ϵͳ��������ģ������
	CAN_MSG_TYPE_111 = 7  //��ϵͳ�б�����
};

/*
*ģ���ַ ռ��IDʶ���bit20-bit25 Ŀ�ĵ�ַ ռ��IDʶ���bit12-bit17
*/
enum
{
	BOARD_ADDR_MAIN         = 0x10 ,  //���ذ�
	BOARD_ADDR_MAINBACK     = 0x11 ,  //���屸�ݵ�Ƭ��
	BOARD_ADDR_LAMP1        = 0x13 ,  //�ƿذ�1
	BOARD_ADDR_LAMP2        = 0x14 ,  //�ƿذ�2
	BOARD_ADDR_LAMP3        = 0x15 ,  //�ƿذ�3
	BOARD_ADDR_LAMP4        = 0x16 ,  //�ƿذ�4
	BOARD_ADDR_LAMP5        = 0x17,   //��չ�ƿذ�1	
	BOARD_ADDR_LAMP6        = 0x18,   //��չ�ƿذ�2 
	BOARD_ADDR_ALLLAMP      = 0x19 ,  //�ƿذ��鲥
	
	BOARD_ADDR_LAMP7        = 0x1a,   //��չ�ƿذ�3 	
	BOARD_ADDR_LAMP8        = 0x1b,   //��չ�ƿذ�4 
	BOARD_ADDR_LAMP9        = 0x1c,   //��չ�ƿذ�5 
	BOARD_ADDR_LAMPa        = 0x1d,   //��չ�ƿذ�6
	BOARD_ADDR_LAMPb        = 0x1e,   //��չ�ƿذ�7 
	BOARD_ADDR_LAMPc        = 0x1f,   //��չ�ƿذ�8
		
	BOARD_ADDR_POWER        = 0x20 ,  //��Դģ��	
	BOARD_ADDR_POWER2       = 0x22 ,  //��Դģ��2
	BOARD_ADDR_DETECTOR1    = 0x24 ,  //�����1
	BOARD_ADDR_DETECTOR2    = 0x25 ,  //�����2
	BOARD_ADDR_ALLDETECTOR  = 0x26 ,  //������鲥
	BOARD_ADDR_INTEDET1     = 0x29 ,  //������ӿ�1
	BOARD_ADDR_INTEDET2     = 0x2A ,  //������ӿ�2
	BOARD_ADDR_ALLINTEDET   = 0x2B ,  //������ӿڰ��鲥
	BOARD_ADDR_FLASH        = 0x2E ,  //Ӳ��������
	BOARD_ADDR_HARD_CONTROL = 0x30 ,  //Ӳ������ģ��
	BOARD_ADDR_LED          = 0x32 ,  //��ʾ���� ADD:20130808 15 50
	BOARD_ADDR_WIRELESS_BTNCTRLA = 0x33 , //����ң����A	���ͨ�Ű棨15����315M��//ADD:20141021
	BOARD_ADDR_WIRELESS_BTNCTRLB = 0x34   //����ң����B	�߼��棬һ�Զ�ң�ؼ�����//ADD:20141021
	
};

/*
*֡ģʽ  ռ��IDʶ���bit18-bit19
*/
enum
{
	FRAME_MODE_P2P        = 0 , //��Ե�
	FRAME_MODE_MULTICAST  = 1 , //�鲥
	FRAME_MODE_BROADCAST  = 2 , //�㲥
	FRAME_MODE_HEART_BEAT = 3   //�㲥����
};


/*
*GBTӦ�ò�Э����������
*/
enum
{
	DATA_HEAD_NOREPLY = 0 , //ͨ�����ݣ�����Ҫ�ظ�
	DATA_HEAD_RESEND  = 1 , //������
	DATA_HEAD_CHECK   = 2 , //ȷ������
	DATA_HEAD_OTHER   = 3   //δ���壬����
};

const int DET_STAT_CYCLE = 60;   //ռ����ͳ�Ƶ����� 1����
enum
{
	DETECTOR_INTERFACE_BOARD1 = 0,  //������ӿڰ�1��ַ
	DETECTOR_INTERFACE_BOARD2 = 1 ,  //������ӿڰ�2��ַ
	DETECTOR_BOARD1           = 2  ,  //�������1
	DETECTOR_BOARD2           = 3     //�������2
};






/************************************************************************************************/
/*
*�źŻ�ϵͳ����ֵ���壬��ӦGBTЭ�鲿�ֳ�������
*/
const int MAX_TIMEGROUP          = 40;   //���ʱ�����ȱ���
const int MAX_SCHEDULE_PER_DAY   = 48;   //ÿ�����ʱ����
const int MAX_TIMEPATTERN        = 32;   //������ʱ������
const int MAX_SCHEDULETIME_TYPE  = 17;   //1-16 �û����壬17�������ⷽ�� �׶���ʱ��������???
const int MAX_SON_SCHEDULE       = 16;   //���׶���ʱ������ӽ׶���
const int MAX_PHASE              = 32;   //�����λ����
const int MAX_OVERLAP_PHASE      = 16;    //��������λ����
const int MAX_CONFLICT_PHASE     = 32;   //����ͻ����
const int MAX_STATUSOUT_PER      = 8;    //ÿ��״̬��������ĸ���
const int MAX_STEP               = 64;   //���Ĳ�����
const int MAX_LAMP_BOARD         = 8;    //����������
const int MAX_LAMP_NUM_PER_BOARD = 12;   //ÿ���ĵƾ�����
const int MAX_LAMPGROUP_PER_BOARD =4 ;
const int MAX_CHANNEL            = MAX_LAMP_BOARD * 4;                        //���ͨ�����ź��飩���� 1��4ͨ��
const int MAX_LAMP                   = MAX_LAMP_BOARD * MAX_LAMP_NUM_PER_BOARD;   //���ƾ���  1ͨ��3�ƾ� 1��12�ƾ�
const int MAX_DET_BOARD          = 4;    //��󳵼��(�ӿڰ�)��Ŀ
const int MAX_DETECTOR_PER_BOARD = 16;   //ÿ�������ļ��������
const int MAX_INTERFACE_PER_BOARD =32 ;  //ÿ��ӿڰ������ͨ������
const int MAX_DETECTOR           = (MAX_DETECTOR_PER_BOARD+MAX_INTERFACE_PER_BOARD)*MAX_DET_BOARD/2;   //�����������

const int MAX_SPESTATUS_CYCLE    = 10;   //ʱ�α��ﶨ�������״̬����ʱ�� 
const int MIN_GREEN_TIME	         = 7;    //��С�̵�ʱ��

const int MAX_REGET_TIME         = 10;   //100ms 1per 1s=10times
const int USLEEP_TIME            = 2000;
const int MAX_WORK_TIME          = 3;    //3��һ���Ĳ�������
const int BOARD_REPEART_TIME     = 5;    //��״̬�жϵ��ظ�����
const int MAX_DREC               = 80;  //���������   201310181705->201401031017 8������ÿ������10��֧��
const Byte MAX_CNTDOWNDEV        = MAX_PHASE ;  //֧�����ĵ���ʱ�豸��32,��λ����ʱ
const Byte MAX_MODULE            =50 ;  //����豸ģ���� 20150112
const Byte MAX_POWERBOARD        =2 ;    //����Դ������
/************************************************************************************************/

/*
�źŻ��Ĺ���ģʽ
*/
enum
{
	MODE_TSC  = 0 , //tscģʽ
	MODE_PSC1 = 1 , //һ�ι���ģʽ
	MODE_PSC2 = 2 , //���ι���ģʽ
	MODE_OTHER =3   //��������ģʽ
};



/*
�źŻ��Ĺ���״̬
*/
enum
{
	SIGNALOFF = 0,  //�ص�
	ALLRED       ,  //ȫ��
	FLASH        ,  //����
	STANDARD        //��׼
};

/*
�źŻ��Ŀ���ģʽ
*/
enum
{
	CTRL_UNKNOWN         = 0  ,  //δ֪����״̬
	CTRL_SCHEDULE        = 1  ,  //��ʱ��
	CTRL_UTCS            = 2  ,  //ϵͳ�Ż���������
	CTRL_WIRELESS        = 3  ,  //�޵�����Э��
	CTRL_LINE            = 4  ,  //���ߵ���Э��
	CTRL_HOST            = 5  ,  //�����߿�
	CTRL_MANUAL          = 6  ,  //�ֶ�����
	CTRL_VEHACTUATED     = 7  ,  //����ȫ��Ӧ
	CTRL_MAIN_PRIORITY   = 8  ,  //�����������Ȱ��Ӧ
    CTRL_SECOND_PRIORITY = 9  ,  //����������Ȱ��Ӧ
	CTRL_ACTIVATE        = 10 ,  //����Ӧ
	CTRL_PANEL           = 11 ,  //������	
	CTRL_SCHEDULE_OFF    = 12 ,  //ʱ�α�ص�	
	CTRL_SCHEDULE_FLASH  = 13 ,  //ʱ�α����	
	CTRL_SCHEDULE_RED    = 14 ,  //ʱ�α�ȫ��	
	CTRL_PREANALYSIS     = 15 ,
	CTRL_LAST_CTRL       = 16    //�ϴεĿ��Ʒ�ʽ
};

/*
����������LCD ����ԭ��20141106
*/
enum
{
	CTRLBOARD_FLASH_NORMAL 		 = 0x0  , //��������»���	
	CTRLBOARD_FLASH_MANUALCTRL	 = 0x01 , //�ֿػ���	
	CTRLBOARD_FLASH_NOPATTERN	 = 0x03 , //�޷�������
	CTRLBOARD_FLASH_LAMPCHECK    = 0x02 , //���ݼ�����
	CTRLBOARD_FLASH_DOWNGRADE 	 = 0x04 , //��������
	CTRLBOARD_FLASH_FORCEFLASH	 = 0x05 , //������ǿ�ƻ���
	CTRLBOARD_FLASH_PHASECONFLIC = 0x06 , //��λ��ͻ
	
};

/*
�źŻ����ƶ�����Ϣö������
*/
enum
{
	TSC_MSG_SWITCH_STATUS = 0,  //����״̬�л�
	TSC_MSG_SWITCH_CTRL      ,  //���Ʒ�ʽ�л�
	TSC_MSG_UPDATE_PARA      ,  //���ݿ�õ�����
	TSC_MSG_NEXT_STEP        ,  //��������
	TSC_MSG_OVER_CYCLE       ,  //���ڽ���
	TSC_MSG_LOCK_STEP        ,  //��������
	TSC_MSG_LOCK_PHASE       ,  //������λ
	TSC_MSG_LOCK_STAGE       ,  //�����׶�
	TSC_MSG_NEXT_STAGE       ,  //��һ���׶�
	TSC_MSG_STATUS_READ      ,  //״̬��ȡ
	TSC_MSG_EXSTATUS_READ    ,  //��չ�����ȡ
	TSC_MSG_EVENT_READ       ,  //�¼���ȡ
	TSC_MSG_LOG_WRITE        ,  //��־��¼
	TSC_MSG_CORRECT_TIME     ,  //ʱ��Уʱ  SpeTimePattern
	TSC_MSG_TIMEPATTERN      ,  //�ض���ʱ�䷽��
	TSC_MSG_GREENCONFLICT    ,   //�̳�ͻ
	TSC_MSG_PATTER_RECOVER   ,    //�ض������л�
	TSC_MSG_MANUALBUTTON_HANDLE   //�����ֿذ�������ADD:201411051548
};

/*
ͨ�Ž���gbt���������Ϣ����ö��
*/
enum
{
	GBT_MSG_FIRST_RECV = 0    ,  //�״ν���
	GBT_MSG_DEAL_RECVBUF      ,  //������յ���BUF
	GBT_MSG_SEND_HOST         ,  //������λ��
	GBT_MSG_TSC_STATUS        ,  //�õ��źŻ�״̬
	GBT_MSG_TSC_EXSTATUS      ,  //�õ��źŻ���չ״̬
	GBT_MSG_SELF_REPORT       ,  //�����ϱ�
	GBT_MSG_OTHER_OBJECT      ,  //gbtЭ����������Ͷ���
	GBT_MSG_EXTEND               //��չ����
};




/*
�豸״̬����ö��
*/
enum
{	
	DEV_IS_GOOD      = 0,  //����

	DEV_ALWAYS_ON    = 1,  //����
	DEV_ALWAYS_OFF   = 2,  //����

	DEV_SILICON_SHOT = 3,  //�ɿع����

	DEV_ALWAYS_ON_CLEAR,
	DEV_ALWAYS_OFF_CLEAR,

	DEV_IS_BROKEN,
	DEV_IS_SHORT_CIRCUIT,

	DEV_IS_BROKEN_CLEAR,
	DEV_IS_SHORT_CIRCUIT_CLEAR,

	DEV_IS_CONNECTED,
	DEV_IS_DISCONNECTED
};

/*
*���ذ���ʾ��״̬����ö��
*/
enum
{
	LED_OP_START   = 0 ,
	
	LED_RUN_ON     = 1 ,
	LED_RUN_OFF    = 2 ,

	LED_GPS_ON     = 3 ,
	LED_GPS_OFF    = 4 ,

	LED_RS485_ON   = 5 , 
	LED_RS485_OFF  = 6 ,
	LED_AUTO_SLEF, 
	LED_AUTO_MANUAL,
	LED_TSCPSC_TSC,
	LED_TSCPSC_PSC,
	LED_MODE_GREEN,
	LED_MODE_YELLOW,
	LED_MODE_RED
};

/*
*����9λͨ�Ź���ö��
*/
enum
{
	com_9bit_save_old 	    = 0x11,
	com_9bit_enable         = 0x12,
	com_9bit_disable        = 0x13,
	com_9bit_restore_old    = 0x14
};

/*
*ͨ������ö��
*/
enum
{
	CHANNEL_OTHER   = 1 ,  //��������ͨ��
	CHANNEL_VEHICLE = 2 ,  //������ͨ��
	CHANNEL_FOOT    = 3 ,  //����ͨ��
	CHANNEL_OVERLAP = 4    //������λͨ��
};

/*����ʱ��������*/
enum
{
	CNTDOWN_15 = 1 ,      //����15�뵹��ʱ
	CNTDOWN_8  = 2,       //8�뵹��ʱ
	CNTDOWN_NORMAL       //��ͨ����ʱ
};

/*
*�źŻ��ֶ���������
*Date:201410211730
*/
enum
{
	Manual_CTRL_NO               = 0 ,	  //�������ֿ�״̬
	Manual_CTRL_PANEL            = 1 ,	  //����ֿ�
	Manual_CTRL_WIRELESSBUTTONS  = 2 , 	  //����ң����
	Manual_CTRL_SYSTEM 	         = 3      //��λ������
};


/*
*�źŻ��ض���������ö��
*/
enum
{
	FUN_SERIOUS_FLASH  = 0  , //���ع��ϻ���
	FUN_COUNT_DOWN     = 1  , //��������ʱ
	FUN_GPS            = 2  , //gps����
	FUN_MSG_ALARM      = 3  , //���ű���
	FUN_CROSS_TYPE     = 4  , //���ַ�ʽ 0-tsc 1-one psc 2-twice psc
	FUN_STAND_STAGEID  = 5  , //�����׶κ�
	FUN_CORSS1_STAGEID = 6  , //����ͨ�н׶κ�1
	FUN_CROSS2_STAGEID = 7  , //����ͨ�н׶κ�2
	FUN_TEMPERATURE    = 8  , //�¶ȼ��û�
	FUN_VOLTAGE        = 9  , //��ѹ��⿪��
	FUN_DOOR           = 10 , //�ſ���
	FUN_COMMU_PARA     = 11 , //ͨ�Žӿ�
	FUN_PORT_LOW       = 12 , //�˿ںŵ��ֽ�
	FUN_PORT_HIGH      = 13 , //�˿ںŸ��ֽ�
	FUN_PRINT_FLAG     = 14 , //��ӡ��־  0����� 1����Դ��   2�յ�Դ�� 3���ƿذ� 4�յƿذ� 5�������� 6�ջ����� 7������Ϣ
	FUN_PRINT_FLAGII   = 15 , //��ӡ��־  0����ʱ 1StartTime 2CostTime
	FUN_CAM			   = 16 , // ����ͷ
	FUN_3G             = 17 , //3Gͨ��
	FUN_WIRELESSBTN	   = 18 , //�����ֿ�
	FUN_CNTTYPE	       = 19 ,  //����ʱ����
	FUN_LIGHTCHECK	   = 20 ,	//���ݼ�⿪��	
	FUN_GPS_INTERVAL   = 21	,	//GPS��ʱ����ʱ�� 1 ��ʾÿ�죬2��ʾÿ2��
	FUN_WIRELESSBTN_TIMEOUT = 22, //�����ֿذ����ֶ����Ƴ�ʱʱ�䵥λ���� ADD:201410231639	
	FUN_CROSSSTREET_TIMEOUT = 23, //���˰�ť��ʱ�� ADD:201501091738
	FUN_RS485_BITRATE       =24 , //485����ʱ������ 0-9600 1-2400-2-4800 3-38400   4-15200
	
	FUN_FLASHCNTDOWN_TIME   =25,  //����ʽ����ʱ����ʱ�� 0-0ms 1-50ms 2-100ms,�Դ�����. //ADD 20150605
	FUN_COUNT          = 26      // �ܵ��ض���������ֵ
};

/*
*�źŻ�����ʱ����
*Date:201503251001
*/
enum
{
	COUNTDOWN_STUDY                    = 0 ,	  //ѧϰʽ����ʱ
	COUNTDOWN_GAT5082004         	   = 1 ,	  //ͨѶʽ����ʱ����GAT508-2004 ,�̶�֧��4������ʱ��ÿ������һ������ʱ
	COUNTDOWN_FLASHOFF 	     		   = 2 ,   	 //����ʽ����ʱ ������ָ��������壬֧��32������ʱ
	COUNTDOWN_GAT5082014  	 	       = 3 ,  	 //ͨѶʽ����ʱ����GAT508-2004���֧��32������ʱ
	COUNTDOWN_GAT5082014V2   		   = 4 ,   	//ͨѶʷ����ʱ����GAT508-2004����֧��4����
	COUNTDOWN_GAT5082004V2    		   = 5      //ͨѶʽ����ʱ����GAT508-2004 ��֧��8������ʱ
};

/*
*�źż���־����ö��
*/
enum 
{
	LOG_TYPE_LAMP         = 0 ,  //�źŵ�
	LOG_TYPE_GREEN_CONFIG = 1 ,  //�̳�ͻ
	LOG_TYPE_DETECTOR     = 2 ,  //�����
	LOG_TYPE_VOLTAGE      = 3 ,  //��ѹ
	LOG_TYPE_TEMPERATURE  = 4 ,  //�¶�
	LOG_TYPE_POWERBOARD   = 5 ,  //��Դ��
	LOG_TYPE_LAMPBOARD    = 6 ,  //�ƿذ�
	LOG_TYPE_DETBOARD     = 7 ,  //�������
	LOG_TYPE_FLASHBOARD   = 8 ,  //��������
	LOG_TYPE_REBOOT       = 9 ,  //��������
	LOG_TYPE_CORRECT_TIME = 10,  //ϵͳʱ���޸�
	LOG_TYPE_DOOR_WARN    = 11,  //�ű���
	LOG_TYPE_MANUAL       = 12,  //�����ֶ�
	LOG_TYPE_OUTLAMP_ERR  = 13,  //��ɫ����쳣
	LOG_TYPE_OUTLAMP_ERR2 = 14,  //��ɫ����쳣2
	LOG_TYPE_OTHER        = 15,
	LOG_TYPE_CAN                //CAN����     
};

/*************************GBTЭ����Ϣ����������************************/
const int MAX_GBT_MSG_LEN        = 484;   //gbtЭ����Ϣ����󳤶�
const int MIN_GBT_MSG_LEN        = 3;     //gbtЭ����Ϣ����С����
const int MAX_CLIENT_NUM         = 0xFF;     //���Ŀͻ���������
const int DEFAULT_GBT_PORT       = 5435;  //Ĭ�Ͽ��ٵĶ˿ں�
const int DEFAULT_BROADCAST_PORT = 8808;  //Ĭ�Ͽ��ٵĶ˿ں�
const int MAX_GBT_PORT           = 10024; //���˿ں�    
const int MIN_GBT_PORT           = 1024;  //��С�˿ں�
const int MAX_BUF_LEN            = 8192;  //���֡�ĳ���
/*************************GBTЭ����Ϣ����������************************/

/*
GBTЭ�����ݰ�����ö��
*/
enum 
{
	GBT_SEEK_REQ      = 0,  //��ѯ����
	GBT_SET_REQ       = 1,  //��������
	GBT_SET_REQ_NOACK = 2,  //�������󣬵�����ȷ��Ӧ��

	GBT_SELF_REPORT   = 3,  //�����ϱ�

	GBT_SEEK_ACK      = 4,  //��ѯӦ��
	GBT_SET_ACK       = 5,  //����Ӧ��
	GBT_ERR_ACK       = 6,  //����Ӧ��

	GBT_OTHER_ACK     = 7   //��������
};



/*
GBT��������ö��
*/
enum
{
	GBT_ERROR_LONG     = 1,  //��Ϣ̫��
	GBT_ERROR_TYPE        ,  //��Ϣ����
	GBT_ERROR_OBJECT_VALUE,  //����ֵ������Χ
	GBT_ERROR_SHORT       ,  //��Ϣ̫��
	GBT_ERROR_OTHER          //��������
};
/*
GBЭ������
*/
enum
{
	GBT20999     = 0x1,  //
	GB25280       =0x2     ,  //
	
};

/*
�źŵ���ɫ����ö��
*/
enum
{
	LAMP_COLOR_RED = 0,  //��ɫ��
	LAMP_COLOR_YELLOW ,  //��ɫ��
	LAMP_COLOR_GREEN     //��ɫ��
};


const int MAX_ADJUST_CYCLE = 3;
const int MAX_PLUS_SCALE   = 30;  //һ���������ӵ������� 
const int MAX_MINU_SCALE   = 20;  //ÿ�����ڼ��ٵ�������
const int PLUS_LINE        = 50;  //���ӵĽ���





#endif  //__COMDEF__H__
