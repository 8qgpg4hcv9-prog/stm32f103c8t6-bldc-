#include "stm32f10x.h"                  // Device header
//初始化霍尔传感器
void HALL_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
}
//将传感器得到的值读取出来返回使用
uint8_t HALL_Read(void){
	uint8_t hall=0;
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==1){hall|=0x01;}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==1){hall|=0x02;}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==1){hall|=0x04;}
	return hall;
}













