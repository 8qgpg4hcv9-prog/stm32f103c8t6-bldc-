#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "bldc.h"
#include "hall.h"
#include "encoder.h"
#include "key.h"
#include "timer.h"
#include "Serial.h"
volatile uint8_t duty;
uint8_t hall;

//电机状态机 0停止 1正转 2反转
uint8_t bldc_state=0;

//pid 需要的变量
volatile float target;
volatile float actual;
volatile float out;//目标值 实际值 输出值
float kp=1.3,ki=0.09,kd=0;//pid系数
float error0,error1;//当前误差 上一次误差 误差的积分
volatile float errorint;
#define DUTY_STEP_UP    2   // 每个PID周期(50ms)最多增加2%占空比
#define DUTY_STEP_DOWN  5  // 下降可以快一点，利于抑制超调

//串口延迟
uint16_t usart_cnt;
int main(void){
	OLED_Init();
	BLDC_Init();
	HALL_Init();
	Encoder_Init();
	key_Init();
	Timer_Init();
	Serial_Init();
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	target=20.0f;
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
	case 1 :bldc_forward(duty);                                                                                                                                                    
			break;
	case 2 :bldc_over(duty);
			break;
	default:BLDC_OFF();
			duty=0;
			break;
	}

	//利用编码器修改target（目标值）
	int32_t enc_delta=Encoder_getdelta();
	
	target += enc_delta * 0.3f;

    // 目标限幅
    if(target > 75.0f) target = 75.0f;  // 最高60rps ≈ 3000rpm
    if(target < 5.0f ) target = 5.0f;

	usart_cnt++;
	if(usart_cnt>=500){
	Serial_Printf("%.1f,%.lf,%.lf\r\n",target,actual,out);
	usart_cnt=0;
	}
		
}

}

//定时器定时中断1ms
void TIM2_IRQHandler(void){
	static uint8_t count;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{

		count++;
		//pid调控周期
		if(count>=50){//pid部分
			count=0;
			//获取实际值
			// 1. 获取原始跳变次数
            uint32_t edge = HALL_Getedge();
            // 2. 将边沿转换为转每秒speed=路程/时间=（edge/12(转了多少圈））/50ms=((float)edge/12)/0.05;
			float speed	= edge*1.666f;
			//一阶低通滤波 新的平滑值 = 上一次平滑值 × 0.85 + 本次新测到的原始值 × 0.15
            actual = actual * 0.85f + speed * 0.15f;
			//获取本次误差和上次误差
			error1=error0;
			error0=target-actual;
			//获取误差的累计值，积分
			if(ki!=0){
			errorint+=error0;
			}
			else{
			errorint=0;
			}
			//pid主要计算
			out=kp*error0+ki*errorint+kd*(error0-error1);
			//pid限幅
			if(out>99){out=99;}
			if(out<0){out=0;}
			//防止占空比变化太大，引起过流保护
			if(out > duty)
			{
				// 上升超过步长就限制
				if(out - duty > DUTY_STEP_UP)
				{
					out = duty + DUTY_STEP_UP;
				}
			}
			else
			{
				// 下降超过步长就限制
				if(duty - out > DUTY_STEP_DOWN)
				{
					out = duty - DUTY_STEP_DOWN;
				}
			}
			duty=out;
		}
		
		
		
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}

