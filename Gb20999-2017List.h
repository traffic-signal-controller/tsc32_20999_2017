#ifndef  _SGB209992017_H_
#define  _SGB209992017_H_
#include "Define.h"
#include "ComDefGb25280.h"

/***********************����GB20999-2017��¼AЭ������֡�ṹ��********************/
struct SGB209992017FrameTableData
{
	Byte ucFrameBeginFlag;                  //Э�鱨�Ŀ�ʼ��־ 0X7E
	unsigned short ucFrameLength;                       //Э�鱨�ĳ���
	unsigned short ucFrameVersion            //Э��汾��
	Byte ucTscClientId;                        //��λ��ID
	Uint32 uiTscId;              //�źŻ�ID
	Byte ucRoadId;               //·��ID
	Byte ucFrameFlowID;          //֡��ˮ��
	Byte ucFrameType;            //֡����
	Byte ucDataFrameCount;       //����ֵ����
	Byte ucDataFrameIndex;       //����ֵ����
	Byte ucDataFrameLength;      //����ֵ����
	Byte ucDataClassId;          //������ID	
	Byte ucDataObjectId;         //����ID
	Byte ucDataAttributeId;      //����ID	
	Byte ucDataElementId;        //Ԫ��ID	
	Byte *pTableData;            //����ֵ
	unsigned short ucFrameCrcData         //CRC-16У��ֵ
	Byte ucFrameEndFlag;                  //Э�鱨�Ľ�����־ 0X7D
};

class CLinklistGb209992017Data
{
private:
	pList pGbDataList;
public:
	CLinklistGb209992017Data();
	~CLinklistGb209992017Data();
	void LIST_Free();
	int LIST_Add(Byte Data);
	int LIST_GetCount();
	void LIST_Print();
	int LIST_Del( int pos);
	int Getelem(int pos, Byte *Item);
	pList GetpList();
};
#endif

