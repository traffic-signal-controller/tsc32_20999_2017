
 /***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   Log.cpp
Date:       2013-1-1
Description:�źŻ���־�����ļ�
Version:    V1.0
History:    
***************************************************************/
#include "Log.h"
#include "DbInstance.h"
#include "TscKernelGb25280.h"

/**************************************************************
Function:       CLogger::CLogger
Description:    CLogger���������ʼ������				
Input:          ��              
Output:         ��
Return:         0
***************************************************************/
CLogger::CLogger()
{
	this->activate();
	ACE_DEBUG((LM_DEBUG,"%s:%d Init Log object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CLogger::WriteEventLog
Description:    ����gbt���ݿ⴦���������־����	
Input:          ucEvtType  ��־����
				uiEvtValue ��־ֵ              
Output:         ��
Return:         ��
***************************************************************/
void CLogger::WriteEventLog(Byte ucEvtType,Uint uiEvtValue)
{
	(CDbInstance::m_cGbtTscDb).AddEventLog(&m_uiLogMaxId,&m_uiLogMinId,ucEvtType,uiEvtValue);
	 if(ucEvtType  ==LOG_TYPE_GREEN_CONFIG||
				ucEvtType ==LOG_TYPE_DOOR_WARN||
				ucEvtType ==LOG_TYPE_OUTLAMP_ERR||
				ucEvtType ==LOG_TYPE_LAMP||
				ucEvtType ==LOG_TYPE_CAN||
				ucEvtType ==LOG_TYPE_FLASHBOARD||
				ucEvtType ==LOG_TYPE_DETECTOR	)
	 {
		 CTscKernelGb25280::CreateInstance()->SelfReportTscError();  //����

	 }
}

/**************************************************************
Function:       CLogger::WriteEventLogActive
Description:    ����������󣬲��뵽���������	
Input:          ucEvtType  ��־����
				uiEvtValue ��־ֵ              
Output:         ��
Return:         ��
***************************************************************/
void CLogger::WriteEventLogActive(Byte ucEvtType,Uint uiEvtValue)
{
	//����������󣬲��뵽���������
	m_queCmd.enqueue(new CLogCmd(this,ucEvtType,uiEvtValue));
}

/**************************************************************
Function:       CLogger::SetMaxMinId
Description:    ������־������Сid	
Input:          uiMaxId  ���ID
				uiMinId  ��СID              
Output:         ��
Return:         ��
***************************************************************/
void CLogger::SetMaxMinId(Uint uiMaxId,Uint uiMinId)
{
	m_uiLogMaxId = uiMaxId;
	m_uiLogMinId = uiMinId;
	//this->Add(); //ADD: 2013 0727
}


/**************************************************************
Function:       CLogger::svc
Description:    CLogger�ൽsrv���������ӻ���̳й���	
Input:          ��          
Output:         ��
Return:         0
***************************************************************/
int CLogger::svc()
{
	while ( true )
	{
		//����������У�ִ������
		auto_ptr<ACE_Method_Request> mo(this->m_queCmd.dequeue ());

		if ( mo->call() == -1 )
		{
			break;
		}
	}
	return 0;
}



/**************************************************************
Function:       CLogCmd::CLogCmd
Description:    CLogCmd����ι��캯��
Input:          pLog    CLogger����ָ�� 
				ucEvtType ��־����     
				uiEvtValue ��־ֵ
Output:         ��
Return:         ��
***************************************************************/
CLogCmd::CLogCmd(CLogger *pLog,Byte ucEvtType,Uint uiEvtValue)
{
	this->m_pLog       = pLog;
	this->m_ucEvtType  = ucEvtType;
	this->m_uiEvtValue = uiEvtValue;
}

/**************************************************************
Function:       CLogCmd::call
Description:    ����CLogger������־��Ӻ���
Input:          ��
Output:         ��
Return:         0
***************************************************************/
int CLogCmd::call()
{
	this->m_pLog->WriteEventLog(m_ucEvtType,m_uiEvtValue);
	return 0;
}


