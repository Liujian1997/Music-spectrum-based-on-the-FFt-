#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "oled.h"
#include "adc.h"
#include "fft.h"
#include "math.h"
#include "timer.h"

#define units 0.015 //y轴计量单位

int time_flag=0;



void OLED_display(int data[]) //画图函数
{
	int x,y;
	OLED_Clear();//清屏
	for(x=0;x<128;x+=2)//
	{
		for(y=0;y<data[x/2]*units;y++)
		{
			OLED_DrawPoint(128-x,y,1);
			OLED_DrawPoint(128-x+1,y,1);
		}
	}
	OLED_Refresh_Gram();//更新显示
}

int main(void)
 {	
	int i;
	u16 adcx;
	float temp;//电压值
	int flag=0;//循环次数标志位
	int data[128]={0};
	struct compx s[FFT_N];
	delay_init();	     //延时初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	TIM3_Int_Init(9,7199);
	uart_init(9600);	   
 	Adc_Init();
  OLED_Init();			//初始化OLED   
	OLED_Refresh_Gram();//更新显示到OLED	 
	while(1) 
	{	
			adcx=Get_Adc_Average(ADC_Channel_1,5);
			temp=adcx*(3.3/4096);
			//adcx=temp;
			//temp-=adcx;
			temp*=100;
		while(flag<128&&time_flag==1)
		{
			s[flag].real = temp;
      s[flag].imag = 0;
			flag++;
			time_flag=0;
		}
		if(flag>=128)
		{
			flag=0;
			FFT(s);
			for(i=0;i<64;i++)
			{
				data[i]=sqrt(s[i].real * s[i].real + s[i].imag * s[i].imag);
				//printf("%d\t",data[i]);
			}
			OLED_display(data);
		}

	}	 
}
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		time_flag=1;
		}
}

