
/***************************************************************
Copyright(c) 2013  AITON. All rights reserved.
Author:     AITON
FileName:   ComFunc.cpp
Date:       2013-9-25
Description:ͨ�ô�����
Version:    V1.0
History:    201310081456  yiodng testmem()
***************************************************************/
#include <sys/ioctl.h>
#include <fcntl.h>
#include "ComFunc.h"
#include "ManaKernel.h"
#include "DbInstance.h"

/**************************************************************
Function:       IsLeapYear
Description:    �ж��Ƿ�ʱ����
Input:          ucYear  ���
Output:         ��
Return:         false-������ true-����
***************************************************************/
bool IsLeapYear(Uint ucYear)
{
    if ( ( 0==ucYear%4 && ucYear%100!=0)||(0==ucYear%400) )
    {
        return true;
    }
    return false;
}

/**************************************************************
Function:       IsRightDate
Description:    �ж����ڵ��Ϸ���
Input:          ucYear  ���
                ucMonth �·�
                ucDay   ����
Output:         ��
Return:         false-������ true-����
***************************************************************/
bool IsRightDate(Uint ucYear,Byte ucMonth,Byte ucDay)
{
    Byte ucMaxDay = 31;

    if ( ucYear < 2000 || ucYear >= 2038 )
    {
        return false;
    }
    if ( ucMonth < 1 || ucMonth > 12 )
    {
        return false;
    }

    switch ( ucMonth )
    {
        case 2:
            if ( IsLeapYear(ucYear) )  //����
            {
                ucDay = 29;
            }
            else
            {
                ucDay = 28;
            }
            break;
        case 4:
            ucMaxDay = 30;
            break;
        case 6:
            ucMaxDay = 30;
            break;
        case 9:
            ucMaxDay = 30;
            break;
        case 11:
            ucMaxDay = 30;
            break;
        default:
            break;
    }

    if ( ucDay > ucMaxDay )
    {
        return false;
    }
    return true;

}

/**************************************************************
Function:       getCurrTime
Description:    ��ȡ��ǰʱ�䣬���ʱ���ȡ������Ҫ�����źŻ���ʱ����
                �����޸�ʱ�䵽ʱ����ɶ�ʱ��ֹͣ
Input:          ��
Output:         ��
Return:         ACEʱ��
***************************************************************/
ACE_Time_Value getCurrTime()
{
    ACE_Time_Value tv;
    struct timespec ts = {0};
    ::clock_gettime(CLOCK_MONOTONIC, &ts);
    ACE_hrtime_t hrt = static_cast<ACE_hrtime_t> (ts.tv_sec) * ACE_U_ONE_SECOND_IN_NSECS+ static_cast<ACE_hrtime_t> (ts.tv_nsec);
    ACE_High_Res_Timer::hrtime_to_tv (tv, hrt);
    return tv;
}

/**************************************************************
Function:       GetCurTime
Description:    ��ȡ��ǰʱ�䣬���ڳ���ʱ��ʱ���ȡ�������ط�
Input:          ��
Output:         ��
Return:         ACEʱ��
***************************************************************/
ACE_Time_Value GetCurTime()
{
#ifdef LINUX
    time_t ti;
    struct tm rtctm;
    int fd = open(DEV_RTC, O_WRONLY, 0);
    int ret = -1;
    if(fd>0)
    {
        ret = ioctl(fd, RTC_RD_TIME, &rtctm);
        close(fd);
    }
    ti = mktime(&rtctm);
    return ACE_Time_Value(ti);
#else
    return ACE_OS::gettimeofday();
#endif
}

/**************************************************************
Function:       CopyFile
Description:    �����ļ�
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void CopyFile(char src[], char dest[])
{
#ifdef LINUX
    int sd, dd;
    char buf[100];
    int n;

    if (strcmp(dest, src) == 0)
        return;

    sd = open(src, O_RDONLY);
    dd = open(dest, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (sd == -1 || dd == -1)
        goto out;

    while ((n = read(sd, buf, sizeof(buf))) > 0)
    {
        write(dd, buf, n);
    }

out:
    if (sd != -1)
        close(sd);
    if (dd != -1)
        close(dd);
#endif
}


/**************************************************************
Function:       CopyFile
Description:    �����ļ��Ĵ�С������iMaxFileLineʱ��ɾ����һ�룬
                ��ֹ�ļ�����
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void AdjustFileSize(char* file,int iMaxFileLine)
{
#ifdef LINUX
    FILE *fp;
    FILE *fp2;
    int line = 0;
    int ch;
    if ((fp = fopen(file, "r")) == NULL)
    {
        return;
    }
    while ( !feof(fp) )
    {
        ch = fgetc(fp);
        if (ch == '\n')
        {
            line++;
        }
    }
    if ( line < iMaxFileLine )
    {
        fclose(fp);
        return;
    }
    fseek(fp, 0, SEEK_SET);
    while (!feof(fp) && ( line > iMaxFileLine / 2 ) )
    {
        ch = fgetc(fp);
        if (ch == '\n')
        {
            line--;
        }
    }
    if ( (fp2 = fopen("tmp.txt", "w")) != NULL)
    {
        while ((ch = fgetc(fp)) != EOF)
        {
            fputc(ch, fp2);
        }
        fclose(fp2);
        fclose(fp);
        CopyFile((char*)"tmp.txt", file);
        remove((char*)"tmp.txt");
    }
    else
    {
        fclose(fp);
    }
#endif
}


/**************************************************************
Function:       RecordTscStartTime
Description:    ��¼ϵͳ��ʼ����ʱ�䣬��д����־
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void RecordTscStartTime()
{
#ifdef LINUX
    ACE_OS::system("echo At $(date) tsc restart !>> TscRun.log");
#endif
    //unsigned long mRestart = 0 ;
    //(CDbInstance::m_cGbtTscDb).GetSystemData("ucDownloadFlag",mRestart);
    CManaKernel::CreateInstance()->SndMsgLog(LOG_TYPE_REBOOT,0,0,0,0);
}

/**************************************************************
Function:       TestMem
Description:    ����ϵͳ�ڴ��ȶ���
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void TestMem(char* funName,int iLine)
{
#ifdef LINUX
    static int iMemCnt = 0;
    int iCurMemCnt     = 0;
    //FILE* fp = NULL;
    system("ps | grep Gb.aiton > mem.txt");

    int sd;
    char buf[100] = {0};
    char cTmp[36] = {0};
    int n;

    sd = open("mem.txt", O_RDONLY);
    n = read(sd, buf, sizeof(buf));
    close(sd);

    ACE_OS::memcpy(cTmp,buf+15,5);

    iCurMemCnt = atoi(cTmp);


    if ( iCurMemCnt != iMemCnt )
    {
        iMemCnt = iCurMemCnt;

        FILE* file = NULL;
        char tmp[64] = {0};
        struct tm *now;
        time_t ti;

        file = fopen("moreMem.txt","a+");
        if ( NULL == file )
        {
            return;
        }

        ti = time(NULL);
        now = localtime(&ti);
        sprintf(tmp,"%d-%d-%d %d %d:%02d:%02d %s:%d memcnt:%d.\n", now->tm_year + 1900,
                now->tm_mon + 1, now->tm_mday, now->tm_wday, now->tm_hour,
                now->tm_min, now->tm_sec,funName,iLine,iCurMemCnt);
        fputs(tmp,file);
        fclose(file);
    }

    //ACE_DEBUG((LM_DEBUG,"buf:%s cTmp:%s iMemCnt:%d \n",buf,cTmp,iMemCnt));
#endif
}


/**************************************************************
Function:       CompareTime
Description:    ʱ���С�Ƚ�
Input:          ucHour1 ���Ƚ�ʱ��1��Сʱֵ
                ucMin1  ���Ƚ�ʱ��1�ķ���ֵ
                ucHour1 ���Ƚ�ʱ��2��Сʱֵ
                ucMin1  ���Ƚ�ʱ��2�ķ���ֵ
Output:         ��
Return:         0:time2��time1С   1��time2��time1��,�����
***************************************************************/
bool CompareTime(Byte ucHour1,Byte ucMin1,Byte ucHour2,Byte ucMin2)
{
    if ( ucHour2 > ucHour1 )
    {
        return 1;
    }
    else if ( ucHour2 == ucHour1 )
    {
        if ( ucMin2 >= ucMin1 )
        {
            return 1;
        }
    }
    return 0;
}


/**************************************************************
Function:       GetEypDevID
Description:    ���ַ��������ܴ���
Input:          src ���������ַ���ָ��
                dec  �����ܺ��ַ���ָ��
Output:         ��
Return:         -1 -�����ַ���ʧ�� 0 - �����ִ����ɹ�
***************************************************************/
int GetEypChar(char *src,char *dec)
{
    if(src == NULL || dec ==NULL)
        return -1 ;
    char *p=src;
    int i=0;
    while(*p!='\0')
    {
        dec[i]=0x01 ^ *p; /* ��������*/
        p++;
        i++;
    }
    dec[i]='\0';
    return 0 ;
}


/**************************************************************

Function:       GetStrHwAddr
Description:    ��ȡ���������ַ�ַ���
Input:          StrHwAddr �����������ַ�ַ���
Output:         ��
Return:         -1 ��ȡʧ��  0 -��ȡ�ɹ�
***************************************************************/
int GetStrHwAddr(char * strHwAddr)
{
    //char strHwAddr[32];
    int fdReq = -1;
    struct ifreq typIfReq;
    char *hw;
    memset(&typIfReq, 0x00, sizeof(typIfReq));
    fdReq = socket(AF_INET, SOCK_STREAM, 0);
    strcpy(typIfReq.ifr_name, "eth0");
    if(ioctl(fdReq, SIOCGIFHWADDR, &typIfReq) < 0)
    {
        printf("fail to get hwaddr %s %d\n", typIfReq.ifr_name, fdReq);
        close(fdReq);
        return -1;
    }
    hw = typIfReq.ifr_hwaddr.sa_data;
    sprintf(strHwAddr, "%02x%02x%02x%02x%02x%02x", *hw, *(hw+1), *(hw+2), *(hw+3), *(hw+4), *(hw+5));
    close(fdReq);

    return 0;
}

/**************************************************************
Function:       SaveEnyDevId
Description:    ���������豸ID���ܴ洢
Input:          SysEnyDevId ��ϵͳ����IDָ��
Output:         ��
Return:         -1 - ���������豸IDʧ�� 0 -���������豸ID�ɹ�
***************************************************************/
int GetSysEnyDevId(char *SysEnyDevId)
{
    char strHwAddr[32]= {0};
    char enyDevId[32] = {0};
    if(GetStrHwAddr(strHwAddr) == -1)
        return -1;
    if(GetEypChar(strHwAddr,enyDevId)==-1)
        return -1;
    ACE_OS::strcpy(SysEnyDevId,enyDevId);

    return 0;
}
/**************************************************************
Function:       GetMainBroadCdKey
Description:    ���������豸ID���ܴ洢
Input:          CdKey ��ϵͳ����IDָ��
Output:         ��
Return:         -1 - ���������豸IDʧ�� 0 -���������豸ID�ɹ�
***************************************************************/
int GetMainBroadCdKey(char *CdKey)
{

    char CCdkey[8] = {0};
    if(!Cdkey::GetCdkey(CCdkey))
    {
        return -1;
    }
    ACE_OS::strcpy(CdKey,CCdkey);
    ACE_DEBUG((LM_DEBUG,"%s:%d***GetMainBroadCdKey*** CdKey : %X !\n",__FILE__,__LINE__,&CdKey));
    return 0;
}
/**************************************************************
Function:      VaildSN
Description:    �Ժ��İ�����к���ϵͳ�����кŽ��жԱ�
Input:          ��
Output:         ��
Return:         1 - �Ϸ� 0 -�Ƿ�
***************************************************************/
bool VaildSN()
{
    char fileSN[8] = {0};
    char deviceSN[8] = {0};
    int i,bol;
    ReadTscSN(fileSN);
    GetMainBroadCdKey(deviceSN);

    bol = ACE_OS::strcmp(fileSN,deviceSN);
    if(bol == 0)
    {
        ACE_DEBUG((LM_DEBUG,"%s:%d***VaildSN*** SN VAILD !\n",__FILE__,__LINE__));
        return true;
    }
    else
    {
        ACE_DEBUG((LM_DEBUG,"%s:%d***VaildSN*** SN INVALID !\n",__FILE__,__LINE__));
        return false;
    }
}
/**************************************************************
Function:      ReadTscSN
Description:    ��ȡϵͳ�����к�
Input:          ��
Output:         ��8���ֽڵ����кű��浽cdkey ָ�� ��
Return:         ��
***************************************************************/
void ReadTscSN(char *cdkey)
{

    char tmp[8];
    FILE *infile;
    int rc;
    infile = fopen("sn.dat", "rb");
    if(infile == NULL)
    {
        ACE_DEBUG((LM_DEBUG,"%s:%d read File error !\n",__FILE__,__LINE__));
        return ;
    }
    rc = fread(tmp,sizeof(char), 8,infile);
    if(rc ==0)
    {
        ACE_DEBUG((LM_DEBUG,"%s:%d read File error !\n",__FILE__,__LINE__));
        return ;
    }
    ACE_OS::strcpy(cdkey,tmp);
    fclose(infile);
}

/**************************************************************
Function:       RecordTscStartTime
Description:    ��¼ϵͳ��ʼ����ʱ�䣬��д����־
Input:          ��
Output:         ��
Return:         ��
***************************************************************/
void RecordTscSN()
{
#ifdef LINUX

    char tmp[8] = {0};
    int i;
    FILE *outfile, *infile;
    infile = fopen("sn.dat","rb");
    if(infile != NULL)
    {
        ACE_DEBUG((LM_DEBUG,"\n%s:%d***RecordTscSN*** SN file is exsit !\n",__FILE__,__LINE__));
        fclose(infile);
        return ;
    }
    outfile = ACE_OS::fopen("sn.dat", "wb" );
    GetMainBroadCdKey(tmp);
    if( outfile == NULL)
    {
        ACE_DEBUG((LM_DEBUG,"%s:%d  open file error !\n",__FILE__,__LINE__));
        return;
    }
    //for(i=0; i<8; i++)
    //{
    //fwrite( tmp, sizeof(char), 8, outfile );
    //ACE_DEBUG((LM_DEBUG,"%s:%d  0x%x !\n",__FILE__,__LINE__,tmp[i]));
    //}
    ACE_OS::fwrite( tmp, sizeof(char), 8, outfile );
    ACE_OS::fclose(outfile);
#endif
}


/**************************************************************
Function:       TscBeep
Description:    �źŻ�ϵͳ����
Input:          ��
Output:         ��
Return:         ��
Date:           20151208
***************************************************************/

void TscBeep()
{
#ifdef LINUX
    ACE_OS::system("echo 113 >/sys/class/gpio/export");
    ACE_OS::system("echo out >/sys/class/gpio/gpio113/direction");
    ACE_OS::system("echo 0 > /sys/class/gpio/gpio113/value");
    ACE_OS::sleep(ACE_Time_Value(0, 100000)); //��ͣ100����
    ACE_OS::system("echo 1 > /sys/class/gpio/gpio113/value");
    ACE_OS::system("echo in >/sys/class/gpio/gpio113/direction");
    ACE_OS::system("echo 113 >/sys/class/gpio/unexport");
#endif

}

/**************************************************************
Function:       crc_16
Description:    CRC16У��
Input:          *data У������ֵָ��
                short len У�����ݳ���
Output:         ��
Return:         CRC16У��ֵ    ���ֽ���ǰ�����ֽ��ں�
Date:           2019518
***************************************************************/

unsigned short crc_16(unsigned char *data, unsigned short len)
{
    
    unsigned short CRC_POLY = 0x1005 ;//x16+x12+x2+1
    unsigned short crc16 = 0x0000;
    while (len--)
    {
        for (unsigned char i = 0x80; i != 0; i >>= 1)
        {
            if ((crc16 & 0x8000) != 0)
            {
                crc16 = crc16 << 1;
                crc16 = crc16 ^ CRC_POLY;
            }
            else
            {
                crc16 = crc16 << 1;
            }
            if ((*data & i) != 0)
            {
                crc16 = crc16 ^ CRC_POLY;
            }
        }
        data++;
    }
    return crc16;
}


