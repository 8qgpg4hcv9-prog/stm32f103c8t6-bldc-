#include "stm32f10x.h"                  // Device header
#include "pwm.h"
#include "hall.h"
//初始化bldc，和使能bldc的gpio
void BLDC_Init (void){
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
}
//使能bldc，上电后要将SHDN位先拉低再拉高
void BLDC_ENABLE(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

/*
正转逻辑
 HU PA1  HV PA2 HWPA3;

 60     U+V-  101 0X05 
 120    U+W-  001 0X01
 180    V+W-  011 0X03
 240    V+U-  010 0X02
 300    W+U-  110 0X06
 360    W+V-  100 0X04
*/

//关闭所有mos管
void BLDC_OFF(void){
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);

}

//旋转磁场正转两相导通，先关闭后打开防止炸管，每次只切换一个相，防止占空比高管子切换快产生电流尖峰，触发过流保护
void BLDC_forward_60(uint8_t duty){//u+v- 
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);//u+
	PWM_setcompare1(duty);//u相上桥占空比20
	
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Enable);//v-
	PWM_setcompare2(100);//v下桥恒高 
}

void BLDC_forward_120(uint8_t duty){//U+w-              
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);//u+
	PWM_setcompare1(duty);//u相上桥占空比20
	
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Enable);//w-
	PWM_setcompare3(100);//w下桥恒高
}
void BLDC_forward_180(uint8_t duty){//v+w-
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Enable);//v+
	PWM_setcompare2(duty);//w相上桥占空比20
	
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Enable);//w-
	PWM_setcompare3(100);//u下桥恒高
}
void BLDC_forward_240(uint8_t duty){//v+u-
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Enable);//v+
	PWM_setcompare2(duty);//w相上桥占空比20
	
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);//u-
	PWM_setcompare1(100);//u下桥恒高
}
void BLDC_forward_300(uint8_t duty){//w+u-
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Enable);//w+
	PWM_setcompare3(duty);//w相上桥占空比20
	
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);//u-
	PWM_setcompare1(100);//u下桥恒高
}

void BLDC_forward_360(uint8_t duty){//w+v-
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Enable);//w+
	PWM_setcompare3(duty);//w相上桥占空比20]
	
		
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Enable);//v-
	PWM_setcompare2(100);//v下桥恒高
}
void bldc_forward(uint8_t duty){
	/*        321    
 60     U+V-  101 0X05
 120    U+W-  001 0X01
 180    V+W-  011 0X03
 240    V+U-  010 0X02
 300    W+U-  110 0X06
 360    W+V-  100 0X04
*/
	BLDC_ENABLE();
	uint8_t hall;
	hall=HALL_Read();
	switch(hall){
	 case 0x05: BLDC_forward_60(duty); break;
	 case 0X01: BLDC_forward_120(duty); break;
	 case 0X03: BLDC_forward_180(duty); break;
     case 0X02: BLDC_forward_240(duty); break;
	 case 0X06: BLDC_forward_300(duty); break;
	 case 0X04: BLDC_forward_360(duty); break;
	 default:BLDC_OFF(); break;  
	}
}


/*
反转逻辑
 HU PA1  HV PA2 HWPA3;

 60     U-V+  101 0X05 
 120    U-W+  001 0X01
 180    V-W+  011 0X03
 240    V-U+  010 0X02
 300    W-U+  110 0X06
 360    W-V+  100 0X04
*/

void BLDC_over_60(uint8_t duty){//u-v+ 
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);//u-
	PWM_setcompare1(100);//u相下桥恒高
	
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Enable);//v+
	PWM_setcompare2(duty);//v上桥占空比 
}

void BLDC_over_120(uint8_t duty){//u-w+              
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Enable);//u-
	PWM_setcompare1(100);//u相下桥恒高
	
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Enable);//w+
	PWM_setcompare3(duty);//w上桥占空比
}

void BLDC_over_180(uint8_t duty){//v-w+
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Enable);//v-
	PWM_setcompare2(100);//V下桥恒高
	
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Enable);//w+
	PWM_setcompare3(duty);//w上桥占空比
}
void BLDC_over_240(uint8_t duty){//v-u+
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Enable);//v-
	PWM_setcompare2(100);//V下桥恒高
	
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);//u+
	PWM_setcompare1(duty);//w上桥占空比
}	
	
void BLDC_over_300(uint8_t duty){//w-u+
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Enable);//w-
	PWM_setcompare3(100);//w相下桥恒高
	
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);//u+
	PWM_setcompare1(duty);//u下桥恒高
}
void BLDC_over_360(uint8_t duty){//w-v+
	TIM_CCxNCmd(TIM1,TIM_Channel_1,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_2,TIM_CCxN_Disable);
	TIM_CCxNCmd(TIM1,TIM_Channel_3,TIM_CCxN_Enable);//w-
	PWM_setcompare3(100);//w相下桥恒高
	
		
	TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Enable);//v+
	PWM_setcompare2(duty);//v下桥恒高
}

void bldc_over(uint8_t duty){
/*
 HU PA1  HV PA2 HWPA3;

 60     U-V+  101 0X05 
 120    U-W+  001 0X01
 180    V-W+  011 0X03
 240    V-U+  010 0X02
 300    W-U+  110 0X06
 360    W-V+  100 0X04
*/
	BLDC_ENABLE();
	uint8_t hall;
	hall=HALL_Read();
	switch(hall){
	 case 0x05: BLDC_over_60(duty); break;
	 case 0X01: BLDC_over_120(duty); break;
	 case 0X03: BLDC_over_180(duty); break;
     case 0X02: BLDC_over_240(duty); break;
	 case 0X06: BLDC_over_300(duty); break;
	 case 0X04: BLDC_over_360(duty); break;
	 default:BLDC_OFF(); break;  
	}
}






