#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "sram.h"
#include "malloc.h"
#include "ILI93xx.h"
#include "led.h"
#include "timer.h"
#include "touch.h"
#include "FreeRTOS.h"
#include "ds18b20.h"
#include "task.h"
#include "limits.h"
#include "gui.h"
#include "GUI.h"
#include "GUIDemo.h"
#include "esp8266.h"
#include "serve.h"
#include "protocol.h"
#include "beep.h"


//�������ȼ�
#define START_TASK_PRIO			1
//�����ջ��С	
#define START_STK_SIZE 			256  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO			2
//�����ջ��С
#define TOUCH_STK_SIZE			128
//������
TaskHandle_t TouchTask_Handler;
//touch����
void touch_task(void *pvParameters);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 			3
//�����ջ��С
#define LED0_STK_SIZE			128
//������
TaskHandle_t Led0Task_Handler;
//led0����
void led0_task(void *pvParameters);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO		4
//�����ջ��С
#define EMWINDEMO_STK_SIZE		512
//������
TaskHandle_t EmwindemoTask_Handler;
//emwindemo_task����
void emwindemo_task(void *pvParameters);

//NET����
//�����������ȼ�
#define NET_TASK_PRIO		5
//�����ջ��С
#define NET_STK_SIZE		512
//������
TaskHandle_t NetTask_Handler;
//emwindemo_task����
void net_task(void *pvParameters);

const char *devSubTopic[] = {"/gvrxJiLWkq4/Stm32Internet/user/get"};	//��������
const char devPubTopic[] = "/gvrxJiLWkq4/Stm32Internet/user/update";	//��������

unsigned short timeCount = 0;	//���ͼ������
unsigned short timeCount_date = 0;	//���ͼ������
unsigned char *dataPtr = NULL;  //esp8266�����������ָ��

extern char PUB_BUF1[256];
int main(void)
{
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//�жϷ�������
	usart1_init(115200);    //����1����������
	usart3_init(115200);    //����3����������
	TFTLCD_Init();			//��ʼ��LCD
	TP_Init();				//��ʼ��������
	LED_Init();   			//LED��ʼ��
	DS18B20_Init();			//�¶ȴ�������ʼ��
	FSMC_SRAM_Init(); 		//SRAM��ʼ��	
	mem_init(SRAMIN); 		//�ڲ�RAM��ʼ��
	mem_init(SRAMEX); 		//�ⲿRAM��ʼ��
	mem_init(SRAMCCM);		//CCM��ʼ��
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	ESP8266_Init();					//��ʼ��ESP8266
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	BEEP = 1;//������ʾ����ɹ�
	delay_ms(250);
	BEEP = 0;	
	OneNet_Subscribe(devSubTopic, 1);
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������                
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  					//STemWin��ʼ��
	WM_MULTIBUF_Enable(1);  		//����STemWin�໺��,RGB�����ܻ��õ�
    taskENTER_CRITICAL();           //�����ٽ���
//	//������������
//    xTaskCreate((TaskFunction_t )touch_task,             
//                (const char*    )"touch_task",           
//                (uint16_t       )TOUCH_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )TOUCH_TASK_PRIO,        
//                (TaskHandle_t*  )&TouchTask_Handler);   	
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,             
                (const char*    )"led0_task",           
                (uint16_t       )LED0_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )LED0_TASK_PRIO,        
                (TaskHandle_t*  )&Led0Task_Handler);  
//    //����EMWIN Demo����
//    xTaskCreate((TaskFunction_t )emwindemo_task,             
//                (const char*    )"emwindemo_task",           
//                (uint16_t       )EMWINDEMO_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )EMWINDEMO_TASK_PRIO,        
//                (TaskHandle_t*  )&EmwindemoTask_Handler);   
    //����Net����
    xTaskCreate((TaskFunction_t )net_task,             
                (const char*    )"net_task",           
                (uint16_t       )NET_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )NET_TASK_PRIO,        
                (TaskHandle_t*  )&NetTask_Handler);   				
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//EMWINDEMO����
void emwindemo_task(void *pvParameters)
{
	WM_MULTIBUF_Enable(1);
	while(1)
	{
		MainTask();
	}
}

//���������������
void touch_task(void *pvParameters)
{
	while(1)
	{
		GUI_TOUCH_Exec();	
		vTaskDelay(5);		//��ʱ5ms
	}
}

//LED0����
void led0_task(void *p_arg)
{
	while(1)
	{
		LED0 = !LED0;
		vTaskDelay(500);		//��ʱ500ms
	}
}
//���������ƺ�������������������ݡ����ջ��������ݿ�������
void net_task(void *pvParameters) 
{
	int date;
	while(1)
	{
		timeCount++;
		timeCount_date++;
		if(timeCount_date % 500 == 0)   //1000ms / 25 = 10 һ��ִ��һ�Σ������¶ȴ��������ݣ�
		{
			//�����¶�����
			date = (int)(DS18B20_Get_Temp()*0.1);
			//�򻺳���PUB_BUF1��д���¶�����
			sprintf(PUB_BUF1,"{\"params\":{\"Temp\":%d},\"method\":\"thing.event.property.post\"}", date);    
			//�����������������PUB_BUF1��Ϣ��������ʪ�ȡ��¶�ֵ
			OneNet_Publish("/sys/gvrxJiLWkq4/Stm32Internet/thing/event/property/post", PUB_BUF1); 
			timeCount_date = 0;
		}
		if(++timeCount >= 5000)									//���ͼ��
		{
			OneNet_Ping();		//���������ܻ������ж�
			timeCount=0; 
		}
	}
}
