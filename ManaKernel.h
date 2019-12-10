#ifndef _MANAKERNEL_H_
#define _MANAKERNEL_H_

#include "ComStruct.h"
#include "Coordinate.h"


class CManaKernel
{
public:
	static CManaKernel* CreateInstance();
	
	void InitWorkPara();
	void DecTime();
	void GoNextStep();
	void OverCycle();
	void SwitchStatus(unsigned int uiWorkStatus);
	void SwitchCtrl(unsigned int uiCtrl);
	void SetUpdateBit();
	void LockStage(Byte ucStage);
	void LockStep(Byte ucStep);
	void LockPhase(Byte PhaseId);
	void ReadTscEvent();
	void CorrectTime(Byte ucType,Byte* pValue);
	void GetTscStatus(Byte ucDealDataIndex);
	void GetTscExStatus(Byte ucDealDataIndex);

	void SetRestart();
	bool GetRestart();
	void RestartTsc();
	bool IsLongStep(int iStepNo);
	int GetCurStepMinGreen(int iStepNo , int* iMaxGreenTime, int* iAddGreenTime);
	void GetVehilePara(bool* pbVehile , bool* bDefStep , int* piAdjustTime , Uint* piCurPhase 
		                              , Uint* piNextPhase, SPhaseDetector* psPhaseDet);
	void AddRunTime(int iAddTime,Byte ucPhaseIndex);
	int GetMaxStageMinGreen(int iStepNo);
	void SpecTimePattern(int iTimePatternId );
	bool IsMainPhaseGrp(Uint uiPhase);
	void LockNextStage();
	void GetOverlapPhaseIndex(Byte iPhaseId , Byte* ucCnt , Byte* pOverlapPhase);
	bool IsInChannel(int iChannelId);
	void DealGreenConflict(Byte ucdata);
	void GetSignalGroupId(bool bIsAllowPhase,Byte ucPhaseId,Byte* pNum,Byte* pSignalGroup);
	void SetCycleStepInfo(Byte ucLockPhase = 0);
	Byte StageToStep(int iStageNo);
	Byte StepToStage(int iStepNo,bool* bStageFirstStep);
	void GetStageTime(Byte* pTotalTime,Byte* pElapseTime);
	
	bool InPhaseDetector(Byte ucDetId);
	void UpdateConfig();
	void GetRunDataStandard();
	void SetCycleBit(bool bSetCycle);

	bool IsVehile(); 		 //ADD: 201307241020
	void ChangePatter(Byte iParama);	 //ADD: 201309181530
	void SndMsgLog(Byte ucLogType,Byte ucLogVau1,Byte ucLogVau2,Byte ucLogVau3,Byte ucLogVau4); //ADD:201309251100
	void  SetWirelessBtnDirecCfg(Uint RecvBtnDirecData , Byte Lampcolor); //ADD:201410211700
	void CwpmGetCntDownSecStep();	
	Byte GetStageMaxDetectorCars(Uint32 StageAllowPhases); //��ȡ�׶���λ����������Ŷӳ���ADD 201508171514
	
	Byte GetMaxGreen2(int iStepNo); //��ȡ���������2



	bool m_bFinishBoot;         //pscģʽʹ�ñ��� ������ɹ��Ȳ��������Խ���pscģʽ
	STscConfig* m_pTscConfig;   //�źŻ�������ϢA
	STscRunData* m_pRunData;    //�źŻ���̬������Ϣ
	/*************�����ض���ʱ������ִ��*************/
	int m_iTimePatternId;    //ָ������ʱ������  �Ϸ�ֵ1-32 0û��ָ��
	
	bool bNextDirec ;      //��һ����
	bool bTmpPattern ;     //��ʱ����
	bool bUTS ;            //����ƽ̨��������
	bool bDegrade ;        //�Ƿ񽵼�
	//bool bChkManul ;       //�����ж�ϵͳ������ʼ�ֿ�״̬NEW:��Ƭ������
	bool bSecondPriority ; //���ڴ��߰��Ӧ�����߼��ж�  ADD��20140618
	private:
	CManaKernel();
	~CManaKernel();
	void SetPhaseColor(bool bOverPhase,Byte iPhaseId);
	void SetLampColor(Byte ColorType);
	void SetDirecChannelColor(Byte iDirecType);
	bool GaGetDirLane(Byte ucTableId , Byte  ucDir );
	void SelectDataFromDb();
	void ResetRunData(Byte ucTime);
	Byte GetScheduleId(Byte ucMonth,Byte ucDay,Byte ucWeek);
	Byte GetTimePatternId(Byte ucScheduleId,Byte* ucCtrl,Byte* ucStatus);
	Byte GetScheduleTimeId(Byte ucTimePatternId,Byte& ucCycle,Byte& ucOffSet);
	bool GetSonScheduleTime(Byte ucScheduleTimeId);
	void SetStepInfoWithStage(Byte ucCurStageIndex,Byte* ucCurStepIndex
		                                        ,SScheduleTime* pScheduleTime);
	void GetPhaseStepTime(Byte ucPhaseId,SScheduleTime* pScheduleTime,Byte* pTime,Byte ucCurStageIndex);
	Byte GetPhaseStepLen( SPhaseStep* pPhaseStep );
	Byte GetPhaseStepIndex( SPhaseStep* pPhaseStep );

	void GetPhaseStatus(SPhaseSts* pPhaseSts);
	void GetOverlapPhaseStatus(SOverlapPhaseSts* pOverlapPhaseSts);
	void GetChannelStatus(SChannelSts* pChannelSts);
	Byte GetPhaseIdWithChannel(Byte ucChannelNo);

	/*
	void GetDetectorSts(SDetectorStsPara* pDetStatus);
	void GetDetectorData(SDetectorDataPara* pDetData);
	void GetDetectorAlarm(SDetectorAlarm* pDetAlarm);
	*/

	void GetCurStageLen(Byte* pCurStageLen);
	void GetCurKeyGreenLen(Byte* pCurKeyGreen);
	void SetDetectorPhase();
	void GetStageDetector(int iStageNo);
	void GetAllStageDetector();
	
	bool InConflictPhase();
	void AllotActiveTime();
	void SetRedOtherLamp(Byte* ucLampOn,Byte* ucLampFlash);
	void UtcsAdjustCycle();

	void GetOverlapPhaseStepTime( 
								  Byte ucCurStageIndex
		                        , Byte ucOverlapPhaseId 
		                        , Byte* pIncludeTime
		                        , Byte* pOverlapTime
								, Byte ucPhaseIndex
								, Byte ucStageYellow);

	void GetUseLampBoard(bool* bUseLampBoard);
	bool GetUseLampBoard(int iLampBoard);
	Byte OverlapPhaseToPhase(Uint uiCurAllowPhase,Byte ucOverlapPhaseIndex);
	int  GetStageMinGreen(Ushort usAllowPhase);
	bool ExitOvelapPhase(Byte ucOverlapPhaseId,Byte ucPhaseCnt,SPhaseStep* pPhaseStep);
	bool ExitStageStretchPhase(SScheduleTime* pScheduleTime);
    
	

	void ValidSoftWare() ;  //ADD 201310221450
	void DealPSC();          //ADD 20160923   
	
	bool m_bRestart;        //�Ƿ����� gbtЭ����Բ���
	bool m_bWaitStandard;   //�ȴ������������׼
	bool m_bSpeStatusTblSchedule;  //ʱ�α��ﶨ������⹤��״̬
	bool m_bVirtualStandard;      //�����������׼״̬
	bool m_bSpeStatus;            //��ʾ����״̬ ��������״̬-->STANDARD����ˢ�²���

	

		/**************�ض���λ����*****************/
	bool m_bSpePhase;           //�ض���λ����
	Byte m_ucLampOn[MAX_LAMP]; //��������Ӷ�Ӧ ����1 ��0
	Byte m_ucLampFlash[MAX_LAMP]; 

	/************************��Ӧ���Ʋ���***************************/
	//bool m_bPhaseDetCfg;  //�Ƿ�����λ�������Ķ�Ӧ����
	bool m_bAddTimeCount; //��Ӧ��������ʱ���Ƿ񵹼�ʱ
	bool m_bVehile;        //�Ƿ��Ӧ����
	Byte m_ucAddTimeCnt;  //һ���׶����ӵ�ʱ���ܺ�
	int  m_iAdjustTime;   //����ʱ�� ��λ�̵��ӳ�ʱ��
	int  m_iMinStepTime;  //������С��
	int  m_iMaxStepTime;  //���������

	bool bValidSoftWare ;

	Byte iCntFlashTime ;

public:
	Uint m_uiStagePhase[MAX_SON_SCHEDULE];  //ÿ���׶ζ�Ӧ����λ      ��ͨ��λ0-15
	SPhaseDetector m_sPhaseDet[MAX_PHASE]; //��λ��������ϵ ��ͨ��λ0-15

	bool m_bCycleBit;              //������һ��������λ  pscʹ��
	bool m_bNextPhase ;
	/*************Ⱥ�̲�����********************/
	Byte m_ucUtcsComCycle;    //���Ĺ�������ʱ��
	Byte m_ucUtscOffset;      //����Э����λ��

	/*****************************************/
	int m_ucStageDynamicMinGreen[MAX_SON_SCHEDULE] ; //��̬Ԥ�������ƽ׶ζ�̬��С��
	int m_ucStageDynamicMaxGreen[MAX_SON_SCHEDULE] ; //��̬Ԥ�������ƽ׶ζ�̬�����
	int m_ucStageDynamicAddGreen[MAX_SON_SCHEDULE] ;
	/*****************************************/
	
	//ACE_Thread_Mutex m_mutexSetCycle;
	//lastUcTimePatternId ��һ�����ڵ����� �����뵱ǰ�Ƿ�һ�¡�
	Byte lastUcTimePatternId;
	ACE_Thread_Mutex  m_mutexRunData;

	//CWirelessCoord m_cableless;     //�޵����¿���
}; 

#endif  //_MANAKERNEL_H_

