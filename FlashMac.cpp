
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   FlashMac.cpp
Date:       2013-1-1
Description:�����������࣬�����������������ز���
Version:    V1.0
History:    201306041700  ��ӻ�����������⺯��
			201306051116  ��ӻ��������û�ȡ����
			201306050907  ����·����������ú���
***************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "ComStruct.h"
#include "FlashMac.h"
#include "ManaKernel.h"
#include "IoOperate.h"
#include "TscMsgQueue.h"
#include "LampBoard.h"
#include "GbtMsgQueue.h"

#include "Can.h" //ADD: 0604 16 49

/*
��������������ö��
*/
enum  
{
 	 FLASH_HEAD_CFGGET = 0x02 ,   //��ȡ��������������
	 FLASH_HEAD_CFGSET = 0x03 ,	  //���û�������������
	 FLASH_HEAD_ENTRY  = 0x04 ,   //ǿ�ƻ���
	 FLASH_HEAD_EXIT   = 0x05 ,   //�˳�������ǿ�ƻ���״̬
	 FLASH_HEAD_VER    =0xff       //�������汾

};

/**************************************************************
Function:       CFlashMac::CFlashMac
Description:    CFlashMac�๹�캯�������ڳ�ʼ���ࡣ				
Input:          ��			      
Output:         ��
Return:         ��
***************************************************************/
CFlashMac::CFlashMac()
{
	m_bSetHardwareFlash = false;   
	
	m_iMaxTemperature   = MAX_DEFAULT_TEMPERATURE;        
	m_iMinTemperature   = MIN_DEFAULT_TEMPERATURE;  	

	m_ucSetColdCnt      = 0;         
	m_ucSetHotCnt       = 0;         
	m_ucSetNullFanCnt   = MAX_WORK_TIME;

	m_ucSetFanSts       = FAN_NULL;
	m_ucLastSetFanSts   = FAN_NULL;

	m_iVoltage          = 210;           
	m_iTemperature      = 40;  

	m_bRecordSts    = true;       
	m_ucErrCheckCnt = 0;    
	m_ucRightCnt    = 0; 	

	m_pTscCfg = CManaKernel::CreateInstance()->m_pTscConfig;
	
	ACE_DEBUG((LM_DEBUG,"%s:%d Init FlashYW object ok !\n",__FILE__,__LINE__));
}


/**************************************************************
Function:       CFlashMac::~CFlashMac
Description:    CFlashMac������������				
Input:          ��			      
Output:         ��
Return:         ��
***************************************************************/
CFlashMac::~CFlashMac()
{
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct FlashYW object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CFlashMac::CreateInstance
Description:    ����CFlashMac��̬����				
Input:          ��			      
Output:         ��
Return:         ��̬����ָ��
***************************************************************/
CFlashMac* CFlashMac::CreateInstance()
{
	static CFlashMac cFlashMac; 
	return &cFlashMac;
}

/**************************************************************
Function:       CFlashMac::FlashHeartBeat
Description:    ���������������������Ϣ��
				ADD: 201306041700 				
Input:          ��			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::FlashHeartBeat()  
{	
	SCanFrame sSendFrameTmp;	
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	
	Can::BuildCanId(CAN_MSG_TYPE_011 , BOARD_ADDR_MAIN	, FRAME_MODE_HEART_BEAT   , BOARD_ADDR_FLASH  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_NOREPLY<< 6 ) ; 
	sSendFrameTmp.ucCanDataLen = 1;	
	Can::CreateInstance()->Send(sSendFrameTmp);
}

/**************************************************************
Function:       CFlashMac::FlashHeartBeat
Description:    �������������ϴ����������źţ����ڻ�ȡ���������á�
				ADD: 201306051116 				
Input:          ��			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::FlashCfgGet() 
{
	SCanFrame sSendFrameTmp;	
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	

	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P  , BOARD_ADDR_FLASH  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND << 6 ) | FLASH_HEAD_CFGGET;
	sSendFrameTmp.ucCanDataLen = 1;
	Can::CreateInstance()->Send(sSendFrameTmp);	

}

/**************************************************************
Function:       CFlashMac::FlashHeartBeat
Description:    �·��������ݸ���������
				ADD: 201306050907				
Input:          ��			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::FlashCfgSet() 
{
		SCanFrame sSendFrameTmp;	
		ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));			
		
		Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BOARD_ADDR_FLASH  , &(sSendFrameTmp.ulCanId));
		sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6 ) | FLASH_HEAD_CFGSET;
		sSendFrameTmp.pCanData[1] = m_ucSetFlashRate ; //������Ƶ������
		sSendFrameTmp.pCanData[1] |= m_ucSetDutyCycle <<4; //������ռ�ձ�����
		sSendFrameTmp.pCanData[2] = m_ucSetSyType;  //������ͬ����ʽ		
		sSendFrameTmp.ucCanDataLen = 3;
	//	ACE_DEBUG((LM_DEBUG,"%s:%d pCanData[1] = %d pCanData[2] = %d m_ucSetFlashRate = %d m_ucSetDutyCycle = %d\n",__FILE__,__LINE__,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2], m_ucSetFlashRate,m_ucSetDutyCycle));
		Can::CreateInstance()->Send(sSendFrameTmp);

}

/**************************************************************
Function:       CFlashMac::FlashForceStart
Description:    ���ذ巢��ǿ�ƻ�����Ϣ����������				
Input:          ucType  ��������				      
Output:         ��
Return:         0
***************************************************************/
void CFlashMac::FlashForceStart(Byte ucType)  //��ʼǿ�ƻ��� ADD:0605 11 30
{
	SCanFrame sSendFrameTmp;					
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
		
	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN	, FRAME_MODE_P2P , BOARD_ADDR_FLASH  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_NOREPLY<< 6 ) | FLASH_HEAD_ENTRY;	
	sSendFrameTmp.pCanData[1] = ucType; //ǿ�ƻ���
	sSendFrameTmp.ucCanDataLen = 2;		
	Can::CreateInstance()->Send(sSendFrameTmp);
	//ACE_DEBUG((LM_DEBUG,"%s:%d Begin force flashing !\n",__FILE__,__LINE__));
	CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_MANUALCONTROL,254);
	
	CLampBoard::CreateInstance()->SetSeriousFlash(true);
	SetHardwareFlash(true);			
	CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_FLASHBOARD,ucType,0,0,0);//ADD:201309251140  ����ӻ�������־��Ϣ
					
}

/**************************************************************
Function:       CFlashMac::FlashForceEnd
Description:    ���ذ巢���˳�������Ϣ����������				
Input:          ��				      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::FlashForceEnd()	//�˳�ǿ�ƻ���	ADD:0605 11 40
{
	SCanFrame sSendFrameTmp;
						
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
		
	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN	, FRAME_MODE_P2P , BOARD_ADDR_FLASH  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_NOREPLY<< 6 ) | FLASH_HEAD_EXIT;				
	sSendFrameTmp.ucCanDataLen = 1;			
	Can::CreateInstance()->Send(sSendFrameTmp);				//�����˳�������Ϣ��������
	CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_MANUALCONTROL,0);
	CLampBoard::CreateInstance()->SetSeriousFlash(false);	
	
	SetHardwareFlash(false);

}

void CFlashMac::FlashGetVer()
{
	SCanFrame sSendFrameTmp;							
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));			
	Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN	, FRAME_MODE_P2P , BOARD_ADDR_FLASH  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = FLASH_HEAD_VER;				
	sSendFrameTmp.ucCanDataLen = 1; 		
	Can::CreateInstance()->Send(sSendFrameTmp); 
}

		
/**************************************************************
Function:       CFlashMac::SetHardwareFlash
Description:    ���û���������״̬���ԡ�				
Input:          isflash   true: ���ڻ���״̬
						  false ���ڷǻ���״̬				      
Output:         �ı�CFlashMac�����m_bSetHardwareFlash����
Return:         ��
***************************************************************/
void CFlashMac::SetHardwareFlash(bool isflash)
{
	if(isflash)
		m_bSetHardwareFlash = true;
	else
		m_bSetHardwareFlash = false;
}


/**************************************************************
Function:       CFlashMac::GetHardwareFlash
Description:    ��ȡ����������״̬���ԡ�				
Input:        	��			      
Output:         ��
Return:         ����boolֵ���Ƿ��ڻ���
***************************************************************/
bool CFlashMac::GetHardwareFlash()
{
	return m_bSetHardwareFlash ;
}

/**************************************************************
Function:       CFlashMac::WriteAndRead
Description:    дand������ 500ms 1per				
Input:        	��			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::WriteAndRead()
{
	Byte ucTx[MAX_FRAME_LEN] = {0};
	Byte ucRx[MAX_FRAME_LEN] = {0};
	Byte ucSum  = 0;
	int  iRet   = 0;
	int  iIndex = 0;
	static int iTick = 0;

#ifndef WINDOWS	
	fd_set fRfds;
	struct timeval 	tTv;
#endif

	SetWriteData(ucTx);

#ifndef WINDOWS
	if ( !CIoOprate::TscWrite(m_iDevFd , ucTx , MAX_FRAME_LEN) )
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d write error",__FILE__,__LINE__));
		return;
	}

	if ( ( (m_pTscCfg->sSpecFun[FUN_PRINT_FLAG].ucValue>>5) & 1 )  != 0 )
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d send to flash: %x %x %x %x \n",__FILE__,__LINE__,ucTx[0],ucTx[1],ucTx[2],ucTx[3]));
	}

	FD_ZERO(&fRfds);
	FD_SET(m_iDevFd, &fRfds);
	tTv.tv_sec  = 0;
	tTv.tv_usec = 20000;
	iRet = select(m_iDevFd + 1, &fRfds, NULL, NULL, &tTv);

	if ( -1 == iRet )  //select error
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d select error\n",__FILE__,__LINE__));
	}
	else if ( 0 == iRet )  //timeout
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d timeout\n",__FILE__,__LINE__));
	}
	else
	{
		if ( !CIoOprate::TscRead(m_iDevFd , ucRx , MAX_FRAME_LEN) )
		{
			ACE_DEBUG((LM_DEBUG,"%s:%d write error",__FILE__,__LINE__));
			return;
		}

		if ( iTick < 60 )  //30s
		{
			iTick++;
			return;
		}

		while ( iIndex < (MAX_FRAME_LEN-1) )
		{
			ucSum += ucRx[iIndex];
			iIndex++;
		}
		if ( ucSum == ucRx[MAX_FRAME_LEN-1] ) //��ȷ������
		{
			if ( ucSum != 0 || ( 0 == ucSum && (ucRx[0]!=0 || ucRx[1]!=0 || ucRx[2]!=0 ) ) )  //Ԥ��û�л������������Ȼ�յ�ȫ0������
			{
				ResolveReadData(ucRx);
				if ( ( (m_pTscCfg->sSpecFun[FUN_PRINT_FLAG].ucValue>>6) & 1 )  != 0 )
				{
					ACE_DEBUG((LM_DEBUG,"%s:%d flash send to me right %x %x %x %x \n\n",__FILE__,__LINE__
										,ucRx[0],ucRx[1],ucRx[2],ucRx[3]));
				}
				SendRecordBoardMsg(0);
				return;
			}	
		}
		//У�������
		if ( ( (m_pTscCfg->sSpecFun[FUN_PRINT_FLAG].ucValue>>6) & 1 )  != 0 )
		{
			ACE_DEBUG((LM_DEBUG,"%s:%d flash send to me error %x %x %x %x \n\n",__FILE__,__LINE__
								,ucRx[0],ucRx[1],ucRx[2],ucRx[3]));
		}
		SendRecordBoardMsg(1);
	}
	
#endif

}

/**************************************************************
Function:       CFlashMac::SendRecordBoardMsg
Description:    ���ͼ�¼������ͨ��״̬ 500ms 1per				
Input:        	ucType  - 0��ȷ 1У�����			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::SendRecordBoardMsg(Byte ucType)
{
	Byte ucCnt   = 0;
	Byte ucByte0 = 0;
	Byte ucByte1 = 0;

	if ( 0 == ucType && m_bRecordSts )  //��ǰ״̬��ȷ���յ���ȷ������
	{
		m_ucErrCheckCnt = 0;
		return;
	}

	if ( ucType != 0 && !m_bRecordSts ) //��ǰ״̬�������յ����������
	{
		m_ucRightCnt = 0;
		return;
	}

	switch ( ucType )
	{
		case 0:
			m_ucRightCnt++;
			ucCnt = m_ucRightCnt;  
			break;
		case 1:
			m_ucErrCheckCnt++;
			ucCnt =  m_ucErrCheckCnt;
			break;
		default:
			return;
	}

	if ( ucCnt > BOARD_REPEART_TIME )
	{
		switch ( ucType )
		{
			case 0:  //��ȷ
				m_bRecordSts    = true;
				m_ucErrCheckCnt = 0;
				ucByte0 = 0;
				ucByte1 = 0;
				break;
			case 1:  //У�����
				m_bRecordSts    = false;
				m_ucRightCnt = 0;
				ucByte0 = 1;
				ucByte1 = 2;
				break;
			default:
				return;
		}
	}
	else
	{
		return;
	}

	CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_FLASHBOARD,0,0,ucByte1,ucByte0);//ADD:201309251140  ����ӻ�������־��Ϣ
	ACE_DEBUG((LM_DEBUG,"%s:%d LOG_TYPE_FLASHBOARD\n",__FILE__,__LINE__));
}


/**************************************************************
Function:       CFlashMac::SetWriteData
Description:    ���÷��������������������ڿ��Ƽ���ɢ��״̬				
Input:        	pTx  - ���淢����Ϣ���ݵ�ַ			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::SetWriteData(Byte* pTx)
{
	Byte ucTmp = 0;

	if ( m_iTemperature < m_iMinTemperature )  //����
	{
		m_ucSetColdCnt       = 0;         
		m_ucSetHotCnt++;        
		m_ucSetNullFanCnt    = 0;  
	}
	else if ( m_iTemperature > m_iMaxTemperature ) //ɢ��
	{ 
		m_ucSetColdCnt++;         
		m_ucSetHotCnt        = 0;        
		m_ucSetNullFanCnt    = 0;  
	}
	else   //������
	{
		if ( ( m_bGetHotFan && m_iTemperature > UP_NORMAL_TEMPERATURE ) //��ǰ���ȴ��ڼ������Ѿ��ӵ������¶�  ֹͣ���ȷ���
			|| (m_bGetColdFan && m_iTemperature < DROP_NORMAL_TEMPERATURE ) )  //���ȴ��ڽ������Ѿ������������¶� ֹͣ����
		{
			m_ucSetColdCnt       = 0;         
			m_ucSetHotCnt        = 0;        
			m_ucSetNullFanCnt++;  
		}
	}

	if ( m_bSetHardwareFlash )  //Ӳ������
	{
		ucTmp |= 1;
	}
	
	if (  m_pTscCfg->sSpecFun[FUN_TEMPERATURE].ucValue != 0 )  //�¶ȼ�
	{
		if ( m_ucSetColdCnt > MAX_WORK_TIME ) 
		{
			m_ucSetFanSts = FAN_COLD;
		}
		else if ( m_ucSetHotCnt > MAX_WORK_TIME )  
		{
			m_ucSetFanSts = FAN_HOT;
		}
		else if ( m_ucSetNullFanCnt > MAX_WORK_TIME )
		{
			m_ucSetFanSts = FAN_NULL;
		}
	}
	else
	{
		m_ucSetFanSts = FAN_NULL;
	}

	if ( m_ucLastSetFanSts != m_ucSetFanSts )
	{
		m_ucLastSetFanSts = m_ucSetFanSts;		
		CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_TEMPERATURE,0,0,m_iTemperature,m_ucLastSetFanSts);//ADD:201309251440  

	}

	switch ( m_ucSetFanSts )
	{
		case FAN_COLD:    //��ɢ����
			ucTmp |= 1<<1;
			break;
		case FAN_HOT:     //�򿪼��ȷ���
			ucTmp |= 1<<2;
			break;
		case FAN_NULL:  //��
			break;
		default:
			break;
	}
	pTx[0] = ucTmp;    //������
	pTx[1] = 0x55;     //����
	pTx[2] = 0xff;     //����	
	pTx[3] = pTx[0] + pTx[1] + pTx[2];    //������
}


/**************************************************************
Function:       CFlashMac::ResolveReadData
Description:    �����ӿ�������ȡ���Ļ�������				
Input:        	pTx  - ��ȡ�������ݻ����ַ			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::ResolveReadData(Byte* pTx)
{
	if ( pTx[0] & 0x80 )  //��������
	{
		m_bPowerType = true;  //������
		m_iVoltage   = (pTx[0] & 0x7F) + 150;
	}
	else
	{
		m_bPowerType = false; //̫����
		m_iVoltage   = (pTx[0] & 0xF) + 150;
	}

	m_iTemperature = pTx[1];  //�¶�
	
	/*Ӳ������*/
	if ( pTx[2] & 0x1 )  
	{
		m_bGetHardwareFlash = true;
	}
	else 
	{
		m_bGetHardwareFlash = false;
	}
	
	/*ɢ�ȷ���*/
	if ( pTx[2] & 0x2 )
	{
		m_bGetColdFan = true;
	}
	else
	{
		m_bGetColdFan = false;
	}
	
	/*���ȷ���*/
	if ( pTx[2] & 0x4 )
	{
		m_bGetHotFan = true;
	}
	else
	{
		m_bGetHotFan = false;
	}

	/*ǰ�Ŵ�*/
	if ( pTx[2] & 0x8 )
	{
		m_bGetForDoor = true;
	}
	else
	{
		m_bGetForDoor = false;
	}

	/*���Ŵ�*/
	if ( pTx[2] & 0x10 )
	{
		m_bGetAfterDoor = true;
	}
	else
	{
		m_bGetAfterDoor = false;
	}
}

/**************************************************************
Function:       CFlashMac::SetMaxMinTemperature
Description:    �������������¶Ⱥ�����¶�				
Input:        	iMaxTpt - ��������¶�
				iMinTpt - ��������¶�			      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::SetMaxMinTemperature(int iMaxTpt,int iMinTpt)
{
	m_iMaxTemperature = iMaxTpt;
	m_iMinTemperature = iMinTpt;
}


/**************************************************************
Function:       CFlashMac::RecvFlashCan
Description:    �����ӻ������������ĸ���Can���ݲ�����				
Input:        	sRecvCanTmp - ���յ���Can���ݰ�						      
Output:         ��
Return:         ��
***************************************************************/
void CFlashMac::RecvFlashCan(SCanFrame sRecvCanTmp)
{
	Byte ucType = 0x0;
	if(sRecvCanTmp.pCanData[0] == 0xff)
		ucType = 0xff ;
	else		
		ucType = sRecvCanTmp.pCanData[0] & 0x3F ;
	switch(ucType)
	{
		case FLASH_HEAD_CFGSET:
			if( (m_ucSetSyType != (sRecvCanTmp.pCanData[2])) || (m_ucSetFlashRate != (sRecvCanTmp.pCanData[1]&0xf)) || (m_ucSetDutyCycle != (sRecvCanTmp.pCanData[1])>>4 ))
			{
				ACE_DEBUG((LM_DEBUG,"%s:%d Set FLASH CFG ERROR!\n",__FILE__,__LINE__));			
			}
		break ;
		case FLASH_HEAD_CFGGET :
			m_ucGetFlashRate = sRecvCanTmp.pCanData[1] & 0xf ;
			m_ucGetDutyCycle = sRecvCanTmp.pCanData[1]>>4  & 0xf ;
			m_ucGetSyType      = sRecvCanTmp.pCanData[2] &0x3;
			m_ucFlashStatus  = sRecvCanTmp.pCanData[3] &0xf;
			
			break ;
		case FLASH_HEAD_VER:
			m_ucFlashVer[0]=sRecvCanTmp.pCanData[1];			
			m_ucFlashVer[1]=sRecvCanTmp.pCanData[2];
			m_ucFlashVer[2]=sRecvCanTmp.pCanData[3];
			m_ucFlashVer[3]=sRecvCanTmp.pCanData[4];
			m_ucFlashVer[4]=sRecvCanTmp.pCanData[5];			
			//ACE_OS::printf("%s:%d FlashBdver:%d %d %d %d %d \n",__FILE__,__LINE__,sRecvCanTmp.pCanData[1],
			//	sRecvCanTmp.pCanData[2],sRecvCanTmp.pCanData[3],sRecvCanTmp.pCanData[4],sRecvCanTmp.pCanData[5]);
			break ;
			
		default :
			return ;
			break ;
	}
	
}

