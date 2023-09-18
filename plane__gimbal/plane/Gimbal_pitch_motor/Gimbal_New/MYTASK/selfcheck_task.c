/* include */
#include "config.h"

/* define */
#define IMU_GIMBAL_TASK_PRIO		5
#define IMU_GIMBAL_TASK_SIZE		128
TaskHandle_t ImugimbalTask_Handler;

#define SHOOT_TASK_PRIO		5
#define SHOOT_TASK_SIZE		128
TaskHandle_t ShootTask_Handler;

struct state droneState;

/* declare */
extern RC_Ctl_t RC_Ctl; 
extern PC_Recv_t PC_Recv;
extern struct GM6020 yaw;
extern struct GM6020 pitch;
extern struct M2006  pluck;
extern struct IMU 	 imu;

static void selfCheck_update(void);
static void led_update(void);
static void control(void );
static uint8_t dis_control(void);
static uint16_t checkComponent(uint32_t now,uint32_t last,uint32_t delay);
static void device_lost_reset(void);


/* ��ʼ��ʱ���ȿ����Լ� Ȼ��300ms���һ���źŸ���(�����Լ�) */
void SelfcheckTask( void * pvParameters )
{
	static uint32_t initTime;
	static short dis_control_flag = 1;
	initTime = xTaskGetTickCount();
	
	/* ����LED������ʾ�Լ������ */
	led_selfCheck();
	IWDG_Config(64,1300);
	/* �Լ�300ms */
	while(xTaskGetTickCount()-initTime<800)
	{
		/* �����Լ����� */
		selfCheck_update();
		IWDG_Feed();
		vTaskDelay(1);
	}
	
	/* �����Լ����� ��λ��ʷ���߼�¼ */
	if(droneState.drone.droneState == 0)
		device_lost_reset();
	
	/* �����豸���� �� ϵͳ���÷��������Լ� */
	if(droneState.drone.droneState == 0||SELFCHECK == 0)
	{
		/* �������� ������̨��������� */
		control();
		
		dis_control_flag = 0;
	}

	/* �����Լ��������� */
	for(;;)
	{
		/* �������Լ�Ƶ��100ms */
		selfCheck_update();
		
		/* LED״̬���� */
		led_update();
		
		/* �������Լ�������ָ���̨���� */
		/* (�����豸���� �� �ϳ�״̬) �� ��ǰδ��ÿ���Ȩ */ 
		if((droneState.drone.droneState == 0)&&(dis_control_flag == 1))
		{
			/* ��ǰ�Լ����� �ָ��������� */		
			control();
			
			dis_control_flag = 0;
		}
		
		/* �������豸���߷�����̨���� */
		/* �������������豸����(����״̬) */
		else if (droneState.drone.droneState != 0 
					&& dis_control_flag == 0
					&& SELFCHECK == 1)
		{
			if(dis_control())
				dis_control_flag = 1;
		}
		IWDG_Feed();
		vTaskDelay(100);
	}
}

/* ���������������� */
static void control(void)
{
	/* IMU�Լ�ͨ�� */
	if(droneState.drone.composeState.IMU_disconneced == 0||SELFCHECK == 0)
	{
		/* ����IMU��̨���� */
		xTaskCreate((TaskFunction_t)ImugimbalTask,      	
								(const char *	)"Imu cotrol gimbal",     
								(uint16_t			)IMU_GIMBAL_TASK_SIZE,    
								(void *				)NULL,              
								(UBaseType_t		)IMU_GIMBAL_TASK_PRIO,    
								(TaskHandle_t *)&ImugimbalTask_Handler);
	}
	
	xTaskCreate((TaskFunction_t)ShootTask,      	
							(const char *	)"shoot",     
							(uint16_t			)SHOOT_TASK_SIZE,    
							(void *				)NULL,              
							(UBaseType_t		)SHOOT_TASK_PRIO,    
							(TaskHandle_t *)&ShootTask_Handler);
}

short temp_disconnetTimes = 0; 
/* ��������Ȩ */
static uint8_t dis_control(void)
{
	/* �����������ǰ���� */
	if(shoot_ProTerminate())
	{
		/* ��������Ȩ */
		temp_disconnetTimes++;
		if(droneState.drone.composeState.IMU_disconneced == 0)
			vTaskDelete(ImugimbalTask);
		
		vTaskDelete(ShootTask);
		return 1;
	}
	else 
		return 0;
}

/* LED״̬���� */
static void led_update(void)
{
	switch (droneState.drone.droneState)
	{
		/* �޵��� */
		case 0x0000:
		{
			/* ����״̬: �������� */
			if((droneState.Gimbal_poweroff == 0)&&(droneState.AimAssist == 0))
				led_normal();
			/* ����״̬: ����˫�� */
			else if((droneState.Gimbal_poweroff == 0)&&(droneState.AimAssist == 1))
				led_AimAssist();
			/* ��̨����: ���Ƴ��� */
			else if(droneState.Gimbal_poweroff == 1)
				led_Gimbal_powerOff();
			break;
		}
		/* IMU���� */
		case 0x0001:led_IMU_Lost();
			break;
		/* ң���źŶ�ʧ */
		case 0x0004:led_RC_Lost(); 
			break;
		/* YAW�������� */
		case 0x0010:led_YAW_Lost();
			break;
		/* Pitch�������� */
		case 0x0040:led_Pitch_Lost();
			break;
		/* ����������� */
		case 0x0100:led_Pluck_Lost();
			break;
		/* ���ش���: ��Ƴ���*/
		default :led_error();
			break;
	}
}

/* �Լ����ݸ��� */
static void selfCheck_update(void)
{
	/* ����ʱ�� */
	uint32_t nowTime = xTaskGetTickCount();
	/* ����IMU״̬ */
	droneState.drone.composeState.IMU_disconneced = checkComponent(nowTime,imu.updateTime,100);
	/* ����YAW����״̬ */
	droneState.drone.composeState.yaw_disconneced = checkComponent(nowTime,yaw.updateTime,100);
	/* ����PITCH����״̬ */
	droneState.drone.composeState.pitch_disconneced = checkComponent(nowTime,pitch.updateTime,100);
	/* ����RC״̬ */
	droneState.drone.composeState.RC_disconneced = checkComponent(nowTime,RC_Ctl.updateTime,1000);
	/* ���²������״̬ */
	droneState.drone.composeState.Pluck_disconneced = checkComponent(nowTime,pluck.updateTime,100);
	/* ������̨�ϵ�״̬ */
	if(RC_Ctl.rc.s1 == 2)
		droneState.Gimbal_poweroff = 1;
	else
		droneState.Gimbal_poweroff = 0;
	/* ���¸��鿪��״̬ */
	
	/* ����PC���ݽ���״̬ */
	droneState.PC_Received = checkComponent(nowTime,PC_Recv.updateTime,200);
	
	/* ��ʷ���߼�¼ */
	if(droneState.drone.composeState.IMU_disconneced == 1)
		imu.Ever_Lost = 1;
	if(droneState.drone.composeState.yaw_disconneced == 1)
		yaw.Ever_Lost = 1;
	if(droneState.drone.composeState.pitch_disconneced == 1)
		pitch.Ever_Lost = 1;
	if(droneState.drone.composeState.RC_disconneced == 1)
		RC_Ctl.Ever_Lost = 1;
	if(droneState.drone.composeState.Pluck_disconneced == 1)
		pluck.Ever_Lost = 1;
}

/* ģ���Լ� */
/* �Լ쳬ʱ -> ����1 */
/* �Լ����� -> ����0 */
static uint16_t checkComponent(uint32_t now,uint32_t last,uint32_t delay)
{
	if((now-last)>delay)
	return 1;
	else 
	return 0;
}

static void device_lost_reset(void)
{
	imu.Ever_Lost = 1;
	yaw.Ever_Lost = 1;
	pitch.Ever_Lost = 1;
	RC_Ctl.Ever_Lost = 1;
	pluck.Ever_Lost = 1;
}

/* end */

