#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB9����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB9���ģʽ
//IO��������
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

unsigned char Read1307(unsigned char add);
unsigned char Write1307(unsigned char add,unsigned char dat);
u8 BCD_TO_DEC(u8 val);
u8 DEC_TO_BCD(u8 val);
int clockInit(void);
u8 getClockSecond(void);
u8 getClockMinute(void);
u8 getClockHour(void);
u8 getClockWeek(void);
u8 getClockDay(void);
u8 getClockMonth(void);
u8 getClockYear(void);

#endif

