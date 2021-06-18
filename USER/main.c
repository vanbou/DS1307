#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "sys.h"
#include "myiic.h"

int main(void)
{
	int second,minute,hour,week,day,month,year;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	uart_init(9600);	//初始化串口波特率为115200
	IIC_Init();			//IIC初始化 
	clockInit();
 	while(1)//检测不到24c02
	{
		second=getClockSecond();
		minute=getClockMinute();
		hour=getClockHour();
		week=getClockWeek();
		day=getClockDay();
		month=getClockMonth();
		year=getClockYear();
	  printf("20%d年%d月%d日，星期%d",year,month,day,week);
		printf("\r\n");
		printf("%d:%d:%d",hour,minute,second);
		printf("\r\n");
		delay_ms(1000);
	}
}
