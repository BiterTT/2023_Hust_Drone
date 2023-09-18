/**********************************************************************************************************
 * @�ļ�     FeedForward.c
 * @˵��     ǰ������
 * @�汾  	 V1.0
 * @����     ����
 * @����     2023.3
**********************************************************************************************************/
#include "config.h"
extern struct GM6020 yaw;
extern struct GM6020 pitch;
/**********************************************************************************************************
*�� �� ��: FeedForward_Cal
*����˵��: ǰ������
*��    ��: ǰ���ṹ�壬 ��ǰֵ
*�� �� ֵ: ���
**********************************************************************************************************/
float FeedForward_Cal(FeedForward_Typedef *ff , float now_val)
{
	ff->val_ddot = (now_val - ff->val) - ff->val_dot;//x(k)-2x(k-1)-x(k-2)
	ff->val_dot = now_val - ff->val;//x(k)-x(k-1)
	ff->val = now_val;

	ff->output = ff->val * ff->param[0] + ff->val_dot * ff->param[1] + ff->val_ddot * ff->param[2];
	
	return LIMIT_MAX_MIN(ff->output, ff->Max_Out, -ff->Max_Out);
}
/**********************************************************************************************************
*�� �� ��: FeedForwardInit
*����˵��: ǰ��ϵ����ʼ��
*��    ��: ��
*�� �� ֵ: ��
**********************************************************************************************************/
void FeedForwardInit()
{
	yaw.AngelFF.param[0] = 0;
	yaw.AngelFF.param[1] = 0;
	yaw.AngelFF.param[2] = 0;
	yaw.AngelFF.val = 0;
	yaw.AngelFF.val_dot = 0;
	yaw.AngelFF.val_ddot = 0;
	yaw.AngelFF.Max_Out = 50;
	
	yaw.SpeedFF.param[0] = 0;
	yaw.SpeedFF.param[1] = 0;
	yaw.SpeedFF.param[2] = 0;
	yaw.SpeedFF.val = 0;
	yaw.SpeedFF.val_dot = 0;
	yaw.SpeedFF.val_ddot = 0;
	yaw.SpeedFF.Max_Out = 0;

	pitch.AngelFF.param[0] = 0;
	pitch.AngelFF.param[1] = 0;
	pitch.AngelFF.param[2] = 0;
	pitch.AngelFF.val = 0;
	pitch.AngelFF.val_dot = 0;
	pitch.AngelFF.val_ddot = 0;
	pitch.AngelFF.Max_Out = 50;

	pitch.SpeedFF.param[0] = 0;
	pitch.SpeedFF.param[1] = 0;
	pitch.SpeedFF.param[2] = 0;
	pitch.SpeedFF.val = 0;
	pitch.SpeedFF.val_dot = 0;
	pitch.SpeedFF.val_ddot = 0;
	pitch.SpeedFF.Max_Out = 0;
	
	
}
