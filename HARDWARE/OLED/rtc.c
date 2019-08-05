#include "rtc.h"

RTC_InitTypeDef RTC_InitStruct_RTC;
RTC_DateTypeDef RTC_DataStructure_RTC;
RTC_TimeTypeDef RTC_TimeStructure_RTC;

/*
 * ��  �ܣ�RTC��ʼ��  
 * ��  ������
 * ����ֵ����
 */
void Rtc_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);/*ʹ�ܺ󱸼Ĵ�������*/
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0xA0A0)
	{

		RCC_LSEConfig(RCC_LSE_ON);/*ʹ��LSE����*/
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)/*�ȴ�����*/
		{
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);/*ѡ��RTCʱ��Դ*/
		RCC_RTCCLKCmd(ENABLE);/*ʹ��RTCʱ��*/
		RTC_WaitForSynchro();/*�ȴ�RTC APB�Ĵ���ͬ��*/
		/*����RTC���ݼĴ����ͷ�Ƶ��*/
		RTC_InitStruct_RTC.RTC_AsynchPrediv	= 0x7f;
		RTC_InitStruct_RTC.RTC_HourFormat		= RTC_HourFormat_24;
		RTC_InitStruct_RTC.RTC_SynchPrediv	= 0xff;
		RTC_Init(&RTC_InitStruct_RTC);
		/*���������պ�����*/
		RTC_DataStructure_RTC.RTC_Year = 0x19;
		RTC_DataStructure_RTC.RTC_Month = 0x7;
		RTC_DataStructure_RTC.RTC_Date = 0x29;
		RTC_DataStructure_RTC.RTC_WeekDay = 0x01;
		RTC_SetDate(RTC_Format_BCD,&RTC_DataStructure_RTC);
		/*����ʱ���� ����ʾ��ʽ*/
		RTC_TimeStructure_RTC.RTC_H12 = RTC_H12_PM;
		RTC_TimeStructure_RTC.RTC_Hours = 0x14;
		RTC_TimeStructure_RTC.RTC_Minutes = 0x52;
		RTC_TimeStructure_RTC.RTC_Seconds = 0x00;
		RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure_RTC);
		/*���ñ��ݼĴ�������ʾ�Ѿ����ù�RTC*/
		RTC_WriteBackupRegister(RTC_BKP_DR0,0xA0A0);
	}
	


}

/*
 * ��  �ܣ�����ʱ��
 * ��  ����
 *		hours:		Сʱ
 *		minutes:	����
 *		seconds:	��
 * ����ֵ����
 */
void Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	hours = ((hours / 10) << 4) | (hours % 10);
	minutes = ((minutes / 10) << 4) | (minutes % 10);
	seconds = ((seconds / 10) << 4) | (seconds % 10);
	RTC_TimeStructure_RTC.RTC_Hours		= hours;
	RTC_TimeStructure_RTC.RTC_Minutes	= minutes;
	RTC_TimeStructure_RTC.RTC_Seconds	= seconds;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure_RTC);
}

/*
 * ��  �ܣ���������
 * ��  ����
 *			 year:	��
 *			month:	��
 *			 date:	��
 * ����ֵ����
 */
void Set_Date(uint8_t year, uint8_t month, uint8_t date)
{
	month = ((month / 10) << 4) | (month % 10);
	date = ((date / 10) << 4) | (date % 10);
	RTC_DataStructure_RTC.RTC_Year	= year;
	RTC_DataStructure_RTC.RTC_Month	= month;
	RTC_DataStructure_RTC.RTC_Date	= date;
	RTC_SetDate(RTC_Format_BCD,&RTC_DataStructure_RTC);
}





