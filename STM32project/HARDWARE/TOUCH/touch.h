#ifndef _TOUCH_H
#define _TOUCH_H
#include "sys.h"
#include "ott2001a.h"
#include "gt9147.h"
#include "ft5206.h"


#define TP_PRES_DOWN	0X80  //����������
#define TP_CATH_PRES	0X40  //�а���������

typedef struct
{
	u8 (*init)(void);  //��ʼ��������
	u8 (*scan)(u8);		 //ɨ�败����0,��Ļɨ��;1,��������
	void (*adjust)(void); //������У׼
	int x[OTT_MAX_TOUCH];  //��ǰ����
	int y[OTT_MAX_TOUCH];  //�����������5������,��������Ϊx[0],y[0]����:�˴�ɨ��ʱ,��������������
												 //x[4],y[4]�洢��һ�ΰ���ʱ������
	u8 sta;  	//�ʵ�״̬
						//b7:����1/�ɿ�0; 
						//b6:0,û�а�������;1,�а�������. 
						//b5:����
						//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////			
	float xfac;
	float yfac;
	float xoff;
	float yoff;
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	u8 touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev; //������������touch.c���涨��

//������оƬ��������
#define PEN		PBin(1)  	//T_PEN
#define DOUT	PBin(2) 	//T_MISO
#define TDIN	PFout(11) 	//T_MOSI
#define TCLK	PBout(0) 	//T_SCK
#define TCS		PCout(13) 	//T_CS

//����������
void TP_Write_Byte(u8 num);						//�����оƬд��һ������
u16 TP_Read_AD(u8 CMD);							//��ȡADת��ֵ
u16 TP_Read_XOY(u8 xy);							//���˲��������ȡ(X/Y)
u8 TP_Read_XY(int *x,int *y);					//˫�����ȡ(X+Y)
u8 TP_Read_XY2(int *x,int *y);					//����ǿ�˲���˫���������ȡ
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color);//��һ������У׼��
void TP_Draw_Big_Point(u16 x,u16 y,u16 color);	//��һ�����
void TP_Save_Adjdata(void);						//����У׼����
u8 TP_Get_Adjdata(void);						//��ȡУ׼����
void TP_Adjust(void);							//������У׼
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac);//��ʾУ׼��Ϣ
//������/������ ���ú���
u8 TP_Scan(u8 tp);								//ɨ��
u8 TP_Init(void);								//��ʼ��
#endif

