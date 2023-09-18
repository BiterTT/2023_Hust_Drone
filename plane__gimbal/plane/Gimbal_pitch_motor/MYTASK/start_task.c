/* include */
#include "config.h"

/* define */


#define START_TASK_PRIO		4
#define START_TASK_SIZE		128
TaskHandle_t StartTask_Handler;

#define SELFCHECK_TASK_PRIO		4
#define SELFCHECK_TASK_SIZE		128
//TaskHandle_t SelfcheckTask_Handler;

#define DEBUG_TASK_PRIO		1
#define DEBUG_TASK_SIZE		128
//TaskHandle_t DebugTask_Handler;

#define JUDGE_TASK_PRIO		1
#define JUDGE_TASK_SIZE		128
//TaskHandle_t JudgeTask_Handler;

#define GRAPHICS_SEND_TASK_PRIO		1
#define GRAPHICS_SEND_TASK_SIZE		128
//TaskHandle_t GraphicSendtask_Handler;

/* define */
#define IMU_GIMBAL_TASK_PRIO		5
#define IMU_GIMBAL_TASK_SIZE		128
//TaskHandle_t ImugimbalTask_Handler;

#define SHOOT_TASK_PRIO		5
#define SHOOT_TASK_SIZE		128
//TaskHandle_t ShootTask_Handler;

// CPUռ�������������
#define CPU_TASK_PRIO 2       //�������ȼ�
#define CPU_TASK_STK_SIZE 128 // �����ջ
//TaskHandle_t CPU_TASK_Handler;

#define CONTROL_TASK_PRIO 4  //�������ȼ�
#define CONTROL_STK_SIZE 256 //�����ջ
//TaskHandle_t CONTROLTask_Handler; //������
#define INS_TASK_PRIO 14  //�������ȼ�
#define INS_TASK_STK_SIZE 128 //�����ջ

TaskHandle_t User_Tasks[TASK_NUM];

void StartTask( void * pvParameters )
{
	taskENTER_CRITICAL();
	/* create selfcheck task */
	xTaskCreate(( TaskFunction_t	)SelfcheckTask,      	
							( const char *		)"Selfcheck Task",     
							( uint16_t			)SELFCHECK_TASK_SIZE,    
							( void *			)NULL,              
							( UBaseType_t		)SELFCHECK_TASK_PRIO,    
							( TaskHandle_t *	)&User_Tasks[SELFCHECK_TASK]);  //�Լ�����
							
	/* create debug task */
//	xTaskCreate(( TaskFunction_t	)DebugTask,      	
//							( const char *		)"Debug Task",     
//							( uint16_t			)DEBUG_TASK_SIZE,    
//							( void *			)NULL,              
//							( UBaseType_t		)DEBUG_TASK_PRIO,    
//							( TaskHandle_t *	)&User_Tasks[DEBUG_TASK]);   //��������(����)
//							
	/* create judge task */
	xTaskCreate(( TaskFunction_t	)JudgeTask,      	
							( const char *		)"Judge Task",     
							( uint16_t			)JUDGE_TASK_SIZE,    
							( void *			)NULL,              
							( UBaseType_t		)JUDGE_TASK_PRIO,    
							( TaskHandle_t *	)&User_Tasks[JUDGE_TASK]);    //��������
	
	/* create graphics task*/
		xTaskCreate(( TaskFunction_t	)GraphicSendtask,      	
							( const char *		)"Graphic Sendtask",     
							( uint16_t			)JUDGE_TASK_SIZE,    
							( void *			)NULL,              
							( UBaseType_t		)JUDGE_TASK_PRIO,    
							( TaskHandle_t *	)&User_Tasks[GRAPHIC_SEND_TASK]);			 //ͼ������			
							
		/* ����IMU��̨���� */
		xTaskCreate((TaskFunction_t)ImugimbalTask,      	
								(const char *	)"Imu cotrol gimbal",     
								(uint16_t			)IMU_GIMBAL_TASK_SIZE,    
								(void *				)NULL,              
								(UBaseType_t		)IMU_GIMBAL_TASK_PRIO,    
								(TaskHandle_t *)&User_Tasks[IMUGIMBAL_TASK]);         //��̨����
		/* ����������� */								
		xTaskCreate((TaskFunction_t)ShootTask,      	
							(const char *	)"shoot",     
							(uint16_t			)SHOOT_TASK_SIZE,    
							(void *				)NULL,              
							(UBaseType_t		)SHOOT_TASK_PRIO,    
							(TaskHandle_t *)&User_Tasks[SHOOT_TASK]);							//�������
		/* ��������ģʽ���� */								
    xTaskCreate((TaskFunction_t)ActionControl_task,         
                (const char *)"ActionControl_task",          
                (uint16_t)CONTROL_STK_SIZE,            
                (void *)NULL,                        
                (UBaseType_t)CONTROL_TASK_PRIO,        
                (TaskHandle_t *)&User_Tasks[CONTROL_TASK]); 			//��������
								
//	xTaskCreate((TaskFunction_t)INS_Task,          //������
//                (const char *)"INS_Task",          //��������
//                (uint16_t)INS_TASK_STK_SIZE,            //�����ջ��С
//                (void *)NULL,                        //���ݸ��������Ĳ���
//                (UBaseType_t)INS_TASK_PRIO,        //�������ȼ�
// 								(TaskHandle_t *)&User_Tasks[INS_TASK]); //������		
//#ifdef DEBUG_MODE_FREERTOS
//    xTaskCreate((TaskFunction_t)CPU_task,               //������
//                (const char *)"CPU_task",               //��������
//                (uint16_t)CPU_TASK_STK_SIZE,            //�����ջ��С
//                (void *)NULL,                           //���ݸ��������Ĳ���
//                (UBaseType_t)CPU_TASK_PRIO,             //�������ȼ�
//                (TaskHandle_t *)&User_Tasks[CPU_TASK]); //������
//#endif // DEBUG_MODE_FREERTOS								
	/* delete start task */
	vTaskDelete( StartTask_Handler );
	taskEXIT_CRITICAL();            //�˳��ٽ���
}

/* create start task */
void vCreateStartTask( void )
{
	//��ʼ����
	xTaskCreate(( TaskFunction_t	)StartTask,      	 			//������
							( const char *		)"SystemStart",     		//��������
							( uint16_t			)START_TASK_SIZE,    			//�����ջ��С
							( void *			)NULL,              				//���ݸ��������Ĳ���
							( UBaseType_t		)START_TASK_PRIO,   			//�������ȼ�
							( TaskHandle_t *	)&StartTask_Handler);		//������	
}


/* end */














