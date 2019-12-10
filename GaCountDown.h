#ifndef _GACOUNTDOWN_H_
#define _GACOUNTDOWN_H_

#include "ComStruct.h"

#define GACOUNTDOWN_CPP "GaCountDown.cpp"

const int GA_MAX_DIRECT    = 8;   //����� �� �� �� �� 
const int GA_MAX_LANE      = 10;   //����·���� �� ֱ �� ���ж��ι��� ��ͷ ���� �����
const int GA_MAX_SEND_BUF  = 5;  //����ʱЭ�鶨���ÿ֡�����ֽ���

/*
*������������ʱ��(����GAT508-2004Э��)
*ÿ������ֻ��һ������ʱ��  Ϊ�����е���ʱ���ڿ��Ƿ�Χ

*������������ʱ��(����GAT508-2014Э��)
*���֧��32������ʱ,֧�ֵ�ַ��Χ0-31 ,�����¹��굹��ʱ����32��
���򵹼�ʱ֧��,�뷽���й�,�������λ

*/


enum
{
	GA_DIRECT_NORTH  = 0 ,  //��
	GA_DIRECT_EAST   = 1 ,  //��
	GA_DIRECT_SOUTH  = 2 ,  //��
	GA_DIRECT_WEST   = 3 ,  //��
	GA_DIRECT_NORTH_EAST  = 4 , //����
	GA_DIRECT_SOUTH_EAST  = 5 , //����
	GA_DIRECT_SOUTH_WEST  = 6 , //����
	GA_DIRECT_NORTH_WEST  = 7  //����
	
};

enum
{
	GA_LANE_LEFT     = 0 ,  //��   
	GA_LANE_STRAIGHT = 1 ,  //ֱ
	GA_LANE_RIGHT    = 2 ,  //��
	GA_LANE_FOOT     = 3 ,  //һ������	
	GA_LANE_SECOND_FOOT    = 4 , //��������
	GA_LANE_TRUNAROUND     = 5 , //��ͷ	
	GA_LANE_LEFT_RIGHT_STRAIGHT = 6 , //��ֱ��(��λ�����ⷽ��)	
	GA_LANE_OTHER               = 7, // ����	
	GA_LANE_LEFT_STRAIGHT       = 8 , //��ֱ
	GA_LANE_RIGHT_STRAIGHT      = 9  //��ֱ
};

enum
{
	
	GANEW_COLOR_OTHER  = 4 , //������ɫ
	GANEW_COLOR_RED    = 3 , //��
	GANEW_COLOR_YELLOW = 2 , //��
	GANEW_COLOR_GREEN  = 1 , //��
	GANEW_COLOR_BLACK  = 0  //��ɫ
};


class CGaCountDown
{
public:
	static CGaCountDown* CreateInstance();

	void GaGetCntDownInfo();
	void GaUpdateCntDownCfg();
	void GaSendStepPer();

	 

	CGaCountDown();
	~CGaCountDown();

	Byte GaGetCntTime(Byte ucSignalId);
	void GaGetDirLane(Byte ucTableId , Byte& ucDir , Byte& ucLane);
	void GaSetSendBuffer();	
	void GaSetSendBufferNewGB(); //ADD:20150324	
	void GaSetSendBufferNewGB4Direc();  //ADD:20150325
	
	bool PackSendBuf( Byte ucDirIndex , Byte ucColor , Byte ucCountTime );	
	bool PackSendBufNewGB4Direc( Byte ucDirIndex , Byte ucColor , Byte ucCountTime );
	void sendblack();    //���ͺ�����Ϣ	
	
	void send8cntdown(); //����8�뵹��ʱ
	void SetClinetCntDown(ACE_INET_Addr& addrClient, Uint uBufCnt , Byte* pBuf) ; //ADD:201401121705
	
	
	bool PackSendBufNewGB();	
	bool ComputeColorCountNewGB();
	
	bool ComputeColorCount( Byte ucDirIndex , Byte& ucColor , Byte& ucCountTime );

	bool m_bGaNeedCntDown[GA_MAX_DIRECT][GA_MAX_LANE]; //��Ҫ����ʱ�ķ���
	Byte m_ucGaRuntime[GA_MAX_DIRECT][GA_MAX_LANE];    //�õ�ɫ����ά�ֵ�ʱ��
	Byte m_ucGaColor[GA_MAX_DIRECT][GA_MAX_LANE];      //��ɫ
	GBT_DB::PhaseToDirec m_sGaPhaseToDirec[GA_MAX_DIRECT][GA_MAX_LANE];
	SendCntDownNum sSendCDN[MAX_CLIENT_NUM] ;  //ADD:201401121705

	Byte m_sGaSendBuf[GA_MAX_DIRECT][GA_MAX_SEND_BUF];   //���ͻ���
	Byte m_ucLampBoardFlashBreak[MAX_LAMP_BOARD]; //201407231400
	Byte m_newsGaSendBuf[MAX_CNTDOWNDEV*2+4]; //��2014����ʱ��׼��������4+32*2 	
	Byte m_newsGaSendBuf4D[12] ; ////��2014����ʱ��׼��������4����
	SNewGBCntDownTimeInfo m_newGbCntDownDevTime[MAX_CNTDOWNDEV] ;//��2014����ʱ��׼ÿ������ʱ����ʱ��
private:
};

#endif //_GACOUNTDOWN_H_
