
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   PreAnalysis.cpp
Date:       2015-5-18
Description:�źŻ����������Ȳ���
Version:    V1.0
History:
***************************************************************/
#include "SerialCtrl.h"
#include "GbtMsgQueue.h"
#include "ManaKernel.h"
#include <termios.h>
#include <stdlib.h>
#include "PreAnalysis.h"
#include "GaCountDown.h"
#include "TscMsgQueue.h"
#include "Rs485.h"

typedef struct
{
	Byte   stx;	  // 0xAA 
	Uint   ECRID;  //����
	Ushort cmd;	  // ����
	Ushort addr;  // ��ַ
	Byte   PgNo;   // ����
	Ushort len;  //  ���ݳ���
	Byte   sum;	  // У����
}
#ifndef WINDOWS
__attribute__( ( packed, aligned(1) ) )
#endif
HECRHEADST;


/**************************************************************
Function:        CPreAnalysis::CPreAnalysis
Description:     CPreAnalysis�๹�캯������ʼ����			
Input:          ��           
Output:         ��
Return:         ��
***************************************************************/
CPreAnalysis::CPreAnalysis()
{
	ACE_DEBUG((LM_DEBUG,"%s:%d Init CBusPriority object !\r\n",__FILE__,__LINE__));
	
	 Byte m_iGpsFd  = CSerialCtrl::CreateInstance()->GetSerialFd2();
	 p_BusDelayTime    = 5 ; //Ĭ�ϳ�������ʱ��
	 p_PerBusCrossTime = 2 ;//Ĭ�ϵ���ͨ��·��ʱ��
	 ACE_OS::memset(&sPreAnalysisParaData,0x0 ,sizeof(sPreAnalysisParaData));
	 for(Byte index = 0x0 ;index <MAX_PHASE ;index++)
	 {
		 sPreAnalysisParaData[index].DevId = index+1 ;		 
		 sPreAnalysisParaData[index].IsUsed= false ;
	 }
}

/**************************************************************
Function:       CBusPriority::~CBusPriority
Description:    CBusPriority����������		
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
CPreAnalysis::~CPreAnalysis()
{
	ACE_DEBUG((LM_DEBUG,"%s:%d Destruct CBusPriority object!\r\n",__FILE__,__LINE__));
}

/**************************************************************
Function:       CBusPriority::CBusPriority
Description:    ����CGps�ྲ̬����		
Input:          ��              
Output:         ��
Return:         CBusPriority��̬����ָ��
***************************************************************/
CPreAnalysis* CPreAnalysis::CreateInstance()
{
	static CPreAnalysis CPreAnalysis;
	return &CPreAnalysis;
}



/**************************************************************
Function:        QueryAccessDev
Description:    ������豸������ѯ����
Input:           arg - �̺߳�������        
Output:         ��
Return:         ��
Date:           20150518
***************************************************************/
void CPreAnalysis::QueryAccessDev()
{
	for(Byte index = 0 ;index <MAX_PHASE ;index++)
	{
		if(sPreAnalysisParaData[index].IsUsed == true)
		{
			//��ÿ���豸���Ͳ�ѯ���� 1��ֱ��һ��
		}
	}
}

/**************************************************************
Function:        PreAnalysisExceptionHandle
Description:    �����豸�쳣����
Input:           arg - �̺߳�������        
Output:         ��
Return:         ��
Date:           20150518
***************************************************************/
void CPreAnalysis::PreAnalysisExceptionHandle()
{

 	//�쳣����ӿں���
 	//�쳣�������ĳЩ�豸ͨѶ�жϡ����ݰ�У�����
}


/**************************************************************
Function:        AcceptDevCarsInfo
Description:    ���ս����豸���ݹ����ĳ�����Ϣ
Input:           arg - �̺߳�������        
Output:         ��
Return:         ��
Date:           20150527
***************************************************************/
void CPreAnalysis::AcceptDevCarsInfo()
{
	//�����豸���ݽ��պ����ӿ�
	//��Ҫ���մ�·���豸���͹����ĳ�����Ϣ��
}


/**************************************************************
Function:        HandPreAnalysis
Description:    ����
Input:           arg - �̺߳�������        
Output:         ��
Return:         ��
Date:           20150518
***************************************************************/
void CPreAnalysis::HandPreAnalysis()
{
	CManaKernel * pManaKernel = CManaKernel::CreateInstance();
	Byte CurStageTotalTime = 0x0 ;
	Byte CurStageElapseTime = 0x0 ;	
	Byte NextStageAllowPhases = 0x0 ;
	Byte NextStageGreenStepNum = 0x0 ;
	int NextStageGreenStepLength = 0x0 ;  

	int NextStageAllowPhasesMinTime = 0x0 ;
	int NextStageAllowPhasesMaxTime = 0x0 ;
	
	pManaKernel->GetStageTime(&CurStageTotalTime,&CurStageElapseTime);
	if(CurStageTotalTime-CurStageElapseTime <=0x2) //�׶�ʣ��2��ʱ���ڣ��¸���λ����Ԥ��ʱ��
	{
		for(Byte index = 0x0 ;index<MAX_SON_SCHEDULE ;index++)
		{
			if(pManaKernel->m_pRunData->ucStageNo+1 >= pManaKernel->m_pRunData->ucStageCount)
				pManaKernel->m_pRunData->ucStageNo = 0x0 ;
			if(pManaKernel->m_pRunData->sScheduleTime[index].ucScheduleId= pManaKernel->m_pRunData->ucStageNo+1)
			{
				NextStageAllowPhases = pManaKernel->m_pRunData->sScheduleTime[index].usAllowPhase ;
			}
			ACE_OS::printf("%s:%d Next Stage Phase :%d \r\n",__FILE__,__LINE__,NextStageAllowPhases);
			
			NextStageGreenStepNum = pManaKernel->StageToStep(pManaKernel->m_pRunData->ucStageNo);

			
			NextStageAllowPhasesMinTime = pManaKernel->GetCurStepMinGreen(NextStageGreenStepNum, &NextStageAllowPhasesMaxTime,NULL);
			if(pManaKernel->IsLongStep(NextStageGreenStepNum))
			{
				pManaKernel->m_pRunData->sStageStepInfo[NextStageGreenStepNum].ucStepLen = 0x0 ; //��һ�׶��̵�ʱ��ֵ
				for(Byte index = 0 ;index <MAX_PHASE ;index++)
				{
					if(sPreAnalysisParaData[index].IsUsed == true && sPreAnalysisParaData[index].CarNumber >0)
						if(NextStageAllowPhases>>(sPreAnalysisParaData[index].PhaseId-1 ==0x1))
						{
						   NextStageGreenStepLength = p_BusDelayTime + sPreAnalysisParaData[index].CarNumber*p_PerBusCrossTime ;
						   if(NextStageGreenStepLength <NextStageAllowPhasesMinTime )
						   	NextStageGreenStepLength = NextStageAllowPhasesMinTime ;
						   else if(NextStageGreenStepLength >NextStageAllowPhasesMaxTime )
						   	NextStageGreenStepLength = NextStageAllowPhasesMaxTime ;

							pManaKernel->m_pRunData->sStageStepInfo[NextStageGreenStepNum].ucStepLen = NextStageGreenStepLength ;
							break ;		//��ȡ��������λ�ĵ�һ����Ч�����豸
							
						}
				}
				
			}
		}

	}
	
		
}




