/* include */
#include "config.h"

/* define */
int QuikeShoot=0;
uint8_t LastPress=0;
uint8_t LastF=0;
uint8_t Shoot=0;
float pluckmotor_speed = 400;	//quike shoot mode pluck speed  
short FrictionWheel_speed_stable1 = 1450;//left frictionwheel speed 
short FrictionWheel_speed_stable2 = 1450;//right frictionwheel speed
uint16_t shoot_period = 700;	//rc mode shoot period
int ShootMode=0;
/* declare */
static void shoot_config(void);
extern struct M2006 pluck;
extern RC_Ctl_t RC_Ctl;
extern struct state droneState;
extern int ContralMode;
/*
 *    			 S2							        S1    
 *  1 --> ��ʼ���         1 --> ����ģʽ	
 * 	                             
 *  3 --> ׼�����         3 --> ң��ģʽ	
 *	                      								
 *  2 --> ֹͣ���         2 --> ֹͣ����
 *										
 */
short FrictionWheel_speed1,FrictionWheel_speed2;
void ShootTask( void * pvParameters )
{
	shoot_config();
	
	/* ������ʱ */
	static short powerOff_delay = 0;
	
	for(;;)
	{
		/* �豸�Լ����� */
		if(ContralMode != Lost&&(droneState.drone.droneState == 0))
		{
			/* key mode */
			if(ContralMode == Pc)
			{
				/* �򿪼��� */
//				laser_ON();
				
				//�����������
				{
				  if(LastF==0&&RC_Ctl.key.f==1)
		      {
			   	  if(Shoot==0)
					  	Shoot=1;
				  	if(Shoot==1)
						  Shoot=0;
		      }
				  LastF=RC_Ctl.key.f;
			  }
				
				/* �趨Ħ����ת�� */
				TIM_SetCompare2(TIM8,FrictionWheel_speed_stable1);		
		    TIM_SetCompare3(TIM8,FrictionWheel_speed_stable2);
				/* Ħ����ת�ٽӽ��趨ֵ */
				if(RC_Ctl.mouse.press_l == 1&&Shoot == 1 )
				{
					/* �������ת�� */
					pluck.setPoint += pluckmotor_speed;	
				}
				else if(Shoot==0&&LastPress==0&&RC_Ctl.mouse.press_l==1)
				{
					pluck.setPoint +=26807;
				}

				LastPress=RC_Ctl.mouse.press_l;
			}
			
			/* rc mode */
			else if(ContralMode == Rc)
			{
				/* ң���������ʱ */
				static int RC_shoot_delay = 0;
				
				/* stop shooting */
				if(RC_Ctl.rc.s2 == 2)
				{
//					LASER_OFF;
					/* ֹͣ�����ʱ */
					TIM_SetCompare2(TIM8,1000);		
		      TIM_SetCompare3(TIM8,1000);
				}
				/* ready for shooting */
				else if(RC_Ctl.rc.s2 == 3)
				{
//					LASER_ON;
					 QuikeShoot++;
					/* ����ֹͣ�����ʱ */					
					/* start friction wheels */
					TIM_SetCompare2(TIM8,FrictionWheel_speed_stable1);		
		      TIM_SetCompare3(TIM8,FrictionWheel_speed_stable2);
				}
				/* start shoot */
				else if(RC_Ctl.rc.s2 == 1)
				{
					//���������
					{
					  if((QuikeShoot>=100)&&(QuikeShoot<=1500))
					  { 
						  if(Shoot==0)
						    Shoot=1;
					    else if(Shoot==1)
						    Shoot=0;
					  }
						QuikeShoot=0;
				  }
					if((RC_shoot_delay>shoot_period)&&Shoot==0)
					{
						pluck.setPoint += 26807;	
						RC_shoot_delay = 0;
					}
					if(Shoot==1)
					{
						pluck.setPoint += pluckmotor_speed;	
					}
					/* set two friction wheels to different speed */
					TIM_SetCompare2(TIM8,FrictionWheel_speed_stable1);		
		      TIM_SetCompare3(TIM8,FrictionWheel_speed_stable2);
					RC_shoot_delay ++;
				}
			}
			
			/* friction wheel speed set */
		
			/* control pluck motor */
			taskENTER_CRITICAL();
			M2006_control();
			taskEXIT_CRITICAL();

			/* ���������������� �ָ�����PID���� */
			if(powerOff_delay > 0)
			{
				PID_struct_init(&pluck.speedPID,1,10000,500,8.0f,-0.f,-.0f);
				PID_struct_init(&pluck.positionPID,1,10000,500,0.25f,0.05f,0.0f);
				
				powerOff_delay = 0;
			}
		}
		
		/* ���� */
		/* �������ֹͣ �� (��̨�豸���� �� �Ƿ����������) */
		else if(RC_Ctl.rc.s1 == 2||droneState.drone.droneState != 0)
		{
			/* poweroff delay */
			if(powerOff_delay<2000)	
			{
				/* �˴�ֻ�Ǳ��� ���޷�����0 */
				pluck.setPoint 		= pluck.Angle_zeroCheck ;
				pluck.speedPID.p -= pluck.speedPID.p/2000;
				pluck.speedPID.i -= pluck.speedPID.i/2000;
				pluck.speedPID.d -= pluck.speedPID.d/2000;
				
				/* control pluck motor */
				taskENTER_CRITICAL();
				M2006_control();
				taskEXIT_CRITICAL();
				
				powerOff_delay ++;
			}
		}

		vTaskDelay(1);	
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
	pluck.zero.CountCycle = 8191; 
	pluck.mode = doubleLoop;
	pluck.receiveId = 0x201;
	ZeroCheck_Init_Interface(&pluck.zero,8191,pluck.receiveAngle);
	pluck.setPoint = pluck.Angle_zeroCheck;

	PID_struct_init(&pluck.speedPID,1,10000,500,20.0f,0.0f,0.0f);
	PID_struct_init(&pluck.positionPID,1,10000,500,0.226f,0.0f,0.7f);
}


/* end */
