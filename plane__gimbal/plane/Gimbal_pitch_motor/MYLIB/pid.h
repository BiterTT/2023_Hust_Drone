#ifndef __pid_H__
#define __pid_H__

#include "config.h"


typedef  struct pid_t//��ͨpid
{
  float p;//��������
  float i;//���ֳ���
  float d;//΢�ֳ���

  float set;//�趨Ŀ��ֵ
  float get;//�������ֵ
  float err[3];//err����,��ǰ���,ǰ�����

  float pout;//�������
  float iout;//�������
  float dout;//΢�����
  float out;

  float input_max_err;    //input max err;
  float output_deadband;  //output deadband; 
  
  uint32_t pid_mode;//pidģʽ��־λ
  uint32_t max_out;//����޷�
  uint32_t integral_limit;//��������

  void (*f_param_init)(struct pid_t *pid, 
                       uint32_t      pid_mode,
                       uint32_t      max_output,
                       uint32_t      inte_limit,
                       float         p,
                       float         i,
                       float         d);
  void (*f_pid_reset)(struct pid_t *pid, float p, float i, float d);
 
} pid_t;


float pid_calc(pid_t *pid, float fdb, float ref);
void PID_struct_init(pid_t* pid,uint32_t mode,uint32_t maxout,uint32_t intergral_limit,float kp,float ki,float kd);
		



/*
-----------------------------------------ģ��pid����
*/	
/*********ģ��pid����*/
typedef struct FuzzyPID
{
		float SetPoint;			//�趨Ŀ��ֵ
	
		float ActualValue;  //ʵ��ֵ

    float DeadZone;			//�������Ʋ���
		
		float LastError;		//ǰ�����
		float PreError;			//��ǰ���
		float SumError;			//�������
	
		float IMax;					//��������
		
		float POut;					//�������
		float IOut;					//�������
		float DOut;					//΢�����
	  float DOut_last;    //��һ��΢�����
		float OutMax;       //�޷�
	  float Out;          //�����
		float Out_last;     //��һ�����
		
		float I_U;          //���ٻ�������
		float I_L;          //���ٻ�������
		
		float RC_DM;        //΢�������˲�ϵ��
		float RC_DF;        //����ȫ΢���˲�ϵ��
	
	  float Kp0;          //PID��ֵ
	  float Ki0;
  	float Kd0;
	
	  float dKp;          //PID�仯��
	  float dKi;
  	float dKd;
	
    float stair ;	      //��̬�����ݶ�   //0.25f
	  float Kp_stair;                      //0.015f
	  float Ki_stair;                      //0.0005f
	  float Kd_stair;                      //0.001f
		
		 void (*fuzzy_param_init)(struct FuzzyPID *   pid,
													uint32_t OutMax,uint32_t IMax,
													float    Kp0,float    Ki0,float    Kd0,
													float		 dKp,float		 dKi,float		 dKd,
													float 	 I_U,float    I_L,float    RC_DM,float 	 RC_DF,	
													float 	 stair,float 	 Kp_stair,float 	 Ki_stair,float 	 Kd_stair);
		void (*fuzzy_pid_reset)(struct FuzzyPID *pid);
	  
} FuzzyPID;
//float FuzzyPID_Calc(FuzzyPID *pid);
float FuzzyPID_Calc(FuzzyPID *P,float SetPoint ,float ActualValue );
void Fuzzy_PID_struct_init(
	FuzzyPID *   pid,
	uint32_t OutMax,
	uint32_t IMax,
	float    Kp0,
	float    Ki0,
	float    Kd0,
	float		 dKp,
	float		 dKi,
	float		 dKd,
	float 	 I_U,
	float    I_L,
	float    RC_DM,
	float 	 RC_DF,	
	float 	 stair,
	float 	 Kp_stair,
	float 	 Ki_stair,
	float 	 Kd_stair);
	
#endif
/* end */		

		
		
		
		
		
		
		
