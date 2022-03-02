#include "ott2001a.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h"


//��OTT2001Aд��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�����
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��
u8 OTT2001A_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	
 	CT_IIC_Send_Byte(OTT_CMD_WR);   //����д���� 	 
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg>>8);   	//���͸�8λ��ַ
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//������
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//����һ��ֹͣ����	    
	return ret; 
}

//��OTT2001A����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���
void OTT2001A_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(OTT_CMD_WR);   //����д���� 	 
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//���͸�8λ��ַ
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(OTT_CMD_RD);   //���Ͷ�����		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //������	  
	} 
  CT_IIC_Stop();//����һ��ֹͣ����    
}

//��������/�رղ���
//cmd:1,�򿪴�����;0,�رմ�����
void OTT2001A_SensorControl(u8 cmd)
{
	u8 regval=0X00;
	if(cmd)regval=0X80;
	OTT2001A_WR_Reg(OTT_CTRL_REG,&regval,1); 
} 

//��ʼ��������
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
u8 OTT2001A_Init(void)
{
	u8 regval = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE); //����GPIOB��GPIOCʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PB1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PB1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //PC13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC13
	
	CT_IIC_Init();  //��ʼ����������I2C����
	OTT_RST = 0;//��λ
	delay_ms(100);
	OTT_RST = 1; //�ͷŸ�λ
	delay_ms(100);
	OTT2001A_SensorControl(1);  //�򿪴�����
	OTT2001A_RD_Reg(OTT_CTRL_REG,&regval,1);//��ȡ���������мĴ�����ֵ���ж�I2Cͨ���Ƿ�����
	printf("CTP ID:%x\r\n",regval);
		if(regval == 0X80)return 0;
	return 1;
}

const u16 OTT_TPX_TBL[5]={OTT_TP1_REG,OTT_TP2_REG,OTT_TP3_REG,OTT_TP4_REG,OTT_TP5_REG};
//ɨ�败����(���ò�ѯ��ʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 OTT2001A_Scan(u8 mode)
{
	u8 buf[4];
	u8 i=0;
	u8 res=0;
	static u8 t=0; //���Ʋ�ѯ���,�Ӷ�����cpuռ����
	t++;
	if((t%10)==0 || t<10) //����ʱ,ÿ����10��CTP_Scan�����ż��һ��,�Ӷ���ʡCPUʹ����
	{
		OTT2001A_RD_Reg(OTT_GSTID_REG,&mode,1);//��ȡ�������״̬ 
		if(mode&0x1F)  //���д����㰴��ʱ
		{	
			tp_dev.sta = (mode&0x1F)|TP_PRES_DOWN|TP_CATH_PRES;
			for(i=0;i<5;i++) //��ȡ����ֵ
			{
				if(tp_dev.sta&(1<<i)) //������Ч��
				{
					OTT2001A_RD_Reg(OTT_TPX_TBL[i],buf,4);//��ȡ����ֵ
					if(tp_dev.touchtype&0x01) //����
					{
						tp_dev.y[i] = (((u16)buf[2]<<8)+buf[3])*OTT_SCAL_Y;
						tp_dev.x[i] = 800-(((u16)buf[0]<<8)+buf[1])*OTT_SCAL_X;
					}else
					{
						tp_dev.x[i] = (((u16)buf[2]<<8)+buf[3])*OTT_SCAL_Y;
						tp_dev.y[i] = (((u16)buf[0]<<8)+buf[1])*OTT_SCAL_X;
					}
				}
			}
			res =1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)mode=0;//���������ݶ���0,����Դ˴�����
			t = 0;//����һ��,��������������10��,�Ӷ����������
		}
	}
	if((mode&0X1F) == 0) //�޴����㰴��
	{
		if(tp_dev.sta & TP_PRES_DOWN) //֮ǰ�Ǳ����µ�
		{
			tp_dev.sta &= ~(1<<7); //��ǰ����ɿ�
		}else //֮ǰ��û�а���
		{
			tp_dev.x[0] = 0Xffff;
			tp_dev.y[0] = 0Xffff;
			tp_dev.sta &= 0XE0; //�������Ч���
		}
	}
	if(t>240)t=10;//���´�10��ʼ����
	return res;
}

