#include "stm32f10x.h"
#include "key.h"

/* 按键状态结构体 */
typedef struct {
    uint8_t  last_sample;  // 上一次采样值（用于消抖比较）
    uint8_t  last_stable;  // 确认后的稳定状态
    uint8_t  debounce_cnt; // 消抖计数器
    uint8_t  trigger;      // 触发标志
} key_obj_t;

/* 三个按键实例 */
static key_obj_t key1_obj = {1, 1, 0, 0}; // PB1  → 停止
static key_obj_t key2_obj = {1, 1, 0, 0}; // PB10 → 正转
static key_obj_t key3_obj = {1, 1, 0, 0}; // PB11 → 反转

#define DEBOUNCE_THRESHOLD   200

void key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t key_getnum(void)
{
    uint8_t ret = 0;
    uint8_t k1_now = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
    uint8_t k2_now = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
    uint8_t k3_now = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);

    /* ========== 按键1 消抖 + 边沿检测 ========== */
    if (k1_now != key1_obj.last_sample) {
        // 和上一次采样不一样 → 电平在跳变，重置消抖
        key1_obj.debounce_cnt = DEBOUNCE_THRESHOLD;
        key1_obj.last_sample = k1_now;
    } else {
        // 和上一次采样一样 → 计数递减
        if (key1_obj.debounce_cnt > 0) {
            key1_obj.debounce_cnt--;
            if (key1_obj.debounce_cnt == 0) {
                // 连续稳定，确认新状态
                if (k1_now == 0 && key1_obj.last_stable == 1) {
                    key1_obj.trigger = 1; // 只响应下降沿
                }
                key1_obj.last_stable = k1_now;
            }
        }
    }

    /* ========== 按键2 消抖 + 边沿检测 ========== */
    if (k2_now != key2_obj.last_sample) {
        key2_obj.debounce_cnt = DEBOUNCE_THRESHOLD;
        key2_obj.last_sample = k2_now;
    } else {
        if (key2_obj.debounce_cnt > 0) {
            key2_obj.debounce_cnt--;
            if (key2_obj.debounce_cnt == 0) {
                if (k2_now == 0 && key2_obj.last_stable == 1) {
                    key2_obj.trigger = 1;
                }
                key2_obj.last_stable = k2_now;
            }
        }
    }

    /* ========== 按键3 消抖 + 边沿检测 ========== */
    if (k3_now != key3_obj.last_sample) {
        key3_obj.debounce_cnt = DEBOUNCE_THRESHOLD;
        key3_obj.last_sample = k3_now;
    } else {
        if (key3_obj.debounce_cnt > 0) {
            key3_obj.debounce_cnt--;
            if (key3_obj.debounce_cnt == 0) {
                if (k3_now == 0 && key3_obj.last_stable == 1) {
                    key3_obj.trigger = 1;
                }
                key3_obj.last_stable = k3_now;
            }
        }
    }

    /* ========== 输出触发事件 ========== */
    if (key1_obj.trigger) {
        ret = 1;
        key1_obj.trigger = 0;
    }
    if (key2_obj.trigger) {
        ret = 2;
        key2_obj.trigger = 0;
    }
    if (key3_obj.trigger) {
        ret = 3;
        key3_obj.trigger = 0;
    }

    return ret;
}                                    

