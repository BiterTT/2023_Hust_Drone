/* include */
#include "config.h"
/* define */
int pluckmotor_speed = 2500;	//single shoot mode pluck speed 750
int plunk_inc_speed;//��������ת��  max 14000
short FrictionWheel_speed_stable1; //15000;// frictionwheel speed 
short FrictionWheel_speed_stable2;
uint16_t shoot_period = 1;	//���������ʱ,10ms��λ 10
uint8_t pluck_stop = 0;//����ͣת  ,0Ϊ����ת
int pluck_stop_position;
int shoot_flag = 0;
short quick_shoot_period=50; //����������ʱ(������)

short friction_speed_last[2]={0,0};
float K1=0.995;//��ͨ�˲�����
float A = 0.5;//�޷�����
float new_value = 0; 
float old_value = 0; 
uint32_t shootinitTime;

int back_distance = 40000;
int back_decrease = 100000;

/* declare */
static void shoot_config(void);
extern struct M2006 pluck;
extern struct M2006  frimition[2];
extern RC_Ctl_t RC_Ctl;
extern struct state droneState;
extern short ContralMode;
extern int shoot_mode;

static void Plane_ID_Shoot_Config(void);
/*
 *    			 S2							        S1    
 *  1 --> 		��           1 --> ����ģʽ	
 * 	                             
 *  3 --> �������         3 --> ����ģʽ	
 *	                      								
 *  2 --> ����ģʽ         2 --> ң��ģʽ
 *										
 */

void ShootTask( void * pvParameters )
{
	shoot_config();
	Plane_ID_Shoot_Config();
	
	shootinitTime = xTaskGetTickCount();
	//��̨�ϵ�ڵ�����,����Ϊ�������ϵ�,���û�����ݴ���,Ĭ��ֵΪ0,����Լ�����,����ϵ������ݺ��ٽ�����̨����
	if(xTaskGetTickCount()-shootinitTime<2500)
	{
		pluck.setPoint = pluck.Angle_zeroCheck;
	}
	/* ������ʱ */
	static short powerOff_delay = 0;
	RC_Ctl.rc.ch3=1024;
	for(;;)
	{
		/* �豸�Լ�������״̬������LOST����------���� */
		//pluck_check();
		if(droneState.drone.composeState.Pluck_disconneced ==1)			
		{
			pluck.setPoint = pluck.Angle_zeroCheck;
			shoot_flag = 0;
		}
		if(RC_Ctl.rc.s2 == 3 || RC_Ctl.rc.s2 == 1)	//�ǵ���ģʽ
		{
			//ң��ģʽ*********************************************************************
			if(ContralMode==Rc||ContralMode==Auto)//ң��ģʽ�͸���ģʽ
			{
				static int RC_shoot_delay = 0;//������,����������			
				if((RC_shoot_delay>shoot_period)&&(shoot_mode==single_shoot))//����ģʽ
				{
					RC_shoot_delay = 0;//������,����������
					if(droneState.drone.composeState.Pluck_disconneced !=1)
					{
						pluck.setPoint += pluckmotor_speed;	
						shoot_flag = 1;
					}
				}
				else if(shoot_mode==cont_shoot)//����ģʽ
				{
					if(droneState.drone.composeState.Pluck_disconneced !=1)
					{
						pluck.setPoint += plunk_inc_speed;
						shoot_flag = 1;
					}
				}
				else if(shoot_mode==stop_shoot)//ֹͣ��
				{
					//pluck_stop=0;
					pluck.setPoint += 0;	
					shoot_flag = 0;
				}
				RC_shoot_delay ++;//����ģʽ��,��RC_shoot_delay>shoot_periodʱ,��һ��
				if((shoot_mode==cont_shoot)||(shoot_mode==single_shoot))
				{
						frimition[0].setPoint=-FrictionWheel_speed_stable1;
						frimition[1].setPoint=FrictionWheel_speed_stable2;
				}
				else 
				{
						frimition[0].setPoint=(K1*(friction_speed_last[0])+(1-K1)*0);//��ͨ�˲�����,��Ȼ���綯�ƴ��ز���ϵͳ��ϵ�
						frimition[1].setPoint=(K1*(friction_speed_last[1])+(1-K1)*0);
				}
				/* control pluck motor */
				friction_speed_last[0]=frimition[0].setPoint;
				friction_speed_last[1]=frimition[1].setPoint;
			}
			
			//����ģʽ*********************************************************************
			if(ContralMode==Pc)
			{
				if(RC_Ctl.mouse.press_l==1)
				{
					if(pluck_stop==0)//��������ڿ������
					{
						if(droneState.drone.composeState.Pluck_disconneced !=1)
						{
							pluck.setPoint += plunk_inc_speed;//����
							shoot_flag = 1;
						}
					}	
				}
				else 
				{
					pluck.setPoint += 0;	
					shoot_flag = 0;
				}
				/* friction wheel speed set */
				if((RC_Ctl.mouse.press_l==1)&&ContralMode==Pc)
				{
					frimition[0].setPoint=-FrictionWheel_speed_stable1;
					frimition[1].setPoint=FrictionWheel_speed_stable2;
				}
				else 
				{
					frimition[0].setPoint=(K1*(friction_speed_last[0])+(1-K1)*0);//��ͨ�˲�����,��Ȼ���綯�ƴ��ز���ϵͳ��ϵ�
					frimition[1].setPoint=(K1*(friction_speed_last[1])+(1-K1)*0);
				}
			/* control pluck motor */
				friction_speed_last[0]=frimition[0].setPoint;
				friction_speed_last[1]=frimition[1].setPoint;			
			}
		}
		if(ContralMode == Lost )
		{
			pluck.setPoint = pluck.Angle_zeroCheck;	
			RC_Ctl.rc.ch0=1024;
			RC_Ctl.rc.ch1=1024;
			RC_Ctl.rc.ch2=1024;
			RC_Ctl.rc.ch3=1024;
			frimition[0].setPoint=(K1*(friction_speed_last[0])+(1-K1)*0);//��ͨ�˲�����,��Ȼ���綯�ƴ��ز���ϵͳ��ϵ�
			frimition[1].setPoint=(K1*(friction_speed_last[1])+(1-K1)*0);
			friction_speed_last[0]=frimition[0].setPoint;
			friction_speed_last[1]=frimition[1].setPoint;
		}
		
		taskENTER_CRITICAL();
		M2006_control();//����������
		taskEXIT_CRITICAL();

		/* ���������������� �ָ�����PID���� */
		if(powerOff_delay > 0)
		{
			PID_struct_init(&pluck.speedPID,1,15000,10000,10.0f,0.5f,0.0f);
			PID_struct_init(&pluck.positionPID,1,20000,20000,0.03f,0.0f,0.0f);
			
			powerOff_delay = 0;
		}

	vTaskDelay(10);	
	}
}

/* ������������ǰ�������(��ʱ) */
unsigned char shoot_ProTerminate(void)
{
	static short delay = 0;
	if(delay<20)
	{
		delay ++;
		return 0;
	}
	else 
	{
		delay = 0;
		return 1;
	}
}

/* �������������� */
static void shoot_config(void)
{
	//����
	pluck.zero.CountCycle = 8191;  //pluck���һȦΪ8192
	pluck.mode = doubleLoop;//ģʽΪ˫��PID
	pluck.receiveId = 0x201;//PLUCK���̵�����ID 201
	ZeroCheck_Init_Interface(&pluck.zero,8191,pluck.receiveAngle);
	pluck.setPoint = pluck.Angle_zeroCheck;//�趨��ʼ��λ�ýǶ�ֵ

	PID_struct_init(&pluck.speedPID,1,15000,10000,10.0f,0.5f,0.0f);//��ʼ���ٶ�PID
	PID_struct_init(&pluck.positionPID,1,20000,20000,0.03f,0.0f,0.0f);//��ʼ��λ��PID
	
	frimition[0].mode=speedLoop;//��Ħ���� �ٶ�PID
	frimition[0].receiveId=0x202;//ID
	frimition[0].setPoint=0;//��ʼ��ֵ = 0
	PID_struct_init(&frimition[0].speedPID,1,10000,700,10.0f,1.0f,2.0f);//Ħ����1 �ٶ�PID��ʼ��
	frimition[1].mode=speedLoop;//��Ħ���� �ٶ�PID
	frimition[1].receiveId=0x203;//ID
	frimition[1].setPoint=0;//��ʼ��ֵ = 0
	PID_struct_init(&frimition[1].speedPID,1,10000,700,10.0f,1.0f,2.0f);//Ħ����2 �ٶ�PID��ʼ��
}

void pluck_check(void)//��������
{
	if(pluck_stop==0&&pluck.setPoint-pluck.Angle_zeroCheck>=back_distance)//������5Ȧ��Ϊ��ת
	{			
		pluck_stop_position=pluck.Angle_zeroCheck;
		pluck.setPoint=pluck.Angle_zeroCheck-back_decrease;
		if(shoot_mode==1)//��������
		{
			pluck_stop=1;
		}
	}			
	else	if(pluck_stop==1&&shoot_mode==cont_shoot&&pluck_stop_position-pluck.Angle_zeroCheck<=back_distance)
	{
	
		pluck_stop=0;//�������ָ̻�
	}
}

float imu_filter(float imu_in)
{ 
	new_value = imu_in;  
	if ((new_value - old_value > A) || (old_value - new_value > A))  
	{
		return old_value;  
	}
	return new_value; 
}

static void Plane_ID_Shoot_Config(void)
{
#if Plane_ID == 1
	FrictionWheel_speed_stable1 = -7000;
	FrictionWheel_speed_stable2 = -7000;
	plunk_inc_speed = 10000;//����ת��
#elif Plane_ID == 2 
	FrictionWheel_speed_stable1 = -10000; //15000;// frictionwheel speed 
	FrictionWheel_speed_stable2 =-10000;
	plunk_inc_speed = 15000;//����ת��
#endif

}
/* end */
