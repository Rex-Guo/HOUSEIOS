#include "led.h"


/*
 * PF9  		--->LED0
 * PF10  		--->LED1
 * PE13  		--->LED2
 * PE14  		--->LED3
 * LED_ON		--->0
 * LED_OFF		--->1
 */
 
/*
 * ��  �ܣ�LED��ʼ��
 * ��  ������
 * ����ֵ����
 */
void LED_Init(void)
{
	//����ṹ�����
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1������ GPIOE GPIOF ʱ��		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);

	//2������ GPIO
	GPIO_InitStruct.GPIO_Mode  =GPIO_Mode_OUT;    	//���ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   	//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;    	//����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; 	//����ٶ�

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_10;      	//�� 9 10 ������
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_14;      	//�� 13 14 ������
	GPIO_Init(GPIOE, &GPIO_InitStruct);	
	

	//��ʼ����
	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14);
}





