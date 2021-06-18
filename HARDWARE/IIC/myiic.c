#include "myiic.h"
#include "delay.h"

//��ʼ��ʱ���������
u8 timeBuf[7]= {0x0B,0x05,0x0A,0x03,0x10,0x06,0x14};

//��ʼ��IIC
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

    //GPIOB8,B9��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA=1;
    IIC_SCL=1;
    delay_us(4);
    IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL=1;
    IIC_SDA=1;//����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA����Ϊ����
    IIC_SDA=1;
    delay_us(1);
    IIC_SCL=1;
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL=0;//ʱ�����0
    return 0;
}
//����ACKӦ��
void IIC_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}
//������ACKӦ��
void IIC_NAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
            IIC_SDA=1;
        else
            IIC_SDA=0;
        txd<<=1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL=1;
        delay_us(2);
        IIC_SCL=0;
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
    for(i=0; i<8; i++ )
    {
        IIC_SCL=0;
        delay_us(2);
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}

//��DS1307��д��ʱ����Ϣ
unsigned char Write1307(unsigned char add,unsigned char dat)//дһ�ֽڵ���Ӧ��ַȥ
{
    unsigned char temp;
    //תBCD����д��1307
//  temp=dat/10;
//  temp<<=4;
//  temp=dat%10+temp;
    temp=DEC_TO_BCD(dat);
    IIC_Start();
    IIC_Send_Byte(0xD0);
    IIC_Wait_Ack();
    IIC_Send_Byte(add);
    IIC_Wait_Ack();
    IIC_Send_Byte(temp);
    IIC_Stop();
    return (0);
}

//��ȡDS1307ʱ������
unsigned char Read1307(unsigned char add)
{
    unsigned char dat;
    IIC_Start();
    IIC_Send_Byte(0xD0);
    IIC_Wait_Ack();
    IIC_Send_Byte(add);
    IIC_Wait_Ack();
    IIC_Stop();
    IIC_Start();
    IIC_Send_Byte(0xD1);
    IIC_Wait_Ack();
    dat=IIC_Read_Byte(add);
    IIC_NAck();
    IIC_Stop();
//  BCDתΪ16����
    dat=BCD_TO_DEC(dat);
    return(dat);
}

//ʱ���ʼ��
int clockInit()
{
    //����ȡ��ݲ�Ϊ2020������г�ʼ��ʱ������
    if(Read1307(0x06)!=0x14) {
        Write1307(0x00,timeBuf[0]);
        Write1307(0x01,timeBuf[1]);
        Write1307(0x02,timeBuf[2]);
        Write1307(0x03,timeBuf[3]);
        Write1307(0x04,timeBuf[4]);
        Write1307(0x05,timeBuf[5]);
        Write1307(0x06,timeBuf[6]);
        return 1;
    }
    return 1;
}

//BCDתʮ����
u8 BCD_TO_DEC(u8 val)
{
    u8 temp=0;
    temp=(val>>4)*10;
    return (temp+(val&0x0f));
}

//ʮ����תBCD
u8 DEC_TO_BCD(u8 val)
{
    u8 high=val / 10;
    u8 low =val %10 ;
    return (high<<4|low);
}

//��ȡ��
u8 getClockSecond()
{
    return Read1307(0x00);
}

//��ȡ����
u8 getClockMinute()
{
    return Read1307(0x01);
}

//��ȡСʱ
u8 getClockHour()
{
    return Read1307(0x02);
}

//��ȡ����
u8 getClockWeek()
{
    return Read1307(0x03);
}

//��ȡ��
u8 getClockDay()
{
    return Read1307(0x04);
}

//��ȡ�·�
u8 getClockMonth()
{
    return Read1307(0x05);
}

//��ȡ���
u8 getClockYear()
{
    return Read1307(0x06);
}
