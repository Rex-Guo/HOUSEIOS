#include "tim.h"


//�궨��
#define TASK_NUM   (8)                  //  ���ﶨ���������Ϊ3����ʾ�����������ʹ�ô˶�ʱ����ʱ��

/************************* ȫ�ֵ��� *********************************/
extern int printf(const char *, ...);
extern unsigned int strlen(const char*);
extern char* strcat(char*, const char*);
extern void delay_s(u32);
extern void delay_ms(u32);
volatile uint8_t IRQ_OFF = 0;
uint16_t T_open = 0;
uint8_t UP_D = 0;

/************************* AT24C02���� *********************************/
extern int AT24C02_PageReadData(uint8_t addr ,uint8_t pBuf[] ,uint8_t length);
extern int AT24C02_PageWriteData(uint8_t addr ,uint8_t pBuf[] ,uint8_t length);
//extern void* memset(void *, int, size_t);

/************************* DHT11��RC522���� *********************************/
volatile char Tem[5] = {0};
char humidity		= 0;
char temperature	= 0;
extern int Read_DHT11(char *c);
extern void RC522_Handel(unsigned char sector ,unsigned char block ,unsigned char writeBuf[]);
extern uint8_t RC522_state;
volatile int Temperature_Standard	= 30;	//�¶ȱȽ�ֵ
volatile int Humidity_Standard		= 31;	//ʪ�ȱȽ�ֵ

extern void OLED_ClearXY(uint8_t x, uint8_t y, uint8_t M);
extern void Show_Num(uint8_t x ,uint8_t y ,char ch[] ,uint8_t num);
extern uint8_t IC_OK;
uint8_t IC_oldpass[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t IC_newpass[6] = {'0','0','0','0','0','0'}; 

/************************* NEC������� *********************************/
extern volatile int Con_Data;
extern void GetNEC(void);

/************************* ESP8266���� *********************************/
extern void OneNet_SendData(char *buf,char *buf1,u8 number);
extern const char* DEVID_T;
extern const char* APIKEY_T;

extern _Bool OneNet_DevLink(const char* devid, const char* auth_key);
extern void ESP8266_Conn(uint8_t * conn);
extern u8 ESP8266_SendCmd(u8 *cmd, u8 *res);

uint8_t Num_T = 0;
uint8_t Num_Y = 0;

uint8_t password[12] = "12345678";
uint8_t wjap[] = "AT+CWJAP=\"WP\",\"";

/************************* MPU6050���� *********************************/
typedef struct Angle
{
    double X_Angle;
    double Y_Angle;
    double Z_Angle;
    
} MPU6050_Angle;
double	T_X = 0,T_Y = 0,T_Z = 0;
char	send_but[100];
extern	void MPU6050_Get_Angle(MPU6050_Angle *data);
extern void MPU6050_Display(void);
extern double MPU6050_Get(int * data_MP);
int ACCEL_GYRO[8] = {0};
uint8_t ACCEL_GYRO_T = 0;

/************************* OLED���� *********************************/
extern	char				temp[];		//�ⲿ��������ģ
extern	char				hum[];
extern	char				state[];
extern	char				config[];
extern	char				Light_ON[];
extern	char				Light_OFF[];
extern	const unsigned char	F8X16[];

extern void OLED_ClearLogin(void);

extern void Show_Zh32(uint8_t x ,uint8_t y ,char ch[] ,uint8_t num);
extern void Show_Zh(uint8_t x ,uint8_t y ,char ch[] ,uint8_t num);
uint8_t light = 0;
uint8_t light_old = 10;
uint8_t led = 0xff;

/************************* RTC���� *********************************/
extern RTC_DateTypeDef RTC_DataStructure_RTC;
extern RTC_TimeTypeDef RTC_TimeStructure_RTC;

/*********************** KEY��LED���� ********************************/
uint8_t key = 0xff;

/************************** �������� ****************************/
void Page0(void);		    //����0
void Page1(void);		    //����1
void Page2(void);		    //����2
void Page3(void);		    //����3
void Page4(void);		    //����4
void Display_Time(void);    //��ʾʱ��
void Get_Time(void);	    //��ȡʱ��
void Set_Config(void);

void Task1(void);		    //����1
void Task2(void);		    //����2
void Task3(void);		    //����3
void Task4(void);		    //����4
void Task5(void);		    //����5
void Task6(void);		    //����6
void Task7(void);		    //����7
void Task8(void);		    //����8

static TASK_COMPONENTS Task[TASK_NUM] = {
	{0 , 7	 ,30	, Task1},  	//20 == 1�룬��ȡ�¶�
	{0 , 5	 ,5		, Task2},	//��ȡ����
	{0 , 8	 ,8		, Task3},	//ˢ��
	{0 , 20	 ,20	, Task4},	//OLED��ʾ

	{0 , 35	 ,60	, Task5},	//��ȡ�ǶȲ��ϴ�
	{0 , 900,1000	, Task6},	//����ƽ̨�豸��
	{0 , 30	 ,40	, Task7},	//,�ϴ��ƿ�
	{0 , 2	 ,60	, Task8},	//��ȡʱ�䡢����������ȡ���ٶȲ��ϴ���
	
};

/*
 * ��  �ܣ���ʱ��3��ʼ��   ÿ�� 1us ����һ�� ���ܹ����� 50000 ��
 * ��  ������
 * ����ֵ����
 */
void TIM3_Init(uint16_t Period ,uint16_t Prescaler)
{
	NVIC_InitTypeDef		NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	
	//������ʱ��ʱ�ӣ����ڲ�CK_INT=84M   
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);	
    //�Զ���װ�ؼĴ�����ֵ(����ֵ)  
	TIM_TimeBaseStructure.TIM_Period=(Period-1);   
	//ʱ��Ԥ��Ƶ����������������ʱ��CK_CNT=CK_INT/(83+1)=1M  
	TIM_TimeBaseStructure.TIM_Prescaler= (Prescaler-1);	
	
	//ʱ�ӷ�Ƶ���ӣ�������ʱ��û�У�����   
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;	
	//�������ļ���ģʽ������������ֻ�����ϼ�����û�м���ģʽ������   
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	
	//�ظ���������ֵ��������ʱ��û�У����ù�		
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    //��ʼ����ʱ��    
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		
	
	//����������жϱ�־λ    
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);	  	
	//�����������ж�    
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);

	//ʹ�ܼ�����    
	TIM_Cmd(TIM3, ENABLE);																		
}

/************************* 50ms �ж�һ�� **************************/
void TIM3_IRQHandler(void)
{
	uint8_t i;
	
	if( ( (TIM3->SR & 0x1 ) != 0) && ( (TIM3->DIER & 0x1) != 0) )
	{
		TIM3->SR &= ~(0x1);		//���ж�
		if(T_open >= 0x400){	//�ܷ�����
			
			__DSB();                                                     /* Ensure all outstanding memory accesses included
																			buffered write are completed before reset */
			SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
						   (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
						   SCB_AIRCR_SYSRESETREQ_Msk);                   /* Keep priority group unchanged */
			__DSB();                                                     /* Ensure completion of memory access */
			while(1);
		}

		if(IRQ_OFF == 0){
			for(i=0;i<TASK_NUM;i++)				//�������ʱ�䴦��
			{	
				if(Task[i].Timer)				//ʱ�䲻Ϊ3
				{
					Task[i].Timer--;			//��ȥһ������
					if(Task[i].Timer == 0)		//ʱ�������
					{
						Task[i].Timer = Task[i].ItvTime;  //�ָ�����ֵ
						Task[i].Run = 1;
					}
				}
			}
		}
		
		T_open++;

	}
	
}


/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : ������
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskProcess(void)
{
	uint8_t i;
	for (i=0; i<TASK_NUM; i++)          // �������ʱ�䴦��
	{
		if (Task[i].Run)           		// ʱ�䲻Ϊ0
		{
			Task[i].TaskHook();         // ��������
			Task[i].Run = 0;          	// ��־��0
			T_open = 0;
		}
	}
	
}

/*
 * ��  �ܣ�����1ִ�к���
 * ��  ������
 * ����ֵ����
 */
void Task1(void)
{
	IRQ_OFF = 1;
	
	Read_DHT11((char *)Tem);            //!!!���
	
	IRQ_OFF = 0;
	
//	if(Tem[4] == (Tem[0] + Tem[1] + Tem[2] + Tem[3])){
//		printf("ʪ�ȣ�%d.%d\r\n",Tem[0],Tem[1]);
//		printf("�¶ȣ�%d.%d\r\n",Tem[2],Tem[3]);
//	}else{
//		printf("��ȡʧ�ܣ�");
//	}
	
	if(Tem[0] > 100)
	{
		Tem[0] = 0;
	}
	if( Tem[2] > 100 )
	{
		Tem[2] = 0;
	}
	
	IRQ_OFF = 1;
	//EXTI->IMR &= ~(0x1 << 8);
	if(humidity != Tem[0]){
		humidity = Tem[0];
		printf("Humidity\r\n");
		OneNet_SendData(send_but,"Humidity",humidity);
		
	}
	if(temperature != Tem[2]){
		temperature = Tem[2];
		printf("Temperature\r\n");
		OneNet_SendData(send_but,"Temperature",temperature);
	}
	
	IRQ_OFF = 0;
	
}

/*
 * ��  �ܣ�����2ִ�к���
 * ��  ������
 * ����ֵ����
 */
void Task2(void)
{
	int i = 0;
	
	//printf("KEY\r\n");
	
	if(key != 0){
		if(KEY0 == KEY_ON){
			L0;
			key = 0;
		}

		if(KEY1 == KEY_ON){
			L1;
			key = 0;
		}

		if(KEY2 == KEY_ON){
			L2;
			key = 0;
		}

		if(KEY3 == KEY_ON){
			L3;
			key = 0;
		}
	}else{
		if( (KEY0 == KEY_OFF) && (KEY1 == KEY_OFF) && (KEY2 == KEY_OFF) && (KEY3 == KEY_OFF) ){
			key = 0xff;
		}
	}
	
	if(Con_Data != 0){
		switch(Con_Data){
			case key1:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '1';
				}else if(light_old == 3){
					L0;
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '1';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '1';
						}
					}
				}
				break;
			}
							
			case key2:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '2';
				}else if(light_old == 3){
					L1;
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '2';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '2';
						}
					}
				}
				break;
			}
							
			case key3:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '3';
				}else if(light_old == 3){
					L2;
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '3';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '3';
						}
					}
				}
				break;
			}
							
			case key4:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '4';
				}else if(light_old == 3){
					L3;
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '4';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '4';
						}
					}
				}
				break;
			}
							
			case key5:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '5';
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '5';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '5';
						}
					}
				}
				break;
			}
							
			case key6:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '6';
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '6';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '6';
						}
					}
				}
				break;
			}
							
			case key7:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '7';
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '7';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '7';
						}
					}
				}
				break;
			}
							
			case key8:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '8';
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '8';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '8';
						}
					}
				}
				break;
			}
							
			case key9:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '9';
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '9';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '9';
						}
					}
				}
				break;
			}
							
			case key0:
			{
				if(light_old == 2){
					password[strlen((char*)password)] = '0';
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 0; i < 6 && IC_oldpass[i] != base_num;i++);
						if(i != 6){
							IC_oldpass[i] = '0';
						}
					}else{
						for(i = 0; i < 6 && IC_newpass[i] != base_num;i++);
						if(i != 6){
							IC_newpass[i] = '0';
						}
					}
				}
				break;
			}
							
			case keyX:
			{
				break;
			}
							
			case keyQ:
			{
				if(light_old == 2){
					password[strlen((char*)password) - 1] = 0;
				}else if(light_old == 4){
					if(UP_D == 0){
						for(i = 5; i > 0 && IC_oldpass[i] == base_num;i--);
						IC_oldpass[i] = base_num;
					}else{
						for(i = 5; i > 0 && IC_newpass[i] == base_num;i--);
						IC_newpass[i] = base_num;
					}
				}
				break;
			}
							
			case keyU:
			{
				if(light_old == 4){
					UP_D = (UP_D + 1) % 2;
				}else if(light_old == 5){
					UP_D = (UP_D + 1) % 2;
				}
				break;
			}
							
			case keyL:
			{
				if(light >= 0 && light < 5){
					light = (light - 1) % 5;
				}else if(light == 5){
					if(UP_D == 0){
						Temperature_Standard--;
					}else{
						Humidity_Standard--;
					}
				}
				break;
			}
							
			case keyOK:
			{
				if(light_old == 2){
					IRQ_OFF = 1;
				
					EXTI->IMR &= ~(0x1 << 8);
					strcat((char*)wjap, (char*)password);
					strcat((char*)wjap,"\"\r\n");
					//ESP8266_Conn(password);
					while(ESP8266_SendCmd((u8*)wjap,(u8*)"WIFI CONNECT")){  //ͨ������3����ATָ��
						if(i > 10)break;
						i++;
						delay_ms(500);
					}
					i = 0;
					while(ESP8266_SendCmd((u8*)"AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n",(u8*)"OK")){   //ͨ������3����ATָ��
						if(i > 10)break;
						i++;
						delay_ms(500);
					}
					OneNet_DevLink(DEVID_T,APIKEY_T);			//����ƽ̨�豸��
					IRQ_OFF = 0;
				}else if(light_old == 1){
					light = 5;
				}else if(light_old == 5){
					
					IRQ_OFF = 1;
					
					OneNet_SendData(send_but,"Humidity_Standard",Humidity_Standard);

					Modify_TH(0);
					
					OneNet_SendData(send_but,"Temperature_Standard",Temperature_Standard);
					
					IRQ_OFF = 0;
					
					light = 1;
				}
				break;
			}
							
			case keyR:
			{
				if( (light >= 0) && (light < 5) ){
					light = (light + 1) % 5;
				}else if(light == 5){
					if(UP_D == 0){
						Temperature_Standard++;
					}else{
						Humidity_Standard++;
					}
				}
				break;
			}

			case keyD:
			{
				if(light_old == 4){
					UP_D = (UP_D - 1) % 2;
				}else if(light_old == 5){
					UP_D = (UP_D - 1) % 2;
				}
				
				break;
			}
		}
		
		Con_Data = 0;
	}else{
		EXTI->IMR |= (0x1 << 8);
	}
}


/*
 * ��  �ܣ�����3ִ�к���
 * ��  ������
 * ����ֵ����
 */
void Task3(void)
{
	if(IC_OK == 1){
		LED0(0);
		IC_OK = 0;
	}else if(IC_OK == 2){
		LED1(0);
		IC_OK = 0;
	}
	
	if(RC522_state < 0xff){
		IRQ_OFF = 1;
		RC522_Handel(1,3,(u8*)"12345");
		IRQ_OFF = 0;
	}
	
	if(IC_OK == 1){
		LED0(0);
		IC_OK = 0;
	}else if(IC_OK == 2){
		LED1(0);
		IC_OK = 0;
	}
}

/*
 * ��  �ܣ�����4ִ�к���
 * ��  ������
 * ����ֵ����
 * ���ʧ����OLED��������в��ɽ��ܺ����жϣ���������ܷ�
 */
void Task4(void)
{
	IRQ_OFF = 1;
	if(light == 0){
		if(light_old != light){
			
			light_old = light;
			OLED_ClearLogin();
			
		}
		
		Page0();
		
	}else if(light == 1){
		if(light_old != light){
			
			light_old = light;
			OLED_ClearLogin();
			
		}
		
		Page1();
		
	}else if(light == 2){
		if(light_old != light){
			
			light_old = light;
			OLED_ClearLogin();
			
		}
		
		Page2();
		
	}else if(light == 3){
		if(light_old != light){
			
			light_old = light;
			OLED_ClearLogin();
			
		}
		
		Page3();
		
	}else if(light == 4){
		if(light_old != light){
			
			light_old = light;
			OLED_ClearLogin();
			
		}
		
		Page4();
	}else if(light == 5){
		if(light_old != light){
			
			light_old = light;
			OLED_ClearLogin();
			
		}
		
		Set_Config();
	}
	
	if(light != 0){
		Display_Time();
	}
	
	IRQ_OFF = 0;
}

/*
 * ��  �ܣ�����5ִ�к���
 * ��  ������
 * ����ֵ����
 */

void Task5(void)
{	
	MPU6050_Angle data;

	IRQ_OFF = 1;
	
	//EXTI->IMR &= ~(0x1 << 8);
	MPU6050_Get_Angle(&data);  // ����������� 
	
	if(Num_T == 0){
		
		if((T_X + Angle_T) <= data.X_Angle || (T_X - Angle_T) >= data.X_Angle)	{
			T_X = data.X_Angle;
			printf("X_Angle\r\n");
			OneNet_SendData(send_but,"X_Angle",T_X);
			
		}
	}else if(Num_T == 1){
		if((T_Y + Angle_T) <= data.Y_Angle || (T_Y - Angle_T) >= data.Y_Angle)	{
			T_Y = data.Y_Angle;
			printf("Y_Angle\r\n");
			OneNet_SendData(send_but,"Y_Angle",T_Y);
		}
	}else if(Num_T == 2){
		if((T_Z + Angle_T) <= data.Z_Angle || (T_Z - Angle_T) >= data.Z_Angle)	{
			T_Z = data.Z_Angle;
			printf("Z_Angle\r\n");
			OneNet_SendData(send_but,"Z_Angle",T_Z);
		}
	}
	Num_T = (Num_T + 1) % 3;
	//MPU6050_Display();
	
	IRQ_OFF = 0;
	
//	printf("X_Angle = %lf�� ", data.X_Angle);
//	printf("Y_Angle = %lf�� ", data.Y_Angle);
//	printf("Z_Angle = %lf�� ", data.Z_Angle);
//	printf("\r\n");
	
}

/*
 * ��  �ܣ�����6ִ�к���
 * ��  ������
 * ����ֵ����
 */
void Task6(void)
{
	IRQ_OFF = 1;
	//ESP8266_init();							//��ʼ��ESP8266ģ�飬����WIFI,����ƽ̨IP
	//EXTI->IMR &= ~(0x1 << 8);
	OneNet_DevLink(DEVID_T,APIKEY_T);			//����ƽ̨�豸��
	IRQ_OFF = 0;
}

/*
 * ��  �ܣ�����7ִ�к���
 * ��  ������
 * ����ֵ����
 *
 * PF9  		--->LED0
 * PF10  		--->LED1
 * PE13  		--->LED2
 * PE14  		--->LED3
 */

void Task7(void)
{
	led = led & 0xF0;
	if( ( (GPIOF->ODR) & (0x1 << 9) ) != 0 ){
		led |= 0x1 << 0;
	}
	
	if( ( (GPIOF->ODR) & (0x1 << 10) ) != 0 ){
		led |=  0x1 << 1;
	}
	
	if( ( (GPIOE->ODR) & (0x1 << 13) ) != 0 ){
		led |= 0x1 << 2;
	}
	
	if( ( (GPIOE->ODR) & (0x1 << 14) ) != 0 ){
		led |= 0x1 << 3;
	}
	
	IRQ_OFF = 1;
	
	//EXTI->IMR &= ~(0x1 << 8);
	
	if( (led & ((0x1) << 0) ) != ( (led >> 4) & (0x1) ) ){
		if( (led & ((0x1) << 0) ) != 0){
			OneNet_SendData(send_but,"LED0",10);
			led |= (0x1) << 4;
		}else{
			OneNet_SendData(send_but,"LED0",100);
			led &= ~( (0x1) << 4);
		}
	}
	
	if( (led & ((0x1) << 1) ) != ( (led >> 4) & 0x2 )){
		if((led & ((0x1) << 1) ) != 0){
			OneNet_SendData(send_but,"LED1",10);
			led |= (0x1) << 5;
		}else{
			OneNet_SendData(send_but,"LED1",100);
			led &= ~( (0x1) << 5);
		}
	}
	
	if( (led & ((0x1) << 2) ) != ( (led >> 4) & 0x4 )){
		if((led & ((0x1) << 2) ) != 0){
			OneNet_SendData(send_but,"LED2",10);
			led |= (0x1) << 6;
		}else{
			OneNet_SendData(send_but,"LED2",100);
			led &= ~( (0x1) << 6);
		}
	}
	
	if( (led & ((0x1) << 3) ) != ( (led >> 4) & 0x8 )){
		if((led & ((0x1) << 3) ) != 0){
			OneNet_SendData(send_but,"LED3",10);
			led |= (0x1) << 7;
		}else{
			OneNet_SendData(send_but,"LED3",100);
			led &= ~( (0x1) << 7);
		}
	}
	IRQ_OFF = 0;
}

/*
 * ��  �ܣ�����8ִ�к���
 * ��  ������
 * ����ֵ����
 */
void Task8(void)
{
#if 1			//����8����ʱ��Ƭ�л�Ƶ�ʣ���ʱȥ��
	
	int data_MP[7] = {0};
	uint8_t i;
	
	IRQ_OFF = 1;
	
	MPU6050_Get(data_MP);
	
	IRQ_OFF = 0;
	
	for(i = 0; i < 3 && ACCEL_GYRO[6] != 1; i++){
		if(data_MP[i] < (ACCEL_GYRO[i] - ACCEL_T) || data_MP[i] > (ACCEL_GYRO[i] + ACCEL_T)){
			ACCEL_GYRO[6] = 1;
		}
	}
	
	for(i = 3; i < 6 && ACCEL_GYRO[7] != 1; i++){
		if(data_MP[i] < (ACCEL_GYRO[i] - GYRO_T) || data_MP[i] > (ACCEL_GYRO[i] + GYRO_T)){
			ACCEL_GYRO[7] = 1;
		}
	}
	
	if(ACCEL_GYRO[6] == 1 && ACCEL_GYRO_T == 0){
		
		IRQ_OFF = 1;
		
		if(data_MP[0] < (ACCEL_GYRO[0] - ACCEL_T) || data_MP[0] > (ACCEL_GYRO[0] + ACCEL_T)){
			ACCEL_GYRO[0] = data_MP[0];
			OneNet_SendData(send_but,"ACCEL_X",ACCEL_GYRO[0]);
		}
		if(data_MP[1] < (ACCEL_GYRO[1] - ACCEL_T) || data_MP[1] > (ACCEL_GYRO[1] + ACCEL_T)){
			ACCEL_GYRO[1] = data_MP[1];
			OneNet_SendData(send_but,"ACCEL_Y",ACCEL_GYRO[1]);
		}
		if(data_MP[2] < (ACCEL_GYRO[2] - ACCEL_T) || data_MP[2] > (ACCEL_GYRO[2] + ACCEL_T)){
			ACCEL_GYRO[2] = data_MP[2];
			OneNet_SendData(send_but,"ACCEL_Z",ACCEL_GYRO[2]);
		}
		
		IRQ_OFF = 0;
		
		ACCEL_GYRO[6] = 0;
	}
	if(ACCEL_GYRO[7] == 1 && ACCEL_GYRO_T == 1){
		
		IRQ_OFF = 1;
		
		if(data_MP[3] < (ACCEL_GYRO[3] - GYRO_T) || data_MP[3] > (ACCEL_GYRO[3] + GYRO_T)){
			ACCEL_GYRO[3] = data_MP[3];
			OneNet_SendData(send_but,"GYRO_X",ACCEL_GYRO[3]);
		}
		if(data_MP[4] < (ACCEL_GYRO[4] - GYRO_T) || data_MP[4] > (ACCEL_GYRO[4] + GYRO_T)){
			ACCEL_GYRO[4] = data_MP[4];
			OneNet_SendData(send_but,"GYRO_Y",ACCEL_GYRO[4]);
		}
		if(data_MP[5] < (ACCEL_GYRO[5] - GYRO_T) || data_MP[5] > (ACCEL_GYRO[5] + GYRO_T)){
			ACCEL_GYRO[5] = data_MP[5];
			OneNet_SendData(send_but,"GYRO_Z",ACCEL_GYRO[5]);
		}
		
		IRQ_OFF = 0;
		
		ACCEL_GYRO[7] = 0;
	}
	
	if(ACCEL_GYRO_T == 0){
		ACCEL_GYRO_T = 1;
	}else{
		ACCEL_GYRO_T = 0;
	}
	
	//printf("ACCEL_X:%d\tACCEL_Y:%d\tACCEL_Z%d\t\r\n",data_MP[0],data_MP[1],data_MP[2]);
	//printf("GYRO_X:%d\tGYRO_Y:%d\tGYRO_Z%d\t\r\n",data_MP[3],data_MP[4],data_MP[5]);
#endif
	
	//Get_Time();				//��ȡʱ��
	//IRQ_OFF = 1;
	
	//EXTI->IMR &= ~(0x1 << 8);
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure_RTC);
	RTC_GetDate(RTC_Format_BIN, &RTC_DataStructure_RTC);
	
	//IRQ_OFF = 0;
	
}

/*
 * ��  �ܣ���ӭҳ
 * ��  ����
 *				name:��������
 *				role:����˵��
 * ����ֵ����
 */
void Login(char name[], char role[])
{
	/******* 1,����1����ʾ���� ���� ***********/
	int i;
//	for(i = 0; i < 3; i++){
//			Show_Zh32( 32*i, 0, name, i);
//	}
//	Show_Zh32( 32*i - 16, 0, name, i);
//	
//	for(i = 0; i < 7; i++){
//			Show_Zh( 16*i, 4, role, i);
//	}
//	
//	for(i = 7; i < 14; i++){
//			Show_Zh( 16*(i - 7), 6, role, i);
//	}
	char welcome[] = "Wellcome HOUSEIOS";
	for(i = 0; i < 8; i++){
		Show_Num(32 + 8*i, 2, (char *)F8X16, welcome[i] - base_num);
	}
	
	for(i = 9; i < 17; i++){
		Show_Num(32 + 8 * (i - 9), 4, (char *)F8X16, welcome[i] - base_num);
	}
}

/*
 * ��  �ܣ�����0
 * ��  ������
 * ����ֵ����
 */
void Page0(void)
{
	uint8_t i;
	uint8_t help[] = "L&R To Choose";
	uint8_t help1[] = "P1:display T&H";
	uint8_t help2[] = "P2:display wifi";
	uint8_t help3[] = "P3:display LED";
	for(i = 0; help[i] != '\0'; i++){
		Show_Num(8*i, 0, (char *)F8X16, help[i] - base_num);
	}
	
	for(i = 0; help1[i] != '\0'; i++){
		Show_Num(8*i, 2, (char *)F8X16, help1[i] - base_num);
	}
	
	for(i = 0; help2[i] != '\0'; i++){
		Show_Num(8*i, 4, (char *)F8X16, help2[i] - base_num);
	}
	
	for(i = 0; help3[i] != '\0'; i++){
		Show_Num(8*i, 6, (char *)F8X16, help3[i] - base_num);
	}
}

/*
 * ��  �ܣ�����1
 * ��  ������
 * ����ֵ����
 */
void Page1(void)
{
	int i;
	int j = 1;
	int Temperature_Obj;
	int Humidity_Obj;
	
	//4,����2���ɼ���ӡ�¶� ʪ�� �ж�״̬
	/******************** ��ʾ�¶� *********************/
	for(i = 0; i < 5 && light_old== 1; i++){
			Show_Zh( 16*i, 0, temp, i);
	}
	
	Temperature_Obj = temperature;
	j = 1;
	while(1){
		if(Temperature_Obj / 10 != 0){
			j *= 10;
			Temperature_Obj /= 10;
		}else{
			break;
		}
	}
	if(light != 1)return;
	
	/************ �ֲ����� ************/
	if(temperature < 10){
		OLED_ClearXY(96, 128, 0);
		OLED_ClearXY(96, 128, 1);
	}else if(temperature < 100){
		OLED_ClearXY(104, 128, 0);
		OLED_ClearXY(104, 128, 1);
	}else{
		OLED_ClearXY(112, 128, 0);
		OLED_ClearXY(112, 128, 1);
	}
	
	i = 0;
	Temperature_Obj = temperature;
	while(Temperature_Obj != 0 || j != 0){
		Show_Num(72 + 8*i, 0, (char *)F8X16, (Temperature_Obj / j + 16));
		i++;
		Temperature_Obj = Temperature_Obj % j;
		j /= 10;
	}
	
	Show_Zh(72 + 8*i, 0, temp, 5);			//��ӡ��C
	
	/********************* ��ʾʪ�� ****************************/
	for(i = 0; i < 5 && light_old== 1; i++){
			Show_Zh( 16*i, 2, hum, i);
	}
	
	Humidity_Obj = humidity;
	j = 1;
	while(1){
		if(Humidity_Obj / 10 != 0){
			j *= 10;
			Humidity_Obj /= 10;
		}else{
			break;
		}
	}
	if(light != 1)return;
	
	/************ �ֲ����� ************/
	if(humidity < 10){
		OLED_ClearXY(104, 128, 2);
		OLED_ClearXY(104, 128, 3);
	}else{
		OLED_ClearXY(112, 128, 2);
		OLED_ClearXY(112, 128, 3);
	}
	
	i = 0;
	Humidity_Obj = humidity;
	while(Humidity_Obj != 0 || j != 0){
		Show_Num(72 + 8*i, 2, (char *)F8X16, (Humidity_Obj / j + 16));
		i++;
		Humidity_Obj %= j;
		j /= 10;
	}
	
	Show_Num(72 + 8*i, 2, (char *)F8X16, 5);			//��ӡ%RH
	i++;
	Show_Num(72 + 8*i, 2, (char *)F8X16, 50);
	i++;
	Show_Num(72 + 8*i, 2, (char *)F8X16, 40);
	if(light != 1)return;
	
	/************************* ��ʾ״̬ ***************************/
	for(i = 0; i < 5 && light_old== 1; i++){
			Show_Zh( 16*i, 4, state, i);
	}
	if(Temperature_Standard > temperature){
		Show_Zh( 16*5, 4, state, 5);
		Show_Zh( 16*6, 4, state, 6);

	}else{
		Show_Zh( 16*5, 4, state, 7);
		Show_Zh( 16*6, 4, state, 8);
	}
	
}

/*
 * ��  �ܣ�����2
 * ��  ������
 * ����ֵ����
 */
void Page2(void)
{
	int i;
	char data[] = "Name: WP",pass[] = "Pass:",Ok[] = "OK to connec";
	for(i = 0; data[i] != '\0'; i++){
		Show_Num(8*i, 0, (char *)F8X16, (data[i] - base_num));
	}
	
	for(i = 0; pass[i] != '\0'; i++){
		Show_Num(8*i, 2, (char *)F8X16, (pass[i] - base_num));
	}
	if(strlen((char*)password) >= 12){
		password[11] = '\0';
	}
	OLED_ClearXY(40 + 8 * strlen((char*)password), 128, 2);
	OLED_ClearXY(40 + 8 * strlen((char*)password), 128, 3);
	for(; password[i - 5] != '\0'; i++){
		Show_Num(8*i, 2, (char *)F8X16, (password[i - 5] - base_num));
	}
	
	for(i = 0; Ok[i] != '\0'; i++){
		Show_Num(8*i, 4, (char *)F8X16, Ok[i] - 32);
	}
	
	Show_Zh(112 ,2 ,config ,8);
}

/*
 * ��  �ܣ�����3
 * ��  ������
 * ����ֵ����
 */
void Page3(void)
{
	if( ( (led >> 4) & (0x1) ) != 0 ){
		Show_Zh32( 0, 0, Light_OFF, 0);
	}else{
		Show_Zh32( 0, 0, Light_ON, 0);
	}
	
	if( ( (led >> 5) & (0x1) ) != 0 ){
		Show_Zh32( 32, 0, Light_OFF, 0);
	}else{
		Show_Zh32( 32, 0, Light_ON, 0);
	}
	
	if( ( (led >> 6) & (0x1) ) != 0 ){
		Show_Zh32( 64, 0, Light_OFF, 0);
	}else{
		Show_Zh32( 64, 0, Light_ON, 0);
	}
	
	if( ( (led >> 7) & (0x1) ) != 0 ){
		Show_Zh32( 96, 0, Light_OFF, 0);
	}else{
		Show_Zh32( 96, 0, Light_ON, 0);
	}
	
	Show_Num(8, 4, (char *)F8X16, 'L' - base_num);
	Show_Num(16, 4, (char *)F8X16, '0' - base_num);
	
	Show_Num(40, 4, (char *)F8X16, 'L' - base_num);
	Show_Num(48, 4, (char *)F8X16, '1' - base_num);
	
	Show_Num(72, 4, (char *)F8X16, 'L' - base_num);
	Show_Num(80, 4, (char *)F8X16, '2' - base_num);
	
	Show_Num(104, 4, (char *)F8X16, 'L' - base_num);
	Show_Num(112, 4, (char *)F8X16, '3' - base_num);
	
}

/*
 * ��  �ܣ�����4
 * ��  ������
 * ����ֵ����
 */
void Page4(void)
{
	//uint8_t IC_oldpass[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	//uint8_t IC_newpass[6] = {0}; 
	uint8_t i;
	uint8_t IC_pass[] = "Modify IC Oldpass: Newpass:";
	uint8_t IC[] = "FFFFFF";
	for(i = 0; i < 9; i++){
		Show_Num(8*i, 0, (char *)F8X16, IC_pass[i] - base_num);
	}
	
	for(i = 10; i < 18; i++){
		Show_Num(8*(i - 10), 2, (char *)F8X16, IC_pass[i] - base_num);
	}
	
	for(i = i - 10; i < 14 ;i++){
		if(IC_oldpass[0] == 0xff){
			Show_Num(8*i, 2, (char *)F8X16, IC[i - 8] - base_num);
		}else{
			Show_Num(8*i, 2, (char *)F8X16, IC_oldpass[i - 8] - base_num);
		}
	}
	
	for(i = 19; i < 27; i++){
		Show_Num(8*(i - 19), 4, (char *)F8X16, IC_pass[i] - base_num);
	}
	
	for(i = i - 19; i < 14 ;i++){
		Show_Num(8*i, 4, (char *)F8X16, IC_newpass[i - 8] - base_num);
	}
	
	if(UP_D == 0){
		OLED_ClearXY(112,128,4);
		OLED_ClearXY(112,128,5);
		Show_Zh(112 ,2 ,config ,8);
	}else{
		OLED_ClearXY(112,128,2);
		OLED_ClearXY(112,128,3);
		Show_Zh(112 ,4 ,config ,8);
	}
	//config
}

/*
 * ��  �ܣ���ȡʱ��
 * ��  ������
 * ����ֵ����
 */
void Get_Time(void)
{
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure_RTC);
	RTC_GetDate(RTC_Format_BIN, &RTC_DataStructure_RTC);
	
//	Display_Time();
	//delay_ms(100);

}

/*
 * ��  �ܣ���ӡʱ��
 * ��  ������
 * ����ֵ����
 */

void Display_Time(void)
{
//	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure_RTC);
//	RTC_GetDate(RTC_Format_BIN, &RTC_DataStructure_RTC);
		/************************* ��ʾʱ�� ***************************/
//	if(light_old == 5){
//		OLED_ClearXY(0, 16, 6);
//		OLED_ClearXY(0, 16, 7);
//		//delay_ms(100);
//	}else if(light_old == 7){
//		OLED_ClearXY(24, 40, 6);
//		OLED_ClearXY(24, 40, 7);
//		//delay_ms(100);
//	}else if(light_old == 9){
//		OLED_ClearXY(48, 64, 6);
//		OLED_ClearXY(48, 64, 7);
//		//delay_ms(100);
//	}else if(light_old == 10){
//		OLED_ClearXY(72, 80, 6);
//		OLED_ClearXY(72, 80, 7);
//		//delay_ms(100);
//	}else if(light_old == 11){
//		OLED_ClearXY(80, 88, 6);
//		OLED_ClearXY(80, 88, 7);
//		//delay_ms(100);
//	}//else if(light_old == 12){
//		OLED_ClearXY(96, 104, 6);
//		OLED_ClearXY(96, 104, 7);
//		//delay_ms(100);
//	}else if(light_old == 13){
//		OLED_ClearXY(104, 112, 6);
//		OLED_ClearXY(104, 112, 7);
//		//delay_ms(100);
//	}
	Show_Num(0, 6, (char *)F8X16, (RTC_DataStructure_RTC.RTC_Month / 10) + 16);
	Show_Num(8, 6, (char *)F8X16, (RTC_DataStructure_RTC.RTC_Month % 10) + 16);
	Show_Num(16, 6, (char *)F8X16, '\\' - 32);
	Show_Num(24, 6, (char *)F8X16, (RTC_DataStructure_RTC.RTC_Date / 10) + 16);
	Show_Num(32, 6, (char *)F8X16, (RTC_DataStructure_RTC.RTC_Date % 10) + 16);
	
	Show_Num(40, 6, (char *)F8X16, 0);
	Show_Num(48, 6, (char *)F8X16, (RTC_TimeStructure_RTC.RTC_Hours / 10) + 16);
	Show_Num(56, 6, (char *)F8X16, (RTC_TimeStructure_RTC.RTC_Hours % 10) + 16);
	Show_Num(64, 6, (char *)F8X16, ':' - 32);
	
	Show_Num(72, 6, (char *)F8X16, (RTC_TimeStructure_RTC.RTC_Minutes / 10) + 16);
	Show_Num(80, 6, (char *)F8X16, (RTC_TimeStructure_RTC.RTC_Minutes % 10) + 16);
//	Show_Num(88, 6, (char *)F8X16, ':' - 32);
//	Show_Num(96, 6, (char *)F8X16, (RTC_TimeStructure_RTC.RTC_Seconds / 10) + 16);
//	Show_Num(104, 6, (char *)F8X16, (RTC_TimeStructure_RTC.RTC_Seconds % 10) + 16);
}

/*
 * ��  �ܣ������¶ȣ�ʪ�ȱȽ�ֵ
 * ��  ������
 * ����ֵ����
 */
void Set_Config()
{
	int i;
	int j = 0;
	int Temperature_Standard_Obj;
	int Humidity_Standard_Obj;
	
	
	//5,����3�������¶ȣ�ʪ�ȱȽ�ֵ
	/************** ��ʾ�¶ȱȽ�ֵ ******************/
	for(i = 0; i < 2; i++){
			Show_Zh( 16*i, 0, config, i);
	}
	
	for(i = 4; i < 8; i++){
			Show_Zh( 16*(i - 2), 0, config, i);
	}
	
	Temperature_Standard_Obj = Temperature_Standard;
	j = 1;
	while(1){
		if(Temperature_Standard_Obj / 10 != 0){
			j *= 10;
			Temperature_Standard_Obj /= 10;
		}else{
			break;
		}
	}
	
	i = 0;
	Temperature_Standard_Obj = Temperature_Standard;
	while(Temperature_Standard_Obj != 0 || j != 0){
		Show_Num(88 + 8*i, 0, (char *)F8X16, (Temperature_Standard_Obj / j + 16));
		i++;
		Temperature_Standard_Obj = Temperature_Standard_Obj % j;
		j /= 10;
	}
	
	//��ʾ��ͷ
	if(UP_D == 0){
			OLED_ClearXY(104, 128, 2);
			OLED_ClearXY(104, 128, 3);
			OLED_ClearXY(112, 128, 6);
			OLED_ClearXY(112, 128, 7);
			Show_Zh(88 + 8*i , 0, config, 8);
	}
	
	/**************** ��ʾʪ�ȱȽ�ֵ ********************/
	for(i = 2; i < 8; i++){
			Show_Zh( 16*(i - 2), 2, config, i);
	}
	
	Humidity_Standard_Obj = Humidity_Standard;
	j = 1;
	while(1){
		if(Humidity_Standard_Obj / 10 != 0){
			j *= 10;
			Humidity_Standard_Obj /= 10;
		}else{
			break;
		}
	}
	
	i = 0;
	Humidity_Standard_Obj = Humidity_Standard;
	while(Humidity_Standard_Obj != 0 || j != 0){
		Show_Num(88 + 8*i, 2, (char *)F8X16, (Humidity_Standard_Obj / j + 16));
		i++;
		Humidity_Standard_Obj = Humidity_Standard_Obj % j;
		j /= 10;
	}
	
		//��ʾ��ͷ
	if(UP_D != 0){
			OLED_ClearXY(104, 128, 0);
			OLED_ClearXY(104, 128, 1);
			OLED_ClearXY(112, 128, 6);
			OLED_ClearXY(112, 128, 7);
			Show_Zh(88 + 8*i , 2, config, 8);
	}
	
}

/*
 * ��  �ܣ��޸Ļ��ʼ��AT24C02
 * ��  ����
 *				�״ν��룺choose == 1
 *				�޸Ĳ�����choose == 0
 * ����ֵ����
 */
void Modify_TH(uint8_t choose)
{
	uint8_t pBuf[10] = {0};
	uint8_t fBuf[20];
	if(choose == 0){
			pBuf[0] = 'F';
			pBuf[1] = Temperature_Standard / 100 + '0';
			pBuf[2] = Temperature_Standard % 100 / 10 + '0';
			pBuf[3] = Temperature_Standard % 10 + '0';
			pBuf[4] = Humidity_Standard / 100 + '0';
			pBuf[5] = Humidity_Standard % 100 / 10 + '0';
			pBuf[6] = Humidity_Standard % 10 + '0';
			AT24C02_PageWriteData(0 ,pBuf,strlen((char *)pBuf));
		
	}else if(choose == 1){
		memset(fBuf,0,sizeof(fBuf));
		AT24C02_PageReadData(0 ,fBuf ,7);
		delay_ms(5);
		if(fBuf[0] == 'F'){
			memset(fBuf,0,sizeof(fBuf));
			AT24C02_PageReadData(0 ,fBuf ,7);
			Temperature_Standard = ((int)fBuf[1] - '0')*100 + ((int)fBuf[2] - '0')*10 + ((int)fBuf[3] - '0');
			Humidity_Standard = ((int)fBuf[4] - '0')*100 + ((int)fBuf[5] - '0')*10 + ((int)fBuf[6] - '0');
		}else{																		
			//�״����д����洢��Ĭ��30��31
			Modify_TH(0);
		}
	}
}
