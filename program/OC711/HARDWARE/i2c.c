/*************************************************************************************************************
舸轮综合电子工作室(GIE工作室)        出品
Gearing Interated Electronics Studio

末世生存预警系统

软件IIC总线相关函数，大部分传感器均在该总线上

加群：714483620开源资料汇总
关注B站UP主：GIE工作室 获得更多视频资源

本开源资料仍需较为专业的技术水平，GIE工作室恕不提供本资料以及QQ群以外任何单独的技术支持
硬件、软件上均可能有未能发现的bug，对于本开源资料的使用可能造成的损失GIE工作室不负任何责任。

2019-3-21第一版
版权所有 禁止用于任何商业用途！
注：本程序中DS3231、SGP30、SHT20、VEML6070相关驱动文件来自互联网
*************************************************************************************************************/
#include "i2c.h"
#include "gpio.h"
#include "delay.h"

/*I2C起始*/
void IIC_Start(void)
{
	IIC_SDA_OUT();	

	SDA_H();
	delay_us(2);	
	SCL_H();
	delay_us(2);		
	SDA_L();
	delay_us(2);	
	SCL_L();
	delay_us(2);
}
/*I2C停止*/
void IIC_Stop(void)
{
	IIC_SDA_OUT();	

	SDA_L();	
	delay_us(2);
	SCL_L();	
	delay_us(2);
	SDA_H();
	delay_us(2);
}
/*I2C发送应答*/
void IIC_Ack(u8 a)
{
	IIC_SDA_OUT();	
	SCL_L();
	if(a)	
	SDA_H();
	else	
	SDA_L();

	delay_us(2);
	SCL_H();	
	delay_us(2);
	SCL_L();
	delay_us(2);

}
u8 I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	uint16_t i=0;
	SDA_H();	        //释放SDA
	SCL_H();        //SCL拉高进行采样
	IIC_SDA_IN();	
	while(IN_SDA())//等待SDA拉低
	{
		i++;      //等待计数
		if(i==500)//超时跳出循环
		break;
	}
	if(IN_SDA())//再次判断SDA是否拉低
	{
		SCL_L(); 
		return RESET;//从机应答失败，返回0
	}
  delay_us(5);//延时保证时钟频率低于40K，
	SCL_L();
	delay_us(5); //延时保证时钟频率低于40K，
	return SET;//从机应答成功，返回1
}
/*I2C写入一个字节*/
u8 IIC_Write_Byte(u8 dat)
{
	u8 i;
	u8 iic_ack=0;	

	IIC_SDA_OUT();	

	for(i = 0;i < 8;i++)
	{
		if(dat & 0x80)	
		SDA_H();
		else	
		SDA_L();
			
		delay_us(2);
		SCL_H();
	    delay_us(2);
		SCL_L();
		dat<<=1;
	}

	SDA_H();	//释放数据线

	IIC_SDA_IN();	//设置成输入

	delay_us(2);
	SCL_H();
	delay_us(2);
	
	iic_ack |= IN_SDA();	//读入应答位
	SCL_L();
	return iic_ack;	//返回应答信号
}
/*I2C读取一个字节*/
u8 IIC_Read_Byte(void)
{
	u8 i;
	u8 x=0;

	SDA_H();	//首先置数据线为高电平

	IIC_SDA_IN();	//设置成输入

	for(i = 0;i < 8;i++)
	{
		x <<= 1;	//读入数据，高位在前

		delay_us(2);
		SCL_H();	//突变
		delay_us(2);
		
		if(IN_SDA())	x |= 0x01;	//收到高电平

		SCL_L();
		delay_us(2);
	}	//数据接收完成

	SCL_L();

	return x;	//返回读取到的数据
}
u8 Single_WriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 data)
{
	  IIC_Start();  //起始信号
	if(!(IIC_Write_Byte(Slave_Address)))	//发送写命令并检查应答位
	{
		if(!(IIC_Write_Byte(REG_Address)))
		{
			if(!(IIC_Write_Byte(data)))
				return SET;
		}
			//发送数据
		IIC_Stop();
		return RESET;
	}
	IIC_Stop();
	return RESET;
	
}
u8 Single_WriteI2C_com(u8 Slave_Address,u8 REG_Address)
{
	  IIC_Start();  //起始信号
	if(!(IIC_Write_Byte(Slave_Address)))	//发送写命令并检查应答位
	{
		if(!(IIC_Write_Byte(REG_Address)))
				return SET;
			//发送数据
		IIC_Stop();
		return RESET;
	}
	IIC_Stop();
	return RESET;
}
u8 Single_MReadI2C_byte(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length)
{
	int i;
		IIC_Start();
		if(!(IIC_Write_Byte(Slave_Address)))	//发送写命令并检查应答位
		{
			if(!(IIC_Write_Byte(REG_Address)))	//定位起始寄存器地址
			{
				IIC_Start();	//重启总线
				
				if(!(IIC_Write_Byte(Slave_Address+1)))	//发送读取命令
				{
					for(i = 0;i < length;i++)
					{
						REG_data[i] = IIC_Read_Byte();	//读取数据
						if(i == (length - 1))	IIC_Ack(1);	//发送非应答信号
						else IIC_Ack(0);	//发送应答信号
						delay_us(10);
					}
					IIC_Stop();
					return 1;
				}
			}
		}
		IIC_Stop();	
		return 0;
}

u8 Single_MWriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length)
{
		int i;
		IIC_Start();
		if(!(IIC_Write_Byte(Slave_Address)))	//发送写命令并检查应答位
		{
			if(!(IIC_Write_Byte(REG_Address)))	//定位起始寄存器地址
			{
				for(i = 0;i < length;i++)
				{
					IIC_Write_Byte(REG_data[i]);	//读取数据
					//if(!I2C_WaitAck()){IIC_Stop(); return 0;}	//发送非应答信
					Soft_delay_ms(1);
				}
				IIC_Stop();
				return 1;
			}
		}
		IIC_Stop();	
		
		return 0;
// 		 IIC_Start();  //起始信号          

//     IIC_Write_Byte(Slave_Address);   //发送设备地址+写信号
//  	  if(!I2C_WaitAck()){IIC_Stop(); return RESET;}
//    
// 		IIC_Write_Byte(REG_Address);    //内部寄存器地址，
//  	  if(!I2C_WaitAck()){IIC_Stop(); return RESET;}
//    
// 	while(length)
// 	{
// 		IIC_Write_Byte(*REG_data++);       //内部寄存器数据，
// 	   if(!I2C_WaitAck()){IIC_Stop(); return RESET;}           //应答
// 		length--;
// 	}
// 	//	I2C_SendByte(*data);       //内部寄存器数据，
//  	//	if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
// 		IIC_Stop();   //发送停止信号		
// 		return SET;
}




