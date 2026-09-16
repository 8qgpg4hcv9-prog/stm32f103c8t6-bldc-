#ifndef _ENCODER_H
#define _ENCODER_H
void Encoder_Init(void);
int32_t Encoder_getcounter(void);
uint8_t encoder_getduty (int32_t encoder_total);
#endif
