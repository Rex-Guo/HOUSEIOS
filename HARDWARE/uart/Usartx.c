#include "usartx.h"
#include "onenetconnet.h"

extern volatile uint8_t IRQ_OFF;
//USART1_TX A9          USART2_TX A2         USART3_TX B10
//USART1_RX A10         USART2_RX A3         USART3_RX B11


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//printf�������ָ��fputc����fputc���������
//����ʹ�ô���1(USART1)���printf��Ϣ
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART1->DR = (u8) ch;      	//дDR,����1����������
	return ch;
}
#endif 


//void usartx_init(USART_TypeDef * USARTX,u32 Pclk,u32 Bound,u8 Able,u8 Groub, u8 Grab, u8 Sub)
//������� 
//USARTX    : USART1 | USART2| USART3   (ѡ�񴮿�)
//Bound     : 1200-                     (���ò�����)

void usartx_init(USART_TypeDef * USARTX,u32 Bound)
{ 
if(USARTX==USART1)
  {
GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//1����������1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//2����������1��Ӧ GPIO ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//3���� PA9 PA10 ����Ϊ����1�����⹦������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_USART1);
	
	//4����ʼ�� PA9 PA10 Ϊ���ù�������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    //��������Ӱ�첻��
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//5�������Լ���ʼ������ģʽ
	USART_InitStruct.USART_BaudRate = Bound;  //������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //�ر�Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;			//��У��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;			//1bitֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;	//8bit����λ
	USART_Init(USART1, &USART_InitStruct);
	
	//6������ ����1 �Ľ����ж�  ---���������ݼĴ�����Ϊ��
	USART_ITConfig(USART1, USART_IT_RXNE,ENABLE);
	
	//7����ʼ�� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;  //����1�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1, ENABLE);	
  }
else if(USARTX==USART2)
  {
   	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//1����������2��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//2����������2��Ӧ GPIO ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//3���� PA2 PA3 ����Ϊ����2�����⹦������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3, GPIO_AF_USART2);
	
	//4����ʼ�� PA2 PA3 Ϊ���ù�������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    //��������Ӱ�첻��
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//5�������Լ���ʼ������ģʽ
	USART_InitStruct.USART_BaudRate = Bound;  //������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //�ر�Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;			//��У��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;			//1bitֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;	//8bit����λ
	USART_Init(USART2, &USART_InitStruct);
	
	//6������ ����2 �Ľ����ж�  ---���������ݼĴ�����Ϊ��
	USART_ITConfig(USART2, USART_IT_RXNE,ENABLE);
	
	//7����ʼ�� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;  //����1�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART2, ENABLE);		
 }
else if(USARTX==USART3)
  {
    GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//1����������3��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//2����������3��Ӧ GPIO ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//3���� PB10 PB11 ����Ϊ����3�����⹦������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11, GPIO_AF_USART3);
	
	//4����ʼ�� PA2 PA3 Ϊ���ù�������
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    //��������Ӱ�첻��
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//5�������Լ���ʼ������ģʽ
	USART_InitStruct.USART_BaudRate = Bound;  //������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //�ر�Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;			//��У��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;			//1bitֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;	//8bit����λ
	USART_Init(USART3, &USART_InitStruct);
	
	//6������ ����3 �Ľ����ж�  ---���������ݼĴ�����Ϊ��
	USART_ITConfig(USART3, USART_IT_RXNE,ENABLE);
	
	//7����ʼ�� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;  //����1�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART3, ENABLE);	
 }
}



//void usartx_send(USART_TypeDef * USARTX,,u8 *string)
//������� 
//USARTX    : USART1 | USART2| USART3   (ѡ�񴮿�)
//buff      :"sadsad"| str              (�ַ����������׵�ַ)
void usartx_send(USART_TypeDef * USARTX,u8 *string)            //�����ַ���
{
  USARTX->SR &=~(1<<6);
  while(*string!='\0')
	  {
	    USARTX->DR = *(string++);
	    while(!(USARTX->SR & (1<<6)));
	  }
}

//void uart_putbuff(USART_TypeDef * USARTX,,u8 *buff, u32 len)
//������� 
//USARTX    : USART1 | USART2| USART3   (ѡ�񴮿�)
//buff      :"sadsad"| str              (�ַ����������׵�ַ)
//len       :strlen(str)                (�ַ��������鳤��)
void usartx_puchar(USART_TypeDef * USARTX,u8 *buff, u32 len)    //�����ַ���
{  
	USARTX->SR &=~(1<<6);
	while(len--)
	  {
 	    USARTX->DR = *(buff++);
	    while(!(USARTX->SR & (1<<6)));
    }
}




//void USART1_IRQHandler(void)
//{
//	u8 receive=0;
//	if(USART1->SR & (1<<5))
//	{
//		receive = USART1->DR;
//		while(USART1->SR & (1<<5));
//	}
//} 

//void USART2_IRQHandler(void)
//{
//    u8 receive=0;
//    if(USART2->SR & (1<<5))
//		  {
//	      receive = USART2->DR;
//	      while(USART2->SR & (1<<5));
//	    }
//}


/*********************************************************************************/
int mode = 0;
char data;
char usart2_rcv_buf[128];
uint8_t usart2_rcv_len = 0;
char rcv_cmd_start = 0;
char usart2_cmd_buf[128];
uint8_t usart2_cmd_len = 0;
#define MAX_CMD_LEN 128 

void USART3_IRQHandler(void)
{
	//if(IRQ_OFF == 0){
		//IRQ_OFF = 1;
		if(mode == 0)
		{
		   if(USART3->SR & (1<<5))
			{	
			  if(Wifi_cnt >= sizeof(Wifi_receive))Wifi_cnt = 0;      //��ֹ���ڱ�ˢ��
				 Wifi_receive[Wifi_cnt] = USART3->DR;                //�Ѵ��ڽ��յ����ݴ�������Wifi_receive
				   if(Wifi_receive[Wifi_cnt]=='E'&&Wifi_cnt>=4)      //�����ж��Ƿ�Ͽ���onenet���Ͽ��˵Ļ����յ�close
					   if(strstr((const char *)Wifi_receive, "CLOSE") != NULL)
							 connect_out=1;
					 Wifi_cnt++;
				 while(USART3->SR & (1<<5));
		   } 
		}else if(mode == 1){
			data = USART3->DR;
			usart2_rcv_buf[usart2_rcv_len++]=data;
			
			if(data=='{') //Լ��ƽ̨�·��Ŀ���������'{'Ϊ��ʼ������}��Ϊ������������������߿����Զ����Լ��Ŀ�ʼ���Ͻ�����
			{
				rcv_cmd_start=1;
			}
			if(rcv_cmd_start==1)
			{
				usart2_cmd_buf[usart2_cmd_len++]=data;
				if((data=='}')||(usart2_cmd_len>=MAX_CMD_LEN-1))
				{
					rcv_cmd_start=0;
					if((NULL != strstr((const char *)usart2_cmd_buf, "LED01")))  //Լ��ƽ̨��������"LED1"Ϊ�򿪵�
					{
						LED0(LED_ON);
						//printf("recv cmd LED1 ok\r\n");
					}else if((NULL != strstr((const char *)usart2_cmd_buf, "LED00")))  //Լ��ƽ̨��������"LED0"Ϊ�رյ�
					{
						LED0(LED_OFF);  
						//printf("recv cmd LED0 ok\r\n");
					}
					
					if((NULL != strstr((const char *)usart2_cmd_buf, "LED11")))  //Լ��ƽ̨��������"LED1"Ϊ�򿪵�
					{
						LED1(LED_ON);
						//printf("recv cmd LED1 ok\r\n");
					}else if((NULL != strstr((const char *)usart2_cmd_buf, "LED10")))  //Լ��ƽ̨��������"LED0"Ϊ�رյ�
					{
						LED1(LED_OFF);  
						//printf("recv cmd LED0 ok\r\n");
					}
					
					if((NULL != strstr((const char *)usart2_cmd_buf, "LED21")))  //Լ��ƽ̨��������"LED1"Ϊ�򿪵�
					{
						LED2(LED_ON);
						//printf("recv cmd LED1 ok\r\n");
					}else if((NULL != strstr((const char *)usart2_cmd_buf, "LED20")))  //Լ��ƽ̨��������"LED0"Ϊ�رյ�
					{
						LED2(LED_OFF);  
						//printf("recv cmd LED0 ok\r\n");
					}
					
					if((NULL != strstr((const char *)usart2_cmd_buf, "LED31")))  //Լ��ƽ̨��������"LED1"Ϊ�򿪵�
					{
						LED3(LED_ON);
						//printf("recv cmd LED1 ok\r\n");
					}else if((NULL != strstr((const char *)usart2_cmd_buf, "LED30")))  //Լ��ƽ̨��������"LED0"Ϊ�رյ�
					{
						LED3(LED_OFF);  
						//printf("recv cmd LED0 ok\r\n");
					}
					memset(usart2_cmd_buf,0,sizeof(usart2_cmd_buf));
					usart2_cmd_len=0;
				}
			} 
		}
		//IRQ_OFF = 0;
	//}
}

