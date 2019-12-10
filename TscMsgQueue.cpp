/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   TscMsgQueue.cpp
Date:       2013-1-1
Description: TSC��Ϣ���д����࣬��������TSC��Ϣ�Ľ��պʹ��������
Version:    V1.0
History:
***************************************************************/

#include "TscMsgQueue.h"
#include "ManaKernel.h"
#include "DbInstance.h"
#include "ComFunc.h"
#include "WirelessButtons.h"

/**************************************************************
Function:        CTscMsgQueue::CTscMsgQueue
Description:     CTscMsgQueue�๹�캯������ʼ����
Input:        	��	      
Output:         ��
Return:         ��
***************************************************************/
CTscMsgQueue::CTscMsgQueue()
{
	Uint uiLogMaxId = 0;
	Uint uiLogMinId = 0;

	m_pMsgQue = ACE_Message_Queue_Factory<ACE_MT_SYNCH>::create_static_message_queue();

	(CDbInstance::m_cGbtTscDb).GetLogMaxMin(&uiLogMaxId,&uiLogMinId);
	m_logger.SetMaxMinId(uiLogMaxId,uiLogMinId);
	//m_logger.activate(THR_NEW_LWP|THR_JOINABLE,1,1,ACE_THR_PRI_OTHER_MIN);   //�߳����ȼ����

	ACE_DEBUG((LM_DEBUG,"%s:%d Init TscMsgQueue object ok !\n",__FILE__,__LINE__));

}

/**************************************************************
Function:        CTscMsgQueue::~CTscMsgQueue
Description:     CTscMsgQueue����������	
Input:        	��	      
Output:         ��
Return:         ��
***************************************************************/
CTscMsgQueue::~CTscMsgQueue()
{
	if ( m_pMsgQue != NULL )
	{
		delete m_pMsgQue;
		m_pMsgQue = NULL;
	}
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct TscMsgQueue object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:        CTscMsgQueue::CreateInstance
Description:     ����CTscMsgQueue��̬����	
Input:        	��	      
Output:         ��
Return:         ��̬����ָ��
***************************************************************/
CTscMsgQueue* CTscMsgQueue::CreateInstance()
{
	static CTscMsgQueue cTscMsgQueue;	
	return &cTscMsgQueue;
}


/**************************************************************
Function:        CTscMsgQueue::SendMessage
Description:     ��TSC��Ϣ������еȴ�����		
Input:        	pMsg  - ��Ϣ���ݽṹ��ָ��
				iLen  - ��Ϣ���ݳ���		      
Output:         ��
Return:         0 -����  1-��ȷ
***************************************************************/
int CTscMsgQueue::SendMessage(SThreadMsg* pMsg,int iLen)
{
	if ( pMsg==NULL || m_pMsgQue==NULL )
	{
#ifdef TSC_DEBUG
		ACE_DEBUG((LM_DEBUG,"pMsg or m_pMsgQue is NULL"));
#endif
		return 0;
	}

	ACE_Message_Block *mb = new ACE_Message_Block(iLen); //������Ϣ��
	mb->copy((char*)pMsg, iLen); // �����ݿ�������Ϣ��

	ACE_Time_Value nowait(getCurrTime()+ACE_Time_Value(1));
	m_pMsgQue->enqueue_tail(mb, &nowait);    //�� ACE_Message_Queue����������ݿ�

#ifdef CHECK_MEMERY
	TestMem(__FILE__,__LINE__);
#endif
	
	return 1;
}


/**************************************************************
Function:        CTscMsgQueue::DealData
Description:     ����TSC������Ϣ����������Ϣ���Ͳ�ͬ�ֱ���				
Input:        	��		      
Output:         ��
Return:         ��
***************************************************************/
void CTscMsgQueue::DealData()
{
	int iLen = 0;
	timeval tTmp;
	ACE_Message_Block *mb = NULL;
	SThreadMsg sMsg; 
	ACE_Time_Value nowait(getCurrTime());

	tTmp.tv_sec  = 0;
	tTmp.tv_usec = 10000;

	CManaKernel* pWorkParaManager = CManaKernel::CreateInstance();
	CWirelessBtn* pWirelessBtn = CWirelessBtn::CreateInstance();

	for(;;)
	{
		if(m_pMsgQue->dequeue_head(mb, &nowait) != -1) //�� ACE_Message_Queue �е�����Ϣ��
		{   
			iLen = (int)mb->length();
			memcpy((char*)&sMsg, mb->base(), iLen);  //����Ϣ���ж�����
			mb->release();
		}
		else
		{
			ACE_OS::sleep(ACE_Time_Value(tTmp));  //��ͣ10����
			continue;
		}

	  	//ACE_DEBUG((LM_DEBUG,"%s:%d TscMsg Dealdata type:%d opt:%d dataLen:%d \n" ,__FILE__,__LINE__,sMsg.ulType,sMsg.ucMsgOpt,sMsg.uiMsgDataLen));//MOD:0515 17:00

		switch ( sMsg.ulType )   //��Ϣ����
		{
		case TSC_MSG_NEXT_STEP:
			pWorkParaManager->GoNextStep();
			break;
		case TSC_MSG_OVER_CYCLE:
			pWorkParaManager->OverCycle();
			break;
		case TSC_MSG_SWITCH_STATUS:
			pWorkParaManager->SwitchStatus(*((Byte*)sMsg.pDataBuf));
			break;
		case TSC_MSG_SWITCH_CTRL:
			//ACE_DEBUG((LM_DEBUG,"%s:%d �յ�TSC�ı���Ʒ�ʽ��\n" ,__FILE__,__LINE__));
			pWorkParaManager->SwitchCtrl(*((Byte*)sMsg.pDataBuf));
			break;
		case TSC_MSG_UPDATE_PARA:
			pWorkParaManager->SetUpdateBit();
			break;
		case TSC_MSG_TIMEPATTERN:
			pWorkParaManager->SpecTimePattern(*((Byte*)sMsg.pDataBuf));
			break;
		case TSC_MSG_LOCK_STEP:
			pWorkParaManager->LockStep(*((Byte*)sMsg.pDataBuf));
			break;
		case TSC_MSG_LOCK_STAGE:
			pWorkParaManager->LockStage(*((Byte*)sMsg.pDataBuf));
			break;
		case TSC_MSG_NEXT_STAGE:
			pWorkParaManager->LockNextStage();
			break;
		case TSC_MSG_LOCK_PHASE:
			pWorkParaManager->LockPhase(*((Byte*)sMsg.pDataBuf));
			break;
		case TSC_MSG_CORRECT_TIME:
			pWorkParaManager->CorrectTime(sMsg.ucMsgOpt,(Byte*)sMsg.pDataBuf);
			break;
		case TSC_MSG_STATUS_READ:
			pWorkParaManager->GetTscStatus(sMsg.ucMsgOpt);
			break;
		case TSC_MSG_EXSTATUS_READ:
			pWorkParaManager->GetTscExStatus(sMsg.ucMsgOpt);
			break;
		case TSC_MSG_GREENCONFLICT:
			pWorkParaManager->DealGreenConflict(sMsg.ucMsgOpt);
			break;
		case TSC_MSG_LOG_WRITE:
			WriteEventLog(sMsg.ucMsgOpt,(Byte*)sMsg.pDataBuf);
			break;
		case TSC_MSG_EVENT_READ:
			break;
		case TSC_MSG_PATTER_RECOVER:
			pWorkParaManager->ChangePatter(sMsg.ucMsgOpt);
			break ;
		case TSC_MSG_MANUALBUTTON_HANDLE:
			pWirelessBtn->HandleSpecialDirec((Uint*)sMsg.pDataBuf);
			break ;
		default:
			break;
		}

		if ( sMsg.pDataBuf != NULL )
		{
			ACE_OS::free(sMsg.pDataBuf);  //ɾ��������ڴ�
			sMsg.pDataBuf = NULL;
		}

	}

}


/**************************************************************
Function:        CTscMsgQueue::WriteEventLog
Description:     ���յ�д��־��Ϣ��Ĵ�����				
Input:        	uEvtType - ��־����		
				pEvtValue -��־ֵ��ַָ��				      
Output:         ��
Return:         ��
***************************************************************/
void CTscMsgQueue::WriteEventLog(Byte uEvtType, Byte* pEvtValue)
{
	int iIndex     = 0;
	Uint iEvtValue = 0;
	while ( iIndex < 4 )
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d pEvtValue[%d] = %d \n" ,__FILE__,__LINE__,iIndex,pEvtValue[iIndex]));//MOD:0515 17:00
		iEvtValue |=  ((pEvtValue[iIndex]&0xff)<<(8*(3-iIndex)));//ֵ�洢���ֽڵ���λ
		iIndex++;
	}
	
	m_logger.WriteEventLogActive(uEvtType,iEvtValue);
	/*
	ACE_DEBUG((LM_DEBUG,"%s:%d %d-%d uEvtType:%d iTotalSec:%d\n"
		  ,__FILE__,__LINE__,m_ucLogMaxId,m_ucLogMinId,uEvtType,iTotalSec));
	*/
	
}

