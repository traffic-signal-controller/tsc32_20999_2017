#ifndef _MAINBOARDLED_H_
#define _MAINBOARDLED_H_

#include "ComStruct.h"
#define MAXLED 10   //���LED����ʾ��Ŀ ADD:20141106


/*********LED������ʾ״̬ö��********20141106*/
enum{

		LED_STATUS_MAINTAIN = 0x0 ,  // ����ԭ����ʾ����
		LED_STATUS_OFF      = 0x1 ,  //LED��Ϩ��
		LED_STATUS_ON       = 0x2 ,  //LED����
		LED_STATUS_FLASH    = 0x3    //LED����˸

};



/*********ģ��LED��ö��*******************20141106*/

enum{

		LED_RADIATOR    = 0x0 ,  // ɢ����LED��
		LED_HEATER      = 0x1 ,  //������LED��
		LED_ILLUMINATOR = 0x2 ,  //������LED��
		LED_NETWORK     = 0x3 ,  //����LED��
		LED_3G    		= 0x4 ,  //3G  LED��
		LED_MSG         = 0x5 ,  //����MSG LED��
		LED_CAM         = 0x6 ,  //CAM LED��		
		LED_WIRELESSBUTTON       = 0x7 ,  //�����ֿ�LED
		LED_GPS         = 0x8 ,   //GPSLED��
		LED_YWFLASH     = 0x9    //������LED��		

};

class CMainBoardLed
{
public:
	static CMainBoardLed* CreateInstance();

	void DoModeLed(bool bLed3Value,bool bLed4Value);
	void DoTscPscLed(bool bValue);
	void DoAutoLed(bool bValue);
	void DoRunLed();
	void DoCan0Led();
	void DoCan1Led();
	bool IsEthLinkUp() ;
	void SetLedBoardShow();//ADD 2013 0809 15 40
	void DoLedBoardShow(); //ADD: 2013 0809 1700
	void SetSignalLed(Byte LedIndex ,Byte LedStatus); //ADD:201411060926
	
	void RecvMainBdLedCan(SCanFrame sRecvCanTmp);
	void GetMBLedVer();
	Byte m_ucMBLedVer[5];
private:
	CMainBoardLed();
	~CMainBoardLed();
	void OpenDev();
	void CloseDev();
	
private:
	bool can0Bool;
	bool can1Bool;
	Byte LedBoardStaus[MAXLED]; //ADD 2013 0809 15 40
	
};

#endif /*_MAINBOARDLED_H_*/
