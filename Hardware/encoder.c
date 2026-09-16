#include "stm32f10x.h"                  // Device header
static int32_t encoder_total_cnt;
static uint16_t encoder_last_cnt;
//编码器模块初始化，利用tim3的编码器模块
void Encoder_Init(void){
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//初始化gpio对应tim3编码器的Channel1和2
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	TIM_InternalClockConfig(TIM3);
	//时基单元初始化
	TIM_TimeBaseInitTypeDef TIM_Timebase_Initstructure;
	TIM_Timebase_Initstructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_Timebase_Initstructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_Timebase_Initstructure.TIM_Period=65535;
	TIM_Timebase_Initstructure.TIM_Prescaler=0;
	TIM_Timebase_Initstructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_Timebase_Initstructure);
	//初始化tim3输入捕获模块
	TIM_ICInitTypeDef TIM_ICInitstructure;
	TIM_ICInitstructure.TIM_Channel=TIM_Channel_1;
	TIM_ICInitstructure.TIM_ICFilter=0xf;
	TIM_ICInit(TIM3,&TIM_ICInitstructure);
	TIM_ICInitstructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInitstructure.TIM_ICFilter=0xf;
	TIM_ICInit(TIM3,&TIM_ICInitstructure);
	//初始化编码器通道12都用，一周期4个下降沿
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Falling);
	//tim使能
	TIM_Cmd(TIM3,ENABLE);
	
	//初始化让encoder_last_cnt读取现在timcnt数值
	encoder_last_cnt=TIM_GetCounter(TIM3);
	encoder_total_cnt=0;
	
	
}


int32_t Encoder_getcounter(void){
	uint16_t encode_now=TIM_GetCounter(TIM3);//新cnt
	int16_t encoder_delta=encode_now-encoder_last_cnt;//cnt变化量
	encoder_total_cnt+=encoder_delta;//将cnt变化量存到total中
	encoder_last_cnt=encode_now;//对齐两者
	return encoder_total_cnt;//返回total
}

#define duty_min 10       //duty最小值
#define duty_max 99      //duty最大值
#define enc_range 1000   //cnt变化范围
#define enc_deadzone 10  //死区

//对encoder_total_cnt进行限幅，和与duty进行缩放
uint8_t encoder_getduty (int32_t encoder_total){
	int32_t temp;
	//先规定cnt变化范围0~1000
	if(encoder_total>enc_range){
	encoder_total=enc_range;
	}
	if(encoder_total<0){
	encoder_total=0;
	}
    //规定死区变化范围0~10返回0
	if(encoder_total<=enc_deadzone){
	return 0;
	}
    //进行缩放
	temp = duty_min + (encoder_total - enc_deadzone) * (duty_max - duty_min) / (enc_range - enc_deadzone);
	//返回值给duty
	return (uint8_t)temp;
}















