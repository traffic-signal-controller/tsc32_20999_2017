/***********************************************************************************************************************
Copyright(c) 2017  AITON. All rights reserved.
Author:     AITON
FileName:   Gb25280timer.cpp
Date:       2017-5-1
Description:�źŻ�GB25280-2016Э�鶨ʱ�������ࡣ
                     ���ڶ�ʱ������������������ѯ����ͨ�������ϴ����źŻ��汾���������ϴ�
                     ��ʱ����������ʱ��1��һ��.                     
Version:    V1.0
History:
************************************************************************************************************************/
#include "Gb25280timer.h"
#include "ComDefGb25280.h"
#include "ace/Date_Time.h"	
/**************************************************************
Function:       CGB25280Timer::CGB25280Timer
Description:    CGB25280Timer�๹�캯��
Input:            ��                 
Output:         ��
Return:         ��
***************************************************************/
CGB25280Timer::CGB25280Timer()
{
	m_ucTick    =0x0;
	m_ucMaxTick = 0xA;
	ucNoQueryResponseTime = 0x0 ;               //������ѯ��δ�յ�Ӧ�����
	pCTscKernelGb25280= CTscKernelGb25280::CreateInstance();	
	pGbtMsgQueue = CGbtMsgQueue::CreateInstance();
	pBytesArrayRequest = pCTscKernelGb25280->ConstruectOnlineRequest(BytesRequestLen);  // ������������������
	pBytesArrayQuery     =   pCTscKernelGb25280->ConstruectOnlineQuery(BytesQueryLen);     //��ȡ������ѯ��������
	pAddrClient = new ACE_INET_Addr(pCTscKernelGb25280->TscGB25280TscConfig.iPort,pCTscKernelGb25280->TscGB25280TscConfig.TscClientIp.c_str());
	ACE_DEBUG((LM_DEBUG,"\n%s:%d Init Gb25280Timer object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CGB25280Timer::~CGB25280Timer
Description:    CGB25280Timer����������
Input:            ��               
Output:         ��
Return:         ��
***************************************************************/
CGB25280Timer::~CGB25280Timer()
{
       if(pBytesArrayRequest != NULL)
      		 delete []pBytesArrayRequest;
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct Gbt25280Timer object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:        CGB25280Timer::CreateInstance
Description:    ����CGB25280Timer��̬��������
Input:             ��              
Output:         ��
Return:         CGB25280Timer��̬����ָ��
***************************************************************/
CGB25280Timer* CGB25280Timer::CreateInstance()
{
	static CGB25280Timer cGB25280Timer;
	return &cGB25280Timer;
}

/******************************************************************************************************
Function:       CGB25280Timer::handle_timeout
Description:    �źŻ���GB25280��ʱ����ʱ�ص�������1000msִ��һ��
Input:             Ĭ�ϴ����û���������              
Output:         ��
Return:         0
******************************************************************************************************/
int CGB25280Timer::handle_timeout(const ACE_Time_Value &tCurrentTime, const void * /* = 0 */)
{       
	switch(m_ucTick)
	{
        case 0:
	  {			
		if(  pCTscKernelGb25280->TscGB25280TscConfig.TscCommunicationStatus== TSC_COMMUICATION_OFFLINE)
		{		
		      pCTscKernelGb25280->SendBytesGbDataToClient(pBytesArrayRequest,BytesRequestLen);   //����λ�˷�����������ָ��		   
		}
		break;
	   }
       case 1:
	   	break;
       case 2:
	   	break;
	   case 3:
	   	{ 	
		     if(pCTscKernelGb25280->TscGB25280TscConfig.bUpTrafficFlow ==true && pCTscKernelGb25280->TscGB25280TscConfig.TscCommunicationStatus == TSC_COMMUICATION_ONLINE)
		     {
				  Byte * pBytesTrafficFlowUp =pCTscKernelGb25280->ConstruectTrafficFlowSelfReport( BytesQueryLen);
				  if(pBytesTrafficFlowUp != NULL)
				  {
					  pCTscKernelGb25280->SendBytesGbDataToClient(pBytesTrafficFlowUp,BytesQueryLen);	  //����λ�˷��ͽ�ͨ�������ϴ�����
					  delete []pBytesTrafficFlowUp;
				  }
				  	
		      }
	      }
	   	break ;
	   case 4:	
	    {			
	         if(pCTscKernelGb25280->TscGB25280TscConfig.TscCommunicationStatus == TSC_COMMUICATION_ONLINE)
		    {	
			     pCTscKernelGb25280->SendBytesGbDataToClient(pBytesArrayQuery,BytesQueryLen);	 //����λ�˷���������ѯָ��	
			     pCTscKernelGb25280->TscGB25280TscConfig.bGetQueryResponse= false ;
		    }
	    }
	   break ;
	   case 5:	
		break ;	   
	   case 6:
	   	break ;
	   case 7:
	   	{
		//������ѯָ���3���û�յ�Ӧ����,δ�յ�Ӧ�����+1,���ۻ������ﵽ3�����źŻ�����Ϊ����״̬
		if(  pCTscKernelGb25280->TscGB25280TscConfig.TscCommunicationStatus == TSC_COMMUICATION_ONLINE&&pCTscKernelGb25280->TscGB25280TscConfig.bGetQueryResponse== false)
		{
		    ucNoQueryResponseTime++;		   
		 //  ACE_OS::printf("%s:%d TscGB25280TscConfig.TscCommunicationStatus time out time =%d!\r\n",__FILE__,__LINE__,ucNoQueryResponseTime);
		    if(ucNoQueryResponseTime ==0x3)
		     {
			     pCTscKernelGb25280->TscGB25280TscConfig.TscCommunicationStatus= TSC_COMMUICATION_OFFLINE;			
			     ucNoQueryResponseTime = 0x0;			
		     }
		}		
	     }
	   	 break ;
	   case 8:	   	
	    {			
	         if(pCTscKernelGb25280->TscGB25280TscConfig.TscCommunicationStatus == TSC_COMMUICATION_ONLINE)
		   {			
		        if(pCTscKernelGb25280->TscGB25280TscConfig.bUpTrafficFlow ==true )
		         {
				  Byte *  pBytesTrafficFlowUp =pCTscKernelGb25280->ConstruectTrafficFlowSelfReport( BytesQueryLen);  //��ͨ����Ϣ�����ϴ����ݷ���
				  if(pBytesTrafficFlowUp != NULL)
				  {				  
					  pCTscKernelGb25280->SendBytesGbDataToClient(pBytesTrafficFlowUp,BytesQueryLen);	
					  delete []pBytesTrafficFlowUp;
				  }		 
				  					  
		         }
			if(pCTscKernelGb25280->TscGB25280TscConfig.bUpTscVerUp ==true)
			{
				Byte *pBytesTssVerUp =pCTscKernelGb25280->ConstruectscVersionSelfReply( BytesQueryLen); //�źŻ��汾���������ϴ����ݷ���
				 if(pBytesTssVerUp != NULL)
				 {
					 pCTscKernelGb25280->SendBytesGbDataToClient(pBytesTssVerUp,BytesQueryLen);    
					 delete []pBytesTssVerUp;
				 }
			}
		    }		 
		     
	       }
	   	break;
	   case 9:	   	
	   	break;
	   default:
	   	break ;
	}
	m_ucTick++;
	if(m_ucTick>=m_ucMaxTick)
		m_ucTick =0x0;
	return 0;
}



