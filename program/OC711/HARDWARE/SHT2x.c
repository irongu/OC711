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
      i=7950;  //自己定义  调试时间：20141116
      while(i--) ;    
   }
}


u8 SHT2x_SoftReset(void) //SHT20软件复位
{
	u8 err=0;
	err=Single_WriteI2C_com(SHT20_Add,SOFT_RESET);
	IIC_Stop();   //发送停止信号
	return err;
}
float SHT2x_GetTempPoll(void)
{
    float TEMP;
    u16 ST;
    u16 i=0;
		u8 pBuff[2];
		IIC_Start();
		if(!(IIC_Write_Byte(SHT20_Add)))	//发送写命令并检查应答位
		{
			IIC_Write_Byte(TRIG_TEMP_MEASUREMENT_POLL);	//定位起始寄存器地址
			Soft_delay_ms(50);
			IIC_Start();	//重启总线
			IIC_Write_Byte(SHT20_Add+1);	//发送读取命令
			for(i = 0;i < 2;i++)
			{
				pBuff[i] = IIC_Read_Byte();	//读取数据
				if(i == (2 - 1))	IIC_Ack(1);	//发送非应答信号
				else IIC_Ack(0);	//发送应答信号
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
		if(!(IIC_Write_Byte(SHT20_Add)))	//发送写命令并检查应答位
		{
			IIC_Write_Byte(TRIG_HUMI_MEASUREMENT_POLL);	//定位起始寄存器地址
			delay_ms(50);
			IIC_Start();	//重启总线
			IIC_Write_Byte(SHT20_Add+1);	//发送读取命令
			for(i = 0;i < 2;i++)
			{
				pBuff[i] = IIC_Read_Byte();	//读取数据
				if(i == (2 - 1))	IIC_Ack(1);	//发送非应答信号
				else IIC_Ack(0);	//发送应答信号
			}
		}
		IIC_Stop();	
    SRH = (pBuff[0] << 8) | (pBuff[1] << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}



