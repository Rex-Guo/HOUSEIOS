#include "key.h"


/*
 * PA0  		--->KEY0
 * PE2  		--->KEY1
 * PE3  		--->KEY2
 * PE4  		--->KEY3
 * KEY_ON		--->0
 * KEY_OFF		--->1
 */
 
/*
 * ��  �ܣ�KEY��ʼ��
 * ��  ������
 * ����ֵ����
 */ 
void KEY_Init(void)
{
	//����ṹ�����
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1������ GPIOE GPIOA ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOA, ENABLE);

	//2������ GPIO
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;    	//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;    	//����

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;      	//�� 0 ������
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;      	//�� 2 3 4 ������
	GPIO_Init(GPIOE, &GPIO_InitStruct);	
}









