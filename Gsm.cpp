/***************************************************************
Copyright(c) 2014  AITON. All rights reserved.
Author:     AITON
FileName:   Gsm.cpp
Date:       2014-5-1
Description:�źŻ�GSM/GPRS ������
Version:    V1.0
History:
***************************************************************/
#include "Gsm.h"
#include "ManaKernel.h"
#include "Gps.h"
#include "Rs485.h"
#include <termios.h>
//#include <asm/termbits.h>
char CGsm::charMsg[500]={0};
char tmpstr[500]={0};



/**************************************************************
Function:        CGsm::CGsm
Description:     CGsm�๹�캯������ʼ����			
Input:          ��           
Output:         ��
Return:         ��
***************************************************************/
CGsm::CGsm()
{
	
	ACE_DEBUG((LM_DEBUG,"%s:%d Init Gsm Object!\n",__FILE__,__LINE__));
	bgsm = true ;
	//pGsmSerial = CSerialCtrl::CreateInstance() ;	
	//m_iGsmFd = pGsmSerial->GetSerialFd(1);
	bInit = false ;
	iInitCount = 0 ;
	ACE_OS::memset(&Msg,0x0,sizeof(SMS_T));
	
	ACE_OS::memset(incomecall,0x0,sizeof(incomecall));
}

/**************************************************************
Function:       CGsm::~CGsm
Description:    CGsm����������		
Input:          ��              
Output:         ��
Return:         ��
***************************************************************/
CGsm::~CGsm()
{
	ACE_DEBUG((LM_DEBUG,"Destruct CGsm object ! \n"));
}

/**************************************************************
Function:       CGsm::CGsm
Description:    ����CGsm�ྲ̬����		
Input:          ��              
Output:         ��
Return:         CGsm��̬����ָ��
***************************************************************/
CGsm* CGsm::CreateInstance()
{
	static CGsm cGsm;
	return &cGsm;
}

/**************************************************************
Function:       CGsm::RunGpsData
Description:    GSM������	
Input:          iTime  �Ƚ�ʱ��ָ��        
Output:         ��
Return:         0
***************************************************************/
void CGsm::RunGsmData()
{		
	if(m_iGsmFd<0)
	{		
		ACE_DEBUG((LM_DEBUG,"\n%s:%d Get GSM fd error!\n",__FILE__,__LINE__));
		return ;
	}
	
	if(!bInit)
	{
		if(iInitCount>5)
			return ;
		if(!GsmInit())
		{
			ACE_DEBUG((LM_DEBUG,"\n%s:%d Init Gsm error!\n",__FILE__,__LINE__));
			iInitCount++ ;
			
			return ;
		}
		else
			bInit = true ;
	}
	  
	  	if(GetString(m_iGsmFd ,charMsg))
		{				
			if(ACE_OS::strstr(charMsg,"+CMT"))
			{				
				//ACE_DEBUG((LM_DEBUG,"\n%s:%d Begin unpack sms message!\n",__FILE__,__LINE__));
				ACE_OS::memset(&Msg,0x0,sizeof(SMS_T));
				DoCmt(charMsg);
			}
			else if(ACE_OS::strstr(charMsg,"+CLIP"))
			{
				char tmpTelNum[20]={0};
				char incallmsg[50]={0};	
				char *callnum = charMsg+10;
				
				ACE_OS::strncpy(tmpTelNum,callnum,11);
				if(!ACE_OS::strncmp(incomecall,tmpTelNum,11))			
					return ;
				//ACE_OS::printf("%s:%d comecall number: %s\n",__FILE__,__LINE__,tmpTelNum);	
				ACE_OS::strcpy(incomecall,tmpTelNum);
				ACE_OS::sprintf(incallmsg,"echo �������:%s ����ʱ��: $(date) >>Msg.txt",tmpTelNum);	
				ACE_OS::system(incallmsg);
				//ACE_OS::printf("\n%s:%d %s",__FILE__,__LINE__,incallmsg);	
				//SendSms(incomecall,"��������ʱ����������;,����ϵ13375926324! -������Ϣ���ڸú�����Զ��ظ�");
				//ACE_OS::printf("\n%s:%d replay for %s \n",__FILE__,__LINE__,incomecall);	
			}
		}
	
  }

	
/**************************************************************
Function:       CGsm::GsmInit
Description:    GSM�豸������ʼ��
Input:          iTime  �Ƚ�ʱ��ָ��        

Output:         ��
Return:         0
***************************************************************/
	bool CGsm::GsmInit()
	{		
		//CRs485::CreateInstance()->CtrolGPPIo(0,10) ; //Open Gps io gpp10
		//pGsmSerial->set_speed(m_iGsmFd, 9600);
		char initsgsm[6][50]={"AT\r\n","ATE0\r\n","AT+CLIP=1\r\n","AT+CNMI=2,2,0,0,1\r\n","AT+CMGF=0\r\n","AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n"};
		static bool initok[6]={false};
		for(Byte i =0 ;i<6;i++)
		{
			if(initok[i])
				continue ;
			//pGsmSerial->serialWrite(initsgsm[i],m_iGsmFd);		
			if(GetString(m_iGsmFd ,charMsg))
			{
				ACE_DEBUG((LM_DEBUG,"%s:%d Init_Func =%s!\n",__FILE__,__LINE__,initsgsm[i]));
				if(!ACE_OS::strstr(charMsg,"OK"))				{
					
					ACE_DEBUG((LM_DEBUG,"%s:%d Init %s fail!\n",__FILE__,__LINE__,initsgsm[i]));
					return false ;
				}
				initok[i] = true ;
			}	
		}
		return true ;

	}

	
/**************************************************************
Function:       CGsm::GetString
Description:    GSM����GSM������Ϣ
Input:          fd  GSM�����ļ����       
			
Output:         gsmstring  ���������Ϣ
Return:         0
***************************************************************/
bool CGsm::GetString(int fd ,char *gsmstring)
{	
	int iGetNUM = 0 ;
	Byte ret = 0 ;
	
	ACE_OS::memset(gsmstring,0x0,strlen(gsmstring));
	tcflush(m_iGsmFd, TCIFLUSH);
	while(true)		
	{ 	  	
   		fd_set rfds;	
    	FD_ZERO(&rfds);      
   	    FD_SET(fd,&rfds);
		struct timeval tv;
		tv.tv_sec =10;
		tv.tv_usec = 0;
		ret = select(fd + 1,&rfds,NULL,NULL,&tv);
		if(ret == -1)
   		 {
       		ACE_DEBUG((LM_DEBUG,"%s:%dGet the GsmSerial1 error!\n",__FILE__,__LINE__));        	
       		return false ;
   		 }
		 else if(ret == 0)
			return false ;
		if(FD_ISSET(fd,&rfds))
   		{
      	 	int iNum = read(m_iGsmFd, tmpstr, 500);			
	  		 if(iNum == -1)
	     	{
				ACE_DEBUG((LM_DEBUG,"%s:%d Get SGM msg error!\n",__FILE__,__LINE__));
				return false ;
	     	}
			else
			{
				ACE_OS::strcat( gsmstring, tmpstr);
				iGetNUM = iGetNUM + iNum;
				if(charMsg[strlen(charMsg)-1] == '\n' &&  charMsg[strlen(charMsg)-2] == '\r')
				{
					ACE_OS::printf("\n %s:%d: GSM msg:%d content:%s\n",__FILE__,__LINE__,iGetNUM,charMsg);		
					
					iGetNUM = 0 ;
				}
				else
				{
					ACE_OS::memset(tmpstr,0x0,500);
					continue ;
				}
			}				
	   		return true ;

		}
}
		return false ;
}


/*******************************************************************
�������ƣ�DoCmt
�������������ն��Ŵ�����
����˵���� smsmsg -PDU�����ַ���
����ֵ��  ��
*******************************************************************/
void CGsm::DoCmt(char* smsmsg)
{
	char tmpstr[500]={0};
	Uint codetype = 0 ;
	Uint length = 0 ;
	smsmsg +=33 ;
	ACE_OS::memcpy(tmpstr ,smsmsg ,2);
	sscanf(tmpstr, "%x", &length);
	smsmsg +=4 ;
	ACE_OS::memcpy(tmpstr ,smsmsg ,length+1);
	
	PhoneNumFormat(Msg.acPhone , tmpstr , Recv_Flag);
	//ACE_OS::printf("\n %s:%d:Msg PhoneNum: %s\n",__FILE__,__LINE__,Msg.acPhone);
	smsmsg +=length+1 ;
	ACE_OS::memset(tmpstr ,0x0 ,500);
	ACE_OS::memcpy(tmpstr ,smsmsg ,4);
	if(ACE_OS::strcmp(tmpstr,"0000") == 0)
		codetype = bit7 ;
	else if(ACE_OS::strcmp(tmpstr,"0008") == 0)
		codetype = ucs2 ;
	smsmsg +=4 ;
	ACE_OS::memcpy(tmpstr ,smsmsg ,12);
	GetSmsRecvTime(Msg.acTime,tmpstr);
	//ACE_OS::printf("\n %s:%d:Msg Time: %s\n",__FILE__,__LINE__,Msg.acTime);
	smsmsg +=14 ;
	ACE_OS::memset(tmpstr,0x0,500);
	ACE_OS::memcpy(tmpstr ,smsmsg , 2);
	sscanf(tmpstr, "%x", &length);
	
	ACE_OS::memcpy(tmpstr , smsmsg+2, length*2);
		
	UCStoUTF(tmpstr , Msg.Message,codetype);
	//ACE_OS::printf("\n %s:%d:Msg Content: %s\n",__FILE__,__LINE__,Msg.Message);
	ACE_OS::memset(tmpstr,0x0,500);
	ACE_OS::sprintf(tmpstr,"echo %s  %s  %s >>Msg.txt",Msg.acPhone,Msg.acTime,Msg.Message);
	ACE_OS::system(tmpstr);
	ACE_OS::sleep(1);
	//pGsmSerial->serialWrite((char *)"AT+CMGD=1, 4\r\n",m_iGsmFd);
}


/*******************************************************************
�������ƣ�PhoneNumFormat
���������� �ֻ��Ÿ�ʽ����
����˵���� char*buff, const char *tel_num, int flag
����ֵ��  ��
*******************************************************************/
void CGsm::PhoneNumFormat(char*buff, const char *tel_num, int flag)
{
	ACE_OS::memset(buff ,0 ,sizeof(buff));
	if(ACE_OS::strlen(tel_num)==11)
	{
	if(ACE_OS::strncmp(tel_num ,"86" , 2) !=0 && flag == Snd_Flag)
		ACE_OS::sprintf(buff, "86%s", tel_num);
	if(ACE_OS::strncmp(tel_num ,"86" , 2) ==0 && flag == Snd_Flag)
		ACE_OS::strcpy(buff , tel_num);
	if(ACE_OS::strncmp(tel_num ,"68" , 2) ==0 && flag == Recv_Flag)
		ACE_OS::strcpy(buff , tel_num+2);
	if(ACE_OS::strncmp(tel_num ,"68" , 2) !=0 && flag == Recv_Flag)
		ACE_OS::strcpy(buff , tel_num);
	}
	else
	ACE_OS::strcpy(buff , tel_num);
	
	if(ACE_OS::strlen(buff)%2 == 1 && flag == Snd_Flag)
		ACE_OS::strcat( buff, "F");
	for(Byte i=0; i<ACE_OS::strlen(buff); i++)
	{
		char temp;
		temp = buff[i];
		buff[i] = buff[i+1];
		buff[i+1] = temp;
		i++;
	}
	if(ACE_OS::strlen(buff)%2 == 0 && flag == Recv_Flag)
	buff[ACE_OS::strlen(buff)-1]=0;	
}


/*******************************************************************
�������ƣ� GetSmsRecvTime
���������� ת��PDU���е�ʱ���ʽ
����˵���� smstime - ת����ʽ��ʱ���ַ���		
		   codetype - ת������ 
����ֵ��  ��
*******************************************************************/
void CGsm::GetSmsRecvTime(char *smstime ,char *recvstr)
{
	
	ACE_OS::strcat(smstime,"20");
	smstime +=2;
	for(Byte i=0;i<6;i++)
	{
		smstime[0] = recvstr[2*i+1];
		smstime[1] = recvstr[2*i];
		if(i<2)		ACE_OS::strcat(smstime , "-");
		if(i==2)	ACE_OS::strcat(smstime , " ");
		if(i>2 && i<5)	ACE_OS::strcat(smstime , ":");		
		smstime+=3 ;
	}
	
}

/*******************************************************************

�������ƣ�UCStoUTF
���������� USC����ת��ΪUTF-8����
����˵���� cunicode - ��ת����
		   utf -ת����UTF8����
		   codetype - ת������ 

����ֵ��  int -ת���ֽ���
*******************************************************************/
int CGsm::UCStoUTF(char *unicode , char *utf, Uint codetype)
{
	//ACE_OS::printf("++++++++++%s++++++++",unicode);
	iconv_t iconv_handle = iconv_open("UTF-8", "UCS-2BE");
	char unicode_buffer[400]={0};
	char utf_buffer[400]={0};
	char buf[4]={0};	
	char *utf_p = utf_buffer;
	char *unicode_p = unicode_buffer;
	int in_len  = strlen(unicode)/2;
	int out_len = 400;
	int iUnicode=0;
	int result = 0 ;
	for(int i = 0; i < in_len; i++)
	{
		buf[0] = unicode[2*i];
		buf[1] = unicode[2*i+1];
		sscanf(buf, "%x", &iUnicode);
		unicode_buffer[i] = (char)iUnicode;
	}
	if(codetype == ucs2 )
	{
		//ACE_OS::printf("\n��ʼ���Ķ��Ž���\n");
		result = iconv(iconv_handle,&unicode_p,(size_t *)&in_len,&utf_p,(size_t *)&out_len);
		
	}
	if(codetype == bit7)
	{
		 //Decode7bitPDU(unicode_buffer,utf_buffer);
		 result =DeCode7Bit(unicode_buffer, utf_buffer, in_len-1) ;
		 
	}
	ACE_OS::strncpy(utf , utf_buffer,ACE_OS::strlen(utf_buffer));
	//ACE_OS::printf("����������Ϣ: %s \n",utf_buffer);
	//ACE_OS::printf("%s:%d GetMsg=%d : %s \n",__FILE__,__LINE__,strlen(utf),utf);
	iconv_close(iconv_handle);
	return 1;
}

/*******************************************************************
�������ƣ�UTFtoUCS
���������� UTF8����ת��ΪUSC
����˵���� const char *utf , char *unicode
����ֵ��  ��
*******************************************************************/
int CGsm::UTFtoUCS(const char *utf , char *unicode)
{
	iconv_t iconv_handle;
	char unicode_buffer[300]={0};
	char utf8_buffer[300]={0};
	ACE_OS::strcpy(utf8_buffer ,utf);
	char *utf_p = utf8_buffer;
	char *unicode_p = unicode_buffer;
	int in_len = 0;
	int out_len = 300;
	in_len = ACE_OS::strlen(utf8_buffer);
	//ACE_OS::printf("%s:%d  UTF_Length =%d  %s \n",__FILE__,__LINE__,in_len,utf_p);
	iconv_handle = iconv_open("UCS-2BE","UTF-8");

	int result = iconv(iconv_handle,&utf_p,(size_t *)&in_len,&unicode_p,(size_t *)&out_len);
	for(int index = 0;index <(300-out_len);index++)
	{
		char tmp[4]={0};
		ACE_OS::sprintf(tmp ,"%02X",(unsigned char)unicode_buffer[index] );
		ACE_OS::strcat(unicode , tmp);
	}
	iconv_close(iconv_handle);

	//ACE_OS::printf("%s:%d  out_Length =%d  unicode_len=%d result = %d %s \n",__FILE__,__LINE__,out_len,strlen(unicode_buffer),result,unicode);
	return 1;
}

/*******************************************************************
�������ƣ�SendSms
���������� ���Ͷ��Ź���
����˵���� sendTelNum --�����ն��ź���
		  sendMsgText --���Ͷ�������
����ֵ��  ��
*******************************************************************/
void CGsm::SendSms( char* sendTelNum ,  char *sendMsgText)
{
	ACE_OS::printf("\n%s:%d Begin to send message!\n",__FILE__,__LINE__);
	if(!bInit)
		return ;	
	char smdbuffer[400]={0};
	int encodedPduNum=0 ;	
	//char sendnum[13]={"13375926324"};
	char atSmsCmd[20] ={0};
	//char smscontent[300] ={0};	
	//ACE_OS::strcpy(smscontent,"��Թ�����?");		
	EncodeSMS(sendTelNum,sendMsgText,encodedPduNum,smdbuffer);	
	//ACE_OS::printf("%s:%d Send�� %s \n",__FILE__,__LINE__,smscontent);
	ACE_OS::sprintf(atSmsCmd,"AT+CMGS=%d\r",encodedPduNum);	
	ACE_OS::printf("%s:%d  %s \n",__FILE__,__LINE__,atSmsCmd);
	//pGsmSerial->serialWrite(atSmsCmd,m_iGsmFd);	
	ACE_OS::sleep(1);
	//pGsmSerial->serialWrite(smdbuffer,m_iGsmFd);	
	//ACE_OS::printf("%s:%d Send sms-serial bytes = %d \n",__FILE__,__LINE__,iNum);

   }



/*******************************************************************
�������ƣ�GetBit
���������� �ж��Ƿ��Ǽ�λ����
����˵����bitnum --����λ , 
		 data   --���ж�����
����ֵ��  int
*******************************************************************/
bool CGsm::GetBit(Byte bitnum,Byte data)   
{   
    bool IsTrue = false;   
    switch (bitnum)   
    {   
        case 0:   
            IsTrue = data & 0x01;   
            break;   
        case 1:   
            IsTrue = data & 0x02;   
            break;   
        case 2:   
            IsTrue = data & 0x04;   
            break;   
        case 3:   
            IsTrue = data & 0x08;   
            break;   
        case 4:   
            IsTrue = data & 0x10;   
            break;   
        case 5:   
            IsTrue = data & 0x20;   
            break;   
        case 6:   
            IsTrue = data & 0x40;   
            break;   
        case 7:   
            IsTrue = data & 0x80;   
            break;   
        default:   
            IsTrue = data & 0x01;   
            break;   
    }   
    return IsTrue;   
}   

/*******************************************************************
�������ƣ�SetBit
���������� ���ֽڴ���λ����
����˵����bitnum --����λ , 
		 data   --�������ֽ�����
����ֵ��  Byte   --������ֽ�����
*******************************************************************/
Byte CGsm::SetBit(Byte bitnum,Byte data,bool bit)   
{   
    Byte op;   
    Byte t = data;   
    switch (bitnum)   
    {   
        case 0:   
            op = 0x01;   
            break;   
        case 1:   
            op = 0x02;   
            break;   
        case 2:   
            op = 0x04;   
            break;   
        case 3:   
            op = 0x08;   
            break;   
        case 4:   
            op = 0x10;   
            break;   
        case 5:   
            op = 0x20;   
            break;   
        case 6:   
            op = 0x40;   
            break;   
        case 7:   
            op = 0x80;   
            break;   
        default:   
            op = 0x01;   
            break;   
    }   
    if (bit) t = t | op;   
		return t;   
}

/*******************************************************************
�������ƣ�Encode7bitPDU
���������� ��7bit����PDU����

����˵����src --������Դ�� , 
		 
����ֵ��  int --���볤��
*******************************************************************/
int CGsm::Encode7bitPDU(char *src, char *output)   
{   
    int i,j,k;   
    Byte data[300],coded[300];   
    ACE_OS::memcpy(data,src,strlen(src)+1);   
       
    for (i=0,j=1;i<(int)strlen(src);i++)   
    {   
        if (i%8==0) j--;   
        coded[j] = data[i] >> ((j) % 7);   
        for (k=0;k<=j%7;k++)   
        {   
            coded[j] = SetBit(7-k,coded[j],GetBit((j%7)-k,data[i+1]));   
        }   
        j++;   
    }   
       
    ACE_OS::strcpy(output,"");   
    char str[3];   
    for (i=0;i<j;i++)   
    {   
        sprintf(str,"%.2X",coded[i]);   
        strcat(output,str);   
    }   
    return strlen(src);   
}

   

/*******************************************************************
�������ƣ�IfUnicode
���������� �ж��Ƿ������ı���
����˵����Text --����λ , 
		 
����ֵ��  bool  true-����  false-������
*******************************************************************/
bool CGsm::IfUnicode(const char *Text)   
{   
    int i,len;   
    Byte data[300];   
    len = ACE_OS::strlen(Text);   
    ACE_OS::memcpy(data,Text,len);   
    for (i=0;i<len;i++)   
    {   
        if (GetBit(7,data[i]))
			 return true;   
    }   
    return false;   
}


/*******************************************************************
�������ƣ�uEncodeSMS
���������� ���ŷ��ͱ��봦��
����˵����s_number  -�����Ͷ��ź���
		 s_sms_text -�����Ͷ�������
		 sndPduNum -PDU�����ͳ���
		s_out_pdu  -PDU�����������				
				
����ֵ�� ��
*******************************************************************/
void  CGsm::EncodeSMS(char * s_number,char * s_sms_text,int& sndPduNum,char * s_out_pdu)
{	
	const char *s_smsc_head="00" ;
	char s_phonenum_head[14]={0} ;
	char s_PhoneNum[20]={0};
	char s_encode_type[8] ={0};	
	const char *s_end_flag = "\x1A";
	bool b_unicode  = false ;	
	char EnodedSms[300]={0};
	int  EnodedSmsLen = 0 ;
	b_unicode = IfUnicode(s_sms_text) ;
	
	//ACE_OS::printf("\n%s:%d smslength=%d TelNum =%s TelSms =%s \n",__FILE__,__LINE__,ACE_OS::strlen(s_sms_text),s_number,s_sms_text);
	if(ACE_OS::strlen(s_number) ==11)
	{
		ACE_OS::strcpy(s_phonenum_head,"11000D91");		
	}
	else
	{
		ACE_OS::sprintf(s_phonenum_head,"1100%02xA1",ACE_OS::strlen(s_number)); //10086 short num
	}

	PhoneNumFormat(s_PhoneNum,s_number,Snd_Flag);
	if(b_unicode)
	{
		ACE_OS::strcpy(s_encode_type,"0008A7");
		UTFtoUCS(s_sms_text, EnodedSms);
	}
	else
	{
		ACE_OS::strcpy(s_encode_type,"0000A7");
		Encode7bitPDU(s_sms_text,EnodedSms);
		//EnCode7Bit(s_sms_text,EnodedSms);
	}
	if(b_unicode)
		EnodedSmsLen = ACE_OS::strlen(EnodedSms)/2;
	else
		EnodedSmsLen = ACE_OS::strlen(s_sms_text);
	ACE_OS::sprintf(s_out_pdu,"%s%s%s%s%02X%s%s",s_smsc_head,s_phonenum_head,s_PhoneNum,s_encode_type,EnodedSmsLen,EnodedSms,s_end_flag);
	//ACE_OS::printf("%s:%d %s \n",__FILE__,__LINE__,s_out_pdu);
	sndPduNum = (ACE_OS::strlen(s_out_pdu)-3)/2;	
	return ;	
}

/*******************************************************************
�������ƣ�DeCode7Bit
���������� ����7λ������봦����
����˵����pSrc  -�������ֽ�
		 pDst - ���������ֽ�
		nSrcLength -�����봮����	
����ֵ�� int  ���볤��
*******************************************************************/
int CGsm::DeCode7Bit( char* pSrc, char* pDst, int nSrcLength)
{
    int nSrc; // Դ�ַ����ļ���ֵ
    int nDst; // Ŀ����봮�ļ���ֵ
    int nByte; // ��ǰ���ڴ���������ֽڵ���ţ���Χ��0-6
    unsigned char nLeft; 
	
    nSrc = 0;
    nDst = 0;
    
    nByte = 0;
    nLeft = 0;   
	//ACE_OS::printf("%s:%d 7bits�����볤��=%d \n",__FILE__,__LINE__,nSrcLength);
    while (nSrc < nSrcLength)
    {
            // ��Դ�ֽ��ұ߲��������������ӣ�ȥ�����λ���õ�һ��Ŀ������ֽ�
            *pDst = ((*pSrc << nByte) | nLeft) & 0x7f;  
			//printf("%c",*pDst);         
            nLeft = *pSrc >> (7 - nByte);            
            pDst++;
            nDst++;            
            nByte++;
            // ����һ������һ���ֽ�
            if (nByte == 7)
            {                   
                *pDst = nLeft;
				//printf("%c",*pDst);                    
                 pDst++;
                 nDst++;                 
                 nByte = 0;
                 nLeft = 0;
            }           
            pSrc++;
            nSrc++;
    }	
    *pDst = 0;  
	//printf("\n");
    return nDst;
}


/*******************************************************************
�������ƣ� OpenTcpConnec
���������� ����GPRS TCP����
����˵����IP  -Ҫ���ӵ�IP��ַ
		 port - Ҫ���ӵĶ˿�		
����ֵ�� ��
*******************************************************************/
void CGsm::OpenTcpConnec(char * IP ,char * port)
{
	if(ACE_OS::strlen(IP)>20)
		return ;
	char conncetstr[50]={0};
	ACE_OS::sprintf(conncetstr,"AT^CIPSTART=TCP,%s,%s\r\n",IP,port);
	//pGsmSerial->serialWrite(conncetstr,m_iGsmFd);	

}

/*******************************************************************
�������ƣ� SendTcpData
���������� ����TCP����
����˵���� senddata  -����������
		  sendtype - ������������ ASCII ����16��������		
����ֵ�� ��
*******************************************************************/
void CGsm::SendTcpData(char * senddata,Byte sendtype)
{
	if(ACE_OS::strlen(senddata)>380 || sendtype > 3)
		return ;
	char conncetstr[400]={0};
	if(sendtype = 1)
		ACE_OS::sprintf(conncetstr,"AT^CIPSEND=%s\r\n",senddata);
	else if(sendtype = 2)
		ACE_OS::sprintf(conncetstr,"AT^CIPSENDHEX=%s\r\n",senddata);
	//pGsmSerial->serialWrite(conncetstr,m_iGsmFd);	

}

/*******************************************************************
�������ƣ� OPerateGprs
���������� �򵥵�GPRS�������߲�ѯ����
����˵���� ��
����ֵ�� ��
*******************************************************************/
void CGsm::OPerateGprs(Byte dotype)	
{	
	switch(dotype)
	{
		case 1:
			//pGsmSerial->serialWrite((char*)"AT^CIPCLOSE\r\n",m_iGsmFd);
		break;
		case 2:
			//pGsmSerial->serialWrite((char*)"AT^CIPSTATUS\r\n",m_iGsmFd);
		break;
		case 3:
			//pGsmSerial->serialWrite((char*)"AT^CIFSR\r\n",m_iGsmFd);
		break;
		default:
		break ;

	}
	
}


