
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   LampBoard.cpp
Date:       2013-1-1
Description:�źŻ��ƿذ����������ļ�.�����ƿذ����ú���Ϣ״̬�Ļ�ȡ������
Version:    V1.0
History:
***************************************************************/

#include "LampBoard.h"
#include "TscMsgQueue.h"

#include "DbInstance.h"
#include "Can.h"
#include "FlashMac.h"
#include "GaCountDown.h"

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
�źŵ�״̬ö��
*/
enum
{
	LAMP_FLASH = 0 , //�źŵ���˸
	LAMP_ON    = 1 , //�źŵ���
	LAMP_OFF   = 2 , //�źŵ���
	LAMP_HOLD  = 3 , //�źŵƱ���ԭ״
};

/*
�ƿذ������������ö��
*/
enum
{
	LAMPBOARD_HEAD_SETCOLOR      = 0x02 ,  //���õƿذ�ƾߵ���ɫ
	LAMPBOARD_HEAD_CFG           = 0x03 ,  //�Եƿذ��·���������
	LAMPBOARD_HEAD_LIGHT_CHK     = 0x04 ,  //����ƿذ巢�͵��ݼ������
	LAMPBOARD_HEAD_ELECT_CHK134  = 0x05 ,  //����ƿذ巢��ͨ��1��3��4�ĵ��ݵ������
	LAMPBOARD_HEAD_ELECT_CHK679  = 0x06 ,  //����ƿذ巢��ͨ��6��7��9�ĵ��ݵ������
	LAMPBOARD_HEAD_ELECT_CHK1012 = 0x07 ,  //����ƿذ巢��ͨ��10��12 �ĵ��ݵ������
	LAMPBOARD_HEAD_TEMPE_CHK     = 0x08 ,  //����ƿذ巢�͵ƿذ�����¶ȼ����¶�ֵ
	LAMPBOARD_HEAD_VER           = 0xff    //����ƿذ�汾����
};

/**************************************************************
Function:        CLampBoard::CLampBoard
Description:     �ƿذ�CLampBoard�๹�캯������ʼ����			
Input:          ��           
Output:         ��
Return:         0
***************************************************************/
CLampBoard::CLampBoard()
{
	ACE_OS::memset(m_ucLampOn    , 0 , MAX_LAMP);
	ACE_OS::memset(m_ucLampFlash , 0 , MAX_LAMP);
	
	ACE_OS::memset(iLampBoardTemp , 0 , MAX_LAMP_BOARD);// ADD: 2013 0712 14 59
	ACE_OS::memset(m_ucLampStas , 0 , MAX_LAMP); // ADD: 2013 0712 10 59
	ACE_OS::memset(m_ucCheckCfg , 0 , MAX_LAMP_BOARD); // ADD: 2013 0712 11 11
	ACE_OS::memset(m_usLampElect , 0 , MAX_LAMP_BOARD*8); // ADD: 2013 0712 14 11
	ACE_OS::memset(m_ucLampOnCfg , 0 , MAX_LAMP_BOARD*3); // ADD: 2013 0712 11 40
	ACE_OS::memset(m_ucLampConflic , 0 , MAX_LAMP_BOARD*4); // ADD: 2013 0802 11 20
	ACE_OS::memset(m_ucLampBoardVer , 0 , MAX_LAMP_BOARD*5); // ADD: 2015011311 20
	
	m_bSeriousFlash = false;
	IsChkLight = true ;
    pManakernel= CManaKernel::CreateInstance() ;
	for (int iIndex = 0; iIndex<MAX_LAMP_BOARD; iIndex++ ) //ADD :20130822 1656
	{
		m_ucLampBoardError[iIndex] = DEV_IS_CONNECTED;
	}
	for (int iIndex=0; iIndex<MAX_LAMP; iIndex++ )
	{
		m_bLampErrFlag[iIndex]     = true;
		//m_bLampGreenFlag[iIndex]   = false;
		//m_bRecordSts[iIndex]       = true;
	}
	ACE_DEBUG((LM_DEBUG,"%s:%d Init LampBoard object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:        CLampBoard::~CLampBoard
Description:     �ƿذ�CLampBoard��������		
Input:          ��           
Output:         ��
Return:         0
***************************************************************/
CLampBoard::~CLampBoard()
{
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct LampBoard object ok !\n",__FILE__,__LINE__));
}


/**************************************************************
Function:        CLampBoard::CreateInstance
Description:     ����CLampBoard�ƿؾ�̬����		
Input:          ��           
Output:         ��
Return:         ��̬����ָ��
***************************************************************/
CLampBoard* CLampBoard::CreateInstance()
{
	static CLampBoard cLampBoard;
	return &cLampBoard;
}


/**************************************************************
Function:        CLampBoard::ReviseLampInfo
Description:     �Ϸ���ɫ��ϢУ��		
Input:          ucType  0-�� 1-��   
				pLampInfo  ��ɫ����        
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::ReviseLampInfo(Byte ucType,Byte pLampInfo[MAX_LAMP])
{
	Byte ucCheckCnt = 0;
	Byte ucColorCnt = 0;
	SThreadMsg sTscMsg;

	ACE_OS::memset(&sTscMsg,0,sizeof(sTscMsg));

	for ( Byte ucIndex=0; ucIndex<MAX_LAMP; ucIndex++ )
	{
		if ( pLampInfo[ucIndex] != 0 )
		{
			ucColorCnt++;
		}
		ucCheckCnt++;

		if ( ucCheckCnt >= 3 )
		{
			if ( ucColorCnt >= 2 )
			{
				pLampInfo[ucIndex]   = 0;  //green
				pLampInfo[ucIndex-1] = 0;  //yellow
				if ( 0 == ucType )  //lampOn
				{
					pLampInfo[ucIndex-2] = 1;  //red
				}
				else  //lampFlash
				{
					pLampInfo[ucIndex-1] = 1;  //yellow
					pLampInfo[ucIndex-2] = 0;  //red
				}
				//ACE_DEBUG((LM_DEBUG,"%s:%d index:%d lamp info error \n",LAMP_CPP,__LINE__,ucIndex));

				sTscMsg.ulType       = TSC_MSG_LOG_WRITE;
				sTscMsg.ucMsgOpt     = LOG_TYPE_OUTLAMP_ERR;
				sTscMsg.uiMsgDataLen = 4;
				sTscMsg.pDataBuf     = ACE_OS::malloc(4);
				((Byte*)(sTscMsg.pDataBuf))[0] = pLampInfo[ucIndex]   > 0 ?  ucIndex + 1 : 0;
				((Byte*)(sTscMsg.pDataBuf))[1] = pLampInfo[ucIndex-1] > 0 ?  ucIndex     : 0;
				((Byte*)(sTscMsg.pDataBuf))[2] = pLampInfo[ucIndex-2] > 0 ?  ucIndex - 1 : 0;  
				((Byte*)(sTscMsg.pDataBuf))[3] = ucType;
				CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));
			}
			ucColorCnt = 0;
			ucCheckCnt = 0;
		}
	}

}


/**************************************************************
Function:        CLampBoard::SetSeriousFlash
Description:     �������ػ������		
Input:          isflash  flase-�ǻ���״̬ 1-����״̬ 
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::SetSeriousFlash(bool isflash)
{
	m_bSeriousFlash = isflash;
}


/**************************************************************
Function:        CLampBoard::SetLamp
Description:     ���õƾ���ɫ������һ����������һ��	
Input:          pLampOn  ������״̬����ָ��
				pLampFlash  ����˸װ������ָ��
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::SetLamp(Byte* pLampOn,Byte* pLampFlash)
{
	ACE_Guard<ACE_Thread_Mutex>  guard(m_mutexLamp);
	if ( NULL == pLampOn || NULL == pLampFlash )
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d SetLamp info error\n",__FILE__,__LINE__));
		return;
	}
	ACE_OS::memcpy(m_ucLampOn,pLampOn,MAX_LAMP);
	ACE_OS::memcpy(m_ucLampFlash,pLampFlash,MAX_LAMP);
}

/**************************************************************
Function:        CLampBoard::GetLamp
Description:     ��ȡ�ƾ���ɫ������һ����������һ��	
Input:          pLampOn  ������״̬����ָ��
			pLampFlash  ����˸װ������ָ��
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::GetLamp(Byte* pLampOn,Byte* pLampFlash)
{
	if ( NULL == pLampOn || NULL == pLampFlash )
	{
		return;
	}
	ACE_OS::memcpy(pLampOn,m_ucLampOn,MAX_LAMP);
	ACE_OS::memcpy(pLampFlash,m_ucLampFlash,MAX_LAMP);
}


/**************************************************************
Function:        CLampBoard::SendSingleLamp
Description:     ���͵�һ���ƿذ�����ݣ����õƿذ�ƾ���ɫ	
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3 4  5  6  7 
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::SendSingleLamp(Byte ucLampBoardId,Byte ucFlashBreak)
{
	Byte ucDataTemp  = 0;
	Byte ucLampIndex = 0;
	Byte ucLampBoardCanAddr = GetLampBoardAddr(ucLampBoardId);
	SCanFrame sSendFrameTmp;	
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	
	Can::BuildCanId(CAN_MSG_TYPE_011 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , ucLampBoardCanAddr  , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_NOREPLY<< 6 ) | LAMPBOARD_HEAD_SETCOLOR;
	for ( int iDataIndex=1; iDataIndex<4; iDataIndex++ ) //һ��ƿذ���3���ֽڴ洢��ɫ��Ϣ��ÿ��ͨ����ɫ��Ϣռ2bitλ,һ���ֽڴ洢4ͨ����ɫ��Ϣ
	{
		ucDataTemp  = 0;
		ucLampIndex = ucLampBoardId * 12 + (iDataIndex-1) * 4;  //1���12��ͨ�� 1�ֽڴ洢4��ͨ�� �ƾ�???? �ñ�����ʾÿ��ƿذ���ʼ�ƾߵ�����.
		
		for ( int ucIncrease=0; ucIncrease<4; ucIncrease++ ) //1���ֽ�ÿ��2bitλ���ã���Ҫѭ��4��.
		{
			if ( 1 == m_ucLampFlash[ucLampIndex+ucIncrease] )  //����
			{
				ucDataTemp |= LAMP_FLASH << (2*ucIncrease); //ÿ���ƶ���λ��ʾһ���ƾ���ɫ
			}
			else
			{
				if ( 1 == m_ucLampOn[ucLampIndex+ucIncrease] )  //����
				{
					ucDataTemp |= LAMP_ON << (2*ucIncrease);
				}
				else 
				{
					ucDataTemp |= LAMP_OFF << (2*ucIncrease);  //���
				}
			}
		}
		sSendFrameTmp.pCanData[iDataIndex] = ucDataTemp; //ѭ��3�Σ�ÿ��4���ƾ�״̬��ֵ��ÿ��һ���ֽ�.
	}
	sSendFrameTmp.pCanData[4] = ~sSendFrameTmp.pCanData[1];
	sSendFrameTmp.pCanData[5] = ~sSendFrameTmp.pCanData[2];
	sSendFrameTmp.pCanData[6] = ~sSendFrameTmp.pCanData[3];
	sSendFrameTmp.pCanData[7] = ucFlashBreak ;
	sSendFrameTmp.ucCanDataLen = 8;

	m_ucLampOnCfg[ucLampBoardId][0] = sSendFrameTmp.pCanData[1]; //ADD:2013 0712 11 45
	m_ucLampOnCfg[ucLampBoardId][1] = sSendFrameTmp.pCanData[2];
	m_ucLampOnCfg[ucLampBoardId][2] = sSendFrameTmp.pCanData[3];	
	
	Can::CreateInstance()->Send(sSendFrameTmp);
}


/**************************************************************
Function:        CLampBoard::SendLamp
Description:     �����еƿذ巢�͵�ɫ��Ϣ	������״̬500ms����һ��
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::SendLamp()
{
	ACE_Guard<ACE_Thread_Mutex>  guard(m_mutexLamp);
	for ( Byte iBdINdex=0; iBdINdex<MAX_LAMP_BOARD; iBdINdex++ )	
	{
		if(m_ucLampBoardError[iBdINdex] == DEV_IS_CONNECTED)
		{
			 
			SendSingleLamp(iBdINdex,0);
/******   ����ÿ�η��͵�֮���ʱ��
			struct timeval    tv;  
		    struct timezone tz;  
		      
		    struct tm         *p; 
		      
		    gettimeofday(&tv, &tz); 
		      
		    p = localtime(&tv.tv_sec);  
		   ACE_OS::printf("Time:%d%d%d%d%d%d.%03ld\n", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);  


		   */
		}
	}
}


/**************************************************************
Function:        CLampBoard::SendCfg
Description:     �����еƿذ巢����������
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::SendCfg()
{
	for ( Byte iBdINdex=0; iBdINdex<MAX_LAMP_BOARD; iBdINdex++ )
	{
		if(m_ucLampBoardError[iBdINdex] == DEV_IS_CONNECTED)
			SendSingleCfg(iBdINdex);
	}
}


/**************************************************************
Function:        CLampBoard::SendSingleCfg
Description:     �Ե����ƿذ巢����������
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::SendSingleCfg(Byte ucLampBoardId)
{	
	
	Byte ucLampBoardCanAddr = GetLampBoardAddr(ucLampBoardId);
	SCanFrame sSendFrameTmp;	
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN , FRAME_MODE_P2P  , ucLampBoardCanAddr , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0]  = ( DATA_HEAD_CHECK << 6 ) | LAMPBOARD_HEAD_CFG;
	//bit0-bit1 00:���ֵ�ǰ���ò��� 01:�رյ��ݼ��      2:�򿪵��ݼ��      3:���� ����
	//bit2-bit3 00:���ֵ�ǰ���ò��� 01:�رպ��̳�ͻ���  2:�򿪺��̳�ͻ���  3:���� ����
	sSendFrameTmp.pCanData[1]  = m_ucCheckCfg[ucLampBoardId] & 0x0f;	
	sSendFrameTmp.ucCanDataLen = 2;
	Can::CreateInstance()->Send(sSendFrameTmp);
	
	
}


/**************************************************************
Function:        CLampBoard::CheckLight
Description:     ����ƿذ巢�͵��ݼ������
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckLight()
{
	//if(IsChkLight == false || pManakernel->m_pRunData->bIsChkLght == false )				
	//	return ;		
	 
	for ( Byte iBdINdex=0; iBdINdex<MAX_LAMP_BOARD; iBdINdex++ )
	{	
		if(m_ucLampBoardError[iBdINdex] == DEV_IS_CONNECTED)
		{	
			CheckSingleLight(iBdINdex);		
		}
	}
}


/**************************************************************
Function:        CLampBoard::CheckSingleLight
Description:     ����һ�ƿذ巢�͵��ݼ��,���̳�ͻ����
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckSingleLight(Byte ucLampBoardId)
{
	
	Byte ucLampBoardCanAddr = GetLampBoardAddr(ucLampBoardId);
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	Can::BuildCanId(CAN_MSG_TYPE_011 , BOARD_ADDR_MAIN , FRAME_MODE_P2P , ucLampBoardCanAddr , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0]  = ( DATA_HEAD_RESEND << 6 ) | LAMPBOARD_HEAD_LIGHT_CHK;  //DATA_HEAD_CHECK--->DATA_HEAD_RESEND MOD:0605 2121
	sSendFrameTmp.ucCanDataLen = 1;
	
	Can::CreateInstance()->Send(sSendFrameTmp);
	/*
	if ( ( (m_pTscCfg->sSpecFun[FUN_PRINT_FLAG].ucValue>>3) & 1 )  != 0 )	
	ACE_DEBUG((LM_DEBUG,"%s:%d SendSingleLight%d:\n",__FILE__,__LINE__,ucLampBoardId+1));
	ACE_DEBUG((LM_DEBUG,"CanId:%08x CanDataLen:%d\nData:",sSendFrameTmp.ulCanId,sSendFrameTmp.ucCanDataLen));
	for ( int iPrtIndex=0; iPrtIndex<sSendFrameTmp.ucCanDataLen;iPrtIndex++)
	{
		ACE_DEBUG((LM_DEBUG,"%02x ",sSendFrameTmp.pCanData[iPrtIndex]));
	}	
	Can::PrintInfo((char*)__FILE__,__LINE__,ucLampBoardId+1,sSendFrameTmp);
	************************************************************** TEST 2013 07 12 14 44
	
	*/
}


/**************************************************************
Function:        CLampBoard::CheckSingleLight
Description:     ���ĳͨ���ĵ���
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3
				ucType  LAMPBOARD_HEAD_ELECT_CHK134    ͨ��134
					    LAMPBOARD_HEAD_ELECT_CHK679	   ͨ��679
					    LAMPBOARD_HEAD_ELECT_CHK1012   ͨ��10 12
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckLampElect(Byte ucLampBoardId,Byte ucType)
{
	
	Byte ucLampBoardCanAddr = GetLampBoardAddr(ucLampBoardId);
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	Can::BuildCanId(CAN_MSG_TYPE_011 , BOARD_ADDR_MAIN , FRAME_MODE_P2P , ucLampBoardCanAddr , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0]  = (DATA_HEAD_RESEND << 6 ) | ucType;
	sSendFrameTmp.ucCanDataLen = 1;

	Can::CreateInstance()->Send(sSendFrameTmp);
	
	/****************************
	//if ( ( (m_pTscCfg->sSpecFun[FUN_PRINT_FLAG].ucValue>>3) & 1 )  != 0 )
	
	ACE_DEBUG((LM_DEBUG,"%s:%d CheckLampElect%d-%d:\n",__FILE__,__LINE__,ucLampBoardId+1,ucType));
	ACE_DEBUG((LM_DEBUG,"CanId:%08x CanDataLen:%d\nData:",sSendFrameTmp.ulCanId,sSendFrameTmp.ucCanDataLen));
	for ( int iPrtIndex=0; iPrtIndex<sSendFrameTmp.ucCanDataLen; iPrtIndex++)
	{
		ACE_DEBUG((LM_DEBUG,"%02x ",sSendFrameTmp.pCanData[iPrtIndex]));
	}	
	*/
}


/**************************************************************
Function:        CLampBoard::CheckSingleElect
Description:     ��ⵥһ�ƿذ�ĵ���
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckSingleElect(Byte ucLampBoardId)
{
	CheckLampElect(ucLampBoardId , LAMPBOARD_HEAD_ELECT_CHK134);
	CheckLampElect(ucLampBoardId , LAMPBOARD_HEAD_ELECT_CHK679);
	CheckLampElect(ucLampBoardId , LAMPBOARD_HEAD_ELECT_CHK1012);
}


/**************************************************************
Function:        CLampBoard::CheckElect
Description:     �������ƿذ�ĵ��ݵ���
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckElect()
{
	for ( Byte ucLampBoardIndex=0; ucLampBoardIndex<MAX_LAMP_BOARD; ucLampBoardIndex++ )
	{
		CheckSingleElect(ucLampBoardIndex);
	}
}


/**************************************************************
Function:        CLampBoard::CheckTemp
Description:     ������еƿذ�忨�¶�
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckTemp()
{
	for ( Byte ucLampBoardIndex=0; ucLampBoardIndex<MAX_LAMP_BOARD; ucLampBoardIndex++ )
	{
		CheckSingleTemp(ucLampBoardIndex);
	}
}


/**************************************************************
Function:        CLampBoard::CheckSingleTemp
Description:     ��ⵥһ�ƿذ�忨���¶�
Input:          ucLampBoardId  �ƿذ��±����� 0 1 2 3
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::CheckSingleTemp(Byte ucLampBoardId)
{
	
	Byte ucLampBoardCanAddr = GetLampBoardAddr(ucLampBoardId);
	SCanFrame sSendFrameTmp;	
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	
	Can::BuildCanId(CAN_MSG_TYPE_011 , BOARD_ADDR_MAIN , FRAME_MODE_P2P , ucLampBoardCanAddr , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0]  = (DATA_HEAD_RESEND << 6 ) | LAMPBOARD_HEAD_TEMPE_CHK;
	sSendFrameTmp.ucCanDataLen = 1;

	Can::CreateInstance()->Send(sSendFrameTmp);

}


/**************************************************************
Function:        CLampBoard::RecvLampCan
Description:     �����ƿذ巢�ͻ�����Can���ݰ���������
Input:          ucBoardAddr  �ƿذ��±����� 0 1 2 3
				sRecvCanTmp  can���ݰ�
Output:         ��
Return:         ��
***************************************************************/
void CLampBoard::RecvLampCan(Byte ucBoardAddr,SCanFrame sRecvCanTmp)
{		
	Byte ucLampBoardId = 0;
	Byte isFlash = 0 ;
	Byte ucType = 0x0;
	if(sRecvCanTmp.pCanData[0] == 0xff)
		ucType = 0xff ;
	else		
		ucType = sRecvCanTmp.pCanData[0] & 0x3F ;
	//ACE_OS::printf("%s:%d BoardAddr:%2x ucType=%d \n",__FILE__,__LINE__,ucBoardAddr,ucType);
	switch ( ucBoardAddr )
	{
		case BOARD_ADDR_LAMP1:
			ucLampBoardId = 0;
			break;
		case BOARD_ADDR_LAMP2:
			ucLampBoardId = 1;
			break;
		case BOARD_ADDR_LAMP3:
			ucLampBoardId = 2;
			break;
		case BOARD_ADDR_LAMP4:
			ucLampBoardId = 3;
			break ;
		case BOARD_ADDR_LAMP5:
			ucLampBoardId = 4;
			break;
		case BOARD_ADDR_LAMP6:
			ucLampBoardId = 5;
			break;
		case BOARD_ADDR_LAMP7:
			ucLampBoardId = 6;
			break ;
		case BOARD_ADDR_LAMP8:
			ucLampBoardId = 7;
			break ;
		default:				
			return;
	}
		
		if ( LAMPBOARD_HEAD_LIGHT_CHK == ucType )
		{
			//ACE_DEBUG((LM_DEBUG,"%s:%d Get the LampBoard%d status!\n",__FILE__,__LINE__,ucLampBoardId+1));			
			//ͨ�����byte1-byte2  	//00:������������  01:���ݴ�ӳ���	//10:�����𻵳���  11:�ɿع���
			m_ucLampStas[ucLampBoardId*12]    = sRecvCanTmp.pCanData[1] & 0x3;          //1
			m_ucLampStas[ucLampBoardId*12+2]   = (sRecvCanTmp.pCanData[1] >> 2 ) & 0x3;   //3
			m_ucLampStas[ucLampBoardId*12+3]   = (sRecvCanTmp.pCanData[1] >> 4 ) & 0x3;   //4
			m_ucLampStas[ucLampBoardId*12+5]   = (sRecvCanTmp.pCanData[1] >> 6 ) & 0x3;	//6
		
			m_ucLampStas[ucLampBoardId*12+6]   = sRecvCanTmp.pCanData[2] & 0x3;            //7
			m_ucLampStas[ucLampBoardId*12+8]   = (sRecvCanTmp.pCanData[2] >> 2 ) & 0x3;     //9
			m_ucLampStas[ucLampBoardId*12+9]   = (sRecvCanTmp.pCanData[2] >> 4 ) & 0x3;     //10
			m_ucLampStas[ucLampBoardId*12+11]  = (sRecvCanTmp.pCanData[2] >> 6 ) & 0x3;      //12

			//���̳�ͻ���byte3	//00:���ݺ����������     01:���ݰ��еĺ��̳�ͻ	//10:Ӳ����⵽�ĺ��̳�ͻ 11:��������
			m_ucLampConflic[ucLampBoardId][0]   = sRecvCanTmp.pCanData[3] & 0x3;
			m_ucLampConflic[ucLampBoardId][1]   = (sRecvCanTmp.pCanData[3] >> 2 ) & 0x3;
			m_ucLampConflic[ucLampBoardId][2]   = (sRecvCanTmp.pCanData[3] >> 4 ) & 0x3;
			m_ucLampConflic[ucLampBoardId][3]   = (sRecvCanTmp.pCanData[3] >> 6 ) & 0x3;			
			
			if(CFlashMac::CreateInstance()->GetHardwareFlash())
			{
				//CFlashMac::CreateInstance()->FlashCfgGet();  // inorder to avoid turing off the flash and reopen
				return;
			}			
										
						for (int iLamp=ucLampBoardId*MAX_LAMP_NUM_PER_BOARD; iLamp<(ucLampBoardId+1)*MAX_LAMP_NUM_PER_BOARD; iLamp++)
						{
							if ( pManakernel->IsInChannel(iLamp/3+1))
							{
								//ACE_DEBUG((LM_DEBUG,"%s:%d LampGrp:%d isInChannel!\n",__FILE__,__LINE__,iLamp/3+1));
								if(pManakernel->m_pTscConfig->sChannelChk[iLamp].ucIsCheck == 1 && m_bLampErrFlag[iLamp] == true)
								{
								//ACE_DEBUG((LM_DEBUG,"%s:%d sChannelChk[%d]=%d is check!\n",__FILE__,__LINE__,iLamp,m_ucLampStas[iLamp]));
								
									if(m_ucLampStas[iLamp] != 0 && m_ucLampStas[iLamp] != 0x3)		
									{						
										m_bLampErrFlag[iLamp] = false ;
										ACE_DEBUG((LM_DEBUG,"%s:%d LampBoard: %d Lamp:%d status:%d \n",__FILE__,__LINE__,ucLampBoardId+1,iLamp+1,m_ucLampStas[iLamp]));												
										isFlash =1 ; // 1 ��ʾ���ݹ���
										if((iLamp+1)==30 && m_ucLampStas[iLamp] == 0x1)
										{
											isFlash = 3;
											(CDbInstance::m_cGbtTscDb).SetSystemData("ucDownloadFlag",0x3);											
											CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_GREEN_CONFIG,0x2,ucLampBoardId+1,(iLamp+1)/3,0); //ADD��201309251130						
										}
										else											
										   CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_LAMP,iLamp+1,m_ucLampStas[iLamp],ucLampBoardId+1,0);			
									}								



									
								}							
							}						
						}

			for(int iLampGrp = 0 ;iLampGrp<4 ;iLampGrp++)
			{
				if(m_ucLampConflic[ucLampBoardId][iLampGrp] != 0 && m_ucLampConflic[ucLampBoardId][iLampGrp] != 3)
				{
					ACE_DEBUG((LM_DEBUG,"%s:%d ucLampBoardI: %d  Lampgrp %d conflict:%d\n",__FILE__,__LINE__,ucLampBoardId+1,iLampGrp+1,m_ucLampConflic[ucLampBoardId][iLampGrp] ));
					//CManaKernel::CreateInstance()->DealGreenConflict(1); //					
						
					CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_GREEN_CONFIG,m_ucLampConflic[ucLampBoardId][iLampGrp],ucLampBoardId+1,iLampGrp+1,0); //ADD��201309251130						
					isFlash = 3 ; // 3 ��ʾ���̳�ͻ���ɵƿذ��жϷ��أ�����Ӳ����ͻ�����ͻ
							
				}
			}

			if (!CFlashMac::CreateInstance()->GetHardwareFlash() && isFlash != 0)
			{
				CFlashMac::CreateInstance()->FlashForceStart(isFlash);				
				CManaKernel::CreateInstance()->m_pRunData->flashType = CTRLBOARD_FLASH_LAMPCHECK;
				(CDbInstance::m_cGbtTscDb).SetSystemData("ucDownloadFlag",isFlash);
				return ;
			}
		}
		if ( LAMPBOARD_HEAD_CFG == ucType )
		{	
			for ( int i=1; i<sRecvCanTmp.ucCanDataLen; i++ )
			{
				if ( m_ucCheckCfg[ucLampBoardId]  != sRecvCanTmp.pCanData[i] )
				{
					ACE_DEBUG((LM_DEBUG,"%s:%d Data error %d:%d\n"	,__FILE__,__LINE__,m_ucCheckCfg[ucLampBoardId],sRecvCanTmp.pCanData[i]));				return;
				}
				ACE_DEBUG((LM_DEBUG,"%s:%d  m_ucCheckCfg[%d] = 0x%02x\n"	,__FILE__,__LINE__,ucLampBoardId+1,m_ucCheckCfg[ucLampBoardId]));
			}

		}	
		if(LAMPBOARD_HEAD_VER == ucType)
		{
			m_ucLampBoardVer[ucLampBoardId][0]=sRecvCanTmp.pCanData[1];
			m_ucLampBoardVer[ucLampBoardId][1]=sRecvCanTmp.pCanData[2];
			m_ucLampBoardVer[ucLampBoardId][2]=sRecvCanTmp.pCanData[3];
			m_ucLampBoardVer[ucLampBoardId][3]=sRecvCanTmp.pCanData[4];
			m_ucLampBoardVer[ucLampBoardId][4]=sRecvCanTmp.pCanData[5];
			//ACE_OS::printf("%s:%d LampBoardver[%d]:%d %d %d %d %d \n",__FILE__,__LINE__,ucLampBoardId,sRecvCanTmp.pCanData[1],
				//	sRecvCanTmp.pCanData[2],sRecvCanTmp.pCanData[3],sRecvCanTmp.pCanData[4],sRecvCanTmp.pCanData[5]);
	   		return ;
	}

	if ( LAMPBOARD_HEAD_SETCOLOR == ucType )
		{
	
			for ( int i=1; i<sRecvCanTmp.ucCanDataLen; i++ )
			{
				if ( m_ucLampOnCfg[ucLampBoardId][i-1]  != sRecvCanTmp.pCanData[i] )
				{
					//ACE_DEBUG((LM_DEBUG,"%s:%d Data error %d:%d\n"	,__FILE__,__LINE__, m_ucLampOnCfg[ucLampBoardId][i-1],sRecvCanTmp.pCanData[i]));
					return;
				}
			
			}

		}
	if ( LAMPBOARD_HEAD_TEMPE_CHK == ucType )
	{
			//byte1 �����¶ȼ����¶�ֵ (�¶ȷ�Χ -127 - +127���϶�)
			int iTemp = (int)sRecvCanTmp.pCanData[1];
			if ( iTemp  > -127 && iTemp < 127 )
			{
				iLampBoardTemp[ucLampBoardId] = iTemp;
			}
			else
				ACE_DEBUG((LM_DEBUG,"%s:%d LampBoard %d temprature error!\n",__FILE__,__LINE__,ucLampBoardId));
			return ;
	
	}

	if ( LAMPBOARD_HEAD_ELECT_CHK134 == ucType || LAMPBOARD_HEAD_ELECT_CHK679 == ucType || LAMPBOARD_HEAD_ELECT_CHK1012)
	{
		switch( ucType )
		{
			case LAMPBOARD_HEAD_ELECT_CHK134:
			    m_usLampElect[ucLampBoardId][0]    = sRecvCanTmp.pCanData[1] | sRecvCanTmp.pCanData[2] << 8;
				m_usLampElect[ucLampBoardId][1]    = sRecvCanTmp.pCanData[3] | sRecvCanTmp.pCanData[4] << 8;
				m_usLampElect[ucLampBoardId][2]    = sRecvCanTmp.pCanData[5] | sRecvCanTmp.pCanData[6] << 8;
				break;
			case LAMPBOARD_HEAD_ELECT_CHK679:
				m_usLampElect[ucLampBoardId][3] 	= sRecvCanTmp.pCanData[1] | sRecvCanTmp.pCanData[2] << 8;
				m_usLampElect[ucLampBoardId][4] 	= sRecvCanTmp.pCanData[3] | sRecvCanTmp.pCanData[4] << 8;
				m_usLampElect[ucLampBoardId][5] 	= sRecvCanTmp.pCanData[5] | sRecvCanTmp.pCanData[6] << 8;
				break;
			case LAMPBOARD_HEAD_ELECT_CHK1012:
				m_usLampElect[ucLampBoardId][6] 	= sRecvCanTmp.pCanData[1] | sRecvCanTmp.pCanData[2] << 8;
				m_usLampElect[ucLampBoardId][7] 	= sRecvCanTmp.pCanData[3] | sRecvCanTmp.pCanData[4] << 8;
				break;
			default:
				break;
		}	

		return ;
	}	
	
}



bool CLampBoard::IsFlash()
{
	return m_bSeriousFlash ;
}

/**************************************************************
Function:        CLampBoard::GetLampBoardAddr
Description:     ��ȡ������CAN��ַ
Input:             LampBoardIndex  �ƿذ��±����� 0 1 2 3 4 5 6 7
Output:         ��
Return:          �������ӦCAN��ַ
***************************************************************/
Byte CLampBoard::GetLampBoardAddr(Byte LampBoardIndex)
{
	Byte LampBoardCanAddr = 0 ;
	switch(LampBoardIndex)
	{
		case 0:
			LampBoardCanAddr = BOARD_ADDR_LAMP1 ;
			break ;
		case 1:			
			LampBoardCanAddr = BOARD_ADDR_LAMP2 ;
			break ;
		case 2:
			LampBoardCanAddr = BOARD_ADDR_LAMP3 ;
			break ;
		case 3:			
			LampBoardCanAddr = BOARD_ADDR_LAMP4 ;
			break ;
		case 4:
			LampBoardCanAddr = BOARD_ADDR_LAMP5 ;
			break ;
		case 5:			
			LampBoardCanAddr = BOARD_ADDR_LAMP6 ;
			break ;
		case 6:
			LampBoardCanAddr = BOARD_ADDR_LAMP7 ;
			break ;
		case 7:			
			LampBoardCanAddr = BOARD_ADDR_LAMP8 ;
			break ;
		case 8:
			LampBoardCanAddr = BOARD_ADDR_LAMP9 ;
			break ;
		case 9:			
			LampBoardCanAddr = BOARD_ADDR_LAMPa ;
			break ;
		case 10:
			LampBoardCanAddr = BOARD_ADDR_LAMPb ;
			break ;
		case 11:			
			LampBoardCanAddr = BOARD_ADDR_LAMPc ;
			break ;
		default:
			break;
	}
	return LampBoardCanAddr ;
}

Byte CLampBoard::GetLampBoardVer(Byte LampBoardIndex)
{
	Byte ucLampBoardCanAddr = 0x0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
	sSendFrameTmp.pCanData[0]  = 0xff;	
	sSendFrameTmp.ucCanDataLen = 1;
	ucLampBoardCanAddr = GetLampBoardAddr(LampBoardIndex);
	Can::BuildCanId(CAN_MSG_TYPE_011 , BOARD_ADDR_MAIN , FRAME_MODE_P2P , ucLampBoardCanAddr , &(sSendFrameTmp.ulCanId));			
	Can::CreateInstance()->Send(sSendFrameTmp);
	
}

void CLampBoard::SetLampChannelColor(Byte ColorType,Byte CountDownTime)
{
	CManaKernel *pManakernel = CManaKernel::CreateInstance();
	CGaCountDown *pGaCountDown = CGaCountDown::CreateInstance(); 
	for(Byte LampIndex= 0 ;LampIndex< MAX_LAMP ;LampIndex++)
	{
		if(m_ucLampOn[LampIndex]==0x1 && ColorType == 0x3 && (LampIndex%0x3 ==0x0))
		{
			if( (pManakernel->m_pTscConfig->sChannel[LampIndex/0x3].ucFlashAuto == 0x4)&& (pGaCountDown->GaGetCntTime(LampIndex) ==CountDownTime))
			{
				
				//ACE_OS::printf("%s:%d LampIndex=%d LampGroup =%d \r\n",__FILE__,__LINE__,LampIndex,LampIndex/0x3);
				 m_ucLampFlash[LampIndex]=0x1 ;

			}
		
		}
	}
	 
}


