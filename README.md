# stm32f103c8t6-bldc-
基于 STM32F103C8T6 + ATK-PB6010B 驱动板的 BLDC 无刷电机有感六步换相驱动器，实现霍尔换相、PWM 调速、正反转、能耗制动，支持按键控制与编码器调速旋钮。

# STM32-BLDC-Six-Step-Commutation
基于 STM32F103C8T6 + ATK-PB6010B 的 BLDC 无刷电机有感六步换相驱动器。

实现霍尔换相、PWM 调速、正反转、能耗制动，支持按键状态机控制与编码器调速旋钮。

---

功能

- 霍尔传感器有感换相
- TIM1 互补 PWM 输出 + 硬件死区
- PWM 无级调速
- 正转 / 反转 / 停止
- 能耗制动
- 按键状态机控制
- 旋转编码器调速旋钮
- OLED 实时显示（调试用）

硬件平台

| 主控 | STM32F103C8T6 |
| 驱动板 |  ATK-PB6010B |
| 电机 | 24V 带霍尔 BLDC |
| 电源 | 24V 直流可调电源 |
| 编码器 | 增量式旋转编码器 |
| 显示 | OLED（调试用） |


代码结构


Src/
main.c          主程序，状态机
bldc.c/h        换相逻辑（正转/反转/停止）
hall.c/h        霍尔传感器读取
pwm.c/h         TIM1 互补 PWM 初始化
encoder.c/h     编码器调速旋钮
key.c/h         按键消抖与事件检测
OLED.c/h        OLED 显示（调试用）
