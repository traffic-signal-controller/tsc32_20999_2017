#ifndef _DBINSTANCE_H_
#define _DBINSTANCE_H_

#include "GbtMsgQueue.h"
#include "GbtDb.h"

/*
*���ݿ������
*/
class CDbInstance
{
public:
	CDbInstance();
	~CDbInstance();

	static GBT_DB::CGbtTscDb m_cGbtTscDb; 
};

#endif //_DBINSTANCE_H_

