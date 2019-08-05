#ifndef __CONNETONENET_H
#define __CONNETONENET_H
#include "stm32f4xx.h"
#include "usartx.h"
#include "systick.h"
#include "time.h"
//协议文件
#include "edpkit.h"
//C库
#include <string.h>
#include <stdio.h>
#define   Wifi_OK    0
#define   Wifi_wait  1
//以下都是AT指令，具体功能看ESP8266的手册
#define   AT          "AT\r\n"	      
#define   CWMODE      "AT+CWMODE=1\r\n"
#define   RST         "AT+RST\r\n"
#define   CIFSR       "AT+CIFSR\r\n"
//#define   CWJAP       "AT+CWJAP=\"OPPOLLH\",\"llhua01234567\"\r\n"
#define   CWJAP       "AT+CWJAP=\"WP\",\"12345678\"\r\n"
#define   CIPSTART    "AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n"	//183.230.40.39 //120.197.89.247
#define   CIPMODE     "AT+CIPMODE=1\r\n"
#define   CIPSEND     "AT+CIPSEND\r\n"
#define   CIPSTATUS   "AT+CIPSTATUS\r\n"
#define   CWQAP		  "AT+CWQAP\r\n"

//以下都是发送AT指令后wifi模块返回的值
#define   WIFICONNECT  "WIFI CONNECT"
#define   CONNECTING  "STATUS:3"
#define   UNCONNECT1  "STATUS:2"
#define   UNCONNECT2  "STATUS:4"
#define   answer1     "OK"
#define   answer2     "no change"
#define   answer3     "+++"
#define   answer4     "ready"
#define   CONNECT     "CONNECT"
#define   CLOSE       "CIPCLOSE"

#define DEVID  	"537812121"        //设备号 可以改
#define APIKEY	"BcFYbDGNFW5hoWqthL7ampG5nPY=" //API钥匙 可以改
 

void ESP8266_Clear(void);

u8 ESP8266_WaitRecive(void);

u8 ESP8266_SendCmd(u8 *cmd, u8*res);

void ESP8266_init(void);

_Bool OneNet_EDPKitCmd(unsigned char *data);


_Bool OneNet_DevLink(const char* devid, const char* auth_key);

void OneNet_FillBuf(char *buf,char *buf1,u8 number);

void OneNet_SendData(char *buf,char *buf1,u8 number);

//系统软复位   
void Sys_Soft_Reset(void);

void ESP8266_SendData(unsigned char *data, int len);

void ESP8266_Conn(uint8_t * conn);

extern u8 Wifi_receive[128];
extern u8 Wifi_cnt;
extern u8 Wifi_lastcnt;
extern u8 connect_out;
#endif	 



