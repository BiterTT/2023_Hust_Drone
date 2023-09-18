/* include */
#include "config.h"

/* declare */
static void BLUE_ON(void);
static void BLUE_OFF(void);
static void BLUE_TOGGLE(void);
static void RED_ON(void);
static void RED_OFF(void);
static void RED_TOGGLE(void);

/* LED GPIO config */
void vLedGpioConfig( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE ); 	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE ); 	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE ); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       
	GPIO_Init( GPIOC, &GPIO_InitStructure );  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       
	GPIO_Init( GPIOC, &GPIO_InitStructure );  

	LED1_OFF;	
	LED2_OFF;
}

/* LED״ָ̬ʾʵ�� */
/* ����ģʽ : �������� */
/* ���ش��� : ��Ƴ��� */
/* �Լ� 		: ��������(��) */
/* IMU���� 	: ���ƿ��� */
/* yaw���� 	: ��ƿ��� */
/* RC ����  �������������� */
/* Pluck���ߣ������������ */
/* ��̨���� �����Ƴ��� */
/* ����ģʽ ������˫�� */

/* ����ģʽ : �������� */
void led_normal(void)
{
	static int cnt;
	
	RED_OFF();
	
	if(cnt%20 == 0||cnt%20 == 2)
	{
		BLUE_TOGGLE();
	}
	
	cnt ++;
	if(cnt>100000)
		cnt = 0;
}

/* ���ش��� : ��Ƴ��� */
void led_error(void)
{
	BLUE_OFF();
	RED_ON();
}

/* �Լ� : ��������(��) */
void led_selfCheck(void)
{
	RED_ON();
	BLUE_ON();
}

/* IMU���� : ���ƿ��� */
void led_IMU_Lost(void)
{
	static int cnt;
	
	RED_OFF();
	
	if(cnt%10 == 0)
	{
		BLUE_ON();
	}
	else if(cnt%10 == 1)
	{
		BLUE_OFF();
	}
	
	cnt ++;
	if(cnt>100000)
		cnt = 0;
}

/* yaw�������� : ��ƿ��� */
void led_YAW_Lost(void)
{
	static int cnt;
	
	BLUE_OFF();
	
	if(cnt%10 == 0)
	{
		RED_ON();
	}
	else if(cnt%10 == 1)
	{
		RED_OFF();
	}
	
	cnt ++;
	if(cnt>100000)
		cnt = 0;
}

/* Pitch�������� :  */
void led_Pitch_Lost(void)
{

}

/* RC���� : ������������ */
void led_RC_Lost(void)
{
	static int cnt;
	
	if(cnt%20 == 0)
	{
		BLUE_ON();
	}
	else if(cnt%20 == 2)
	{
		BLUE_OFF();
	}
	if(cnt%20 == 10)
	{
		RED_ON();
	}
	else if(cnt%20 == 12)
	{
		RED_OFF();
	}
	
	cnt ++;
	if(cnt>100000)
		cnt = 0;
}

/* Pluck���� : ����������� */
void led_Pluck_Lost(void)
{
	static int cnt;
	
	if(cnt%10 == 0)
	{
		BLUE_ON();
	}
	else if(cnt%10 == 1)
	{
		BLUE_OFF();
	}
	if(cnt%10 == 5)
	{
		RED_ON();
	}
	else if(cnt%10 == 6)
	{
		RED_OFF();
	}
	
	cnt ++;
	if(cnt>100000)
		cnt = 0;
}

/* ��̨���� : ���Ƴ��� */ 
void led_Gimbal_powerOff(void)
{
	BLUE_ON();
	RED_OFF();
}

/* ����ģʽ : ����˫�� */
void led_AimAssist(void)
{
	static int cnt;
	
	RED_OFF();
	
	if(cnt%200 == 0||cnt%200 == 10||cnt%200 == 20||cnt%200 == 30)
	{
		BLUE_TOGGLE();
	}
	
	cnt ++;
	if(cnt>100000)
		cnt = 0;
}


/* LED���� : for other document use */
void LED_OFF(void)
{
	LED1_OFF;
	LED2_OFF;
}

void LED_RED(void)
{
	LED1_ON;
	LED2_OFF;
}

void LED_BLUE(void)
{
	LED1_OFF;
	LED2_ON;
}

void LED_PURPLE(void)
{
	LED1_ON;
	LED2_ON;
}


/* LED״̬ */
static void BLUE_ON(void)
{
	LED2_ON;
}
static void BLUE_OFF(void)
{
	LED2_OFF;
}
static void BLUE_TOGGLE(void)
{
	LED2_TOGGLE;
}
static void RED_ON(void)
{
	LED1_ON;
}
static void RED_OFF(void)
{
	LED1_OFF;
}
static void RED_TOGGLE(void)
{
	LED1_TOGGLE;
}


/* END */



