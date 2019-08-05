#ifndef _RTC_H
#define _RTC_H 			    

//ͷ�ļ�
#include "stm32f4xx.h"						
							
//��������
/*
 * ��  �ܣ�RTC��ʼ��  
 * ��  ������
 * ����ֵ����
 */
void Rtc_Init(void);

/*
 * ��  �ܣ�����ʱ��
 * ��  ����
 *				hours:	Сʱ
 *			minutes:	����
 *			seconds:	��
 * ����ֵ����
 */
void Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);

/*
 * ��  �ܣ���������
 * ��  ����
 *			 year:	��
 *			month:	��
 *			 date:	��
 * ����ֵ����
 */
void Set_Date(uint8_t year, uint8_t month, uint8_t date);

#endif
