#ifndef _GB_H_
#define _GB_H_

static void* SignalMsgQueue(void* arg);  //�����źŻ�������Ϣ����
static void* GbtMsgQueue(void* arg);     //����gbt��Ϣ�������
static void* BroadCast(void* arg);       //�����㲥�߳� 
void RunGb();
void StartBeep();


#endif   //_GB_H_ 
