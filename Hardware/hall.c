#include "stm32f10x.h"                  // Device header
//初始化霍尔传感器
static uint32_t hall_cnt;//记录霍尔跳变边沿次数
void HALL_Init(void){
	//初始化gpio和afio时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//初始化usart2gpio
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	//设置afio
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
	//初始化外部中断
	EXTI_InitTypeDef EXTI_Initstructure;
	EXTI_Initstructure.EXTI_Line=EXTI_Line1|EXTI_Line4|EXTI_Line5;
	EXTI_Initstructure.EXTI_LineCmd=ENABLE;
	EXTI_Initstructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_Initstructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_Initstructure);
	//初始化nvic
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_Initstructure;
	NVIC_Initstructure.NVIC_IRQChannel=EXTI1_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_Initstructure);
	
	NVIC_Initstructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_Init(&NVIC_Initstructure);
    
    NVIC_Initstructure.NVIC_IRQChannel = EXTI9_5_IRQn;   // 注意：PA5 属于 5-9 组！
    NVIC_Init(&NVIC_Initstructure);
	
	
}
//三个霍尔检测口检测到跳变边沿就让计数器加1
void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        hall_cnt++;
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
void EXTI4_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
        hall_cnt++;
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}
void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
        hall_cnt++;
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}

//将传感器得到的值读取出来返回使用，六步换相依靠这个
uint8_t HALL_Read(void){
	uint8_t hall=0;
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==1){hall|=0x01;}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==1){hall|=0x02;}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==1){hall|=0x04;}
	return hall;
}
//返回hall边沿数量，可以结合定时器定时中断计算转速
uint32_t HALL_Getedge(void){
	uint32_t edge=hall_cnt;
	hall_cnt=0;
	return edge;
}













