#ifndef __FEEDFORWARD_H__
#define __FEEDFORWARD_H__
#include "config.h"

typedef struct
{
	float param[3];//ǰ��ϵ��
	float val;
	float val_dot;//һ�ײ��
	float val_ddot;//���ײ��
	
	float output;
	float Max_Out;
	
	
}FeedForward_Typedef;

float FeedForward_Cal(FeedForward_Typedef *ff,float now_val);
void FeedForwardInit(void);

#endif
