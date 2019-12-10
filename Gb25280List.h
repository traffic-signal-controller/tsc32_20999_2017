#ifndef _SGB25280_H_
#define _SGB25280_H_
#include "Define.h"
#include "ComDefGb25280.h"

/***********************����GB25280-2016��¼Э������֡�ṹ��********************/
struct SGB25280FrameTableData
{
	Byte ucVsersion;                        //Э��汾
	Byte ucSendFlag;                       //���ͷ���־
	Byte ucRecvFlag;                        //���շ���־
	Byte ucDataLinkCode;               //������·����
	Byte ucAreaCode;                      //�����
	unsigned short ucRoadCode;   //·�ں�
	Byte ucOperaType;                    //��������
	Byte ucObjCode;                        //��������
	Byte ucReserveDara[5];              //�����ֶ�
	Byte *pTableData;                       //��������ָ��
	Uint32 uiTableDataLen;              //�������ݳ���
};

class CLinklistGbData
{
private:
	pList pGbDataList;
public:
	CLinklistGbData();
	~CLinklistGbData();
	void LIST_Free();
	int LIST_Add(Byte Data);
	int LIST_GetCount();
	void LIST_Print();
	int LIST_Del( int pos);
	int Getelem(int pos, Byte *Item);
	pList GetpList();
};
#endif

