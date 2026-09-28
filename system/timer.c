#include "stm32f10x.h"                  // Device header

void Timer_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//开启时钟
	
	TIM_InternalClockConfig(TIM2);//选择内部时钟源
	
	TIM_TimeBaseInitTypeDef TIM_TimebaseInitstructure;//时基单元初始化
	TIM_TimebaseInitstructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimebaseInitstructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimebaseInitstructure.TIM_Period=99;//ARR
	TIM_TimebaseInitstructure.TIM_Prescaler=719;//PSC
	TIM_TimebaseInitstructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimebaseInitstructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//由于时基单元初始化完成会进入一次中断，这里清除中断标志位
	
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//使能tim定时中断，当溢出发生中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置nvic分组
	
	NVIC_InitTypeDef NVIC_Initstructure;
	NVIC_Initstructure.NVIC_IRQChannel=TIM2_IRQn;				//选择配置NVIC的TIM2线
	NVIC_Initstructure.NVIC_IRQChannelCmd=ENABLE;				//指定NVIC线路使能
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=2;	//指定NVIC线路的抢占优先级为2
	NVIC_Initstructure.NVIC_IRQChannelSubPriority=1;	
	NVIC_Init(&NVIC_Initstructure);
	
	TIM_Cmd(TIM2,ENABLE);
}
/*void TIM2_IRQHandler(void){
if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
{
	
	
	
	
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

}*/












