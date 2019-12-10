/***********************************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   ManaKernel.cpp
Date:       2013-1-1
Description:�źŻ�����ҵ�������ļ�
Version:    V1.0
History:    201308010930  ���48�����ݼ����������ݿ��ͱ���
		    201309251030 �޸���λ��ͻ���λ�ã�����һ�����Ƶ���һ�������м��
		    201309251120 �����־��Ϣ���ͺ���������ͬ����־��Ϣͳһ��һ����������
		    201309251030 �޸���λ��ͻ���λ�ã�����һ�����Ƶ���һ�������м��
		    201309281025 Add "ComFunc.h" headfile
		    201412091508.ע�͵�Manaker.cpp 131 132��ԭ���� ����Ĭ�����ݺ�����
		                 ȥ��Manaker.cpp ��void MemeryConfigDataRun() Ĭ�������ݿ����ݡ�
***********************************************************************************/
#include <stdlib.h>
#include "ace/Date_Time.h"
#include "ManaKernel.h"
#include "TscMsgQueue.h"
#include "LampBoard.h"
#include "GbtMsgQueue.h"
#include "TimerManager.h"
#include "DbInstance.h"
#include "GbtDb.h"
#include "MainBoardLed.h"
#include "Detector.h"
#include "GaCountDown.h"
#include "Coordinate.h"
#include "FlashMac.h"
#include "Rs485.h"
#include "ComFunc.h" 
#include "MainBackup.h"
#include "Configure.h"
#include"PscMode.h"
#define CNTDOWN_TIME 8


/**************************************************************
Function:       CManaKernel::CManaKernel
Description:    �źŻ�����CManaKernel�๹�캯������ʼ�����ı���
Input:          ucMaxTick                 
Output:         ��
Return:         ��
***************************************************************/
CManaKernel::CManaKernel()
{
	

	m_bFinishBoot           = false;
	m_bRestart              = false;
	m_bSpePhase             = false;
	m_bWaitStandard         = false;
	m_bSpeStatus            = false;
	m_bSpeStatusTblSchedule = false;
	m_bVirtualStandard      = false;
	m_bVehile 				= true ;   //ADD 20130815 1500
	m_bNextPhase			= false ;
	m_ucUtcsComCycle        = 0;   
	m_ucUtscOffset          = 0; 
	//m_ucStageDynamicMaxGreen = 0x0 ; //ADD 201508171045
	//m_ucStageDynamicMinGreen = 0x0 ; //ADD 201508171045

	m_pTscConfig = new STscConfig;      //�źŻ�������Ϣ
	m_pRunData   = new STscRunData;     //�źŻ���̬������Ϣ
	bNextDirec = false ;
	bTmpPattern = false ;
	bValidSoftWare = true ;
	bUTS  = false ;
	bDegrade = false ;    //ADD:201311121140
	//bChkManul = true ;  //ADD:201403311059
	iCntFlashTime = 0  ;
	bSecondPriority = false ; //ADD:201406191130
	//ACE_OS::memset(m_pTscConfig->sOverlapPhase,0,MAX_OVERLAP_PHASE*sizeof(SOverlapPhase));
	ACE_OS::memset(m_pTscConfig , 0 , sizeof(STscConfig) );
	ACE_OS::memset(m_pRunData , 0 , sizeof(STscRunData) );
	ACE_DEBUG((LM_DEBUG,"\n%s:%d����CManaKernel���� Init ManaKernel objetc ok!\r\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CManaKernel::~CManaKernel
Description:    CManaKernel�������������ͷ������
Input:          ��               
Output:         ��
Return:         ��
***************************************************************/
CManaKernel::~CManaKernel()
{
	if ( m_pTscConfig != NULL )
	{
		delete m_pTscConfig;
		m_pTscConfig = NULL;
	}
	if ( m_pRunData != NULL )
	{
		delete m_pRunData;
		m_pRunData = NULL;
	}
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct ManaKernel objetc ok!\n",__FILE__,__LINE__));
}


/**************************************************************
Function:       CManaKernel::CreateInstance
Description:    ����CManaKernel�ྲ̬����		
Input:          ��              
Output:         ��
Return:         CManaKernel��̬����ָ��
***************************************************************/
CManaKernel* CManaKernel::CreateInstance()
{
	static CManaKernel cWorkParaManager; 

	return &cWorkParaManager;
}


/**************************************************************
Function:       CManaKernel::InitWorkPara
Description:    ��ʼ��CManaKernel�����ಿ�ֹ������������ݿ�	
Input:          ��  ������������
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::InitWorkPara()
{	
	ACE_DEBUG((LM_DEBUG,"\n%s:%d����InitWorkPara���� Booting TSC ,initializing kernel parameters! \r\n",__FILE__,__LINE__)); //ADD: 20130523 1053	
	////����Ĭ��ֵ�����ݿ� ������ݿ��Ϊ��
	UpdateConfig();

	m_pRunData->bNeedUpdate  = false;
	m_pRunData->ucLockPhase    = 0;
	m_pRunData->bOldLock          = false;
	m_pRunData->uiCtrl                 = CTRL_UNKNOWN;
	m_pRunData->uiOldCtrl           = CTRL_UNKNOWN;
	m_pRunData->uiWorkStatus    = FLASH;
	m_pRunData->uiOldWorkStatus = FLASH;
	m_pRunData->bStartFlash  = true;
	m_pRunData->ucWorkMode   = m_pTscConfig->sSpecFun[FUN_CROSS_TYPE].ucValue ;

	//��ʼ�����ݼ�����ݣ������ݿ���ȡ��
	if(m_pTscConfig->sSpecFun[FUN_LIGHTCHECK].ucValue == 0)
		m_pRunData->bIsChkLght   = false ;
	else
		m_pRunData->bIsChkLght   = true ;
	//ACE_DEBUG((LM_DEBUG,"%s:%d m_pTscConfig->sSpecFun[FUN_LIGHTCHECK].ucValue == %d \n",__FILE__,__LINE__,m_pTscConfig->sSpecFun[FUN_LIGHTCHECK].ucValue)); //ADD: 20130523 1053	
	m_pRunData->b8cndtown    = false ;
	ResetRunData(0);	
	CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn
							    	,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);

	CMainBoardLed::CreateInstance()->DoModeLed(false,true);
	if(m_pTscConfig->sSpecFun[FUN_COMMU_PARA].ucValue== MAC_CTRL_NOTHING)
		CMainBoardLed::CreateInstance()->DoAutoLed(true);
	else		
		CMainBoardLed::CreateInstance()->DoAutoLed(false);
	if(m_pRunData->ucWorkMode==MODE_TSC)
		CMainBoardLed::CreateInstance()->DoTscPscLed(true);
	else			
		CMainBoardLed::CreateInstance()->DoTscPscLed(false);
}




/**************************************************************
Function:       CManaKernel::SelectDataFromDb
Description:    ��Sqlite���ݿ��ȡ�źż�ȫ����������	
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::SelectDataFromDb() 
{
	Byte ucCount           = 0;
	unsigned short usCount = 0;
	int iIndex             = 0;
	int iBoardIndex        = 0;
	int iDetId             = 0;
	GBT_DB::TblPlan         tblPlan;
	GBT_DB::Plan*           pPlan = NULL;
	GBT_DB::TblSchedule     tblSchedule;
	GBT_DB::Schedule*       pSchedule = NULL;
	GBT_DB::TblPattern      tblPattern;
	GBT_DB::Pattern*        pPattern = NULL;
	GBT_DB::TblStagePattern tblStage;
	GBT_DB::StagePattern*   pStage = NULL;
	GBT_DB::TblPhase        tblPhase;
	GBT_DB::Phase*          pPhase = NULL;
	GBT_DB::TblOverlapPhase tblOverlapPhase;
	GBT_DB::OverlapPhase*   pOverlapPhase = NULL;
	GBT_DB::TblChannel      tblChannel;
	GBT_DB::Channel*        pChannel = NULL;
	GBT_DB::TblDetector     tblDetector;
	GBT_DB::Detector*       pDetector = NULL;
	GBT_DB::TblCollision    tblCollision;
	GBT_DB::Collision*      pCollision = NULL;
	GBT_DB::TblSpecFun      tblSpecFun;
	GBT_DB::SpecFun*        pSpecFun = NULL;
	GBT_DB::TblModule       tblModule;
	GBT_DB::Module*         pModule  = NULL;
	GBT_DB::TblDetExtend    tblDetExtend;
	GBT_DB::DetExtend*      pDetExtend = NULL;
	GBT_DB::DetectorPara    sDetPara;

	GBT_DB::TblChannelChk   tblChannelChk;     //ADD:20130801 09 30 
	GBT_DB::ChannelChk*     pChannelChk = NULL;

	GBT_DB::TblPhaseToDirec   tblPhaseToDirec;     //ADD:201310181652
	GBT_DB::PhaseToDirec*     pPhaseToDirec = NULL;
	GBT_DB::TblCntDownDev tblCntDownDev;     //ADD:201312301518
	GBT_DB::CntDownDev*     pCntDownDev = NULL;
	
	(CDbInstance::m_cGbtTscDb).QueryDetPara(sDetPara);               //������ṹ�����,��tbl_system tbl_constant ���в�ѯ
	//CDetector::CreateInstance()->SetStatCycle(sDetPara.ucDataCycle);  //���ݲɼ���������

	ACE_OS::memset(m_pTscConfig , 0 , sizeof(STscConfig) ); //�źŻ����ò���ȫ����ʼ��Ϊ0
	
	
	/*************��λ�뷽�����ñ�****************/  //ADD:201310181706
	(CDbInstance::m_cGbtTscDb).QueryPhaseToDirec(tblPhaseToDirec);
	pPhaseToDirec = tblPhaseToDirec.GetData(ucCount);
	iIndex = 0;
	while(iIndex<ucCount)
	{
		ACE_OS::memcpy(m_pTscConfig->sPhaseToDirec+iIndex,pPhaseToDirec,sizeof(GBT_DB::PhaseToDirec));
		pPhaseToDirec++;
		iIndex++;
	}

	/*************ͨ�����ݼ�����ñ��****************/  //ADD:20130801 15 35
	(CDbInstance::m_cGbtTscDb).QueryChannelChk(tblChannelChk);
	pChannelChk = tblChannelChk.GetData(ucCount);
	iIndex = 0;
	while(iIndex<ucCount)
	{
		ACE_OS::memcpy(m_pTscConfig->sChannelChk+iIndex,pChannelChk,sizeof(GBT_DB::ChannelChk));
		pChannelChk++;
		iIndex++;

	}

    /*************���ȼƻ���****************/
	(CDbInstance::m_cGbtTscDb).QueryPlan(tblPlan);
	pPlan = tblPlan.GetData(ucCount);
	iIndex = 0;
	while ( iIndex < ucCount )
	{
		ACE_OS::memcpy(m_pTscConfig->sTimeGroup+iIndex,pPlan,sizeof(GBT_DB::Plan));
		pPlan++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sTimeGroup+iIndex,0,sizeof(GBT_DB::Plan));


	/*************һ���ʱ�α��ȡ****************/
	ACE_Date_Time tvTime(ACE_OS::gettimeofday());	
	ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb***Year=%d Mon=%d Day=%d WeekDay=%d !\n",__FILE__,__LINE__,tvTime.year(),(Byte)tvTime.month(),(Byte)tvTime.day(),(Byte)tvTime.weekday()));
	Byte ucCurScheduleId = GetScheduleId((Byte)tvTime.month(),(Byte)tvTime.day(),(Byte)tvTime.weekday()); 
	
	if(ucCurScheduleId == 0)
	{
		ucCurScheduleId = 1 ;
		ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** Get CurScheduledId = %d ,Set default 1 !\n",__FILE__,__LINE__,ucCurScheduleId));
	}
	else
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** Get CurScheduledId = %d from Tbl_plan !\n",__FILE__,__LINE__,ucCurScheduleId)); 
	}
	m_pRunData->ucScheduleId = ucCurScheduleId ; //ADD:20130917 1600
	(CDbInstance::m_cGbtTscDb).QuerySchedule(ucCurScheduleId,tblSchedule);
	pSchedule = tblSchedule.GetData(usCount);
	iIndex = 0;
	while ( iIndex < usCount )
	{
		ACE_OS::memcpy(m_pTscConfig->sSchedule+iIndex,pSchedule,sizeof(GBT_DB::Schedule));
		pSchedule++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sSchedule+iIndex,0,sizeof(SSchedule));
	

	/*************��ʱ���������****************/
	(CDbInstance::m_cGbtTscDb).QueryPattern(tblPattern);
	pPattern = tblPattern.GetData(ucCount);
	iIndex = 0;
	while ( iIndex < ucCount )
	{
		ACE_OS::memcpy(m_pTscConfig->sTimePattern+iIndex,pPattern,sizeof(GBT_DB::Pattern));
		pPattern++;
		iIndex++;
	}
	//ACE_OS::memset(&(m_pTscConfig->sTimePattern[iIndex]),0,sizeof(STimePattern));
	ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** Got Patterns  !\n",__FILE__,__LINE__)); 

	/************�׶���ʱ��*************/
	Byte ucScheType = 0x0;
	Byte ucSubSche  = 0x0;
	//Uint ucAllowPhases = 0x0 ;  
	
	ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** Got StagePatterns  %d !\n",__FILE__,__LINE__,usCount)); 
	(CDbInstance::m_cGbtTscDb).QueryStagePattern(tblStage);
	
	pStage = tblStage.GetData(usCount);
	iIndex = 0;
	
	while ( iIndex < usCount )
	{
		ucScheType = pStage->ucStagePatternId;  
		ucSubSche  = pStage->ucStageNo;
		//ucAllowPhases = pStage->usAllowPhase ;
		
		if ( (ucScheType > 0x0) && (ucSubSche > 0x0)) 
		{
			ACE_OS::memcpy(&(m_pTscConfig->sScheduleTime[ucScheType-1][ucSubSche-1]),pStage,sizeof(GBT_DB::StagePattern));
		}
		pStage++;
		iIndex++;
	}

	/********************��λ��*****************/
	(CDbInstance::m_cGbtTscDb).QueryPhase(tblPhase);
	pPhase = tblPhase.GetData(ucCount);
	iIndex = 0;
	while ( iIndex < ucCount )
	{
		ACE_OS::memcpy(m_pTscConfig->sPhase+iIndex,pPhase,sizeof(GBT_DB::Phase));
		pPhase++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sPhase+iIndex,0,sizeof(SPhase));

	/********************������λ��****************/
	(CDbInstance::m_cGbtTscDb).QueryOverlapPhase(tblOverlapPhase);
	pOverlapPhase = tblOverlapPhase.GetData(ucCount);
	iIndex = 0;
	while ( iIndex < ucCount )
	{
		ACE_OS::memcpy(m_pTscConfig->sOverlapPhase+iIndex,pOverlapPhase,sizeof(GBT_DB::OverlapPhase));
		pOverlapPhase++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sOverlapPhase+iIndex,0,sizeof(SOverlapPhase));

	/********************��ͻ��λ��****************/
	(CDbInstance::m_cGbtTscDb).QueryCollision(tblCollision);
	pCollision = tblCollision.GetData(ucCount);
	iIndex = 0;
	while ( iIndex < ucCount )
	{
		if ( pCollision->ucPhaseId > 0 )
		{
			ACE_OS::memcpy(m_pTscConfig->sPhaseConflict+pCollision->ucPhaseId-1,pCollision,sizeof(GBT_DB::Collision));
		}
		pCollision++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sOverlapPhase+iIndex,0,sizeof(GBT_DB::Collision));
	
	/********************ͨ����*******************/
	(CDbInstance::m_cGbtTscDb).QueryChannel(tblChannel);
	pChannel = tblChannel.GetData(ucCount);
	iIndex = 0;
	while ( iIndex < ucCount )
	{
		ACE_OS::memcpy(m_pTscConfig->sChannel+iIndex,pChannel,sizeof(GBT_DB::Channel));
		pChannel++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sChannel+iIndex,0,sizeof(SChannel));
	

	/**********************��Ԫ���� �������� ȫ��ʱ��*******************/
	Byte ucStartFlash     = 0;
	Byte ucStartAllRed    = 0;
	Byte ucRemoteCtrlFlag = 0; 
	Byte ucFlashFreq      = 0;
	(CDbInstance::m_cGbtTscDb).GetUnitCtrlPara(ucStartFlash, ucStartAllRed, ucRemoteCtrlFlag, ucFlashFreq);
	m_pTscConfig->sUnit.ucStartFlashTime  = ucStartFlash;
	m_pTscConfig->sUnit.ucStartAllRedTime = ucStartAllRed;
	
	/**********************����ģʽ ������׼����*******************/
	(CDbInstance::m_cGbtTscDb).GetDegradeCfg(m_pTscConfig->DegradeMode, m_pTscConfig->DegradePattern);

	/**********************��������ʱ�� Э����λ��********************/
	if ( false == (CDbInstance::m_cGbtTscDb).GetGlobalCycle(m_ucUtcsComCycle) )
	{
		m_ucUtcsComCycle = 0; 
	}
	if ( false == (CDbInstance::m_cGbtTscDb).GetCoorPhaseOffset(m_ucUtscOffset) )
	{
		m_ucUtscOffset   = 0;
	}

	
	/*********************���������***************************/
	(CDbInstance::m_cGbtTscDb).QueryDetector(tblDetector);
	pDetector = tblDetector.GetData(ucCount);
	iIndex = 0;
	ACE_OS::memset(m_pTscConfig->sDetector , 0 , MAX_DETECTOR*sizeof(GBT_DB::Detector) );
	ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** Get Detector num from DB = %d \n",__FILE__,__LINE__,ucCount));
	while ( iIndex < ucCount )
	{
		if ( pDetector->ucDetectorId > 0 )
		{
			ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** pDetector->ucDetectorId = %d \n",__FILE__,__LINE__,pDetector->ucDetectorId));
			ACE_OS::memcpy(m_pTscConfig->sDetector+(pDetector->ucDetectorId-1) , pDetector , sizeof(GBT_DB::Detector));
			//CDetector::CreateInstance()->m_ucDetError[pDetector->ucDetectorId-1] = DET_NORMAL ; //�����ڼ����ʱ��Ĭ�Ϲ���״̬����
		}		
		pDetector++;
		iIndex++;
	}
	//ACE_OS::memset(m_pTscConfig->sDetector+iIndex , 0 , sizeof(GBT_DB::Detector));
	SetDetectorPhase();  //���������λ��ϵ
	
	/*********************���⹦�ܿ�����***********************/ //�����źŻ����⹦�ܳ�ʼ������
	(CDbInstance::m_cGbtTscDb).QuerySpecFun(tblSpecFun);
	pSpecFun = tblSpecFun.GetData(ucCount);
	iIndex   = 0;
	//ACE_OS::memset(m_pTscConfig->sSpecFun , 0 , FUN_COUNT*sizeof(GBT_DB::SpecFun) );
	while ( iIndex < ucCount )
	{
		if ( pSpecFun->ucFunType > 0 )
		{
			ACE_OS::memcpy(m_pTscConfig->sSpecFun+(pSpecFun->ucFunType-1) ,	pSpecFun , sizeof(GBT_DB::SpecFun));
		}
		pSpecFun++;
		iIndex++;
	}
	
	/********************��ȡ���õĵƿذ����********************/
	//GetUseLampBoard(bLampBoardExit);
	//CLamp::CreateInstance()->GetLampBoardExit(bLampBoardExit);

	/********************��ȡģ��� ���������������***********/
	//ACE_OS::memset(m_pTscConfig->iDetCfg , 0 , MAX_DET_BOARD*sizeof(int));
	(CDbInstance::m_cGbtTscDb).QueryModule(tblModule);
	pModule = tblModule.GetData(ucCount);
	iIndex = 0;	
	while ( iIndex < ucCount)
	{			
		if ( strstr(pModule->strDevNode.GetData(),"DET") != NULL )
		{
			sscanf(pModule->strDevNode.GetData(),"DET-%d-%d",&iBoardIndex,&iDetId);			
			m_pTscConfig->iDetCfg[iBoardIndex] = iDetId;
			ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** m_pTscConfig->iDetCfg[%d] = %d\n",__FILE__,__LINE__,iBoardIndex,iDetId));
		}
		/*
		if(pModule->ucModuleId>0)
		{
			if(ACE_OS::strcmp((pModule->strDevNode.GetData() ,"36")==0)			
				m_pTscConfig->iDetCfg[0] = 1;
			else if(ACE_OS::strcmp((pModule->strDevNode.GetData() ,"37")==0)				
				m_pTscConfig->iDetCfg[1] = 17;
			if(ACE_OS::strcmp((pModule->strDevNode.GetData() ,"41")==0)			
				m_pTscConfig->iDetCfg[2] = 33;
			else if(ACE_OS::strcmp((pModule->strDevNode.GetData() ,"42")==0)				
				m_pTscConfig->iDetCfg[3] = 65;
		}	
		*/
		pModule++;	
		iIndex++;
	}

	CDetector::CreateInstance()->SelectDetectorBoardCfg(m_pTscConfig->iDetCfg);
	
	 
	/********************��ȡ�������չ��**********************/
	(CDbInstance::m_cGbtTscDb).QueryDetExtend(tblDetExtend);
	pDetExtend = tblDetExtend.GetData(ucCount);
	iIndex     = 0;

	while ( iIndex < ucCount )
	{
		if ( pDetExtend->ucId > 0 )
		{
			ACE_OS::memcpy(m_pTscConfig->sDetExtend+pDetExtend->ucId-1,pDetExtend,sizeof(GBT_DB::DetExtend));
		}
		pDetExtend++;
		iIndex++;
	}

	//CAdaptive::CreateInstance()->SynPara();  //����Ӧ���Ʋ���
	(CDbInstance::m_cGbtTscDb).QueryCntDownDev(tblCntDownDev);
	pCntDownDev = tblCntDownDev.GetData(ucCount);
	iIndex= 0 ;	
	while(iIndex<ucCount)
	{
		ACE_OS::memcpy(m_pTscConfig->sCntDownDev+iIndex,pCntDownDev,sizeof(GBT_DB::CntDownDev));
		//ACE_DEBUG((LM_DEBUG,"%s:%d***SelectDataFromDb*** InitDb ucDevId =%d ucPhase =%d ucOverPhase=%d ,cnttime=%d \n",__FILE__,__LINE__,m_pTscConfig->sCntDownDev[iIndex].ucDevId,
			//m_pTscConfig->sCntDownDev[iIndex].usPhase ,m_pTscConfig->sCntDownDev[iIndex].ucOverlapPhase,(m_pTscConfig->sCntDownDev[iIndex].ucMode>>3)&0xf));
		pCntDownDev++;
		iIndex++;
	} 
	if ( m_pTscConfig->sSpecFun[FUN_COUNT_DOWN].ucValue != COUNTDOWN_STUDY)  //����ʱ�豸����
	{
		CGaCountDown::CreateInstance()->GaUpdateCntDownCfg();
	}
    ACE_TString  stringGbtype;
     Configure::CreateInstance()->GetString("COMMUNICATION","standard",stringGbtype);
    if(ACE_OS::strcmp("GBT20999",stringGbtype.c_str())==0x0)
      m_pTscConfig->cGbType = GBT20999;
      else
      m_pTscConfig->cGbType = GB25280;
    ACE_OS::printf("%s:%d Gbprotol type %s \r\n",__FILE__,__LINE__,stringGbtype.c_str());
	
}


/**************************************************************
Function:       CManaKernel::SetDetectorPhase
Description:    ���ü��������λ�Ķ�Ӧ��ϵ	
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::SetDetectorPhase()
{
	//bool bIsOverlap = false;
	int iIndex      = 0;
	int iRoadwayCnt = 0;
	Byte ucDetId    = 0;
	Byte ucPhaseId  = 0;

	ACE_OS::memset(m_sPhaseDet , 0 , /*(*/MAX_PHASE/*+MAX_OVERLAP_PHASE)*/*sizeof(SPhaseDetector));

	//m_bPhaseDetCfg = false;

	while ( iIndex < MAX_DETECTOR )
	{
		if ( 0 == m_pTscConfig->sDetector[iIndex].ucDetectorId )
		{
			iIndex++;
			continue;
		}

		//bIsOverlap = ( m_pTscConfig->sDetector[iIndex].ucOptionFlag >> 6 ) & 1;
		ucDetId    = m_pTscConfig->sDetector[iIndex].ucDetectorId;
		ucPhaseId  = m_pTscConfig->sDetector[iIndex].ucPhaseId;
	
		if ( ucPhaseId > 0 )
		{		

			iRoadwayCnt = m_sPhaseDet[ucPhaseId-1].iRoadwayCnt;
			m_sPhaseDet[ucPhaseId-1].iDetectorId[iRoadwayCnt] = ucDetId;
			m_sPhaseDet[ucPhaseId-1].iRoadwayCnt++;
		}

		iIndex++;
	}
}



/**************************************************************
Function:       CManaKernel::UpdateConfig
Description:    �����źŻ���̬���ò���,�����ݿ���ظ��������Ϣ��
				�ڴ���,�����ݿ�õ�����ʱ����һ�����ڽ�������øú���
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::UpdateConfig()
{	
	
	ACE_DEBUG((LM_DEBUG,"%s:%d***UpdateConfig-> Reload database parameters !\n",__FILE__,__LINE__));
	SelectDataFromDb();
}


/**************************************************************
Function:       CManaKernel::DecTime
Description:    1sִ��һ�Σ�����ʱ��++ ��Ҫ�������ж��Ƿ�����һ
				��ʱ�䣬�ǵĻ��ͷ���TSC��Ϣ���������ǵĻ��򽫵�ǰ��
				ʣ��ʱ�����һ��
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::DecTime()
{
	m_bAddTimeCount = true;
	/*   �������Ҫ��Ҫ�ڿ���ȫ�������½��е��ݼ�� <<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	if(m_bFinishBoot == false && m_pRunData->uiWorkStatus == ALLRED)
	{
		static Byte iChkTime = 0 ;
		iChkTime ++ ;
		if(CLampBoard::CreateInstance()->IsChkLight == false && m_pRunData->bIsChkLght == true && iChkTime == 3)
		{		
			iChkTime = 0 ;
			ACE_DEBUG((LM_DEBUG,"%s:%d  Begin LampCheck!\n",__FILE__,__LINE__));		
			CLampBoard::CreateInstance()->IsChkLight = true ;   
		}

	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	
	//if(m_pRunData->uiWorkStatus == STANDARD && m_pRunData->uiCtrl == CTRL_VEHACTUATED )
	//ACE_DEBUG((LM_DEBUG,"%s:%d ctrl:%d  ucElapseTime:%02d,ucStepTime:%02d m_iMinStepTime:%02d\n",
		//	__FILE__,__LINE__,m_pRunData->uiCtrl , m_pRunData->ucElapseTime , m_pRunData->ucStepTime,m_iMinStepTime	));	

	m_pRunData->ucElapseTime++;
	
	//ACE_DEBUG((LM_DEBUG,"%s:%d	StepNo=%d Steptime=%d elapsetime=%d !\n",__FILE__,__LINE__,m_pRunData->ucStepNo,
	//	m_pRunData->ucStepTime,m_pRunData->ucElapseTime));		
	if ( m_bWaitStandard )  //�źŻ������ڻ���-->ȫ��  �ֶ��ֽ���
	{
		if ( CTRL_PANEL == m_pRunData->uiCtrl )  //����ֶ�����
		{
			return;
		}
	}
	else if ( !m_bWaitStandard )
	{
		if ( CTRL_MANUAL == m_pRunData->uiCtrl )  //�ֶ�����
		{
			return;
		}
		if ( ( CTRL_PANEL == m_pRunData->uiCtrl ) && ( (SIGNALOFF == m_pRunData->uiWorkStatus ) || (ALLRED == m_pRunData->uiWorkStatus) 
			|| (FLASH == m_pRunData->uiWorkStatus ) ) )  //���صơ�ȫ�졢����
		{
			return;
		}
		if ( (CTRL_PANEL == m_pRunData->uiCtrl) && (0 == m_iTimePatternId ))//|| (250 == m_iTimePatternId)))  //�������ҷ�ָ����ʱ����
		{
			//ACE_DEBUG((LM_DEBUG,"\n%s:%d CTRL_PANEL and m_iTimePatternId=0 or 250\n",__FILE__,__LINE__));
			return;
		}
		if ( CTRL_SECOND_PRIORITY== m_pRunData->uiCtrl )  //���߰��Ӧ
		{
			if(CManaKernel::CreateInstance()->bSecondPriority == false && m_pRunData->ucElapseTime < m_pRunData->ucCycle*3)
			{
			// ACE_OS::printf("%s:%d CTRL_SECOND_PRIORITY bSecondPriorty=%s ucElapseTime=%d\r\n",__FILE__,__LINE__,(CreateInstance()->bSecondPriority == false)?"false":"true",m_pRunData->ucElapseTime);
			 return ;
			}
			else
			{				
				if(CManaKernel::CreateInstance()->bSecondPriority == false)
					CManaKernel::CreateInstance()->bSecondPriority = true ; //3�����ڳ�ʱ������������ڵ�ɫ����
				//ACE_OS::printf("%s:%d CTRL_SECOND_PRIORITY bSecondPriorty=%s ucElapseTime=%d\r\n",__FILE__,__LINE__,(CreateInstance()->bSecondPriority == false)?"false":"true",m_pRunData->ucElapseTime);
			}
		}	
		
	}
	if ( m_pRunData->ucElapseTime >= m_pRunData->ucStepTime)
	{			
		SThreadMsg sMsg;
		sMsg.ulType = TSC_MSG_NEXT_STEP;
		sMsg.pDataBuf = NULL;
		CTscMsgQueue::CreateInstance()->SendMessage(&sMsg,sizeof(sMsg));
		return ;
	}		
	if(m_pRunData->uiCtrl == CTRL_PREANALYSIS  ||m_pRunData->uiCtrl == CTRL_ACTIVATE )//���ȷ�������
	{
		bool bStageFirstStep = false ;
		int NextStage = (m_pRunData->ucStageNo+1 == m_pRunData->ucStageCount)?0x0:(m_pRunData->ucStageNo+1) ;
		StepToStage(m_pRunData->ucStepNo,&bStageFirstStep);
		if ( bStageFirstStep && IsLongStep(m_pRunData->ucStepNo) )   //������Ϊ�׶εĵ�һ�� �������ǵ�ǰ���̵���
		{		
			if(m_pRunData->ucStepTime == m_pRunData->ucElapseTime+1) //�̵����һ��
			{				
				//ACE_OS::printf("%s:%d CTRL_PREANALYSIS CurrentStageNo=%d ,CurrentStepNo=%d CurrentStepTime=%d\r\n",__FILE__,__LINE__,m_pRunData->ucStageNo,m_pRunData->ucStepNo,m_pRunData->ucStepTime);
				Uint32 CurrentllowPhase = m_pRunData->sScheduleTime[m_pRunData->ucStageNo].usAllowPhase ;
					Byte iIndex =0x0 ;
					Byte iDetCnt =0x0 ;
					while(iIndex < MAX_PHASE )
					{
						if ( CurrentllowPhase>>iIndex & 0x1 )
						{
							iDetCnt = m_sPhaseDet[iIndex].iRoadwayCnt;				
							Byte iDetIndex = 0x0;
							while ( iDetIndex < iDetCnt )
							{
								// Byte iDetId = m_sPhaseDet[iIndex].iDetectorId[iDetIndex];								
								//ACE_DEBUG((LM_DEBUG,"%s:%d DetectoPhaseID=%d ,DetecId=%d\n"	,__FILE__,__LINE__,iIndex+1,m_sPhaseDet[iIndex].iDetectorId[iDetIndex]));
								//ACE_OS::printf("%s:%d DetectorId =%d cars from %d to Zero \r\n",__FILE__,__LINE__,m_sPhaseDet[iIndex].iDetectorId[iDetIndex],m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]);
								
								m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex] = 0x0 ; //�Ѷ�Ӧ������λ������Ŷӳ�������
								iDetIndex++;
							}
						}
						iIndex++;
					}
				if((m_pRunData->sScheduleTime[NextStage].ucOption)&0x2 && m_pRunData->uiCtrl == CTRL_PREANALYSIS)
				{					
					//ACE_OS::printf("%s:%d CTRL_PREANALYSIS NextStageNo=%d ,Option=%d\r\n",__FILE__,__LINE__,NextStage,m_pRunData->sScheduleTime[NextStage].ucOption);
					Uint32 NextAllowPhase = m_pRunData->sScheduleTime[NextStage].usAllowPhase ;
					Byte iIndex =0x0 ;
					Byte iDetCnt =0x0 ;
					bool bHaveCar = false ;
					while(iIndex < MAX_PHASE )
					{
						if ( NextAllowPhase>>iIndex & 1 )
						{
							iDetCnt = m_sPhaseDet[iIndex].iRoadwayCnt;

							Byte iDetIndex = 0;
							while ( iDetIndex < iDetCnt )
							{
								// Byte iDetId = m_sPhaseDet[iIndex].iDetectorId[iDetIndex];
								
								//ACE_OS::printf("%s:%d CTRL_PREANALYSIS PhaseId=%d DetecId=%d CarNumer=%d \r\n",__FILE__,__LINE__,iIndex+1,m_sPhaseDet[iIndex].iDetectorId[iDetIndex],m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]);
								 if(m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex] !=0x0)
								 {								 	
									//ACE_OS::printf("%s:%d CTRL_PREANALYSIS PhaseId=%d DetecId=%d CarNumer=%d \r\n",__FILE__,__LINE__,iIndex+1,m_sPhaseDet[iIndex].iDetectorId[iDetIndex],m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]);
									bHaveCar = true ;
									break ;
								 }														
								iDetIndex++;
							}
						}
						if(bHaveCar)
							break;
						iIndex++;
					}
						if(!bHaveCar)
						{						
							//��һ�׶εĲ������ȶ�Ϊ0 						
							Byte NextStageStepNum= m_pRunData->ucStageIncludeSteps[NextStage];
						//	Byte NextStageStepNo = m_pRunData->ucStepNo+ m_pRunData->ucStageIncludeSteps[m_pRunData->ucStageNo];
							Byte NextStageStepNo=StageToStep(NextStage);							
							if(m_pRunData->ucStageCount >=NextStage+1)
							{
								m_pRunData->sScheduleTime[NextStage+1].usAllowPhase = m_pRunData->sScheduleTime[NextStage].usAllowPhase|m_pRunData->sScheduleTime[NextStage+1].usAllowPhase;
										
								//SetCycleStepInfo(0); //���ı䲽������ֻ�Ƕ�̬�ı䲽����λ
								Byte NextStageStepNo1=StageToStep(NextStage+1);
								SetStepInfoWithStage(NextStage+1,&NextStageStepNo1,&(m_pRunData->sScheduleTime[NextStage+1]));// ucStageIndex �ӽ׶κ�
								ACE_OS::printf("%s:%d CTRL_PREANALYSIS NextStage+1=%d usAllowPhase=%d\r\n",__FILE__,__LINE__,NextStage+1,m_pRunData->sScheduleTime[NextStage+1].usAllowPhase);
								for(Byte index =0x0 ; index<NextStageStepNum ;index++)
								{						
									m_pRunData->sStageStepInfo[NextStageStepNo+index].ucStepLen = 0x0 ; 							
									//ACE_OS::printf("%s:%d CTRL_PREANALYSIS StepNo=%d StepTime==0 NextStageStepNum=%d\r\n",__FILE__,__LINE__,NextStageStepNo+index,NextStageStepNum);
								}
							}							
							m_pRunData->ucCycle = 0;
							for ( int i=0; i<m_pRunData->ucStepNum; i++ )
							{
								m_pRunData->ucCycle += m_pRunData->sStageStepInfo[i].ucStepLen;
							}	
							return ;
						}					
						
					}
	
				Byte NextStageStepNo = StageToStep(NextStage);
				if(m_pRunData->sStageStepInfo[NextStageStepNo+1].ucStepLen ==0x0)	
				{
					m_pRunData->sStageStepInfo[NextStageStepNo+1].ucStepLen =0x2;
					m_pRunData->sStageStepInfo[NextStageStepNo+2].ucStepLen =0x3;
				}

				if(m_pRunData->uiCtrl == CTRL_PREANALYSIS)
				{
				int TempMin = GetMaxStageMinGreen(NextStageStepNo) ;
				Byte Max2= GetMaxGreen2(NextStageStepNo);
				Byte NextStageCars = GetStageMaxDetectorCars(m_pRunData->sScheduleTime[NextStage].usAllowPhase) ;
				if(NextStageCars >0x7)
				{
					srand((int)time(NULL));					
					NextStageCars = rand()%8 + 0x5 ;
				}
				int NextCycleMaxTime = TempMin+NextStageCars*m_ucStageDynamicAddGreen[NextStage];
				int NextStageMinTime = TempMin+(NextStageCars-3)*m_ucStageDynamicAddGreen[NextStage];
				
				m_ucStageDynamicMinGreen[NextStage] =(TempMin >NextStageMinTime)? TempMin:NextStageMinTime; //�¸��׶ζ�̬��С��				
				m_ucStageDynamicMaxGreen[NextStage]=NextCycleMaxTime>Max2?Max2:NextCycleMaxTime ;	//���׶��¸����ڶ�̬�����
				m_ucStageDynamicMaxGreen[NextStage] = (m_ucStageDynamicMaxGreen[NextStage]==TempMin)?(TempMin+8):m_ucStageDynamicMaxGreen[NextStage];
				//m_ucStageDynamicMinGreen[NextStage] = m_ucStageDynamicMinGreen[NextStage]>Max2?
				
				ACE_OS::printf("%s:%d CTRL_PREANALYSIS NextStageCars =%d NextStage=%d StandMinGreen=%d NextStage m_ucStageDynamicMinGreen==%d  NextStageMaxTime=%d m_ucNextStageDynamicAddGreen=%d\r\n",__FILE__,__LINE__,NextStageCars,NextStage,TempMin,m_ucStageDynamicMinGreen[NextStage],
				m_ucStageDynamicMaxGreen[NextStage],m_ucStageDynamicAddGreen[NextStage] );
				m_pRunData->sStageStepInfo[NextStageStepNo].ucStepLen = (m_ucStageDynamicMinGreen[NextStage]>m_ucStageDynamicMaxGreen[NextStage])?(m_ucStageDynamicMaxGreen[NextStage]-9):m_ucStageDynamicMinGreen[NextStage];
				//ACE_OS::printf("%s:%d CTRL_PREANALYSIS NextStageCars =%d NextStage=%d StandMinGreen=%d NextStage m_ucStageDynamicMinGreen==%d  NextStageMaxTime=%d m_ucNextStageDynamicAddGreen=%d\r\n",__FILE__,__LINE__,NextStageCars,NextStage,TempMin,m_ucStageDynamicMinGreen[NextStage],
				//m_ucStageDynamicMaxGreen[NextStage],m_ucStageDynamicAddGreen[NextStage] );
				}	
			}

		  }
		}
		//pPreAnalysis->HandPreAnalysis();
	}




/**************************************************************
Function:       CManaKernel::GoNextStep
Description:    һ���������꣬�л�����һ������
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::GoNextStep()
{
	bool bStageFirstStep      = false;
	Byte ucStageNo            = 0;
	static Byte ucLastStageNo = 255;

	ACE_Guard<ACE_Thread_Mutex>  guard(m_mutexRunData);
	
	m_pRunData->ucStepNo++;
	while(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen <=0x0)
	{
		
	//ACE_DEBUG((LM_DEBUG,"%s:%d StepNo=%d StepTime=%d\n" ,__FILE__,__LINE__,m_pRunData->ucStepNo,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen));
		m_pRunData->ucStepNo++;
	}
	
	//MOD:201309251030 �޸���λ��ͻ���λ�ã�����һ�����Ƶ���һ�������м��
	if ( InConflictPhase() )  //��λ��ͻ�� ����ͬʱ�����̵�
	{			
		SndMsgLog(LOG_TYPE_GREEN_CONFIG,3,0,0,0); // 3��ʾ��λ��ͻ ADD��201309251130
		DealGreenConflict(1);
		//ACE_DEBUG((LM_DEBUG,"%s:%d InConflictPhase !\n",__FILE__,__LINE__));
		//CFlashMac::CreateInstance()->FlashForceStart(2) ; //�̳�ͻ��ǿ�ƻ���˸		

		m_pRunData->flashType = CTRLBOARD_FLASH_PHASECONFLIC;
		bDegrade = true ;
		return ;
	}
	if ( m_pRunData->ucStepNo >= m_pRunData->ucStepNum )
	{
		SThreadMsg sMsg;
		sMsg.ulType = TSC_MSG_OVER_CYCLE;
		sMsg.pDataBuf = NULL;
		CTscMsgQueue::CreateInstance()->SendMessage(&sMsg,sizeof(sMsg));
		return;
	}
	else if ( m_pRunData->ucStepNo < m_pRunData->ucStepNum )
	{
		//�����еĲ�������ټ�����һ��
		switch ( m_pRunData->uiCtrl )
		{
			case CTRL_WIRELESS:
				m_pRunData->ucStepTime = CWirelessCoord::CreateInstance()->GetStepLength(m_pRunData->ucStepNo);
				//m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				m_pRunData->ucRunTime  = m_pRunData->ucStepTime; //ADD:20141114
				break;

			case CTRL_UTCS:
				if ( (m_ucUtcsComCycle != 0) || (m_ucUtscOffset != 0) )
				{   
					m_pRunData->ucStepTime = CWirelessCoord::CreateInstance()->GetStepLength(m_pRunData->ucStepNo);
					
					//ACE_DEBUG((LM_DEBUG,"%s:%d After Utsc StepNo =%d StepTime =%d !\n",__FILE__,__LINE__,m_pRunData->ucStepNo,m_pRunData->ucStepTime ));
					if ( 0 == m_pRunData->ucStepTime )
					{   
						m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
						
						//ACE_DEBUG((LM_DEBUG,"%s:%d If after Utsc StepNo =%d StepTime=0 ,Set StepTime=%d !\n",__FILE__,__LINE__,m_pRunData->ucStepNo,m_pRunData->ucStepTime ));
					}
				
				}
				else
				{	
					 m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				}
				m_pRunData->ucRunTime  = m_pRunData->ucStepTime; //ADD:201411114
				break;
		

			case CTRL_VEHACTUATED:   //��Ӧ����״̬�²����ı�
			case CTRL_MAIN_PRIORITY: 
			case CTRL_SECOND_PRIORITY: 
				StepToStage(m_pRunData->ucStepNo,&bStageFirstStep);
				if ( bStageFirstStep && IsLongStep(m_pRunData->ucStepNo) )   //������Ϊ�׶εĵ�һ�� �������ǵ�ǰ���̵���
				{
					m_iMinStepTime = GetCurStepMinGreen(m_pRunData->ucStepNo , &m_iMaxStepTime , &m_iAdjustTime  );
					m_bVehile = true;
				}
				else
				{
					m_bVehile = false;
				}
				m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				break;
			case CTRL_PREANALYSIS: //���������λ��Ӧ������Ŷӳ���
			case CTRL_ACTIVATE:
				StepToStage(m_pRunData->ucStepNo,&bStageFirstStep);
				if ( bStageFirstStep && IsLongStep(m_pRunData->ucStepNo) )	 //������Ϊ�׶εĵ�һ�� �������ǵ�ǰ���̵���
				{					
					m_iMinStepTime = GetCurStepMinGreen(m_pRunData->ucStepNo , &m_iMaxStepTime , &m_iAdjustTime  );
					m_bVehile = true;
					
											
				}
				else
				{
					m_bVehile = false;

				}
				
				if ( bStageFirstStep && IsLongStep(m_pRunData->ucStepNo))	 //������Ϊ�׶εĵ�һ�� �������ǵ�ǰ���̵���
				{
					m_pRunData->ucStepTime = m_ucStageDynamicMinGreen[m_pRunData->ucStageNo];
				}
				else
				{					
					m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
					
					//ACE_DEBUG((LM_DEBUG,"%s:%d CTRL_PREANALYSIS  StepNo=%d StepTime=%d\n" ,__FILE__,__LINE__,m_pRunData->ucStepNo,m_pRunData->ucStepTime));
				}
				
				break ;

			default:
				m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				break;
		}
		m_pRunData->ucElapseTime = 0;
		//m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
		m_pRunData->ucStageNo = StepToStage(m_pRunData->ucStepNo,NULL); //���ݲ����Ż�ȡ�׶κ�

		CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn
			,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
	}	
	
	if ( ucLastStageNo != m_pRunData->ucStageNo )
	{
		m_ucAddTimeCnt = 0;
		ucLastStageNo  = m_pRunData->ucStageNo;
	}	
	//CwpmGetCntDownSecStep();
	CTscKernelGb25280::CreateInstance()->SelfReportLampGroupColor();  //���͵�ɫ�ı���Ϣ
	StepToStage(m_pRunData->ucStepNo,&bStageFirstStep);
	if ( bStageFirstStep && IsLongStep(m_pRunData->ucStepNo) )	 //������Ϊ�׶εĵ�һ�� �������ǵ�ǰ���̵���
	{	
	 //   ACE_OS::printf("\r\n****%s:%d Self report workstatus!\r\n",__FILE__,__LINE__);
		CTscKernelGb25280::CreateInstance()->SelfReportWorkStatus();  //���͹���״̬�ı���Ϣ																
	}
}


/**************************************************************
Function:       CManaKernel::OverCycle
Description:    һ����������
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::OverCycle()
{
	bool bStageFirstStep      = false;
	Byte ucStageNo            = 0;
	int iCurTimePatternId     = 1;
	int iStepLength[MAX_STEP] = {0};
	

	if ( m_pRunData->bNeedUpdate )
	{
		UpdateConfig();
	}
	
	 //һ�����ڼ��һ�μ��������״̬���ڸ�Ӧ������������
	CDetector::CreateInstance()->SearchAllStatus(false,true);
	if ( FLASH == m_pRunData->uiWorkStatus )  //������ ����ȫ��
	{	
		if ( m_bSpeStatusTblSchedule )  //ʱ�α���Ļ���״̬
		{
			m_pRunData->uiOldWorkStatus = FLASH;
			m_pRunData->uiWorkStatus    = STANDARD;
			m_bVirtualStandard          = true;
			
			ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** m_bSpeStatusTblSchedule �� m_bVirtualStandard == true chaned from FLASH to STANDARD !\n",__FILE__,__LINE__));
		}
		else
		{	
			m_pRunData->uiWorkStatus = ALLRED;
			
			ACE_DEBUG((LM_DEBUG,"\n%s:%d***OverCycle*** m_bSpeStatusTblSchedule == false chaned from FLASH to RED !\n",__FILE__,__LINE__));
		}
		ResetRunData(0);		
	}
	else
	{
		if ( m_bSpeStatusTblSchedule )  //ʱ�α���Ĺصƻ���ȫ��
		{
			m_pRunData->uiOldWorkStatus = m_pRunData->uiWorkStatus;
			m_pRunData->uiWorkStatus    = STANDARD;
			m_bVirtualStandard         = true;
			
			ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** m_bSpeStatusTblSchedule �� m_bVirtualStandard == true chaned from %d to STANDARD !\n",__FILE__,__LINE__,m_pRunData->uiWorkStatus));
		}

		if ( ALLRED == m_pRunData->uiWorkStatus )  //ȫ���� �����׼
		{
			
			m_bWaitStandard             = false;
			m_pRunData->uiWorkStatus    = STANDARD;
			m_bFinishBoot               = true;
			ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** m_bFinishBoot == true chaned from ALLRED to STANDARD !\n",__FILE__,__LINE__,m_pRunData->uiWorkStatus));
			Ulong mRestart = 0 ;
		(CDbInstance::m_cGbtTscDb).GetSystemData("ucDownloadFlag",mRestart);

		//���ڵ��ݼ�⣬����ϵͳ����й����������Ҫ���½�������
		if(mRestart >0)
		{
			#ifdef LINUX
			usleep(100000);
			#endif
			CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_OTHER,2,mRestart,0,0);	
			(CDbInstance::m_cGbtTscDb).SetSystemData("ucDownloadFlag",0);  // ��������
		}  			//
			
	}

	
		if ( (m_pRunData->uiWorkStatus!=FLASH) && (m_pRunData->uiWorkStatus!=ALLRED) && (m_pRunData->uiWorkStatus!=SIGNALOFF) )
		{
			/*----->�������к���֤
			bValidSoftWare = VaildSN();
			if(bValidSoftWare == false)
			{
				//pWorkParaManager->SndMsgLog(LOG_TYPE_CAN,0,0,0,0);
				CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_SYSTEMCONTROL,255);
				return ;
			}	*///---------<�������к���֤/
			
			switch ( m_pRunData->uiCtrl )
			{
			case CTRL_MANUAL:
				break;
			case CTRL_UTCS:
				ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** UtcsHeartBeat= %d ucCycle =%d\n",__FILE__,__LINE__,m_pRunData->uiUtcsHeartBeat,m_pRunData->ucCycle));
				if ( (Byte)m_pRunData->uiUtcsHeartBeat >= m_pRunData->ucCycle )  //1������û���յ���������ָ��
				{					
						bUTS = false ;
						bDegrade = true ;
						m_pRunData->uiOldCtrl = CTRL_UTCS;
						
						if(m_pTscConfig->DegradeMode > 0)
						{
							if(m_pTscConfig->DegradeMode >= 5)
							{
								m_pRunData->uiCtrl    = CTRL_SCHEDULE;	
								SndMsgLog(LOG_TYPE_OTHER,1,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl,1); //��־��¼���Ʒ�ʽ�л� ADD?201311041530													
								ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** oldctrl = %d newctrl =%d\n"	,__FILE__,__LINE__,CTRL_UTCS,CTRL_SCHEDULE));

								
								if(m_pTscConfig->DegradePattern>0)
								{
									m_iTimePatternId = m_pTscConfig->DegradePattern;
									bTmpPattern = true ;
								}
							}
							else
							{
								CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_CONTROL,m_pTscConfig->DegradeMode);
								if(m_pTscConfig->DegradeMode == 2) //falsh degrade								
									m_pRunData->flashType = CTRLBOARD_FLASH_DOWNGRADE;
							}
							m_pRunData->bNeedUpdate = true;
							
						}
						else
						{
							m_pRunData->uiOldCtrl =CTRL_UTCS;
							m_pRunData->uiCtrl    = CTRL_SCHEDULE;
							m_pRunData->bNeedUpdate = true;
							SndMsgLog(LOG_TYPE_OTHER,1,CTRL_UTCS,CTRL_SCHEDULE,1); //��־��¼���Ʒ�ʽ�л� ADD?201311041530
							ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** oldctrl = %d newctrl =%d \n"	,__FILE__,__LINE__,CTRL_UTCS,CTRL_SCHEDULE));
						}
						CWirelessCoord::CreateInstance()->SetDegrade() ;   //������������  ADD��201311121430
						CMainBoardLed::CreateInstance()->DoModeLed(true,true);
						ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** too long no utcs command��\n"	,__FILE__,__LINE__));
				}
				break;
			case CTRL_VEHACTUATED:
			case CTRL_MAIN_PRIORITY:   
			case CTRL_SECOND_PRIORITY:
			case CTRL_PREANALYSIS :
				{
					bool bHaveDetectors = CDetector::CreateInstance()->HaveDetBoard();
					if ( !bHaveDetectors||(CTRL_PREANALYSIS != m_pRunData->uiScheduleCtrl&&CTRL_VEHACTUATED != m_pRunData->uiScheduleCtrl&&CTRL_MAIN_PRIORITY!= m_pRunData->uiScheduleCtrl && CTRL_SECOND_PRIORITY!= m_pRunData->uiScheduleCtrl))/*|| !m_bPhaseDetCfg*/  /*�����ڼ������*/
					{
						ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** bHaveDetectors=%d , or uiCtrl != VEHACTUATED\n",__FILE__,__LINE__,bHaveDetectors));
						m_pRunData->uiOldCtrl = m_pRunData->uiCtrl;					
						m_pRunData->uiCtrl    = CTRL_SCHEDULE;					
						m_pRunData->bNeedUpdate = true;	
						bDegrade = true ;	
						CMainBoardLed::CreateInstance()->DoModeLed(true,true);				
						SndMsgLog(LOG_TYPE_OTHER,1,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl,1); //��־��¼���Ʒ�ʽ�л� ADD?201311041530
						ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** oldctrl = %d newctrl = %d\n"	,__FILE__,__LINE__,m_pRunData->uiCtrl,CTRL_SCHEDULE));
						break;
					}
				}
				break;
			case CTRL_ACTIVATE:
				break;

			case CTRL_WIRELESS:
				iCurTimePatternId = m_pRunData->ucTimePatternId > 0 ? m_pRunData->ucTimePatternId : 1;
				if ( CTRL_WIRELESS != m_pRunData->uiScheduleCtrl
					|| m_pTscConfig->sTimePattern[iCurTimePatternId-1].ucPhaseOffset > 99 
					|| CManaKernel::CreateInstance()->m_pTscConfig->sSpecFun[FUN_GPS].ucValue == 0 )
				{
					m_pRunData->uiOldCtrl = m_pRunData->uiCtrl;
					m_pRunData->uiCtrl    = CTRL_SCHEDULE;
					ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** oldctrl = %d newctrl = %d\n"	,__FILE__,__LINE__,CTRL_WIRELESS,CTRL_SCHEDULE));
					SndMsgLog(LOG_TYPE_OTHER,1,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl,1); //��־��¼���Ʒ�ʽ�л� ADD?201311041530
				}
				break;
			default:
				iCurTimePatternId =(m_pRunData->ucTimePatternId > 0 )? m_pRunData->ucTimePatternId : 1;
			
				if ( CTRL_WIRELESS == m_pRunData->uiScheduleCtrl && m_pTscConfig->sTimePattern[iCurTimePatternId-1].ucPhaseOffset < 99 
					&& CManaKernel::CreateInstance()->m_pTscConfig->sSpecFun[FUN_GPS].ucValue != 0 )  //������λ��ҿ���gps
				{
					m_pRunData->uiOldCtrl = m_pRunData->uiCtrl;
					m_pRunData->uiCtrl    = CTRL_WIRELESS;
					SndMsgLog(LOG_TYPE_OTHER,1,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl,1); //��־��¼���Ʒ�ʽ�л� ADD?201311041530
				}				
				break;
			}
		}
		
		
		ResetRunData(0);  //���������»�ȡ��̬����
		
		m_pRunData->bNeedUpdate = false;

		if ( m_pRunData->bOldLock )
		{
			m_pRunData->bOldLock = false;
			ACE_DEBUG((LM_DEBUG,"%s:%d***OverCycle*** begin setcyclestepinfo\n",__FILE__,__LINE__));
			SetCycleStepInfo(0); //���������������ڵĲ�����Ϣ����
		}

		m_pRunData->ucStepNo     = 0;
		m_pRunData->ucStageNo    = 0;
		m_pRunData->ucElapseTime = 0;
		m_pRunData->ucStageNo = StepToStage(m_pRunData->ucStepNo,NULL); //���ݲ����Ż�ȡ�׶κ� MOD:201406191430
		m_pRunData->ucStepTime   = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
		m_pRunData->ucRunTime    = m_pRunData->ucStepTime;
	}

	if ( CTRL_WIRELESS == m_pRunData->uiCtrl )
	{
		for ( int i=0; i<MAX_STEP && i<m_pRunData->ucStepNum; i++ )
		{
			iStepLength[i] = m_pRunData->sStageStepInfo[i].ucStepLen;
		}
		//ACE_DEBUG((LM_DEBUG,"%s:%d  iCurTimePatternId=%d ucPhaseOffset=%d\n",__FILE__,__LINE__,m_pRunData->ucTimePatternId, m_pTscConfig->sTimePattern[m_pRunData->ucTimePatternId-1].ucPhaseOffset));
		CWirelessCoord::CreateInstance()->SetStepInfo( false
							    , m_pRunData->ucStepNum
								, m_pRunData->ucCycle
			                    , m_pTscConfig->sTimePattern[m_pRunData->ucTimePatternId-1].ucPhaseOffset
								, iStepLength);
		CWirelessCoord::CreateInstance()->OverCycle();
	}
	else if ( (CTRL_UTCS == m_pRunData->uiCtrl) && (m_ucUtcsComCycle != 0)) //*&& (m_ucUtscOffset != 0)*/ )   //���ر�׼ʱ ��ʹ��λ��Ϊ0Ҳ��������ʱ���ĵ���
	{
		if ( m_pRunData->ucCycle != m_ucUtcsComCycle )
		{  
			UtcsAdjustCycle();
		}
		for ( int i=0; i<MAX_STEP && i<m_pRunData->ucStepNum; i++ )
		{
			iStepLength[i] = m_pRunData->sStageStepInfo[i].ucStepLen;
		}
		CWirelessCoord::CreateInstance()->SetStepInfo(true, m_pRunData->ucStepNum, m_ucUtcsComCycle	, m_ucUtscOffset, iStepLength);
		CWirelessCoord::CreateInstance()->OverCycle();
	}
	else if ( ( CTRL_VEHACTUATED     == m_pRunData->uiCtrl ) 
		   || ( CTRL_MAIN_PRIORITY   == m_pRunData->uiCtrl ) 
		   || ( CTRL_SECOND_PRIORITY == m_pRunData->uiCtrl )
		    || ( CTRL_PREANALYSIS == m_pRunData->uiCtrl))
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d  when uiCtrl == 7 , GetAllStageDetector!\n",__FILE__,__LINE__));
		GetAllStageDetector();
	}
	else if ( (CTRL_ACTIVATE == m_pRunData->uiCtrl)  && ( STANDARD  == m_pRunData->uiWorkStatus) )
	{
		GetAllStageDetector();
		//CAdaptive::CreateInstance()->OverCycle();
	}

	switch ( m_pRunData->uiCtrl )
	{
		case CTRL_WIRELESS:
			m_pRunData->ucStepTime = CWirelessCoord::CreateInstance()->GetStepLength(m_pRunData->ucStepNo);
			m_pRunData->ucRunTime  = m_pRunData->ucStepTime;
			break;

		case CTRL_UTCS:
			if ( (m_ucUtcsComCycle != 0)||(m_ucUtscOffset != 0) )
			{
				
				m_pRunData->ucStepTime = CWirelessCoord::CreateInstance()->GetStepLength(m_pRunData->ucStepNo);

				if ( 0 == m_pRunData->ucStepTime )
				{
					m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				}
			}
			else
			{
				m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
			}
			m_pRunData->ucRunTime  = m_pRunData->ucStepTime; //ADD:20141114
			break;

		case CTRL_VEHACTUATED:
		case CTRL_MAIN_PRIORITY: 
		case CTRL_SECOND_PRIORITY: 
		case CTRL_PREANALYSIS:
			StepToStage(m_pRunData->ucStepNo,&bStageFirstStep);
			if ( IsLongStep(m_pRunData->ucStepNo) && bStageFirstStep )   //������Ϊ�׶εĵ�һ��
			{  //ACE_DEBUG((LM_DEBUG,"%s:%d  uiCtrl == 7 and islongsetp and bFirstStep and m_bVehile == true!\n",__FILE__,__LINE__));
				m_iMinStepTime = GetCurStepMinGreen(m_pRunData->ucStepNo , &m_iMaxStepTime , &m_iAdjustTime  );
				m_bVehile = true;
			}
			else
			{
				m_bVehile = false;
			}
			if(m_pRunData->uiCtrl == CTRL_PREANALYSIS)
			{
				
				bool bStageFirstStep = false ;
				int NextStage = (m_pRunData->ucStageNo+1 == m_pRunData->ucStageCount)?0x0:(m_pRunData->ucStageNo+1) ;
				StepToStage(0,&bStageFirstStep);
				if ( bStageFirstStep && IsLongStep(0) )	 //������Ϊ�׶εĵ�һ�� �������ǵ�ǰ���̵���
				{		
						if((m_pRunData->sScheduleTime[0].ucOption)&0x2)
						{					
							ACE_OS::printf("%s:%d CTRL_PREANALYSIS NextStageNo=%d ,Option=%d\r\n",__FILE__,__LINE__,NextStage,m_pRunData->sScheduleTime[NextStage].ucOption);
							Uint32 NextAllowPhase = m_pRunData->sScheduleTime[0].usAllowPhase ;
							Byte iIndex =0x0 ;
							Byte iDetCnt =0x0 ;
							bool bHaveCar = false ;
							while(iIndex < MAX_PHASE )
							{
								if ( NextAllowPhase>>iIndex & 1 )
								{
									iDetCnt = m_sPhaseDet[iIndex].iRoadwayCnt;
				
									Byte iDetIndex = 0;
									while ( iDetIndex < iDetCnt )
									{
										// Byte iDetId = m_sPhaseDet[iIndex].iDetectorId[iDetIndex];
										
										//ACE_OS::printf("%s:%d CTRL_PREANALYSIS PhaseId=%d DetecId=%d CarNumer=%d \r\n",__FILE__,__LINE__,iIndex+1,m_sPhaseDet[iIndex].iDetectorId[iDetIndex],m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]);
										 if(m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex] !=0x0)
										 {									
											ACE_OS::printf("%s:%d CTRL_PREANALYSIS PhaseId=%d DetecId=%d CarNumer=%d \r\n",__FILE__,__LINE__,iIndex+1,m_sPhaseDet[iIndex].iDetectorId[iDetIndex],m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]);
											bHaveCar = true ;
											break ;
										 }														
										iDetIndex++;
									}
								}
								if(bHaveCar)
									break;
								iIndex++;
							}
								if(!bHaveCar)
								{
									if(m_pRunData->ucStageCount >0x1)
									{
										//Byte setp0 = 0x0 ;
										Byte StageStepNo = StageToStep(1);
										m_pRunData->sScheduleTime[1].usAllowPhase = m_pRunData->sScheduleTime[1].usAllowPhase|m_pRunData->sScheduleTime[0].usAllowPhase;
										SetStepInfoWithStage(1,&StageStepNo,&(m_pRunData->sScheduleTime[1]));// ucStageIndex �ӽ׶κ�
										//SetCycleStepInfo(0);
										for(Byte index = 0x0 ;index< m_pRunData->ucStageIncludeSteps[0];index++)
										{
											m_pRunData->sStageStepInfo[index].ucStepLen = 0x0 ;
											
											ACE_OS::printf("%s:%d CTRL_PREANALYSIS stepno %d steptime=%d \r\n",__FILE__,__LINE__,index,m_pRunData->sStageStepInfo[index].ucStepLen);
											
										}
										m_pRunData->ucStepTime = 0x0 ;
									}
									
								m_pRunData->ucCycle = 0;
								for ( int i=0; i<m_pRunData->ucStepNum; i++ )
								{
									m_pRunData->ucCycle += m_pRunData->sStageStepInfo[i].ucStepLen;
								}
								}					
								
							}
						
					}

			}
			break;
		case CTRL_ACTIVATE:
			ucStageNo = StepToStage(m_pRunData->ucStepNo,&bStageFirstStep);
			if ( bStageFirstStep && IsLongStep(m_pRunData->ucStepNo) )   //������Ϊ�׶εĵ�һ��
			{
				//m_pRunData->ucStepTime = 
					//CAdaptive::CreateInstance()->GetStageGreenLen(m_pRunData->ucStepNo,ucStageNo);
			}
			else
			{
				m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
			}
			break;
		default:
			break;
	}

	CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn	,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
	if ( m_pRunData->bStartFlash ) 
	{
		m_pRunData->bStartFlash = false;
	}
	
	if(m_pRunData->uiCtrl == CTRL_SECOND_PRIORITY || m_pRunData->uiCtrl == CTRL_MAIN_PRIORITY) //ÿ���ڷ���Ĭ�ϴ��߸�Ӧ������λ�޳���ʱ��Ϊ0 ADD:201406191430
	{
		bSecondPriority = false ; 
		//ACE_OS::printf("%s:%d CTRL_SECOND_PRIORITY bSecondPriorty=%s steptime=%d\r\n",__FILE__,__LINE__,(CreateInstance()->bSecondPriority == false)?"false":"true",m_pRunData->ucStepTime);
		//SetUpdateBit();
	}
	
	//CwpmGetCntDownSecStep();
	
	//�����������ļ��ж��� 0 ��ʾ �����ñ��ݵ�Ƭ����1��ʾ ���ñ��ݵ�Ƭ��>>>>>>>>>
	int backuptruefalse;
	Configure::CreateInstance()->GetInteger("FUNCTION","BACKUP",backuptruefalse);
	if(backuptruefalse == 1)
	{
	//lastUcTimePatternId ��һ����ʱ���Ƿ��뵱ǰ����ʱ��һ��  
	//������Ҫ��ͬ���ķ��������в������ݵ���Ƭ����
		if(lastUcTimePatternId != m_pRunData->ucTimePatternId)
		{			
			ACE_DEBUG((LM_DEBUG,"%s:%d<<<<<OverCycle<<<<< SCM backup LastUcTimePatternId =%d,m_pRunData->ucTimePatternId=%d\n",__FILE__,__LINE__,lastUcTimePatternId,m_pRunData->ucTimePatternId));
			MainBackup::CreateInstance()->DoSendStep(m_pRunData);
			lastUcTimePatternId = m_pRunData->ucTimePatternId;			
		}
		
		
	}
	///<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	//CLamp::CreateInstance()->SetOverCycle();
	SetCycleBit(true);
}


/**************************************************************
Function:       CManaKernel::SetCycleBit
Description:    ��������λ
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::SetCycleBit(bool bSetCycle)
{
	//ACE_Guard<ACE_Thread_Mutex>  guard(m_mutexSetCycle);
	m_bCycleBit = bSetCycle;
}


/**************************************************************
Function:       CManaKernel::CwpmGetCntDownSecStep
Description:    ���͵���ʱ��Ϣ
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::CwpmGetCntDownSecStep()
{
	if ( (SIGNALOFF     == m_pRunData->uiWorkStatus)|| (ALLRED      == m_pRunData->uiWorkStatus) 
		|| (FLASH       == m_pRunData->uiWorkStatus)|| (CTRL_MANUAL == m_pRunData->uiCtrl) 
		|| (CTRL_PANEL == m_pRunData->uiCtrl )) 
	{
		return ;
	}
	if ( m_pTscConfig->sSpecFun[FUN_COUNT_DOWN].ucValue != COUNTDOWN_STUDY&& MODE_TSC == m_pRunData->ucWorkMode )
	{
		#ifdef GA_COUNT_DOWN		
		CGaCountDown::CreateInstance()->GaSendStepPer();
		#endif
	}
	
}


/**************************************************************
Function:       CManaKernel::ResetRunData
Description:    ���������źŻ���̬����,����һ������һ��,�ı��źŻ�
			״̬��ʱ��Ҳ�����
Input:          ucTime :  ������ȫ���ʱ��           
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::ResetRunData(Byte ucTime)
{

	if ( FLASH == m_pRunData->uiWorkStatus )// ��ʼ��ֵΪ CTRL_SCHEDULE;
	{
		if ( m_bSpeStatusTblSchedule )
		{
			m_pRunData->ucStepTime = MAX_SPESTATUS_CYCLE;			
			m_pRunData->flashType = CTRLBOARD_FLASH_NORMAL;
		}
		else if ( 0 == ucTime )
		{
			m_pRunData->ucStepTime = m_pTscConfig->sUnit.ucStartFlashTime;
			ACE_DEBUG((LM_DEBUG,"%s:%d***ResetRunData*** When ucTime para is 0 ,YWFLASH 12 seconds!\n" ,__FILE__,__LINE__));
		}
		else
		{
			m_pRunData->ucStepTime = ucTime;
		}
		
		m_pRunData->ucElapseTime = 0;
		m_pRunData->ucStepNo     = 0;
		m_pRunData->ucStepNum    = 1;
		m_pRunData->ucRunTime    = m_pRunData->ucStepTime;
		m_pRunData->ucStageNo    = 0;
		m_pRunData->ucStageCount = 1;
		
		if ( m_bSpePhase )  //�ض���λ����
		{
			m_bSpePhase = false;
			for ( int i=0; i<MAX_LAMP; i++ )
			{
				if ( ( LAMP_COLOR_GREEN == i % 3 ) && ( 1 == m_ucLampOn[i] )  )
				{
					m_ucLampFlash[i] = 1;
				}
			}
			ACE_OS::memcpy(m_pRunData->sStageStepInfo[0].ucLampOn    , m_ucLampOn    , MAX_LAMP);
			ACE_OS::memcpy(m_pRunData->sStageStepInfo[0].ucLampFlash , m_ucLampFlash , MAX_LAMP);
		}
		else
		{
			ACE_OS::memset( &m_pRunData->sStageStepInfo[0] , 0 , sizeof(SStepInfo) );
			/******************* ����֮ǰ������Ϩ���źţ��������������**************************************/
			CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[0].ucLampOn, m_pRunData->sStageStepInfo[0].ucLampFlash);
			CLampBoard::CreateInstance()->SendLamp();
			/******************* ����֮ǰ������Ϩ���źţ��������������**************************************/
			m_pRunData->sStageStepInfo[0].ucStepLen = m_pRunData->ucStepTime;
			SetLampColor(1);			
		}
		
		m_bSpeStatus = true;
	}
	else if ( m_pRunData->uiWorkStatus == ALLRED )  //ȫ��
	{
		if ( m_bSpeStatusTblSchedule )
		{
			m_pRunData->ucStepTime = MAX_SPESTATUS_CYCLE;
		}
		else if ( 0 == ucTime )
		{
			m_pRunData->ucStepTime = m_pTscConfig->sUnit.ucStartAllRedTime;
	        ACE_DEBUG((LM_DEBUG,"%s:%d***ResetRunData*** After YWFLASH , RedFLASH 6 seconds!\n" ,__FILE__,__LINE__));
		}
		else
		{
			m_pRunData->ucStepTime = ucTime;
		}
		
		m_pRunData->ucElapseTime = 0;
		m_pRunData->ucStepNo = 0;
		m_pRunData->ucStepNum = 1;
		m_pRunData->ucRunTime = m_pRunData->ucStepTime;
		m_pRunData->ucStageNo = 0;
		m_pRunData->ucStageCount = 1;
		
		ACE_OS::memset( &m_pRunData->sStageStepInfo[0] , 0 , sizeof(SStepInfo) );		
		m_pRunData->sStageStepInfo[0].ucStepLen = m_pRunData->ucStepTime;		
		SetLampColor(2);
		m_bSpeStatus = true;
	}
	else if ( m_pRunData->uiWorkStatus == SIGNALOFF )  //�ص�
	{
		if ( m_bSpeStatusTblSchedule )
		{
			m_pRunData->ucStepTime = MAX_SPESTATUS_CYCLE;
		}
		else
		{
			m_pRunData->ucStepTime   = ucTime;
		}
		m_pRunData->ucElapseTime = 0;
		m_pRunData->ucStepNo     = 0;
		m_pRunData->ucStepNum    = 1;	
		
		ACE_OS::memset( &m_pRunData->sStageStepInfo[0] , 0 , sizeof(SStepInfo) );
		SetLampColor(0);
		m_pRunData->sStageStepInfo[0].ucStepLen = m_pRunData->ucStepTime;	
		m_bSpeStatus = true;
	}
	else if ( m_pRunData->uiWorkStatus == STANDARD )  //��׼
	{
	//////////////	ACE_DEBUG((LM_DEBUG,"\n%s:%d***ResetRunData*** When uiWorkStatus == STANDARD,call GetRunDataStandard()\n" ,__FILE__,__LINE__));
		
		GetRunDataStandard(); //�����ع��춯̬����
		m_bSpeStatus = false;
		
	}

}


/**************************************************************
Function:       CManaKernel::GetRunDataStandard
Description:    ��׼����£������߼��Ļ�ȡ��̬��Ϣ ������Ҫ���¼�
				��ʱ�α�  ��ȡ��ʱ������ ��ȡ�׶���ʱ���,��ȡ��
				����Ϣ
Input:          ��          
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::GetRunDataStandard()
{
	bool bNewDay         = false;
	Byte ucCurScheduleId = m_pRunData->ucScheduleId ;
	static Byte ucDay = 0 ;
	ACE_Date_Time tvTime(ACE_OS::gettimeofday());
	if(ucDay == 0)
		ucDay = tvTime.day();
	
	if ( m_pRunData->bNeedUpdate || (0==tvTime.hour() && tvTime.day() !=ucDay ) )  //��һ��,������Ҫ���¼���ʱ�α���Ϣ
	{
		bNewDay = true;
		if( m_pRunData->bNeedUpdate)
			 ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** m_pRunData->bNeedUpdate== true!\n" ,__FILE__,__LINE__));
		else 
		{
			ucDay = tvTime.day();
			ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** 0==tvTime.hour()!\n" ,__FILE__,__LINE__));
		}
	}

	if ( bNewDay )
	{
		//�����ݿ������ֶ���Ҫ���¼��ػ����µ�һ�죬���»�ȡʱ�α�
		ucCurScheduleId = GetScheduleId((Byte)tvTime.month(),(Byte)tvTime.day(),(Byte)tvTime.weekday());  
		
		if (ucCurScheduleId != m_pRunData->ucScheduleId)
		{
			//���´����ݿ����1��ʱ�α���Ϣ 
			unsigned short usCount = 0;
			int iIndex   = 0;
			GBT_DB::TblSchedule tblSchedule;
			GBT_DB::Schedule* pSchedule = NULL;
			ACE_OS::memset(&tblSchedule,0,sizeof(GBT_DB::TblSchedule));			
		
			(CDbInstance::m_cGbtTscDb).QuerySchedule(ucCurScheduleId,tblSchedule);
			pSchedule = tblSchedule.GetData(usCount); 
			iIndex = 0;
			while ( iIndex < usCount )
			{
				ACE_OS::memcpy(m_pTscConfig->sSchedule+iIndex,pSchedule,sizeof(SSchedule));
				pSchedule++;
				iIndex++;
			}
			ACE_OS::memset(m_pTscConfig->sSchedule+iIndex,0,sizeof(SSchedule));
			
		}
	}			
		//����ʱ����Ϣ���»�ȡ��ʱ������
		Byte ucCurCtrl          = m_pRunData->uiCtrl;
		Byte ucCurStatus        = m_pRunData->uiWorkStatus;
		Byte ucCurTimePatternId = 0 ;
		m_pRunData->ucScheduleId = ucCurScheduleId ;
		// �������ݵ�ǰʱ������������ʱ������,���Ʒ�ʽ,״̬
		ucCurTimePatternId = 
		GetTimePatternId( ucCurScheduleId , &ucCurCtrl , &ucCurStatus );
		//��������ʱ�α���Ʒ�ʽ����
		m_pRunData->uiScheduleCtrl  = ucCurCtrl;
		if(m_pRunData->uiScheduleCtrl == CTRL_SCHEDULE && m_pRunData->uiCtrl ==CTRL_SCHEDULE&& bDegrade == true)
		{
			//ADD 20150120 ������ʱ���л�����ʱ������
			ACE_OS::printf("%s:%d Now changed to CTRL_SCHEDULE ,bDegrade changed to false !\n",__FILE__,__LINE__);
			bDegrade = false ;
		}

		//�������ض�����
		if(bTmpPattern == true  && m_iTimePatternId != 0)
		{
			ucCurTimePatternId = m_iTimePatternId;		
			ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** Set m_iTimePatternId = %d \n" ,__FILE__,__LINE__,m_iTimePatternId));
			CTscKernelGb25280::CreateInstance()->SelfReportWorkStatus();  //���͹���״̬�ı�
		}		  
		 //���Ʒ�ʽ�ı�
		if ( ucCurCtrl != m_pRunData->uiCtrl) 
		{
			SwitchCtrl(ucCurCtrl); 				
		//	ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** Schedule Table Control Mode Changed from %d to %d !\n" ,__FILE__,__LINE__,m_pRunData->uiCtrl,ucCurCtrl));
		}
	
	//��ȡ��ʱ�׶α�� 
	Byte ucCurScheduleTimeId = GetScheduleTimeId(ucCurTimePatternId,m_ucUtcsComCycle,m_ucUtscOffset); 
	if(ucCurScheduleTimeId ==0x0) //��ȡ�����׶���ʱ���(�޷���)
	{
		bDegrade = true ;
		CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_SYSTEMCONTROL,254);
		ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** StageSchedulePatternID ==0 ,Degrade to YEFLASH!\n" ,__FILE__,__LINE__));	
		CMainBoardLed::CreateInstance()->DoModeLed(true,true);
		m_pRunData->flashType = CTRLBOARD_FLASH_NOPATTERN;
		return ;
	}

	//��ȡ�׶���ʱ��
	 if(m_pRunData->bNeedUpdate || ucCurTimePatternId != m_pRunData->ucTimePatternId ||m_pRunData->uiCtrl == CTRL_PREANALYSIS)
	 {		
		m_pRunData->ucTimePatternId = ucCurTimePatternId;
		
		//��ȡ��ʱ�׶α�� 
//		Byte ucCurScheduleTimeId = GetScheduleTimeId(ucCurTimePatternId,m_ucUtcsComCycle,m_ucUtscOffset); 
				
		if ( m_pRunData->bNeedUpdate|| (ucCurScheduleTimeId != m_pRunData->ucScheduleTimeId) ||m_pRunData->uiCtrl == CTRL_PREANALYSIS )
		{
			m_pRunData->ucScheduleTimeId = ucCurScheduleTimeId;
			//���¼��ض�̬���ݵĽ׶���ʱ��ϢsScheduleTime
			if(!GetSonScheduleTime(ucCurScheduleTimeId))
			{
				bDegrade = true ;
				CGbtMsgQueue::CreateInstance()->SendTscCommand(OBJECT_SWITCH_SYSTEMCONTROL,254);
				ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** StageSchedulePatternID ==0 ,Degrade to YEFLASH!\n" ,__FILE__,__LINE__));	
				CMainBoardLed::CreateInstance()->DoModeLed(true,true);
				m_pRunData->flashType = CTRLBOARD_FLASH_NOPATTERN;
				return ;	
			}
			ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** ReConstruct every step info,when manual update database or PatternId changed!\n" ,__FILE__,__LINE__));		
			SetCycleStepInfo(0);
			
		}
	}
	else if ( m_bSpeStatus && STANDARD == ucCurStatus)  //������״̬-->STANDARD ��Ҫ���»�ȡ����
	{
		Byte ucCurScheduleTimeId = GetScheduleTimeId(ucCurTimePatternId,m_ucUtcsComCycle,m_ucUtscOffset);

		m_pRunData->ucTimePatternId = ucCurTimePatternId;
		m_pRunData->ucScheduleTimeId = ucCurScheduleTimeId;
		GetSonScheduleTime(ucCurScheduleTimeId);
		SetCycleStepInfo(0);
		ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** ReConstruct every step info,when no manual update database and PatterId  not changed but from m_bSpeStatus changed to STANDARD!�\\n" ,__FILE__,__LINE__));
				
	}
	if ( ucCurStatus != m_pRunData->uiWorkStatus && m_pRunData->uiCtrl != CTRL_UTCS)  //����״̬�ı� ��������ʱ������
	{		
		ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** Status changed from %d to %d !\n" ,__FILE__,__LINE__,m_pRunData->uiWorkStatus,ucCurStatus));
		SwitchStatus(ucCurStatus);
	}
	else if ( m_bVirtualStandard && (STANDARD == ucCurStatus) ) 
	{
		//STANDARD(FLASH��ALLRED��SINGLEALL) --> STANDARD
		
		ACE_DEBUG((LM_DEBUG,"%s:%d***GetRunDataStandard*** m_bVirtualStandard ==true ,uiWorkStatus =%d uiOldWorkStatus=%d \n" ,__FILE__,__LINE__,m_pRunData->uiWorkStatus,m_pRunData->uiOldWorkStatus));
		m_pRunData->uiWorkStatus = m_pRunData->uiOldWorkStatus;
		m_bVirtualStandard      = false;
		SwitchStatus(ucCurStatus);
	}
	
	if(m_pRunData->uiCtrl == CTRL_PREANALYSIS) //��ʼ����̬��С�������
	{
		if(m_ucStageDynamicMinGreen[0] ==0x0 && m_ucStageDynamicMaxGreen[0] ==0x0)
		{
			for(Byte index=0x0 ;index<m_pRunData->ucStageCount ;index++)
			{
				Byte StepNo = StageToStep(index);
				if(IsLongStep(StepNo))
					{
					m_ucStageDynamicMinGreen[index] = GetCurStepMinGreen(StepNo , &m_ucStageDynamicMaxGreen[index],&m_ucStageDynamicAddGreen[index]);
				//	m_ucStageDynamicMaxGreen[index] = m_ucStageDynamicMaxGreen[index]-10 ;

					}

				}
			ACE_OS::printf("%s:%d m_ucStageDynamicMinGreen[0]=%d m_ucStageDynamicMaxGreen[0]=%d m_ucStageDynamicAddGreen[0]=%d \r\n,",__FILE__,__LINE__,m_ucStageDynamicMinGreen[0],m_ucStageDynamicMaxGreen[0],m_ucStageDynamicAddGreen[0]);
		}

	}	

}


/**************************************************************
Function:       CManaKernel::GetScheduleId
Description:    �����¡��ա����ڻ�ȡ��ǰ��ʱ�α��
Input:            ucWeek �� �ֽ�λ0���ã��ֽ�λ1������һ���������ƣ��ֽ�λ7��������
			ucMonth ���ֽ�λ0���ã��ֽ�λ1��ʾ1�£��������ƣ��ֽ�λ12����12�� 
			ucDay   ���ֽ�λ0���ã��ֽ�λ1��ʾ1�ã��������ƣ��ֽ�λ31����31��      
Output:         ��
Return:         ���յ�ʱ�α�ţ���û�в�ѯ���򷵻�1
***************************************************************/
Byte CManaKernel::GetScheduleId(Byte ucMonth,Byte ucDay , Byte ucWeek)
{
	//���� �ڼ��� �ض��� + �ض��� + ��ȫѡ
	//ACE_DEBUG((LM_DEBUG,"%s:%d***GetScheduleId*** ucId= %d, ucScheduledId = %d,month = %d ,day = %u ,week = %d \n" ,__FILE__,__LINE__,m_pTscConfig->sTimeGroup[0].ucId,m_pTscConfig->sTimeGroup[0].ucScheduleId,m_pTscConfig->sTimeGroup[0].usMonth,m_pTscConfig->sTimeGroup[0].uiDayWithMonth,m_pTscConfig->sTimeGroup[0].ucDayWithWeek));
	
	for (int i=0; i<MAX_TIMEGROUP; i++ )
	{
		if ( m_pTscConfig->sTimeGroup[i].ucId != 0 )
		{
			//ACE_DEBUG((LM_DEBUG,"%s:%d sTimeGroup[%d].ucId!=0\n",__FILE__,__LINE__,i));
			if ( (m_pTscConfig->sTimeGroup[i].usMonth>>ucMonth) & 0x1 
				&& ( (m_pTscConfig->sTimeGroup[i].usMonth & 0x1ffe) != 0x1ffe ) )
			{
				if ( (((m_pTscConfig->sTimeGroup[i].uiDayWithMonth)>>ucDay) & 0x1 )
				&& ( (m_pTscConfig->sTimeGroup[i].uiDayWithMonth & 0xfffffffe) != 0xfffffffe) )
				{
					if ( (m_pTscConfig->sTimeGroup[i].ucDayWithWeek & 0xfe) == 0xfe )
					{
						//ACE_DEBUG((LM_DEBUG,"%s:%d***GetScheduleId***  ucScheduleId = %d \n" ,__FILE__,__LINE__,m_pTscConfig->sTimeGroup[i].ucScheduleId));
						if (m_pTscConfig->sTimeGroup[i].ucScheduleId >0)
							{
							    m_pRunData->ucPlanId = m_pTscConfig->sTimeGroup[i].ucId ;
								return m_pTscConfig->sTimeGroup[i].ucScheduleId;
							}
					}
				}
			}
		}
		else
		{
			break;  //û�и����ʱ�䷽��
		}
	}

	//���ܣ�ȫ��   + ȫ��   + �ض���
	for (int i=0; i<MAX_TIMEGROUP; i++ )
	{	
		if ( m_pTscConfig->sTimeGroup[i].ucId != 0 )
		{
			//ACE_DEBUG((LM_DEBUG,"%s:%d sTimeGroup[%d].ucId!=0\n",__FILE__,__LINE__,i));
			if ( (m_pTscConfig->sTimeGroup[i].usMonth & 0x1ffe) == 0x1ffe )
			{
				if ( (m_pTscConfig->sTimeGroup[i].uiDayWithMonth & 0xfffffffe) == 0xfffffffe )
				{
					if(ucWeek ==0)
						ucWeek = 7 ;  //ACE ����ֵλ0
					if ((((m_pTscConfig->sTimeGroup[i].ucDayWithWeek)>>ucWeek) & 0x1)
						&& ( ( m_pTscConfig->sTimeGroup[i].ucDayWithWeek & 0xfe) != 0xfe ) )
					{
						//ACE_DEBUG((LM_DEBUG,"%s:%d***GetScheduleId***  ucScheduleId = %d \n" ,__FILE__,__LINE__,m_pTscConfig->sTimeGroup[i].ucScheduleId));
						if (m_pTscConfig->sTimeGroup[i].ucScheduleId >0)
							{
							    m_pRunData->ucPlanId = m_pTscConfig->sTimeGroup[i].ucId ;
								return m_pTscConfig->sTimeGroup[i].ucScheduleId;
							}
					}
				}
			}
		}
		else
		{
			break;  //û�и����ʱ�䷽��
		}
	}

	//���£��ض��� + ȫ��   + ȫ��
	for (int i=0; i<MAX_TIMEGROUP; i++ )
	{	
		if ( m_pTscConfig->sTimeGroup[i].ucId != 0 )
		{
			//ACE_DEBUG((LM_DEBUG,"%s:%d sTimeGroup[%d].ucId!=0\n",__FILE__,__LINE__,i));
			if ( (m_pTscConfig->sTimeGroup[i].usMonth>>ucMonth) &0x1 
			 && ((m_pTscConfig->sTimeGroup[i].usMonth & 0x1ffe) != 0x1ffe ) )
			{
				if ( (m_pTscConfig->sTimeGroup[i].uiDayWithMonth & 0xfffffffe) == 0xfffffffe )
				{
					if ( (m_pTscConfig->sTimeGroup[i].ucDayWithWeek & 0xfe) == 0xfe )
					{
					//	ACE_DEBUG((LM_DEBUG,"%s:%d***GetScheduleId***  ucScheduleId = %d \n" ,__FILE__,__LINE__,m_pTscConfig->sTimeGroup[i].ucScheduleId));
						if (m_pTscConfig->sTimeGroup[i].ucScheduleId >0)
							{
								m_pRunData->ucPlanId = m_pTscConfig->sTimeGroup[i].ucId ;
								return m_pTscConfig->sTimeGroup[i].ucScheduleId;
							}
					}
				}
			}
		}
		else
		{
			break;  //û�и����ʱ�䷽��
		}
	}

	//�������ض��� + �ض��� + �ض���
	for (int i=0; i<MAX_TIMEGROUP; i++ )
	{	
		if ( m_pTscConfig->sTimeGroup[i].ucId != 0 )
		{
			
			if ( (m_pTscConfig->sTimeGroup[i].usMonth>>ucMonth) &0x1 )//�·���ȷ b1:1�� b12:12
			{
				if(ucWeek ==0)     //MOD:201403311117
					ucWeek = 7 ;  //ACE ����ֵλ0
				if ( ( (m_pTscConfig->sTimeGroup[i].ucDayWithWeek>>(ucWeek)) & 0x1 ) //b1������ b2����1 
					&& ( (m_pTscConfig->sTimeGroup[i].uiDayWithMonth>>ucDay) & 0x1 ) )  //b1:1�� b2
				{
				//	ACE_DEBUG((LM_DEBUG,"%s:%d sTimeGroup[%d].ucId=%d\n",__FILE__,__LINE__,i,m_pTscConfig->sTimeGroup[i].ucId));
						
				   m_pRunData->ucPlanId = m_pTscConfig->sTimeGroup[i].ucId ;
				   return m_pTscConfig->sTimeGroup[i].ucScheduleId;
							
				}
			}
		}
		else
		{
			break;  //û�и����ʱ�䷽��
		}
	}
	ACE_OS::printf("%s:%d***GetScheduleId*** Cant get ScheduleId from DB ,Default return 1!\n",__FILE__,__LINE__);
	
	m_pRunData->ucPlanId = 0 ;
	return 1; //Ĭ�Ϸ���ʱ�α�1
}



/**************************************************************
Function:       CManaKernel::GetTimePatternId
Description:    �����¡��ա����ڻ�ȡ��ǰ��ʱ�α��
Input:          ucScheduleId ��ʱ�α��       
Output:         ucCtrl	 �� ��ǰ����ģʽ
			ucStatus �� ��ǰ����״
Return:         ��ʱ������ 
***************************************************************/
Byte CManaKernel::GetTimePatternId(Byte ucScheduleId , Byte* ucCtrl , Byte* ucStatus )
{	
	/////////////ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** m_pRunData->uiCtrl= %d m_iTimePatternId= %d\n" ,__FILE__,__LINE__,m_pRunData->uiCtrl ,m_iTimePatternId));
	bool bLastTem           = false;
	int  iIndex             = 0;
	Byte ucCurTimePatternId = 0;
	Byte ucCtrlTmp          = 0;
	ACE_Date_Time tvTime(ACE_OS::gettimeofday());
	for ( ; iIndex<MAX_SCHEDULE_PER_DAY; iIndex++)
	{		
		if ( m_pTscConfig->sSchedule[iIndex].ucId != 0 && (m_pTscConfig->sSchedule[iIndex].ucEventId != 0 ) )
		{
			if ( m_pTscConfig->sSchedule[iIndex+1].ucId == ucScheduleId )//�ж��Ƿ������һ��ʱ��
			{				
				if ((CompareTime(m_pTscConfig->sSchedule[iIndex].ucHour,m_pTscConfig->sSchedule[iIndex].ucMin ,
								(Byte)tvTime.hour(),(Byte)tvTime.minute()) )
								 && 
					(!CompareTime( m_pTscConfig->sSchedule[iIndex+1].ucHour,m_pTscConfig->sSchedule[iIndex+1].ucMin,
								 (Byte)tvTime.hour(),(Byte)tvTime.minute()))
					 )
				{
					ucCurTimePatternId = m_pTscConfig->sSchedule[iIndex].ucTimePatternId;
					ucCtrlTmp          = m_pTscConfig->sSchedule[iIndex].ucCtrl;	
					
				///////////////////	ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ucScheduleId=%d index=%d hour=%d min=%d ucCtrlTmp=%d !\n",__FILE__,__LINE__,ucScheduleId,iIndex,(Byte)tvTime.hour(),(Byte)tvTime.minute(),ucCtrlTmp));
					break;
				}
			}
		}
		else //Ӧ����ǰȥ
		{	
			bLastTem = true; //
			break;           //
		}
	}

	if ( bLastTem && (iIndex != 0 )) //���һ�ε�ʱ��׶�
	{
		
		ucCurTimePatternId =  m_pTscConfig->sSchedule[iIndex-1].ucTimePatternId;
		ucCtrlTmp = m_pTscConfig->sSchedule[iIndex-1].ucCtrl;
		
		//ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ucScheduleId=%d index=%d hour=%d min=%d ucCtrlTmp=%d !\n",__FILE__,__LINE__,ucScheduleId,iIndex-1,(Byte)tvTime.hour(),(Byte)tvTime.minute(),ucCtrlTmp));
	}	
	m_bSpeStatusTblSchedule = false;	 
    
	switch ( ucCtrlTmp )
	{
		case 0:     //0:��ʱ��
			*ucCtrl   = CTRL_SCHEDULE;
			*ucStatus = STANDARD;
			
			////////////////ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,STRL_SCHEDULE!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		case 1:    //1:�ص�
			*ucCtrl   = CTRL_SCHEDULE_OFF;
			*ucStatus = SIGNALOFF;
			m_bSpeStatusTblSchedule = true;
		///	/AC//////////E_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,SIGNALOFF!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		case 2:    //2:����
			*ucCtrl   = CTRL_SCHEDULE_FLASH;
			*ucStatus = FLASH;
			m_bSpeStatusTblSchedule = true;
			ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,FLASH!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		case 3:    //3:ȫ��
			*ucCtrl   = CTRL_SCHEDULE_RED;
			*ucStatus = ALLRED;
			m_bSpeStatusTblSchedule = true;
			ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,ALLRED!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		case 4:    //4�����߰��Ӧ
			*ucCtrl   = CTRL_MAIN_PRIORITY;
			*ucStatus = STANDARD;
			break;
		case 5:    //5:���߰��Ӧ
			*ucCtrl   = CTRL_SECOND_PRIORITY;
			*ucStatus = STANDARD;
			break;
		case 6:    //6:ȫ��Ӧ
			*ucCtrl   = CTRL_VEHACTUATED;
			*ucStatus = STANDARD;		
		/////////////	ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,STRL_VEHACTUATED!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		case 7:    //7:�޵�����
			*ucCtrl   = CTRL_WIRELESS;
			*ucStatus = STANDARD;
			break;
		case 8:    //8:����Ӧ����
			*ucCtrl   = CTRL_ACTIVATE;
			*ucStatus = STANDARD;
			/////////////ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,CTRL_ACTIVATE!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		case 12:   //12:����
			*ucCtrl   = CTRL_UTCS;
			*ucStatus = STANDARD;
			ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,CTRL_UTCS!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		
		case 9:   //9:��̬Ԥ����
			*ucCtrl   = CTRL_PREANALYSIS;
			*ucStatus = STANDARD;
			///////////ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,CTRL_PREANALYSIS!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
		
		default:   //Ĭ�϶�ʱ��
			*ucCtrl   = CTRL_SCHEDULE;
			*ucStatus = STANDARD;
			/////////////ACE_DEBUG((LM_DEBUG,"%s:%d***GetTimePatternId*** ScheduleId:%d ,default CTRL_SCHEDULE!\n\n",__FILE__,__LINE__,ucScheduleId));
			break;
	}	
	return ucCurTimePatternId;
}


	

/**************************************************************
Function:       CManaKernel::GetScheduleTimeId
Description:    �����¡��ա����ڻ�ȡ��ǰ��ʱ�α��
Input:          ucTimePatternId - ��ʱ�������       
Output:         ucCycle         - ����
*       	    ucOffSet        - ��λ��
Return:         �׶���ʱ��� �����û�в�ѯ���Ϸ��׶���ʱ����򷵻�1
***************************************************************/
Byte CManaKernel::GetScheduleTimeId(Byte ucTimePatternId,Byte& ucCycle,Byte& ucOffSet)
{	
	for ( int i=0; i<MAX_TIMEPATTERN; i++ )
	{
		if ( m_pTscConfig->sTimePattern[i].ucId != 0 )
		{
			if ( ucTimePatternId == m_pTscConfig->sTimePattern[i].ucId )
			{
				ucCycle  = m_pTscConfig->sTimePattern[i].ucCycleLen;
				ucOffSet = m_pTscConfig->sTimePattern[i].ucPhaseOffset;
				return m_pTscConfig->sTimePattern[i].ucScheduleTimeId;
			}	
		}
		else 
		{
			break;
		}
	}
	return 0;  //���Ϸ��Ĳ���,������
}


/**************************************************************
Function:       CManaKernel::GetSonScheduleTime
Description:    ���ݶ�̬���ݵĽ׶���ʱ���(ucScheduleTimeId)��ȡ
				�����ӽ׶ε���Ϣ
Input:          ucScheduleTimeId - �׶���ʱ���       
Output:         ��
Return:         ��
***************************************************************/
bool CManaKernel::GetSonScheduleTime(Byte ucScheduleTimeId)
{
	Byte ucIndex = 0;
	m_pRunData->ucStageCount = 0;

	ACE_OS::memset(m_pRunData->sScheduleTime,0,MAX_SON_SCHEDULE*sizeof(SScheduleTime));

	for ( int i=0; i<MAX_SCHEDULETIME_TYPE; i++ )
	{
		for ( int j=0; j<MAX_SON_SCHEDULE; j++ )
		{
			if ( m_pTscConfig->sScheduleTime[i][j].ucId == ucScheduleTimeId )
			{
				ACE_OS::memcpy(&m_pRunData->sScheduleTime[ucIndex++], &m_pTscConfig->sScheduleTime[i][j] , sizeof(SScheduleTime));
			
				m_pRunData->ucStageCount++;
			}
			else
			{
				break;
			}
		}
	}
	if(m_pRunData->ucStageCount > 0)
		return true;
	else
		return false ;
}


/**************************************************************
Function:       CManaKernel::StageToStep
Description:    ���ݽ׶κŻ�ȡ������ �ý׶κŶ�Ӧ����������
Input:          iStageNo:�׶κ�      
Output:         ��
Return:         ������
***************************************************************/
Byte CManaKernel::StageToStep(int iStageNo)
{
	Byte ucCurrStepNo  = 0;
	Byte ucCurrStageNo = iStageNo;	
	for ( int i = 0; i<ucCurrStageNo; i++ )
	{
		ucCurrStepNo += m_pRunData->ucStageIncludeSteps[i];
	}
	return ucCurrStepNo;
}


/**************************************************************
Function:       CManaKernel::StepToStage
Description:    ���ݲ����Ż�ȡ�׶κ�
Input:          iStepNo:������      
Output:         bStageFirstStep:�Ƿ�Ϊ�׶εĵ�һ��
Return:         �׶κ�
***************************************************************/
Byte CManaKernel::StepToStage(int iStepNo,bool* bStageFirstStep)
{
	int ucCurrStepNo = iStepNo;

	for ( int i=0; i<MAX_SON_SCHEDULE; i++ )
	{
		ucCurrStepNo = ucCurrStepNo - m_pRunData->ucStageIncludeSteps[i];
		if ( (0 == ucCurrStepNo) || (0==iStepNo) )
		{
			if ( bStageFirstStep != NULL )
			{	
				*bStageFirstStep = true;
			}

			if (0==iStepNo)
			{
				if(m_pRunData->ucStageIncludeSteps[i] == 0)
					return i+1 ;
				return 0;
			}
			return i+1;
		}
		if ( ucCurrStepNo < 0 )
		{
			return i;
		}
	}

	return 0;
}


/**************************************************************
Function:       CManaKernel::GetStageTime
Description:    ��ȡ�׶ε�Ԥ����ʱ��ͽ׶��Ѿ����е�ʱ��
Input:          ��     
Output:         pTotalTime-�׶ε�Ԥ����ʱ��  pElapseTime-�׶���
				�����е�ʱ��
Return:         ��
***************************************************************/
void CManaKernel::GetStageTime(Byte* pTotalTime,Byte* pElapseTime)
{
	Byte ucStartStepNo  = 0;
	Byte ucEndStepNo    = 0;
	Byte ucCurStepNo    = m_pRunData->ucStepNo;
	Byte ucCurStageNo   = m_pRunData->ucStageNo;
	Byte ucTotalTime    = 0;
	Byte ucElapseTime   = 0;

	ucStartStepNo = StageToStep(ucCurStageNo);
	if ( (ucCurStageNo+1) < m_pRunData->ucStageCount )
	{
		ucEndStepNo = StageToStep(ucCurStageNo+1) - 1;
	}
	else
	{
		ucEndStepNo = m_pRunData->ucStepNum - 1;
	}

	for ( Byte ucStepNo=ucStartStepNo; ucStepNo<=ucEndStepNo && ucStepNo<m_pRunData->ucStepNum; ucStepNo++ )
	{
		ucTotalTime += m_pRunData->sStageStepInfo[ucStepNo].ucStepLen;
	}

	for ( Byte ucStepNo=ucStartStepNo; ucStepNo<ucCurStepNo && ucStepNo<m_pRunData->ucStepNum; ucStepNo++ )
	{
		ucElapseTime += m_pRunData->sStageStepInfo[ucStepNo].ucStepLen;
	}
	ucElapseTime += m_pRunData->ucElapseTime + 1;

	if (CTRL_PREANALYSIS == m_pRunData->uiCtrl||CTRL_VEHACTUATED== m_pRunData->uiCtrl || CTRL_MAIN_PRIORITY == m_pRunData->uiCtrl || CTRL_SECOND_PRIORITY == m_pRunData->uiCtrl)
	{
		if ( !IsLongStep(ucCurStepNo) )
		{
			ucElapseTime += m_ucAddTimeCnt;
		}		
	}

	if ( pTotalTime != NULL )
	{
		*pTotalTime = ucTotalTime;
	}

	if ( pElapseTime != NULL )
	{
		*pElapseTime = ucElapseTime;		
	}
}


/**************************************************************
Function:       CManaKernel::SetCycleStepInfo
Description:    �����������ڵĲ�����Ϣ��������Ϣ�洢��
				m_pRunData->sStageStepInfo��
Input:          ucLockPhase - ������λ��     
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::SetCycleStepInfo(Byte ucLockPhase)
{
	ACE_Guard<ACE_Thread_Mutex>  guard(m_mutexRunData);

	Byte ucStepIndex  = 0;
	Byte ucStageIndex = 0;

	ACE_OS::memset(m_pRunData->sStageStepInfo,0,sizeof(m_pRunData->sStageStepInfo));

	if ( ucLockPhase != 0 )  //������λ 
	{
		//����������λ�����������ڵĲ�����Ϣ..............
		m_pRunData->ucStepNo = 0;
		m_pRunData->ucStepNum = 1;
		m_pRunData->ucStageNo = 0;
		m_pRunData->ucStageCount = 1;
		m_pRunData->bOldLock = true;
		return;
	}

	//�������ò�����Ϣ
	for (ucStageIndex=0; ucStageIndex<MAX_SON_SCHEDULE; ucStageIndex++ )//MAX_SON_SCHEDULE
	{
 		if ( m_pRunData->ucScheduleTimeId == m_pRunData->sScheduleTime[ucStageIndex].ucId )  //ucScheduleTimeId  ucId�׶���ʱ���
 		{
			//����һ���ӽ׶ι��첽����
			SetStepInfoWithStage(ucStageIndex,&ucStepIndex,&(m_pRunData->sScheduleTime[ucStageIndex]));// ucStageIndex �ӽ׶κ�
			// ACE_DEBUG((LM_DEBUG,"%s:%d ucStageIncludeSteps= %d \n" ,__FILE__,__LINE__,m_pRunData->ucStageIncludeSteps[ucStageIndex]));
		}
		else
		{
			break;  //ȫ�����ӽ׶α������
		}
	}

	m_pRunData->ucStepNo     = 0;
	m_pRunData->ucStepNum    = ucStepIndex;
	m_pRunData->ucStageNo    = 0;
	m_pRunData->ucStageCount = ucStageIndex;
	m_pRunData->ucStepTime   = m_pRunData->sStageStepInfo[0].ucStepLen;
	m_pRunData->ucRunTime    = m_pRunData->ucStepTime;
	m_pRunData->ucElapseTime = 0;

	m_pRunData->ucCycle = 0;
	for ( int i=0; i<m_pRunData->ucStepNum; i++ )
	{
		m_pRunData->ucCycle += m_pRunData->sStageStepInfo[i].ucStepLen;
	}

	//ACE_DEBUG((LM_DEBUG,"%s:%d ucCycle= %d ucStepNum=%d ucStageCount=%d uc0StepTime= %d\n" ,__FILE__,__LINE__,m_pRunData->ucCycle ,m_pRunData->ucStepNum,m_pRunData->ucStageCount,m_pRunData->ucStepTime));
}


/**************************************************************
Function:       CManaKernel::GetOverlapPhaseIndex
Description:    ���ݰ�����λid��ȡȫ���ĸ�����λ
				m_pRunData->sStageStepInfo��
Input:          iPhaseId      - ������λid     
Output:         ucCnt         - ������λ����
*       	    pOverlapPhase - �洢����������λ
Return:         ��
***************************************************************/
void CManaKernel::GetOverlapPhaseIndex(Byte iPhaseId , Byte* ucCnt , Byte* pOverlapPhase)
{
	int iIndex           = 0;
	int jIndex           = 0;
	int iIncludePhaseCnt = 0;
	*ucCnt = 0;

	while ( iIndex < MAX_OVERLAP_PHASE )
	{
		if ( m_pTscConfig->sOverlapPhase[iIndex].ucId != 0 )
		{
			iIncludePhaseCnt = m_pTscConfig->sOverlapPhase[iIndex].ucIncludePhaseLen;
			jIndex = 0;
			while ( jIndex < iIncludePhaseCnt )
			{
				if ( iPhaseId == m_pTscConfig->sOverlapPhase[iIndex].ucIncludePhase[jIndex] )
				{
					pOverlapPhase[*ucCnt] = iIndex;
					*ucCnt += 1;
					break;
				}
				jIndex++;
			}
		}

		iIndex++;
	}
}


/**************************************************************
Function:       CManaKernel::OverlapPhaseToPhase
Description:   ���ݸ�����λ��ȡ������λindex				
Input:          iCurAllowPhase      - �׶η�����λ
*    		    ucOverlapPhaseIndex - ������λ�±� 0 1 ....7    
Output:       
Return:         return: ��Ӧ������λ��index 0 1 .....15
***************************************************************/
Byte CManaKernel::OverlapPhaseToPhase(Uint uiCurAllowPhase,Byte ucOverlapPhaseIndex)
{
	Byte ucIndex           = 0;
	Byte ucPhaseId         = 0;
	Byte ucIncludePhaseCnt = m_pTscConfig->sOverlapPhase[ucOverlapPhaseIndex].ucIncludePhaseLen;

	while ( ucIndex < ucIncludePhaseCnt )
	{
		ucPhaseId = m_pTscConfig->sOverlapPhase[ucOverlapPhaseIndex].ucIncludePhase[ucIndex] - 1;

		if ( (uiCurAllowPhase >> ucPhaseId) & 1 )
		{
			return ucPhaseId;
		}
		ucIndex++;
	}

	return MAX_PHASE+1;
}


/*********************************************************************************
Function:       CManaKernel::SetStepInfoWithStage
Description:    ����һ���ӽ׶����ò�����Ϣ			
Input:          ucStageIndex  : ��ǰ���ӽ׶��±�
*      	     ucCurStepIndex����ǰ�ۼ����õĲ�����,��sStageStepInfo���±�
*       		pScheduleTime : �ӽ׶���Ϣ��ָ�룬Ϊ��ǰ�ӽ׶κŶ�Ӧ�׶���ʱ�������
Output:         ucCurStepIndex: ��ǰ�ۼ����õĲ�����,��sStageStepInfo���±�
Return:         ��
**********************************************************************************/
void CManaKernel::SetStepInfoWithStage(Byte ucCurStageIndex, Byte* ucCurStepIndex,SScheduleTime* pScheduleTime)
{
	Byte ucPhaseIndex      = 0;
	Byte ucMinStepLen      = 200;
	Byte ucTmpStepLen      = 0;
	Byte ucSignalGrpNum    = 0;
	Byte ucSignalGrpIndex  = 0;
	Byte ucLampIndex       = 0;
	Byte ucCnt             = 0;
	Byte ucAllowPhaseIndex = 0;
	Byte *pPhaseColor      = NULL;
	Byte ucSignalGrp[MAX_CHANNEL]          = {0}; 
	Byte ucOverlapPhase[MAX_OVERLAP_PHASE] = {0};
	SPhaseStep sPhaseStep[MAX_PHASE+MAX_OVERLAP_PHASE];	
	ACE_OS::memset(sPhaseStep,0,(MAX_PHASE+MAX_OVERLAP_PHASE)*sizeof(SPhaseStep));
	for (Byte i=0; i<MAX_PHASE; i++ )
	{
		if(( pScheduleTime->usAllowPhase>>i ) & 0x1 )  //������λ���,��ȡ���ӽ׶η�����λ���̻ƺ�ʱ��
		{
			sPhaseStep[ucPhaseIndex].ucPhaseId     = i + 1;
			sPhaseStep[ucPhaseIndex].bIsAllowPhase = true;
			GetPhaseStepTime(i,pScheduleTime,sPhaseStep[ucPhaseIndex].ucStepTime,ucCurStageIndex); //��ȡ����λ�� �� ��ɾ �� ��ʱ��  4���ֽڱ�ʾ
			ucAllowPhaseIndex = ucPhaseIndex;
			//ACE_DEBUG((LM_DEBUG,"%s:%d PhaseId=%d  sPhaseStep[%d] = %d,Green=%d,GF = %d Yellow = %d Red = %d\n" ,__FILE__,__LINE__,i+1,ucPhaseIndex,sPhaseStep[ucPhaseIndex].ucPhaseId,sPhaseStep[ucPhaseIndex].ucStepTime[0],sPhaseStep[ucPhaseIndex].ucStepTime[1],sPhaseStep[ucPhaseIndex].ucStepTime[2],sPhaseStep[ucPhaseIndex].ucStepTime[3])); // ADD: 20130708 ���Ը�Ӧ������ʱ�䳤?
			ucPhaseIndex++;			
			//������λ���   READ:20130703 1423
			GetOverlapPhaseIndex(i+1 , &ucCnt , ucOverlapPhase);
			if ( ucCnt > 0 )  //����ͨ��λΪ������λ������������λ
			{
				int iIndex = 0;
				while ( iIndex < ucCnt )
				{
					if ( !ExitOvelapPhase(ucOverlapPhase[iIndex]+1,ucPhaseIndex,sPhaseStep) )
					{
						GetOverlapPhaseStepTime(  
							  ucCurStageIndex 
							, ucOverlapPhase[iIndex]
						    , sPhaseStep[ucAllowPhaseIndex].ucStepTime  
							, sPhaseStep[ucPhaseIndex].ucStepTime
							, i
							, pScheduleTime->ucYellowTime);
						sPhaseStep[ucPhaseIndex].ucPhaseId = ucOverlapPhase[iIndex] + 1;
						sPhaseStep[ucPhaseIndex].bIsAllowPhase = false;
					  //  ACE_DEBUG((LM_DEBUG,"%s:%d overPhaseId=%d  sPhaseStep[ucPhaseIndex] = %d,Green=%d,GF = %d Yellow = %d\n" ,__FILE__,__LINE__,ucOverlapPhase[iIndex] + 1,ucPhaseIndex,sPhaseStep[ucPhaseIndex].ucStepTime[0],sPhaseStep[ucPhaseIndex].ucStepTime[1],sPhaseStep[ucPhaseIndex].ucStepTime[2])); // ADD: 20130708 ���Ը�Ӧ������ʱ�䳤?

						ucPhaseIndex++;
					}
					iIndex++;
				}
			}
		}
	}// �õ����ӽ׶����з�����λ�� ����ʱ��

	//ACE_DEBUG((LM_DEBUG,"%s:%d ucCurStageIndex= %d ucPhaseIndex=%d \n" ,__FILE__,__LINE__,ucCurStageIndex,ucPhaseIndex));

	//�������е���λ ��sPhaseStepת��Ϊ������Ϣ
	m_pRunData->ucStageIncludeSteps[ucCurStageIndex] = 0;	

	for ( int i=0; i<ucPhaseIndex; i++ ) //ucPhaseIndex Ϊ������λ��
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d i=%d sPhaseStep[%d].phase=%d \n" ,__FILE__,__LINE__,i,i,sPhaseStep[i].ucPhaseId)); 
		if ( (ucTmpStepLen = GetPhaseStepLen(&sPhaseStep[i]) ) > 0 )
		{
			ucMinStepLen = (ucMinStepLen > ucTmpStepLen ? ucTmpStepLen : ucMinStepLen);
			//ACE_DEBUG((LM_DEBUG,"%s:%d i=%d ucMinStepLen = %d sPhaseStep[%d].phase=%d\n" ,__FILE__,__LINE__,i,ucMinStepLen,i,sPhaseStep[i].ucPhaseId)); 
		}
	
	   // ACE_DEBUG((LM_DEBUG,"%s:%d i=%d,ucMinStepLen= %d ucPhaseIndex=%d \n" ,__FILE__,__LINE__,i,ucMinStepLen,ucPhaseIndex));		
		if ( ( i + 1 ) ==  ucPhaseIndex )  //����1����������    �ﵽ�����λ��
		{
		//	ACE_DEBUG((LM_DEBUG,"%s:%d ��ʼ���첽������ \n" ,__FILE__,__LINE__));
			if ( 200 == ucMinStepLen )  //ȫ���������
			{
				// ACE_DEBUG((LM_DEBUG,"%s:%d ucStageIncludeSteps= %d \n" ,__FILE__,__LINE__,m_pRunData->ucStageIncludeSteps[ucCurStageIndex]));
				return;
			}

			Byte ucColorIndex = 0;
			m_pRunData->ucStageIncludeSteps[ucCurStageIndex]++;
			m_pRunData->sStageStepInfo[*ucCurStepIndex].ucStepLen = ucMinStepLen;				
			//ACE_DEBUG((LM_DEBUG,"%s:%d sStageStepInfo[%d].ucStepLen=%d !\n",__FILE__,__LINE__,*ucCurStepIndex,m_pRunData->sStageStepInfo[*ucCurStepIndex].ucStepLen ));

			for ( int j=0; j<ucPhaseIndex; j++ ) //����ÿ����λ
			{
				ucColorIndex = GetPhaseStepIndex(&sPhaseStep[j]);
				if ( 5 == ucColorIndex )
				{
					continue;
				}
				
				if ( sPhaseStep[j].bIsAllowPhase )
				{
					m_pRunData->sStageStepInfo[*ucCurStepIndex].uiAllowPhase |= 
						                  (1<<(sPhaseStep[j].ucPhaseId-1) & 0xffffffff ) ;
					pPhaseColor = m_pRunData->sStageStepInfo[*ucCurStepIndex].ucPhaseColor;
				}
				else
				{
					m_pRunData->sStageStepInfo[*ucCurStepIndex].uiOverlapPhase |= 
						                   (1<<(sPhaseStep[j].ucPhaseId-1) & 0xffffffff );
					pPhaseColor = m_pRunData->sStageStepInfo[*ucCurStepIndex].ucOverlapPhaseColor;
				}

				if ( pPhaseColor )
				{
					if ( 0 == ucColorIndex )  //green
					{
						pPhaseColor[sPhaseStep[j].ucPhaseId-1] = LAMP_COLOR_GREEN;
					}
					else if ( 1 == ucColorIndex ) //green flash
					{
						pPhaseColor[sPhaseStep[j].ucPhaseId-1] = LAMP_COLOR_GREEN;
					}
					else if ( 2 == ucColorIndex  ) //yellow
					{
						pPhaseColor[sPhaseStep[j].ucPhaseId-1] = LAMP_COLOR_YELLOW;
					}
					else  //red
					{
						pPhaseColor[sPhaseStep[j].ucPhaseId-1] = LAMP_COLOR_RED;
					}
				}

				ucSignalGrpNum = 0;
				GetSignalGroupId(sPhaseStep[j].bIsAllowPhase,sPhaseStep[j].ucPhaseId, &ucSignalGrpNum,ucSignalGrp);
				
				ucSignalGrpIndex = 0;
				while ( ucSignalGrpIndex < ucSignalGrpNum )
				{
					ucLampIndex = ( ucSignalGrp[ucSignalGrpIndex] - 1 ) * 3; //��λ���red�±�
					
					if ( 0 == ucColorIndex )  //green
					{
						//if(m_pRunData->ucStageNo ==0 && m_pRunData->ucStepNo==0) // test red green conflict
						//m_pRunData->sStageStepInfo[*ucCurStepIndex].ucLampOn[ucLampIndex] = 1; //TEST :2013 0717 11 40
						ucLampIndex = ucLampIndex + 2;
					}
					else if ( 1 == ucColorIndex ) //green flash
					{
						ucLampIndex = ucLampIndex + 2;
						m_pRunData->sStageStepInfo[*ucCurStepIndex].ucLampFlash[ucLampIndex] = 1;
					}
					else if ( 2 == ucColorIndex  ) //yellow
					{
						ucLampIndex = ucLampIndex + 1;
					}
					else if( 3 == ucColorIndex) //red
					{
						ucLampIndex = ucLampIndex ;
					}
					m_pRunData->sStageStepInfo[*ucCurStepIndex].ucLampOn[ucLampIndex] = 1;
					
					ucSignalGrpIndex++;
				}

				sPhaseStep[j].ucStepTime[ucColorIndex] -= ucMinStepLen;
				
			} //end for
			
			SetRedOtherLamp(m_pRunData->sStageStepInfo[*ucCurStepIndex].ucLampOn,m_pRunData->sStageStepInfo[*ucCurStepIndex].ucLampFlash);

			ucMinStepLen = 200;
			(*ucCurStepIndex)++;
			if ( *ucCurStepIndex > MAX_STEP )
			{
				return;
			}
			i = -1;  //���¿�ʼ������һ�ֲ���
			
		}
	
	}  //end for
	
}



/*********************************************************************************
Function:       CManaKernel::ExitOvelapPhase
Description:    ��ǰ�Ƿ��Ѵ��ڸø�����λ			
Input:          ucOverlapPhaseId  �� ��λ��
*      			ucPhaseCnt ����λ��
*       		pPhaseStep : ��λ������Ϣ
Output:         
Return:         true - ����  false - ������
**********************************************************************************/
bool CManaKernel::ExitOvelapPhase(Byte ucOverlapPhaseId,Byte ucPhaseCnt,SPhaseStep* pPhaseStep)
{
	for ( Byte ucIndex=0; ucIndex<ucPhaseCnt; ucIndex++ )
	{
		if ( !pPhaseStep[ucIndex].bIsAllowPhase && (ucOverlapPhaseId == pPhaseStep[ucIndex].ucPhaseId))
		{
			return true;
		}
	}
	return false;
}


/*********************************************************************************
Function:       CManaKernel::SetRedOtherLamp
Description:    ��δ���õ�ɫ��Ϣ���źŵ���ǿ������Ϊ�����			
Input:          ��
Output:         ucLampOn  - �źŵ���������ָ��
Return:         ��
**********************************************************************************/
void CManaKernel::SetRedOtherLamp(Byte* ucLampOn,Byte *ucLampFlash)
{
	bool bRed = false;
	for ( int i=0; i<MAX_LAMP; i=i+3 )
	{
		bRed = true;
		for (int j=i; j<(i+3); j++)
		{
			if ( 1 == ucLampOn[j] )        //�õ����б���λ
			{
				bRed = false;
				break;
			}
		}
		if ( bRed && IsInChannel(i/3+1) )  //�õ���(ͨ��)�б�����
		{
			if(m_pTscConfig->sChannel[i/3].ucFlashAuto == 0x2) //ͨ��ǿ�ƻ���,��ͨ����λû�н׶η���
			{
				ucLampOn[i+1] = 1;
				ucLampFlash[i+1] = 1;
			}
			else
			{
				ucLampOn[i] = 1;         	   //�õ���ǿ�������
			}
		}
	}
}


/*********************************************************************************
Function:       CManaKernel::ExitStageStretchPhase
Description:    �жϽ׶ε���λ���Ƿ���ڵ�����λ			
Input:          pScheduleTime - �ӽ׶���Ϣ
Output:         ��
Return:         true -������λ   false -�ǵ�����λ
**********************************************************************************/
bool CManaKernel::ExitStageStretchPhase(SScheduleTime* pScheduleTime)
{
	for (Byte ucIndex=0; ucIndex<MAX_PHASE; ucIndex++ )
	{
		if ( ( pScheduleTime->usAllowPhase>>ucIndex ) & 0x1 )  //������λ���
		{
			if ( (m_pTscConfig->sPhase[ucIndex].ucType>>5) & 0x1 )
			{
				return true;
			}
		}
	}
	return false;
}


/*********************************************************************************
Function:       CManaKernel::GetPhaseStepTime
Description:    ������λ�ţ���ȡ����λ���̡��������ơ�ȫ��ʱ��(��ͨ��λ)			
Input:          ucPhaseId:��λ��  pScheduleTime:�ӽ׶���Ϣ
Output:         pTime:����ʱ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetPhaseStepTime(Byte ucPhaseId,SScheduleTime* pScheduleTime,Byte* pTime,Byte ucCurStageIndex)
{
	bool bExitNextStage             = false;   
	Byte ucNextStageIndex           = ucCurStageIndex + 1;  //��һ���׶��±�
	Uint32 usNextAllowPhase = 0; 
	pTime[2] = pScheduleTime->ucYellowTime;    //yellow
	pTime[3] = pScheduleTime->ucRedTime;       //red        

	if ( /*( (m_pTscConfig->sPhase[ucPhaseId].ucType>>5) & 0x1 )*/
		ExitStageStretchPhase(pScheduleTime)
		&& ((CTRL_PREANALYSIS == m_pRunData->uiCtrl)|| (CTRL_VEHACTUATED == m_pRunData->uiCtrl)|| (CTRL_MAIN_PRIORITY == m_pRunData->uiCtrl )|| (CTRL_SECOND_PRIORITY == m_pRunData->uiCtrl)||(CTRL_ACTIVATE== m_pRunData->uiCtrl) ) 
		/*&& (m_sPhaseDet[ucPhaseId].iRoadwayCnt > 0)*/ )  //��Ӧ����  �׶ε���λ����ڵ�����λ 
	{
		//pTime[0] = m_pTscConfig->sPhase[ucPhaseId].ucMinGreen;  //min green
		pTime[0] = GetStageMinGreen(pScheduleTime->usAllowPhase);
		pTime[1] = m_pTscConfig->sPhase[ucPhaseId].ucGreenFlash; //green flash
	}
	else 
	{
		if ( 0 == pScheduleTime->ucGreenTime )  //�׶��̵�Ϊ0
		{
			pTime[1] = 0;  //green flash
			pTime[0] = 0;  //green
		}
		else if ( pScheduleTime->ucGreenTime > m_pTscConfig->sPhase[ucPhaseId].ucGreenFlash )
		{
			pTime[1] = m_pTscConfig->sPhase[ucPhaseId].ucGreenFlash;  //green flash
			pTime[0] = pScheduleTime->ucGreenTime - pTime[1];         //green
		}
		else
		{
			pTime[1] = m_pTscConfig->sPhase[ucPhaseId].ucGreenFlash;  //green flash
			pTime[0] = pScheduleTime->ucGreenTime;         //green
		}
	}
	if ( (m_pTscConfig->sPhase[ucPhaseId].ucOption>>1) & 0x1 )      //������λ
	{
		pTime[2] = 0;    //������λû�лƵ�
	}
	if ( ucNextStageIndex >= m_pRunData->ucStageCount )
	{
		ucNextStageIndex = 0;
	}
	usNextAllowPhase = m_pRunData->sScheduleTime[ucNextStageIndex].usAllowPhase;

	if((usNextAllowPhase>>ucPhaseId)&0x1)
	{
		pTime[0] = pTime[0]+pTime[1]+pScheduleTime->ucYellowTime+pTime[3];
		pTime[1]=0x0;
		pTime[2]=0x0;
		pTime[3]=0x0;
	}
	
	//ACE_DEBUG((LM_DEBUG,"%s:%d ucPhaseId=%d  green time=%d,gf time = %d yellow time = %d\n" ,__FILE__,__LINE__,ucPhaseId+1,pTime[0],pTime[1],pTime[2]));
}


/*********************************************************************************
Function:       CManaKernel::ExitStageStretchPhase
Description:    ��ȡ������λ���̡��������ơ�ȫ��ʱ��			
Input:           ucCurStageIndex  - ��ǰ�׶���ʱ�±�  
*      			 ucOverlapPhaseId - ������λ�±� 0 1 2 .... 7 
*				 pIncludeTime     - ������λ�İ�����λ���� ���� �� ȫ��ʱ��
Output:         pTime - �̡��������ơ�ȫ��ʱ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetOverlapPhaseStepTime(
											    Byte ucCurStageIndex
											  , Byte ucOverlapPhaseId 
											  , Byte* pIncludeTime
											  , Byte* pOverlapTime
											  , Byte ucPhaseIndex
											  , Byte ucStageYellow )
{
	bool bExitNextStage             = false;                //������λ������һ���׶���
	Byte ucCnt                      = 0;
	Byte ucNextStageIndex           = ucCurStageIndex + 1;  //��һ���׶��±�
	Uint32 usNextAllowPhase = 0;                    //��һ����λ��
	int  iIndex                     = 0;
	int  jIndex                     = 0;
	Byte ucOverlapPhase[MAX_OVERLAP_PHASE] = {0};

	if ( ucNextStageIndex >= m_pRunData->ucStageCount )
	{
		ucNextStageIndex = 0;
	}
	usNextAllowPhase = m_pRunData->sScheduleTime[ucNextStageIndex].usAllowPhase;	
	while ( iIndex < MAX_PHASE  ) 
	{
		if ( ( usNextAllowPhase >> iIndex ) & 1 )
		{
			GetOverlapPhaseIndex(iIndex+1 , &ucCnt , ucOverlapPhase);  //��ȡ��һ���׶η�����λ�ĸ�����λ
			if ( ucCnt > 0 )
			{
				jIndex = 0;
				while ( jIndex < ucCnt )
				{
					if ( ucOverlapPhaseId == ucOverlapPhase[jIndex] )
					{
						bExitNextStage = true;
						break;
					}
					jIndex++;
				}

			}
		}
		
		if ( bExitNextStage )
		{
			break;
		}

        iIndex++;
	}

	if ( bExitNextStage )   //������λ��������һ���׶�
	{
		pOverlapTime[0] = pIncludeTime[0] + pIncludeTime[1] + pIncludeTime[2] + pIncludeTime[3];  //green
		//�����ٸ���β���̵ơ�β���Ƶơ�β����� ����������չ���� .......
		pOverlapTime[1] = 0;  //green flash
		pOverlapTime[2] = 0;  //yellow
		pOverlapTime[3] = 0;  //red

		//if ( (m_pTscConfig->sPhase[ucPhaseIndex].ucOption>>1) & 0x1 )      //������λMOD:20150803 �����޸�
		// ������λ��ĸ��λ������������λ����������ͨ��λ����Ϊ�������е�
		//ʱ�����ĸ��λ�������������У�����������ӻƵ�ʱ�䡣
	//	{
		//pOverlapTime[0] += ucStageYellow;  //������λ ���ϻ�����ʱ��
	//	}
	}
	else  //������λ�ڸý׶���������������Ҫ���������Ĺ��Ȳ�
	{
		Byte ucTailGreen = 0 ;
		Byte ucTailYw = 0 ;
		Byte ucTailRed = 0 ;
		for(int iDex = 0 ; iDex<MAX_OVERLAP_PHASE;iDex++)
		{
			if((m_pTscConfig->sOverlapPhase)[iDex].ucId ==  ucOverlapPhaseId+1)
			{
				ucTailGreen = m_pTscConfig->sOverlapPhase[iDex].ucTailGreen ;
				ucTailYw =  m_pTscConfig->sOverlapPhase[iDex].ucTailYellow ;
				ucTailRed = m_pTscConfig->sOverlapPhase[iDex].ucTailRed ;
				//ACE_DEBUG((LM_DEBUG,"%s:%d ucTailGreen =%d ,ucTailYw = %d ,ucTailRed = %d  \n" ,__FILE__,__LINE__,ucTailGreen,ucTailYw,ucTailRed));
				break ;				
			}
		}
		if(ucTailGreen != 0)
		{
			pOverlapTime[0] = pIncludeTime[0]+pIncludeTime[1]+ucTailGreen;  //green
			pOverlapTime[1] = 0;  //green flash green flash
			pOverlapTime[2] = ucTailYw;  //yellow yellow
			pOverlapTime[3] = ucTailRed; //red red
		}
		else
		{
			pOverlapTime[0] = pIncludeTime[0];  //green
			pOverlapTime[1] = pIncludeTime[1];  //green flash
			pOverlapTime[2] = pIncludeTime[2];  //yellow
			pOverlapTime[3] = pIncludeTime[3]; //red

		}
	}
}


/*********************************************************************************
Function:       CManaKernel::GetPhaseStepLen
Description:    ��ȡ��λ��ĳ��������Ӧ��ȡ�ĳ���			
Input:          pPhaseStep:��λ������Ϣָ��
Output:         ��
Return:         ��λ��ȡ�����Ĵ�С
**********************************************************************************/
Byte CManaKernel::GetPhaseStepLen( SPhaseStep* pPhaseStep)
{
	int i = 0;
	while ( i < 4 )
	{
		//ACE_DEBUG((LM_DEBUG,"%s:%d i= %d pPhaseStep->ucStepTime[%d]=%d \n" ,__FILE__,__LINE__,i,i,pPhaseStep->ucStepTime[i]));
		if((i == 2) && (m_pTscConfig->sPhase[pPhaseStep->ucPhaseId -1].ucOption ==0x2 )) //�����������������λ
		{	
			//ACE_DEBUG((LM_DEBUG,"%s:%d return 0 when people phase! \n" ,__FILE__,__LINE__));
			return 0 ;
		}
		if ( pPhaseStep->ucStepTime[i] >0 )
		{
				//ACE_DEBUG((LM_DEBUG,"%s:%d i= %d pPhaseStep->ucStepTime[%d]=%d \n" ,__FILE__,__LINE__,i,i,pPhaseStep->ucStepTime[i]));
				//if(i == 2)
				//ACE_DEBUG((LM_DEBUG,"%s:%d ucOption= %d phase = %d \n" ,__FILE__,__LINE__,m_pTscConfig->sPhase[pPhaseStep->ucPhaseId -1].ucOption,pPhaseStep->ucPhaseId));
				
			return pPhaseStep->ucStepTime[i];
		}
		i++;
	}
	return 0;
}


/*********************************************************************************
Function:       CManaKernel::GetPhaseStepIndex
Description:    ������λ���±꣬���жϸ���λ�Ƿ��Ѿ�ȫ����ȡ���			
Input:          pPhaseStep:��λ������Ϣָ��
Output:         ��
Return:         0-3����ȡ�±�� 5:��ȡ���
**********************************************************************************/
Byte CManaKernel::GetPhaseStepIndex( SPhaseStep* pPhaseStep )
{
	int i = 0;

	while ( i < 4 )
	{
		if ( pPhaseStep->ucStepTime[i] >0 )  //0:green 1:green flash 2:yellow 3:red 
		{
			return i;
		}
		i++;
	}
	return 5;
}


/*********************************************************************************
Function:       CManaKernel::GetSignalGroupId
Description:    ������λ�Ż�ȡͨ��			
Input:          bIsAllowPhase - �Ƿ�Ϊ������λ false - ������λ
*        		ucPhaseId     - ��λID 
*       		pNum          - ͨ������ 
*       		pSignalGroup  - ����ͨ����
Output:         pNum:ͨ������ pSignalGroup:����ͨ����
Return:         0-3����ȡ�±�� 5:��ȡ���
**********************************************************************************/
void CManaKernel::GetSignalGroupId(bool bIsAllowPhase , Byte ucPhaseId,Byte* pNum,Byte* pSignalGroup)
{
	*pNum = 0;
	for (int i=0; i<MAX_CHANNEL; i++ )
	{
		if ( 0 == m_pTscConfig->sChannel[i].ucSourcePhase)
		{
			continue;
		}
		else 
		{
			if ( m_pTscConfig->sChannel[i].ucSourcePhase == ucPhaseId )
			{
				if ( ( bIsAllowPhase && ( CHANNEL_OVERLAP == m_pTscConfig->sChannel[i].ucType ) ) 
					|| ( !bIsAllowPhase && ( CHANNEL_OVERLAP != m_pTscConfig->sChannel[i].ucType ) ) )
				{
					continue;
				}

				if ( *pNum > MAX_CHANNEL )
				{
					continue;
				}
				pSignalGroup[*pNum] = m_pTscConfig->sChannel[i].ucId;
				(*pNum)++;
			}
		}
		
	}
}


/*********************************************************************************
Function:       CManaKernel::SwitchStatus
Description:    �źŻ�����״̬�л�			
Input:          uiWorkStatus - �¹���״̬
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::SwitchStatus(unsigned int uiWorkStatus)
{
	ACE_DEBUG((LM_DEBUG,"%s:%d SwitchStatus new WorkStatus:%d,Old WorkStatus:%d\n",__FILE__,__LINE__,uiWorkStatus,m_pRunData->uiWorkStatus));
	//m_pRunData->uiWorkStatus = m_pRunData->uiOldWorkStatus;
	if ( uiWorkStatus == m_pRunData->uiWorkStatus )
	{
		return;
	}
	if ( ( SIGNALOFF == m_pRunData->uiWorkStatus ) && ( STANDARD == uiWorkStatus ) )  //�ص��л�����׼ ��Ҫ��������
	{
		m_pRunData->uiOldWorkStatus = SIGNALOFF ;
		m_pRunData->uiWorkStatus = FLASH;
	}
	else if ( (FLASH == m_pRunData->uiWorkStatus) && ( STANDARD == uiWorkStatus ) )  //�����л�����׼ ��Ҫ����ȫ�� 
	{
		m_bWaitStandard          = true;
		m_pRunData->uiOldWorkStatus = FLASH ;
		m_pRunData->uiWorkStatus = ALLRED;
	}
	else
	{
		m_pRunData->uiOldWorkStatus = m_pRunData->uiWorkStatus ;
		m_pRunData->uiWorkStatus = uiWorkStatus;
	}
	 /*ȥ���ֶ�ʱ��Ҫ���¼���ͨ����Ϣ��һϵ����Ϣ*/
	m_pRunData->bNeedUpdate = true;  
	ResetRunData(0);  //���������»�ȡ��̬����
	CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn	,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
	CTscKernelGb25280::CreateInstance()->SelfReportLampGroupColor();  //���͵�ɫ�ı���Ϣ

}


/*********************************************************************************
Function:       CManaKernel::SwitchCtrl
Description:    �źŻ�����ģʽ�л�		
Input:          uiCtrl - �¿���ģʽ
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::SwitchCtrl(unsigned int uiCtrl)
{
	if ( uiCtrl == m_pRunData->uiCtrl )
	{
		return;
	}	
	ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** New Ctrl:%d,Old Ctrl:%d\n",__FILE__,__LINE__,uiCtrl,m_pRunData->uiCtrl));

	if(uiCtrl == CTRL_SCHEDULE && m_pRunData->uiCtrl == CTRL_VEHACTUATED)
	{
		m_pRunData->bNeedUpdate = true;  //��Ҫ�ض����ݿ�����̲�ʱ��
	}
	if ( (CTRL_MANUAL == m_pRunData->uiCtrl) && m_bSpePhase )  //��ǰ�Ŀ��Ʒ�ʽΪ�ֶ��ض���λ��׼��ȥ���ֶ�
	{
		SThreadMsg sTscMsgSts;
		sTscMsgSts.ulType             = TSC_MSG_SWITCH_STATUS;  
		sTscMsgSts.ucMsgOpt           = 0;
		sTscMsgSts.uiMsgDataLen       = 1;
		sTscMsgSts.pDataBuf           = ACE_OS::malloc(1);
		*((Byte*)sTscMsgSts.pDataBuf) = FLASH;  //����
		CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsgSts,sizeof(sTscMsgSts));
	}	

	if((CTRL_PREANALYSIS == uiCtrl)||(CTRL_VEHACTUATED == uiCtrl) || (CTRL_MAIN_PRIORITY == uiCtrl) || (CTRL_SECOND_PRIORITY == uiCtrl) || (CTRL_ACTIVATE == uiCtrl)) 
	{	
		if ( !CDetector::CreateInstance()->HaveDetBoard())   //MOD: 201309231450
		{				
			bDegrade = true ;
			if(m_pRunData->uiCtrl == CTRL_SCHEDULE )         //��ֹ�ظ�д����Ʒ�ʽ������־
			{			
				return ;
			}
			else
			{
				uiCtrl    = CTRL_SCHEDULE;
			}
			CMainBoardLed::CreateInstance()->DoModeLed(true,true);								
			ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** when no DecBoard ,new uiCtrl = %d \n" ,__FILE__,__LINE__,uiCtrl));
		}
		else if ( CTRL_ACTIVATE != uiCtrl )
		{
			m_pRunData->bNeedUpdate = true;   //��Ҫ��ȡ�������Ϣ
			ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** CTRL_VEHACTUATED == uiCtrl,m_pRunData->bNeedUpdate == true \n" ,__FILE__,__LINE__));
			if((m_pRunData->uiCtrl == CTRL_SCHEDULE) && bDegrade == true )
			{
				bDegrade = false ;
				CMainBoardLed::CreateInstance()->DoModeLed(false,true);			
			}
			//ACE_DEBUG((LM_DEBUG,"%s:%d CTRL_VEHACTUATED == uiCtrl,m_pRunData->bNeedUpdate == true \n" ,__FILE__,__LINE__));
		}
	}
	if(CTRL_UTCS== uiCtrl && bDegrade == true && bUTS == false)//����Ѿ�������������ʱ�α���Ʒ�ʽ������������Ӧ
	{
		return ;
	}
	if ( uiCtrl == CTRL_LAST_CTRL )  //�ϴο��Ʒ�ʽ
	{		
		unsigned int uiCtrlTmp = m_pRunData->uiCtrl ;
		m_pRunData->uiCtrl      = m_pRunData->uiOldCtrl;
		//m_pRunData->uiOldCtrl   = uiCtrlTmp;
		m_pRunData->uiOldCtrl   = uiCtrlTmp;
		ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** uiCtrl = %d ,old_ctrl =%d new_ctrl = %d \n" ,__FILE__,__LINE__,uiCtrl,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl));
	}
	else
	{
		m_pRunData->uiOldCtrl = m_pRunData->uiCtrl;
		m_pRunData->uiCtrl    = uiCtrl;
		ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** m_pRunData->uiOldCtrl =%d m_pRunData->uiCtrl = %d \n" ,__FILE__,__LINE__,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl));
	}

	if ( uiCtrl == CTRL_PANEL || uiCtrl == CTRL_MANUAL )               //������л����ֶ��������������ͺ�������ʱ
	{             
		if ( m_pTscConfig->sSpecFun[FUN_COUNT_DOWN].ucValue != COUNTDOWN_STUDY && m_pTscConfig->sSpecFun[FUN_COUNT_DOWN].ucValue != COUNTDOWN_FLASHOFF)
			CGaCountDown::CreateInstance()->sendblack();
	}
	
	if ( CTRL_PANEL == m_pRunData->uiOldCtrl && m_iTimePatternId != 0)  //MOD:20130928 ������жϴ������ƶ�������
	{		
		m_iTimePatternId = 0;
		ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** After CTRL_PANEL to auto m_iTimePatternId = %d \n" ,__FILE__,__LINE__,m_iTimePatternId));
	}
	ACE_DEBUG((LM_DEBUG,"%s:%d***SwitchCtrl*** oldctrl = %d newctrl = %d\n"	,__FILE__,__LINE__,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl));
	SndMsgLog(LOG_TYPE_OTHER,1,m_pRunData->uiOldCtrl,m_pRunData->uiCtrl,0); //��־��¼���Ʒ�ʽ�л� ADD?201311041530
	
	CTscKernelGb25280::CreateInstance()->SelfReportWorkStatus();  //���͹���״̬�ı�
}



/*********************************************************************************
Function:       CManaKernel::SetUpdateBit
Description:    �������ݿ��������Ϊ��		
Input:          uiCtrl - �¿���ģʽ
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::SetUpdateBit()
{
#ifndef NO_CHANGEBY_DATABASE
	m_pRunData->bNeedUpdate = true;
#endif
}

/*********************************************************************************
Function:       CManaKernel::ChangePatter
Description:    �л��źŻ�����ʱ�����������л������ⷽ��
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::ChangePatter(Byte iParama)
{
	//if(m_iTimePatternId == 250) //���ⷽ��-�ķ������
	//{
		 SetDirecChannelColor(iParama) ; //������������̵Ʒ��У�����������ͨ��
	//}
		
}


/*********************************************************************************
Function:       CManaKernel::LockStage
Description:    �����׶�
Input:          ucStage - �׶κ�
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::LockStage(Byte ucStage)
{
	Byte ucCurStepNo = m_pRunData->ucStepNo;
	int iMinGreen    = 0;
	if( ucStage < 0 || ucStage >= m_pRunData->ucStageCount)
	{
		return;
	}
	ACE_DEBUG((LM_DEBUG,"%s:%d ucStage:%d m_pRunData->ucStageNo:%d LongStep:%d\n" , __FILE__,__LINE__,ucStage , m_pRunData->ucStageNo , IsLongStep(m_pRunData->ucStepNo) ));
	m_bSpePhase = false;
	if ( ucStage == m_pRunData->ucStageNo && (IsLongStep(m_pRunData->ucStepNo)) )  //�����׶ξ��ǵ�ǰ�׶Σ��ҽ׶δ����̵ƽ׶�
	{
		return;
	}
	//��֤������С��
	if(m_iTimePatternId == 0)
	{
		if ( IsLongStep(m_pRunData->ucStepNo) )
		{
			iMinGreen = GetMaxStageMinGreen(m_pRunData->ucStepNo);
			if ( m_pRunData->ucElapseTime < iMinGreen )
			{
				//return;
				ACE_OS::sleep(iMinGreen-m_pRunData->ucElapseTime);  //������С��
			}
		}
		
		//���ɲ�
		while (true)
		{
			m_pRunData->ucStepNo++;
			if ( m_pRunData->ucStepNo >= m_pRunData->ucStepNum )
			{
				m_pRunData->ucStepNo = 0;
			}

			if ( !IsLongStep(m_pRunData->ucStepNo) )  //�ǳ���
			{
				m_pRunData->ucElapseTime = 0;
				m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				m_pRunData->ucStageNo = StepToStage(m_pRunData->ucStepNo,NULL); //���ݲ����Ż�ȡ�׶κ�

				CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn
					,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
				ACE_OS::sleep(m_pRunData->ucStepTime);
				//while ( m_pRunData->ucElapseTime < m_pRunData->ucStepTime )
				//{
					//ACE_OS::printf("%s:%d ucElapseTime=%d ucStepTime=%d \r\n",__FILE__,__LINE__,m_pRunData->ucElapseTime,m_pRunData->ucStepTime);
					//ACE_OS::sleep(ACE_Time_Value(0,500));
					//continue;
					//ACE_OS::sleep(iMinGreen-m_pRunData->ucElapseTime);  //������С��
				//}

			}
			else
			{
				
				ACE_OS::printf("%s:%d Go over guodubu \r\n",__FILE__,__LINE__);
				break;
			}

			if ( ucCurStepNo == m_pRunData->ucStepNo )
			{
				break;
			}
		}	
	}
	m_pRunData->ucStageNo = ucStage;
	m_pRunData->ucStepNo = StageToStep(m_pRunData->ucStageNo);  //���ݽ׶κŻ�ȡ������
	m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
	m_pRunData->ucElapseTime = 0;
	m_pRunData->ucRunTime = m_pRunData->ucStepTime;

	CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn
		,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
	m_bNextPhase = false ;
}


/*********************************************************************************
Function:       CManaKernel::LockNextStage
Description:    �л�����һ���׶�����
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::LockNextStage()
{	
	int iCurStageNo = m_pRunData->ucStageNo;	
	SThreadMsg sTscMsgSts;	
	m_bSpePhase = false; 
	iCurStageNo++;

	if ( iCurStageNo >= m_pRunData->ucStageCount )
	{
		iCurStageNo  = 0;
	}

	ACE_DEBUG((LM_DEBUG,"%s:%d iCurStageNo:%d cnt:%d\n",__FILE__,__LINE__,iCurStageNo,m_pRunData->ucStageCount));

	ACE_OS::memset( &sTscMsgSts , 0 , sizeof(SThreadMsg) );
	sTscMsgSts.ulType       = TSC_MSG_LOCK_STAGE;  
	sTscMsgSts.ucMsgOpt     = 0;
	sTscMsgSts.uiMsgDataLen = 1;
	sTscMsgSts.pDataBuf     = ACE_OS::malloc(1);
	*((Byte*)sTscMsgSts.pDataBuf) = iCurStageNo; 
	CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsgSts,sizeof(sTscMsgSts));

}


/*********************************************************************************
Function:       CManaKernel::LockStep
Description:    ��������
Input:          ucStep - 0:��һ�� other:ָ������
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::LockStep(Byte ucStep)
{
	//��֤������С��
	int iMinGreen = 0;

	if ( STANDARD != m_pRunData->uiWorkStatus )  //������ȫ���Լ��ص�û����һ��
	{
		return;
	}
	
	m_bSpePhase   = false; 

	if ( IsLongStep(m_pRunData->ucStepNo) )
	{
		iMinGreen = GetMaxStageMinGreen(m_pRunData->ucStepNo);

		if ( m_pRunData->ucElapseTime < iMinGreen )
		{
			return;
		}
	}
		
	if ( 0 == ucStep )
	{
		m_pRunData->ucStepNo++;
		
		//ACE_DEBUG((LM_DEBUG,"*******%d %d \n",m_pRunData->ucStepNo,m_pRunData->ucStepNum));

		if ( m_pRunData->ucStepNo >= m_pRunData->ucStepNum )
		{
			m_pRunData->ucStepNo  = 0;
		}
	}
	else if ( (ucStep>0) && (ucStep<m_pRunData->ucStepNum) )
	{
		m_pRunData->ucStepNo = ucStep - 1;
	}

	//�����еĲ�������ټ�����һ��
	m_pRunData->ucStepTime = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
	m_pRunData->ucElapseTime = 0;
	m_pRunData->ucRunTime = m_pRunData->ucStepTime;
	m_pRunData->ucStageNo = StepToStage(m_pRunData->ucStepNo,NULL); //���ݲ����Ż�ȡ�׶κ�

	CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn	,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
	
}


/*********************************************************************************
Function:       CManaKernel::LockStep
Description:    ������λ����
Input:          PhaseId  - ������λ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::LockPhase(Byte PhaseId)
{
	int iMinGreen         = 0;
	Byte ucLampIndex      = 0;
	Byte ucSignalGrpNum   = 0;
	Byte ucSignalGrpIndex = 0;
	Byte ucSignalGrp[MAX_CHANNEL] = {0};

	if ( !m_bSpePhase )
	{
		//������֤������С��
		if ( IsLongStep(m_pRunData->ucStepNo) )
		{
			iMinGreen = GetMaxStageMinGreen(m_pRunData->ucStepNo);
			if ( m_pRunData->ucElapseTime < iMinGreen )
			{
#ifndef WINDOWS
				sleep(iMinGreen-m_pRunData->ucElapseTime); 
#endif
			}
		}

		//���ɲ�
		while ( true )
		{
			m_pRunData->ucStepNo++;
			if ( m_pRunData->ucStepNo >= m_pRunData->ucStepNum )
			{
				m_pRunData->ucStepNo = 0;
			}
			if ( !IsLongStep(m_pRunData->ucStepNo) )  //�ǳ���
			{
				m_pRunData->ucElapseTime = 0;
				m_pRunData->ucStepTime   = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucStepLen;
				m_pRunData->ucStageNo    = StepToStage(m_pRunData->ucStepNo,NULL); //���ݲ����Ż�ȡ�׶κ�

				CLampBoard::CreateInstance()->SetLamp(m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn
					,m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash);
#ifndef WINDOWS
				sleep(m_pRunData->ucStepTime);
#endif
			}
			else
			{
				m_pRunData->ucStepNo--;
				break;
			}
		}
	}

	m_bSpePhase = true;
	ACE_OS::memset(m_ucLampOn , 0 , MAX_LAMP);
	ACE_OS::memset(m_ucLampFlash , 0 , MAX_LAMP);

	GetSignalGroupId(true , PhaseId , &ucSignalGrpNum , ucSignalGrp);

	ucSignalGrpIndex = 0;
	while ( ucSignalGrpIndex < ucSignalGrpNum )  //red yellow green
	{
		ucLampIndex = ( ucSignalGrp[ucSignalGrpIndex] - 1 ) * 3; //��λ���red�±�
		ucLampIndex = ucLampIndex + 2;  //green
		m_ucLampOn[ucLampIndex] = 1;
		ucSignalGrpIndex++;
	}
	SetRedOtherLamp(m_ucLampOn,m_ucLampFlash);
	CLampBoard::CreateInstance()->SetLamp(m_ucLampOn,m_ucLampFlash);
}


/*********************************************************************************
Function:       CManaKernel::ReadTscEvent
Description:    �¼���ȡ
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::ReadTscEvent()
{
	//������
}


/*********************************************************************************
Function:       CManaKernel::ReadTscEvent
Description:    �źŻ�Уʱ�ӿ�,�����ֶ�Уʱ��GPSУʱ����
Input:          ucType - Уʱ����
				pValue - ʱ��ֵָ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::CorrectTime(Byte ucType,Byte* pValue)
{
	int iIndex = 0;
	int fd     = 0;
	Ulong iTotalSec = 0;
	ACE_Time_Value tvCurTime;
	ACE_Date_Time  tvDateTime;

	while ( iIndex < 4 )
	{
		iTotalSec |=  ((pValue[iIndex]&0xff)<<(8*(3-iIndex)));
		iIndex++;
	}  
//	ACE_OS::printf("\r\n\r\n%s:%d iTotalSec=%d  %02x %02x %02x %02x \r\n",__FILE__,__LINE__,iTotalSec,pValue[0],pValue[1],pValue[2],pValue[3]);
	if ( ucType == OBJECT_UTC_TIME) //utcʱ��
	{
		iTotalSec = iTotalSec - 8 * 3600;
	}
	
	tvCurTime.sec(iTotalSec);
	tvDateTime.update(tvCurTime);
	
	if (!IsRightDate((Uint)tvDateTime.year(),(Byte)tvDateTime.month(),(Byte)tvDateTime.day()))
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d error date:%d-%d-%d\n",__FILE__,__LINE__,tvDateTime.year(),tvDateTime.month(),tvDateTime.day()));
		return;
	}

	CTimerManager::CreateInstance()->CloseAllTimer();

	struct tm now;
	time_t ti = iTotalSec;
	now.tm_year = tvDateTime.year();
	now.tm_mon  = tvDateTime.month();
	now.tm_mday = tvDateTime.day();
	now.tm_hour = tvDateTime.hour();
	now.tm_min  = tvDateTime.minute();
	now.tm_sec  = tvDateTime.second();

	now.tm_year -= 1900;
	now.tm_mon--;
	now.tm_zone = 0;	
	ti = mktime(&now);
	stime(&ti);   //����ϵͳʱ��	
	fd = open(DEV_RTC, O_WRONLY, 0);
	if(fd > 0)
	{
		ioctl(fd, RTC_SET_TIME, &now);
		close(fd);
	}
	else
	{
		ACE_OS::printf("%s:%d open RTC error, cant write time to RTC!\r\n",__FILE__,__LINE__);
	}
	
	CTimerManager::CreateInstance()->StartAllTimer();
	SetUpdateBit();
	if(m_pRunData->uiCtrl == CTRL_WIRELESS) //��������Э������
		CWirelessCoord::CreateInstance()->ForceAssort();
	
	SThreadMsg sTscMsg;
	sTscMsg.ulType       = TSC_MSG_LOG_WRITE;
	sTscMsg.ucMsgOpt     = LOG_TYPE_CORRECT_TIME;
	sTscMsg.uiMsgDataLen = 4;
	sTscMsg.pDataBuf     = ACE_OS::malloc(4);
	iTotalSec += 8*3600 ;
	((Byte*)(sTscMsg.pDataBuf))[3] = iTotalSec &0xff ;
	((Byte*)(sTscMsg.pDataBuf))[2] = (iTotalSec>>8) &0xff ;
	((Byte*)(sTscMsg.pDataBuf))[1] = (iTotalSec>>16) &0xff ;
	((Byte*)(sTscMsg.pDataBuf))[0] = (iTotalSec>>24) &0xff ;
	CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));
}


/*********************************************************************************
Function:       CManaKernel::GetTscStatus
Description:    �źŻ�״̬��ȡ
Input:          ucDealDataIndex - ������ͻ����±�
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetTscStatus(Byte ucDealDataIndex)
{
	GBT_DB::TblDetector tblDetector;
	SThreadMsg sGbtMsg;
	sGbtMsg.ulType       = GBT_MSG_TSC_STATUS;
	sGbtMsg.ucMsgOpt     = ucDealDataIndex;
	sGbtMsg.uiMsgDataLen = sizeof(STscStatus);
	sGbtMsg.pDataBuf     = ACE_OS::malloc(sizeof(STscStatus));
	ACE_OS::memset(sGbtMsg.pDataBuf,0,sizeof(STscStatus));
	
	((STscStatus*)(sGbtMsg.pDataBuf))->bStartFlash   = m_pRunData->bStartFlash;
	((STscStatus*)(sGbtMsg.pDataBuf))->uiWorkStatus  = m_pRunData->uiWorkStatus;
	((STscStatus*)(sGbtMsg.pDataBuf))->uiCtrl        = m_pRunData->uiCtrl;
	((STscStatus*)(sGbtMsg.pDataBuf))->ucStepNo      = m_pRunData->ucStepNo + 1;
	((STscStatus*)(sGbtMsg.pDataBuf))->ucStageNo     = m_pRunData->ucStageNo + 1;
	((STscStatus*)(sGbtMsg.pDataBuf))->ucActiveSchNo = m_pRunData->ucScheduleId;

	((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarm2    = 0;
	if ( m_bRestart )
	{
		((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarm2 |= (1<<5);  /*�źŻ�ֹͣ����*/
	}

	((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarm1    = 0;
	if ( FLASH == m_pRunData->uiWorkStatus )
	{
		((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarm1 |= (1<<5);   /*�����������*/  
	}
	if ( CTRL_UTCS != m_pRunData->uiCtrl )
	{
		((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarm1 |= (1<<6);   /*���ص������*/  
	}

	/*
	ACE_DEBUG((LM_DEBUG,"@@@@@@@@@@@@@@ %d %d %d",m_pRunData->uiWorkStatus,m_pRunData->uiCtrl
		,((STscStatus*)(sGbtMsg.pDataBuf))->ucTscWarn1));
		*/

	((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarmSummary  = 0;
	if ( CDetector::CreateInstance()->IsDetError() )
	{
		((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarmSummary |= (1<<5); /*���������*/
	}
	if ( m_bRestart )
	{
		((STscStatus*)(sGbtMsg.pDataBuf))->ucTscAlarmSummary |= (1<<7); /*�źŻ�����ֹͣ����*/
	}

	/*���������*/
	/*
	((STscStatus*)(sGbtMsg.pDataBuf))->ucActiveDetCnt = tblDetector.GetCount();
	GetDetectorSts(((STscStatus*)(sGbtMsg.pDataBuf))->sDetSts);
	GetDetectorData(((STscStatus*)(sGbtMsg.pDataBuf))->sDetData);
	GetDetectorAlarm(((STscStatus*)(sGbtMsg.pDataBuf))->sDetAlarm);
	*/
	((STscStatus*)(sGbtMsg.pDataBuf))->ucActiveDetCnt =  CDetector::CreateInstance()->GetActiveDetSum();
	CDetector::CreateInstance()->GetDetStatus(((STscStatus*)(sGbtMsg.pDataBuf))->sDetSts);
	CDetector::CreateInstance()->GetDetData(((STscStatus*)(sGbtMsg.pDataBuf))->sDetData);
	CDetector::CreateInstance()->GetDetAlarm(((STscStatus*)(sGbtMsg.pDataBuf))->sDetAlarm);

	GetCurStageLen(((STscStatus*)(sGbtMsg.pDataBuf))->ucCurStageLen);
	GetCurKeyGreenLen(((STscStatus*)(sGbtMsg.pDataBuf))->ucCurKeyGreen);

	GetPhaseStatus(((STscStatus*)(sGbtMsg.pDataBuf))->sPhaseSts);
	GetOverlapPhaseStatus(& (((STscStatus*)(sGbtMsg.pDataBuf))->sOverlapPhaseSts) );
	GetChannelStatus(((STscStatus*)(sGbtMsg.pDataBuf))->sChannelSts);
	CGbtMsgQueue::CreateInstance()->SendGbtMsg(&sGbtMsg,sizeof(SThreadMsg));
}


/*********************************************************************************
Function:       CManaKernel::GetTscStatus
Description:    �źŻ���չ״̬��ȡ
Input:          ucDealDataIndex - ������ͻ����±�
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetTscExStatus(Byte ucDealDataIndex)
{
	Byte ucTmp             = 0;
	Byte ucStageTotalTime  = 0;
	Byte ucStageElapseTime = 0;
	Byte ucChanIndex       = 0;
	Ulong ulChanOut[4]     = {0,0,0,0};
	Byte pLampOn[MAX_LAMP]    ={0}; 
	Byte pLampFlash[MAX_LAMP] = {0}; 
	SThreadMsg sGbtMsg;
	sGbtMsg.ulType         = GBT_MSG_TSC_EXSTATUS;
	sGbtMsg.ucMsgOpt       = ucDealDataIndex;
	sGbtMsg.uiMsgDataLen   = 28;
	sGbtMsg.pDataBuf       = ACE_OS::malloc(28);
	ACE_OS::memset(sGbtMsg.pDataBuf,0,28);

	//((Byte*)sGbtMsg.pDataBuf)[0] = 1;

	ucTmp |= (m_pRunData->ucWorkMode      & 0x3);  //bit0-bit1
	ucTmp |= (m_pRunData->uiWorkStatus<<2 & 0xC);  //bit2-bit3
	ucTmp |= (m_pRunData->uiCtrl<<4       & 0xF0); //bit4-bit7
	((Byte*)sGbtMsg.pDataBuf)[0] = ucTmp;

	//ACE_DEBUG((LM_DEBUG,"m_pRunData->uiCtrl:%d ucTmp:%d\n",m_pRunData->uiCtrl,ucTmp));

	((Byte*)sGbtMsg.pDataBuf)[1] = m_pRunData->ucScheduleId;
	((Byte*)sGbtMsg.pDataBuf)[2] = m_pRunData->ucTimePatternId;
	((Byte*)sGbtMsg.pDataBuf)[3] = m_pRunData->ucScheduleTimeId;
	((Byte*)sGbtMsg.pDataBuf)[4] = m_pRunData->ucStageCount;
	((Byte*)sGbtMsg.pDataBuf)[5] = m_pRunData->ucStageNo + 1;
	 GetStageTime(&ucStageTotalTime,&ucStageElapseTime);
	((Byte*)sGbtMsg.pDataBuf)[6] = ucStageTotalTime;   //�׶���ʱ��
	((Byte*)sGbtMsg.pDataBuf)[7] = ucStageElapseTime;  //�׶��Ѿ�����ʱ��

	if ( m_bSpePhase )  //ָ����λ
	{
		//pLampOn    = m_ucLampOn;
		//pLampFlash = m_ucLampFlash;
		ACE_OS::memcpy(pLampOn,m_ucLampOn,MAX_LAMP);
		ACE_OS::memcpy(pLampFlash,m_ucLampFlash,MAX_LAMP);
	}
	else
	{
		//pLampOn    = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampOn;
		//pLampFlash = m_pRunData->sStageStepInfo[m_pRunData->ucStepNo].ucLampFlash;
		 CLampBoard::CreateInstance()->GetLamp(pLampOn,pLampFlash);
	}

	//���ͨ��״̬
	for ( int i=0; i<MAX_LAMP; i++ )
	{
		ucChanIndex = i / 3;  

		if ( pLampOn[i] == 0 )  //�ǵ���
		{
			continue;
		}
		switch ( i%3 )
		{
			case LAMP_COLOR_RED:
				ulChanOut[0] |= 1<<ucChanIndex;
				break;
			case LAMP_COLOR_YELLOW:
				ulChanOut[1] |= 1<<ucChanIndex;
				break;
			case LAMP_COLOR_GREEN:
				ulChanOut[2] |= 1<<ucChanIndex;
				break;
			default:
				break;
		}

		if ( 1 == pLampFlash[i] )  //ͨ����˸���״̬
		{
			ulChanOut[3] |= 1<<ucChanIndex;
		}
	}

	ACE_OS::memcpy((Byte*)(sGbtMsg.pDataBuf)+8 , ulChanOut , 16);
	((Byte*)sGbtMsg.pDataBuf)[24] = m_pRunData->ucCycle;
	((Byte*)sGbtMsg.pDataBuf)[25] = m_pRunData->ucPlanId;
	((Byte*)sGbtMsg.pDataBuf)[26] = m_ucStageDynamicMinGreen[m_pRunData->ucStageNo];
	((Byte*)sGbtMsg.pDataBuf)[27] = m_ucStageDynamicMaxGreen[m_pRunData->ucStageNo];
	// ACE_OS::printf("%s:%d m_ucStageDynamicMinGreen[%d] = %d m_ucStageDynamicMaxGreen[%d] = %d\r\n",__FILE__,__LINE__,m_pRunData->ucStageNo+1,
	 //	m_ucStageDynamicMinGreen[m_pRunData->ucStageNo],m_pRunData->ucStageNo+1,m_ucStageDynamicMaxGreen[m_pRunData->ucStageNo]);
	CGbtMsgQueue::CreateInstance()->SendGbtMsg(&sGbtMsg,sizeof(SThreadMsg));
}


/*********************************************************************************
Function:       CManaKernel::GetCurStageLen
Description:    ��ȡ��ǰ�������׶�ʱ��
Input:          
Output:         pCurStageLen - �׶�ʱ��ָ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetCurStageLen(Byte* pCurStageLen)
{
	Byte ucIndex = 0;
	for (Byte ucStageIndex=0; ucStageIndex<MAX_SON_SCHEDULE; ucStageIndex++ )
	{
		if ( m_pRunData->sScheduleTime[ucStageIndex].ucId != 0 )
		{
			pCurStageLen[ucIndex++] = m_pRunData->sScheduleTime[ucStageIndex].ucRedTime 
										+ m_pRunData->sScheduleTime[ucStageIndex].ucGreenTime
										+ m_pRunData->sScheduleTime[ucStageIndex].ucYellowTime;
		}
	}
}


/*********************************************************************************
Function:       CManaKernel::GetCurKeyGreenLen
Description:    ��ǰ�������ؼ���λ�̵�ʱ��
Input:          
Output:         pCurKeyGreen - �ؼ���λʱ������ָ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetCurKeyGreenLen(Byte* pCurKeyGreen)
{
	Byte ucIndex = 0;
	for (Byte ucStageIndex=0; ucStageIndex<MAX_SON_SCHEDULE; ucStageIndex++ )
	{
		if ( m_pRunData->sScheduleTime[ucStageIndex].ucId != 0 )
		{
			pCurKeyGreen[ucIndex++] = m_pRunData->sScheduleTime[ucStageIndex].ucGreenTime;
		}
	}
}


/*********************************************************************************
Function:       CManaKernel::GetDetectorSts
Description:    ��ȡ�����״̬
Input:          
Output:         pDetStatus - �����״ָ̬��
Return:         ��
**********************************************************************************/
/*
void CManaKernel::GetDetectorSts(SDetectorStsPara* pDetStatus)
{
	Byte ucDetCnt = 0;
	GBT_DB::TblDetector tblDetector;
	GBT_DB::Detector*   pDet = NULL;

	CDbInstance::m_cGbtTscDb.QueryDetector(tblDetector);
	pDet = tblDetector.GetData(ucDetCnt);
	
	ACE_OS::memset(pDetStatus,0,8*sizeof(SDetectorStsPara));

	while ( ucDetCnt > 0 )
	{
		pDetStatus->ucId = pDet->ucDetectorId;
		ucDetCnt--;
	}
}
*/


/*********************************************************************************
Function:       CManaKernel::GetDetectorData
Description:    ��ȡ���������
Input:          
Output:         pDetStatus - ���������ָ��
Return:         ��
**********************************************************************************/
/*
void CManaKernel::GetDetectorData(SDetectorDataPara* pDetData)
{
	Byte ucDetCnt = 0;
	GBT_DB::TblDetector tblDetector;
	GBT_DB::Detector*   pDet = NULL;

	CDbInstance::m_cGbtTscDb.QueryDetector(tblDetector);
	pDet = tblDetector.GetData(ucDetCnt);

	ACE_OS::memset(pDetData,0,48*sizeof(SDetectorDataPara));

	while ( ucDetCnt > 0 )
	{
		pDetData->ucId = pDet->ucDetectorId;
		ucDetCnt--;
	}
}
*/


/*********************************************************************************
Function:       CManaKernel::GetDetectorAlarm
Description:    ��ȡ����������澯����
Input:          
Output:         pDetStatus - ������������ָ��
Return:         ��
**********************************************************************************/
/*
void CManaKernel::GetDetectorAlarm(SDetectorAlarm* pDetAlarm)
{
	Byte ucDetCnt = 0;
	GBT_DB::TblDetector tblDetector;
	GBT_DB::Detector*   pDet = NULL;

	CDbInstance::m_cGbtTscDb.QueryDetector(tblDetector);
	pDet = tblDetector.GetData(ucDetCnt);

	ACE_OS::memset(pDetAlarm,0,48*sizeof(SDetectorAlarm));

	while ( ucDetCnt > 0 )
	{
		pDetAlarm->ucId = pDet->ucDetectorId;
		ucDetCnt--;
	}
}
*/


/*********************************************************************************
Function:       CManaKernel::GetPhaseStatus
Description:    ��ȡ��ǰ����λ״̬��Ϣ
Input:          ��
Output:         pPhaseStatus - ��λ״̬��Ϣ����ָ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetPhaseStatus(SPhaseSts* pPhaseStatus)
{
	Byte ucCurStepNo  = m_pRunData->ucStepNo;
	Uint uiWorkStatus = m_pRunData->uiWorkStatus;

	for ( int i=0; i<(MAX_PHASE/8); i++ )
	{
		pPhaseStatus[i].ucId = i + 1;

		switch ( uiWorkStatus )
		{
		case SIGNALOFF: //�ص�
			continue;
		case ALLRED:    //ȫ��
			pPhaseStatus[i].ucRed    = 0xff;
			continue;
		case FLASH:     //����
			pPhaseStatus[i].ucYellow = 0xff;
			continue;
		default:
			break;
		}

		for ( int j = 0; j < 8; j++ )
		{ 
			switch ( m_pRunData->sStageStepInfo[ucCurStepNo].ucPhaseColor[i*8+j] ) 
			{
			case LAMP_COLOR_RED:
				pPhaseStatus[i].ucRed |= (1<<j) & 0xff;
				break;
			case LAMP_COLOR_YELLOW:
				pPhaseStatus[i].ucYellow |= (1<<j) & 0xff;
				break;
			case LAMP_COLOR_GREEN:
				pPhaseStatus[i].ucGreen |= (1<<j) & 0xff;
				break;
			default:
				pPhaseStatus[i].ucRed |= (1<<j) & 0xff;
				break;
			}
		}
	}
}


/*********************************************************************************
Function:       CManaKernel::GetOverlapPhaseStatus
Description:    ��ȡ������λ״̬
Input:          ��
Output:         pOverlapPhaseSts - ������λ״̬��Ϣ����ָ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetOverlapPhaseStatus(SOverlapPhaseSts* pOverlapPhaseSts)
{
	Byte ucCurStepNo  = m_pRunData->ucStepNo;
	Uint uiWorkStatus = m_pRunData->uiWorkStatus;
	
	pOverlapPhaseSts->ucId = 1;

	switch ( uiWorkStatus )
	{
	case SIGNALOFF: //�ص�
		return;
	case ALLRED:    //ȫ��
		pOverlapPhaseSts->ucRed    = 0xff;
		return;
	case FLASH:     //����
		pOverlapPhaseSts->ucYellow = 0xff;
		return;
	default:
		break;
	}

	for ( int i=0; i<MAX_OVERLAP_PHASE; i++ )
	{
		switch ( m_pRunData->sStageStepInfo[ucCurStepNo].ucOverlapPhaseColor[i] ) 
		{
			case LAMP_COLOR_RED:
				pOverlapPhaseSts->ucRed |= (1<<i) & 0xff;
				break;
			case LAMP_COLOR_YELLOW:
				pOverlapPhaseSts->ucYellow |= (1<<i) & 0xff;
				break;
			case LAMP_COLOR_GREEN:
				pOverlapPhaseSts->ucGreen |= (1<<i) & 0xff;
				break;
			default:
				pOverlapPhaseSts->ucRed |= (1<<i) & 0xff;
				break;
		}
	}
}



/*********************************************************************************
Function:       CManaKernel::GetChannelStatus
Description:    ͨ��״̬��Ϣ��ȡ
Input:          ��
Output:         pChannelSts - ͨ��״̬��Ϣ����ָ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetChannelStatus(SChannelSts* pChannelSts)
{
	Byte ucPhaseNo    = 0;
	Byte ucPhaseType  = 0;
	Byte ucIndex      = 0;
	Byte* pPhaseColor = NULL;
	Byte ucCurStepNo = m_pRunData->ucStepNo;
	Uint uiWorkStatus = m_pRunData->uiWorkStatus;
	
	while ( ucIndex < 2 )
	{
		switch ( uiWorkStatus )
		{
		case SIGNALOFF: //�ص�
			ucIndex++;
			continue;
		case ALLRED:    //ȫ��
			pChannelSts[ucIndex].ucRed = 0xff;
			ucIndex++;
			continue;
		case FLASH:     //����
			pChannelSts[ucIndex].ucYellow = 0xff;
			ucIndex++;
			continue;
		default:
			break;
		}

		pChannelSts[ucIndex].ucId = ucIndex + 1;
		for ( int i=0; i<8; i++ )
		{
			//ucPhaseNo = GetPhaseIdWithChannel(8*ucIndex+i+1);
			ucPhaseNo   = m_pTscConfig->sChannel[8*ucIndex+i].ucSourcePhase;
			ucPhaseType = m_pTscConfig->sChannel[8*ucIndex+i].ucType;
			if ( CHANNEL_OVERLAP == ucPhaseType )
			{
				pPhaseColor = m_pRunData->sStageStepInfo[ucCurStepNo].ucOverlapPhaseColor;
			}
			else
			{
				pPhaseColor = m_pRunData->sStageStepInfo[ucCurStepNo].ucPhaseColor;
			}
				
			if ( (ucPhaseNo>0) && (ucPhaseNo<MAX_PHASE) )  //���ڸ�ͨ����Ϣ
			{
				switch ( pPhaseColor[ucPhaseNo-1] )  
				{
				case LAMP_COLOR_RED:
					pChannelSts[ucIndex].ucRed |= (1<<i);
					break;
				case LAMP_COLOR_YELLOW:
					pChannelSts[ucIndex].ucYellow |= (1<<i);
					break;
				case LAMP_COLOR_GREEN:
					pChannelSts[ucIndex].ucGreen |= (1<<i);
					break;
				default:
					pChannelSts[ucIndex].ucRed |= (1<<i);
					break;
				}
			}
		}
		ucIndex++;
	}
}


/*********************************************************************************
Function:       CManaKernel::GetPhaseIdWithChannel
Description:    ����ͨ���Ż�ȡͨ����Ӧ������λ��
Input:          ucChannelNo -ͨ���źŵ����
Output:         ��
Return:         ��λ�ţ�����ȡ���󷵻� MAX_PHASE+1(����)
**********************************************************************************/
Byte CManaKernel::GetPhaseIdWithChannel(Byte ucChannelNo)
{
	for (Byte ucIndex=0; ucIndex<MAX_CHANNEL; ucIndex++ )
	{
		if ( m_pTscConfig->sChannel[ucIndex].ucId == 0 )
		{
			break;
		}
		else if (  m_pTscConfig->sChannel[ucIndex].ucId == ucChannelNo )
		{
			return m_pTscConfig->sChannel[ucIndex].ucSourcePhase;
		}
	}

	return MAX_PHASE+1;
}




/*********************************************************************************
Function:       CManaKernel::SetRestart
Description:    ������������Ϊ��ֵ
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::SetRestart()
{
	m_bRestart = true;
}


/*********************************************************************************
Function:       CManaKernel::GetRestart
Description:    ��ȡ������������ֵ
Input:          ��
Output:         ��
Return:         ������������ֵ
**********************************************************************************/
bool CManaKernel::GetRestart()
{
	return m_bRestart;
}


/*********************************************************************************
Function:       CManaKernel::RestartTsc
Description:    �����źŻ�,1s����һ��
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::RestartTsc()
{
	static int iTick = 0;

	if ( !m_bRestart )
	{
		return;
	}

	iTick++;
	if ( iTick > 60 )
	{
		system("killall Gb.aiton");
		
		ACE_OS::sleep(2);
		system("./gb.aiton");
		
	}
}


/*********************************************************************************
Function:       CManaKernel::IsLongStep
Description:    �жϵ�ǰ�����Ƿ�Ϊ���������̵�����
Input:          iStepNo  - ������
Output:         ��
Return:         true - ����  false - �ǳ���
**********************************************************************************/
bool CManaKernel::IsLongStep(int iStepNo)
{
	bool bGreen      = false;
	bool bGreenFlash = false;
	int iIndex       = 0;

	while ( iIndex < MAX_LAMP )
	{
		if ( LAMP_COLOR_RED == iIndex % 3 )
		{
			iIndex++;
			continue;
		}		
		if ( ( LAMP_COLOR_YELLOW == iIndex % 3 )&& (1 == m_pRunData->sStageStepInfo[iStepNo].ucLampOn[iIndex]) )  //���ڻƵ�
		{
			return false;
		}

		if ( 1 == m_pRunData->sStageStepInfo[iStepNo].ucLampOn[iIndex] )  //�����̵���
		{
			if ( 0 == m_pRunData->sStageStepInfo[iStepNo].ucLampFlash[iIndex] ) //�̵Ʋ���
			{
				bGreen = true;
				//break ; //ADD: 2013 0702 1723 Ӧ������ѭ��
				
			}
			else  //���� 
			{
				bGreenFlash = true;
			}

			break ; ////Ӧ�������� ADD?20130702 17 21
		}
		iIndex++;
	}

	if ( bGreen && !bGreenFlash ) //�����̵� ����û��(�ƻ����������)
	{
		return true;
	}

	return false;   
}



/*********************************************************************************
Function:       CManaKernel::GetStageMinGreen
Description:    ��ȡ�׶���λ�����С��
Input:          usAllowPhase  - ������λ�������
Output:         ��
Return:         ��С��ֵ
**********************************************************************************/
int CManaKernel::GetStageMinGreen(Ushort usAllowPhase)
{
	int iMinGreen = 0; 

	for (Byte i=0; i<MAX_PHASE; i++ )
	{
		if ((usAllowPhase >>i)& 0x1) 
		{
			iMinGreen = (iMinGreen < m_pTscConfig->sPhase[i].ucMinGreen) ?m_pTscConfig->sPhase[i].ucMinGreen:iMinGreen;

		}
	}
	return iMinGreen;
}



/*********************************************************************************
Function:       CManaKernel::GetCurStepMinGreen
Description:    ��ȡ��ǰ��������С��(������λ����С��)�������ڽ׶εĵ�һ����ȡ
Input:          iStepNo  - ������
Output:         iMaxGreenTime:��������� iAddGreenTime:��С��λ�̵��ӳ�ʱ��
Return:         ��ǰ��������С��
**********************************************************************************/
int CManaKernel::GetCurStepMinGreen(int iStepNo , int* iMaxGreenTime, int* iAddGreenTime)
{
	int iIndex       = 0;
	int iMinGreen    = 0;
	int iAddGreen    = 200;
	int iMaxGreen    = 0;
	int iTmpMinGreen = 0;
	int iTmpAddGreen = 0;
	int iTmpMaxGreen = 0;
	unsigned int uiAllowPhase = m_pRunData->sStageStepInfo[iStepNo].uiAllowPhase;
	//unsigned int uiOverlapPhase = m_pRunData->sStageStepInfo[iStepNo].uiOverlapPhase;

	while ( iIndex < MAX_PHASE )
	{
		if ( /*(*/ (uiAllowPhase >> iIndex) & 1 /*) || ( (uiOverlapPhase >> iIndex) & 1 )*/ )
		{
			//if ( m_pTscConfig->sPhase[iIndex].ucType>>5 & 1  )  /*������λ*/
			//{
				iTmpMinGreen = m_pTscConfig->sPhase[iIndex].ucMinGreen;
				iTmpAddGreen = m_pTscConfig->sPhase[iIndex].ucGreenDelayUnit;
				iTmpMaxGreen = m_pTscConfig->sPhase[iIndex].ucMaxGreen1;
			//}
			/*
			else if ( m_pTscConfig->sPhase[iIndex].ucType>>6 & 1  ) //������λ
			{
				iTmpMinGreen = m_pTscConfig->sPhase[iIndex].ucFixedGreen;
			}
			*/
			/*
			else if ( m_pTscConfig->sPhase[iIndex].ucType>>7 & 1 )  //�̶���λ 
			{
				iTmpMinGreen = m_pRunData->sScheduleTime[StepToStage(m_pRunData->ucStepNo,NULL)].ucGreenTime 
					               - m_pTscConfig->sPhase[iIndex].ucGreenFlash;      //green
			}
			else
			{
				continue;
			}
			*/

			iMinGreen = iTmpMinGreen < iMinGreen    ? iMinGreen    : iTmpMinGreen; //�Ҵ�
			if ( iTmpAddGreen > 0 )
			{
				iAddGreen = iAddGreen    < iTmpAddGreen ? iAddGreen    : iTmpAddGreen; //��С
			}
			iMaxGreen = iMaxGreen    < iTmpMaxGreen ? iTmpMaxGreen : iMaxGreen;    //�Ҵ�
		} 
		iIndex++;
	}
	
	if ( iMaxGreenTime != NULL )
	{
		*iMaxGreenTime = iMaxGreen;
	}
	if ( iAddGreenTime != NULL )
	{
		*iAddGreenTime = iAddGreen;
	}

	return iMinGreen;
}


/*********************************************************************************
Function:       CManaKernel::AddRunTime
Description:    ��Ӧ���ƣ������̵�����ʱ��
Input:          iAddTime     - ��λ�������λ���ӵ���Сʱ��
				ucPhaseIndex - �г�����λindex 0 - 15
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::AddRunTime(int iAddTime,Byte ucPhaseIndex)
{
	
	Byte OldStepTime = m_pRunData->ucStepTime;		
	Byte iLeftRunTime = OldStepTime-m_pRunData->ucElapseTime ;
	//Byte iStepUnitTime = m_pTscConfig->sPhase[ucPhaseIndex].ucGreenDelayUnit ;
	//	Byte iPhaseFlashTime = m_pTscConfig->sPhase[ucPhaseIndex].ucGreenFlash ;  
	//Byte ucCntDownType = m_pTscConfig->sSpecFun[FUN_CNTTYPE].ucValue;	
	
	//	ACE_DEBUG((LM_DEBUG,"%s:%d: PhaseID =%d ucElapseTime:%d,ucStepTime=%d StepLeftTime=%d iStepUnitTime=%d \r\n",__FILE__,__LINE__	,ucPhaseIndex+1,m_pRunData->ucElapseTime,OldStepTime,iLeftRunTime,iStepUnitTime));
		if(!((m_pRunData->ucElapseTime >= m_pRunData->ucStepTime-m_iAdjustTime) && (m_pRunData->ucElapseTime <= m_pRunData->ucStepTime-1)))
			return ; //��Ӧʱ�䲻��ʱ�����䷶Χ�ڣ���������ʱ��
		
		m_pRunData->ucStepTime = m_pRunData->ucStepTime+m_iAdjustTime;//;iStepUnitTime ;  
		
		ACE_DEBUG((LM_DEBUG,"%s:%d: PhaseID =%d ucElapseTime:%d,NewStepTime=%d StepLeftTime=%d iStepUnitTime=%d\r\n",__FILE__,__LINE__,ucPhaseIndex+1,m_pRunData->ucElapseTime,m_pRunData->ucStepTime,iLeftRunTime,m_iAdjustTime));
		if(m_pRunData->uiCtrl != CTRL_PREANALYSIS)
		{			
			if(m_pRunData->ucStepTime >m_iMaxStepTime)
			{
				m_pRunData->ucStepTime = m_iMaxStepTime;
			}			
			ACE_DEBUG((LM_DEBUG,"%s:%d:After ucElapseTime:%d,ucStepTime from %d to %d m_iMaxStepTime:%d\n\n",__FILE__,__LINE__	,m_pRunData->ucElapseTime,OldStepTime,m_pRunData->ucStepTime ,m_iMaxStepTime));
		}
		else
		{
			//Byte MaxTime2 = GetMaxGreen2(m_pRunData->ucStepNo);
			if(m_pRunData->ucStepTime >m_ucStageDynamicMaxGreen[m_pRunData->ucStageNo])
			{		
				m_pRunData->ucStepTime = m_ucStageDynamicMaxGreen[m_pRunData->ucStageNo] ; 		
			}			
			//ACE_DEBUG((LM_DEBUG,"%s:%d:After ucElapseTime:%d,StepTime from %d to %d MaxStepTime2:%d\n\n",__FILE__,__LINE__	,m_pRunData->ucElapseTime,OldStepTime,m_pRunData->ucStepTime ,MaxTime2));

		}	
		m_pRunData->ucCycle = m_pRunData->ucCycle-OldStepTime+m_pRunData->ucStepTime;
			
		//	iCntFlashTime = iPhaseFlashTime ;    //   ���ڵ���ʱ��������	
		m_ucAddTimeCnt         = m_pRunData->ucStepTime - OldStepTime;   //�ý׶����ӵ��̵�ʱ��
}


/*********************************************************************************
Function:       CManaKernel::GetStageDetector
Description:    ��Ӧ���ƣ���ȡ�׶εĶ�Ӧ��Ȧ ȥ���ں�׶λ�����ֵ���λ,������ͬ�ĸ�
				����λȥ��ǰһ���׶εĵ���λ
Input:          iStageNo     - �׶κ�
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetStageDetector(int iStageNo)
{	
	Byte ucPhaseIndex        = 0;
	int iNextStage           = iStageNo + 1;
	int iNextStep            = 0;
	int iCurStepNo           = StageToStep(iStageNo);
	Uint iCurAllowPhase      = m_pRunData->sStageStepInfo[iCurStepNo].uiAllowPhase;
	int iNextAllowPhase      = 0;
	//Uint iCurOverlapPhase    = m_pRunData->sStageStepInfo[iCurStepNo].uiOverlapPhase;
	Uint iNextOverlapPhase   = 0;
	unsigned int uiTmp       = 0;
 
	if ( iNextStage > m_pRunData->ucStageCount )
	{
		iNextStage = 0;
	}

	iNextStep         = StageToStep(iNextStage);
	iNextAllowPhase   = m_pRunData->sStageStepInfo[iNextStep].uiAllowPhase;
	iNextOverlapPhase = m_pRunData->sStageStepInfo[iNextStep].uiOverlapPhase;
	
	//��ͨ��λ
	while ( ucPhaseIndex < MAX_PHASE )
	{
		if ( (iCurAllowPhase>>ucPhaseIndex & 1) 
			&& (iNextAllowPhase>>ucPhaseIndex & 1) )  //��һ���׶�Ҳ���ڸ���λ��ȥ������λ
		{
			uiTmp = 0;
			uiTmp |= ~(1<<ucPhaseIndex);
			iCurAllowPhase  = iCurAllowPhase & uiTmp;
		}
		ucPhaseIndex++;
	}

	/*/������λ
	while ( ucOverlapPhaseIndex < MAX_OVERLAP_PHASE )
	{
		if ( (iCurOverlapPhase>>ucOverlapPhaseIndex & 1) 
		  && (iNextOverlapPhase>>ucOverlapPhaseIndex & 1) )  //��һ���׶�Ҳ���ڸ���λ��ȥ������λ
		{
			ucDelPhase = OverlapPhaseToPhase(iCurAllowPhase,ucOverlapPhaseIndex);  //���ݸ�����λ��ȡ������λ���±�
			if ( ucDelPhase < MAX_PHASE )
			{
				uiTmp = 0;
				uiTmp |= ~(1<<ucDelPhase);
				iCurAllowPhase  = iCurAllowPhase & uiTmp;
			}
		}

		ucOverlapPhaseIndex++;
	}
*/
	m_uiStagePhase[iStageNo] |= iCurAllowPhase;
}


/*********************************************************************************
Function:       CManaKernel::GetAllStageDetector
Description:    ��Ӧ���ƣ���ȡ���н׶εĶ�Ӧ��Ȧ��һ������һ��
Input:          iStageNo     - �׶κ�
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::GetAllStageDetector()
{
	int iIndex = 0;

	ACE_OS::memset(m_uiStagePhase,0,MAX_SON_SCHEDULE*sizeof(unsigned int));

	while ( iIndex < m_pRunData->ucStageCount )
	{
		GetStageDetector(iIndex);
		iIndex++;
	}
}


/*********************************************************************************
Function:       CManaKernel::GetMaxStageMinGreen
Description:    ��ȡĳ����������λ�������С�̣����ڽ׶��л�ʱ��֤������С��
Input:          iStepNo - ������
Output:         ��
Return:         ��
**********************************************************************************/
int CManaKernel::GetMaxStageMinGreen(int iStepNo)
{
	int iIndex       = 0;
	int iMinGreen    = 0;
	int iTmpMinGreen = 0;
	unsigned int uiAllowPhase = m_pRunData->sStageStepInfo[iStepNo].uiAllowPhase;
	unsigned int uiOverlapPhase = m_pRunData->sStageStepInfo[iStepNo].uiOverlapPhase;

	while ( iIndex < 32 )
	{
		if ( ( (uiAllowPhase >> iIndex) & 1 ) || ( (uiOverlapPhase >> iIndex) & 1 ) )
		{
			iTmpMinGreen = m_pTscConfig->sPhase[iIndex].ucMinGreen;	
			iMinGreen = iTmpMinGreen < iMinGreen ? iMinGreen : iTmpMinGreen; 
		} 
		iIndex++;
	}

	return iMinGreen;
}


/*********************************************************************************
Function:       CManaKernel::SpecTimePattern
Description:    ָ���ض�����ʱ������
Input:          iTimePatternId - ָ������ʱ������
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::SpecTimePattern(int iTimePatternId )
{
	if ( iTimePatternId != m_pRunData->ucTimePatternId )   //�¸����ڻ���ô˷�����
	{
		SetUpdateBit();
	}
	m_iTimePatternId = iTimePatternId;

	ACE_DEBUG((LM_DEBUG,"%s:%d \n@@@@@@@@@@%d\n",__FILE__,__LINE__,m_iTimePatternId));
}


/*********************************************************************************
Function:       CManaKernel::GetVehilePara
Description:    ��ȡ�����Ӧ���������õ�ֵ
Input:          ��
Output:         pbVehile     - �Ƿ�Ϊ��Ӧ����        bDefStep    - Ŀǰ�����µĲ��� 
*       	    piAdjustTime - ÿ�εĵ���ʱ��        piCurPhase  - ��ǰ����ִ�е���λ�� 
*       	    piNextPhase  - �¸�����ִ�е���λ��  psPhaseDet  - ��λ�������Ĺ�ϵ  
Return:         ��
**********************************************************************************/
void CManaKernel::GetVehilePara(bool* pbVehile
									 , bool* bDefStep
									 , int* piAdjustTime 
									 , Uint* piCurPhase 
									 , Uint* piNextPhase
									 , SPhaseDetector* psPhaseDet)
{
	Byte ucNextStage        = 0;
	Uint ucNextPhaseTmp     = 0;
	int  iIndex             = 0;
	static Byte ucCurStepNo = 222;

	//ACE_DEBUG((LM_DEBUG,"\n %s:%d m_bVehile:%d ucCurStepNo:%d \n" ,__FILE__,__LINE__,m_bVehile,ucCurStepNo));

	*pbVehile     = m_bVehile;
	if ( !m_bVehile )
	{
		return;
	}

	if ( ucCurStepNo == m_pRunData->ucStepNo )
	{
		return;
	}
	else
	{
		*bDefStep = true;
		ucCurStepNo = m_pRunData->ucStepNo;
	}

	//*pbVehile     = m_bVehile;
	*piAdjustTime = m_iAdjustTime;
	*piCurPhase   = m_uiStagePhase[m_pRunData->ucStageNo];
	ACE_OS::memcpy(psPhaseDet , m_sPhaseDet , MAX_PHASE*sizeof(SPhaseDetector) );

	if ( CTRL_VEHACTUATED == m_pRunData->uiCtrl ||CTRL_PREANALYSIS == m_pRunData->uiCtrl )
	{
		return;
	}
	
	/*��ȡ��һ����λ����ֻ�ڰ��Ӧ�����õ�*/
	iIndex = m_pRunData->ucStepNo + 1;  //Ѱ����һ����λ��
	while ( iIndex < m_pRunData->ucStepNum )
	{
		if ( IsLongStep(iIndex) )
		{
			ucNextStage    = StepToStage(iIndex,NULL);
			ucNextPhaseTmp = m_uiStagePhase[ucNextStage];
			if ( IsMainPhaseGrp(*piCurPhase) != IsMainPhaseGrp(ucNextPhaseTmp) ) //�� �� ��������
			{
				*piNextPhase = ucNextPhaseTmp;
				//ACE_DEBUG((LM_DEBUG,"\n %s:%d *piCurPhase:%x *piNextPhase:%x \n" ,__FILE__,__LINE__,*piCurPhase ,*piNextPhase ));
				return;
			}
		}
		iIndex++;
	}
	
	*piNextPhase = m_uiStagePhase[0];	

	//ACE_DEBUG((LM_DEBUG,"\n %s:%d *piCurPhase:%x *piNextPhase:%x \n" ,__FILE__,__LINE__,*piCurPhase ,*piNextPhase ));
}


/*********************************************************************************
Function:       CManaKernel::GetVehilePara
Description:    �ж�ĳ����λ���Ƿ�Ϊ����λ�飬����������λ�����ڰ��Ӧ����
Input:          uiPhase -��λ�������
Output:         ��
Return:         true - ������ false - ��������
**********************************************************************************/
bool CManaKernel::IsMainPhaseGrp(Uint uiPhase)
{
	int iIndex    = 0;
	int iDetCnt   = 0;
	int iDetIndex = 0;
	int iDetId    = 0;

	while ( iIndex < MAX_PHASE )
	{
		if ( uiPhase>>iIndex & 1 )
		{
			iDetCnt = m_sPhaseDet[iIndex].iRoadwayCnt;

			iDetIndex = 0;
			while ( iDetIndex < iDetCnt )
			{
				iDetId = m_sPhaseDet[iIndex].iDetectorId[iDetIndex] - 1;
				if ( (m_pTscConfig->sDetector[iDetId].ucOptionFlag>>1) & 1 )  //��ͨ������Ϊ�ؼ���λ
				{
					return true;
				}
				iDetIndex++;
			}
		}

		iIndex++;
	}

	return false;
}


/*********************************************************************************
Function:       CManaKernel::IsInChannel
Description:    �жϸ�ͨ���Ƿ�����
Input:          iChannelId : ͨ����
Output:         ��
Return:         true - ���� false - δ����
**********************************************************************************/
bool CManaKernel::IsInChannel(int iChannelId)
{
	int iIndex = 0;

	while ( iIndex < MAX_CHANNEL )
	{
		if ( ( iChannelId == m_pTscConfig->sChannel[iIndex].ucId ) 
			 && (m_pTscConfig->sChannel[iIndex].ucSourcePhase != 0) )
		{
			return true;
		}
		iIndex++;
	}
	return false;
}


/*********************************************************************************
Function:       CManaKernel::InConflictPhase
Description:    �ж���λ�Ƿ��ͻ���������̳�ͻ
Input:          ��
Output:         ��
Return:         true - ��ͻ false - δ��ͻ
**********************************************************************************/
bool CManaKernel::InConflictPhase()
{
	int  iIndex = 0;
	int  iCurStepNo   = m_pRunData->ucStepNo;
	Uint uiAllowPhase = m_pRunData->sStageStepInfo[iCurStepNo].uiAllowPhase;
	Uint uiCollision  = 0;

	GBT_DB::Collision* pConflictPhase = m_pTscConfig->sPhaseConflict;

	while ( iIndex < MAX_PHASE )
	{
		if ( (uiAllowPhase >> iIndex) & 1 )  //���ڸ���λ
		{
			uiCollision = (pConflictPhase+iIndex)->usCollisionFlag;
			if ( (uiAllowPhase & uiCollision) != 0 )
			{
				ACE_DEBUG((LM_DEBUG,"%s:%d ConflictPhase StepNum:%d AllowPhase:%d Collision:%d !\n ",__FILE__,__LINE__,iCurStepNo,uiAllowPhase,uiCollision));

				return true;
			}
		}
		iIndex++;
	}
	return false;
}


/*********************************************************************************
Function:       CManaKernel::DealGreenConflict
Description:    ������ֵ��̳�ͻ
Input:          ucdata   - ����ֵ
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::DealGreenConflict(Byte ucdata)
{
	if ( m_pTscConfig->sSpecFun[FUN_SERIOUS_FLASH].ucValue != 0 || 1)
	{
		CLampBoard::CreateInstance()->SetSeriousFlash(true);
		//CFlashMac::CreateInstance()->SetHardwareFlash(true);

		SThreadMsg sTscMsg;
		sTscMsg.ulType       = TSC_MSG_SWITCH_CTRL;  
		sTscMsg.ucMsgOpt     = 0;
		sTscMsg.uiMsgDataLen = 1;
		sTscMsg.pDataBuf     = ACE_OS::malloc(1);
		*((Byte*)sTscMsg.pDataBuf) = CTRL_MANUAL;  //�ֶ�
		CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));

		SThreadMsg sTscMsgSts;
		sTscMsgSts.ulType       = TSC_MSG_SWITCH_STATUS;  
		sTscMsgSts.ucMsgOpt     = 0;
		sTscMsgSts.uiMsgDataLen = 1;
		sTscMsgSts.pDataBuf     = ACE_OS::malloc(1);
		*((Byte*)sTscMsgSts.pDataBuf) = FLASH;  //����
		CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsgSts,sizeof(sTscMsgSts));
	}
	
}


/*********************************************************************************
Function:       CManaKernel::AllotActiveTime
Description:    ����Ӧ���Ƹ��ݸ���������ϸ����ڵ�ռ���ʣ������̵�ʱ��
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::AllotActiveTime()
{
	int iIndex      = 0;
	int jIndex      = 0;
	int zIndex      = 0;
	int iStagePhase = 0;
	int iStepNo     = 0; 
	int iDetSum     = 0;
	unsigned int iDetId       = 0;
	unsigned long iRecordSum  = 0;
	int iGreenTimeSum  = 0;
	int iStepGreenTime = 0;
	unsigned long ulTotalHaveCarTime = 0; 
	unsigned int iStageHaveCarTime[MAX_SON_SCHEDULE] = {0};   //�����׶������õļ�������г�ʱ��
	GBT_DB::VehicleStat*   pVehicleStat = NULL;
	GBT_DB::TblVehicleStat tblVehicleStat;

	(CDbInstance::m_cGbtTscDb).QueryVehicleStat(tblVehicleStat);
	pVehicleStat = tblVehicleStat.GetData(iRecordSum);

	if ( iRecordSum <= 0 )
	{
		return;
	}

	while ( iIndex < m_pRunData->ucStageCount )
	{
		iStepNo     = StageToStep(iIndex);
		iStagePhase = ((m_pRunData->sStageStepInfo[iStepNo].uiAllowPhase)|((m_pRunData->sStageStepInfo[iStepNo].uiOverlapPhase)<<MAX_PHASE)); //MOD:201401031449

		jIndex = 0;
		while ( jIndex < (MAX_PHASE+MAX_OVERLAP_PHASE) )
		{
			if ( (iStagePhase>>jIndex) & 1 )
			{
				iDetSum = m_sPhaseDet[jIndex].iRoadwayCnt;
				zIndex  = 0;
				//while ( zIndex < iDetSum )
				//{
					iDetId = m_sPhaseDet[jIndex].iDetectorId[zIndex];
					if (  iDetId < iRecordSum )
					{
						iStageHaveCarTime[iIndex] += (pVehicleStat+iDetId-1)->ucOccupancy;     //��ȡռ����
					}
					//zIndex++;
				//}
				 
			}
			jIndex++;
		}
		
		iGreenTimeSum      += m_pRunData->sStageStepInfo[iStepNo].ucStepLen;   //�̵�������ʱ��
		ulTotalHaveCarTime += iStageHaveCarTime[iIndex];       //���г�ʱ��      
		iStagePhase = 0;
		iIndex++;
	}

	iIndex = 0;
	while ( iIndex < m_pRunData->ucStageCount )
	{
		iStepNo        = StageToStep(iIndex);
		iStepGreenTime = iGreenTimeSum * iStageHaveCarTime[iIndex] / ulTotalHaveCarTime + 1 ;

		if ( iStepGreenTime < MIN_GREEN_TIME )
		{
			iStepGreenTime = MIN_GREEN_TIME;
		}

		m_pRunData->sStageStepInfo[iStepNo].ucStepLen = iStepGreenTime;  //�޸Ľ׶ε��̵�ʱ��
		iIndex++;
	}
}


/*********************************************************************************
Function:       CManaKernel::UtcsAdjustCycle
Description:    Э�����ƣ����������ṩ�Ĺ�������ʱ���������ڵ���������ٲ�
Input:          ��
Output:         ��
Return:         ��
**********************************************************************************/
void CManaKernel::UtcsAdjustCycle()
{
	bool bPlus         = false;
	int iAdjustCnt     = 0;
	int iMinGreen      = 0;
	int iMaxGreen      = 0;
	int iAdjustPerStep = 0;
	Byte ucStageCnt    = m_pRunData->ucStageCount;
	Byte ucAdjustGreenLen[MAX_STEP] = {0};

	if ( m_pRunData->ucCycle > m_ucUtcsComCycle )  //-
	{
		bPlus      = false;
		iAdjustCnt = m_pRunData->ucCycle - m_ucUtcsComCycle;
	}
	else if ( m_pRunData->ucCycle < m_ucUtcsComCycle )  //+
	{
		bPlus      = true;
		iAdjustCnt = m_ucUtcsComCycle - m_pRunData->ucCycle;
	}
	else
	{
		return;
	}
	
	ACE_DEBUG((LM_DEBUG,"%s:%d Cycle iAdjustCnt = %d bPlus=%d\n",__FILE__,__LINE__,iAdjustCnt,bPlus));
	//ƽ�ֵ�������
	for ( int i = 0; i < MAX_STEP && i < m_pRunData->ucStepNum; i++ ) 
	{
		if ( !IsLongStep(i) )
		{
			continue;
		}
		ucAdjustGreenLen[i] = iAdjustCnt / ucStageCnt; //5��
	}
	iAdjustCnt -= ucStageCnt * ( iAdjustCnt / ucStageCnt ); //0

	//����Լ��
	for ( int i = 0; i < MAX_STEP && i < m_pRunData->ucStepNum; i++ ) 
	{
		if ( !IsLongStep(i) )
		{
			continue;
		}

		iMinGreen = GetCurStepMinGreen(i , &iMaxGreen , NULL);

		if ( bPlus )
		{
			iAdjustPerStep = iMaxGreen - m_pRunData->sStageStepInfo[i].ucStepLen;	
		}
		else
		{
			iAdjustPerStep = m_pRunData->sStageStepInfo[i].ucStepLen - iMinGreen;
		}

		if ( iAdjustPerStep < 0 )  //Ԥ����С�̻�����̵����ô���
		{
			iAdjustPerStep = m_pRunData->sStageStepInfo[i].ucStepLen / MAX_ADJUST_CYCLE;
		}

		if ( ucAdjustGreenLen[i] > iAdjustPerStep )  //������Χ����
		{
			iAdjustCnt         += ( ucAdjustGreenLen[i] - iAdjustPerStep );
			ucAdjustGreenLen[i] = iAdjustPerStep;
		}
		else
		{
			iAdjustPerStep = iAdjustPerStep - ucAdjustGreenLen[i];
		}

		if ( iAdjustPerStep > iAdjustCnt )  //�������
		{
			iAdjustPerStep = iAdjustCnt;
		}

		if ( bPlus ) 
		{
			m_pRunData->sStageStepInfo[i].ucStepLen += ( ucAdjustGreenLen[i] + iAdjustPerStep );
		}
		else
		{
			m_pRunData->sStageStepInfo[i].ucStepLen -= ( ucAdjustGreenLen[i] + iAdjustPerStep );
		}
		iAdjustCnt -= iAdjustPerStep;

		//ACE_DEBUG((LM_DEBUG,"%s:%d After cooridate m_pRunData->sStageStepInfo[%d].ucStepLen:%d\n",__FILE__,__LINE__,i,m_pRunData->sStageStepInfo[i].ucStepLen));
	}

	m_pRunData->ucCycle = m_ucUtcsComCycle;
}


/*********************************************************************************
Function:       CManaKernel::GetUseLampBoard
Description:    ����ͨ��ID�������õĵƿذ�
Input:          ��
Output:         bUseLampBoard: true-���� false-û������
Return:         ��
**********************************************************************************/
void CManaKernel::GetUseLampBoard(bool* bUseLampBoard)
{
	int iIndex = 0;

	if ( NULL == bUseLampBoard )
	{
		return;
	}

	while ( iIndex < MAX_LAMP_BOARD )
	{
		if ( GetUseLampBoard(iIndex) )  //�õƿذ������õ�ͨ��
		{
			*(bUseLampBoard+iIndex) = true;
		}
		iIndex++;
	}
}


/*********************************************************************************
Function:       CManaKernel::GetUseLampBoard
Description:    �жϵƿذ��Ƿ����������Ϣ
Input:          iLampBoard - �ƿذ� 0 , 1 , 2 , 3
Output:         ��
Return:         ��
**********************************************************************************/
bool CManaKernel::GetUseLampBoard(int iLampBoard)
{
	int iChannel = 0;
	iChannel = iLampBoard * 4 + 1;
	for ( iChannel=iLampBoard * 4 + 1; iChannel<(iLampBoard+1) * 4 + 1; iChannel++ )
	{
		if ( IsInChannel(iChannel)  )
		{
			return true;
		}
	}

	return false;
}


/*********************************************************************************
Function:       CManaKernel::InPhaseDetector
Description:    �жϼ�����Ƿ��ڵ�ǰ������
Input:          ucDetId -�����ID��
Output:         ��
Return:         true - �ڵ�ǰ����  false -���ڵ�ǰ����
**********************************************************************************/
bool CManaKernel::InPhaseDetector(Byte ucDetId)
{
	Byte ucPhaseIndex = 0;
	Byte ucDetIndex   = 0;
	int  iRoadwayCnt  = 0;

	while ( ucPhaseIndex < MAX_PHASE )
	{
		ucDetIndex  = 0;
		iRoadwayCnt = m_sPhaseDet[ucPhaseIndex].iRoadwayCnt;
		while ( ucDetIndex < iRoadwayCnt)
		{
			if ( ucDetId == m_sPhaseDet[ucPhaseIndex].iDetectorId[ucDetIndex] )
			{
				return true;
			}
			ucDetIndex++;
		}
		ucPhaseIndex++;
	}

	return false;
}

bool CManaKernel::IsVehile()
     {
		return m_bVehile ;
    }

/**************************************************************
Function:        CManaKernel::SndMsgLog
Description:     �����־��Ϣ���������ڷ��͸���д��־��Ϣ				
Input:          ucLogType  ��־����
				ucLogVau1  ��־ֵ1
				ucLogVau2  ��־ֵ2
				ucLogVau3  ��־ֵ3
				ucLogVau4  ��־ֵ4              
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::SndMsgLog(Byte ucLogType,Byte ucLogVau1,Byte ucLogVau2,Byte ucLogVau3,Byte ucLogVau4)
{
	SThreadMsg sTscMsg;
	sTscMsg.ulType       = TSC_MSG_LOG_WRITE;
	sTscMsg.ucMsgOpt     = ucLogType;
	sTscMsg.uiMsgDataLen = 4;
	sTscMsg.pDataBuf     = ACE_OS::malloc(4);
	((Byte*)(sTscMsg.pDataBuf))[0] = ucLogVau1; 
	((Byte*)(sTscMsg.pDataBuf))[1] = ucLogVau2;	
	((Byte*)(sTscMsg.pDataBuf))[2] = ucLogVau3; 
	((Byte*)(sTscMsg.pDataBuf))[3] =ucLogVau4;														
	CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));
}

/**************************************************************
Function:        CManaKernel::ValidSoftWare
Description:    ��֤����ĺϷ���			
Input:          ��    
Output:         ��
Return:         ��
***************************************************************/
void CManaKernel::ValidSoftWare()
	{
		char SerialNum1[32]={0};
		char SerialNum2[32]={0};
		//��6410���������õ��ǣ����뱣�浽���ݿ�
		//(CDbInstance::m_cGbtTscDb).GetEypSerial(SerialNum1);
		GetSysEnyDevId(SerialNum2);

		//ACE_DEBUG((LM_DEBUG,"%s:%d SerialNum1:%s SerialNum2:%s\n",__FILE__,__LINE__,SerialNum1,SerialNum2));
		if(ACE_OS::strcmp(SerialNum1,SerialNum2)==0)
			bValidSoftWare = true ;
		else
			bValidSoftWare = false ;
	}


void CManaKernel::SetLampColor(Byte ColorType)
{

	if(ColorType == 0)//OFF
	{
		ACE_OS::memset( &m_pRunData->sStageStepInfo[0] , 0 , sizeof(SStepInfo) );
	}
	else if(ColorType == 1)//FLASH
	{
		for ( int i=0; i<MAX_LAMP_BOARD; i++ )
			{
				for ( int j=0; j<MAX_LAMP_NUM_PER_BOARD; j++ )
				{
					if ( (j%3) == LAMP_COLOR_YELLOW )
					{
						m_pRunData->sStageStepInfo[0].ucLampOn[i*MAX_LAMP_NUM_PER_BOARD+j]    = 1;
						m_pRunData->sStageStepInfo[0].ucLampFlash[i*MAX_LAMP_NUM_PER_BOARD+j] = 1;
					}
					else
					{						
						m_pRunData->sStageStepInfo[0].ucLampOn[i*MAX_LAMP_NUM_PER_BOARD+j]    = 0;
						m_pRunData->sStageStepInfo[0].ucLampFlash[i*MAX_LAMP_NUM_PER_BOARD+j] = 0;
					}
				}
			}

	}
	else if(ColorType == 2)//RED
	{
		for ( int i=0; i<MAX_LAMP_BOARD; i++ )
		{
			for ( int j=0; j<MAX_LAMP_NUM_PER_BOARD; j++ )
			{
				if ( (j%3) == LAMP_COLOR_RED )
				{
					m_pRunData->sStageStepInfo[0].ucLampOn[i*MAX_LAMP_NUM_PER_BOARD+j] = 1;
				}
				else
				{
					m_pRunData->sStageStepInfo[0].ucLampOn[i*MAX_LAMP_NUM_PER_BOARD+j] = 0;
				}
				m_pRunData->sStageStepInfo[0].ucLampFlash[i*MAX_LAMP_NUM_PER_BOARD+j] = 0;
			}
		}
	}
}

/**************************************************************
Function:        CManaKernel::SetPhaseColor
Description:    ������λ����ͨ���̵���			
Input:          bOverPhase -�Ƿ����ڸ�����λ true-��ͨ��λ false-������λ
				iPhaseId   - ��λ��   
Output:         ��
Return:         ��

***************************************************************/
void CManaKernel::SetPhaseColor(bool bOverPhase,Byte iPhaseId)
{
	Byte ucLampIndex = 0;
	Byte ucSignalGrpNum   = 0;
	Byte ucSignalGrpIndex = 0;
	Byte ucSignalGrp[MAX_CHANNEL] = {0};
	GetSignalGroupId(bOverPhase, iPhaseId, &ucSignalGrpNum , ucSignalGrp);
	while ( ucSignalGrpIndex < ucSignalGrpNum )
	{
		ucLampIndex = ( ucSignalGrp[ucSignalGrpIndex] - 1 ) * 3; //��λ���red�±�
		ucLampIndex = ucLampIndex + 2;
		m_ucLampOn[ucLampIndex] = 1;
		ucSignalGrpIndex++;
	}
}


/**************************************************************
Function:        CManaKernel::GaGetDirLane
Description:    �жϷ���ֵ�Ƿ�����ĳ������		
Input:          ucTableId -����ֵ �ڲ���������͸÷������ֱ������ͨ������
				ucDir   - �������� 0-���� 1-���� 2-�Ϸ� 3-����  
					  ucTableId   0      2      4     6
Output:         ��
Return:         true- ���ڸ÷��� false-�����ڸ÷���
***************************************************************/
bool CManaKernel::GaGetDirLane(Byte ucTableId , Byte  ucDir )
{
	Byte ucDirCfg        = (ucTableId >> 5) & 0x07;
	Byte ucCrossWalkFlag  = (ucTableId >> 3) & 0x03;
	if(ucDirCfg == (ucDir*2)  && ucCrossWalkFlag ==0)
	{
		ACE_DEBUG((LM_DEBUG,"%s:%d ucTableId ==%d ucDir==%d !\n",__FILE__,__LINE__,ucTableId,ucDir));
		return true ;
	}
	else
		return false;
}

/**************************************************************
Function:        CManaKernel::SetDirecChannelColor

Description:    ����ĳ���������̵Ʒ���		
Input:          ucTableId -����ֵ �ڲ���������͸÷������ֱ������ͨ������

				ucDir   - �������� 0-���� 1-���� 2-�Ϸ� 3-����  
Output:         ��
Return:         true- ���ڸ÷��� false-�����ڸ÷���
***************************************************************/

void CManaKernel::SetDirecChannelColor(Byte iDirecType)
{
	Byte ucIndex = 0;
	Uint ucDirVaule = 0;
	ACE_OS::memset(m_ucLampOn,0,MAX_LAMP);
	ACE_OS::memset(m_ucLampFlash,0,MAX_LAMP);	
	while(ucIndex< MAX_DREC)	
	{
		ucDirVaule = (m_pTscConfig->sPhaseToDirec[ucIndex]).ucId ;
		//ACE_DEBUG((LM_DEBUG,"%s:%d ucDirVaule == %d !\n",__FILE__,__LINE__,ucDirVaule));
		if((m_pTscConfig->sPhaseToDirec[ucIndex]).ucPhase>0 || (m_pTscConfig->sPhaseToDirec[ucIndex]).ucOverlapPhase>0)
		{
			
			if(GaGetDirLane(ucDirVaule,iDirecType ))
			{
				if((m_pTscConfig->sPhaseToDirec[ucIndex]).ucPhase != 0)
					SetPhaseColor(true,(m_pTscConfig->sPhaseToDirec[ucIndex]).ucPhase);
				else if((m_pTscConfig->sPhaseToDirec[ucIndex]).ucOverlapPhase != 0)
					SetPhaseColor(false,(m_pTscConfig->sPhaseToDirec[ucIndex]).ucOverlapPhase);
			}
		}
		else
		{
			ucIndex++;
			continue;
		}
		ucIndex++;
	}	
	
	 SetRedOtherLamp(m_ucLampOn,m_ucLampFlash);
	 CLampBoard::CreateInstance()->SetLamp(m_ucLampOn,m_ucLampFlash);
}

/**************************************************************
Function:        CManaKernel::SetWirelessBtbDirecCfg
Description:    ��������ң�ذ���һ�����������ϵ��̵Ʒ���	
Input:          RecvBtnDirecData -4�ֽڵı�������������̵Ʒ���,ÿ������һ���ֽڣ�ÿ���ֽ�
		     ���� ��ֱ�������е�ͷ��				  
Output:         ��
Return:         ��
Date:            2014-10-21 16:06
***************************************************************/
void CManaKernel::SetWirelessBtnDirecCfg(Uint RecvBtnDirecData ,Byte Lampcolor)
{	
	Byte BtnDirecData = 0 ;
	//Byte ucLampIndex = 0 ;
	ACE_OS::memset(m_ucLampOn,0,MAX_LAMP);
	ACE_OS::memset(m_ucLampFlash,0,MAX_LAMP);
	for(Byte idex = 0 ; idex <4 ; idex++)   // ������������ 0 1 2 3
	{
		BtnDirecData = (RecvBtnDirecData>>8*idex) ;
		for(Byte idex2 = 0 ; idex2< 5 ; idex2++) //��ֱ�����е�ͷ����
		{
			if(BtnDirecData>>idex2 &0x1)
			{
				if(idex2 ==0x4) //��ͷ������������ͷͨ��Ĭ��17 18 19 20
				{
					if(Lampcolor == 0x0)
						m_ucLampOn[(16+idex)*3+2] = 1;
					else if(Lampcolor == 0x1)
					{
						m_ucLampOn[(16+idex)*3+1] = 1;
					}
					else if(Lampcolor == 0x2)
					{
						m_ucLampOn[(16+idex)*3+2] = 1;
						m_ucLampFlash[(16+idex)*3+2] = 1;
					}
				}
				else
				{
					if(Lampcolor == 0x0)
					{
						m_ucLampOn[(4*idex+idex2)*3+2] = 1; //��ֱ������	
						
					}
					else if(Lampcolor == 0x1)
					{
						if(idex2 !=0x3)
							m_ucLampOn[(4*idex+idex2)*3+1] = 1; //���лƵƲ���˸
						
						//ACE_OS::printf("%s:%d Transit setp Yellow lamp= %d !\n",__FILE__,__LINE__,(4*idex+idex2)*3+1);
					}
					else if(Lampcolor == 0x2)
					{
						m_ucLampOn[(4*idex+idex2)*3+2] = 1;
						m_ucLampFlash[(4*idex+idex2)*3+2] = 1;
						
						//ACE_OS::printf("%s:%d Transit setp Green Flash lamp= %d!\n",__FILE__,__LINE__,(4*idex+idex2)*3+2);
					}
					else if(Lampcolor == 0x3)
					{
						if(idex2 !=0x3)
						{							
							m_ucLampOn[(4*idex+idex2)*3+1] = 1;						
							m_ucLampFlash[(4*idex+idex2)*3+1] = 1;
						}
					}
				}
			}
		}
		
	}	
	SetRedOtherLamp(m_ucLampOn,m_ucLampFlash);
	CLampBoard::CreateInstance()->SetLamp(m_ucLampOn,m_ucLampFlash);
	
}

Byte CManaKernel::GetStageMaxDetectorCars(Uint32 StageAllowPhases) //��ȡ�׶���λ����������Ŷӳ���
{
	Byte iIndex =0x0 ;
	Byte iDetCnt =0x0 ;
	Byte MaxPhaseDetectorCars =0x0 ;
	while(iIndex < MAX_PHASE )
	{
		if ( StageAllowPhases>>iIndex & 0x1 )
		{
			iDetCnt = m_sPhaseDet[iIndex].iRoadwayCnt;

			Byte iDetIndex = 0x0;
			while ( iDetIndex < iDetCnt )
			{							
				 if(m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex]>MaxPhaseDetectorCars)
				 {								 	
				 	MaxPhaseDetectorCars = m_sPhaseDet[iIndex].iDetectorCarNumbers[iDetIndex] ;
				 }														
				iDetIndex++;
			}
		}
	iIndex++;
 	}
	
	//ACE_OS::printf("%s:%d StageAllowPhases= %d  MaxCars=%d !\n",__FILE__,__LINE__,StageAllowPhases,MaxPhaseDetectorCars);
	return MaxPhaseDetectorCars ;


}


Byte CManaKernel::GetMaxGreen2(int iStepNo) //��ȡ���������2
{
	
	int iIndex		 = 0;
		int iMaxGreen2	 = 255;
		int iTmpMaxGreen = 0;
		unsigned int uiAllowPhase = m_pRunData->sStageStepInfo[iStepNo].uiAllowPhase;
	
		while ( iIndex < MAX_PHASE)
		{
			if ( ( (uiAllowPhase >> iIndex) & 1 ) )
			{
				iTmpMaxGreen = m_pTscConfig->sPhase[iIndex].ucMaxGreen2; 
				iMaxGreen2 = iTmpMaxGreen <iMaxGreen2?iTmpMaxGreen :iMaxGreen2; 
			} 
			iIndex++;
		}
	
		return iMaxGreen2;


}

/**************************************************************
Function:       DealPSC
Description:    �����������˰����������
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
void CPscMode::DealPSC()
{
    /*
	bool bPressBtn          = false;
	Byte ucIndex            = 0;
	Byte ucStageStand       = 0;
	Byte ucStageCross1      = 0;
	Byte ucStageCross2      = 0;
	Byte ucButton           = 0;
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
*/
}



