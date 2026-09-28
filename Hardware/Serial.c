#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

// ========== 新增：发送环形缓冲区 ==========
#define TX_BUF_SIZE  128      // 缓冲区大小，调试用128字节足够
static uint8_t  tx_buf[TX_BUF_SIZE];
static uint16_t tx_write = 0; // 写指针：主循环写入
static uint16_t tx_read  = 0; // 读指针：中断读取

volatile uint8_t Serial_RxData;  // 跨中断共享必须加volatile
volatile uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	// 初始化时不开启发送空中断，第一次发数据时自动开启
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  // 优先级调低，低于TIM2和换相
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

// ========== 核心修改：非阻塞发送字节 ==========
void Serial_SendByte(uint8_t Byte)
{
    // 1. 写入环形缓冲区
    tx_buf[tx_write] = Byte;
    tx_write = (tx_write + 1) % TX_BUF_SIZE;
    
    // 2. 开启发送空中断，触发后中断自动取数据发送
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint16_t i;  // 修正：改为16位，避免长字符串溢出
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

// ========== 核心修改：中断增加发送处理 ==========
void USART2_IRQHandler(void)
{
	// 接收中断（原有逻辑完全不变）
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART2);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
	
	// 新增：发送空中断处理
	if (USART_GetITStatus(USART2, USART_IT_TXE) == SET)
	{
        if (tx_read != tx_write)  // 缓冲区非空，发送下一个字节
        {
            USART_SendData(USART2, tx_buf[tx_read]);
            tx_read = (tx_read + 1) % TX_BUF_SIZE;
        }
        else  // 缓冲区空，关闭发送中断，避免空转耗CPU
        {
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        }
        USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
}
