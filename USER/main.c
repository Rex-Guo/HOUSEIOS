#include "stm32f4xx.h"
#include "systick.h"
#include "usartx.h"
#include "tim.h"
#include "onenetconnet.h"
#include "dht.h"
#include "infrared.h"
#include "rc522.h"
#include "mpu6050.h"
#include "oled.h"
#include "chinese.h"
#include "oledfont.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "at24c02.h"
#include "At24c02_Read.h"

extern int mode;

const char* DEVID_T = DEVID;
const char* APIKEY_T = APIKEY;

int main(void)
{
	/**************************** Ӳ����ʼ�� *******************************/
	//EXTI_DeInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	Systick_Init();
	
	
	usartx_init(USART1,115200);             //��ʼ������1�������������жϣ���Ҫ�����������ݴ�����ʾ����
	usartx_init(USART3,115200);             //��ʼ������3�����������жϣ������ж��������������ȼ� 
	TIM3_Init(30000,84);
	
	ESP8266_init();							//��ʼ��ESP8266ģ�飬����WIFI,����ƽ̨IP
	OneNet_DevLink(DEVID,APIKEY);			//����ƽ̨�豸��
	
	DHT11_Init();
	Infrared_Init();
	RC522_Init();
	OLed_Init();
	Rtc_Init();
	KEY_Init();
	LED_Init();
	AT24C02_Init();
	mode = 1;								//����3���ս����������ģʽ
	printf("OneNet_DevLink Success!\r\n");
	
	MPU6050_Init();	
	
	/*********************** ��AT24C02��ȡ�¶ȡ�ʪ�ȱȽ�ֵ ****************************/
	Modify_TH(1);
	
	printf("HardWare Init Success!\r\n");

	
	OLED_ClearLogin();
	Login(author, role);					//��ӭҳ��author��������role����˵��
	delay_s(2);								//2,��ʱ2s
	
	/****************************** ʵ�ֵĹ��� ************************************/
	while(1)
	{
		TaskProcess();
	}
}
