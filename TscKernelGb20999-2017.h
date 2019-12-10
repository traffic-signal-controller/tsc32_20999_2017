#ifndef _TSCKERNELGB20999_H_
#define _TSCKERNELGB20999_H_
#include "Define.h"
#include "Gb20999-2017List.h"
#include "Configure.h"

class CTscKernelGb20999
{
private:
    CTscKernelGb20999();
    ~CTscKernelGb20999();
public:
    sGB25280TscConfig  TscGB25280TscConfig ;
    static CTscKernelGb25280* CreateInstance();
    Byte  GetGbFrameCheckCode(CLinklistGbData  *pListFrameTableData); //��ȡУ����
    CLinklistGbData* BytesSendConstructedGB25580FrameData(SGB25280FrameTableData GbFrameDataSource);
    CLinklistGbData* BytesRecvDecodeGB25580FrameData(Byte RecvedGbFrameData[],Uint RecvDataLen);
    Byte GetTscCommunicationStatus();
    bool ListToBytesArray(CLinklistGbData * pLinkListGbData,Byte BytesArray[],int BytesArrayLen);
    Byte* ConstruectOnlineRequest(int & BytesArrayLen); //������������ָ������


    Byte* ConstruectOnlineQuery(int & BytesArrayLen); //����������ѯָ������

    Byte* ConstruectTrafficFlowSelfReport(int & BytesArrayLen); //���콻ͨ�������ϱ�ָ������

    Byte* ConstruectWorkStatusReply(int & BytesArrayLen); //���칤��״̬Ӧ��ָ������

    Byte* ConstruectWorkStatusSelfReply(int & BytesArrayLen); //���칤��״̬�ı������ϴ�ָ������

    Byte* ConstruectLampColorStatusReply(int & BytesArrayLen); //�����ɫ״̬Ӧ��ָ������

    Byte* ConstruectLampColorStatusSelfReply(int & BytesArrayLen); //�����ɫ״̬����Ӧ��ָ������

    Byte* ConstruectTimeQueryReply(int & BytesArrayLen); //����ʱ��Ӧ��ָ������

    Byte* ConstruectTimeSetReply(int & BytesArrayLen); //����ʱ��Ӧ��ָ������


    Byte* ConstruectLampGrpQueryReply(int & BytesArrayLen); //�����źŵ����ѯӦ��ָ������
    Byte* ConstruectLampGrpSetReply(int & BytesArrayLen,Byte*pData,int Len);       //�����źŵ�������Ӧ��ָ������


    Byte* ConstruectPlanPatternQueryReply(int & BytesArrayLen); //���췽�����ȼƻ���ѯӦ��ָ������

    Byte* ConstruectPlanPatternSetReply(int & BytesArrayLen,Byte*pData,int Len); //���췽�����ȼƻ�����Ӧ��ָ������


    Byte* ConstruectWorkTypeSetReply(int & BytesArrayLen); //���칤����ʽ����Ӧ��ָ������

    Byte* ConstruectWorkTypeQueryReply(int & BytesArrayLen); //���칤����ʽ��ѯӦ��ָ������

    Byte* ConstruectErrorQueryReply(int & BytesArrayLen); //�����źŻ����ϲ�ѯӦ��ָ������
    Byte* ConstruectErrorSelfReply(int & BytesArrayLen); //�����źŻ����������ϴ�ָ������

    Byte* ConstruectTscVersionQueryReply(int & BytesArrayLen); //�����źŻ��汾��ѯӦ��ָ������
    Byte* ConstruectscVersionSelfReply(int & BytesArrayLen); //�����źŻ��汾�����ϴ�ָ������


    Byte* ConstruectParameterVerQueryReply(int & BytesArrayLen); //�������������汾��ѯӦ��ָ������
    Byte* ConstruectParameterVerSetReply(int & BytesArrayLen); //����������������Ӧ��ָ������

    Byte* ConstruectTscIDQueryReply(int & BytesArrayLen); //�����źŻ�ʶ��汾��ѯӦ��ָ������

    Byte* ConstruectTscRemoteCtrlReply(int & BytesArrayLen); //�����źŻ�Զ������Ӧ��ָ������


    Byte* ConstruectDetectorQueryReply(int & BytesArrayLen); //����������ѯӦ��ָ������
    Byte* ConstruectDetectorSetReply(int & BytesArrayLen,Byte*pData,int Len);  //������������Ӧ��ָ������


    Byte* ConstruectStagePatternQueryReply(int & BytesArrayLen); //�����ź����η�����ѯӦ��ָ������
    Byte* ConstruectStagePatternSetReply(int & BytesArrayLe,Byte*pData,int Lenn); //�����ź���ʱ����Ӧ��ָ������


    Byte* ConstruectPhaseQueryReply(int & BytesArrayLen); //������λ��ѯӦ��ָ������
    Byte* ConstruectPhaseSetReply(int & BytesArrayLen,Byte*pData,int Len); //������λ����Ӧ��ָ������


    Byte* ConstruectErrorReport(int & BytesArrayLen,Byte ErrorCode,Byte ErrorObject);  //�������Ӧ��ָ������
    void SendBytesGbDataToClient(Byte *pBytes,int BytesLen);
    bool CheckGB25280Msg(Byte ucDealDataIndex,Uint iBufLen,Byte* pBuf); // ���GB25280���ݺϷ���
    void  DealGb25280RecvBuf(Byte ucDealDataIndex) ;   //������յ���GB28250Э������();
    void SelfReportLampGroupColor();         //�����ϴ������ɫ�仯
    void SelfReportWorkStatus();                //�����ϴ�����״̬
    void SelfReportTscError();                    // �����ϴ��źŻ�����
};

#endif

