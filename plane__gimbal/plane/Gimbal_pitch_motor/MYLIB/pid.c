/* include */
#include "config.h"

/* limit */
static void abs_limit(float *a, float ABS_MAX)
{
  if (*a > ABS_MAX)
    *a = ABS_MAX;
  if (*a < -ABS_MAX)
    *a = -ABS_MAX;
}

/*  pid init */
static void pid_param_init(
	pid_t*   pid,
	uint32_t mode,
	uint32_t maxout,
	uint32_t intergral_limit,
	float    kp,
	float    ki,
	float    kd)
{
  pid->integral_limit = intergral_limit;
  pid->max_out        = maxout;
  pid->pid_mode       = mode;

  pid->p = kp;
  pid->i = ki;
  pid->d = kd;
}

/*  pid reset */
static void pid_reset(pid_t *pid, float kp, float ki, float kd)
{
  pid->p = kp;
  pid->i = ki;
  pid->d = kd;
  
  pid->pout = 0;
  pid->iout = 0;
  pid->dout = 0;
  pid->out  = 0;
}

/* pid calculate */
float pid_calc(pid_t *pid, float get, float set)
{
	pid->get = get;//��ȡ��ʵ��ֵ
	pid->set = set;//Ŀ���趨ֵ
	pid->err[0] = set - get;//��ȡ��ǰ���
	pid->input_max_err += pid->err[0];//������+��ǰ���
	
  abs_limit(&pid->input_max_err,pid->integral_limit);	//��������ֲ��ܳ�����������
	
	pid->pout = pid->p * pid->err[0];//����ϵ��
	pid->iout = pid->i * pid->input_max_err;//����ϵ��
	pid->dout = pid->d * (pid->err[0]-pid->err[1]);//΢��ϵ��
	pid->out = pid->pout+pid->iout+pid->dout;
	
	pid->err[1] = pid->err[0];//��ֵ

	abs_limit(&pid->out,pid->max_out);//����޷�
	
	return pid->out;
	
}

/* pid init */
void PID_struct_init(
	pid_t*   pid,
	uint32_t mode,
	uint32_t maxout,
	uint32_t intergral_limit,
	float kp,
	float ki,
	float kd )
{
  pid->f_param_init = pid_param_init;
  pid->f_pid_reset  = pid_reset;

  pid->f_param_init(pid, mode, maxout, intergral_limit, kp, ki, kd);
  pid->f_pid_reset(pid, kp, ki, kd);
}
/*
ģ��pidģ��
*/
/****************************************************************ģ��pid����*/
#define NL   -3
#define NM	 -2
#define NS	 -1
#define ZE	 0
#define PS	 1
#define PM	 2
#define PL	 3
//Kp������� 
static const float fuzzyRuleKp[7][7]={
	PL,	PL,	PM,	PM,	PS,	ZE,	ZE,
	PL,	PL,	PM,	PS,	PS,	ZE,	NS,
	PM,	PM,	PM,	PS,	ZE,	NS,	NS,
	PM,	PM,	PS,	ZE,	NS,	NM,	NM,
	PS,	PS,	ZE,	NS,	NS,	NM,	NM,
	PS,	ZE,	NS,	NM,	NM,	NM,	NL,
	ZE,	ZE,	NM,	NM,	NM,	NL,	NL
};
//Ki������� 
static const float fuzzyRuleKi[7][7]={
	NL,	NL,	NM,	NM,	NS,	ZE,	ZE,
	NL,	NL,	NM,	NS,	NS,	ZE,	ZE,
	NL,	NM,	NS,	NS,	ZE,	PS,	PS,
	NM,	NM,	NS,	ZE,	PS,	PM,	PM,
	NS,	NS,	ZE,	PS,	PS,	PM,	PL,
	ZE,	ZE,	PS,	PS,	PM,	PL,	PL,
	ZE,	ZE,	PS,	PM,	PM,	PL,	PL
};
//Kd������� 
static const float fuzzyRuleKd[7][7]={
	PS,	NS,	NL,	NL,	NL,	NM,	PS,
	PS,	NS,	NL,	NM,	NM,	NS,	ZE,
	ZE,	NS,	NM,	NM,	NS,	NS,	ZE,
	ZE,	NS,	NS,	NS,	NS,	NS,	ZE,
	ZE,	ZE,	ZE,	ZE,	ZE,	ZE,	ZE,
	PL,	NS,	PS,	PS,	PS,	PS,	PL,
	PL,	PM,	PM,	PM,	PS,	PS,	PL
};
 

 //�ؼ��㷨
void fuzzy( FuzzyPID*  fuzzy_PID)//ģ���㷨
{
     float e = fuzzy_PID ->PreError/ fuzzy_PID->stair;//����E��̬�Ƚ�
	   float ec = (fuzzy_PID ->Out - fuzzy_PID ->Out_last) / fuzzy_PID->stair;
     short etemp,ectemp;//����E,EC����ж�ֵ����
     float eLefttemp,ecLefttemp;    //������
     float eRighttemp ,ecRighttemp; 
 
     short eLeftIndex,ecLeftIndex;  //��ǩ
     short eRightIndex,ecRightIndex;

	  //ģ����,�ɴ�С
     if(e>=PL)
			 etemp=PL;//������Χ
		 else if(e>=PM)
			 etemp=PM;
		 else if(e>=PS)
			 etemp=PS;
		 else if(e>=ZE)
			 etemp=ZE;
		 else if(e>=NS)
			 etemp=NS;
		 else if(e>=NM)
			 etemp=NM;
		 else if(e>=NL)
			 etemp=NL;
		 else 
			 etemp=2*NL;
 
		 if( etemp == PL)
		{
		 //����E������
				eRighttemp= 0 ;    //�����
				eLefttemp= 1 ;
			
     //�����ǩ
	   eLeftIndex = 6 ;      
	   eRightIndex= 6 ;
			
		}else if( etemp == 2*NL )
    {

			//����E������
				eRighttemp = 1;    //�����
				eLefttemp = 0;
	
     //�����ǩ
	   eLeftIndex = 0 ;       
	   eRightIndex = 0 ;
			
		}	else 
    {

			//����E������
				eLefttemp=(e-etemp);  //���Ժ�����Ϊ��������
				eRighttemp=(1- eLefttemp);
			
     //�����ǩ
	   eLeftIndex =(short) (etemp-NL);       //���� etemp=2.5��NL=-3����ô�õ������к�Ϊ5  ��0 1 2 3 4 5 6��
	   eRightIndex=(short) (eLeftIndex+1);
			
		}		
	   
		
		 if(ec>=PL)
			 ectemp=PL;
		 else if(ec>=PM)
			 ectemp=PM;
		 else if(ec>=PS)
			 ectemp=PS;
		 else if(ec>=ZE)
			 ectemp=ZE;
		 else if(ec>=NS)
			 ectemp=NS;
		 else if(ec>=NM)
			 ectemp=NM;
		 else if(ec>=NL)
			 ectemp=NL;
		 else 
			 ectemp=2*NL;
		 
	  
   if( ectemp == PL )
	 {
    //����EC������		 
		 ecRighttemp= 0 ;      //�����
		 ecLefttemp= 1 ;
			
		 ecLeftIndex = 6 ;  
	   ecRightIndex = 6 ;	 
	 
	 } else if( ectemp == 2*NL)
	 {
    //����EC������		 
		 ecRighttemp= 1 ;
		 ecLefttemp= 0 ;
			
		 ecLeftIndex = 0 ;  
	   ecRightIndex = 0 ;	 	 
	 }else
	 {
    //����EC������		 
		 ecLefttemp=(ec-ectemp);
		 ecRighttemp=(1- ecLefttemp);
			
		 ecLeftIndex =(short) (ectemp-NL);  
	   ecRightIndex= (short)(eLeftIndex+1);
	 }	

 
/*************************************��ģ��*************************************/
	fuzzy_PID->dKp = fuzzy_PID->Kp_stair * (eLefttemp * ecLefttemp * fuzzyRuleKp[eLeftIndex][ecLeftIndex]                   
   + eLefttemp * ecRighttemp * fuzzyRuleKp[eLeftIndex][ecRightIndex]
   + eRighttemp * ecLefttemp * fuzzyRuleKp[eRightIndex][ecLeftIndex]
   + eRighttemp * ecRighttemp * fuzzyRuleKp[eRightIndex][ecRightIndex]);
 
	fuzzy_PID->dKi = fuzzy_PID->Ki_stair * (eLefttemp * ecLefttemp * fuzzyRuleKi[eLeftIndex][ecLeftIndex]
   + eLefttemp * ecRighttemp * fuzzyRuleKi[eLeftIndex][ecRightIndex]
   + eRighttemp * ecLefttemp * fuzzyRuleKi[eRightIndex][ecLeftIndex]
   + eRighttemp * ecRighttemp * fuzzyRuleKi[eRightIndex][ecRightIndex]);

 
	fuzzy_PID->dKd = fuzzy_PID->Kd_stair * (eLefttemp * ecLefttemp * fuzzyRuleKd[eLeftIndex][ecLeftIndex]
   + eLefttemp * ecRighttemp * fuzzyRuleKd[eLeftIndex][ecRightIndex]
   + eRighttemp * ecLefttemp * fuzzyRuleKd[eRightIndex][ecLeftIndex]
   + eRighttemp * ecRighttemp * fuzzyRuleKd[eRightIndex][ecRightIndex]);
 
}


float FuzzyPID_Calc(FuzzyPID *P,float SetPoint,float ActualValue)//ģ������
{
	
	  P->LastError = P->PreError;
	  
	  if((ABS(P->PreError)< P->DeadZone ))   //��������
		{
			P->PreError = 0.0f;			
		}
		else
		{
			P->PreError = SetPoint - ActualValue;
		}
		
		fuzzy(P);      //ģ������  kp,ki,kd   �β�1��ǰ���β�2ǰ�����Ĳ�ֵ
	
    float Kp = P->Kp0 + P->dKp , Ki = P->Ki0 + P->dKi , Kd = P->Kd0 + P->dKd ;   //PID��ģ��
//	float Kp = P->Kp0 + P->dKp , Ki = P->Ki0  , Kd = P->Kd0 + P->dKd ;           //��PD��ģ��
//	float Kp = P->Kp0 + P->dKp , Ki = P->Ki0  , Kd = P->Kd0 ;                    //��P��ģ��

		
		
		      //΢������
		float DM = Kd*(P->Out - P->Out_last);   //΢������	
         //���ٻ���
    if(ABS(P->PreError) < P->I_L )			
		{
	       //���λ���
		P->SumError += (P->PreError+P->LastError)/2;    
		P->SumError = LIMIT_MAX_MIN(P->SumError,P->IMax,- P->IMax);
		}
		 else if( ABS(P->PreError) < P->I_U )
		{
	       //���λ���
		P->SumError += (P->PreError+P->LastError)/2*(P->PreError - P->I_L)/(P->I_U - P->I_L);    
		P->SumError = LIMIT_MAX_MIN(P->SumError,P->IMax,- P->IMax);		
		}
			
		P->POut = Kp * P->PreError;
		
		P->IOut = Ki * P->SumError;
		    
		    //����ȫ΢��
		P->DOut_last = P->DOut; 
		P->DOut = DM * P->RC_DF + P->DOut_last * ( 1 - P->RC_DF );    
		
		P->Out_last  = P->Out;
		
		P->Out = LIMIT_MAX_MIN(P->POut+P->IOut+P->DOut,P->OutMax,-P->OutMax);
		
    return P->Out;                             

}


//ģ��PID��ʼ������
static void Fuzzy_pid_param_init(
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
	float 	 Kd_stair)
{
  pid->IMax = IMax;
  pid->OutMax = OutMax;

  pid->Kp0 = Kp0;
  pid->Ki0 = Ki0;
  pid->Kd0 = Kd0;
	
	pid->dKp = dKp;
	pid->dKi = dKi;
	pid->dKd = dKd;
	
	pid->I_U = I_U;
	pid->I_L = I_L;
	
	pid->stair = stair;
	pid->Kp_stair = Kp_stair;
	pid->Ki_stair = Ki_stair;
	pid->Kd_stair = Kd_stair;
	
	pid->RC_DF = RC_DF;
	pid->RC_DM = RC_DM;
}
//ģ��PID���ú���
static void Fuzzy_pid_reset(FuzzyPID *pid)
{
  pid->dKp = 0;
  pid->dKi = 0;
  pid->dKd = 0;
  
  pid->POut = 0;
  pid->IOut = 0;
  pid->DOut = 0;
  pid->Out  = 0;
	pid->Out_last = 0;
}
//ģ��PID�ṹ���ʼ����
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
	float 	 Kd_stair)
{
  pid->fuzzy_param_init = Fuzzy_pid_param_init;
  pid->fuzzy_pid_reset  = Fuzzy_pid_reset;

  pid->fuzzy_param_init(pid ,OutMax ,IMax ,Kp0 ,Ki0 ,Kd0 ,dKp ,dKi ,dKd ,I_U ,I_L ,RC_DM ,RC_DF ,stair ,Kp_stair ,Ki_stair ,Kd_stair);
  pid->fuzzy_pid_reset(pid);
}
/* end */















