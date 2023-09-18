#ifndef __SELFCHECK__
#define __SELFCHECK__

#include "config.h"


/* λ���÷�: Ϊ�˽�ʡ�ڴ�ռ� ���Խ�һ���������Ͳ�ֳ�λ��ʹ�� */
/* ð�ź����ʾ��λ����ռ��λ�� ����:ÿ��λ�򳤶�Ϊ2Bit */
/* ע��: λ�����洢��ͬһ�������� ���ܿ����� ͬʱҲ˵��λ��ĳ��Ȳ��ᳬ�����������͵ĳ���
   ���һ���������͵�Ԫ����ʣ�ռ��޷������һ���� ����һ����Ӧ�ô���һ��Ԫ��ʼ��� */
struct compose {
	/* ע�����ȶ����λ�������ݵ�ĩβ(���Ӻ���ǰ���) */
	unsigned int IMU_disconneced 			: 2; //�������:0x0001
	unsigned int RC_disconneced 			: 2; //�������:0x0040

	unsigned int yaw_disconneced 			: 2; 
	unsigned int pitch_disconneced 		: 2;
    
	unsigned int Pluck_disconneced 		: 2;
	
} __attribute__ ((__packed__));

typedef struct state
{
	/* union�е�����ָ��ͬһ����ַ */
	union
	{
		unsigned short droneState;
		struct compose composeState;
	}drone;
	
	uint8_t Gimbal_poweroff;
	uint8_t AimAssist;
	uint8_t PC_Received;
	
}state;


void SelfcheckTask( void * pvParameters );


#endif


/* end */
















