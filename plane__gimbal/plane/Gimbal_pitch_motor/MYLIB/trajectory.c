///* include */
//#include "config.h"

///* define */
//struct trajectory_t trajectory;

///* declare */
//extern PC_Recv_t PC_Recv;
//extern RC_Ctl_t RC_Ctl;

//float Iterate_PitchPos = 0.0f;
//float Iterate_estimatehigh = 0.0f;
//float Iterate_delta_theta = 0.0f;
//float final_PitchPos = 0.0f;
///* trajectory estimation (considering g onlg) */
//void trajectory_equation(void)
//{
//	if(TRAJECTORY)
//	{
//		uint8_t num = 0;

//		/* ԭ���� && trajectory.finish_flag == 0 */
//		if(RC_Ctl.key.x == 1)
//		{
//			/* Ŀ��ľ��ԽǶȴ�PC��ȡ */
//			trajectory.pcpitch = PC_Recv.PCTargetPitch;
//			/* �ܷ�Ӳ���ϵͳ�������� */
//			trajectory.realspeed = 30.0f;
//			/* ��������ȡ�߶�(��������ֵ�为 ��ʾ���ڷ���߶�) */
//			trajectory.realheight = -2.0f;
//			
//			trajectory.targetheight = trajectory.realheight + real_target_heightdiff;
//			trajectory.realdistance = ABS(trajectory.targetheight/sin(trajectory.pcpitch));
//			Iterate_PitchPos = trajectory.pcpitch;
//			
//			/* iterate to gain on */
//			for(num=0;num<5;num++)
//			{
//				trajectory.glidetime = trajectory.realdistance/trajectory.realspeed;
//				trajectory.glideheight = 0.5*gravity*trajectory.glidetime*trajectory.glidetime;
//				Iterate_estimatehigh = trajectory.realspeed*sin(Iterate_PitchPos)*trajectory.glidetime - trajectory.glideheight;
//				Iterate_delta_theta = (Iterate_estimatehigh - trajectory.targetheight)*0.1f;
//				Iterate_PitchPos = Iterate_PitchPos - Iterate_delta_theta;
//			}
//			
//			final_PitchPos = Iterate_PitchPos*180/3.14f;
//			trajectory.setpitch = final_PitchPos;
//			
//			trajectory.finish_flag = 1;
//		}
//		/* ԭ���� && trajectory.finishflag == 1 */
//		else if(RC_Ctl.key.x == 0 && ((trajectory.finish_flag==1)||(final_PitchPos!=0.0f)))
//		{
//			final_PitchPos = 0;
//			trajectory.finish_flag = 0;
//		}
//	}
//}


///* end */
