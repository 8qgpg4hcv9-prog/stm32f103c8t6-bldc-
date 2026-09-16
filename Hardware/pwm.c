#include "stm32f10x.h"                  // Device header
//初始化pwm互补输出，设置死区，pwm频率
void PWM_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	TIM_InternalClockConfig(TIM1);
	
	TIM_TimeBaseInitTypeDef TIM_TimebaseInitstructure;
	TIM_TimebaseInitstructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimebaseInitstructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimebaseInitstructure.TIM_Period=99;//ARR
	TIM_TimebaseInitstructure.TIM_Prescaler=71;//PSC
	TIM_TimebaseInitstructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1,&TIM_TimebaseInitstructure);

	TIM_OCInitTypeDef TIM_OCInitstucture;
	TIM_OCInitstucture.TIM_OCMode=TIM_OCMode_PWM1;//pwm1 ccr<cnt输出有效电平 pwm2反过来
	TIM_OCInitstucture.TIM_OCIdleState=TIM_OCIdleState_Reset;//当关闭pwm时主引脚状态，出事时拉低让管子关断
	TIM_OCInitstucture.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//当关闭pwm时互补引脚状态，出事时拉低让管子关断
	TIM_OCInitstucture.TIM_OCNPolarity=TIM_OCNPolarity_High;//pwm模式中有效电平输出高还是低，互补通道，都设置高不会互补
	TIM_OCInitstucture.TIM_OCPolarity=TIM_OCPolarity_High;//pwm模式中有效电平输出高还是低，互补通道
	TIM_OCInitstucture.TIM_OutputNState=TIM_OutputNState_Disable;//互补通道使能,分闸
	TIM_OCInitstucture.TIM_OutputState=TIM_OutputState_Disable;//主输出通道使能，分闸
	TIM_OCInitstucture.TIM_Pulse=0;
	TIM_OC1Init(TIM1,&TIM_OCInitstucture);
	TIM_OC2Init(TIM1,&TIM_OCInitstucture);
	TIM_OC3Init(TIM1,&TIM_OCInitstucture);
	
	TIM_BDTRInitTypeDef TIM_BDTRInitstructure;
	TIM_BDTRInitstructure.TIM_AutomaticOutput=TIM_AutomaticOutput_Disable;//互补通道输出的开关
	TIM_BDTRInitstructure.TIM_Break=TIM_Break_Disable;//硬件刹车信号，给一个信号pwm停止输出，电机切割线圈刹车
	TIM_BDTRInitstructure.TIM_BreakPolarity=TIM_BreakPolarity_High;//给高电平刹车
	TIM_BDTRInitstructure.TIM_DeadTime=100;//死区时间
	TIM_BDTRInitstructure.TIM_LOCKLevel=TIM_LOCKLevel_OFF;//bdtr内容是否可以修改，锁
	TIM_BDTRInitstructure.TIM_OSSIState=TIM_OSSIState_Enable;//停止时moe被刹车信号置0保护电平不乱跳防止mos误触发
	TIM_BDTRInitstructure.TIM_OSSRState=TIM_OSSRState_Enable;//运行时，分闸关闭状态，保护电平不乱跳
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitstructure);
	
	//TIM_CtrlPWMOutputs(TIM1,ENABLE);//TIM1pwm输出通道的总电闸
	
	TIM_Cmd(TIM1,ENABLE);
	

}
void PWM_setcompare1(uint16_t compare){

TIM_SetCompare1(TIM1,compare);

}

void PWM_setcompare2(uint16_t compare){

TIM_SetCompare2(TIM1,compare);

}


void PWM_setcompare3(uint16_t compare){

TIM_SetCompare3(TIM1,compare);

}

