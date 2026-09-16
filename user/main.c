#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "bldc.h"
#include "hall.h"
#include "encoder.h"
#include "key.h"
uint8_t duty;
uint8_t hall;
int32_t encoder_total_cnt;

//电机状态机 0停止 1正转 2反转
uint8_t bldc_state=0;

//uint16_t oled_counter=0;
int main(void){
	OLED_Init();
 	Encoder_Init();
	key_Init();
	OLED_ShowString(1,1,"hall:");
	OLED_ShowString(2,1,"duty:");
	BLDC_Init();
	HALL_Init();
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
while(1){
	uint8_t keynum=key_getnum();
//按键调整电机状态
	if(keynum==1){
	bldc_state=0;//按键1pb0 停止
	}
	if(keynum==2){
	bldc_state=1;//按键2pb10 正转
	}
	if(keynum==3){
	bldc_state=2;//按键1pb11 反转
	}
	
	//根据按键改变的状态机，选择电机状态
	switch(bldc_state){
	case 1 :encoder_total_cnt=Encoder_getcounter();
			duty=encoder_getduty(encoder_total_cnt);
			bldc_forward(duty);                                                                                                                                                    
			break;
	case 2 :encoder_total_cnt=Encoder_getcounter();
			duty=encoder_getduty(encoder_total_cnt);
			bldc_over(duty);
			break;
	default:BLDC_OFF();
			duty=0;
			break;
	}
	 //调试用的代码                     
	/*oled_counter++;
	if(oled_counter>=1000){
	
	hall=HALL_Read();
	OLED_ShowHexNum(1,6,hall,5);
	OLED_ShowNum(2,6,duty,5);
	OLED_ShowNum(3,6,bldc_state,5);
	uint8_t pb11_level = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
            OLED_ShowNum(4,6, pb11_level, 5);
	oled_counter=0;
	}*/    
}

}
