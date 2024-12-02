#include "lcd.h"
void lcd_delay(uchar i)
{
	uchar x = 100;
	while(i--)
	while(x--);
}
uchar Read_LCD_State()
{
 	uchar state;
	lcdrs=0;lcdrw=1;lcden=1;lcd_delay(1);
	state=P0;
	lcden = 0;lcd_delay(1);
	return state;
}
void lcd_check_busy()
{
 	while((Read_LCD_State()&0x80)==0x80);
	lcd_delay(1);
}
void lcd_write_com(uchar com)
{
	lcd_check_busy();
	lcdrs=0;			//ѡ��д����״̬
	lcdrw=0;
	P0=com;			//����͵�P0��
	lcd_delay(1);			//��ʱʹ�����ȶ�
	lcden=1;			//��ʹ�ܶ�һ��������
	lcd_delay(1);			//��ʱ
	lcden=0;			//ʹ�ܶ���0��ɸ�����
}


void lcd_write_data(uchar _data)
{
	lcd_check_busy();
	lcdrs=1;	
	lcdrw=0;			
	P0=_data;
	lcd_delay(1);
	lcden=1;
	lcd_delay(1);
	lcden=0;
}
void lcd_init()
{
	lcd_write_com(0x38);			//����16*2��ʾ��5*7����8λ���ݽӿ�
	lcd_write_com(0x0c);			//���ÿ���ʾ������ʾ���
	lcd_write_com(0x06);			//дһ���ַ����ַָ���1
	lcd_write_com(0x01);			//��ʾ��0������ָ����0
	lcden=0;	
}
void lcd_wstring(uchar *s)
{
	while(*s)
	{
		lcd_write_data(*s++);
	}
}
void lcd_clear()
{
	lcd_write_com(0x01);
}