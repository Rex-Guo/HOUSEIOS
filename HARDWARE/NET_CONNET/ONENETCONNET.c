#include "onenetconnet.h"

u8 Wifi_receive[128];
u8 Wifi_cnt=0;
u8 Wifi_lastcnt=0;
u8 connect_out=0;
EdpPacket *send_pkg,*send_pkg1;	//Э���
u8 warit_T = 0;
extern volatile uint8_t IRQ_OFF;
//������ڽ��յ��ַ���
void ESP8266_Clear(void)
{
	memset(Wifi_receive, 0, sizeof(Wifi_receive));
	Wifi_cnt = 0;
}

/**********************************************************
 *	�������ܣ�	�ȴ��������
 *	��ڲ�����	��
 *	���ز�����	Wifi_OK-�������		Wifi_wait-���ճ�ʱδ���
 *	˵����		ѭ�����ü���Ƿ�������
 ***********************************************************/
u8 ESP8266_WaitRecive(void)
{
	if(Wifi_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
	  return Wifi_wait;
	if(Wifi_cnt == Wifi_lastcnt)
	{											//�����һ�ε�ֵ�������ͬ����˵���������
		Wifi_cnt = 0;							//��0���ռ���
		return Wifi_OK;							//���ؽ�����ɱ�־
	}
	Wifi_lastcnt=Wifi_cnt;						//��Ϊ��ͬ
	  return Wifi_wait;							//���ؽ���δ��ɱ�־
}


/**********************************************************
 *	�������ܣ�	��������
 *	��ڲ�����	cmd������
 *	res����Ҫ���ķ���ָ��
 *	���ز�����	0-�ɹ�	1-ʧ��
 **********************************************************/
u8 ESP8266_SendCmd(u8 *cmd, u8 *res)
{
	unsigned char timeOut = 50;
	//ESP8266_Clear();	
	warit_T ++;
	memset(Wifi_receive, 0, sizeof(Wifi_receive));
	Wifi_cnt = 0;
	usartx_puchar(USART3, (u8*)cmd, strlen((const char *)cmd));   //����2����ATָ���
	usartx_puchar(USART1, (u8*)cmd, strlen((const char *)cmd));   //��������ã�ͨ������1�鿴
	//printf("%s\r\n",cmd);
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == Wifi_OK){						             	//����յ�����
	  	//usartx_puchar(USART1, (u8*)Wifi_receive, strlen((const char *)Wifi_receive));
			if(strstr((const char *)Wifi_receive, (const char *)res) != NULL)
			{	//����������ؼ���
				//ESP8266_Clear();									                      //��ջ���			
				memset(Wifi_receive, 0, sizeof(Wifi_receive));
				Wifi_cnt = 0;
				return 0;
			}
			//ESP8266_Clear();
			memset(Wifi_receive, 0, sizeof(Wifi_receive));
			Wifi_cnt = 0;			
		}
		delay_ms(10);
	}
		 
	return 1;
}

/**********************************************************
 *	�������ܣ�	ESP8266��ʼ��
 *	��ڲ�����	��
 *	���ز�����	��
 **********************************************************/
void ESP8266_init()
{
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)AT, (u8*)answer1)){        //ͨ������3����ATָ��
		if(warit_T > 20)return;
		delay_ms(500);
	}
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)CWMODE,(u8*)answer1)){     //ͨ������3����ATָ��
		if(warit_T > 20)return;
		delay_ms(500);
	}
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)RST,(u8*)answer1)){        //ͨ������3����ATָ��
		if(warit_T > 20)return;
		delay_ms(500);
	}
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)CIFSR,(u8*)answer1)){      //ͨ������3����ATָ��
		if(warit_T > 20)return;
		delay_ms(500);
	}
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)CWJAP,(u8*)WIFICONNECT)){  //ͨ������3����ATָ��
		if(warit_T > 20)return;
		delay_ms(500);
	}
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)CIPSTART,(u8*)answer1)){   //ͨ������3����ATָ��
		if(warit_T > 20)return;
		delay_ms(500);
	}
	usartx_send(USART1, (u8*)"WIFI Init OK\r\n");
}

void ESP8266_Conn(uint8_t * conn)
{
	uint8_t wjap[] = "AT+CWJAP=\"WP\",\"";
	strcat((char*)wjap, (char*)conn);
	strcat((char*)wjap,"\"\r\n");
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)wjap,(u8*)WIFICONNECT)){  //ͨ������3����ATָ��
		if(warit_T > 10)break;
		delay_ms(500);
	}
	warit_T = 0;
	while(ESP8266_SendCmd((u8*)CIPSTART,(u8*)answer1)){   //ͨ������3����ATָ��
		if(warit_T > 10)goto END;
		delay_ms(500);
	}
END:
	usartx_send(USART1, (u8*)"WIFI Init OK\r\n");
}


/************************************************************
//	�������ƣ�	A7_EnterTrans
//
//	�������ܣ�	����͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
**************************************************************/
void ESP8266_EnterTrans(void)
{
	ESP8266_SendCmd((u8*)"AT+CIPMODE=1\r\n", (u8*)"OK");//ʹ��͸��
	delay_ms(200); 
	ESP8266_SendCmd((u8*)"AT+CIPSEND\r\n", (u8*)">");  //ͨ������2����ATָ��
}

/***********************************************************
//	�������ƣ�	A7_QuitTrans
//
//	�������ܣ�	�˳�͸��ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��������������+����Ȼ��ر�͸��ģʽ
************************************************************/
void ESP8266_QuitTrans(void)
{
	delay_ms(500); 
	while((USART3->SR & 0X40) == 0);	//�ȴ����Ϳ�
	USART3->DR = '+';
	delay_ms(15); 					//���ڴ�����֡ʱ��(10ms)
	
	while((USART3->SR & 0X40) == 0);	//�ȴ����Ϳ�
	USART3->DR = '+';        
	delay_ms(15); 					//���ڴ�����֡ʱ��(10ms)
	
	while((USART3->SR & 0X40) == 0);	//�ȴ����Ϳ�
	USART3->DR = '+';        
	delay_ms(50); 					//���ڴ�����֡ʱ��(10ms)

	//usartx_puchar(USART1,(u8*)"+++\r\n",5);  //��������ã�ͨ������1�鿴

}


/***********************************************************
//	�������ƣ�	A7_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
************************************************************/
void ESP8266_SendData(unsigned char *data, int len)
{
	ESP8266_Clear();							//��ս��ջ���
	ESP8266_EnterTrans();
	//usartx_puchar(USART1, data, len);			//�����豸������������
	usartx_puchar(USART3, data, len);			//�����豸������������
	//usartx_send(USART1,(u8*)"\r\n");
	ESP8266_QuitTrans();
}


_Bool OneNet_EDPKitCmd(unsigned char *data)
{
	if(data[0] == CONNRESP) //������Ӧ
	{		
		//usartx_send(USART1,(u8*)"DevLink: ");   
		//0		���ӳɹ�
		//1		��֤ʧ�ܣ�Э�����
		//2		��֤ʧ�ܣ��豸ID��Ȩʧ��
		//3		��֤ʧ�ܣ�������ʧ��
		//4		��֤ʧ�ܣ��û�ID��Ȩʧ��
		//5		��֤ʧ�ܣ�δ��Ȩ
		//6		��֤ʧ�ܣ�������δ����
		//7		��֤ʧ�ܣ����豸�ѱ�����
		//8		��֤ʧ�ܣ��ظ��������������
		//9		��֤ʧ�ܣ��ظ��������������
		if(data[3] == 0)
			return 1;
		else
			return 0;
	}
	
	return 0;

}
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenCIPTMODEetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(const char* devid, const char* auth_key)
{
	_Bool status = 0;
	send_pkg = PacketConnect1(devid, auth_key);										//����devid �� apikey��װЭ���     
	ESP8266_Clear();																//��ջ���
	ESP8266_SendData(send_pkg->_data, send_pkg->_write_pos);						//�������ݵ�ƽ̨
	delay_ms(100);	
//	usartx_send(USART1,(u8*)"\r\n{");
//	usartx_puchar(USART1, (u8*)Wifi_receive, strlen((const char *)Wifi_receive));
//	usartx_send(USART1,(u8*)"}\r\n");   
	DeleteBuffer(&send_pkg);										//ɾ��
	ESP8266_Clear();
	return status;
}

//����������Ϊƽ̨��ʶ��ĸ�ʽ
void OneNet_FillBuf(char *buf,char *buf1,u8 number)  
{
	char text[25] = {0};
	memset(buf, 0, sizeof((char*)buf));
	strcat((char *)buf, ",;");	
	strcat((char *)buf, buf1);
	strcat((char *)buf, ",");
	sprintf(text,"%d",number);
	strcat((char *)buf, text);
	strcat((char *)buf, ";");
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(char *buf,char *buf1,u8 number)
{
	OneNet_FillBuf(buf,buf1,number);														//��װ������	
	
	send_pkg = PacketSavedataSimpleString(NULL,(const char *) buf);							//���-Type5
	
	ESP8266_SendData(send_pkg->_data, send_pkg->_write_pos);
	
	DeleteBuffer(&send_pkg);		 		//ɾ��
	
}

//	OneNet_SendData(send_but,"shidu",humidity);
//	OneNet_SendData(send_but,"wendu",temperature);



















