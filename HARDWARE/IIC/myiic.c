#include "myiic.h"
#include "delay.h"

//初始化时间变量设置
u8 timeBuf[7]= {0x0B,0x05,0x0A,0x03,0x10,0x06,0x14};

//初始化IIC
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

    //GPIOB8,B9初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
}

//产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA=1;
    IIC_SCL=1;
    delay_us(4);
    IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL=1;
    IIC_SDA=1;//发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA设置为输入
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
    IIC_SCL=0;//时钟输出0
    return 0;
}
//产生ACK应答
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
//不产生ACK应答
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
            IIC_SDA=1;
        else
            IIC_SDA=0;
        txd<<=1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL=1;
        delay_us(2);
        IIC_SCL=0;
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}

//往DS1307中写入时间信息
unsigned char Write1307(unsigned char add,unsigned char dat)//写一字节到对应地址去
{
    unsigned char temp;
    //转BCD码再写入1307
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

//读取DS1307时钟数据
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
//  BCD转为16进制
    dat=BCD_TO_DEC(dat);
    return(dat);
}

//时间初始化
int clockInit()
{
    //若读取年份不为2020，则进行初始化时间设置
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

//BCD转十进制
u8 BCD_TO_DEC(u8 val)
{
    u8 temp=0;
    temp=(val>>4)*10;
    return (temp+(val&0x0f));
}

//十进制转BCD
u8 DEC_TO_BCD(u8 val)
{
    u8 high=val / 10;
    u8 low =val %10 ;
    return (high<<4|low);
}

//获取秒
u8 getClockSecond()
{
    return Read1307(0x00);
}

//获取分钟
u8 getClockMinute()
{
    return Read1307(0x01);
}

//获取小时
u8 getClockHour()
{
    return Read1307(0x02);
}

//获取星期
u8 getClockWeek()
{
    return Read1307(0x03);
}

//获取天
u8 getClockDay()
{
    return Read1307(0x04);
}

//获取月份
u8 getClockMonth()
{
    return Read1307(0x05);
}

//获取年份
u8 getClockYear()
{
    return Read1307(0x06);
}
