#ifndef _IOOPERATE_H_
#define _IOOPERATE_H_

#include "ComStruct.h"

/*
*IO������ ��Ҫ��֤write��read�ĳɹ�ִ��
*/
class CIoOprate
{
public:
	CIoOprate();
	~CIoOprate();

	static bool TscWrite(int iDevFd , Byte* pWriteData , int iWriteCnt);
	static bool TscRead(int iDevFd , Byte* pReadData , int iReadCnt );

};


#endif   //_IOOPERATE_H_
