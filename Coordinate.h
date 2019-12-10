#ifndef _WIRELESSCOORD_H_
#define _WIRELESSCOORD_H_

#include "ComStruct.h"
#include "ComDef.h"


/*
*�޵�����Э��
*/
class CWirelessCoord
{
public:
	void SetStepInfo(bool bUtcsPara,int iStepNum , int iCycle , int iOffset , int* iStepLen );
	void GetDeflection();
	void GetAdjust();
	//�ж��Ƿ�����Ҫ����������Ҫ�����������Ҫ����
	Uint IsMasterMachine();
	//ͬ����Ҫ��������Ϣ
	void SyncSubMachine();
	void OverCycle();
	int GetStepLength(int iCurStepNo);

	void ForceAssort();
    void SetDegrade();       //��������ʱ���ò���
	static CWirelessCoord* CreateInstance();

private:
	CWirelessCoord();
	~CWirelessCoord();

private:
	//bool m_bForceAssort;     //ǿ��Э��
	bool m_bUtcs;             //����Э��
	bool m_bPlus;             //��������
	bool m_bMaster;
	int m_iUtsCycle;       //���Ĺ�������ʱ��
	int m_iUtscOffset;     //����Э������λ��
	
	int m_iCycle;          //�źŻ��������е�����                          
	int m_iOffset;         //�źŻ�����Э������λ��
	int m_iAdjustCnt;      //�ܵ���ʱ��
	int m_iStepNum;                //��������
	int m_iStepLen[MAX_STEP];      //������
	int m_iAdjustSecond[MAX_STEP]; //������������
	time_t m_tLastTi;              //�ϴ�Э����ϵ�ʱ��
};

#endif  //_WIRELESSCOORD_H_
