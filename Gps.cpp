
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   MainBoardLed.cpp
Date:       2013-1-1
Description:�źŻ�GPSУʱ����
Version:    V1.0
History:
***************************************************************/
#include "Gps.h"
#include "SerialCtrl.h"
#include "TscMsgQueue.h"
#include "GbtMsgQueue.h"
#include "ManaKernel.h"
#include <termios.h>
#include "MainBoardLed.h"


/**************************************************************
Function:        CGps::CGps
Description:     CGps�๹�캯������ʼ����
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
CGps::CGps()
{
    ACE_DEBUG((LM_DEBUG,"%s:%d Init GPS object !\r\n",__FILE__,__LINE__));
    ierrorcount = 0 ;
    m_bGpsTime = false ;
    m_tLastTi = 0 ;
    m_iGpsFd  = CSerialCtrl::CreateInstance()->GetSerialFd2();
    ACE_OS::memset(m_cBuf,0,128);
}

/**************************************************************
Function:       CGps::~CGps
Description:    CGps����������
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
CGps::~CGps()
{

    ACE_DEBUG((LM_DEBUG,"%s:%d Destruct CGps object!\r\n",__FILE__,__LINE__));

}

/**************************************************************
Function:       CGps::CGps
Description:    ����CGps�ྲ̬����
Input:          ��
Output:         ��
Return:         CGps��̬����ָ��
***************************************************************/
CGps* CGps::CreateInstance()
{
    static CGps cGps;
    return &cGps;
}

/**************************************************************
Function:       CGps::RunGpsData
Description:    GPS�߳���ں���
Input:          arg  Ĭ��NULL
Output:         ��
Return:         0
***************************************************************/
void CGps::RunGpsData()
{
    if ( m_iGpsFd < 0 )
    {
        ACE_DEBUG((LM_DEBUG,"\n%s:%d Open Gps Serial error!\n",__FILE__,__LINE__));
        return ;
    }
    char *pBuf   = m_cBuf;
    int iFlagGps = 0;
    tcflush(m_iGpsFd, TCIFLUSH); //��ջ�����,�����ܵ��ϴ�GPS���յ�����Ϣ����
    while ( true )
    {
        if ( ACE_OS::read(m_iGpsFd, pBuf,1) <= 0 )
        {
            ACE_OS::sleep(ACE_Time_Value(0,100*1000));
            ierrorcount ++ ;
            if(ierrorcount > 300) // 30�뻹��û�ж�ȡ��������һ����
            {
                ACE_DEBUG((LM_DEBUG,"\n%s:%d Cant read gps info than 300 times!\n",__FILE__,__LINE__));
                ierrorcount = 0 ;
                //return ; TODO  �����ȡ1000�ζ�û�ж�ȡ��GPS��Ϣ����˼�¼���ݿ���־�С�
                CMainBoardLed::CreateInstance()->SetSignalLed(LED_GPS,LED_STATUS_FLASH); //ADD:20141106 ��GPS������������˸
                ACE_OS::sleep(60);
            }
            continue;
        }
        ACE_OS::printf("%c",*pBuf);
        if ( '$' == *pBuf )
        {
            pBuf++;
            iFlagGps = 1;
        }
        else if ( 1 == iFlagGps )
        {
            if ( ('*' == *(pBuf-2) ) || ( (pBuf - m_cBuf) >= (int)sizeof(m_cBuf) - 1) )
            {
                *(pBuf+1) = '\n';
                if ( 0 == ACE_OS::strncmp(GPRMC, m_cBuf, ACE_OS::strlen(GPRMC)) )
                {
                    //$GPRMC,023543.00,A,2308.28715,N,11322.09875,E,0.195,,240213,,,A*78
                    ACE_DEBUG((LM_DEBUG,"\n%s,%d gps_read %s \n", __FILE__, __LINE__, m_cBuf));
                    if ( CheckSum(m_cBuf) )
                    {
                        Byte result = Extract();
                        if(result == 1)
                        {
                            CMainBoardLed::CreateInstance()->SetSignalLed(LED_GPS,LED_STATUS_ON); //ADD:20141106 ������������������
                            //day��ʾ���죬Ҳ�����߳����߼��졣1��ʾÿ�����Уʱ��2��ʾ���������Уʱ......
                            Byte day = CManaKernel::CreateInstance()->m_pTscConfig->sSpecFun[FUN_GPS_INTERVAL].ucValue;
                            m_bGpsTime = false ;
                            Uint m_iIntervalSec = day*3600*24 ;
                            m_iIntervalSec = 3*3600; //����Ĭ��3СʱУʱһ��
                            while(m_iIntervalSec/60 >0) //��ȡһ��������һ�Σ��ж��Ƿ���
                            {
                                if(m_tLastTi == 0)  //ǿ��Уʱ
                                    break ;
                                m_iIntervalSec -= 60 ;
                                ACE_OS::sleep(60);
                            }
                            m_bGpsTime = true ;
                            tcflush(m_iGpsFd, TCIFLUSH); //��ջ�����,�����ܵ��ϴ�GPS���յ�����Ϣ����
                        }
                    }
                    else
                    {
                        ACE_DEBUG((LM_DEBUG,"\n%s,%d gps_read checksum error! \n", __FILE__, __LINE__));
                    }
                }
                pBuf     = m_cBuf;
                iFlagGps = 0;
                ACE_OS::memset(m_cBuf,0,128);

            }
            else
            {
                pBuf++;
            }
        }
        else
        {
            pBuf = m_cBuf;
        }

    }

    return ;
}

/**************************************************************
Function:       CGps::Extract
Description:    ����GPS����
Input:          ��
Output:         ��
Return:         0-ʧ�� 1-�ɹ�
***************************************************************/
int CGps::Extract()
{
    char  cValid = 'V';
    char* pBuf   = m_cBuf;
    int   iYear  = 0;
    int   iMon   = 0;
    int   iDay   = 0;
    int   iHour  = -1;
    int   iMin   = -1;
    int   iSec   = -1;
    int   iMsec  = -1;
    int   iIndex = 0;

#ifndef WINDOWS
    sscanf(pBuf, GPRMC "," "%02d%02d%02d.%03d,%c,"  "%*s\r\n", &iHour, &iMin, &iSec, &iMsec, &cValid);

    for ( iIndex = 0; pBuf && iIndex < 9; iIndex++ )
    {
        pBuf = index(pBuf, ',');
        if ( pBuf == NULL )
        {
            return 0;
        }
        pBuf++;
    }

    if ( pBuf )
    {
        sscanf(pBuf, "%02d%02d%02d,%*s\r\n", &iDay, &iMon, &iYear);
    }

    if (! (iYear >= 0 && iYear <= 99 && iMon >= 1  && iMon <= 12 && iDay  >= 1  && iDay <= 31 && iHour >= 0 && iHour <= 23 && iMin >= 0  &&
           iMin <= 59 && iSec  >= 0  && iSec <= 59 ))

    {
        return 0;
    }

    if ( cValid != 'A' )
    {
        return 0;
    }
    SetTime(2000 + iYear, iMon, iDay, iHour, iMin, iSec );


#endif
    return 1;
}


/**************************************************************
Function:       CGps::CheckSum
Description:    У��GPS����
Input:          cMsg  GPS����ָ��
Output:         ��
Return:         false-ʧ�� true-�ɹ�
***************************************************************/
bool CGps::CheckSum(char *cMsg)
{
    char *pBuf         = NULL;
    int iChecksum      = 0;
    Byte ucSum         = 0;

#ifndef WINDOWS
    if ( NULL == cMsg )
    {
        return false;
    }

    if ( NULL == (pBuf = index(cMsg, '*')) )
    {
        return false;
    }

    if ( sscanf(pBuf, "*%X\r\n", &iChecksum) != 1 )
    {
        return false;
    }
    if ( '$' == *cMsg )
    {
        cMsg++;
    }
    while ( cMsg && *cMsg != '*' )
    {
        ucSum ^= *cMsg;
        cMsg++;
    }
    //ACE_DEBUG((LM_DEBUG,"receive checksum is %02X and the calculation is %02X\n", iChecksum, ucSum ));
#endif

    if ( ucSum == iChecksum )
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**************************************************************
Function:       CGps::SetTime
Description:    ����ϵͳʱ��
Input:          iYear-��    iMon-��      iDay-��
             iHour-ʱ    iMin-��      iSec-��
Output:         ��
Return:         ��
***************************************************************/
void CGps::SetTime(int iYear, int iMon, int iDay, int iHour, int iMin, int iSec)
{
    time_t Ttime;
    struct tm *pTheTime;
    SThreadMsg sTscMsg;

    Ttime = time(NULL);
    pTheTime = localtime(&Ttime);
    pTheTime->tm_year = iYear - 1900;
    pTheTime->tm_mon  = iMon - 1;
    pTheTime->tm_mday = iDay;
    pTheTime->tm_hour = iHour;
    pTheTime->tm_min  = iMin;
    pTheTime->tm_sec  = iSec;
    Ttime = mktime(pTheTime);
    m_tLastTi = Ttime ;
    char sgpstime[200]= {0} ;
    ACE_OS::sprintf(sgpstime,"echo LocalSysTime:$(date) GpsTimeUTC: %d-%d-%d %d:%02d:%02d >>GpsTime.info", iYear,iMon,iDay,iHour,iMin,iSec);
    ACE_OS::system(sgpstime);
    //ACE_OS::printf("%s:%d GpsTime: %d-%d-%d %d:%02d:%02d Gps2Localtime: %d-%d-%d %d:%02d:%02d\r\n",__FILE__,__LINE__,iYear,iMon,iDay,iHour,iMin,iSec,pLocalTime->tm_year+1900,pLocalTime->tm_mon+1,pLocalTime->tm_mday,pLocalTime->tm_hour,pLocalTime->tm_min,pLocalTime->tm_sec);
    sTscMsg.ulType       = TSC_MSG_CORRECT_TIME;
    sTscMsg.ucMsgOpt     = OBJECT_UTC_TIME;
    sTscMsg.uiMsgDataLen = 4;
    sTscMsg.pDataBuf     = ACE_OS::malloc(4);
    Ttime += 2*8*3600 ;  // ������Ҫ�������λ������λ������ʱ���ʱ��ȡֵ.
    *((Byte*)sTscMsg.pDataBuf+3)  = Ttime & 0xff;
    *((Byte*)sTscMsg.pDataBuf+2)  = (Ttime>>8) & 0xff;
    *((Byte*)sTscMsg.pDataBuf+1)  = (Ttime>>16) & 0xff;
    *((Byte*)sTscMsg.pDataBuf)    = (Ttime>>24) & 0xff;
    CTscMsgQueue::CreateInstance()->SendMessage(&sTscMsg,sizeof(sTscMsg));

}

/**************************************************************
Function:       CGps::ForceAdjust
Description:    ����ǿ��Уʱ    ����
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CGps::ForceAdjust()
{
    m_tLastTi = 0;
}
/**************************************************************
Function:       CGps::GetLastTi
Description:    ����GPSУʱ�Ƚ�ʱ��
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
time_t CGps::GetLastTi()
{
    return m_tLastTi ;
}



