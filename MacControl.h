#ifndef __MACCONTROL__H__
#define __MACCONTROL__H__

#include "ComDef.h"

#include "ComStruct.h"

class CMacControl
{
public:
	static CMacControl* CreateInstance();

	//���ذ��������ģ�鷢�ͻ�������
	void GetEnvSts();

	//���ذ巢�͸�����ģ����������
	void SetCfg1();

	//���ذ巢�͸�����ģ����������
	void SetCfg2();

	//���ذ�ֱ�ӿ���ģ�����豸
	void MainBoardCtrl();
	void RecvMacCan(SCanFrame sRecvCanTmp); //ADD 20130815 0850
	void SndLcdShow() ; //ADD 201309101706
	Byte GetCtrlStaus() ; //ADD 201309101400
	void GetMacControlVer();

private:
	CMacControl();
	~CMacControl();
		
	Byte LcdCtrlMod ;          //��ǰ����ģʽ״̬
	
	int m_iSetAddHotTemp;  //�������������¶�ֵ
	int m_iSetReduHotTemp; //ɢ�����������¶�ֵ
	Byte m_ucSetHumWarn;     //ʪ��Ԥ����ֵ ʵ�ʵİٷֱ���ֵ
	Byte m_ucSetAddHotPlan;  //������Ԥ�� 0-���� 1-�Զ����� 2-��������� 
	Byte m_ucSetReduHotPlan; //ɢ����Ԥ�� 0-���� 1-�Զ����� 2-���������
	Byte m_ucSetCabinetWarn; //������   0-���� 1-�𶯱����� 2-�𶯱����ر�
	Byte m_ucSetHumPlan;     //ʪ��Ԥ��   0-���� 1-�Զ���ɢ�� 2-�Զ��򿪼��� 3-����

	int m_iGetAddHotTemp; 
	int m_iGetReduHotTemp;  
	Byte m_ucGetHumWarn;      
	Byte m_ucGetAddHotPlan;   
	Byte m_ucGetReduHotPlan;  
	Byte m_ucGetCabinetWarn;  
	Byte m_ucGetHumPlan; 

	//in
	Byte m_ucSetLightPlan;  //�����ƿ���Ԥ��
	Byte m_ucSetDoorPlan;   //�ſ���
	Byte m_ucSetLedLight;   //LCD����
	Byte m_ucSetLedDisplay; //LCD��ʾ

	Byte m_ucGetLightPlan;  
	Byte m_ucGetDoorPlan;    
	Byte m_ucGettLedLight;   
	Byte m_ucGettLedDisplay; 

	//in
	Byte m_ucSetMainAddHot;  //������
	Byte m_ucSetMainReduHot; //ɢ����
	Byte m_ucSetMainWarn;    //���ر�����
	Byte m_ucSetMainLight;   //�����豸
	Byte m_ucSetFarIo1;      //Զ��1
	Byte m_ucSetFarIo2;      //Զ��2

	Byte m_ucGetMainAddHot;   
	Byte m_ucGetMainReduHot; 
	Byte m_ucGetMainWarn;     
	Byte m_ucGetMainLight;    
	Byte m_ucGetFarIo1;       
	Byte m_ucGetFarIo2;      
	bool bSendCtrlOk ; //�ж��Ƿ���Ʒ���LCD��ʾ�ɹ�

	Byte uiOldLcdCtrl ;
	Byte uiOldPatternNo  ;
	Byte uiOldFlashType ;
	bool uiOldbDegrade ; 
		
public:
	//out
	//Byte m_ucManualSts;  //�ֿ�״̬
	//Byte m_ucManual;     //0�Զ����� 1�ֶ�����
	Byte m_ucDoorFront;  //ǰ�� 0�ر� 1��
	Byte m_ucDoorBack;   //����
	Byte m_ucLightDev;   //�����豸
	Byte m_ucWarnDev;    //���ر�����

	Byte m_ucTemp;   //��ǰ�¶�
	Byte m_ucHum;    //��ǰʪ��

	Byte m_ucFarOut1;  //Զ�������1
	Byte m_ucFarOut2;  //Զ�������2
	Byte m_ucFarIn1;   //Զ�������1
	Byte m_ucFarIn2;   //Զ�������2

	Byte m_ucAddHot;   //������
	Byte m_ucReduHot;  //ɢ����
	Byte m_ucCabinet;  //����  0��ֹ 1��
	Byte m_ucMacContolVer[5]; // ����������汾
	Byte m_ucPsc ;  //���˰�ťֵ
	
	
};


#endif //__MACCONTROL__H__
