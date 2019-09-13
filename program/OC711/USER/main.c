//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : Evk123
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : 0.87寸OLED 接口演示例程(STM32F103C8系列IIC)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PA5（SCL）
//              SDA   接PA7（SDA）            
//              ----------------------------------------------------------------
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "SHT2x.h"
#include "ds3231.h"
#include "gpio.h"
#include "usart.h"
#include "adc.h"
#include "wkup.h" 


u8 temp_flag=0,sht_cnt=0,bat_cnt=0,point_flag=0;
u8 temp_disp[8],humi_disp[8],time_disp[17],bat_disp[8];
int sec_flag,second_last;


void SHT_read()
{
	float temp_cache,humi_cache;
	
	temp_cache=SHT2x_GetTempPoll();
	delay_ms(10);
	humi_cache=SHT2x_GetHumiPoll();
	
	if(temp_cache>=0)temp_flag=0;
		else temp_flag=1;
	
	temp_cache=temp_cache*100;
	
	if(temp_flag==0)
	{
		temp_disp[0]=(u16)temp_cache%10000/1000+48;
		temp_disp[1]=(u16)temp_cache%1000/100+48;
		temp_disp[2]='.';
		temp_disp[3]=(u16)temp_cache%100/10+48;
		temp_disp[4]=(u16)temp_cache%10+48;
		temp_disp[5]='C';
		temp_disp[6]=' ';
		temp_disp[7]=0;
	}
	else if(temp_flag==1)
	{
		temp_disp[0]='-';
		temp_disp[1]=(u16)temp_cache%10000/1000+48;
		temp_disp[2]=(u16)temp_cache%1000/100+48;
		temp_disp[3]='.';
		temp_disp[4]=(u16)temp_cache%100/10+48;
		temp_disp[5]=(u16)temp_cache%10+48;
		temp_disp[6]=' ';
		temp_disp[7]=0;
	}
	
	humi_cache=humi_cache*100;
	humi_disp[0]=(u16)humi_cache%10000/1000+48;
	humi_disp[1]=(u16)humi_cache%1000/100+48;
	humi_disp[2]='.';
	humi_disp[3]=(u16)humi_cache%100/10+48;
	humi_disp[4]=(u16)humi_cache%10+48;
	humi_disp[5]='%';	
	humi_disp[6]=' ';
	humi_disp[7]=0;
}

void time_read()
{
	Time_Handle();
	
	time_disp[0]=Display_Date[0];
	time_disp[1]=Display_Date[1];
	time_disp[2]=Display_Date[2];
	time_disp[3]=Display_Date[3];
	time_disp[4]='/';
	time_disp[5]=Display_Date[5];
	time_disp[6]=Display_Date[6];
	time_disp[7]='/';
	time_disp[8]=Display_Date[8];
	time_disp[9]=Display_Date[9];
	time_disp[10]=' ';
	time_disp[11]=Display_Time[0];
	time_disp[12]=Display_Time[1];
	if(point_flag==0)time_disp[13]=':';
	if(point_flag==1)time_disp[13]=' ';
	time_disp[14]=Display_Time[3];
	time_disp[15]=Display_Time[4];
	time_disp[16]=0;	
	
}

void bat_read()
{
	float adc_cache;
//		GPIO_SetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	delay_ms(3);
	adc_cache=Get_Adc_Average(ADC_Channel_1,4);
	adc_cache=(float)adc_cache*(3.3/4096);
	adc_cache=adc_cache*200;
	
//	bat_disp[0]=(u16)adc_cache/1000+48;
	bat_disp[0]=(u16)adc_cache%1000/100+48;
	bat_disp[1]='.';
	bat_disp[2]=(u16)adc_cache%100/10+48;
	bat_disp[3]=(u16)adc_cache%10+48;
	bat_disp[4]='V';
	bat_disp[5]=' ';
	bat_disp[6]=' ';
	bat_disp[7]=0;
	
//	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}


int main(void)
{	
	RCC_ClocksTypeDef get_rcc_clock;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	
	GPIO_Config();
	IIC_SDA_OUT();
	
	uart_init(9600);	 //串口初始化为9600
	
	 

	//调用RCC_GetClocksFreq获取系统时钟状态
	RCC_GetClocksFreq(&get_rcc_clock);  
	
	SHT2x_Init();
	SHT2x_SoftReset();
	
	DS3231_Write_Byte(Address_control,RESET_BBSQW);//int/sqw高阻
	DS3231_Write_Byte(Address_control_status,Disable_OSC32768);//EN32KHz高阻
	
	TimeValue.year=0x2019;//如果下面那行不打开，这些初始时间均无效	
	TimeValue.month=0x09;
	TimeValue.week=0x07;
	TimeValue.date=0x08;
	TimeValue.hour=0x12;
	TimeValue.minute=0x44;
	TimeValue.second=0x30;
//	DS3231_Time_Init(&TimeValue);//调试用，程序强制写入时间值
	
	OLED_Init();			//初始化OLED  
	OLED_Clear(); 
	OLED_ShowStringDelay(0,0,"System",8,50);
	OLED_ShowStringDelay(0,1,"Initializing",8,50);
	delay_ms(300);
	OLED_ShowStringDelay(0,2,"................",8,200);
	OLED_ShowStringDelay(0,3,"Done",8,50);
	delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"Temp:reading",8);
	OLED_ShowString(0,1,"Humi:reading",8);
	OLED_ShowString(0,2,"2019/9/08 16:26",8);
	OLED_ShowString(0,3,"Battery:reading",8);
	while(1) 
	{		
		if(USART_RX_STA&0x8000)
		{			
			TimeValue.year=(USART_RX_BUF[0]-30<<12)+(USART_RX_BUF[1]-30<<8)+(USART_RX_BUF[2]-30<<4)+USART_RX_BUF[3]-30;
			TimeValue.month=(USART_RX_BUF[4]-30<<4)+USART_RX_BUF[5]-30;
			TimeValue.date=(USART_RX_BUF[6]-30<<4)+USART_RX_BUF[7]-30;
			TimeValue.hour=(USART_RX_BUF[8]-30<<4)+USART_RX_BUF[9]-30;
			TimeValue.minute=(USART_RX_BUF[10]-30<<4)+USART_RX_BUF[11]-30;
			
			USART_RX_STA=0;
			
			OLED_Clear();
			OLED_ShowString(0,0,"Time Refreshed",8);
			delay_ms(500);
			OLED_ShowString(0,0,"Temp:",8);
			OLED_ShowString(0,1,"Humi:",8);
			//OLED_ShowString(0,2,"2019/09/22 23:04",8);
			OLED_ShowString(0,3,"Battery:",8);
		}
		
		time_read();
		if(sec_flag==1)
		{
			sht_cnt++;
			bat_cnt++;
			if(point_flag==0)point_flag=1;
				else if(point_flag==1)point_flag=0;
			if(sht_cnt==10)
			{
				sht_cnt=0;
				SHT_read();
			}
			if(bat_cnt==10)
			{
				bat_cnt=0;
				bat_read();
			}				
			
					
			OLED_ShowString(40,0,temp_disp,8);
			OLED_ShowString(40,1,humi_disp,8);
			OLED_ShowString(0,2,time_disp,8);
			OLED_ShowString(64,3,bat_disp,8);
		}
		
		delay_ms(300);


	}	  	
}

