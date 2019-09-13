#include "SHT2x.h"
#include "delay.h"
#include "i2c.h"
#include <stdio.h>

SHT2x_data SHT20;
u8 SHT2x_Init(void)
{
	u8 err;
   err = SHT2x_SoftReset();
	return err;
}

void Soft_delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=7950;  //�Լ�����  ����ʱ�䣺20141116
      while(i--) ;    
   }
}


u8 SHT2x_SoftReset(void) //SHT20�����λ
{
	u8 err=0;
	err=Single_WriteI2C_com(SHT20_Add,SOFT_RESET);
	IIC_Stop();   //����ֹͣ�ź�
	return err;
}
float SHT2x_GetTempPoll(void)
{
    float TEMP;
    u16 ST;
    u16 i=0;
		u8 pBuff[2];
		IIC_Start();
		if(!(IIC_Write_Byte(SHT20_Add)))	//����д������Ӧ��λ
		{
			IIC_Write_Byte(TRIG_TEMP_MEASUREMENT_POLL);	//��λ��ʼ�Ĵ�����ַ
			Soft_delay_ms(50);
			IIC_Start();	//��������
			IIC_Write_Byte(SHT20_Add+1);	//���Ͷ�ȡ����
			for(i = 0;i < 2;i++)
			{
				pBuff[i] = IIC_Read_Byte();	//��ȡ����
				if(i == (2 - 1))	IIC_Ack(1);	//���ͷ�Ӧ���ź�
				else IIC_Ack(0);	//����Ӧ���ź�
			}
		}
		IIC_Stop();	
    ST = (pBuff[0] << 8) | (pBuff[1] << 0);
	
    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127) - 46.85;

    return (TEMP);	  
}

float SHT2x_GetHumiPoll(void)
{
    float HUMI;
    u16 SRH;
		u16 i=0; 
		u8 pBuff[2];
		IIC_Start();
		if(!(IIC_Write_Byte(SHT20_Add)))	//����д������Ӧ��λ
		{
			IIC_Write_Byte(TRIG_HUMI_MEASUREMENT_POLL);	//��λ��ʼ�Ĵ�����ַ
			delay_ms(50);
			IIC_Start();	//��������
			IIC_Write_Byte(SHT20_Add+1);	//���Ͷ�ȡ����
			for(i = 0;i < 2;i++)
			{
				pBuff[i] = IIC_Read_Byte();	//��ȡ����
				if(i == (2 - 1))	IIC_Ack(1);	//���ͷ�Ӧ���ź�
				else IIC_Ack(0);	//����Ӧ���ź�
			}
		}
		IIC_Stop();	
    SRH = (pBuff[0] << 8) | (pBuff[1] << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}



