#ifndef _TIM_H
#define _TIM_H 			    

/********************** ͷ�ļ� ***********************/
#include "stm32f4xx.h"
#include "string.h" 
#include "key.h"
#include "led.h"

/********************** �궨�� ***********************/
#define key1 162
#define key2 98
#define key3 226
#define key4 34
#define key5 2
#define key6 194
#define key7 224
#define key8 168
#define key9 144
#define keyX 104
#define key0 152
#define keyQ 176
#define keyU 24
#define keyL 16
#define keyOK 56
#define keyR 90
#define keyD 74
#define base_num 32

/********************** ����ṹ ***********************/
#define Angle_T	5
#define ACCEL_T	10
#define GYRO_T	10

typedef struct _TASK_COMPONENTS
{
    uint8_t Run;                 	// �������б�ǣ�0-�����У�1����
    uint16_t Timer;              	// ��ʱ��
    uint16_t ItvTime;              	// �������м��ʱ��
    void (*TaskHook)(void);    		// Ҫ���е�������
}TASK_COMPONENTS;       			// ������

/********************** �������� ***********************/
/*
 * ��  �ܣ���ʱ��3��ʼ��
 * ��  ������
 * ����ֵ����
 */
void TIM3_Init(uint16_t Period ,uint16_t Prescaler);
/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : ������
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskProcess(void);

/*
 * ��  �ܣ���ӭҳ
 * ��  ����
 *				name:��������
 *				role:����˵��
 * ����ֵ����
 */
void Login(char name[], char role[]);

/*
 * ��  �ܣ��޸Ļ��ʼ��AT24C02
 * ��  ����
 *				�״ν��룺choose == 1
 *				�޸Ĳ�����choose == 0
 * ����ֵ����
 */
void Modify_TH(uint8_t choose);
	
#endif
