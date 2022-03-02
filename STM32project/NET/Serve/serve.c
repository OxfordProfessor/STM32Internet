#include "serve.h"
//�����豸
#include "esp8266.h"
//Э���ļ�
#include "protocol.h"
//Ӳ������
#include "usart.h"
#include "delay.h"
//C��
#include <string.h>
#include <stdio.h>
#include "cJSON.h"
/*
	onenet�Ʒ�����ƽ̨
*/
#include "led.h"
#include "ds18b20.h"
#define PROID		"Stm32Internet&gvrxJiLWkq4"//�������豸����

#define AUTH_INFO	"5ea42da99af0edcf2f32c8a4db389abbad3596dc"//�������豸����

#define DEVID		"FESA234FBDS24|securemode=3,signmethod=hmacsha1,timestamp=789|"//��Ʒ

extern float Light;
char PUB_BUF1[256];//�ϴ����ݵ�buf

extern unsigned char esp8266_buf[128];
extern u8 alarmFlag;//�Ƿ񱨾��ı�־
extern u8 alarm_is_free;

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���
	
	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_Ping
//
//	�������ܣ�	����
//
//	��ڲ�����	��
//				
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����		
//==========================================================
void OneNet_Ping(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	if(MQTT_PacketPing(&mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���ͱ�������
		UsartPrintf(USART_DEBUG, "PING SUCCESS");
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}
	else
	{
		UsartPrintf(USART_DEBUG, "PING FAIL");
	}

}


//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص����� Ҳ����*cmd
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *json , *json_value;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

				MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
			}
		break;
			
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				// �����ݰ�req_payload����JSON��ʽ����
				json = cJSON_Parse(req_payload);
				if (!json)UsartPrintf(USART_DEBUG,"Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					json_value = cJSON_GetObjectItem(json , "wechat");
//					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->string);
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->valuestring);
					if(strstr(json_value->valuestring,"led0 on") != NULL)    //���ƴ�LED0
					{
						LED0=1;
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update","{\"esp8266\":\"LED_0 ON\"}");  //���·���LED0״̬
//						json_value = cJSON_GetObjectItem(json , "value");
//						if(json_value->valueint)LED0 = 1;//����
//						else LED0 = 0;//�ص�
					}
					else if(strstr(json_value->valuestring,"led0 off") != NULL)
					{
						LED0=0;
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update","{\"esp8266\":\"LED_0 OFF\"}");
//						json_value = cJSON_GetObjectItem(json , "value");
//						if(json_value->valueint)alarmFlag = 1;//�򿪱�����
//						else alarmFlag = 0;//�رձ�����
//						alarm_is_free=0;//��λ�����Ƹ��ֶ����ƾ���һ�������ȼ� �������Զ����ƣ����Ҫ��alarm_is_free ��0
					}
					else if(strstr(json_value->valuestring,"led2 on") != NULL)  
					{
						LED1=1;
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update","{\"esp8266\":\"LED_1 ON\"}");  
					}
					else if(strstr(json_value->valuestring,"led2 off") != NULL)
					{
						LED1=0;
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update","{\"esp8266\":\"LED_1 OFF\"}");
					}
				
					else if(strstr(json_value->valuestring,"led on") != NULL)
					{
						LED0=LED1=1;
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update","{\"esp8266\":\"LED ON\"}");
					}
					else if(strstr(json_value->valuestring,"led off") != NULL)
					{
						LED0=LED1=0;
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update","{\"esp8266\":\"LED OFF\"}");
					}
					
					else if(strstr(json_value->valuestring,"update") != NULL)
					{
						/*
						   ���ݸ�ʽ:
						   sprintf(���������飬"{\"��������1����ʪ�ȣ�\"\:��������(%d).С������(%d)��
												\"��������2�����¶ȣ�\"\:��������.С�����֣�
												\"��������3�����ǿ��\"\:��������(��%.lf)��
											    \"esp8266\":\"UPDATE OK\}",
								   ����1�������ֲɼ����ݣ�����1С�����ֲɼ����ݣ�����2�������ֲɼ����ݣ�����2С�����ֲɼ����ݣ�
								   ����3�ɼ�����
								  )
						*/
						sprintf(PUB_BUF1,"{\"Temp\":%d,\"esp8266\":\"UPDATE OK\"}", (int)DS18B20_Get_Temp());      //�򻺳���PUB_BUF1��д��ʪ�ȡ��¶�����
						OneNet_Publish("/a1o7XYqLD1A/mqttfx/user/update", PUB_BUF1);  //�����������������PUB_BUF1��Ϣ��������ʪ�ȡ��¶�ֵ
					}
					
//					if(json_value->valueint)//json_value > 0��Ϊ����
//					{
//						LED0 = 0;//��LED0 
//					}
//					else
//					{
//						LED0 = 1;//�ر�LED0 
//					}
				}
				cJSON_Delete(json);
			}
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send PublishRel\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send PublishComp\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//����Subscribe��Ϣ��Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//����UnSubscribe��Ϣ��Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
