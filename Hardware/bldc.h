#ifndef _BLDC_H
#define _BLDC_H
void BLDC_Init (void);
void BLDC_ENABLE(void);
void BLDC_OFF(void);
void BLDC_forward_60(uint8_t duty);
void BLDC_forward_120(uint8_t duty);
void BLDC_forward_180(uint8_t duty);
void BLDC_forward_240(uint8_t duty);
void BLDC_forward_300(uint8_t duty);
void BLDC_forward_360(uint8_t duty);
void bldc_forward(uint8_t duty);
void BLDC_over_60(uint8_t duty);
void BLDC_over_120(uint8_t duty);
void BLDC_over_180(uint8_t duty);
void BLDC_over_240(uint8_t duty);
void BLDC_over_300(uint8_t duty);
void BLDC_over_360(uint8_t duty);
void bldc_over(uint8_t duty);
#endif

