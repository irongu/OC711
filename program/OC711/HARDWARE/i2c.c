/*************************************************************************************************************
�����ۺϵ��ӹ�����(GIE������)        ��Ʒ
Gearing Interated Electronics Studio

ĩ������Ԥ��ϵͳ

���IIC������غ������󲿷ִ��������ڸ�������

��Ⱥ��714483620��Դ���ϻ���
��עBվUP����GIE������ ��ø�����Ƶ��Դ

����Դ���������Ϊרҵ�ļ���ˮƽ��GIE������ˡ���ṩ�������Լ�QQȺ�����κε����ļ���֧��
Ӳ��������Ͼ�������δ�ܷ��ֵ�bug�����ڱ���Դ���ϵ�ʹ�ÿ�����ɵ���ʧGIE�����Ҳ����κ����Ρ�

2019-3-21��һ��
��Ȩ���� ��ֹ�����κ���ҵ��;��
ע����������DS3231��SGP30��SHT20��VEML6070��������ļ����Ի�����
*************************************************************************************************************/
#include "i2c.h"
#include "gpio.h"
#include "delay.h"

/*I2C��ʼ*/
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
/*I2Cֹͣ*/
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
/*I2C����Ӧ��*/
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
u8 I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	uint16_t i=0;
	SDA_H();	        //�ͷ�SDA
	SCL_H();        //SCL���߽��в���
	IIC_SDA_IN();	
	while(IN_SDA())//�ȴ�SDA����
	{
		i++;      //�ȴ�����
		if(i==500)//��ʱ����ѭ��
		break;
	}
	if(IN_SDA())//�ٴ��ж�SDA�Ƿ�����
	{
		SCL_L(); 
		return RESET;//�ӻ�Ӧ��ʧ�ܣ�����0
	}
  delay_us(5);//��ʱ��֤ʱ��Ƶ�ʵ���40K��
	SCL_L();
	delay_us(5); //��ʱ��֤ʱ��Ƶ�ʵ���40K��
	return SET;//�ӻ�Ӧ��ɹ�������1
}
/*I2Cд��һ���ֽ�*/
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

	SDA_H();	//�ͷ�������

	IIC_SDA_IN();	//���ó�����

	delay_us(2);
	SCL_H();
	delay_us(2);
	
	iic_ack |= IN_SDA();	//����Ӧ��λ
	SCL_L();
	return iic_ack;	//����Ӧ���ź�
}
/*I2C��ȡһ���ֽ�*/
u8 IIC_Read_Byte(void)
{
	u8 i;
	u8 x=0;

	SDA_H();	//������������Ϊ�ߵ�ƽ

	IIC_SDA_IN();	//���ó�����

	for(i = 0;i < 8;i++)
	{
		x <<= 1;	//�������ݣ���λ��ǰ

		delay_us(2);
		SCL_H();	//ͻ��
		delay_us(2);
		
		if(IN_SDA())	x |= 0x01;	//�յ��ߵ�ƽ

		SCL_L();
		delay_us(2);
	}	//���ݽ������

	SCL_L();

	return x;	//���ض�ȡ��������
}
u8 Single_WriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 data)
{
	  IIC_Start();  //��ʼ�ź�
	if(!(IIC_Write_Byte(Slave_Address)))	//����д������Ӧ��λ
	{
		if(!(IIC_Write_Byte(REG_Address)))
		{
			if(!(IIC_Write_Byte(data)))
				return SET;
		}
			//��������
		IIC_Stop();
		return RESET;
	}
	IIC_Stop();
	return RESET;
	
}
u8 Single_WriteI2C_com(u8 Slave_Address,u8 REG_Address)
{
	  IIC_Start();  //��ʼ�ź�
	if(!(IIC_Write_Byte(Slave_Address)))	//����д������Ӧ��λ
	{
		if(!(IIC_Write_Byte(REG_Address)))
				return SET;
			//��������
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
		if(!(IIC_Write_Byte(Slave_Address)))	//����д������Ӧ��λ
		{
			if(!(IIC_Write_Byte(REG_Address)))	//��λ��ʼ�Ĵ�����ַ
			{
				IIC_Start();	//��������
				
				if(!(IIC_Write_Byte(Slave_Address+1)))	//���Ͷ�ȡ����
				{
					for(i = 0;i < length;i++)
					{
						REG_data[i] = IIC_Read_Byte();	//��ȡ����
						if(i == (length - 1))	IIC_Ack(1);	//���ͷ�Ӧ���ź�
						else IIC_Ack(0);	//����Ӧ���ź�
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
		if(!(IIC_Write_Byte(Slave_Address)))	//����д������Ӧ��λ
		{
			if(!(IIC_Write_Byte(REG_Address)))	//��λ��ʼ�Ĵ�����ַ
			{
				for(i = 0;i < length;i++)
				{
					IIC_Write_Byte(REG_data[i]);	//��ȡ����
					//if(!I2C_WaitAck()){IIC_Stop(); return 0;}	//���ͷ�Ӧ����
					Soft_delay_ms(1);
				}
				IIC_Stop();
				return 1;
			}
		}
		IIC_Stop();	
		
		return 0;
// 		 IIC_Start();  //��ʼ�ź�          

//     IIC_Write_Byte(Slave_Address);   //�����豸��ַ+д�ź�
//  	  if(!I2C_WaitAck()){IIC_Stop(); return RESET;}
//    
// 		IIC_Write_Byte(REG_Address);    //�ڲ��Ĵ�����ַ��
//  	  if(!I2C_WaitAck()){IIC_Stop(); return RESET;}
//    
// 	while(length)
// 	{
// 		IIC_Write_Byte(*REG_data++);       //�ڲ��Ĵ������ݣ�
// 	   if(!I2C_WaitAck()){IIC_Stop(); return RESET;}           //Ӧ��
// 		length--;
// 	}
// 	//	I2C_SendByte(*data);       //�ڲ��Ĵ������ݣ�
//  	//	if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
// 		IIC_Stop();   //����ֹͣ�ź�		
// 		return SET;
}




