#include "config.h"

short LastCtrl=0;
short LastShift=0;
short ContralMode=Lost;
int LastRcS1=3;
int shoot_mode = stop_shoot;//��ʼ��ģʽΪֹͣ��
extern RC_Ctl_t RC_Ctl;
extern PC_Recv_t PC_Recv;
extern int RC_IMU_control;
extern int RC_Motor_control;
extern struct state droneState;
extern float yaw_RC_sensitivity;									//ң��ģʽyaw������_imu����
extern float pitch_RC_sensitivity;								//ң��ģʽpitch������ imu����
extern float yaw_RC_sensitivity_motor;						//ң��ģʽyaw������_�������
extern float pitch_RC_sensitivity_motor;					//ң��ģʽpitch������ �������
extern float yaw_PC_Mouse_sensitivity;						//����ģʽyaw��������� imu����
extern float pitch_PC_Mouse_sensitivity;					//����ģʽpitch��������� imu����
extern float yaw_PC_Mouse_sensitivity_motor;			//����ģʽyaw��������� �������
extern float pitch_PC_Mouse_sensitivity_motor;		//����ģʽpitch��������� �������
extern float yaw_PC_Keyboard_sensitivity;					//����ģʽyaw���������� imu����
extern float pitch_PC_Keyboard_sensitivity;				//����ģʽpitch���������� imu����
extern float yaw_PC_Keyboard_sensitivity_motor;		//����ģʽyaw���������� �������
extern float pitch_PC_Keyboard_sensitivity_motor;	//����ģʽpitch���������� �������

RC_Ctl_t rc_ctrl_last;
short q_rising_flag,w_rising_flag,e_rising_flag,r_rising_flag,
		a_rising_flag,s_rising_flag,d_rising_flag,f_rising_flag,g_rising_flag,
		z_rising_flag,x_rising_flag,c_rising_flag,v_rising_flag,b_rising_flag,
		shift_rising_flag,ctrl_rising_flag,mouse_Press_l_rising_flag,mouse_Press_r_rising_flag;
void key_rising_check(void);
void key_refresh(void);
void shoot_mode_check(void);
void ChangeMode(void);
/*
 *    			S2							    S1    
 *  1 --> 	��         1 --> ����ģʽ	
 * 	                             
 *  3 --> �������     3 --> ����ģʽ	
 *	                     								
 *  2 --> ����ģʽ     2 --> ң��ģʽ
 *					
 * (���ظ���: S1 turn from rc mode to key mode and turn back in 1.5 seconds) 
 */

/**
  * @brief  �ı�״̬�����ڼ�Ⲧ�ˣ�����״̬����ʼ״̬
  * @param  None
  * @retval None
  */
void key_refresh(void)
{
	rc_ctrl_last.rc.s1 = RC_Ctl.rc.s1;
	rc_ctrl_last.rc.s2 = RC_Ctl.rc.s2;
	rc_ctrl_last.key.q = RC_Ctl.key.q;
	rc_ctrl_last.key.w = RC_Ctl.key.w;
	rc_ctrl_last.key.e = RC_Ctl.key.e;
	rc_ctrl_last.key.r = RC_Ctl.key.r;
	rc_ctrl_last.key.a = RC_Ctl.key.a;
	rc_ctrl_last.key.s = RC_Ctl.key.s;
	rc_ctrl_last.key.d = RC_Ctl.key.d;
	rc_ctrl_last.key.f = RC_Ctl.key.f;
	rc_ctrl_last.key.g = RC_Ctl.key.g;
	rc_ctrl_last.key.z = RC_Ctl.key.z;
	rc_ctrl_last.key.x = RC_Ctl.key.x;
	rc_ctrl_last.key.c = RC_Ctl.key.c;
	rc_ctrl_last.key.v = RC_Ctl.key.v;
	rc_ctrl_last.key.b = RC_Ctl.key.b;
	rc_ctrl_last.key.ctrl = RC_Ctl.key.ctrl;
	rc_ctrl_last.key.shift = RC_Ctl.key.shift;
	
	rc_ctrl_last.mouse.press_l = RC_Ctl.mouse.press_l;
	rc_ctrl_last.mouse.press_r = RC_Ctl.mouse.press_r;
}

void key_rising_check(void)
{
	q_rising_flag = RC_Ctl.key.q - rc_ctrl_last.key.q;
	w_rising_flag = RC_Ctl.key.w - rc_ctrl_last.key.w;
	e_rising_flag = RC_Ctl.key.e - rc_ctrl_last.key.e;
	r_rising_flag = RC_Ctl.key.r - rc_ctrl_last.key.r;
	
	a_rising_flag = RC_Ctl.key.a - rc_ctrl_last.key.a;
	s_rising_flag = RC_Ctl.key.s - rc_ctrl_last.key.s;
	d_rising_flag = RC_Ctl.key.d - rc_ctrl_last.key.d;
	f_rising_flag = RC_Ctl.key.f - rc_ctrl_last.key.f;
	g_rising_flag = RC_Ctl.key.g - rc_ctrl_last.key.g;
	
	z_rising_flag = RC_Ctl.key.z - rc_ctrl_last.key.z;
	x_rising_flag = RC_Ctl.key.x - rc_ctrl_last.key.x;
	c_rising_flag = RC_Ctl.key.c - rc_ctrl_last.key.c;
	v_rising_flag = RC_Ctl.key.v - rc_ctrl_last.key.v;
	b_rising_flag = RC_Ctl.key.b - rc_ctrl_last.key.b;
	
	ctrl_rising_flag = RC_Ctl.key.ctrl - rc_ctrl_last.key.ctrl;
	shift_rising_flag = RC_Ctl.key.shift - rc_ctrl_last.key.shift;
	
	mouse_Press_l_rising_flag = RC_Ctl.mouse.press_l - rc_ctrl_last.mouse.press_l;
	mouse_Press_r_rising_flag = RC_Ctl.mouse.press_r - rc_ctrl_last.mouse.press_r;
}

void Change_Sensitivity()
{
	if(x_rising_flag == 1)//����������
	{
		if(RC_IMU_control == 1)
		{
			yaw_PC_Keyboard_sensitivity += 0.05f;
			pitch_PC_Keyboard_sensitivity += 0.01f;
		}
		if(RC_Motor_control == 1)
		{
			yaw_PC_Keyboard_sensitivity_motor += 0.05f;
			pitch_PC_Keyboard_sensitivity_motor += 0.01f;
		}
	}
	if(c_rising_flag == 1)//��С������
	{
		if(RC_IMU_control == 1)
		{
			yaw_PC_Keyboard_sensitivity -= 0.05f;
			pitch_PC_Keyboard_sensitivity -= 0.01f;
		}
		if(RC_Motor_control == 1)
		{
			yaw_PC_Keyboard_sensitivity_motor -= 0.05f;
			pitch_PC_Keyboard_sensitivity_motor -= 0.01f;
		}
	}
	limit_max_min(&yaw_PC_Keyboard_sensitivity,1.5f,0.0f);
	limit_max_min(&pitch_PC_Keyboard_sensitivity,1.0f,0.0f);
	limit_max_min(&yaw_PC_Keyboard_sensitivity_motor,1.0f,0.0f);
	limit_max_min(&pitch_PC_Keyboard_sensitivity_motor,1.0f,0.0f);
}

void ChangeMode(void)				//�ٿ�ģʽ�������
{

	//PC contral change mode
	{
		//PC contral auto
		if(RC_Ctl.key.ctrl == 1 && LastCtrl == 0)
		{
			if( ContralMode == Pc)//ң�����л�������,��ctrl�л�����
			{
				 ContralMode = Auto;
			}
			else if( ContralMode == Auto)//ң�����л�������,��ctrl�л�����
			{
				 ContralMode = Pc;
			}	 
		}			
		LastCtrl=RC_Ctl.key.ctrl;
	}
	{
//		if(RC_Ctl.key.shift == 1 && LastShift == 0)
//		{
//			if( ContralMode == Pc)
//		}
	}
	//Rc contral change mode
	{
		if((RC_Ctl.rc.s1 == 2)&&(RC_Ctl.rc.s2 != 2))
		{
			ContralMode = Rc;
		}
		if((RC_Ctl.rc.s1 == 3)&&(RC_Ctl.rc.s2 != 2))
		{
			ContralMode = Auto;
		}		
		if(RC_Ctl.rc.s2 == 2)
		{
			ContralMode = Lost;
		}			
		if((RC_Ctl.rc.s1 == 1&&LastRcS1!=1)&&(RC_Ctl.rc.s2 != 2))
		{
			ContralMode = Pc;
		}
		LastRcS1=RC_Ctl.rc.s1;
	}
	if(((ContralMode == Rc) || (ContralMode == Pc))&& (RC_Ctl.rc.s2 == 3))//RCģʽ�������Ƿ�������
	{
		RC_IMU_control =1;
		RC_Motor_control =0;
	}
	if(((ContralMode == Rc) || (ContralMode == Pc)) && (RC_Ctl.rc.s2 == 1))//RCģʽ�µ���Ƿ�������
	{
		RC_IMU_control =0;
		RC_Motor_control =1;
	}
	//if((ContralMode == Rc) || (ContralMode == Pc))
}

void shoot_mode_check(void)
{
	if((ContralMode != Lost))//�Լ�����
	{
		if((RC_Ctl.rc.ch3 - 1024 < 100)&&(RC_Ctl.rc.ch3 - 1024 > -100))
		{
			shoot_mode = stop_shoot;//ͣ��
		}	
		if((RC_Ctl.rc.ch3 - 1024 > 100))
		{
			shoot_mode = single_shoot;//����
		}	
		if((RC_Ctl.rc.ch3 - 1024 < -100))
		{
			shoot_mode = cont_shoot;//����
		}			
	}		
}

void ActionControl_task(void *pvParameters)
{
	while (1) {
		
		key_rising_check();
		key_refresh();
		Change_Sensitivity();
		//if ((droneState.drone.droneState) != 0) //�����ߣ�����������Ϊֹͣ
		if(RC_Ctl.rc.s2 == 2)
		{
			ContralMode = Lost;
			shoot_mode = stop_shoot;
		}
		else
		{
			ChangeMode();
			shoot_mode_check();
		}  
       vTaskDelay(2); 

#if INCLUDE_uxTaskGetStackHighWaterMark
//        ActionControl_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}
