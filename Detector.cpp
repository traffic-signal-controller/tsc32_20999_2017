
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   Detetctor.cpp
Date:       2013-1-1
Description:�źŻ�������������ļ���
Version:    V1.0
History:
***************************************************************/
#include "ace/Synch.h"
#include <ace/Log_Msg.h>
#include "Define.h"
#include "Detector.h"
#include "ManaKernel.h"
#include "IoOperate.h"
#include "TscMsgQueue.h"
#include "Configure.h"
#include "Can.h"
#include "ComFunc.h"
#include "DbInstance.h"

#ifndef WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ace/Date_Time.h>

#endif

/*
	�������Ȧ����״̬
*/
enum
{
	DET_NORMAL = 0,  //����
	DET_CARVE     ,  //��·
	DET_SHORT     ,  //��·
	DET_STOP         //ͣ��
};

/**************************************************************
Function:        CDetector::CDetector
Description:     �����CDetector�๹�캯������ʼ����			
Input:          ��           
Output:         ��
Return:         ��
***************************************************************/
CDetector::CDetector()
{
#ifdef CHECK_MEMERY
	TestMem(__FILE__,__LINE__);//MOD:201310081445
#endif
	//m_iDevFd   = Serial1::CreateInstance()->GetSerial1Fd();
	m_pTscCfg  = CManaKernel::CreateInstance()->m_pTscConfig; 

	m_iTotalDistance =300;    //5min
	m_iChkType = 0 ;
	ACE_OS::memset(m_iBoardErr  , 0xB , 4);
		
	ACE_OS::memset(m_iDetStatus       , 0 , sizeof(int)  * MAX_DETECTOR);
	ACE_OS::memset(m_ucDetError       , 0 , sizeof(Byte) * MAX_DETECTOR);
	ACE_OS::memset(m_ucLastDetError   , 0 , sizeof(Byte) * MAX_DETECTOR);
	ACE_OS::memset(m_ucDetErrTime     , 0 , sizeof(Byte) * MAX_DETECTOR);
	ACE_OS::memset(m_iDetTimeLen      , 0 , sizeof(int)  * MAX_DETECTOR);
	ACE_OS::memset(m_iDetOccupy       , 0 , sizeof(int)  * MAX_DETECTOR);
	ACE_OS::memset(m_ucTotalStat      , 0 , sizeof(Byte) * MAX_DETECTOR);
	ACE_OS::memset(m_iAdapDetTimeLen  , 0 , sizeof(int)  * MAX_DETECTOR);
	ACE_OS::memset(m_iAdapTotalStat   , 0 , sizeof(int)  * MAX_DETECTOR);
	ACE_OS::memset(m_iLastDetSts      , 0 , sizeof(int)  * MAX_DETECTOR);
	ACE_OS::memset(m_ucRoadSpeed      , 0 , sizeof(Byte) * MAX_DET_BOARD * 8);
	ACE_OS::memset(m_ucSetRoadDis     , 0 , sizeof(Byte) * MAX_DET_BOARD * 8);
	ACE_OS::memset(m_ucGetRoadDis     , 0 , sizeof(Byte) * MAX_DET_BOARD * 8);
	ACE_OS::memset(m_ucSetDetDelicacy , 0 , sizeof(Byte) * MAX_DETECTOR);
	ACE_OS::memset(m_ucGetDetDelicacy , 0 , sizeof(Byte) * MAX_DETECTOR);
	ACE_OS::memset(m_sSetLookLink     , 0 ,  MAX_DET_BOARD * 8);
	ACE_OS::memset(m_sGetLookLink     , 0 ,  MAX_DET_BOARD * 8);
	ACE_OS::memset(m_ucDetSts         , 0 , sizeof(Byte) * MAX_DET_BOARD*MAX_DETECTOR_PER_BOARD); //ADD: 20130710 10 12
	ACE_OS::memset(m_ucNoCnt          , 0 , MAX_DET_BOARD);
	ACE_OS::memset(m_ucErrAddrCnt     , 0 , MAX_DET_BOARD);
	ACE_OS::memset(m_ucErrCheckCnt    , 0 , MAX_DET_BOARD);
	ACE_OS::memset(m_ucRightCnt       , 0 , MAX_DET_BOARD);
	
	ACE_OS::memset(m_ucGetFrency   , 0 , sizeof(Byte) * MAX_DET_BOARD*MAX_DETECTOR_PER_BOARD); //ADD: 20130805 1720
	ACE_OS::memset(m_ucSetFrency   , 0 , sizeof(Byte) * MAX_DET_BOARD*MAX_DETECTOR_PER_BOARD); //ADD: 20130805 1720
	ACE_OS::memset(m_ucSetSensibility   , 0 , sizeof(Byte) * MAX_DET_BOARD*MAX_DETECTOR_PER_BOARD); //ADD: 20130816 1420
	ACE_OS::memset(m_ucGetSensibility   , 0 , sizeof(Byte) * MAX_DET_BOARD*MAX_DETECTOR_PER_BOARD); //ADD: 20130816 1420
	ACE_OS::memset(m_ucDecBoardVer     , 0 , sizeof(Byte)*MAX_DET_BOARD * 5); //ADD:20141201

	ACE_OS::memset(m_bErrFlag     , 0 , sizeof(Byte)*MAX_DETECTOR);
	ACE_OS::memset(m_ucDetError   , DET_CARVE , sizeof(Byte)*MAX_DETECTOR);

	ACE_OS::memset(m_ucDecCarsAnaly   , 0x0 , sizeof(VehicleStat)*MAX_DETECTOR);
	for ( int i=0; i<MAX_DETECTOR; i++ )
	{
		m_ucDecCarsAnaly[i].ucDetId= i+1 ; //MOD: 2015 0812 15 56
		m_ucDecCarsAnaly[i].ulId = i+1 ;
	}
	ACE_DEBUG((LM_DEBUG,"%s:%d Init Detector object ok !\n",__FILE__,__LINE__));

}

/**************************************************************
Function:        CDetector::~CDetector
Description:     �����CDetector��������		
Input:          ��           
Output:         ��
Return:         ��
***************************************************************/
CDetector::~CDetector()
{
	//if ( m_iDevFd > 0 )
	//{
		//#ifndef WINDOWS
		//	close(m_iDevFd);
		//#endif
	//}

	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct Detector object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:        CDetector::CreateInstance
Description:     ����CreateInstance�������̬����		
Input:          ��           
Output:         ��
Return:         ��̬����ָ��
***************************************************************/
CDetector* CDetector::CreateInstance()
{
	static CDetector cDetector; 
	return &cDetector;
}


/**************************************************************
Function:        CDetector::SelectDetectorBoardCfg
Description:     ��ȡ����������������ȡ��������		
Input:          pDetCfg   ��������ýṹ��ָ��           
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SelectDetectorBoardCfg(int *pDetCfg)
{
//	ACE_OS::memset(m_iBoardErr,0,4);
	ACE_OS::memcpy(m_iDetCfg,pDetCfg,sizeof(int)*MAX_DET_BOARD);

	for ( int iIndex=0; iIndex<MAX_DET_BOARD; iIndex++ )
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d m_iDetCfg[%d] = %d\n",__FILE__,__LINE__,iIndex,m_iDetCfg[iIndex] ));
		if ( 1 == m_iDetCfg[iIndex] )
		{
			m_ucActiveBoard1 = iIndex;     // 1-16��Ӧ�ļ������m_ucActiveBoard1 = 0  0-15
		}
		else if ( 17 == m_iDetCfg[iIndex] )
		{
			m_ucActiveBoard2 = iIndex;    // 17 - 32 ��Ӧ�ļ������  m_ucActiveBoard2=1 16-31
		}
		else if(33 == m_iDetCfg[iIndex]) //33-64 ��Ӧ�ӿڰ�1
		{
			m_ucActiveBoard3 = iIndex;
		}
		else if(65 == m_iDetCfg[iIndex]) //65-96 ��Ӧ�ӿڰ�2
		{
			m_ucActiveBoard4 = iIndex;
		}
	}	
	ACE_DEBUG((LM_DEBUG,"%s:%d DetBoard1 index = %d ,DetBoard2 index = %d \n",__FILE__,__LINE__,m_ucActiveBoard1,m_ucActiveBoard2));
}



/**************************************************************
Function:        CDetector::PrintDetInfo
Description:     ��ӡ���յ��ĵ���������Ϣ		
Input:          pFileName:�ļ��� 			iFileLine:�ļ�����  
				ucBoardIndex:����±�		sErrSrc:����ԭ�� 
			   iPrintCnt:��ӡ�ĸ��� 			ucRecvBuf����ӡ���ַ���          
Output:         ��
Return:         ��
***************************************************************/
void CDetector::PrintDetInfo(char* pFileName,int iFileLine,Byte ucBoardIndex, char* sErrSrc,int iPrintCnt,Byte* ucRecvBuf)
{
	if ( ( (m_pTscCfg->sSpecFun[FUN_PRINT_FLAG].ucValue) & 1 )  != 0 )
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d iIndex:%d %s:",pFileName,iFileLine,ucBoardIndex,sErrSrc));
		for ( int iTmp = 0; iTmp<iPrintCnt; iTmp++ )
		{
			ACE_DEBUG((LM_DEBUG,"  %02x",ucRecvBuf[iTmp]));
		}
		ACE_DEBUG((LM_DEBUG,"\n"));
	}
}


/**************************************************************
Function:        CDetector::SelectBrekonCardStatus
Description:     ��ѯĳ���ĳ���״̬������״̬��100msһ��	
Input:         iBoardIndex:����±�  iAddress:��ȡ����Ϣ�ĵ�ַ(Э��涨)          
Output:         �ޣ�AACCVVCXXBBB
Return:         false:�ð�鲻����
***************************************************************/
bool CDetector::SelectBrekonCardStatus(Byte ucBoardIndex,Byte ucAddress)
{
	Byte ucSum         = 0;
	Byte ucTmp         = 0;
	Byte ucDetIndex    = 0;
	Byte ucDetCfgIndex = 0;
	Byte ucData[16]    = {0};
	int iPreReadCnt    = 0;
	int iReadCnt       = 0;
	int iIndex         = 0;

#ifndef WINDOWS
	fd_set rfds;
	struct timeval 	tv;

	//usleep(USLEEP_TIME);
	ioctl(m_iDevFd, com_9bit_enable, 0);
	//usleep(USLEEP_TIME);

	if ( !CIoOprate::TscWrite(m_iDevFd , &ucAddress , sizeof(ucAddress)) )
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d write error",__FILE__,__LINE__));
		return false;
	}

	//usleep(USLEEP_TIME);
	ioctl(m_iDevFd, com_9bit_disable, 0);
	//usleep(USLEEP_TIME);

	ucSum = ~ucAddress;
	if ( !CIoOprate::TscWrite(m_iDevFd , &ucSum , sizeof(ucSum)) )
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d write error",__FILE__,__LINE__));
		return false;
	}

	FD_ZERO(&rfds);
	FD_SET(m_iDevFd, &rfds);
    tv.tv_sec  = 0;
	tv.tv_usec = 20000;
	iReadCnt = select(m_iDevFd + 1, &rfds, NULL, NULL, &tv);
#endif
									 
    if ( -1 == iReadCnt )  /*select error*/
	{
#ifndef WINDOWS
		//usleep(USLEEP_TIME);
		ioctl(m_iDevFd, com_9bit_disable, 0);
		//usleep(USLEEP_TIME);
		SendRecordBoardMsg(ucBoardIndex,3);
	}
	else if ( 0 == iReadCnt )  /*timeout*/
	{
		if( m_iBoardErr[ucBoardIndex] != DEV_IS_DISCONNECTED )
		{
			m_iBoardErr[ucBoardIndex] = DEV_IS_DISCONNECTED;
		}
		//usleep(USLEEP_TIME);
		ioctl(m_iDevFd, com_9bit_disable, 0);
		//usleep(USLEEP_TIME);

		//ACE_DEBUG((LM_DEBUG,"%s:%d timeout(%d)\n",__FILE__,__LINE__,ucBoardIndex));
		SendRecordBoardMsg(ucBoardIndex,3);
		PrintDetInfo((char*)"CDetector.cpp",__LINE__,ucBoardIndex,(char*)"Timeout",0,NULL);
#endif
	}
	else  /*�0�8�0�9�0�8�0�4�0�8�0�5�0�0�0�6*/
	{
		m_iBoardErr[ucBoardIndex] = DEV_IS_CONNECTED;

#ifndef WINDOWS
		//ACE_DEBUG((LM_DEBUG,"%s:%d ucBoardIndex:%d value:%d\n"
			//,__FILE__,__LINE__,ucBoardIndex,m_iBoardErr[ucBoardIndex]));
		
		//usleep(USLEEP_TIME);
		ioctl(m_iDevFd, com_9bit_disable, 0);
		//usleep(USLEEP_TIME);
#endif

		if ( 0 == ucBoardIndex || 1 == ucBoardIndex ) 
		{
			iPreReadCnt = 5;
		}
		else
		{
			iPreReadCnt = 9;
		}

#ifndef WINDOWS
		//iReadCnt = read(m_iDevFd, ucData ,iPreReadCnt);
		if ( !CIoOprate::TscRead(m_iDevFd , ucData , iPreReadCnt) )
		{
			//ACE_DEBUG((LM_DEBUG,"%s:%d write error\n",__FILE__,__LINE__));
			return false;
		}
#endif

#ifdef WINDOWS
		 if ( 2 == ucBoardIndex )
		{			
			static bool bFlag = true;

			if ( bFlag )
			{
				ucData[0] = 0x12; 
				ucData[1] = 0x06;
				ucData[2] = 0x04;
				ucData[3] = 0;
				ucData[4] = 0;
				ucData[5] = 0x55;
				ucData[6] = 0x55;
				ucData[7] = 0x45;
				ucData[8] = 0xf4;
			}
			else
			{
				ucData[0] = 0x12; 
				ucData[1] = 6;
				ucData[2] = 0;
				ucData[3] = 0;
				ucData[4] = 0;
				ucData[5] = 0;
				ucData[6] = 0;
				ucData[7] = 0;
				ucData[8] = 0xE7;
			}
			bFlag = !bFlag;
		}
		else
		{
			return false;
		}
#endif
		iReadCnt = iPreReadCnt;
		
		if ( ucData[0] != ( ucAddress + 16 ) )  //��ַ��
		{			
			SendRecordBoardMsg(ucBoardIndex,2);
			PrintDetInfo((char*)"Detector.cpp",__LINE__,ucBoardIndex,(char*)"Addr Error",iReadCnt,ucData);
			return false;
		}
		else
		{
			iIndex = 0;
			ucSum = 0;
			while ( iIndex < (iReadCnt-1) )
			{
				ucSum +=  ucData[iIndex];
				iIndex++;
			}
			ucSum = ~ucSum;

			if ( ucSum != ucData[iReadCnt-1] )
			{
				SendRecordBoardMsg(ucBoardIndex,1);
				PrintDetInfo((char*)"Detector.cpp",__LINE__,ucBoardIndex,(char*)"Check Sum Error",iReadCnt,ucData);
				return false;
			}

			SendRecordBoardMsg(ucBoardIndex,0);
			PrintDetInfo((char*)"Detector.cpp",__LINE__,ucBoardIndex,(char*)"Right",iReadCnt,ucData);

			if ( (0x10+ucBoardIndex) == ucData[0] )
			{
				//����״̬
				ucTmp = ucData[2];
				iIndex = 0;
				while ( iIndex < 8 )
				{
					m_iDetStatus[16*ucBoardIndex+iIndex]  = 0; 

					if ( 1 == ( (ucTmp>>iIndex) & 1 ) )
					{
						m_iDetTimeLen[16*ucBoardIndex+iIndex] += 1;     //����ռ����
						m_iDetStatus[16*ucBoardIndex+iIndex]  = 1;      //�������г�

						m_iAdapDetTimeLen[16*ucBoardIndex+iIndex] += 1;
						
						if ( 0 == m_iLastDetSts[16*ucBoardIndex+iIndex] )
						{
							m_ucTotalStat[16*ucBoardIndex+iIndex] += 1;    //���㳵���� �ϴ��޳��˴��г���Ϊ�г� 
							m_iAdapTotalStat[16*ucBoardIndex+iIndex] += 1; 
						}
					}
					m_iLastDetSts[16*ucBoardIndex+iIndex] = (ucTmp>>iIndex) & 1;
					iIndex++;
				}

				ucTmp = ucData[3];
				iIndex = 0;
				while ( iIndex < 8 )
				{
					m_iDetStatus[16*ucBoardIndex+8+iIndex]  = 0;  

					if ( 1 == ( (ucTmp>>iIndex) & 1 ) )
					{
						m_iDetTimeLen[16*ucBoardIndex+8+iIndex]     += 1;     //����ռ����
						m_iDetStatus[16*ucBoardIndex+8+iIndex]       = 1;     //�������г�
						m_iAdapDetTimeLen[16*ucBoardIndex+8+iIndex] += 1;   

						if ( 0 == m_iLastDetSts[16*ucBoardIndex+8+iIndex] )
						{
							m_ucTotalStat[16*ucBoardIndex+8+iIndex]     += 1;    //���㳵���� �ϴ��޳��˴��г���Ϊ�г� 
							m_iAdapTotalStat[16*ucBoardIndex+8+iIndex] += 1;
						}
					}
					m_iLastDetSts[16*ucBoardIndex+8+iIndex] = (ucTmp>>iIndex) & 1;
					iIndex++;
				}
				
				if ( 0 == ucBoardIndex || 1 == ucBoardIndex )  //������ӿڰ�û�й���״̬��Ϣ
				{
					for ( int i=0; i<16; i++ )
					{
						m_ucDetError[16*ucBoardIndex+i] = DET_NORMAL;
					}
					return true;
				}
				//����״̬
				for (int i = 0; i< 4; i++ )
				{
					ucTmp = ucData[4+i];
					for (int j=0; j<4; j++)
					{
						ucDetIndex = 16 * ucBoardIndex + i * 4 + j;
						m_ucDetError[ucDetIndex] = (Byte)((ucTmp>>(2*j)) & 3);
						
						if ( m_ucDetError[ucDetIndex] == m_ucLastDetError[ucDetIndex] )
						{
							if ( m_ucDetErrTime[ucDetIndex] > 15 )
							{
								if ( ( m_bErrFlag[ucDetIndex] && m_ucDetError[ucDetIndex] != DET_NORMAL)
								  || (!m_bErrFlag[ucDetIndex] && m_ucDetError[ucDetIndex] == DET_NORMAL) )
								{
									m_bErrFlag[ucDetIndex] = !m_bErrFlag[ucDetIndex];

									ucDetCfgIndex = m_pTscCfg->iDetCfg[ucDetIndex/MAX_DETECTOR_PER_BOARD] - 1
													+ ucDetIndex % MAX_DETECTOR_PER_BOARD;
									if ( ucDetCfgIndex < MAX_DETECTOR 
										&& m_pTscCfg->sDetector[ucDetCfgIndex].ucPhaseId != 0 )  //�������õļ�����ż�¼
									{
										//���ͼ�¼����������
										//�ȴ���������ϼ���ȶ����Ÿù���
										SThreadMsg sTscMsg;
										sTscMsg.ulType       = TSC_MSG_LOG_WRITE;
										sTscMsg.ucMsgOpt     = LOG_TYPE_DETECTOR;
										sTscMsg.uiMsgDataLen = 4;
										sTscMsg.pDataBuf     = ACE_OS::malloc(4);
										((Byte*)(sTscMsg.pDataBuf))[0] = 0;
										((Byte*)(sTscMsg.pDataBuf))[1] = 0;
										((Byte*)(sTscMsg.pDataBuf))[2] = m_ucDetError[ucDetIndex];
										((Byte*)(sTscMsg.pDataBuf))[3] = ucDetCfgIndex + 1; 
										CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));
									}
								}
							}
							else
							{
								m_ucDetErrTime[ucDetIndex]++;
							}
						}
						else
						{
							m_ucLastDetError[ucDetIndex] = m_ucDetError[ucDetIndex];
							m_ucDetErrTime[ucDetIndex]   = 0;
						}
						 
					}
				}
			}
		}
	
		return true;
	}

	return false;
}


/**************************************************************
Function:        CDetector::SendRecordBoardMsg
Description:     ���ͼ�¼�ƿذ�ͨ��״̬	
Input:          ucType  - 0��ȷ 1У����� 2��ַ���� 3û������        
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendRecordBoardMsg(Byte ucDetIndex,Byte ucType)
{
	Byte ucCnt   = 0;
	Byte ucByte0 = 0;
	Byte ucByte1 = 0;

	if ( ( 0 == ucType && m_bRecordSts[ucDetIndex] ) || ( ucType != 0 && !m_bRecordSts[ucDetIndex] ) )
	{
		switch ( ucType )
		{
			case 0:  //��ȷ	
				m_ucErrCheckCnt[ucDetIndex] = 0;
				m_ucNoCnt[ucDetIndex]       = 0;          
				m_ucErrAddrCnt[ucDetIndex]  = 0; 
				return;
			case 1:   //У�����
				m_ucNoCnt[ucDetIndex]       = 0;          
				m_ucErrAddrCnt[ucDetIndex]  = 0;     
				m_ucRightCnt[ucDetIndex]    = 0;
				return;
			case 2:   //��ַ����
				m_ucErrCheckCnt[ucDetIndex] = 0;
				m_ucNoCnt[ucDetIndex]       = 0;            
				m_ucRightCnt[ucDetIndex]    = 0;
				return;
			case 3:  //û������
				m_ucErrCheckCnt[ucDetIndex] = 0;        
				m_ucErrAddrCnt[ucDetIndex]  = 0;     
				m_ucRightCnt[ucDetIndex]    = 0;
				return;
			default:
				return;
		}
	}

	switch ( ucType )
	{
		case 0:
			m_ucRightCnt[ucDetIndex]++;
			ucCnt = m_ucRightCnt[ucDetIndex];
			break;
		case 1:
			m_ucErrCheckCnt[ucDetIndex]++;
			ucCnt =  m_ucErrCheckCnt[ucDetIndex];
			break;
		case 2:
			m_ucErrAddrCnt[ucDetIndex]++;
			ucCnt = m_ucErrAddrCnt[ucDetIndex];
			break;
		case 3:
			m_ucNoCnt[ucDetIndex]++;
			ucCnt = m_ucNoCnt[ucDetIndex];
			break;
		default:
			return;
	}

	if ( ucCnt > BOARD_REPEART_TIME )
	{
		switch ( ucType )
		{
			case 0:  //��ȷ
				m_bRecordSts[ucDetIndex] = true;
				m_ucErrCheckCnt[ucDetIndex] = 0;
				m_ucNoCnt[ucDetIndex]       = 0;          
				m_ucErrAddrCnt[ucDetIndex]  = 0; 
				ucByte0 = 0;
				ucByte1 = 0;
				break;
			case 1:  //У�����
				m_bRecordSts[ucDetIndex] = false;
				m_ucNoCnt[ucDetIndex]       = 0;          
				m_ucErrAddrCnt[ucDetIndex]  = 0;     
				m_ucRightCnt[ucDetIndex]    = 0;
				ucByte0 = 1;
				ucByte1 = 2;
				break;
			case 2:  //��ַ����
				m_bRecordSts[ucDetIndex] = false;
				m_ucErrCheckCnt[ucDetIndex] = 0;
				m_ucNoCnt[ucDetIndex]       = 0;            
				m_ucRightCnt[ucDetIndex]    = 0;
				ucByte0 = 1;
				ucByte1 = 1;
				break;
			case 3:  //û������
				m_bRecordSts[ucDetIndex] = false;
				m_ucErrCheckCnt[ucDetIndex] = 0;        
				m_ucErrAddrCnt[ucDetIndex]  = 0;     
				m_ucRightCnt[ucDetIndex]    = 0;
				ucByte0 = 1;
				ucByte1 = 0;
				break;
			default:
				return;
		}
	}
	else
	{
		return;
	}

	SThreadMsg sTscMsg;
	sTscMsg.ulType       = TSC_MSG_LOG_WRITE;
	sTscMsg.ucMsgOpt     = LOG_TYPE_DETBOARD;
	sTscMsg.uiMsgDataLen = 4;
	sTscMsg.pDataBuf     = ACE_OS::malloc(4);
	((Byte*)(sTscMsg.pDataBuf))[0] = 0;
	((Byte*)(sTscMsg.pDataBuf))[1] = ucDetIndex;
	((Byte*)(sTscMsg.pDataBuf))[2] = ucByte1;
	((Byte*)(sTscMsg.pDataBuf))[3] = ucByte0; 
	CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));

	//ACE_DEBUG((LM_DEBUG,"%s:%d ucDetIndex:%d ucByte1:%d ucByte0:%d\n"
	//	,__FILE__,__LINE__,ucDetIndex,ucByte1,ucByte0));
}


/**************************************************************
Function:        CDetector::SearchAllStatus
Description:     ��ȡ������峵��״̬������״̬��100msһ��	
Input:        	  bchkdetstatus -�Ƿ��ѯ�������ӿڰ����Ӻ�ͨ��״̬
		    	  bchkcar - �Ƿ���������ӿڰ�ͨ���Ƿ��г�
Output:         ��
Return:         ��
***************************************************************/
 void CDetector::SearchAllStatus(bool bchkcar ,bool bchkdetstatus)
 {
	Byte ucIndex = 0;
	while ( ucIndex < MAX_DET_BOARD )
	{
		//if ( ( DEV_IS_CONNECTED == m_iBoardErr[ucIndex] )	&& ( m_iDetCfg[ucIndex] != 0 ))
		//{			
			//SelectBrekonCardStatus(ucIndex, ucIndex);  //��iIndexƬ������峵��״̬������״̬ MOD:20130723 1620
			if(bchkdetstatus)
		 	{
				GetAllVehSts(DET_HEAD_STS,ucIndex);    //��ѯ�����ͨ��״̬  ��· ���� ��·�������
				GetDecVars(ucIndex,0xff);                         //��ѯÿ��������ӿڰ�����״̬ ADD 2015-02-02
		    }
			if(bchkcar) //ADD:2013 0724 1010
			{	
				GetAllVehSts(DET_HEAD_VEHSTS,ucIndex); //��ѯ������г��޳�ͨ��
			}		 
	//}		
		ucIndex++;
	}	
	//ACE_Date_Time tvTime(ACE_OS::gettimeofday());	
	//ACE_DEBUG((LM_DEBUG,"%s:%d  Query car!Time %d:%d\n",__FILE__,__LINE__,tvTime.minute(),tvTime.second()));

}

/**************************************************************

Function:        CDetector::GetAllWorkSts
Description:     ��ȡ���м������Ĺ���״̬��һ������һ��	
Input:          ��        
Output:         ��
Return:         ��
***************************************************************/
void CDetector::GetAllWorkSts()
{
	Byte ucIndex = 0;	

	while ( ucIndex < MAX_DET_BOARD )
	{
		if ( ( DEV_IS_CONNECTED == m_iBoardErr[ucIndex] )&& (m_iDetCfg[ucIndex] != 0 ))
		{	 
			GetAllVehSts(DET_HEAD_STS,ucIndex);   //��ѯ�����ͨ��״̬  ��· ���� ��·�������								
		}		
		ucIndex++;
	}


}

/**************************************************************
Function:        CDetector::SendDetLink
Description:     ���ü������Ӧ��ϵ ��Ȧ��Ӧ��ϵ	
Input:          ucBoardIndex - 0    1    2    3   �����±�  
				SetType  - DET_HEAD_COIL0104_SET 
						   DET_HEAD_COIL0508_SET ����������      
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendDetLink(Byte ucBoardIndex,Byte SetType)
{
	
		Byte iNdex = 0 ;
		Byte BoardAddr = 0;
		SCanFrame sSendFrameTmp;
		ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
		
		if(SetType != DET_HEAD_COIL0104_SET && SetType != DET_HEAD_COIL0508_SET)
		{
			ACE_DEBUG((LM_DEBUG,"%s:%d, settype:%02x \n", __FILE__, __LINE__, SetType));
			return ;

		}
		BoardAddr = GetDecAddr(ucBoardIndex);			

		Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));

		sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | SetType;
		
		 if(SetType == DET_HEAD_COIL0508_SET)
			iNdex = 4 ;
		sSendFrameTmp.pCanData[1] = m_sSetLookLink[ucBoardIndex][0+iNdex]; 		
		sSendFrameTmp.pCanData[2] = m_sSetLookLink[ucBoardIndex][1+iNdex];		
		sSendFrameTmp.pCanData[3] = m_sSetLookLink[ucBoardIndex][2+iNdex]; 		
		sSendFrameTmp.pCanData[4] = m_sSetLookLink[ucBoardIndex][3+iNdex];  	
		
		sSendFrameTmp.ucCanDataLen = 5;	
		Can::CreateInstance()->Send(sSendFrameTmp);
		ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:%02x  %02x  %02x  %02x  %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,SetType,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2],sSendFrameTmp.pCanData[3],sSendFrameTmp.pCanData[4]));  

}


/**************************************************************
Function:        CDetector::GetDecAddr
Description:     ͨ�������±��ȡ����CAN��ַ	
Input:          ucBoardIndex - 0    1    2    3   �����±�   
Output:         ��
Return:         ��������ַ
***************************************************************/
Byte CDetector::GetDecAddr(Byte ucBoardIndex)
{
		Byte BoardAddr = 0 ;
		switch(ucBoardIndex)
		{
			case 0:
				BoardAddr = BOARD_ADDR_DETECTOR1 ;
				break ;
			case 1:
				BoardAddr = BOARD_ADDR_DETECTOR2 ;
				break ;
			case 2:
				BoardAddr = BOARD_ADDR_INTEDET1 ;
				break ;
			case 3:
				BoardAddr = BOARD_ADDR_INTEDET2 ;
				break ;
			default:
				return 0;
		}	

		return BoardAddr ;

}



/**************************************************************
Function:        CDetector::SendDelicacy
Description:     ���ü���������ͨ���������ȵȼ�
Input:          ucBoardIndex - 0 1 2 3
			   SetType DET_HEAD_SEN0108_SET  
			   		   DET_HEAD_SEN0916_SET ͨ�������� 
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendDelicacy(Byte ucBoardIndex,Byte SetType)
{
	Byte iNdex = 0 ;
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
		
	if(SetType != DET_HEAD_SEN0108_SET && SetType != DET_HEAD_SEN0916_SET)
	{
		return ;
	}
	BoardAddr = GetDecAddr( ucBoardIndex);			

	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | SetType;

	 if(SetType == DET_HEAD_SEN0916_SET)
		iNdex = 8 ;
	sSendFrameTmp.pCanData[1] = m_ucSetDetDelicacy[ucBoardIndex][0+iNdex];  ; 
	sSendFrameTmp.pCanData[1] |=  m_ucSetDetDelicacy[ucBoardIndex][1+iNdex]<<4; 
		
	sSendFrameTmp.pCanData[2] = m_ucSetDetDelicacy[ucBoardIndex][2+iNdex];  ; 
	sSendFrameTmp.pCanData[2] |=  m_ucSetDetDelicacy[ucBoardIndex][3+iNdex]<<4; 
		
	sSendFrameTmp.pCanData[3] = m_ucSetDetDelicacy[ucBoardIndex][4+iNdex];  ; 
	sSendFrameTmp.pCanData[3] |=  m_ucSetDetDelicacy[ucBoardIndex][5+iNdex]<<4; 

	sSendFrameTmp.pCanData[4] = m_ucSetDetDelicacy[ucBoardIndex][6+iNdex];  ; 
	sSendFrameTmp.pCanData[4] |=  m_ucSetDetDelicacy[ucBoardIndex][7+iNdex]<<4; 
		
	sSendFrameTmp.ucCanDataLen = 5;	

	Can::CreateInstance()->Send(sSendFrameTmp);
	ACE_DEBUG((LM_DEBUG,"%s:%d DecBoard:%d SetType:Delicacy  %02x - %02x - %02x - %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2],sSendFrameTmp.pCanData[3],sSendFrameTmp.pCanData[4]));


}



/**************************************************************
Function:        CDetector::GetDecVars
Description:    ��ѯ���������״̬���͵�ֵ
Input:          ucBoardIndex - 0 1 2 3
   				SetType  ��ѯ����
Output:         ��
Return:         ��
***************************************************************/
void CDetector::GetDecVars(Byte ucBoardIndex,Byte GetType)
{		
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
		
	if(ucBoardIndex<0 || ucBoardIndex >3)
		return ;
	BoardAddr = GetDecAddr( ucBoardIndex);	

	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));

	if(GetType == 0xff)
		sSendFrameTmp.pCanData[0]= 0xff;
	else if(GetType == 0x0)		
		sSendFrameTmp.pCanData[0]= 0x0;
	else
		sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | GetType;
	sSendFrameTmp.ucCanDataLen = 1;	

	Can::CreateInstance()->Send(sSendFrameTmp);

}



/**************************************************************
Function:        CDetector::SendRoadDistance
Description:    ���ü��������Ȧ���� 
Input:          ucBoardIndex - 0 1 2 3
   				SetType  ��������
   						 DET_HEAD_DISTAN0104_SET DET_HEAD_DISTAN0508_SET
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendRoadDistance(Byte ucBoardIndex,Byte SetType)
{
	Byte iNdex = 0 ;
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
		
	if(SetType != DET_HEAD_DISTAN0104_SET && SetType != DET_HEAD_DISTAN0508_SET)
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d, settype:%02x \n", __FILE__, __LINE__, SetType));
		return ;
	}
	BoardAddr = GetDecAddr( ucBoardIndex);			

	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | SetType;
		
	if(SetType == DET_HEAD_DISTAN0508_SET)
		iNdex = 4 ;
	sSendFrameTmp.pCanData[1] = m_ucSetRoadDis[ucBoardIndex][0+iNdex]; 		
	sSendFrameTmp.pCanData[2] = m_ucSetRoadDis[ucBoardIndex][1+iNdex];		
	sSendFrameTmp.pCanData[3] = m_ucSetRoadDis[ucBoardIndex][2+iNdex]; 		
	sSendFrameTmp.pCanData[4] = m_ucSetRoadDis[ucBoardIndex][3+iNdex];  	
		
	sSendFrameTmp.ucCanDataLen = 5;	
	Can::CreateInstance()->Send(sSendFrameTmp);
	ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:RoadDistance %02x - %02x - %02x - %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2],sSendFrameTmp.pCanData[3],sSendFrameTmp.pCanData[4]));
}


/**************************************************************
Function:        CDetector::SendDecWorkType
Description:    ���ü����������ʽ  0 -- ������ 1--������
Input:          ucBoardIndex - 0 1 2 3
   				
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendDecWorkType(Byte ucBoardIndex)
{	
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	

	BoardAddr = GetDecAddr( ucBoardIndex);			

	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | DET_HEAD_WORK_SET;		
	sSendFrameTmp.pCanData[1] =m_iChkType; 		
	sSendFrameTmp.ucCanDataLen = 2;	
	Can::CreateInstance()->Send(sSendFrameTmp);
	ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:DecWorkType %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,m_iChkType));
}



/**************************************************************
Function:        CDetector::SendDecIsLink
Description:    ���ü�������Ƿ��������Ȧ��
Input:          ucBoardIndex - 0 1 2 3
   				IsAllowLink true-���� false-������
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendDecIsLink(Byte ucBoardIndex,Byte IsAllowLink)
{
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	

	BoardAddr = GetDecAddr( ucBoardIndex);			
	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | DET_HEAD_COILALLOW_SET;		
	sSendFrameTmp.pCanData[1] =IsAllowLink; 		
	sSendFrameTmp.ucCanDataLen = 2;	
	Can::CreateInstance()->Send(sSendFrameTmp);
}



/**************************************************************
Function:        CDetector::SendDecFrency
Description:    ���ü��������Ƶ��
Input:          ucBoardIndex - 0 1 2 3
   				IsAllowLink true-���� false-������
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendDecFrency(Byte ucBoardIndex)
{		
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	
	
	BoardAddr = GetDecAddr( ucBoardIndex);			
	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));
	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | DET_HEAD_FRENCY_SET;		
	for(int i = 0 ; i < MAX_DETECTOR_PER_BOARD ; i++)			
		sSendFrameTmp.pCanData[i/4+1] |= m_ucSetFrency[ucBoardIndex][i] <<(i%4*2); 		
	sSendFrameTmp.ucCanDataLen = 5;	
	Can::CreateInstance()->Send(sSendFrameTmp);
	ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:DecFrency %02x - %02x - %02x - %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2],sSendFrameTmp.pCanData[3],sSendFrameTmp.pCanData[4]));

}




/**************************************************************
Function:        CDetector::SendDecSenData
Description:    ���ü��������������ֵ
Input:          ucBoardIndex - 0 1 2 3
   				ucSetType ����������������
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SendDecSenData(Byte ucBoardIndex,Byte ucSetType)
{	
	Byte BoardAddr = 0;
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));	

	if(ucSetType != DET_HEAD_SENDATA0107_SET && ucSetType != DET_HEAD_SENDATA0814_SET  && ucSetType != DET_HEAD_SENDATA1516_SET)
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d, settype:%02x \n", __FILE__, __LINE__, ucSetType));
		return ;
	}
	BoardAddr = GetDecAddr( ucBoardIndex);			

	Can::BuildCanId(CAN_MSG_TYPE_101 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P , BoardAddr  , &(sSendFrameTmp.ulCanId));

	sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND<< 6) | ucSetType;		
		
	 if(ucSetType == DET_HEAD_SENDATA0107_SET)
	{			
		for(int ucIndex = 0 ; ucIndex < 7 ;ucIndex++)
		{
			sSendFrameTmp.pCanData[1+ucIndex] = m_ucSetSensibility[ucBoardIndex][ucIndex]; 
			
		}
		sSendFrameTmp.ucCanDataLen = 8;	
		ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:DecSenData %02x - %02x - %02x - %02x - %02x - %02x - %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2],sSendFrameTmp.pCanData[3],sSendFrameTmp.pCanData[4],sSendFrameTmp.pCanData[5],sSendFrameTmp.pCanData[6],sSendFrameTmp.pCanData[7]));
	}
	else if(ucSetType == DET_HEAD_SENDATA0814_SET)
	{
		for(int ucIndex = 0 ; ucIndex < 7 ;ucIndex++)
		{
			sSendFrameTmp.pCanData[1+ucIndex] = m_ucSetSensibility[ucBoardIndex][7+ucIndex]; 
			
		}
			sSendFrameTmp.ucCanDataLen = 8;
			ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:DecSenData %02x - %02x - %02x - %02x - %02x - %02x - %02x\n ",__FILE__,__LINE__,ucBoardIndex+1,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2],sSendFrameTmp.pCanData[3],sSendFrameTmp.pCanData[4],sSendFrameTmp.pCanData[5],sSendFrameTmp.pCanData[6],sSendFrameTmp.pCanData[7]));
		}
	else if(ucSetType == DET_HEAD_SENDATA1516_SET)
	{
		sSendFrameTmp.pCanData[1] = m_ucSetSensibility[ucBoardIndex][14];
		sSendFrameTmp.pCanData[2] = m_ucSetSensibility[ucBoardIndex][15];
		sSendFrameTmp.ucCanDataLen = 3;
		ACE_DEBUG((LM_DEBUG,"\n%s:%d DecBoard:%d SetType:DecSenData %02x - %02x  \n ",__FILE__,__LINE__,ucBoardIndex+1,sSendFrameTmp.pCanData[1],sSendFrameTmp.pCanData[2]));
	}				
	Can::CreateInstance()->Send(sSendFrameTmp);
	
}


/**************************************************************
Function:        CDetector::GetOccupy
Description:    ��ȡռ���� 1s per  5minͳ��һ��
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CDetector::GetOccupy()
{
	static int iTick = 0;
	Ushort iIndex       = 0;
	CManaKernel *pManaKernel = CManaKernel::CreateInstance();
	iTick++;
	//ACE_DEBUG((LM_DEBUG,"%s:%d ******iTick =%d m_iTotalDistance=%d \n",__FILE__,__LINE__,iTick,m_iTotalDistance));
	if ( iTick <pManaKernel->m_pRunData->ucCycle-1)// m_iTotalDistance )  //5min 5*60
	{		
		//ACE_DEBUG((LM_DEBUG,"%s:%d ******iTick =%d cycle=%d \n",__FILE__,__LINE__,iTick,pManaKernel->m_pRunData->ucCycle));
		return;
	}  
	iTick = 0;
	//ACE_Guard<ACE_Thread_Mutex> guard(m_sMutex);
	//ACE_OS::memcpy( m_iLastDetTimeLen , m_iDetTimeLen , sizeof(int)*MAX_DETECTOR );
	iIndex = MAX_DETECTOR;		
	while ( iIndex-- >0)
	{
		if(pManaKernel->m_pTscConfig->sDetector[iIndex].ucPhaseId>0)
		{	
			//ACE_DEBUG((LM_DEBUG,"%s:%d ******DetecId =%d ucPhaseId=%d cars=%d m_iDetTimeLen=%d\n",__FILE__,__LINE__,iIndex+1,pManaKernel->m_pTscConfig->sDetector[iIndex].ucPhaseId,m_ucTotalStat[iIndex],m_iDetTimeLen[iIndex]));
		
			m_iDetOccupy[iIndex] = (m_iDetTimeLen[iIndex]*1.0*100/ (pManaKernel->m_pRunData->ucCycle)) ; 
			pManaKernel->m_pTscConfig->sDetector[iIndex].ucSaturationOccupy = m_iDetOccupy[iIndex] ;
			
			pManaKernel->m_pTscConfig->sDetector[iIndex].usSaturationFlow = m_ucTotalStat[iIndex] ;
			 (CDbInstance::m_cGbtTscDb).ModDetector(pManaKernel->m_pTscConfig->sDetector[iIndex].ucDetectorId,pManaKernel->m_pTscConfig->sDetector[iIndex] );
			(CDbInstance::m_cGbtTscDb).AddVehicleStat(iIndex+1 , m_ucTotalStat[iIndex] ,  m_iDetOccupy[iIndex]);
		}
	}
	
	ACE_OS::memset(m_iDetTimeLen , 0 , sizeof(int)*MAX_DETECTOR);
	ACE_OS::memset(m_ucTotalStat  , 0 , sizeof(Byte)*MAX_DETECTOR);
}


/**************************************************************
Function:        CDetector::GetActiveDetSum
Description:    ��ȡ����������
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
int CDetector::GetActiveDetSum()
{
	int iSum   = 0;
	int iIndex = 0;
	int jIndex = 0;
	//ACE_Guard<ACE_Thread_Mutex> guard(m_sMutex);
	while ( iIndex < 4 )
	{
		if ( DEV_IS_CONNECTED == m_iBoardErr[iIndex] )
		{
			jIndex = 0;
			while ( jIndex < 16)
			{
				if ( DET_NORMAL == m_ucDetError[iIndex*16+jIndex] )
				{
					iSum += 1;
				}
				jIndex++;
			}
			
		}
		iIndex++;
	}
	//ACE_DEBUG((LM_DEBUG,"%s:%d iSum:%d\n",__FILE__,__LINE__,iSum));
	return iSum;
}



/**************************************************************
Function:        CDetector::GetDetBoardType
Description:    ��ȡ������������
Input:          ��
Output:         ��
Return:         0:����� 1:������ӿڰ�
***************************************************************/
int CDetector::GetDetBoardType()
{
	if ( (DEV_IS_CONNECTED == m_iBoardErr[0]) || (DEV_IS_CONNECTED == m_iBoardErr[1]) )
	{
		return 1;
	}

	return 0;
}

/**************************************************************
Function:        CDetector::GetDetStatus
Description:    ��ȡ������������
Input:          ��
Output:         pDetStatus �����״̬�ṹ������ָ��
Return:         ��
***************************************************************/
void CDetector::GetDetStatus(SDetectorSts* pDetStatus)
{
	int iIndex    = 0;
	int jIndex    = 0;
	int iDetId    = 0;
	Byte ucStutas = 0;
	Byte ucAlarm  = 0;
	Byte ucBoardIndex      = m_ucActiveBoard1;
	
	while ( iIndex < 8 )
	{
		(pDetStatus+iIndex)->ucId = iIndex + 1;
		iIndex++;
	}

	iIndex = 0;
	while ( iIndex < 8 )
	{
		ucStutas = 0;
		ucAlarm  = 0;
		jIndex   = 0;

		if ( (2 == iIndex) || (3 == iIndex) )  
		{
			ucBoardIndex = m_ucActiveBoard2;
		}

		while ( jIndex < 8 )
		{
			if(iIndex <4)
			{
				
				iDetId	  = MAX_DETECTOR_PER_BOARD * ucBoardIndex + ( iIndex % 2 ) * 8 + jIndex;
				if ( ( m_ucDetError[iDetId] != DET_NORMAL )  && ( m_pTscCfg->sDetector[iIndex*8+jIndex].ucPhaseId != 0 ) )
				{
					ucAlarm  |= (1<<jIndex);
				}
			}
			else
			{				
				iDetId	  = MAX_INTERFACE_PER_BOARD  + ( iIndex % 4) * 8 + jIndex;
			}
			ucStutas |= (m_iDetStatus[iDetId]<<jIndex);			
			jIndex++;
		}
		(pDetStatus+iIndex)->ucStatus = ucStutas;
		(pDetStatus+iIndex)->ucAlarm  = ucAlarm;
		iIndex++;
	}
}


/**************************************************************
Function:        CDetector::GetDetData
Description:    ��ȡ��ͨ��������ݱ�
Input:          ��
Output:         pDetData ��ͨ������ݽṹ��ָ��
Return:         ��
***************************************************************/
void CDetector::GetDetData(SDetectorData* pDetData)
{
	Byte ucBoardIndex = m_ucActiveBoard1;
	int iIndex    = 0;
	int iDetId    = 0;

	while ( iIndex < 48 )
	{
		(pDetData+iIndex)->ucId = iIndex + 1;
		iIndex++;
	}

	iIndex    = 0;
	while ( iIndex < 32 )
	{
		if ( iIndex >= MAX_DETECTOR_PER_BOARD )
		{
			ucBoardIndex = m_ucActiveBoard2;  //16 - 31
		}
		iDetId = ucBoardIndex * MAX_DETECTOR_PER_BOARD + iIndex % 16;
		//(pDetData+iIndex)->ucId        = iIndex + 1;
		(pDetData+iIndex)->ucVolume      = m_ucTotalStat[iDetId];
		(pDetData+iIndex)->ucLongVolume  = 0;
		(pDetData+iIndex)->ucSmallVolume = 0;
		
		//��һ�����ڵ�ռ����
		(pDetData+iIndex)->ucOccupancy = m_iDetOccupy[iDetId] * 2; //��λ0.5
		//��ǰ��ռ����
		//(pDetData+iIndex)->ucOccupancy   = m_iDetTimeLen[iDetId] * 100 * 2 / ( m_iTotalDistance * 10 ) ; //��λ0.5		
	       (pDetData+iIndex)->ucVelocity  = m_iDetSpeedAvg[iDetId];
		(pDetData+iIndex)->ucVehLen      = 0;
		iIndex++;
	}

#if 0
	iIndex = 0;
	while ( iIndex < 48 )
	{
		ACE_DEBUG((LM_DEBUG,"id:%d Volume:%d LongVolume:%d SmallVolume:%d Occupancy:%d Velocity:%d VehLen:%d\n"
			,(pDetData+iIndex)->ucId
			,(pDetData+iIndex)->ucVolume
			,(pDetData+iIndex)->ucLongVolume
			,(pDetData+iIndex)->ucSmallVolume
			,(pDetData+iIndex)->ucOccupancy
			,(pDetData+iIndex)->ucVelocity
			,(pDetData+iIndex)->ucVehLen));
		iIndex++;
	}
	ACE_DEBUG((LM_DEBUG,"\n"));
#endif
}


/**************************************************************
Function:        CDetector::GetDetAlarm
Description:    ��ȡ����������澯��
Input:          ��
Output:         pDetAlarm �����澯���ݽṹ��ָ��
Return:         ��
***************************************************************/
void CDetector::GetDetAlarm(SDetectorAlarm* pDetAlarm)
{
	Byte ucBoardIndex = m_ucActiveBoard1;
	int iIndex        = 0;
	int iDetId        = 0;
	//STscConfig* pTscConfig = CManaKernel::CreateInstance()->m_pTscConfig;

	while ( iIndex < 48 )
	{
		(pDetAlarm+iIndex)->ucId = iIndex + 1;
		iIndex++;
	}
	iIndex = 0;
	while ( iIndex < 32 ) 
	{
		if ( iIndex >=  MAX_DETECTOR_PER_BOARD )
		{
			ucBoardIndex = m_ucActiveBoard2;  // 16 - 31
		}
		
		if ( m_iBoardErr[ucBoardIndex] == DEV_IS_DISCONNECTED )
		{
			(pDetAlarm+iIndex)->ucDetAlarm |= (1<<3);  /*ͨ�Ź���*/
			iIndex++;
			continue;
		}

		iDetId = ucBoardIndex * MAX_DETECTOR_PER_BOARD + iIndex % 16;

		if ( ( m_ucDetError[iDetId] != DET_NORMAL )
		  /*&& ( pTscConfig->sDetector[iIndex].ucPhaseId != 0 ) */)
		{
			(pDetAlarm+iIndex)->ucDetAlarm |= (1<<7); /*δ֪����*/
		}

		switch ( m_ucDetError[iDetId] )
		{
			case DET_CARVE:  //��·
				(pDetAlarm+iIndex)->ucCoilAlarm |= (1<<2);    //��·
				break;
			case DET_SHORT:   //��·
				(pDetAlarm+iIndex)->ucCoilAlarm |= (1<<4);  //��Ӧ�仯���� 
				break;
			case DET_STOP:    //ͣ��
				(pDetAlarm+iIndex)->ucCoilAlarm |= (1<<3);  //��в���
				break;
			default:
				break;
		}
		iIndex++;
	}

#if 0
	iIndex = 0;
	while ( iIndex < 48 )
	{
		ACE_DEBUG((LM_DEBUG,"id:%d ucDetAlarm:%d ucCoilAlarm:%d \n"
			,(pDetAlarm+iIndex)->ucId
			,(pDetAlarm+iIndex)->ucDetAlarm
			,(pDetAlarm+iIndex)->ucCoilAlarm));
		iIndex++;
	}
	ACE_DEBUG((LM_DEBUG,"\n"));
#endif

}


/**************************************************************
Function:        CDetector::IsDetError
Description:    �ж��Ƿ���ڼ�������� ����������+���������
Input:          ��
Output:         ��
Return:         false:�����ڰ����û�м�������� true:���ڼ��������
***************************************************************/
bool CDetector::IsDetError()
{
	int iIndex = 0;

	while ( iIndex < MAX_DETECTOR )
	{
		if ( DEV_IS_CONNECTED == m_iBoardErr[iIndex/16] )
		{
			if ( m_ucDetError[iIndex] != DET_NORMAL ) 
			{
				return true;
			}
		}
		iIndex++;
	}

	return false;
}

/***************************************************************

��ȡ�г���ʱ���¼1s��ȡһ��
#ifndef WINDOWS
void CDetector::GetHaveCarTime(time_t* pTime)
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_sMutex);

	ACE_OS::memcpy(pTime,m_tHaveCarTime,MAX_DETECTOR);
}
#endif

*****************************************************************/

/**************************************************************
Function:        CDetector::HaveDetBoard
Description:    �Ƿ���ڼ������ �� ���ڼ�����Ǻõ�   �������
				����һ��������һ��������򷵻��� �����Ҫ�Ǵ�����
				�����ж��Ƿ��м����
Input:          ��
Output:         ��
Return:         true:����  false:������
***************************************************************/
bool CDetector::HaveDetBoard()
{	
	int iDetId =0x0;
	int iMaxDetId =0x0 ;
	STscConfig* pTscConfig = CManaKernel::CreateInstance()->m_pTscConfig;
	/*�жϵ�ǰ�Ƿ������ü������*/
	for ( Byte iBoardIndex=0x0; iBoardIndex<MAX_DET_BOARD; iBoardIndex++ )
	{		
		//ACE_DEBUG((LM_DEBUG,"%s:%d m_iBoardErr[%d] = %02X \n",__FILE__,__LINE__,iBoardIndex,m_iBoardErr[iBoardIndex]));
			if(DEV_IS_CONNECTED == m_iBoardErr[iBoardIndex] )
			{
				if(iBoardIndex < 2) //�������
				{
				 iDetId = iBoardIndex*MAX_DETECTOR_PER_BOARD ;
				 iMaxDetId = (iDetId + MAX_DETECTOR_PER_BOARD-1) ;
				}
				else  //�ӿڰ�
				{
				 iDetId = (iBoardIndex-1)*MAX_INTERFACE_PER_BOARD;
				 iMaxDetId = (iDetId + MAX_INTERFACE_PER_BOARD-1) ;
				}
				
				while(iDetId <= iMaxDetId)
				{
		//ACE_DEBUG((LM_DEBUG,"%s:%d iBoardIndex =%d sDetector[%d].ucPhaseId=%d \n",__FILE__,__LINE__,iBoardIndex,iDetId,pTscConfig->sDetector[iDetId].ucPhaseId));
					if(pTscConfig->sDetector[iDetId].ucPhaseId != 0)
					{
						//ACE_DEBUG((LM_DEBUG,"%s:%d iBoardIndex =%d sDetector[%d].ucPhaseId=%d \n",__FILE__,__LINE__,iBoardIndex,iDetId,pTscConfig->sDetector[iDetId].ucPhaseId));
						if((iBoardIndex <2) && (DET_NORMAL == m_ucDetError[iDetId]))
						{
							
						//	m_iBoardErr[iBoardIndex] = DEV_IS_DISCONNECTED ; //��ֹ���ܽ���
							return true ;
						}
						else if(iBoardIndex >=2) //�ӿڰ岻���ж�ͨ���û�
						{
							
							//m_iBoardErr[iBoardIndex] = DEV_IS_DISCONNECTED ; //��ֹ���ܽ���
							return true ;
						}						
					}
					iDetId++ ;
				}
				
			}		
			
		}					
	//ACE_OS::printf("\r\n%s:%d No active detector found!!!\r\n",__FILE__,__LINE__);
	return false;
}


/**************************************************************
Function:        CDetector::IsVehileHaveCar
Description:    ��Ӧ����,�ж��Ƿ��г� ����г�����̵��ӳ�ʱ��
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CDetector::IsVehileHaveCar()
{
	static bool bVehile      = false;
	static bool bDefStep     = false;
	static bool bOtherWayCar = false;   //��һ�������Ƿ��г�
	bool bOtherTmp           = false;
	bool bHaveCar            = false;
	bool bCurMainDrive       = false;
	Byte ucPhaseIndex        = 0;
	static int  iAdjustTime  = 0;
	static Uint uiCurPhase   = 0;
	static Uint uiNextPhase  = 0;
	Uint uiTscCtrl           = 0;
	static SPhaseDetector sPhaseDet[MAX_PHASE];
	CManaKernel * pManaKernel = CManaKernel::CreateInstance();
	
	uiTscCtrl     = pManaKernel->m_pRunData->uiCtrl;
	pManaKernel->GetVehilePara( &bVehile , &bDefStep , &iAdjustTime 
		                                           , &uiCurPhase , &uiNextPhase , sPhaseDet);
	//ACE_Date_Time tvTime(ACE_OS::gettimeofday());		
	//ACE_DEBUG((LM_DEBUG,"%s:%d CTRL_PreAnalysis %d:%d:%d:%d!\n",__FILE__,__LINE__,tvTime.hour(),tvTime.minute(),tvTime.second(),tvTime.microsec()));
			
	if ( !bVehile && uiTscCtrl != CTRL_PREANALYSIS) //������ǽ׶γ�ʼ���������̳�������ֱ�ӷ��� ���������̲���
	{
		return;
	}

	if ( bDefStep )  //�µĲ���
	{
		bDefStep     = false;
		bOtherWayCar = false;
		
	}

	if ( uiTscCtrl != CTRL_VEHACTUATED || uiTscCtrl != CTRL_PREANALYSIS)
	{
		bCurMainDrive = pManaKernel->IsMainPhaseGrp(uiCurPhase);  //ֻ���ǽ׶ε�һ���������̳�����������и�Ӧ��������
	}
	switch ( uiTscCtrl )
	{
		case CTRL_VEHACTUATED:
			if (IsHaveCarPhaseGrp( uiCurPhase, ucPhaseIndex , sPhaseDet) ) //uiCurPhase��ǰ�׶η�����λ ÿ�����ڻ��ȡһ�θ����׶εķ�����λ
			{
				bHaveCar = true; //������λ�κ�һ����λ���κ�һ��������г� �� ����г�	 sPhaseDet ��λ��������Ӧ��ϵ 
			}
			break;
		case CTRL_MAIN_PRIORITY:
			if ( bCurMainDrive )  //��ǰΪ����λ
			{
				if ( IsHaveCarPhaseGrp( uiCurPhase, ucPhaseIndex , sPhaseDet) ) //�������г�
				{
					bHaveCar = true;
				}
			}
			else  if ( !bOtherWayCar ) //��ǰΪ����λ + �������ڸò���һֱû�г�
			{
				bOtherTmp = IsHaveCarPhaseGrp(uiNextPhase, ucPhaseIndex , sPhaseDet);   //��ǰ�������Ƿ��г�
				if ( bOtherTmp )
				{
					bOtherWayCar = true;
				}

				if ( !bOtherWayCar && IsHaveCarPhaseGrp(uiCurPhase, ucPhaseIndex ,sPhaseDet) )  //�������ڸò���һֱû�г� + �γ����г� 
					                          
				{
					bHaveCar = true;    
				}
			}
			break;
		case CTRL_SECOND_PRIORITY:
			if ( !bCurMainDrive ) //��ǰΪ����λ
			{
				if ( IsHaveCarPhaseGrp( uiCurPhase, ucPhaseIndex , sPhaseDet) )   //�γ����г�
				{
					bHaveCar = true;
				}
				
				//ACE_OS::printf("%s:%d CTRL_SECOND_PRIORITY bOtherTmp=%s uiCurPhase=%d \r\n",__FILE__,__LINE__,(bOtherTmp == false)?"false":"true",uiCurPhase);
			}
			else  if ( !bOtherWayCar ) //��ǰ����λ + �γ����ڸò���һֱû�г�
			{
				bOtherTmp = IsHaveCarPhaseGrp(uiNextPhase, ucPhaseIndex , sPhaseDet);   //��ǰ�γ����Ƿ��г�
				//ACE_OS::printf("%s:%d CTRL_SECOND_PRIORITY bOtherTmp=%s uiNextPhase=%d \r\n",__FILE__,__LINE__,(bOtherTmp == false)?"false":"true",uiNextPhase);
				if ( bOtherTmp )
				{
					bOtherWayCar = true; //�γ����г�,�γ������Է����̵�
					if(pManaKernel->bSecondPriority ==false)
						pManaKernel->bSecondPriority = true ;
				}			
			}
			break;
		case  CTRL_ACTIVATE:
		case CTRL_PREANALYSIS:
		{										
			Byte iIndex =0x0 ;
			Byte iDetCnt =0x0 ;
			Byte ucPhaseIndex =0x0 ;
			Byte iDetIndex =0x0 ;
			Byte iDetId = 0x0 ;
			while(iIndex < MAX_PHASE )
			{
				ucPhaseIndex = iIndex;  //������λ
				iDetCnt      = pManaKernel->m_sPhaseDet[iIndex].iRoadwayCnt;//�жϸ���λ�м��������

				iDetIndex = 0;
				while ( iDetIndex < iDetCnt )//����λ���еļ����ѭ��һ��
				{
					iDetId = pManaKernel->m_sPhaseDet[iIndex].iDetectorId[iDetIndex] - 1; //����������±�
				
					if(0x1==m_iDetStatus[iDetId])
					{							
				   		if( iDetId/MAX_INTERFACE_PER_BOARD ==0x0 && DET_NORMAL==m_ucDetError[iDetId])
				   		{
				   			pManaKernel->m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex] +=1 ;
							
							//ACE_OS::printf("%s:%d CTRL_PreAnalysis Phase=%d DetecId=%d cars=%d\r\n",__FILE__,__LINE__,ucPhaseIndex+1,iDetId+1,pManaKernel->m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]);
				   		}
				  		 else if(iDetId/MAX_INTERFACE_PER_BOARD >=0x1) 
				   		{				   	  
					 	    pManaKernel->m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex] +=1 ;
				   		}						 
					}			
					iDetIndex++;
				}
				iIndex++;
			}
			
			if ( IsHaveCarPhaseGrp( uiCurPhase, ucPhaseIndex , sPhaseDet)&&bVehile ) //uiCurPhase��ǰ�׶η�����λ ÿ�����ڻ��ȡһ�θ����׶εķ�����λ
			{
				bHaveCar = true; //������λ�κ�һ����λ���κ�һ��������г� �� ����г�	 sPhaseDet ��λ��������Ӧ��ϵ 
			}
									
		}
		break ;
		default:
			break;
	}

	if ( bHaveCar )
	{
		//ACE_DEBUG((LM_DEBUG,"\n\n%s:%d now add green time!\n\n",__FILE__,__LINE__));
		CManaKernel::CreateInstance()->AddRunTime(iAdjustTime,ucPhaseIndex);
	}
}


/**************************************************************
Function:        CDetector::IsHaveCarPhaseGrp
Description:    �ж�ĳ����λ���Ƿ��г�
Input:          uiPhase-��λ��  pPhaseDet-������λ�������Ĺ�ϵ
Output:         ucPhaseIndex - �г�����λ 0 - 15
Return:         true-�г�   false-�޳�
***************************************************************/
bool CDetector::IsHaveCarPhaseGrp(Uint uiPhase,Byte& ucPhaseIndex , SPhaseDetector* pPhaseDet)//uiPhase �׶γ����̵ƽ׶η�����λ�� ȡֵ��Դ������overcycle�еĺ���
{
	bool bHaveCar  = false;
	int  iIndex    = 0;
	int  iDetCnt   = 0;
	int  iDetIndex = 0;
	int  iDetId    = 0;

	while ( iIndex <  MAX_PHASE  )
	{
		if ( (uiPhase>>iIndex) & 1 )
		{
			ucPhaseIndex = iIndex;  //������λ
			iDetCnt      = pPhaseDet[iIndex].iRoadwayCnt;//�жϸ���λ�м��������

			iDetIndex = 0;
			while ( iDetIndex < iDetCnt )//����λ���еļ����ѭ��һ��
			{
				iDetId = pPhaseDet[iIndex].iDetectorId[iDetIndex] - 1; //����������±�
				
				if(0x1==m_iDetStatus[iDetId])
				{
				   if( iDetId/MAX_INTERFACE_PER_BOARD ==0x0 && DET_NORMAL==m_ucDetError[iDetId])
				   	{
				   		bHaveCar = true;
					//	ACE_DEBUG((LM_DEBUG,"\n%s:%d  phaseid=%d Dector No.:%d\n\n",__FILE__,__LINE__,iIndex+1,iDetId+1));
						break ;
				   	}
				   else if(iDetId/MAX_INTERFACE_PER_BOARD >=0x1) 
				   {
				   	   bHaveCar = true;
						//ACE_DEBUG((LM_DEBUG,"\n%s:%d Dector No.:%d\n\n",__FILE__,__LINE__,iDetId+1));
					   break ;
				   }				    
				}

			//	if ( (1==m_iDetStatus[iDetId]) && (DET_NORMAL==m_ucDetError[iDetId]) && (m_iDetOccupy[iDetId] < 99 ) )  //�г��Ҽ����û������ռ����С��99
				//{
					//ACE_DEBUG((LM_DEBUG,"\n%s:%d Dector No.:%d\n\n",__FILE__,__LINE__,iDetId+1));

				//	bHaveCar = true; //���������λ�̵Ƴ���ʱ����κ�һ��������г����ж��˳�ѭ�� ���ҷ����г� true 
				//	break;
				//}
				iDetIndex++;
			}
		}

		if ( bHaveCar )
		{
			return true;
		}
		iIndex++;
	}

	return false;
}


/**************************************************************
Function:        CDetector::GetAdaptInfo
Description:   ����Ӧ����
Input:           pDetTimeLen - ������г���ʱ��
Output:         pTotalStat  - ������
Return:         ��
***************************************************************/
void CDetector::GetAdaptInfo(int* pDetTimeLen , int* pTotalStat)
{
	if ( (NULL == pDetTimeLen) || (NULL == pTotalStat) )
	{
		return;
	}

	ACE_OS::memcpy(pDetTimeLen , m_iAdapDetTimeLen , sizeof(int)*MAX_DETECTOR);   //�0�7�0�4�0�6�0�8�0�8���0�3�0�1 �0�9�0�0�0�9�0�3�0�7�0�4�0�0�0�8
	ACE_OS::memcpy(pTotalStat  , m_iAdapTotalStat  , sizeof(int)*MAX_DETECTOR);   //�0�6�0�8�0�9�0�9�0�7

	ACE_OS::memset(m_iAdapDetTimeLen , 0 , sizeof(int)*MAX_DETECTOR);
	ACE_OS::memset(m_iAdapTotalStat  , 0 , sizeof(int)*MAX_DETECTOR);
}


/**************************************************************
Function:        CDetector::SetStatCycle
Description:    ���òɼ�����
Input:           ucCycle - ����
Output:         ��
Return:         ��
***************************************************************/
void CDetector::SetStatCycle(Byte ucCycle)
{
	m_iTotalDistance = ucCycle;
}


/**************************************************************
Function:        CDetector::RecvDetCan
Description:     ���ذ崦��Ӽ�����巢�͹�����CAN���ݰ�
Input:           ucBoardAddr-��������ַ 
				 sRecvCanTmp-���յ�CAN����֡
Output:         ��
Return:         ��
***************************************************************/
void CDetector::RecvDetCan(Byte ucBoardAddr,SCanFrame sRecvCanTmp)
{
		
	Byte ucDetBoardIndex = 0;
	Byte ucValueTmp = 0;
	Byte RecvType = 0 ;
	RecvType = (sRecvCanTmp.pCanData[0] == 0xff)?0xff:(sRecvCanTmp.pCanData[0] & 0x3F) ;
	
    //ACE_DEBUG((LM_DEBUG,"%s:%d Dectyep=%02x!\n",__FILE__,__LINE__,sRecvCanTmp.pCanData[0]));
	ACE_Date_Time tvTime(ACE_OS::gettimeofday());	
	ucDetBoardIndex = GetDecBoardIndex(ucBoardAddr) ; //ADD :20150202
	if ( DET_HEAD_VEHSTS ==RecvType )
		{				
			//ACE_DEBUG((LM_DEBUG,"%s:%d Get car info %2x:%2x !Time %d:%d\n",__FILE__,__LINE__,sRecvCanTmp.pCanData[1],sRecvCanTmp.pCanData[2],tvTime.minute(),tvTime.second()));
				//�����ֽ�ֻʹ�õ������ֽ�
			for ( int i=1; i<sRecvCanTmp.ucCanDataLen; i++ )
			{				
				ucValueTmp = sRecvCanTmp.pCanData[i];			
				Byte iDetId = 0 ;
				for ( int j=0; j<8; j++ )
				{
					//iDetId = ucDetBoardIndex*16+(i-1)*8+j ;
					if(ucDetBoardIndex <2)
						iDetId = ucDetBoardIndex*16+(i-1)*8+j ;    //�������
					else 
						iDetId = (ucDetBoardIndex-1)*32+(i-1)*8+j ; //�ӿڰ�
					m_iDetStatus[iDetId] = (ucValueTmp >> j) & 0x01;
					if((ucValueTmp >> j) & 0x1)
					{
						m_iDetTimeLen[iDetId] += 1;    //����ռ����
						m_iDetStatus[iDetId]  = 1;     //�������г�

						m_iAdapDetTimeLen[iDetId] += 1;
						
						if ( 0 == m_iLastDetSts[iDetId] )
						{
							//ACE_Date_Time tvTime(ACE_OS::gettimeofday());       
						//	ACE_DEBUG((LM_DEBUG,"%s:%d Detcotr No.:%d has car,%02x %d:%d:%d:%d!\n",__FILE__,__LINE__,iDetId+1,sRecvCanTmp.pCanData[i],tvTime.hour(),tvTime.minute(),tvTime.second(),tvTime.microsec()));
							//if(m_ucTotalStat[iDetId]==0xff)
								//m_ucTotalStat[iDetId]=0;
							m_ucTotalStat[iDetId] += 1;   //���㳵���� �ϴ��޳��˴��г���Ϊ�г� 
							
							m_iAdapTotalStat[iDetId] += 1; 
						}
					}
					m_iLastDetSts[iDetId] = (ucValueTmp >> j) & 0x01;
				}
			}
	
			
		}
		else if(DET_HEAD_STS == RecvType && ucDetBoardIndex<0x2) //ֻ�޼������
		{			
			Byte ucDecId = 0 ;
			CManaKernel * pManakernel = CManaKernel::CreateInstance() ;
			
			//ACE_DEBUG((LM_DEBUG,"%s:%d Get det status %2x:%2x !Time %d:%d\n",__FILE__,__LINE__,sRecvCanTmp.pCanData[1],sRecvCanTmp.pCanData[2],tvTime.minute(),tvTime.second()));
			for ( int i=1; i<sRecvCanTmp.ucCanDataLen; i++ )
			{
				ucValueTmp = sRecvCanTmp.pCanData[i];
			
				for ( int j=0; j<8; j++ )
				{
					if(j%2 == 0)
					{						
						ucDecId = 16*ucDetBoardIndex+(i-1)*4+j/2 ;
						if (pManakernel->m_pTscConfig->sDetector[ucDecId].ucDetectorId == 0 || 
							pManakernel->m_pTscConfig->sDetector[ucDecId].ucPhaseId == 0)     
							continue ;
						m_ucDetError[ucDecId] = (ucValueTmp >> j) & 0x3;//
						if (m_ucLastDetError[ucDecId] == m_ucDetError[ucDecId] )
							continue ;
						m_ucLastDetError[ucDecId] = m_ucDetError[ucDecId] ;
						//ACE_DEBUG((LM_DEBUG,"%s:%d m_ucDetError[%d] = %d!\n",__FILE__,__LINE__,ucDecId,m_ucDetError[ucDecId]));
						if((ucValueTmp >> j) & 0x3)
						{
							
							CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_DETECTOR, ucDecId+1,m_ucDetError[ucDecId],0,0);			
						//	ACE_DEBUG((LM_DEBUG,"%s:%d m_ucDetError[%d] bad:%d!\n",__FILE__,__LINE__,ucDecId,(ucValueTmp >> j) & 0x3));
						}
					}
				}
			}	


		}
	else if(DET_HEAD_SEN0108_GET == RecvType|| DET_HEAD_SEN0916_GET == RecvType )
			{
			int iNdex = 0 ;
			if(DET_HEAD_SEN0916_GET == RecvType)
				iNdex =8 ;

			m_ucGetDetDelicacy[ucDetBoardIndex][0+iNdex]= sRecvCanTmp.pCanData[1] &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][1+iNdex]= sRecvCanTmp.pCanData[1]>>4 &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][2+iNdex]= sRecvCanTmp.pCanData[2] &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][3+iNdex]= sRecvCanTmp.pCanData[2]>>4 &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][4+iNdex]= sRecvCanTmp.pCanData[3] &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][5+iNdex]= sRecvCanTmp.pCanData[3]>>4 &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][6+iNdex]= sRecvCanTmp. pCanData[4] &0xf ;
			m_ucGetDetDelicacy[ucDetBoardIndex][7+iNdex]= sRecvCanTmp.pCanData[4]>>4 &0xf ;

			}
			else if(DET_HEAD_SPEED0104 == RecvType || DET_HEAD_SPEED0508 == RecvType)
			{
			int iNdex = 0 ;
			if(DET_HEAD_SPEED0508 == RecvType)
				iNdex = 4 ;
			m_ucRoadSpeed[ucDetBoardIndex][0+iNdex] = sRecvCanTmp.pCanData[1] ;
			m_ucRoadSpeed[ucDetBoardIndex][1+iNdex] = sRecvCanTmp.pCanData[2] ;
			m_ucRoadSpeed[ucDetBoardIndex][2+iNdex] = sRecvCanTmp.pCanData[3] ;
			m_ucRoadSpeed[ucDetBoardIndex][3+iNdex] = sRecvCanTmp.pCanData[4] ;
			ACE_DEBUG((LM_DEBUG,"%s:%d DecRoadSpeed : m_ucRoadSpeed[%d][%d]:%d m_ucRoadSpeed[%d][%d]:%d m_ucRoadSpeed[%d][%d]:%d m_ucRoadSpeed[%d][%d]:%d\n",__FILE__,__LINE__,ucDetBoardIndex,1+iNdex,sRecvCanTmp.pCanData[1] ,ucDetBoardIndex,2+iNdex,sRecvCanTmp.pCanData[2] ,ucDetBoardIndex,3+iNdex,sRecvCanTmp.pCanData[3], ucDetBoardIndex,4+iNdex,sRecvCanTmp.pCanData[4]));
		}
		else if(DET_HEAD_COIL0104_GET == RecvType || DET_HEAD_COIL0508_GET == RecvType)
		{
			int iNdex = 0 ;
			if(DET_HEAD_COIL0508_GET == RecvType)
				iNdex = 4 ;
			m_sGetLookLink[ucDetBoardIndex][0+iNdex] = sRecvCanTmp.pCanData[1] ;
			m_sGetLookLink[ucDetBoardIndex][1+iNdex] = sRecvCanTmp.pCanData[2] ;
			m_sGetLookLink[ucDetBoardIndex][2+iNdex] = sRecvCanTmp.pCanData[3] ;
			m_sGetLookLink[ucDetBoardIndex][3+iNdex] = sRecvCanTmp.pCanData[4] ;
			ACE_DEBUG((LM_DEBUG,"%s:%d DecLookLink : m_sGetLookLink[%d][%d]:%2x m_sGetLookLink[%d][%d]:%2x m_sGetLookLink[%d][%d]:%2x m_sGetLookLink[%d][%d]:%2x\n",__FILE__,__LINE__,ucDetBoardIndex,1+iNdex,sRecvCanTmp.pCanData[1] ,ucDetBoardIndex,2+iNdex,sRecvCanTmp.pCanData[2] ,ucDetBoardIndex,3+iNdex,sRecvCanTmp.pCanData[3], ucDetBoardIndex,4+iNdex,sRecvCanTmp.pCanData[4]));
		}
		else if(DET_HEAD_DISTAN0104_GET== RecvType || DET_HEAD_DISTAN0508_GET == RecvType)
		{
			int iNdex = 0 ;
			if(DET_HEAD_DISTAN0508_GET == RecvType)
				iNdex = 4 ;
			
			m_ucGetRoadDis[ucDetBoardIndex][0+iNdex] = sRecvCanTmp.pCanData[1] ;
			m_ucGetRoadDis[ucDetBoardIndex][1+iNdex] = sRecvCanTmp.pCanData[2] ;
			m_ucGetRoadDis[ucDetBoardIndex][2+iNdex] = sRecvCanTmp.pCanData[3] ;
			m_ucGetRoadDis[ucDetBoardIndex][3+iNdex] = sRecvCanTmp.pCanData[4] ;
			ACE_DEBUG((LM_DEBUG,"%s:%d DecRoadDis : m_ucGetRoadDis[%d][%d]:%d m_ucGetRoadDis[%d][%d]:%d m_ucGetRoadDis[%d][%d]:%d m_ucGetRoadDis[%d][%d]:%d\n",__FILE__,__LINE__,ucDetBoardIndex,1+iNdex,sRecvCanTmp.pCanData[1] ,ucDetBoardIndex,2+iNdex,sRecvCanTmp.pCanData[2] ,ucDetBoardIndex,3+iNdex,sRecvCanTmp.pCanData[3],ucDetBoardIndex,4+iNdex,sRecvCanTmp.pCanData[4]));
			
		}
		else if(DET_HEAD_FRENCY_GET== RecvType)
		{
			for(int i = 0 ; i < MAX_DETECTOR_PER_BOARD ; i++)			
			{
				 m_ucGetFrency[ucDetBoardIndex][i] = ((sRecvCanTmp.pCanData[i/4+1])>>(i%4)*2) &0x3 ;
				 ACE_DEBUG((LM_DEBUG,"\n%s:%d m_ucGetFrency[%d][%d]:%d\n",__FILE__,__LINE__,ucDetBoardIndex,i,m_ucGetFrency[ucDetBoardIndex][i]));

			}
			
		}
		else if(DET_HEAD_SENDATA0107_GET ==RecvType || DET_HEAD_SENDATA0814_GET ==RecvType || DET_HEAD_SENDATA1516_GET ==RecvType )
		{
			if(DET_HEAD_SENDATA0107_GET ==RecvType)
			{
				for(int ucIndex = 0 ; ucIndex< 7 ;ucIndex++)
				{
					m_ucGetSensibility[ucDetBoardIndex][ucIndex] = sRecvCanTmp.pCanData[ucIndex+1] ;
					//printf(" %d  ",m_ucGetSensibility[ucDetBoardIndex][ucIndex]);
				}
				//printf("\n");
			}
			else if(DET_HEAD_SENDATA0814_GET ==RecvType )
			{
				for(int ucIndex = 0 ; ucIndex< 7 ;ucIndex++)
				{
					m_ucGetSensibility[ucDetBoardIndex][7+ucIndex] = sRecvCanTmp.pCanData[ucIndex+1] ;
					//printf(" %d  ",m_ucGetSensibility[ucDetBoardIndex][7+ucIndex]);
				}
				//printf("\n");
			}
			else if(DET_HEAD_SENDATA1516_GET ==RecvType )
			{
				m_ucGetSensibility[ucDetBoardIndex][14] = sRecvCanTmp.pCanData[1];
				m_ucGetSensibility[ucDetBoardIndex][15] = sRecvCanTmp.pCanData[2];
				//printf("\n%d   %d \n",m_ucGetSensibility[ucDetBoardIndex][14],m_ucGetSensibility[ucDetBoardIndex][15]);
			}
			else if(DET_HEAD_WORK_SET == RecvType)
			{
				m_iChkType = sRecvCanTmp.pCanData[1];
				//printf(" Get m_iChkType=%d ",sRecvCanTmp.pCanData[1]);
			}
			
			
		}
		else if(DET_HEAD_VER ==RecvType)
		{			
			ACE_OS::memset(m_ucDecBoardVer[ucDetBoardIndex],0x0 ,5);
			m_ucDecBoardVer[ucDetBoardIndex][0]=sRecvCanTmp.pCanData[1];
			m_ucDecBoardVer[ucDetBoardIndex][1]=sRecvCanTmp.pCanData[2];
			m_ucDecBoardVer[ucDetBoardIndex][2]=sRecvCanTmp.pCanData[3];
			m_ucDecBoardVer[ucDetBoardIndex][3]=sRecvCanTmp.pCanData[4];
			m_ucDecBoardVer[ucDetBoardIndex][4]=sRecvCanTmp.pCanData[5];
			if(sRecvCanTmp.pCanData[1] == 0x0)
			{
				m_iBoardErr[ucDetBoardIndex] = DEV_IS_DISCONNECTED ;
			}
			else
			{				
				m_iBoardErr[ucDetBoardIndex] = DEV_IS_CONNECTED ;
			}				
			//ACE_OS::printf("%s:%d m_iBoardErr[%d] = %s sRecvCanTmp.pCanData[1] == %d \n",__FILE__,__LINE__,ucDetBoardIndex,m_iBoardErr[ucDetBoardIndex] ==0xa?"DEV_IS_CONNECTED":"DEV_IS_DISCONNECTED",sRecvCanTmp.pCanData[1]);
	   	}
		else if(DET_HEAD_ID==RecvType)
		{
			m_ucDecBoardId[ucDetBoardIndex][0]=sRecvCanTmp.pCanData[1];
			m_ucDecBoardId[ucDetBoardIndex][1]=sRecvCanTmp.pCanData[2];
			m_ucDecBoardId[ucDetBoardIndex][2]=sRecvCanTmp.pCanData[3];
			m_ucDecBoardId[ucDetBoardIndex][3]=sRecvCanTmp.pCanData[4];				
	   }
		
}



/**************************************************************
Function:        CDetector::GetAllVehSts
Description:     ��ȡ�������״̬ ADD :201307101054
Input:           QueryType-��ѯ״̬���� �г��޳�������Ȧ״̬ 
				 ucBdindex-0 1 2 3 ��������±�
Output:         ��
Return:         ��
***************************************************************/
void CDetector::GetAllVehSts(Byte QueryType,Byte ucBdindex)
{	
//	ACE_DEBUG((LM_DEBUG,"%s:%d send CAN data to BOARD:%d\n",__FILE__,__LINE__,ucBdindex));
	switch(ucBdindex)
	{
		case 2 :
			GetVehSts(BOARD_ADDR_INTEDET1,QueryType);			
			break ;
		case 3 :
			GetVehSts(BOARD_ADDR_INTEDET2,QueryType);				
			break ;
		case 0 :
			GetVehSts(BOARD_ADDR_DETECTOR1,QueryType);				
			break ;
		case 1 :
			GetVehSts(BOARD_ADDR_DETECTOR2,QueryType);				
			break ;
		default :
				 ;

	}	
		
	
}


/**************************************************************
Function:        CDetector::GetVehSts
Description:     ���ذ�������������16��ͨ���ĳ������״̬ 
				 ADD:201307101054
Input:           QueryType-��ѯ״̬���� �г��޳�������Ȧ״̬ 
				 ucBoardAddr-��������ַ
Output:         ��
Return:         ��
***************************************************************/
void CDetector::GetVehSts(Byte ucBoardAddr,Byte QueryType)
{
	SCanFrame sSendFrameTmp;
	ACE_OS::memset(&sSendFrameTmp , 0 , sizeof(SCanFrame));
	
	switch(QueryType)
	{
		case DET_HEAD_VEHSTS :
			Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P  ,ucBoardAddr  , &(sSendFrameTmp.ulCanId));
			sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND << 6 ) | DET_HEAD_VEHSTS; //DATA_HEAD_CHECK ->>> DATA_HEAD_RESEND MOD?2013 0710 1645			
			sSendFrameTmp.ucCanDataLen = 1; //����Ƿ��г�
	//		ACE_DEBUG((LM_DEBUG,"%s:%d send CAN data to BOARD_ADDR:%x,QueryType=%d\n",__FILE__,__LINE__,ucBoardAddr,QueryType));
			break ;
		case DET_HEAD_STS :
			if(ucBoardAddr == 0x29 || ucBoardAddr == 0x2A)
				break ;
			Can::BuildCanId(CAN_MSG_TYPE_100 , BOARD_ADDR_MAIN  , FRAME_MODE_P2P  ,ucBoardAddr  , &(sSendFrameTmp.ulCanId));

			sSendFrameTmp.pCanData[0] = ( DATA_HEAD_RESEND << 6 ) | DET_HEAD_STS; ////DATA_HEAD_CHECK ->>> DATA_HEAD_RESEND ��Ҫ�ظ�������ڲ�����
			sSendFrameTmp.ucCanDataLen = 1; //���ÿ�����������״̬
	//		ACE_DEBUG((LM_DEBUG,"%s:%d send CAN data to BOARD_ADDR:%x,QueryType=%d\n",__FILE__,__LINE__,ucBoardAddr,QueryType));
			break ;
		default :
			return ;
	}
	
	Can::CreateInstance()->Send(sSendFrameTmp);
}

/**************************************************************
Function:        CDetector::GetAllDecVer
Description:    ��ѯ���м������ӿڰ��Ƿ����Ӵ���
Input:            ��
Output:         ��
Return:         ��
Date :          2015-02-02
***************************************************************/
void CDetector:: GetAllDecVer()
{
	for(Byte BoardIndex = 0 ; BoardIndex < MAX_DET_BOARD ;BoardIndex++)
	{
		GetDecVars(BoardIndex,0xff);
	}

}

/**************************************************************
Function:        CDetector::GetDecBoardIndex
Description:    ��ѯ���м������ӿڰ��Ƿ����Ӵ���
Input:           DecBoardHexAddr -�������ӿڰ��ַ16����
Output:         ���ص�ַ���� 0-�������1 1-�������2 2-�ӿڰ�1 3-�ӿڰ�2
Return:         ��
Date :          2015-02-02
***************************************************************/
Byte CDetector::GetDecBoardIndex(Byte DecBoardHexAddr)
{
	switch (DecBoardHexAddr)
	{
		case BOARD_ADDR_DETECTOR1: 
			return 0 ;
		case BOARD_ADDR_INTEDET1:
			return 2;
			break;
		case BOARD_ADDR_DETECTOR2:
			return 1 ;
		case BOARD_ADDR_INTEDET2:
			return 3 ;			
		default:
			ACE_DEBUG((LM_DEBUG,"%s:%d error ,return default 0\n",__FILE__,__LINE__));
			return 0;
    }

}


