
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   GbtTimer.cpp
Date:       2013-1-1
Description:�źŻ�gbt��Ϣ����ʱ�������ڴ���ʱ�ϴ����������
Version:    V1.0
History:    
***************************************************************/
#include "GbtTimer.h"
#include "GbtMsgQueue.h"

/**************************************************************
Function:       CGbtTimer::CGbtTimer
Description:    CGbtTimer�๹�캯�����������ʼ������				
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
CGbtTimer::CGbtTimer()
{
	m_ucTick    = 0;
	m_ucMaxTick = 10;

	ACE_OS::memset(m_sReportInfo , 0 , MAX_REPORT_NUM*sizeof(SReportSelf) );
	ACE_DEBUG((LM_DEBUG,"%s:%d Init GbtTimer object ok !\n\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CGbtTimer::~CGbtTimer
Description:    CGbtTimer��	��������	
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
CGbtTimer::~CGbtTimer()
{
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct GbtTimer object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CGbtTimer::CreateInstance
Description:    ����	CGbtTimer��̬����
Input:          ��              
Output:         ��
Return:         ��̬����ָ��
***************************************************************/
CGbtTimer* CGbtTimer::CreateInstance()
{
	static CGbtTimer cGbtTimer;
	return &cGbtTimer;
}


#ifdef GBT_TCP
/**************************************************************
Function:       CGbtTimer::TailorReport
Description:    ���������ϱ���Ϣ������ȡ������tcp��ʽ����
Input:          ucGbtArrIndex ������gbt��Ϣ�±�
				uBufCnt   ���յ�����Ϣ����
				pBuf      ���յ�����Ϣ����ָ��              
Output:         ��
Return:         ��
***************************************************************/
void CGbtTimer::TailorReport(Byte ucGbtArrIndex , Uint uBufCnt , Byte* pBuf)
{
	Byte ucObjectNum   = pBuf[3];        //������
	Byte ucRecvOptType = pBuf[0] & 0xf; //�յ�֡�Ĳ�������
	Byte ucBufIndex    = 1;
	Byte ucObjectCmd   = 0;
	Ushort usCycle     = 0;

	ucBufIndex = 4;
	if ( GBT_SET_REQ == ucRecvOptType || GBT_SET_REQ_NOACK == ucRecvOptType ) 
	{
		for ( Byte ucIndex=0; ucIndex<ucObjectNum; ucIndex++ )
		{
			ucObjectCmd = pBuf[ucBufIndex++];  //cmdObj
			if ( ucBufIndex > uBufCnt )
			{
				SendErrMsg(ucGbtArrIndex);
				return;
			}

			usCycle  = 0;
			usCycle |= (pBuf[ucBufIndex++]<<8);
			usCycle |= pBuf[ucBufIndex++];

			if ( ucBufIndex > uBufCnt )
			{
				SendErrMsg(ucGbtArrIndex);
				return;
			}

			if ( false == CreateReport(ucGbtArrIndex,ucObjectCmd,usCycle) )  //�ϱ���Ϣ������ȡ��
			{
				SendErrMsg(ucGbtArrIndex);
				return;
			}
		}

		//��ȷ��Ӧ��
		if ( GBT_SET_REQ == ucRecvOptType )
		{
			Byte sSendMsg[4] = {0};
			SGbtDealData* pGbtDealData = CGbtMsgQueue::CreateInstance()->GetGbtDealDataPoint();
			
			sSendMsg[0] = ( pBuf[0] & 0xf0 ) | GBT_SET_ACK;
			sSendMsg[1] = pBuf[1];
			sSendMsg[2] = pBuf[2];
			sSendMsg[3] = pBuf[3];

			pGbtDealData[ucGbtArrIndex].SockStreamClient.send(sSendMsg , 4);
		}
	}
	else 
	{
		SendErrMsg(ucGbtArrIndex);
	}
}

#else

/**************************************************************
Function:       CGbtTimer::TailorReport
Description:    ���������ϱ���Ϣ������ȡ������udp��ʽ����
				���磺0x81 0xf7 0x0 0x2 0xf8 0x0 0xa 
*                     0xa0 0x0 0x2 ���ƶ�0xf8��0xa0���������Զ��ϴ�
				ǰ���ϴ�Ƶ��1sһ�Σ�����200msһ�Ρ�
Input:          addrClient �ͻ��˵�ַ
				uBufCnt   ���յ�����Ϣ����
				pBuf      ���յ�����Ϣ����ָ��              
Output:         ��
Return:         ��
***************************************************************/
void CGbtTimer::TailorReport(ACE_INET_Addr& addrClient, Uint uBufCnt , Byte* pBuf)
{
	Byte ucObjectNum   = pBuf[3];        //������
	Byte ucRecvOptType = pBuf[0] & 0xf; //�յ�֡�Ĳ�������
	Byte ucBufIndex    = 1;
	Byte ucObjectCmd   = 0;
	Ushort usCycle     = 0;

	ucBufIndex = 4;
	if ( GBT_SET_REQ == ucRecvOptType || GBT_SET_REQ_NOACK == ucRecvOptType )  //����
	{
		for ( Byte ucIndex=0; ucIndex<ucObjectNum; ucIndex++ )
		{
			ucObjectCmd = pBuf[ucBufIndex++];  //cmdObj
			if ( ucBufIndex > uBufCnt )
			{
				SendErrMsg(addrClient);
				return;
			}

			usCycle  = 0;
			usCycle |= (pBuf[ucBufIndex++]<<8);
			usCycle |= pBuf[ucBufIndex++];

			if ( ucBufIndex > uBufCnt )
			{
				SendErrMsg(addrClient);
				return;
			}

			if ( false == CreateReport(addrClient,ucObjectCmd,usCycle) )  //�ϱ���Ϣ������ȡ��
			{
				SendErrMsg(addrClient);
				return;
			}
		}

		//��ȷ��Ӧ��
		if ( GBT_SET_REQ == ucRecvOptType )
		{
			CGbtMsgQueue* pGbtMsgQueue = CGbtMsgQueue::CreateInstance();
			Byte sSendMsg[4] = {0};

			sSendMsg[0] = ( pBuf[0] & 0xf0 ) | GBT_SET_ACK;
			sSendMsg[1] = pBuf[1];
			sSendMsg[2] = pBuf[2];
			sSendMsg[3] = pBuf[3];

			pGbtMsgQueue->m_sockLocal.send(sSendMsg , 4 , addrClient);
		}
	}
	else 
	{
		SendErrMsg(addrClient);
	}
}
#endif

#ifdef GBT_TCP
/**************************************************************
Function:       CGbtTimer::CreateReport
Description:    ���������ϱ���Ϣ������ȡ������tcp��ʽ����				
Input:          ucGbtArrIndex ��������Ϣ�±�
				ucObjectCmd   �ϴ�����
				usCycle       �ϴ�����              
Output:         ��
Return:         false-ʧ�� true-�ɹ�
***************************************************************/
bool CGbtTimer::CreateReport(Byte ucGbtArrIndex,Byte ucObjectCmd , Ushort usCycle)
{
	int iNewIndex = -1;

	ACE_Guard<ACE_Thread_Mutex> guard(m_sMutex);

	for ( Byte ucIndex=0; ucIndex<MAX_REPORT_NUM; ucIndex++ )
	{
		if ( m_sReportInfo[ucIndex].usCycle > 0  )
		{
			if ( m_sReportInfo[ucIndex].ucCmd ==  ucObjectCmd  //�����޸�  ����ȡ��
				&& m_sReportInfo[ucIndex].ucGbtDealDataIndex== ucGbtArrIndex )
			{
				m_sReportInfo[ucIndex].usCycle   = usCycle;
				return true;
			}
		}
		else if ( (0 == m_sReportInfo[ucIndex].usCycle) && (-1 == iNewIndex) )  
		{
			iNewIndex = ucIndex;
		}
	}

	if ( iNewIndex >= 0 && iNewIndex < MAX_REPORT_NUM ) //�µ��ϱ���Ϣ����
	{
		m_sReportInfo[iNewIndex].ucTick             = 0;
		m_sReportInfo[iNewIndex].ucGbtDealDataIndex = ucGbtArrIndex;
		m_sReportInfo[iNewIndex].ucCmd              = ucObjectCmd;
		m_sReportInfo[iNewIndex].usCycle            = usCycle;
		return true;
	}

	return false;
}

#else

/**************************************************************
Function:       CGbtTimer::CreateReport
Description:    ���������ϱ���Ϣ������ȡ������UDP��ʽ����				
Input:          addrClient �ͻ��˵�ַ
				ucObjectCmd   �ϴ�����
				usCycle       �ϴ�����              
Output:         ��
Return:         false-ʧ�� true-�ɹ�
***************************************************************/
bool CGbtTimer::CreateReport(ACE_INET_Addr& addrClient,Byte ucObjectCmd , Ushort usCycle)
{
	int iNewIndex = -1;

	ACE_Guard<ACE_Thread_Mutex> guard(m_sMutex);

	for ( Byte ucIndex=0; ucIndex<MAX_REPORT_NUM; ucIndex++ )
	{
		if ( m_sReportInfo[ucIndex].usCycle > 0  )
		{
			if ( m_sReportInfo[ucIndex].ucCmd ==  ucObjectCmd  //�����޸�  ����ȡ��
				&& m_sReportInfo[ucIndex].addClient.get_ip_address() == addrClient.get_ip_address() )
			{
				m_sReportInfo[ucIndex].usCycle   = usCycle;
				m_sReportInfo[ucIndex].addClient = addrClient;
				return true;
			}
		}
		else if ( (0 == m_sReportInfo[ucIndex].usCycle) && (-1 == iNewIndex) )  
		{
			iNewIndex = ucIndex;
		}
	}

	if ( iNewIndex >= 0 && iNewIndex < MAX_REPORT_NUM ) //�µ��ϱ���Ϣ����
	{
		m_sReportInfo[iNewIndex].ucTick    = 0;
		m_sReportInfo[iNewIndex].addClient = addrClient;
		m_sReportInfo[iNewIndex].ucCmd     = ucObjectCmd;
		m_sReportInfo[iNewIndex].usCycle   = usCycle;
		return true;
	}

	return false;
}
#endif

#ifdef GBT_TCP
/**************************************************************
Function:       CGbtTimer::SendErrMsg
Description:    ���ʹ�����Ϣ����tcp��ʽ����				
Input:          ucGbtArrIndex ��������Ϣ�±�    
Output:         ��
Return:         ��
***************************************************************/
void CGbtTimer::SendErrMsg(Byte ucGbtArrIndex)
{
	SGbtDealData* pGbtDealData = CGbtMsgQueue::CreateInstance()->GetGbtDealDataPoint();
	Byte sErrMsg[3] = {0x86 , GBT_ERROR_OTHER , 0};

	pGbtDealData[ucGbtArrIndex].SockStreamClient.send(sErrMsg , 3);
}
#else
/**************************************************************
Function:       CGbtTimer::SendErrMsg
Description:    ���ʹ�����Ϣ����udp��ʽ����				
Input:          addrClient �ͻ��˵�ַ    
Output:         ��
Return:         ��
***************************************************************/
void CGbtTimer::SendErrMsg(ACE_INET_Addr& addrClient)
{
	CGbtMsgQueue* pGbtMsgQueue = CGbtMsgQueue::CreateInstance();
	Byte sErrMsg[3] = {0x86 , GBT_ERROR_OTHER , 0};

	pGbtMsgQueue->m_sockLocal.send(sErrMsg , 3 , addrClient);

}
#endif


/**************************************************************
Function:       CGbtTimer::SendErrMsg
Description:    ��ʱ���ص�������100ms����һ��				
Input:          tCurrentTime  ��ǰʱ��   
Output:         ��
Return:         0
***************************************************************/
int CGbtTimer::handle_timeout(const ACE_Time_Value &tCurrentTime, const void * /* = 0 */)
{
	static Byte ucDealDataIndex = 0;
    static SThreadMsg sMsg;
	static CGbtMsgQueue* pGbtMsgQue = CGbtMsgQueue::CreateInstance();
#ifdef GBT_TCP
	static SGbtDealData* pGbtDealData = pGbtMsgQue->GetGbtDealDataPoint();
#endif

	for ( Byte ucIndex=0; ucIndex<MAX_REPORT_NUM; ucIndex++ )
	{
		if ( m_sReportInfo[ucIndex].usCycle != 0 )
		{
			if ( m_sReportInfo[ucIndex].ucTick == m_sReportInfo[ucIndex].usCycle-1 )
			{
#ifdef GBT_TCP
				ucDealDataIndex = m_sReportInfo[ucIndex].ucGbtDealDataIndex;
				pGbtDealData[ucDealDataIndex].bReportSelf = true;
#else
				ucDealDataIndex = pGbtMsgQue->GetDealDataIndex(true , m_sReportInfo[ucIndex].addClient);
#endif

				if ( ucDealDataIndex < MAX_CLIENT_NUM )
				{
					sMsg.ulType               = GBT_MSG_FIRST_RECV;
					sMsg.ucMsgOpt             = ucDealDataIndex;
					sMsg.uiMsgDataLen 		  = 3;
					sMsg.pDataBuf             = ACE_OS::malloc(3);
					((Byte*)sMsg.pDataBuf)[0] = 0x80;
					((Byte*)sMsg.pDataBuf)[1] = m_sReportInfo[ucIndex].ucCmd;
					((Byte*)sMsg.pDataBuf)[2] = 0;
					pGbtMsgQue->SendGbtMsg(&sMsg,sizeof(sMsg));
					//MOD:0515 17:30
					//ACE_DEBUG((LM_DEBUG,"report self %x %x %x\n",((Byte*)sMsg.pDataBuf)[0],((Byte*)sMsg.pDataBuf)[1],((Byte*)sMsg.pDataBuf)[2] ));
				}
				m_sReportInfo[ucIndex].ucTick = 0;
			}
			else
			{
				m_sReportInfo[ucIndex].ucTick++;
			}
		}
	}

	return 0;
}

