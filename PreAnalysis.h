#ifndef _PREANALYSIS_H_
#define _PREANALYSIS_H_

#include "ComDef.h"
#include "ComStruct.h"

class CPreAnalysis
{
public:
	static CPreAnalysis* CreateInstance();	
	bool b_SmoothTransition ; //�Ƿ�ƽ������ ÿ�����Ӽ���ʱ�������ƻ���ֱ�Ӱ������ȼ���ʱ��
	bool b_HandPriority   ; //�Ƿ��ڹ������ȵ�����
	Byte p_BusDelayTime   ; //Ĭ�ϳ�������Ԥ��ʱ��
	Byte p_PerBusCrossTime ; //����ͨ��·��ƽ��ʱ��
	Byte p_AccessDeviceType ; //�����豸���� 0-��Ȧ 1-��Ƶ 2-�״�3-����
	void QueryAccessDev(); //��ѯ�����豸��Ϣ
	void HandPreAnalysis();
	void PreAnalysisExceptionHandle(); //�쳣�����������ͨ���жϡ
	void AcceptDevCarsInfo();
private:
	CPreAnalysis();
	~CPreAnalysis();
	SPreAnalysisParaData sPreAnalysisParaData[MAX_PHASE] ;
};

#endif 


