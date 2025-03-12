#include "stm32f10x.h"

void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. ����GPIOA��ʱ�ӣ�APB2���ߣ�
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. ����A0����Ϊ�������ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;          // ѡ��Pin 0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // �ٶ�50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. ��ѡ����ʼ��A0����Ϊ�ߵ�ƽ��LED��
    //GPIO_SetBits(GPIOA, GPIO_Pin_0);                   // �ߵ�ƽ
    // ���ʼ��Ϊ�͵�ƽ��LED����
    // GPIO_ResetBits(GPIOA, GPIO_Pin_0);              // �͵�ƽ
}