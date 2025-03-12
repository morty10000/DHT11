#include "stm32f10x.h"

void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. 开启GPIOA的时钟（APB2总线）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. 配置A0引脚为推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;          // 选择Pin 0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 可选：初始化A0引脚为高电平（LED灭）
    //GPIO_SetBits(GPIOA, GPIO_Pin_0);                   // 高电平
    // 或初始化为低电平（LED亮）
    // GPIO_ResetBits(GPIOA, GPIO_Pin_0);              // 低电平
}