/**
 ******************************************************************************
 * @file    RefereeTask.c
 * @brief   ������ϵͳ����ͼ�λ�������Ϣ,���ղ���ϵͳ����
 ******************************************************************************
 * @attention
 ******************************************************************************
 */
#include "config.h"

/**
 * @brief ͼ�ν������ݷ���(�����ַ��ͽ���)
 * @param[in] void
 */
void Refereetask(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1; // 1kHZ

	while (1)
	{
		Referee_UnpackFifoData();

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
