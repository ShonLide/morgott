
#include "config.h"

extern void  delay(unsigned long uldata);

uint8 idata ucRegVal;
extern uint8 idata nAsrStatus;

void ProcessInt0(void);


void LD_Reset()
{
  RSTB=1;
  delay(5);
  RSTB=0;
  delay(5);
  RSTB=1;

  delay(5);
  CSB=0;
  delay(5);
  CSB=1;
  delay(5);
}

void LD_Init_Common()
{
  LD_ReadReg(0x06);
  LD_WriteReg(0x17, 0x35);
  delay(10);
  LD_ReadReg(0x06);

  LD_WriteReg(0x89, 0x03);
  delay(5);
  LD_WriteReg(0xCF, 0x43);
  delay(5);
  LD_WriteReg(0xCB, 0x02);

  /*PLL setting*/
  LD_WriteReg(0x11, LD_PLL_11);

  LD_WriteReg(0x1E,0x00);
  LD_WriteReg(0x19, LD_PLL_ASR_19);
  LD_WriteReg(0x1B, LD_PLL_ASR_1B);
  LD_WriteReg(0x1D, LD_PLL_ASR_1D);
  delay(10);

  LD_WriteReg(0xCD, 0x04);
//	LD_WriteReg(0x17, 0x4c);
  delay(5);
  LD_WriteReg(0xB9, 0x00);
  LD_WriteReg(0xCF, 0x4F);
  LD_WriteReg(0x6F, 0xFF);
}

void LD_Init_ASR()
{
  LD_Init_Common();
  LD_WriteReg(0xBD, 0x00);
  LD_WriteReg(0x17, 0x48);
  delay( 10 );
  LD_WriteReg(0x3C, 0x80);
  LD_WriteReg(0x3E, 0x07);
  LD_WriteReg(0x38, 0xff);
  LD_WriteReg(0x3A, 0x07);
  LD_WriteReg(0x40, 0);
  LD_WriteReg(0x42, 8);
  LD_WriteReg(0x44, 0);
  LD_WriteReg(0x46, 8);
  delay( 1 );
}


void ProcessInt0(void)
{
  uint8 nAsrResCount=0;

  EX0=0;
  ucRegVal = LD_ReadReg(0x2B);
  LD_WriteReg(0x29,0) ;
  LD_WriteReg(0x02,0) ;
  if((ucRegVal & 0x10) &&
      LD_ReadReg(0xb2)==0x21 &&
      LD_ReadReg(0xbf)==0x35)			/*识别成功*/
  {
    nAsrResCount = LD_ReadReg(0xba);
    if(nAsrResCount>0 && nAsrResCount<=4)
    {
      nAsrStatus=LD_ASR_FOUNDOK;
    }
    else
    {
      nAsrStatus=LD_ASR_FOUNDZERO;
    }
  }															 /*没有识别结果*/
  else
  {
    nAsrStatus=LD_ASR_FOUNDZERO;
  }

  LD_WriteReg(0x2b, 0);
  LD_WriteReg(0x1C,0);/*写0:ADC不可用*/

  LD_WriteReg(0x29,0) ;
  LD_WriteReg(0x02,0) ;
  LD_WriteReg(0x2B,  0);
  LD_WriteReg(0xBA, 0);
  LD_WriteReg(0xBC,0);
  LD_WriteReg(0x08,1);	 /*清除FIFO_DATA*/
  LD_WriteReg(0x08,0);	/*清除FIFO_DATA后 再次写0*/


  EX0=1;
}

uint8 RunASR(void)
{
  uint8 i=0;
  uint8 asrflag=0;
  for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
  {
    LD_AsrStart();
    delay(50);
    if (LD_AsrAddFixed()==0)
    {
      LD_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
      delay(50);			//	并从初始化开始重新ASR识别流程
      continue;
    }
    delay(10);
    if (LD_AsrRun() == 0)
    {
      LD_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
      delay(50);			//	并从初始化开始重新ASR识别流程
      continue;
    }
    asrflag=1;
    break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
  }

  return asrflag;
}

uint8 LD_Check_ASRBusyFlag_b2()
{
  uint8 j;
  uint8 flag = 0;
  for (j=0; j<10; j++)
  {
    if (LD_ReadReg(0xb2) == 0x21)
    {
      flag = 1;
      break;
    }
    delay(10);
  }
  return flag;
}

void LD_AsrStart()
{
  LD_Init_ASR();
}

uint8 LD_AsrRun()
{
  EX0=0;
  LD_WriteReg(0x35, MIC_VOL);
  LD_WriteReg(0x1C, 0x09);
  LD_WriteReg(0xBD, 0x20);
  LD_WriteReg(0x08, 0x01);
  delay( 1 );
  LD_WriteReg(0x08, 0x00);
  delay( 1 );

  if(LD_Check_ASRBusyFlag_b2() == 0)
  {
    return 0;
  }
//	LD_WriteReg(0xB6, 0xa); 
//	LD_WriteReg(0xB5, 0x1E); 
//	LD_WriteReg(0xB8, 10); 

//	LD_WriteReg(0x1C, 0x07); 
  LD_WriteReg(0x1C, 0x0b); 
  LD_WriteReg(0xB2, 0xff);
  delay( 1);
  LD_WriteReg(0x37, 0x06);
  delay( 1 );
  LD_WriteReg(0x37, 0x06);
  delay( 5 );
  LD_WriteReg(0x29, 0x10);

  LD_WriteReg(0xBD, 0x00);
  EX0=1;
  return 1;
}

uint8 LD_AsrAddFixed()
{
  uint8 k, flag;
  uint8 nAsrAddLength;
#define DATE_A 50   
#define DATE_B 70		
  uint8 code sRecog[DATE_A][DATE_B] =
  {
    "mo ge te",\ 
    "gu de dei ou men king ",\ 
		"you er fei li er",\ 
		"fao ta ni shi de",\
		"ken gu rai ci mai lao d",\
		"mei k a si tei de world" 
		
  };	
  uint8 code pCode[DATE_A] =
  {
    CODE_CMD, \
		CODE_1, \
		CODE_2, \
		CODE_3, \
		CODE_4, \
		CODE_5, \
  };	
  flag = 1;
  for (k=0; k<DATE_A; k++)
  {

    if(LD_Check_ASRBusyFlag_b2() == 0)
    {
      flag = 0;
      break;
    }

    LD_WriteReg(0xc1, pCode[k] );
    LD_WriteReg(0xc3, 0 );
    LD_WriteReg(0x08, 0x04);
    delay(1);
    LD_WriteReg(0x08, 0x00);
    delay(1);

    for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
    {
      if (sRecog[k][nAsrAddLength] == 0)
        break;
      LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
    }
    LD_WriteReg(0xb9, nAsrAddLength);
    LD_WriteReg(0xb2, 0xff);
    LD_WriteReg(0x37, 0x04);
  }
  return flag;
}

uint8 LD_GetResult()
{
  return LD_ReadReg(0xc5 );
}



