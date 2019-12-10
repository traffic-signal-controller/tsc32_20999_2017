 /***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   LampBoard.cpp
Date:       2013-4-13
Description:信号机灯手控操作处理类文件.包含对手控面板全红，黄闪，步进,手动自动切换按钮操作的处理。
Version:    V1.0
History:    2013.05.29 修改按钮按下和松开的处理。
		  2013.05.31 添加当手动状态维护5分钟以上时自动切换为自动状态.。
		  2013.09.10 修改按钮接收处理方式，简化代码。
		  2014.10.06 修改按钮接受处理方式，由位移循环判断修改位读取键值进行判断
		  2014.10.23 �����֧�����߰����ֿؿ���
********************************************************************************************/
#include "WirelessButtons.h"
#include "TscMsgQueue.h"
#include "ace/Date_Time.h"
#include "MainBoardLed.h"
#include "TimerManager.h"
#include "GbtMsgQueue.h"
#include "Can.h"
#include "LampBoard.h"
#include "ComFunc.h"

/***************��ǰ�����ֿ�����״̬****************************************/
enum{
	 BUTTON_MANUAL_SELF 		= 0x0,	//��������
	 BUTTON_MANUAL_MANUAL 		= 0x1,	//�ֶ�����	
	 BUTTON_MANUAL_YELLOW_FLASH	= 0x2,	//����
	 BUTTON_MANUAL_ALL_RED		= 0x3,	//ȫ��
	 BUTTON_MANUAL_NEXT_STEP	= 0x4,	//��һ��
	 BUTTON_MANUAL_NEXT_PHASE	= 0x5,	//��һ��λ
	 BUTTON_MANUAL_NEXT_DIREC	= 0x6,	//��һ����
	 BUTTON_SPECIAL_DIREC		= 0x7	//���������ָ���������λ��������
	
};


/***************��ǰ���е�״̬****************************************/
enum{

	LAMP_OFF = 0x0 ,         //Ϩ��
	LAMP_GreenOn = 0x1 ,     //�̵���
	LAMP_GreenFLASH = 0x2 ,  //����
	LAMP_YELLOW = 0x3 ,      //�Ƶ���
	LAMP_YELLOWFLASH = 0x4 , //����
	LAMP_RED = 0x5           //�����
};

/**************************************************************
Function:       CWirelessBtn::CWirelessBtn
Description:    CWirelessBtn类构造函数，用于类初始化处理				
Input:          无              
Output:         无
Return:         无
***************************************************************/
CWirelessBtn::CWirelessBtn() 
{	
	m_ucLastManualSts = BUTTON_MANUAL_SELF; //��ʼ�������Զ�����״̬	
	m_ucdirectype = 0x0 ; 					//�������� 0-���� 1- ���� 2- �Ϸ� 3-����	
	bFirstOperate = true ;
	bTransitSetp  = false ;
	RecvoldDirec = 0 ;
	bHandleWirelessBtbMsg = false ;
	pManaKernel = CManaKernel::CreateInstance();
	m_pWirelessBtnTimer = CWirelessBtnTimer::CreateInstance();
	m_tActiveWirelessBtnTimer.timer_queue()->gettimeofday(getCurrTime);
	m_tActiveWirelessBtnTimer.activate();
}

/**************************************************************
Function:       CWirelessBtn::~CWirelessBtn
Description:    CWirelessBtn类	析构函数	
Input:          无              
Output:         无
Return:         无
***************************************************************/
CWirelessBtn::~CWirelessBtn() 
{
	;
}

/**************************************************************
Function:       Manual::CreateInstance
Description:    创建	Manual静态对象
Input:          无              
Output:         无
Return:         静态对象指针
***************************************************************/
CWirelessBtn* CWirelessBtn::CreateInstance()
{
	static CWirelessBtn cWirelessBtn;
	return &cWirelessBtn;
}

/****************************************************************
Function:       CWirelessBtn::EntryWirelessManul
Description:    �˳�����ң�ذ������ָ���������
Input:           ��          
Output:         ��
Return:         ��
Date:           20141023
*****************************************************************/

void CWirelessBtn::BackToAuto()
{
	if(m_ucLastManualSts==BUTTON_MANUAL_NEXT_DIREC || m_ucLastManualSts==BUTTON_SPECIAL_DIREC) //���ⷽ���Զ����ص�ʱ���й��ɲ�
	{		
			bTransitSetp = true ;	
			pManaKernel->SetWirelessBtnDirecCfg(RecvoldDirec,0x2);						
			ACE_OS::sleep(2);
			pManaKernel->SetWirelessBtnDirecCfg(RecvoldDirec,0x1);
			ACE_OS::sleep(3);		
			bTransitSetp = false ;
		
	}	
	else if(m_ucLastManualSts==BUTTON_MANUAL_NEXT_STEP || m_ucLastManualSts==BUTTON_MANUAL_MANUAL) //�����ֶ���һ���˳� ��Ƶƹ���3��
	{
		
		bTransitSetp = true ;
		SetCurrenStepFlash(0x1);		
		ACE_OS::sleep(3);
		bTransitSetp = false ;
	}
	CMainBoardLed::CreateInstance()->DoAutoLed(true);
	CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_SYSTEMCONTROL,0);	
	pManaKernel->SetCycleStepInfo(0); //���������������ڵĲ�����Ϣ����
	
	CLampBoard::CreateInstance()->SetLamp(pManaKernel->m_pRunData->sStageStepInfo[pManaKernel->m_pRunData->ucStepNo].ucLampOn,pManaKernel->m_pRunData->sStageStepInfo[pManaKernel->m_pRunData->ucStepNo].ucLampFlash); //�˳�����ʱ�����¸��ƿذ��ɫ��Ϣ
	bFirstOperate = true ;
	m_ucLastManualSts = BUTTON_MANUAL_SELF;
	pManaKernel->m_pRunData->ucManualType = Manual_CTRL_NO;
	CMainBoardLed::CreateInstance()->SetSignalLed(LED_WIRELESSBUTTON,LED_STATUS_ON);
	
	ACE_DEBUG((LM_DEBUG,"%s:%d Back to Auto-Ctrl from WirelessButtons! \n",__FILE__,__LINE__));
}



/****************************************************************
*
Function:       CWirelessBtn::EntryWirelessManul
Description:    ���ν������߰����ֶ�״̬
Input:           ��          
Output:         ��
Return:         ��
Date:           20141023
*****************************************************************/

void CWirelessBtn::EntryWirelessManul()
{
	CMainBoardLed::CreateInstance()->DoAutoLed(false);					
	CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_CURTSC_CTRL,1);
	ACE_DEBUG((LM_DEBUG,"%s:%d First Entry  WirelessBtnManual TscMsg! \n",__FILE__,__LINE__));
	bTransitSetp = true ;
	if(!SetCurrenStepFlash(0x2)) //��ǰ������λ������̵�������
	//�����ǰ�Ƿ��̵�״̬������������Ч������������Ƶƽ������������ж�
	{
		SetCurrenStepFlash(0x3); //��ǰ������λ�Ƶ������
	}
	bTransitSetp = false ;
	RecvoldDirec = 0 ;	
	m_ucLastManualSts = BUTTON_MANUAL_MANUAL;
	pManaKernel->m_pRunData->ucManualType = Manual_CTRL_WIRELESSBUTTONS;	
	CMainBoardLed::CreateInstance()->SetSignalLed(LED_WIRELESSBUTTON,LED_STATUS_FLASH);

}

/****************************************************************
*
Function:       CWirelessBtn::RecvMacCan
Description:    �Դ�ң������������İ���CAN���ݽ��д���
Input:            Can���߽��յ������߰�������֡            
Output:         ��
Return:         ��
Date:           20141023
*****************************************************************/
void CWirelessBtn::RecvMacCan(SCanFrame sRecvCanTmp)
{
	if(pManaKernel->m_pRunData->ucManualType != Manual_CTRL_NO && pManaKernel->m_pRunData->ucManualType != Manual_CTRL_WIRELESSBUTTONS)
		return ; 
	Byte RecvType = sRecvCanTmp.pCanData[0] & 0x3F ;
	if(RecvType == 0x2 && bTransitSetp == false) //RecvType == 0x2 ��ʾ���հ�����Ϣ,�ֿطǹ��ɲ�
	{
		sWirelessBtnCanMsg = sRecvCanTmp ;
		bHandleWirelessBtbMsg = true ;    //�����ֿ��߳̿��Դ�������Ϣ
	}
	else
		return ;
}


/**************************************************************
Function:       CWirelessBtn::HandWirelessBtnMsg
Description:    �̴߳��������ֿ���Ϸ���ķ��е�ɫ
Input:           ��             
Output:         ��
Return:          ��
Date:            20141117
***************************************************************/
void CWirelessBtn::HandWirelessBtnMsg() //ADD:201411171521
{		
		bHandleWirelessBtbMsg = false ;
		CGbtMsgQueue *pGbtMsgQueue = CGbtMsgQueue::CreateInstance();
		Byte RecvButtonType = sWirelessBtnCanMsg.pCanData[1] ;
		Uint DirecButtonCfg = ((sWirelessBtnCanMsg.pCanData[2]|sWirelessBtnCanMsg.pCanData[3]<<8)|sWirelessBtnCanMsg.pCanData[4]<<16)|sWirelessBtnCanMsg.pCanData[5]<<24;
		Byte iAuto_Manul = RecvButtonType&0x1 ;	
		if(iAuto_Manul == 0x0) //�Զ�״̬
		{
			if(m_ucLastManualSts == BUTTON_MANUAL_SELF) //����ϴ�״̬�Ѿ�����������
				return ;
			else
			{
				//TscBeep();
				BackToAuto();
				CloseWirelessBtn();								
				m_pWirelessBtnTimer->Resettimeout();  //�˳���ʱ����ʱ������Ϊ0
			}
		}
		else if(iAuto_Manul == 0x1) //�ֿ�״̬
		{				
			if(m_ucLastManualSts == BUTTON_MANUAL_SELF) //����ϴ�״̬����������
			{
				//TscBeep();
				EntryWirelessManul();
				StartWirelessBtnTimer();				
			}		
			else
			{									
				CWirelessBtnTimer::CreateInstance()->Resettimeout();  //���ڽ��������ֿز����򣬳�ʱ������Ϊ0
				if((RecvButtonType>>1)&0x1)  //�����������
				{
					if(m_ucLastManualSts == BUTTON_MANUAL_YELLOW_FLASH)
					{
						return ;
					}
					else
					{
						//TscBeep();
						pGbtMsgQueue->SendTscCommand(OBJECT_SWITCH_SYSTEMCONTROL,254);						
						pManaKernel->SndMsgLog(LOG_TYPE_MANUAL,2,0,0,0); //ADD:20141031
						ACE_DEBUG((LM_DEBUG,"%s:%d Send WirelessBtn FLASH! TscMsg!\n",__FILE__,__LINE__));
						m_ucLastManualSts = BUTTON_MANUAL_YELLOW_FLASH;
						pManaKernel->m_pRunData->flashType = CTRLBOARD_FLASH_MANUALCTRL;
						bFirstOperate = true ;
					}
				}
				else if((RecvButtonType>>2)&0x1)//����ȫ�����
				{
					if (m_ucLastManualSts == BUTTON_MANUAL_ALL_RED)
					{
						return ;
					}
					else
					{
						//TscBeep();
						pGbtMsgQueue->SendTscCommand(OBJECT_SWITCH_SYSTEMCONTROL,253);
						pManaKernel->SndMsgLog(LOG_TYPE_MANUAL,4,0,0,0); //ADD:20141031
						ACE_DEBUG((LM_DEBUG,"%s:%d Send WirelessBtn  ALLRED TscMsg!\n",__FILE__,__LINE__));
						m_ucLastManualSts = BUTTON_MANUAL_ALL_RED;
						bFirstOperate = true ;
					}
				}
				else if((RecvButtonType>>3)&0x1)//�����һ������
				{
					if(m_ucLastManualSts == BUTTON_MANUAL_MANUAL || m_ucLastManualSts == BUTTON_MANUAL_NEXT_STEP)  //��һ��ֻ�����ֿغ�ֱ�ӽ��룬�����������޷�ֱ�ӽ���
					{
						//TscBeep();
						pGbtMsgQueue->SendTscCommand(OBJECT_GOSTEP,0);
						ACE_DEBUG((LM_DEBUG,"%s:%d Send WirelessBtn Next Step TscMsg ! \n",__FILE__,__LINE__));
						pManaKernel->SndMsgLog(LOG_TYPE_MANUAL,6,0,0,0);
						m_ucLastManualSts = BUTTON_MANUAL_NEXT_STEP;
						bFirstOperate = true ;						
					}
				}
				else if((RecvButtonType>>4)&0x1) //��һ��λ
				{
					if(m_ucLastManualSts == BUTTON_MANUAL_MANUAL || m_ucLastManualSts == BUTTON_MANUAL_NEXT_PHASE)  //��һ��ֻ�����ֿغ�ֱ�ӽ��룬�����������޷�ֱ�ӽ���
					{
						//TscBeep();
						pGbtMsgQueue->SendTscCommand(OBJECT_SWITCH_STAGE,0);
						ACE_DEBUG((LM_DEBUG,"%s:%d Send WirelessBtn Next Phase TscMsg !\n",__FILE__,__LINE__));
						if(m_ucLastManualSts != BUTTON_MANUAL_NEXT_PHASE)
						m_ucLastManualSts = BUTTON_MANUAL_NEXT_PHASE;	
						bFirstOperate = true ;						
					}
				}
				else if((RecvButtonType>>5)&0x1)//��һ����
				{
					//TscBeep();
					if(m_ucLastManualSts == BUTTON_MANUAL_MANUAL && m_ucAllowLampStatus == LAMP_GreenFLASH) 
					{
						
						SetCurrenStepFlash(0x1); //����ֶ�������������Ƶƹ���
						ACE_OS::sleep(3);
					}
					else if(m_ucLastManualSts == BUTTON_MANUAL_NEXT_STEP)
					{
						
						bTransitSetp = true ;	
						SetCurrenStepFlash(0x2); //��ǰ������λ�����	
						ACE_OS::sleep(2);	
						SetCurrenStepFlash(0x1); //��ǰ������λ�Ƶ�
						bTransitSetp = false ;
					}
					SThreadMsg sTscMsg ;
					sTscMsg.ulType       = TSC_MSG_MANUALBUTTON_HANDLE; 
					sTscMsg.ucMsgOpt     = 0;
					sTscMsg.uiMsgDataLen = 0;			
					sTscMsg.pDataBuf     = ACE_OS::malloc(4);
					ACE_OS::memset((Byte*)sTscMsg.pDataBuf,0x0,4);
					((Byte*)sTscMsg.pDataBuf)[(m_ucdirectype++)%4] = 0x7 ;
					CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));	
					ACE_DEBUG((LM_DEBUG,"%s:%d Send Next WirelessBtn DIRE=%d !\n",__FILE__,__LINE__,m_ucdirectype-1));
					if(m_ucLastManualSts != BUTTON_MANUAL_NEXT_DIREC)
						m_ucLastManualSts = BUTTON_MANUAL_NEXT_DIREC;
					pManaKernel->m_pRunData->uiWorkStatus = STANDARD ;					
				}
				else if(DirecButtonCfg != 0x0)
				{
					//TscBeep();
					if(m_ucLastManualSts == BUTTON_MANUAL_MANUAL && m_ucAllowLampStatus == LAMP_GreenFLASH) 
					{
						SetCurrenStepFlash(0x1); //����ֶ�������������Ƶƹ���
						ACE_OS::sleep(3);
					}
					else if(m_ucLastManualSts == BUTTON_MANUAL_NEXT_STEP)
					{
						bTransitSetp = true ;	
						SetCurrenStepFlash(0x2); //��ǰ������λ�����	
						ACE_OS::sleep(2);	
						SetCurrenStepFlash(0x1); //��ǰ������λ�Ƶ�
						bTransitSetp = false ;
					}
					SThreadMsg sTscMsg ;
					sTscMsg.ulType       = TSC_MSG_MANUALBUTTON_HANDLE; 
					sTscMsg.ucMsgOpt     = 0;
					sTscMsg.uiMsgDataLen = 4;			
					sTscMsg.pDataBuf     = ACE_OS::malloc(4);
					ACE_OS::memcpy((Byte*)sTscMsg.pDataBuf,&DirecButtonCfg,4);
					CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));
					if(m_ucLastManualSts != BUTTON_SPECIAL_DIREC)
						m_ucLastManualSts = BUTTON_SPECIAL_DIREC;
					pManaKernel->m_pRunData->uiWorkStatus = STANDARD ; //�����޷��ٴν��������ȫ��					
				}		
			}
		}
	
	
}

/**************************************************************
Function:       CWirelessBtn::GetbHandleWirelessBtnMsg
Description:    ���������ֿ���Ϸ���ķ��е�ɫ
Input:           RecvDircData  -���з�������             
Output:         ��
Return:         boolֵ,true-���̴߳��������ֿذ��� false-���账��
Date:            20141117
***************************************************************/
bool CWirelessBtn::GetbHandleWirelessBtnMsg()
{
	return bHandleWirelessBtbMsg ;
}

/**************************************************************
Function:       CWirelessBtn::HandleSpecialDirec
Description:    ���������ֿ���Ϸ���ķ��е�ɫ
Input:           RecvDircData  -���з�������             
Output:         ��
Return:         ��
Date:            20141105
***************************************************************/
void CWirelessBtn::HandleSpecialDirec(Uint * RecvtmpDircData)
{
	Uint RecvDircData = *RecvtmpDircData ;	
	if(bTransitSetp == false)  //�ǹ��ɲ�
	{	
		if(bFirstOperate)
		{
			pManaKernel->SetWirelessBtnDirecCfg(RecvDircData,0x0);
			bFirstOperate = false ;
			RecvoldDirec = RecvDircData ;
			//ACE_OS::printf("%s:%d First special direc ,no Transit setp !\n",__FILE__,__LINE__);
		}
		else
		{
			bTransitSetp = true ;	
			pManaKernel->SetWirelessBtnDirecCfg(RecvoldDirec,0x2);
						
			//ACE_OS::printf("%s:%d Transit setp Green Flash sleep 2 sec!\n",__FILE__,__LINE__);
			ACE_OS::sleep(2);
			pManaKernel->SetWirelessBtnDirecCfg(RecvoldDirec,0x1);
						
			//ACE_OS::printf("%s:%d Transit setp Yellow  sleep 3 sec!\n",__FILE__,__LINE__);
			ACE_OS::sleep(3);
			pManaKernel->SetWirelessBtnDirecCfg(RecvDircData,0x0);
			bTransitSetp  = false ;
			RecvoldDirec = RecvDircData ;
		}			
	
	}

}

/**************************************************************
Function:       CWirelessBtn::SetCurrenStepFlash
Description:    ���õ�ǰ�������ں��ֵ�ɫ״̬�����ڽ����ֿص�ʱ�����
Input:           LampColor--��ɫ״̬           
Output:         ��
Return:         ��
Date:            20141105
***************************************************************/

bool  CWirelessBtn::SetCurrenStepFlash(Byte LampColor)
{
	//ACE_OS::printf("%s:%d First WirelessManual Flash!\n",__FILE__,__LINE__);
	bool bSetLampColor = false ;
	Byte LampColorStatus = 0 ;
	Byte m_ucLampOn[MAX_LAMP]={0}; 
	Byte m_ucLampFlash[MAX_LAMP]={0}; 
	ACE_OS::memcpy(m_ucLampOn,pManaKernel->m_pRunData->sStageStepInfo[pManaKernel->m_pRunData->ucStepNo].ucLampOn,MAX_LAMP);	
	ACE_OS::memcpy(m_ucLampFlash,pManaKernel->m_pRunData->sStageStepInfo[pManaKernel->m_pRunData->ucStepNo].ucLampFlash,MAX_LAMP);
	
	for(Byte LampIndex = 0 ;LampIndex< 60 ; LampIndex++)
	{
		LampColorStatus  = pManaKernel->m_pRunData->sStageStepInfo[pManaKernel->m_pRunData->ucStepNo].ucLampOn[LampIndex];
		if(LampColorStatus ==1)
		{
			 if(LampIndex%3 ==2)
			{
				if(LampColor ==0x2)//����
					{
						m_ucLampFlash[LampIndex]=0x1;
						if(!bSetLampColor)
							bSetLampColor = true ;
						m_ucAllowLampStatus = LAMP_GreenFLASH;
					}
				if(LampColor == 0x1)//�Ƶ�
				{		
					m_ucLampOn[LampIndex]=0;
					m_ucLampFlash[LampIndex]=0;
					
					if((LampIndex+1)%12==0) //����
					{
						m_ucLampOn[LampIndex-2] = 0x1 ;
						
					}
					else
					{
						m_ucLampOn[LampIndex-1]=0x1;
						
					}
					if(!bSetLampColor)
							bSetLampColor = true ;
					m_ucAllowLampStatus = LAMP_YELLOW ;
				}
				if(LampColor == 0x3)//����
				{	
					if(m_ucLampFlash[LampIndex]==0x1)
					{
						m_ucLampOn[LampIndex]=0x0;
						m_ucLampFlash[LampIndex]=0x0;
						if((LampIndex+1)%12==0) //���в��Ƶƺ���˸
						{
							
							m_ucLampOn[LampIndex-2] = 0x1 ; 	 //���������						
						}
						else
						{
							m_ucLampOn[LampIndex-1]=0x1;    //�Ƶ���
							m_ucLampFlash[LampIndex-1]=0x1;
						}
						
						if(!bSetLampColor)
							bSetLampColor = true ;
						m_ucAllowLampStatus = LAMP_YELLOWFLASH ;
					}
				}
			
			}
			if(LampIndex%3 ==1)
			{
				if(LampColor == 0x3)//����
				{
						if((LampIndex+2)%12==0) //���в��Ƶƺ���˸
						{
							m_ucLampOn[LampIndex]=0x0;
							m_ucLampFlash[LampIndex]=0x0;
							m_ucLampOn[LampIndex-1] =0x1;
						}
						else
							m_ucLampFlash[LampIndex]=0x1;
					if(!bSetLampColor)
							bSetLampColor = true ;
					m_ucAllowLampStatus = LAMP_YELLOWFLASH;
				}

			}
		}
	}
	
	if(bSetLampColor)
		CLampBoard::CreateInstance()->SetLamp(m_ucLampOn,m_ucLampFlash);
}


/**************************************************************
Function:		CWirelessBtn::StartWirelessBtnTimer
Description:	��������ң������ʱ��ʱ��,Ƶ��1��
Input:		�� 			
Output: 		��
Return: 		��
Date:		 20141118
***************************************************************/

void CWirelessBtn::StartWirelessBtnTimer()
{
	m_lWirelessBtnTimerId = m_tActiveWirelessBtnTimer.schedule(m_pWirelessBtnTimer,NULL,getCurrTime(),ACE_Time_Value(1)); 
}

/**************************************************************
Function:		CWirelessBtn::CloseWirelessBtn
Description:	�ر�����ң������ʱ��ʱ��
Input:			 �� 			
Output: 		��
Return: 		��
Date:	     20141118
***************************************************************/
void CWirelessBtn::CloseWirelessBtn()
{
	if ( m_lWirelessBtnTimerId > 0 )
	{
		m_tActiveWirelessBtnTimer.cancel(m_lWirelessBtnTimerId);
	}
}


/**************************************************************
Function:       CWirelessBtnTimer::CWirelessBtnTimer
Description:    CWirelessBtnTimer�๹�캯��
Input:           ��             
Output:         ��
Return:         ��
Date:            20141023
***************************************************************/
CWirelessBtnTimer::CWirelessBtnTimer()
{
	m_uctimeout = 0 ;
	//bAutoExit = false ;
	m_ucSystemSetTimeOut = CManaKernel::CreateInstance()->m_pTscConfig->sSpecFun[FUN_WIRELESSBTN_TIMEOUT].ucValue*60 ;
	ACE_DEBUG((LM_DEBUG,"%s:%d Init WirelessBtnTimer object ok !\n",__FILE__,__LINE__));

}

/**************************************************************
Function:       CWirelessBtnTimer::~CWirelessBtnTimer
Description:    CTscTimer����������
Input:          ��               
Output:         ��
Return:         ��
Date:            20141023
***************************************************************/
CWirelessBtnTimer::~CWirelessBtnTimer()
{
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct CWirelessBtnTimer object ok !\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CWirelessBtnTimer::CreateInstance
Description:    ������������
Input:          ��          
Output:       ��
Return:       CWirelessBtnTimer����ָ��
***************************************************************/

CWirelessBtnTimer* CWirelessBtnTimer::CreateInstance()
{
	static CWirelessBtnTimer cWirelessBtnTimer;
	return &cWirelessBtnTimer;
}


/**************************************************************
Function:       CWirelessBtnTimer::handle_timeout
Description:    �źŻ�ң������ť��ʱ����ʱ�ص�������1sִ��һ�Ρ������źŻ�
				������ʱ������
Input:           Ĭ�ϴ����û���������              
Output:         ��
Return:         0
Date:            20141023
***************************************************************/
int CWirelessBtnTimer::handle_timeout(const ACE_Time_Value &tCurrentTime, const void * /* = 0 */)
{
	m_uctimeout++;	
	if(m_uctimeout >=  m_ucSystemSetTimeOut )
	{			
		CWirelessBtn::CreateInstance()->BackToAuto();
		CWirelessBtn::CreateInstance()->CloseWirelessBtn();		
		m_uctimeout = 0 ;
		ACE_OS::printf("%s:%d WirelessBtn Manual Control timeout ,Back to AutoSelf Control!\r\n",__FILE__,__LINE__);
		
	}
	return 0 ;
}

/**************************************************************
Function:       CWirelessBtnTimer::Resettimeout
Description:    ���ó�ʱ����

Input:           ��           
Output:         ��
Return:         ��
Date:            20141023
***************************************************************/

void CWirelessBtnTimer::Resettimeout()
{
	m_uctimeout = 0 ;
}


