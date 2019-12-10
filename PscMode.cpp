
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   PscMode.cpp
Date:       2013-1-1
Description:���˰�ťģʽ�����ļ���
Version:    V1.0
History:
***************************************************************/
#include "PscMode.h"
#include "PowerBoard.h"
#include "LampBoard.h"
#include "GaCountDown.h"
#include "MacControl.h"

/*
���˰�ťģʽ״̬����ö��
*/
enum
{
	PSC_STAND_STATUS     = 0 ,  //����״̬
	PSC_WAIT_PG_STATUS   = 1 ,  //�ȴ�����״̬
	PSC_PG_ENABLE_STATUS = 2 ,  //����״̬
	PSC_NEXT_PG_STATUS   = 3 ,
	PSC_OTHER_STATUS    
};

/*
���˰�ť�ֲ�ö��
*/
enum
{
	PSC_LEFT_PUSH1  = 0 ,  
	PSC_RIGHT_PUSH1 = 1 ,
	PSC_LEFT_PUSH2  = 2 ,
	PSC_RIGHT_PUSH2 = 3 ,
	PSC_OTHER_PUSHN
};

/**************************************************************
Function:       CPscMode::CPscMode
Description:    CPscMode�๹�캯��
Input:          ��                 
Output:         ��
Return:         ��
***************************************************************/
CPscMode::CPscMode()
{	
	InitPara();
#ifdef TSC_DEBUG
	ACE_DEBUG((LM_DEBUG,"create CPscMode\n"));
#endif
}

/**************************************************************
Function:       CPscMode::~CPscMode
Description:    CPscMode����������
Input:          ��               
Output:         ��
Return:         ��
***************************************************************/
CPscMode::~CPscMode()
{
#ifdef TSC_DEBUG
	ACE_DEBUG((LM_DEBUG,"delete CPscMode\n"));
#endif
}

/**************************************************************
Function:       CPscMode::CreateInstance
Description:    ����	CPscMode��̬����
Input:          ��              
Output:         ��
Return:         ��̬����ָ��
***************************************************************/
CPscMode* CPscMode::CreateInstance()
{
	static CPscMode oPscMode;

	return &oPscMode;
}


/**************************************************************
Function:       CPscMode::InitPara
Description:    ��ʼ�����˰�ť�������ʼ��
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CPscMode::InitPara()
{
	ACE_OS::memset(m_bBoxPush , 0 , MAX_PUSHS);
	m_ucPscStatus       = PSC_STAND_STATUS;
	m_bStep1ToStep2     = true;
	m_bCrossStage1First = true;
	m_pWorkParaManager  = CManaKernel::CreateInstance();
	m_bStartCntCown     = false;
	m_ucBtnNum = 0 ;
	m_ucNextTime = 0 ;
	m_psc_intervaltime = 10 ;
}


/**************************************************************
Function:       CPscMode::DealButton
Description:    ��ȡ���˰�ť����״̬ 500ms��ѯһ��
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CPscMode::DealButton()
{
	bool bPressBtn          = false;
	Byte ucIndex            = 0;
	Byte ucStageStand       = 0;
	Byte ucStageCross1      = 0;
	Byte ucStageCross2      = 0;
	Byte ucButton           = 0;
	//CPowerBoard::CreateInstance()->m_ucCurFootBtn;	
	time_t ucNow            = time(NULL);
	GBT_DB::SpecFun* pSpecFun = m_pWorkParaManager->m_pTscConfig->sSpecFun;
	m_psc_intervaltime = pSpecFun[FUN_CROSSSTREET_TIMEOUT].ucValue ;;
	m_ucBtnNum = CMacControl::CreateInstance()->m_ucPsc ;
	while ( ucIndex < MAX_PUSHS )
	{
		if ( (m_ucBtnNum >> ucIndex) & 1 )
		{
			m_bBoxPush[ucIndex] = true;
		}
		ucIndex++;
	}

	ucIndex = 0;
	while ( ucIndex < MAX_PUSHS )
	{
		if ( m_bBoxPush[ucIndex] )
		{
			bPressBtn = true;
		}
		ucIndex++;
	}
	
	if ( !bPressBtn || PSC_STAND_STATUS != m_ucPscStatus|| (ucNow -m_ucNextTime < m_psc_intervaltime ) )
	{
		//printf("\niCtn = %d\n",iCtn);
		m_ucBtnNum = 0 ;
		return;
	}
	
	ACE_DEBUG((LM_DEBUG,"%s:%d You have push button number is %d\n",__FILE__,__LINE__,m_ucBtnNum));

	m_ucCurStep = m_pWorkParaManager->m_pRunData->ucStepNo;
	if ( pSpecFun[FUN_STAND_STAGEID].ucValue > 0 )
	{
		ucStageStand   = pSpecFun[FUN_STAND_STAGEID].ucValue - 1;
		m_ucStandStep  = m_pWorkParaManager->StageToStep(ucStageStand);
	}
	if ( pSpecFun[FUN_CORSS1_STAGEID].ucValue > 0 )
	{
		ucStageCross1  = pSpecFun[FUN_CORSS1_STAGEID].ucValue - 1;
		m_ucCrossStep1 = m_pWorkParaManager->StageToStep(ucStageCross1);
	}
	if ( pSpecFun[FUN_CROSS2_STAGEID].ucValue > 0 )
	{
		ucStageCross2  = pSpecFun[FUN_CROSS2_STAGEID].ucValue - 1;
		m_ucCrossStep2 = m_pWorkParaManager->StageToStep(ucStageCross2);
	}
	
	if ( (PSC_STAND_STATUS == m_ucPscStatus) && (m_ucCurStep == m_ucStandStep) )
	{
		if ( MODE_PSC2 == m_pWorkParaManager->m_pRunData->ucWorkMode )
		{
			if ( m_bBoxPush[PSC_LEFT_PUSH2] || m_bBoxPush[PSC_RIGHT_PUSH2] )
			{
				m_bStep1ToStep2    = false;  //step2-->step1 step--
			}
			else
			{
				m_bStep1ToStep2 = true;   //step1-->step2 step++ 
			}

			if (   ( m_bCrossStage1First  && !m_bStep1ToStep2 )
				|| ( !m_bCrossStage1First && m_bStep1ToStep2  ) )  //��Ҫ�޸Ľ׶δ���
			{
				SScheduleTime sStageTmp;
				sStageTmp = m_pWorkParaManager->m_pRunData->sScheduleTime[ucStageCross1];
				m_pWorkParaManager->m_pRunData->sScheduleTime[ucStageCross1]
					= m_pWorkParaManager->m_pRunData->sScheduleTime[ucStageCross2];
				m_pWorkParaManager->m_pRunData->sScheduleTime[ucStageCross2]
					= sStageTmp;
				m_pWorkParaManager->SetCycleStepInfo(0);
				m_bCrossStage1First = !m_bCrossStage1First;
			}
		}

		m_ucPscStatus = PSC_WAIT_PG_STATUS;
	}

	if ( !m_bStartCntCown )  //�������¿�ʼ
	{
		m_bStartCntCown = true;

		#ifdef GA_COUNT_DOWN
		CGaCountDown::CreateInstance()->GaSendStepPer();
		#endif
	}

}


/**************************************************************
Function:       CPscMode::DecTime
Description:    ���˰�ť����ʱ����� �����źż����ڹ���ģʽʱ��1s����һ��
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CPscMode::DecTime()
{
	//if ( ( (m_pWorkParaManager->m_pTscConfig->sSpecFun[FUN_PRINT_FLAG].ucValue>>7) & 1 )  != 0 )
	//{
	//	ACE_DEBUG((LM_DEBUG,"Psc:%d ctrl:%d status:%d stage:%d stepNo:%d ucElapseTime:%d,ucStepTime:%d\n",
	//	m_ucPscStatus , m_pWorkParaManager->m_pRunData->uiCtrl , m_pWorkParaManager->m_pRunData->uiWorkStatus 
	//	, m_pWorkParaManager->m_pRunData->ucStageNo , m_pWorkParaManager->m_pRunData->ucStepNo 
	//	, m_pWorkParaManager->m_pRunData->ucElapseTime , m_pWorkParaManager->m_pRunData->ucStepTime));
//	}

	if ( ( m_ucPscStatus == PSC_STAND_STATUS )	|| (CTRL_MANUAL == m_pWorkParaManager->m_pRunData->uiCtrl ) 
		|| ( ( CTRL_PANEL == m_pWorkParaManager->m_pRunData->uiCtrl ) &&   ( (SIGNALOFF == m_pWorkParaManager->m_pRunData->uiWorkStatus ) 
		|| (ALLRED == m_pWorkParaManager->m_pRunData->uiWorkStatus)   || (FLASH == m_pWorkParaManager->m_pRunData->uiWorkStatus ) 
			 ) ) ) 
		//�ֶ����ƻ�(���صơ�ȫ�졢����)
	{
		return;
	}

	m_pWorkParaManager->m_pRunData->ucElapseTime++;

	if ( m_pWorkParaManager->m_pRunData->ucElapseTime >= m_pWorkParaManager->m_pRunData->ucStepTime )
	{
		GoNextStep();
	}

	if ( m_bStartCntCown 
		&& m_pWorkParaManager->m_pRunData->ucElapseTime != m_pWorkParaManager->m_pRunData->ucStepTime - 1 )  
		//������η��͵���ʱ��Ϣ
	{
	}
}


/**************************************************************
Function:       CPscMode::GoNextStep
Description:    �źŵƽ�����һ����
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CPscMode::GoNextStep()
{
	if ( ( (m_pWorkParaManager->m_pTscConfig->sSpecFun[FUN_PRINT_FLAG].ucValue>>7) & 1 )  != 0 )
	{
		ACE_DEBUG((LM_DEBUG,"\n%s:%d GoNextSetp\n" , CPSCMODE_FILE , __LINE__));
	}
	
	Byte ucNextStage      = 0;
	STscRunData* pRunData = m_pWorkParaManager->m_pRunData;

	pRunData->ucStepNo++;
		if ( pRunData->ucStepNo >= pRunData->ucStepNum )  //����һ������
		{
			pRunData->ucStepNo = m_ucStandStep;

			m_bStartCntCown = false;

			if ( ( (m_pWorkParaManager->m_pTscConfig->sSpecFun[FUN_PRINT_FLAG].ucValue>>7) & 1 )  != 0 )
			{
				ACE_DEBUG((LM_DEBUG,"\n%s:%d OverCycle\n" , CPSCMODE_FILE , __LINE__));
			}

		
		}


	pRunData->ucElapseTime = 0;
	pRunData->ucStepTime   = pRunData->sStageStepInfo[pRunData->ucStepNo].ucStepLen;
	ucNextStage = m_pWorkParaManager->StepToStage(pRunData->ucStepNo,NULL);
	CLampBoard::CreateInstance()->SetLamp(pRunData->sStageStepInfo[pRunData->ucStepNo].ucLampOn
		,pRunData->sStageStepInfo[pRunData->ucStepNo].ucLampFlash);

	if ( ucNextStage != pRunData->ucStageNo )  //��λ���л�
	{
		pRunData->ucStageNo = ucNextStage;
		PscSwitchStatus();
	}

	if ( m_bStartCntCown )
	{
		#ifdef GA_COUNT_DOWN
		
		CGaCountDown::CreateInstance()->GaSendStepPer();
		#endif
	}
	
}


/**************************************************************
Function:       CPscMode::PscSwitchStatus
Description:    psc�ı����˹���״̬
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CPscMode::PscSwitchStatus()
{
	if ( MODE_PSC1 == m_pWorkParaManager->m_pRunData->ucWorkMode )
	{
		if ( PSC_WAIT_PG_STATUS == m_ucPscStatus )
		{
			m_ucPscStatus = PSC_PG_ENABLE_STATUS;
		}
		else if ( PSC_PG_ENABLE_STATUS == m_ucPscStatus )
		{
			m_ucPscStatus = PSC_STAND_STATUS;
		}
	}
	else 
	{
		if ( PSC_WAIT_PG_STATUS == m_ucPscStatus )
		{
			m_ucPscStatus = PSC_PG_ENABLE_STATUS;
		}
		else if ( PSC_PG_ENABLE_STATUS == m_ucPscStatus )
		{
			m_ucPscStatus = PSC_NEXT_PG_STATUS;
		}
		else if ( PSC_NEXT_PG_STATUS == m_ucPscStatus )
		{
			m_ucPscStatus = PSC_STAND_STATUS;
		}
	}

	if ( m_ucPscStatus == PSC_STAND_STATUS )
	{
		for ( int iIndex = 0; iIndex < MAX_PUSHS; iIndex++ )
		{
			m_bBoxPush[iIndex] = false;
		}
		if ( m_pWorkParaManager->m_pRunData->bNeedUpdate )
		{
			m_pWorkParaManager->UpdateConfig();
		}
		m_pWorkParaManager->GetRunDataStandard();
		m_ucNextTime = time(NULL);
	}
}

