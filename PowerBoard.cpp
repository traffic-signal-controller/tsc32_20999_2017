#include "Can.h"
#include "PowerBoard.h"

#ifndef WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#endif

/*
��Դģ�鹦������ö��
*/
enum
{
	POWER_HEAD_ENV      = 0x02 ,  //��ѹ���ݣ���ѹ״̬
	POWER_HEAD_CFG_GET  = 0x03 ,  //�����Դģ���������ݣ�������ѹ�ߵ�ѹԤ����ѹֵ�Լ�ǿ�������Ԥ��
    POWER_HEAD_CFG_SET  = 0x04 ,  //���͵�Դģ����������
	POWER_HEAD_HEARBEAT = 0x05 , //������
	POWER_HEAD_VER      = 0xff  //����汾
};

/*
��Դģ��Ԥ����������ö��
*/
enum
{
	PLAN_KEEP  = 0 ,  //Ԥ����ʽ����
	PLAN_ON    = 1 ,  //��������
	PLAN_OFF   = 2 ,  //�����ر�
	PLAN_OTHER = 3    //����
};

/**************************************************************
Function:       CPowerBoard::CPowerBoard
Description:    CPowerBoard�๹�캯��		
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
CPowerBoard::CPowerBoard()
{
	m_iStongVoltage     = 220;
	m_iWeakVoltage = 5;
	m_iBusVoltage  = 0;
	ACE_DEBUG((LM_DEBUG,"%s:%d Init PowerBoard object ok !\n",__FILE__,__LINE__));
}

Byte CPowerBoard::iHeartBeat = 0;

/**************************************************************
Function:       CPowerBoard::~CPowerBoard
Description:    CPowerBoard����������		
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
CPowerBoard::~CPowerBoard()
{	
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct PowerBoard object ok !\n",__FILE__,__LINE__));
}


/**************************************************************
Function:       CPowerBoard::CreateInstance
Description:    ������Դ���ྲ̬����		
Input:          ��              
Output:         ��
Return:         CPowerBoard��̬����ָ��
***************************************************************/
CPowerBoard* CPowerBoard::CreateInstance()
{
	static CPowerBoard cPowerBoard; 
	return &cPowerBoard;
}


/**************************************************************
Function:       CPowerBoard::GetPowerBoardCfg
Description:    ���ذ������Դģ�鷢�͵�ѹ���ݣ���ѹ״̬		
Input:          ��              
Output:         ��
Return:         0
***************************************************************/
void CPowerBoard::CheckVoltage()
{
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN , FRAME_MODE_P2P  , BOARD_ADDR_POWER , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND << 6 ) | POWER_HEAD_ENV;
	sSendFrameTmp.ucCanDataLen = 1;

	Can::CreateInstance()->Send(sSendFrameTmp);

}


/**************************************************************
Function:       CPowerBoard::GetPowerBoardCfg
Description:    ���ذ������Դģ�鷢���������ݣ�������ѹ�ߵ�ѹԤ
				����ѹֵ�Լ�ǿ�������Ԥ��			
Input:          ��              
Output:         ��
Return:         0
***************************************************************/
void CPowerBoard::GetPowerBoardCfg()
{
	SCanFrame sSendFrameTmp;
	//SCanFrame sRecvFrameTmp;

	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	//ACE_OS::memset(&sRecvFrameTmp , 0 , sizeof(SCanFrame));

	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN
				  , FRAME_MODE_P2P   , BOARD_ADDR_POWER
				  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND << 6 ) | POWER_HEAD_CFG_GET;
	sSendFrameTmp.ucCanDataLen = 1;

	Can::CreateInstance()->Send(sSendFrameTmp);

	
}


/**************************************************************
Function:       CPowerBoard::SetPowerBoardCfg
Description:    ���ذ����õ�Դ����������				
Input:          ��              
Output:         ��
Return:         0
***************************************************************/
void CPowerBoard::SetPowerBoardCfg()
{
	SCanFrame sSendFrameTmp;
	//SCanFrame sRecvFrameTmp;

	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	//ACE_OS::memset(&sRecvFrameTmp , 0 , sizeof(SCanFrame));

	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN
				  , FRAME_MODE_P2P   , BOARD_ADDR_POWER
				  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND << 6 ) | POWER_HEAD_CFG_SET;
	sSendFrameTmp.pCanData[1] = m_iSetWarnHighVol;
	sSendFrameTmp.pCanData[2] = m_iSetWarnLowVol ;
	sSendFrameTmp.pCanData[3] |= m_ucSetStongHighVolPlan;
	sSendFrameTmp.pCanData[3] |= m_ucSetStongLowVolPlan << 2;
	sSendFrameTmp.pCanData[3] |= m_ucSetWeakHighVolPlan << 4;
	sSendFrameTmp.pCanData[3] |= m_ucSetWeakLowVolPlan << 6;
	sSendFrameTmp.pCanData[4] = m_ucSetWatchCfg ;
	sSendFrameTmp.ucCanDataLen = 5;

	Can::CreateInstance()->Send(sSendFrameTmp);

	
}
/**************************************************************
Function:       CPowerBoard::SetPowerCfgData
Description:    ���ذ����õ�Դ���������ݲ���				
Input:          WarnHighVol -�ߵ�ѹԤ��ֵ
		     WarnLowVol  -�͵�ѹԤ��ֵ
		     VolPlan         -��ѹ���Ʒ���
		     Output:         ��
Return:         0
***************************************************************/

void CPowerBoard::SetPowerCfgData(Byte m_ucWarnHighVol,Byte m_ucWarnLowVol,Byte m_ucVolPlan,Byte m_ucDogCfg)
{
	m_iSetWarnHighVol = m_ucWarnHighVol  ;
	m_iSetWarnLowVol  = m_ucWarnLowVol  ;
	m_ucSetStongHighVolPlan = m_ucVolPlan &0x03 ;
	m_ucSetStongLowVolPlan  = (m_ucVolPlan>>2) &0x03 ;
	m_ucSetWeakHighVolPlan  = (m_ucVolPlan>>4) &0x03 ;
	m_ucSetWeakLowVolPlan   = (m_ucVolPlan>>6) &0x03 ;
	m_ucSetWatchCfg         =  m_ucDogCfg ;
	return ;
}

/**************************************************************
Function:       CPowerBoard::HeartBeat
Description:    ��Դ�巢�͹㲥����Can���ݰ������ڱ�������Ͳ�����
				����״̬
Input:          ��              
Output:         ��
Return:         0
***************************************************************/
void CPowerBoard::HeartBeat()
{
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	
	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN , FRAME_MODE_HEART_BEAT  , BOARD_ADDR_POWER , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_NOREPLY << 6 ) | POWER_HEAD_HEARBEAT;
	sSendFrameTmp.ucCanDataLen = 1;
	
	Can::CreateInstance()->Send(sSendFrameTmp);
	
}

/**************************************************************
Function:        CPowerBoard::RecvPowerCan
Description:    ���ذ���յ�Դ�巵��Can���ݰ�������������				
Input:          ucBoardAddr  ��Դ���ַ
				sRecvCanTmp  Can���ݰ�              
Output:         ��
Return:         0
***************************************************************/
void CPowerBoard::RecvPowerCan(Byte ucBoardAddr,SCanFrame sRecvCanTmp)
{
		Byte PowerBdIndex = ((ucBoardAddr == 0x20)?0:1);
		Byte ucType = 0;
		if(sRecvCanTmp.pCanData[0] == 0xff)
			ucType = 0xff ;
		else		
			ucType = sRecvCanTmp.pCanData[0] & 0x3F ;
		ACE_Guard<ACE_Thread_Mutex>  guard(m_mutexVoltage); 
		switch(ucType)
		{
		case POWER_HEAD_ENV :	
			m_iStongVoltage = sRecvCanTmp.pCanData[1] + 150;  //ǿ���ѹ		
			m_iWeakVoltage  = 0;  //�����ѹ
			m_iWeakVoltage  = (sRecvCanTmp.pCanData[2] & 0xf)  << 8;
			m_iWeakVoltage  = m_iWeakVoltage | sRecvCanTmp.pCanData[3];
				
			m_iWeakVoltage  = m_iWeakVoltage / 500;		
			m_iBusVoltage   = 0;  //���ߵ�ѹ
			m_iBusVoltage   = (sRecvCanTmp.pCanData[4] & 0xf)  << 8;
			m_iBusVoltage  = m_iBusVoltage | sRecvCanTmp.pCanData[5];
			m_iBusVoltage  = m_iBusVoltage / 500;
		
			//ACE_DEBUG((LM_DEBUG,"%s:%d StongVol:%d WeakVol:%d BusVol:%d			\n",__FILE__,__LINE__,m_iStongVoltage,m_iWeakVoltage,m_iBusVoltage)); //MOD:0604 1738
			break ;
		case POWER_HEAD_CFG_GET :
			m_iGetWarnHighVol = sRecvCanTmp.pCanData[1] ;
			m_iGetWarnLowVol  = sRecvCanTmp.pCanData[2] ;
			m_ucGetStongHighVolPlan = sRecvCanTmp.pCanData[3] & 0x3;
			m_ucGetStongLowVolPlan  = (sRecvCanTmp.pCanData[3] >> 2 )& 0x3;
			m_ucGetWeakHighVolPlan  = (sRecvCanTmp.pCanData[3] >> 4 )& 0x3; 
			m_ucGetWeakLowVolPlan   = (sRecvCanTmp.pCanData[3] >> 6 )& 0x3;
			m_ucSetWatchCfg  = sRecvCanTmp.pCanData[4];
			ACE_DEBUG((LM_DEBUG,"%s:%d VolHigh:%d VolLow:%d VolPlan:%d PowerWatchDog:%d !\n",__FILE__,__LINE__,m_iGetWarnHighVol,m_iGetWarnLowVol,sRecvCanTmp.pCanData[3],m_ucSetWatchCfg));
			break;
		case POWER_HEAD_HEARBEAT :
			 iHeartBeat = 0;
			 //ACE_DEBUG((LM_DEBUG,"%s:%d Get from PowerBoard,iHeartBeat = %d		\n",__FILE__,__LINE__,iHeartBeat)); //MOD:0604 1738
			 break ;
		case POWER_HEAD_VER:
			m_ucPowerBoardVer[PowerBdIndex][0] = sRecvCanTmp.pCanData[1];
			m_ucPowerBoardVer[PowerBdIndex][1] = sRecvCanTmp.pCanData[2];
			m_ucPowerBoardVer[PowerBdIndex][2] = sRecvCanTmp.pCanData[3];
			m_ucPowerBoardVer[PowerBdIndex][3] = sRecvCanTmp.pCanData[4];
			m_ucPowerBoardVer[PowerBdIndex][4] = sRecvCanTmp.pCanData[5];
			
		//	ACE_OS::printf("%s:%d PowerBoardver[%d]:%d %d %d %d %d \n",__FILE__,__LINE__,PowerBdIndex,sRecvCanTmp.pCanData[1],
				//	sRecvCanTmp.pCanData[2],sRecvCanTmp.pCanData[3],sRecvCanTmp.pCanData[4],sRecvCanTmp.pCanData[5]);
			break ;			
			default :				
				break ;
		}

}


/**************************************************************
Function:        CPowerBoard::GetStongVoltage
Description:    ��ȡǿ���ѹ����ֵ�����ڿ�����LCD��ѹ��ʾ��				
Input:          ��
Output:         ��
Return:         ǿ���ѹ
***************************************************************/
int  CPowerBoard::GetStongVoltage()
{
	return m_iStongVoltage ;
}

void CPowerBoard::GetPowerVer(Byte PowerBdindex)
{
	Byte PowerAddr = ((PowerBdindex ==0)?BOARD_ADDR_POWER:BOARD_ADDR_POWER2) ;
	SCanFrame sSendFrameTmp;	
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN , FRAME_MODE_P2P , PowerAddr , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = POWER_HEAD_VER;
	sSendFrameTmp.ucCanDataLen = 1;	
	Can::CreateInstance()->Send(sSendFrameTmp);
}
