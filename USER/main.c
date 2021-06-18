#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "sys.h"
#include "myiic.h"

int main(void)
{
	int second,minute,hour,week,day,month,year;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ����
	uart_init(9600);	//��ʼ�����ڲ�����Ϊ115200
	IIC_Init();			//IIC��ʼ�� 
	clockInit();
 	while(1)//��ⲻ��24c02
	{
		second=getClockSecond();
		minute=getClockMinute();
		hour=getClockHour();
		week=getClockWeek();
		day=getClockDay();
		month=getClockMonth();
		year=getClockYear();
	  printf("20%d��%d��%d�գ�����%d",year,month,day,week);
		printf("\r\n");
		printf("%d:%d:%d",hour,minute,second);
		printf("\r\n");
		delay_ms(1000);
	}
}
