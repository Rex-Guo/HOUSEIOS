#ifndef _KEY_H_ 
#define _KEY_H_

//ͷ�ļ�
#include "stm32f4xx.h"

//�궨��
#define KEY_ON	0
#define KEY_OFF	1

#define KEY0	GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define KEY1	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)
#define KEY2	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)
#define KEY3	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)

//��������
void KEY_Init(void);


#endif
