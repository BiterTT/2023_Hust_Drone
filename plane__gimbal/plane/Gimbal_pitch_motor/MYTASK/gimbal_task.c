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
extern short ContralMode;
extern float new_value; 
extern float old_value; 
extern short UI_Change_Mode;

uint32_t gimbalinitTime;
short LastContralMode=Lost;
int Grativity  = 0;
float set_pitch;
float set_yaw;
float set_yaw_Motor;
float set_pitch_Motor;

float INCAngle_Low,INCAngle_High;

int RC_IMU_control = 1;//Ĭ�������Ƿ���
int RC_Motor_control = 0;

int yaw_left_limit;
int yaw_right_limit;
int pitch_high_limit_imu; 
int pitch_low_limit_imu;
int pitch_high_limit_motor;
int pitch_low_limit_motor;

float yaw_RC_sensitivity 									=	0.0008f;	//ң��ģʽyaw������_imu����
float pitch_RC_sensitivity								=	0.0004f;	//ң��ģʽpitch������ imu����
float yaw_RC_sensitivity_motor 						=	0.016f;		//ң��ģʽyaw������_�������
float pitch_RC_sensitivity_motor					=	0.008f;		//ң��ģʽpitch������ �������
float yaw_PC_Mouse_sensitivity						=	0.03f;		//����ģʽyaw��������� imu����
float pitch_PC_Mouse_sensitivity 					= 0.04f;		//����ģʽpitch��������� imu����
float yaw_PC_Mouse_sensitivity_motor			=	0.03f;		//����ģʽyaw��������� �������
float pitch_PC_Mouse_sensitivity_motor 		= 0.04f;		//����ģʽpitch��������� �������
float yaw_PC_Keyboard_sensitivity 				= 0.80f;		//����ģʽyaw���������� imu����
float pitch_PC_Keyboard_sensitivity 			= 0.25f;		//����ģʽpitch���������� imu����
float yaw_PC_Keyboard_sensitivity_motor 	= 0.10f;		//����ģʽyaw���������� �������
float pitch_PC_Keyboard_sensitivity_motor = 0.15f;		//����ģʽpitch���������� �������

//static void limit_max_min(float *value,float max,float min);
static void imugimbal_config(void);
static void Plane_ID_Init(void );
int a,b;
/*
 *    			S2							    					S1    
 *  1 --> ������̨�����(���)    1 --> ����ģʽ	
 * 	                             
 *  3 --> ������̨�����(IMU)     3 --> ����ģʽ	
 *	                     								
 *  2 --> ����ģʽ     						2 --> ң��ģʽ
 *					
 * (���ظ���: S1 turn from rc mode to key mode and turn back in 1.5 seconds) 
 */
 //IMU���ұ�� YAW���ұ��
 //IMU�ϵ��´�С,�����¸�,PITCH�ϵ��±�С


/* IMU��̨�������� */
void ImugimbalTask( void * pvParameters )
{
	/* IMU��̨�������� */
	imugimbal_config();
	Plane_ID_Init();
	//vTaskDelay(5000);
	gimbalinitTime = xTaskGetTickCount();

	//��̨�ϵ�ڵ�����,����Ϊ�������ϵ�,���û�����ݴ���,Ĭ��ֵΪ0,����Լ�����,����ϵ������ݺ��ٽ�����̨����
	if(xTaskGetTickCount()-gimbalinitTime<2000)
	{
		//����ǿ���
		if(RC_Motor_control ==1)
		{
			set_yaw_Motor = yaw.receiveAngle;			
			set_pitch_Motor = pitch.receiveAngle;
			yaw.setPoint = set_yaw_Motor;
			pitch.setPoint = set_pitch_Motor;
		}
		//�����ǿ���
		if(RC_IMU_control ==1) 
		{
			new_value = imu.pitchAngle;
			old_value = imu.pitchAngle;
			set_yaw = imu.yawAngle;
			//IMU_Control_Limit(3800,4700,&pitch,&set_pitch,imu.pitchAngle);
			set_pitch = imu.pitchAngle;
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
			yaw.setPoint = set_yaw;
			pitch.setPoint = set_pitch;
		}
	}
	for(;;)
	{
		yaw.zero.Circle=0;
		pitch.zero.Circle=0;
		/* gimbal running */		
		//if(ContralMode!=Lost)
		if((droneState.drone.droneState&0x55) == 0 && ContralMode != Lost)
		{
			//����ģʽ
			PC_gimbal();
			//����ģʽ
			Auto_gimbal();
			//ң��ģʽ
			RC_gimbal();
		}
		else
		{	//����ģʽ
			Lost_gimbal();
		}
		taskENTER_CRITICAL();
		GM6020_control();
   	taskEXIT_CRITICAL();
		/* send data to TX2 */
		PCSend();
		vTaskDelay(10);
	}
}

/* limit */
void limit_max_min(float *value,float max,float min)//��λ����
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

void Auto_gimbal(void)
{
	if(ContralMode == Auto)//����ģʽ
	{
			set_yaw_Motor = yaw.receiveAngle;			
			set_pitch_Motor = pitch.receiveAngle;
		/**************�����������ֵΪ0,��setpoint��Ϊimu��ǰֵ   */
		if(PC_Recv.PCTargetYaw!=0)
		{
			set_yaw = PC_Recv.PCTargetYaw;
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ

		}
		else 
		{
			set_yaw = imu.yawAngle;
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
		}
		
		if(PC_Recv.PCTargetPitch!=0)
		{
			set_pitch = PC_Recv.PCTargetPitch;
			limit_max_min(&set_pitch,pitch_high_limit_imu,pitch_low_limit_imu); 
		}
		else
		{
			set_pitch = imu.pitchAngle;
			limit_max_min(&set_pitch,pitch_high_limit_imu,pitch_low_limit_imu); 

		}
		yaw.setPoint = (float)(set_yaw);	
		pitch.setPoint = (float)(set_pitch);
	}
}
void PC_gimbal(void)
{
	if(ContralMode == Pc)//����ģʽ
	{
		if(RC_IMU_control == 1)//IMU��������
		{
			set_yaw_Motor = yaw.receiveAngle;			
			set_pitch_Motor = pitch.receiveAngle;
			
			set_yaw += (((float)RC_Ctl.rc.ch0 - 1024) * yaw_RC_sensitivity);
			set_pitch +=  ((float)RC_Ctl.rc.ch1 - 1024) * pitch_RC_sensitivity;		
			
			set_pitch -= (float)RC_Ctl.mouse.y * pitch_PC_Mouse_sensitivity;
			set_yaw += (float)RC_Ctl.mouse.x * yaw_PC_Mouse_sensitivity;
				
			/* keyboard for fine tuning */
			if(UI_Change_Mode == 0)
			{
				set_pitch = set_pitch + ((float)RC_Ctl.key.w - (float)RC_Ctl.key.s) * pitch_PC_Keyboard_sensitivity;
				set_yaw = set_yaw + ((float)RC_Ctl.key.d - (float)RC_Ctl.key.a)*yaw_PC_Keyboard_sensitivity;
			}
			
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
			yaw.setPoint = set_yaw;
			yaw.ffout = FeedForward_Cal(&yaw.AngelFF , imu.yawAngle);//yawǰ��
			limit_max_min(&set_pitch,pitch_high_limit_imu,pitch_low_limit_imu);//�����ǽǶ�
			//IMU_Control_Limit(3800,4600,&pitch,&set_pitch,imu.pitchAngle);
			pitch.setPoint = set_pitch;
			pitch.ffout = FeedForward_Cal(&pitch.AngelFF ,imu.pitchAngle);//pitchǰ��
			
//		yaw.ffout = FeedForward_Cal(&yaw.AngelFF , yaw.Angle_zeroCheck);//ǰ��
//		limit_max_min(&set_pitch,4700,3800);//�����
//		pitch.setPoint = (int)set_pitch;
//		pitch.ffout = FeedForward_Cal(&pitch.AngelFF ,pitch.Angle_zeroCheck);//ǰ��	
		}
		if(RC_Motor_control == 1)	//�����������
		{
			set_yaw = imu.yawAngle;
			//IMU_Control_Limit(3800,4700,&pitch,&set_pitch,imu.pitchAngle);
			set_pitch = imu.pitchAngle;
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
				
			set_yaw_Motor += (((float)RC_Ctl.rc.ch0 - 1024) * yaw_RC_sensitivity_motor);
			set_pitch_Motor -=  ((float)RC_Ctl.rc.ch1 - 1024) * pitch_RC_sensitivity_motor;		
			
			set_yaw_Motor += (float)RC_Ctl.mouse.x * yaw_PC_Mouse_sensitivity_motor;
			set_pitch_Motor -=  (float)RC_Ctl.mouse.y * pitch_PC_Mouse_sensitivity_motor;		
			
			if(UI_Change_Mode == 0)
			{
				set_pitch_Motor = set_pitch_Motor + ((float)RC_Ctl.key.w - (float)RC_Ctl.key.s) * pitch_PC_Keyboard_sensitivity_motor;
				set_yaw_Motor = set_yaw_Motor + ((float)RC_Ctl.key.d - (float)RC_Ctl.key.a) * yaw_PC_Keyboard_sensitivity_motor;
			}
			
			limit_max_min(&set_yaw_Motor,yaw_right_limit,yaw_left_limit);//�����
			yaw.setPoint = set_yaw_Motor;
			yaw.ffout = FeedForward_Cal(&yaw.AngelFF , yaw.Angle_zeroCheck);//ǰ��
			
			limit_max_min(&set_pitch_Motor,pitch_high_limit_motor,pitch_low_limit_motor);//�����
			pitch.setPoint = set_pitch_Motor;
			pitch.ffout = FeedForward_Cal(&pitch.AngelFF ,pitch.Angle_zeroCheck);//ǰ��				
		}
	}	
}
void RC_gimbal(void)
{
	if(ContralMode ==Rc)//ң��ģʽ
	{
		if(RC_IMU_control == 1)//IMU��������
		{
			set_yaw_Motor = yaw.receiveAngle;			
			set_pitch_Motor = pitch.receiveAngle;
			if(((RC_Ctl.rc.ch0-1024<10)&&(RC_Ctl.rc.ch0-1024>-10))) 
			{
				RC_Ctl.rc.ch0=1024;
			}
			if(((RC_Ctl.rc.ch1-1024<10)&&(RC_Ctl.rc.ch1-1024>-10)))
			{	
				RC_Ctl.rc.ch1=1024;
			}
			set_yaw += (((float)RC_Ctl.rc.ch0 - 1024) * yaw_RC_sensitivity);//0.008f
			set_pitch +=  ((float)RC_Ctl.rc.ch1 - 1024) * pitch_RC_sensitivity;		//0.015f
			
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
			yaw.setPoint = set_yaw;
			
			limit_max_min(&set_pitch,pitch_high_limit_imu,pitch_low_limit_imu);//�����ǽǶ�
			//IMU_Control_Limit(3800,4600,&pitch,&set_pitch,imu.pitchAngle);
			pitch.setPoint = set_pitch;
			pitch.ffout = FeedForward_Cal(&pitch.AngelFF ,imu.pitchAngle);//ǰ��
		}
			
		if(RC_Motor_control == 1)	//�����������
		{
			set_yaw = imu.yawAngle;
			//IMU_Control_Limit(3800,4700,&pitch,&set_pitch,imu.pitchAngle);
			set_pitch = imu.pitchAngle;
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
			//�����
			if(ContralMode==Lost||((RC_Ctl.rc.ch0-1024<10)&&(RC_Ctl.rc.ch0-1024>-10))) 
			{
				RC_Ctl.rc.ch0=1024;
			}
			if(ContralMode==Lost||((RC_Ctl.rc.ch1-1024<10)&&(RC_Ctl.rc.ch1-1024>-10)))
			{	
				RC_Ctl.rc.ch1=1024;
			}
				
			set_yaw_Motor += (((float)RC_Ctl.rc.ch0 - 1024) * yaw_RC_sensitivity_motor);//0.008f
			set_pitch_Motor -=  ((float)RC_Ctl.rc.ch1 - 1024) * pitch_RC_sensitivity_motor;		//0.015f

			limit_max_min(&set_yaw_Motor,yaw_right_limit,yaw_left_limit);//�����
			yaw.setPoint = set_yaw_Motor;
			yaw.ffout = FeedForward_Cal(&yaw.AngelFF , yaw.Angle_zeroCheck);//ǰ��
			
			limit_max_min(&set_pitch_Motor,pitch_high_limit_motor,pitch_low_limit_motor);//�����
			pitch.setPoint = set_pitch_Motor;
			pitch.ffout = FeedForward_Cal(&pitch.AngelFF ,pitch.Angle_zeroCheck);//ǰ��			
		}
	}
}

void Lost_gimbal(void)//����ģʽ
{
	//if(ContralMode==Lost)
	//if((droneState.drone.droneState&0x55) != 0 ||ContralMode == Lost)
	{
		if(RC_Motor_control == 1)//����Ƿ�������_����
		{
			set_yaw_Motor = yaw.Angle_zeroCheck;
			set_pitch_Motor = pitch.Angle_zeroCheck;
			yaw.setPoint = set_yaw_Motor;
			pitch.setPoint = set_pitch_Motor;
			RC_Ctl.rc.ch0=1024;
			RC_Ctl.rc.ch1=1024;
			RC_Ctl.rc.ch2=1024;
			RC_Ctl.rc.ch3=1024;
		}
		if(RC_IMU_control == 1)//�����Ƿ�������_����
		{
			new_value = imu.pitchAngle;
			old_value = imu.pitchAngle;
			
			set_pitch = imu.pitchAngle;
			//IMU_Control_Limit(3800,4600,&pitch,&set_pitch,imu.pitchAngle);
			set_yaw = imu.yawAngle;
			IMU_Control_Limit(yaw_left_limit,yaw_right_limit,&yaw,&set_yaw,imu.yawAngle);//����ǿ�����������λ
			yaw.setPoint = set_yaw;
			pitch.setPoint = set_pitch;
			RC_Ctl.rc.ch0=1024;
			RC_Ctl.rc.ch1=1024;
			RC_Ctl.rc.ch2=1024;
			RC_Ctl.rc.ch3=1024;
		}
	}
}

void IMU_Control_Limit(float min,float max, struct GM6020 *Motor, float *set, float imuAngle)
{
	INCAngle_Low =(min - Motor->Angle_zeroCheck)/8192*360;
	
	INCAngle_High = (max - Motor->Angle_zeroCheck)/8192*360;
	limit_max_min(set, (imuAngle + INCAngle_High) , (imuAngle + INCAngle_Low) );
	a = (imuAngle + INCAngle_High);
	b = (imuAngle + INCAngle_Low);
}

/* imu gimbal config */
static void imugimbal_config(void)			//imu��̨��ʼ������
{
#if Plane_ID == 1
	FeedForwardInit();
	/* yaw���ʼ�� */
	yaw.id = 0;
	yaw.receiveId = 0x207;
	ZeroCheck_Init_Interface(&yaw.zero,8191,yaw.receiveAngle);
	yaw.mode = imuYaw;	
	//								PID�ṹ��	 mode ����޷�	�����޷�	 P				 I			D		
	//��ͨPID---RC	
	PID_struct_init(&yaw.speedPID,		1,	30000.0f,	100.0f,	45.0f,0.5f, -0.0f);	//-1700.0f,-0.00f, -2500.0f   75.0f 0 0
	PID_struct_init(&yaw.positionPID,	1,	30000.0f,	100.0f,	2.5f,		0.0f,	 2.0f	 );	//1.405f,		0.0051f,	 .9f		2.5F 0.005 1
	//���������---RC
	PID_struct_init(&yaw.Motor_SpeedPID ,		1,	30000.0f,	100.0f,	-350.0f, -0.01f, -0.0f);	
	PID_struct_init(&yaw.Motor_PositionPID,	1,	30000.0f,	100.0f,	0.5f,		0.001f,	 0.0f	 );	
	
	//�������  speed 1,	30000.0f,	100.0f,	45.0f, 0.0001f, -0.0f
	//   				position 1,	30000.0f,	100.0f,	1.5f,		0.0005f,	 1.0f	 
	//ģ��PID---RC
	Fuzzy_PID_struct_init(&yaw.speedPID_fuzzy, 30000.0f, 100.0f,15.0f,0.3f,-0.3f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,5.0f,0.5f,0.15f,0.1f);
	Fuzzy_PID_struct_init(&yaw.positionPID_fuzzy, 30000.0f, 100.0f,8.0f,0.0f,-0.0f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,3.0f,0.5f,0.05f,0.1f);
	//����PID---Auto
	PID_struct_init(&yaw.AutospeedPID,		1,	30000.0f,	35.0f,	-5200.0f,	-10.0f, -600.0f);  //ͬRc
	//PID_struct_init(&yaw.AutopositionPID,	1,	30000.0f,	35.0f,	 0.35f,		0.02f,	 0.0f);	//0.2 0.004 0.3
	PID_struct_init(&yaw.AutopositionPID,	1,	30000.0f,	35.0f,	 0.35f,		0.02f,	 0.f);	//0.2 0.004 0.3
	
	/* pitch���ʼ�� */
	pitch.id = 0;
	pitch.receiveId = 0x206;
	ZeroCheck_Init_Interface(&pitch.zero,8191,pitch.receiveAngle);
	pitch.mode = imuPitch;	
	
	//									PID�ṹ��	  mode ����޷�	 �����޷�	 P			I			  D		
	//��ͨPID---RC	
	PID_struct_init(&pitch.speedPID,		1,	30000.0f,	100.0f,	700.0f,	 -20.0f, -0.0f);//6000f,	0.00f, 8000f   -46 0.001
	PID_struct_init(&pitch.positionPID,	1,	30000.0f,	200.0f,	0.2f,		0.0f,	0.0f	 );//0.15f,		0.00f,	0.15f	   0.1   0.0001
	//���������---RC
	PID_struct_init(&pitch.Motor_SpeedPID,		1,	30000.0f,	100.0f,	-35.0f,	 0.0f, 0.0f); //-50 0 0
	PID_struct_init(&pitch.Motor_PositionPID,	1,	30000.0f,	200.0f,	1.0f,		0.01f,	1.5f	 );//1.5 0.01 1.5
	//ģ��PID---RC
	Fuzzy_PID_struct_init(&pitch.speedPID_fuzzy, 30000.0f, 100.0f,15.0f,0.2f,-0.3f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,5.0f,0.5f,0.15f,0.1f);
	Fuzzy_PID_struct_init(&pitch.positionPID_fuzzy, 30000.0f, 100.0f,8.0f,0.0f,-0.0f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,3.0f,0.5f,0.05f,0.1f);
	//����PID---Auto
	PID_struct_init(&pitch.AutospeedPID,		1,	30000.0f,	20000.0f,	4000.0f,	3.0f, 4500.0f); //ͬRc
	PID_struct_init(&pitch.AutopositionPID,	1,	30000.0f,	100.0f,	 0.25f, 0.005f,	 0.1f	 );	//0.4 0.32
#elif Plane_ID ==2
	FeedForwardInit();
	/* yaw���ʼ�� */
	yaw.id = 0;
	yaw.receiveId = 0x207;
	ZeroCheck_Init_Interface(&yaw.zero,8191,yaw.receiveAngle);
	yaw.mode = imuYaw;	
	//								PID�ṹ��	 mode ����޷�	�����޷�	 P				 I			D		
	//��ͨPID---RC	
	PID_struct_init(&yaw.speedPID,		1,	30000.0f,	100.0f,	45.0f,0.5f, -0.0f);	//-1700.0f,-0.00f, -2500.0f   75.0f 0 0
	PID_struct_init(&yaw.positionPID,	1,	30000.0f,	100.0f,	2.5f,		0.0f,	 2.0f	 );	//1.405f,		0.0051f,	 .9f		2.5F 0.005 1
	//���������---RC
	PID_struct_init(&yaw.Motor_SpeedPID ,		1,	30000.0f,	100.0f,	-350.0f, -0.01f, -0.0f);	
	PID_struct_init(&yaw.Motor_PositionPID,	1,	30000.0f,	100.0f,	0.5f,		0.0001f,	 0.0f	 );	
	
	//�������  speed 1,	30000.0f,	100.0f,	45.0f, 0.0001f, -0.0f
	//   				position 1,	30000.0f,	100.0f,	1.5f,		0.0005f,	 1.0f	 
	//ģ��PID---RC
	Fuzzy_PID_struct_init(&yaw.speedPID_fuzzy, 30000.0f, 100.0f,15.0f,0.3f,-0.3f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,5.0f,0.5f,0.15f,0.1f);
	Fuzzy_PID_struct_init(&yaw.positionPID_fuzzy, 30000.0f, 100.0f,8.0f,0.0f,-0.0f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,3.0f,0.5f,0.05f,0.1f);
	//����PID---Auto
	PID_struct_init(&yaw.AutospeedPID,		1,	30000.0f,	35.0f,	-4500.0f,	-10.0f, -600.0f);  //ͬRc
	PID_struct_init(&yaw.AutopositionPID,	1,	30000.0f,	35.0f,	 0.35f,		0.02f,	 0.0f);	//0.2 0.004 0.3
	
	/* pitch���ʼ�� */
	pitch.id = 0;
	pitch.receiveId = 0x206;
	ZeroCheck_Init_Interface(&pitch.zero,8191,pitch.receiveAngle);
	pitch.mode = imuPitch;	
	
	//									PID�ṹ��	  mode ����޷�	 �����޷�	 P			I			  D		
	//��ͨPID---RC	
	PID_struct_init(&pitch.speedPID,		1,	30000.0f,	100.0f,	700.0f,	 -20.0f, -0.0f);//6000f,	0.00f, 8000f   -46 0.001
	PID_struct_init(&pitch.positionPID,	1,	30000.0f,	200.0f,	0.2f,		0.0f,	0.0f	 );//0.15f,		0.00f,	0.15f	   0.1   0.0001
	//���������---RC
	PID_struct_init(&pitch.Motor_SpeedPID,		1,	30000.0f,	100.0f,	-50.0f,	 0.0f, 0.0f); //-50 0 0
	PID_struct_init(&pitch.Motor_PositionPID,	1,	30000.0f,	200.0f,	2.0f,		0.001f,	1.5f	 );//1.5 0.01 1.5
	//ģ��PID---RC
	Fuzzy_PID_struct_init(&pitch.speedPID_fuzzy, 30000.0f, 100.0f,15.0f,0.2f,-0.3f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,5.0f,0.5f,0.15f,0.1f);
	Fuzzy_PID_struct_init(&pitch.positionPID_fuzzy, 30000.0f, 100.0f,8.0f,0.0f,-0.0f,0.0f,0.0f,0.0f,0.4f,0.2f,0.0f,0.5f,3.0f,0.5f,0.05f,0.1f);
	//����PID---Auto
	PID_struct_init(&pitch.AutospeedPID,		1,	30000.0f,	20000.0f,	4100.0f,	3.0f, 4000.0f); //ͬRc
	PID_struct_init(&pitch.AutopositionPID,	1,	30000.0f,	100.0f,	 0.25f, 0.015f,	 0.0f	 );	//0.4 0.32
#endif
}

static void Plane_ID_Init(void )
{
#if Plane_ID == 1
//*************************************************		
		yaw_left_limit  = 1695;//900
		yaw_right_limit =  4000;//6700
    pitch_high_limit_imu  = 10; 
		pitch_low_limit_imu = -30;//-40
		pitch_high_limit_motor = 4000;
		pitch_low_limit_motor = 150;
	
#elif Plane_ID == 2
//*************************************************	
		yaw_left_limit  = 5200;
		yaw_right_limit =  8100;
    pitch_high_limit_imu  = 10; 
		pitch_low_limit_imu = -30;4
		pitch_high_limit_motor = 7500;
		pitch_low_limit_motor = 6500;
	
#endif

}
/* end */

