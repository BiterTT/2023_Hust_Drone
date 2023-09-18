/* include */
#include "config.h"

/* get zerocheck data */
float Get_ZeroCheck_Value_Interface(ZeroCheck_Typedef *zero,float value)
{
	return ZeroCheck_cal(zero,value);//����datareceive�Ľӿں���
}

/* zerocheck init */
void ZeroCheck_Init_Interface(ZeroCheck_Typedef *zeroCheck,float CountCycle,float NowValue)
{
	zeroCheck->ActualValue = NowValue;
	zeroCheck->CountCycle  = CountCycle;
	zeroCheck->LastValue   = 0;
	zeroCheck->PreError    = 0;
	zeroCheck->Circle = 0;
}

/* ��������ֵ���� */
float ResetZeroCheck(ZeroCheck_Typedef *zeroCheck)
{
	float temp = zeroCheck->CountCycle;
	ZeroCheck_Init_Interface(zeroCheck,temp,0);
	return 1;
}

/* λ��ʽ���ٶ�ʽ������ */
/* ʹ�ô˺���ǰҪ������Ӧ������ṹ��� Zero->CountCycle��Zero->LastValue */

/* paramter:	
		ZeroCheck_Typedef *Zero 	������ṹ��
			Zero->ActualValue 	��ʾ�������ǰֵ
			Zero->LastValue 		��ʾ�������һ��ֵ
			Zero->CountCycle 		��ʾ���������ʱԽ��ֵ(����������)
			Zero->PreError 			��ʾ�������ֵ
		float value  							������� */
float ZeroCheck_cal(ZeroCheck_Typedef *Zero,float value)
{
	Zero->ActualValue=value;
	
	Zero->PreError=Zero->ActualValue-Zero->LastValue;
	
	Zero->LastValue=Zero->ActualValue;
	
	if(Zero->PreError>0.7f*Zero->CountCycle)
	{
		Zero->PreError=Zero->PreError-Zero->CountCycle;
		Zero->Circle++;
	}
	
	if(Zero->PreError<-0.7f*Zero->CountCycle)
	{
		Zero->PreError=Zero->PreError+Zero->CountCycle;
		Zero->Circle--;
	}
	
	return Zero->ActualValue - Zero->Circle * Zero->CountCycle;
}


/* end */
















