/* include */
#include "config.h"

/* declare */
extern struct state droneState;
extern struct GM6020 yaw;
extern struct GM6020 pitch;
extern struct IMU 	 imu;
extern struct trajectory_t trajectory;
extern RC_Ctl_t RC_Ctl;
extern PC_Recv_t PC_Recv;
short ContralMode=Rc;
uint32_t AimAssist_On_Delay = 0;
uint32_t gimbalinitTime;
short LastContralMode=Rc;
static void limit_max_min(float *value,float max,float min);
static void imugimbal_config(void);

/*
 *    		S2							       S1    
 *  1 --> ��ʼ���         1 --> ����ģʽ	
 * 	                             
 *  3 --> ׼�����         3 --> ң��ģʽ	
 *	                      								
 *  2 --> ֹͣ���         2 --> ��̨����
 *					
 * (���ظ���: S1 turn from rc mode to key mode and turn back in 1.5 seconds) 
 */
float yaw_offset,pitch_offset;
float set_pitch;
/* IMU��̨�������� */
void ImugimbalTask( void * pvParameters )
{
	static float yaw_scale = -23.0f;
	static float pitch_scale = -23.0f;//����ȡ���ڵ���ĽǶ��������������Ƿ�һ��
	static float set_yaw; 
	
	/* IMU��̨�������� */
	imugimbal_config();
	
	gimbalinitTime = xTaskGetTickCount();

	/* �״�ִ������Yaw�ᵱǰֵ��Ϊ�趨ֵ */
	/* ��������������Ťͷ�ϵ��ֱ�Ӱڵ����� ������̨����������ϵ�(��Ť)�ڿ�����̨������
			��������Ϊ�����һ�ν�������ʱ�趨һ�ε�ǰ�Ƕ�ΪĿ���ʧ��
			���޸�Ϊ����500ms�趨��ǰ�Ƕ� */
	while(xTaskGetTickCount()-gimbalinitTime<500)
	{
		set_yaw = imu.yawAngle;
		set_pitch = imu.pitchAngle;
		vTaskDelay(1);
	}
		
	for(;;)
	{
		/* gimbal running */
		ChangeMode();		
		/* key mode */
		if(ContralMode!=Lost)
		{
			if(ContralMode == Pc)
	  	{
				/* AimAssist mode count time */
				  AimAssist_On_Delay++;
				
					/* Pitch -> mouse Y aixs 
						 Yaw	 -> mouse X aixs */
					set_pitch -= (float)RC_Ctl.mouse.y * 0.001f;
					set_yaw -= (float)RC_Ctl.mouse.x * 0.001f;
					
					/* keyboard for fine tuning */
					set_pitch = set_pitch + (float)RC_Ctl.key.w * 0.01f - (float)RC_Ctl.key.s * 0.01f;
					set_yaw = set_yaw - (float)RC_Ctl.key.d * 0.01f + (float)RC_Ctl.key.a * 0.01f;
		  }
				/* aimassist mode */
		  if(ContralMode == Auto)
		  {
		    	/* �������̽��㣺ע������������� ������������ */
//		  	trajectory_equation();
					
				  set_yaw = PC_Recv.PCTargetYaw;
				  	
				  /* witheout trajectory equation */
				  set_pitch = PC_Recv.PCTargetPitch;
		  }
			/* rc mode */
		  else if(ContralMode == Rc)
			{
				
				
				/* AimAssist mode change */
				{
					if((AimAssist_On_Delay >= 100)&&(AimAssist_On_Delay <= 1500))
				  {
					  /* change AimAssist state (ON<==>OFF) */
					  if(LastContralMode == Rc)
					  {
						  ContralMode = Auto;
					  }
					  else if(LastContralMode == Auto)
				  	{
						  ContralMode = Rc;	
		          set_yaw = imu.yawAngle;
		          set_pitch = imu.pitchAngle;
	          }
					  LastContralMode=ContralMode;
				  }
				}
				
				
					AimAssist_On_Delay = 0;
				
					if((RC_Ctl.rc.ch0-1024<10)&&(RC_Ctl.rc.ch0-1024>-10)) RC_Ctl.rc.ch0=1024;
					if((RC_Ctl.rc.ch1-1024<10)&&(RC_Ctl.rc.ch1-1024>-10)) RC_Ctl.rc.ch1=1024;
					
					set_yaw -= ((float)RC_Ctl.rc.ch0 - 1024) * 0.0001f;
					set_pitch -=  ((float)RC_Ctl.rc.ch1 - 1024) * 0.00003f;		
			}
			
			yaw_offset = (yaw.Angle_zeroCheck - YawMotorCenter)/yaw_scale - imu.yawAngle; 
			pitch_offset = (pitch.Angle_zeroCheck - PitchMotorCenter)/pitch_scale - imu.pitchAngle;
			/* yaw_Maxȷ���ķ�Χ���Ե�������Ƕ�Ϊ��׼���޷� */
			/* �˴�ͨ������ƫ�ý���Χ�޸�Ϊ��������Ϊ��׼���޷� ��Ϊ����������Ϊ��׼���п���*/
			//limit_max_min(&set_yaw,-yaw_offset+yaw_Max,-yaw_offset+yaw_Min);
			yaw.setPoint = set_yaw;
			//limit_max_min(&set_pitch,pitch_offset+pitch_Max,pitch_offset+pitch_Min);
			pitch.setPoint = set_pitch;
			
			taskENTER_CRITICAL();
    	GM6020_control();
   		taskEXIT_CRITICAL();
		}
    if(ContralMode==Lost)
		{
			set_yaw = imu.yawAngle;
			set_pitch = imu.pitchAngle;
		}
		
		/* send data to TX2 */
		PCSend();
		
		vTaskDelay(1);
	}
}

/* limit */
static void limit_max_min(float *value,float max,float min)
{
	if(*value >max)
	{
		*value = max;
	}
	else if(*value<min)
	{
		*value = min;
	}
}
uint8_t Last_AimAssist = 0;
int LastRcS1=3;
short LastC=0;
short LastZ=0;
void ChangeMode(void)
{
	//PC contral change mode
	{
	  //PC contral Lost
		  if(RC_Ctl.key.c == 1 && LastC == 0)
		  {
			  if( ContralMode == Pc)
			       ContralMode = Lost;
			  if( ContralMode == Lost)
			       ContralMode = Pc;
		  }
			
		  LastC=RC_Ctl.key.c;
			
		  //Pc contral RcCOntral or PcContral
		  if (RC_Ctl.key.z == 1 && LastZ == 0)
		  {
			  if(ContralMode == Pc)
			       ContralMode = Rc;
			  if(ContralMode == Rc)
			       ContralMode = Pc;
		  }
			LastZ=RC_Ctl.key.z;
		}
	
		
	//Rc contral change mode
		{
			
		    //Rc contral  PcContral
		    if(LastRcS1 == 3 && RC_Ctl.rc.s1 == 1)
		    {
			    ContralMode = Pc;
		    }
		
		    //Rc contral RcContral
		    if(LastRcS1 == 1 && RC_Ctl.rc.s1 == 3)
		    {
			    ContralMode = Rc;
		    }
		
		    //Rc contral Lost or not
			  if(LastRcS1 != 2 && RC_Ctl.rc.s1 == 2)
		    {
			    ContralMode=Lost;
		    }
        if(RC_Ctl.rc.s1 != 2 && LastRcS1 == 2)
		    {
		  	  ContralMode=Rc;
	  	  }
  	    
				LastRcS1=RC_Ctl.rc.s1;
			
		}
}
/* imu gimbal config */

static void imugimbal_config(void)
{
	/* yaw */
	yaw.id = 0;
	yaw.receiveId = 0x207;
	ZeroCheck_Init_Interface(&yaw.zero,8191,yaw.receiveAngle);
	yaw.mode = imuYaw;
			
//										 PID�ṹ��	 mode ����޷�	�����޷�	 P				 I			D		
	/* 2020.1.11 */
		PID_struct_init(&yaw.speedPID,		1,	30000.0f,	100.0f,	-2000.0f,-0.00f, -2500.0f);	//-1700.0f,-0.00f, -2500.0f
		PID_struct_init(&yaw.positionPID,	1,	30000.0f,	100.0f,	 1.2f,		0.00f,	 .0f	 );	//1.405f,		0.0051f,	 .9f	

		PID_struct_init(&yaw.PcpositionPID,	1,	30000.0f,	100.0f,	 0.6f,		0.0f,	 0.0f	 );	//0.6 0 0
	  PID_struct_init(&yaw.PcspeedPID,		1,	30000.0f,	100.0f,	-1700.0f,	-0.006f, -2500.0f);  //ͬRc
	
			//auto
	  PID_struct_init(&yaw.AutopositionPID,	1,	30000.0f,	100.0f,	 0.2f,		0.004f,	 0.3f	 );	//0.2 0.004 0.3
	  PID_struct_init(&yaw.AutospeedPID,		1,	30000.0f,	100.0f,	-1700.0f,	-0.006f, -2500.0f);  //ͬRc
	
	/* pitch */
	pitch.id = 0;
	pitch.receiveId = 0x206;
	ZeroCheck_Init_Interface(&pitch.zero,8191,pitch.receiveAngle);
	pitch.mode = imuPitch;

//										 PID�ṹ��	  mode ����޷�	 �����޷�	 P			I			  D		
//rc����
	PID_struct_init(&pitch.speedPID,		1,	30000.0f,	100.0f,	2500.0f,	  0.00f, 1500.0f);//4500.0f,	0.00f, 1.5f
	PID_struct_init(&pitch.positionPID,	1,	30000.0f,	100.0f,	1.f,		0.015f,	3.5f	 );//0.4f,		0.02f,	0.01f
	
//pc����
	PID_struct_init(&pitch.PcpositionPID,	1,	30000.0f,	100.0f,	 -2.0f,		-0.0f,	 -0.0f	 );	//-2 0 0
	PID_struct_init(&pitch.PcspeedPID,		1,	30000.0f,	100.0f,	400.0f,	0.00f, 100.0f); //ͬRc
	  
//������׼
	PID_struct_init(&pitch.AutopositionPID,	1,	30000.0f,	100.0f,	 -2.0f,		-0.1f,	 -20.0f	 );	//-2 -0.1 -20
	PID_struct_init(&pitch.AutospeedPID,		1,	30000.0f,	100.0f,	400.0f,	0.00f, 100.0f); //ͬRc
}

/* end */










