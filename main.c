#include <reg52.h>
#include <intrins.h>
#include <string.h>
#include <lcd.h>
#define uchar unsigned char 
#define uint  unsigned int
#define flag  unsigned char


sfr ISP_CONTR = 0xe7;			//����Ĵ���
sbit lcd_bk_light=P1^3;			//lcd������ƶ˿�

sbit P14 = P1^4;				//����ң��A��
sbit P15 = P1^5;			    //����ң��B��
sbit P16 = P1^6;				//����ң��C��
sbit P17 = P1^7;				//����ң��D��

sbit  infrared_flag=P2^0;		//���⴫����״̬
sbit  lamp=P2^1;				//̨�ƣ��Ӽ̵������͵�ƽ����
sbit  light_button=P2^2;		//����led�ֶ�����
sbit  light=P2^3;				//����led
sbit  light_status=P2^4;		//����״̬
sbit  pilot_light=P2^5;			//�����ֶ�����ָʾ��
sbit  beep=P2^6;				//������

uint  light_num=0;				//��������led��������
uint  infrared_num=0;			//������ʱ����
uint  lcd_bk_num=0;				//lcd��������

flag light_flag=0;			    //����led�Զ����Ŀ���
flag infrared_ex_status;		//������һ״̬
flag infrared_2stimeflag;		//����2s��رձ�־
flag infrared_switch=0;			//�����Ӧ�ܿ���,0/1:���ؿ�/���ع�
flag bluetooth_lamp_flag=0;		//����̨���ֶ����أ�0/1:�رձ��/�򿪱��
flag lamp_close_flag=0;			//���ܺ�����Ʊ��,0/1:�رձ��/�򿪱��
flag beepflag=0;				//������ǣ�0/1:������/������
flag firebeep_flag = 0;			//��������
flag lcd_bk_flag = 0;			//lcd�����ǣ�0/1:�أ���
flag wuxian_flag = 0;			//���߱��

void  delay(uchar i)			//��ʱ����
{
	uchar j=110;
	while(i--)
	while(j--);
}
void beepbeep()					//������
{
	if(beepflag==0)
	{
		beep=0;
		delay(5);
		beep=1;
	}
	
}
void a_com()				   //̨�ƿ���ǿ�ƿ������ܳ��׹ر�ģʽ����
{
	if(lamp==1)
	{
		lamp=0;
		lamp_close_flag=0;		//�رճ��׹ر�ģʽ
		bluetooth_lamp_flag=1;
		infrared_2stimeflag = 0;	
	
	}
	
}
void b_com()				//̨�ƹأ��رպ�Ϊ�ܺ������״̬
{
	if(lamp==0)
	{
		lamp=1;
		bluetooth_lamp_flag=0;
	
	}
	
}
void c_com()				//led������
{
	if(light==1)
	{
		light=0;
		light_flag=1;
	}
	
}

void f_com()						//����Ӱģʽ���ر�lamp�����ܺ������
{
	if(lamp_close_flag==0)			//�򿪱��
	{
		lamp_close_flag=1;
		if(lamp==0)
		{
			infrared_2stimeflag	= 0;
			b_com();					//�ر�̨��
		}
	
	}
}
void g_com()						//�رտ���Ӱģʽ���ص�����״̬
{
	if(lamp_close_flag==1)
	{
		lamp_close_flag=0;
		infrared_2stimeflag = 1;
		beepbeep();
	}
}
void r_com()						//ϵͳ��λ
{
	ISP_CONTR = 0x20;	
}

void v_com()
{
	if(beepflag==0) beepflag=1;
	else beepflag=0;
}
void int0()interrupt 0	        //�ⲿ�ж�0������򣬵͵�ƽ����������������
{
	beep = 0;
	delay(30);
	beep = 1;
	delay(30);
}
void int1()interrupt 2
{
	delay(20);								//�ȴ��˿ڷ�Ӧ
	if(P14==0)							   //�򿪹ر�/̨��
	{
		if(wuxian_flag == 0)
		{
			a_com();
			wuxian_flag = 1;
		}
		
		else
		{
			 b_com();
			 wuxian_flag = 0;
		 }
	}
	
	if(P15==0)						    //������led
	{
		c_com();
	}
	if(P16==0)							//��lcd����
	{
		lcd_bk_flag=1;
	}
	if(P17==0)							//������Ƭ��
	{
		r_com();
	}
	beepbeep();
}
void time0()interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	if(light_flag)				//����led��ʱ
	{
		light_num++;
		if(light_num>20*15)	    //15���ص�
		{
			light=1;
			light_num=0;
			light_flag=0;
		}
	}
	
	if(lcd_bk_flag)
	{
		lcd_bk_num++;
		if(lcd_bk_num>=20*10)	 //10���ص�
		{
			 lcd_bk_flag = 0;
			 lcd_bk_num = 0;
		}
	}
	if(infrared_2stimeflag==1)		//̨�ƹر�ʱ��ʱ����
	{
		infrared_num++;
		if(infrared_num>=20*2)
		{
			lamp=1;					//̨�ƹ�
			infrared_num=0;
			infrared_2stimeflag=0;
			infrared_ex_status=0;
			if(light_status==1)		//���뿪�����Ӧʱ����⻷������
			{
				light_flag=1;
				light=0;
			}
			else light_flag=0;
		}
	}
	
	
}
void sendmes(uchar *p)				  //�����ַ�������
{
	while(*p)
	{
		SBUF=*p;
		while(TI==0);
		TI=0;
		p++;
	}
}
void check()						   //��鵱ǰ״̬����
{
	
	uchar *str1="Lamp:Always On!";
	uchar *str2="Lamp:On! ";
	uchar *str3="Lamp:Off!";
	uchar *str4="Lamp:Always Off!";
	uchar *str5="The infrared:OFF!";
	uchar *str6="The infrared:On!";
	uchar *str7="Beep:Open!";
	uchar *str8="Beep:close!";

	lcd_write_com(0x01);			   
	if(bluetooth_lamp_flag==1)
	{
		sendmes(str1);
		lcd_write_com(0x80);
		lcd_wstring(str1);
	}
	if(bluetooth_lamp_flag==0&&lamp==0)
	{
		sendmes(str2);
		lcd_write_com(0x80);
		lcd_wstring(str2);
	}
	if(lamp==1&&lamp_close_flag==0)
	{
		sendmes(str3);
		lcd_write_com(0x80);
		lcd_wstring(str3);
	}
	if(lamp_close_flag==1)
	{
		sendmes(str4);
		lcd_write_com(0x80);
		lcd_wstring(str4);
	}
	if(beepflag==0)
	{
		sendmes(str7);
		lcd_write_com(0xc0);
		lcd_wstring(str7);
	}
	else 
	{
		sendmes(str8);
		lcd_write_com(0xc0);
		lcd_wstring(str8);
	}

	beepbeep();
	
}


void com_chose(uchar com)
{	
	switch(com)
	{
		case 'x':check();break;					   	//��ѯ״̬
		case 'a':a_com();check();break;		  			   //���ƣ����ܺ������
		case 'b':b_com();check();break;				       //�صƣ��ܺ������
		case 'c':c_com();check();break;					   //������led
		case 'f':f_com();check();break;						//����Ӱģʽ���صƣ����ܺ������
		case 'g':g_com();check();break;						//�رտ���Ӱģʽ��������״̬
		case 'r':r_com();check();break;						//�����λ
		case 'v':v_com();check();break;						//��Ч����
	}
}
void serial_int() interrupt 4		//�����жϷ������
{
	uchar com;
	if(RI)
	{
		com=SBUF;
		com_chose(com);
		RI=0;
	}

}

void Infrared()
{
	if(!infrared_switch)
	{
		if(bluetooth_lamp_flag==1)		//�����ֶ�����
		{
			lamp=0;
		}
		else if(infrared_flag==0)		//�����Ӧ��⵽����
		{
			if(lamp_close_flag==0)		//���κ����־
			lamp=0;						//̨�ƿ�
			infrared_ex_status=1;		//״̬��Ϊ1
		}
		else if(infrared_ex_status==0)
		{
			lamp=1;
		}
		else if(infrared_ex_status==1)
		{
			infrared_2stimeflag=1;
		}
	}
		
	
	
}
void Light()
{
	if(!light_button)		
	{
		
		pilot_light=0;		//ָʾ�ƿ�
		if(light_flag==1) 
		{light_flag=0;}		//����Ƿ����Զ�����ģʽ���Ǿ͹ر��Զ�����ģʽ
		light=0;
		
	}
	else 
	{
	
		pilot_light=1;		//ָʾ�ƹ�
		if(light_flag==0)	//����Ƿ����Զ�����ģʽ����͹رյ�
		light=1;
		
	}
	

	if(lcd_bk_flag)		 	//����lcd����
	{
		lcd_bk_light = 0;	
	}
	else lcd_bk_light = 1;
}


void init()
{
	TMOD=0x01;
	TMOD|=0x20;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;

	TH1=0xfd;
	TL1=0xfd;
	
	EA=1;
	ET0=1;
	
	EX0=1;
	IT0=0;

	EX1=1;
	IT1=1;

	TR0=1;
	
	SCON=0x50;	  
	PCON=0x00;
	TR1=1;
	ES=1;

	lcd_init();
	lcd_write_com(0x82);
	lcd_wstring("Welcome To");
	lcd_write_com(0xc0);
	lcd_wstring("Intelligent Home");

}

void main()
{
	
	
	init();
	
	
	while(1)
	{
		Light();
		
		Infrared();
		
	
	}
}