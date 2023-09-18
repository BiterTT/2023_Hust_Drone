/**
 ******************************************************************************
 * @file    ins.c
 * @author  Wang Hongxi
 * @version V2.0.0
 * @date    2022/2/23
 * @brief
 ******************************************************************************
 * @attention
 * copied from https://github.com/WangHongxi2001/RoboMaster-C-Board-INS-Example
 ******************************************************************************
 */
#include "ins.h"
#include "QuaternionEKF.h"
#include "arm_math.h"
INS_t INS;
IMU_Param_t IMU_Param;

const float xb[3] = {1, 0, 0};
const float yb[3] = {0, 1, 0};
const float zb[3] = {0, 0, 1};

uint32_t INS_Count = 0;
float dt = 0, t = 0;
uint8_t ins_debug_mode = 0;
float RefTemp = 40;
static void IMU_Param_Correction(IMU_Param_t *param, float gyro[3], float accel[3]);


void INS_Init(void)
{
    IMU_Param.scale[X] = 1;
    IMU_Param.scale[Y] = 1;
    IMU_Param.scale[Z] = 1;
    IMU_Param.Yaw = 0;
    IMU_Param.Pitch = 0;
    IMU_Param.Roll = 0;
    IMU_Param.flag = 1;

    IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0);

    INS.AccelLPF = 0.0085;
}

void INS_DtatHandle(IMU *IMUReceive, IMU_Data_t *icm_20602)
{
  	static uint32_t count = 0;
    const float gravity[3] = {0.0f, 0.0f, GRAVITY};
    dt = GetDeltaT(&INS_Count);
    t += dt;

    // ins update
    if ((count % 1) == 0)
    {
        ICM_20602_Decode(IMUReceive, icm_20602);//数据转换IMUReceive到icm_20602

        for (int16_t index = 0; index < 3; index++)
        {
            icm_20602->Accel[index] *= icm_20602->AccelScale;
            icm_20602->Gyro[index] -= icm_20602->GyroOffset[index];
        }

        INS.Accel[X] = icm_20602->Accel[X];
        INS.Accel[Y] = icm_20602->Accel[Y];
        INS.Accel[Z] = icm_20602->Accel[Z];
        INS.Gyro[X] = icm_20602->Gyro[X];
        INS.Gyro[Y] = icm_20602->Gyro[Y];
        INS.Gyro[Z] = icm_20602->Gyro[Z];

        // demo function,用于修正安装误差,可以不管,本demo暂时没用
        IMU_Param_Correction(&IMU_Param, INS.Gyro, INS.Accel);

        // 计算重力加速度矢量和b系的XY两轴的夹角,可用作功能扩展,本demo暂时没用
        INS.atanxz = -atan2f(INS.Accel[X], INS.Accel[Z]) * 180 / PI;
        INS.atanyz = atan2f(INS.Accel[Y], INS.Accel[Z]) * 180 / PI;

        // 核心函数,EKF更新四元数
        IMU_QuaternionEKF_Update(INS.Gyro[X], INS.Gyro[Y], INS.Gyro[Z], INS.Accel[X], INS.Accel[Y], INS.Accel[Z], dt);

        memcpy(INS.q, QEKF_INS.q, sizeof(QEKF_INS.q));

        // 机体系基向量转换到导航坐标系，本例选取惯性系为导航系
        BodyFrameToEarthFrame(xb, INS.xn, INS.q);
        BodyFrameToEarthFrame(yb, INS.yn, INS.q);
        BodyFrameToEarthFrame(zb, INS.zn, INS.q);

        // 将重力从导航坐标系n转换到机体系b,随后根据加速度计数据计算运动加速度
        float gravity_b[3];
        EarthFrameToBodyFrame(gravity, gravity_b, INS.q);
        for (uint8_t i = 0; i < 3; i++) // 同样过一个低通滤波
        {
            INS.MotionAccel_b[i] = (INS.Accel[i] - gravity_b[i]) * dt / (INS.AccelLPF + dt) + INS.MotionAccel_b[i] * INS.AccelLPF / (INS.AccelLPF + dt);
        }
        BodyFrameToEarthFrame(INS.MotionAccel_b, INS.MotionAccel_n, INS.q); // 转换回导航系n

        // 获取最终数据
        INS.Yaw = QEKF_INS.Yaw;
        INS.Pitch = QEKF_INS.Pitch;
        INS.Roll = QEKF_INS.Roll;
        INS.YawTotalAngle = QEKF_INS.YawTotalAngle;
				
				INS.Pitch_Speed = (INS.Pitch - INS.Pitch_Last)/dt;
				INS.Yaw_Speed = (INS.YawTotalAngle - INS.Yaw_Last)/dt;
				INS.Roll_Speed = (INS.Roll - INS.Roll_Last)/dt;
				
				INS.Pitch_Last = INS.Pitch;
				INS.Yaw_Last = INS.YawTotalAngle;
				INS.Roll_Last = INS.Roll;
				
		//此处roll和pitch对调
//		GimbalPos.GZ = (INS.YawTotalAngle - GimbalPos.Yaw_w) / dt;
//		GimbalPos.GY = (- INS.Roll - GimbalPos.Pitch_w) / dt;
//		GimbalPos.Roll_w = INS.Pitch;
//		GimbalPos.Pitch_w = - INS.Roll;
//		GimbalPos.Yaw_w = INS.YawTotalAngle;//过零检测后的Yaw轴
//		
////    pure_gyro_roll  += (icm_gyro[0])	* T / 3.1415926f * 180.0f;
//		pure_gyro_pitch += (icm_gyro[1]) * T / 3.1415926f * 180.0f;
//     pure_acc_pitch = -atan2((double)icm_accel[0], sqrt(icm_accel[2]*icm_accel[2]+icm_accel[1]*icm_accel[1])) / 3.1415926f * 180.0f;
//     pure_acc_roll  =  atan2((double)icm_accel[1], sqrt(icm_accel[2]*icm_accel[2]+icm_accel[0]*icm_accel[0])) / 3.1415926f * 180.0f;
//     yaw_w_cal= (icm_gyro[2]) *cos(combAngle_pitch*3.1415926f/180.0f)* cos(combAngle_roll*3.1415926f/180.0f) + (icm_gyro[0]) * (-sin(combAngle_pitch*3.1415926f/180.0f)) + \
//                          (icm_gyro[1]) * sin(combAngle_roll*3.1415926f/180.0f)*cos(combAngle_pitch*3.1415926f/180.0f);


//		GimbalPos.GY = GimbalPos.GY * GimbalPos.IndexGY + (1 - GimbalPos.IndexGY) * GimbalPos.lastGY ;
//		GimbalPos.GZ = GimbalPos.GZ * GimbalPos.IndexGZ + (1 - GimbalPos.IndexGZ) * GimbalPos.lastGZ ;

//		GimbalPos.lastGY = GimbalPos.GY ;
//		GimbalPos.lastGZ = GimbalPos.GZ ;
    
	}

    // temperature control
    if ((count % 2) == 0)
    {
        // 500hz
    }

    if ((count % 1000) == 0)
    {
        // 200hz
    }

    count++;
}

/**
 * @brief          Transform 3dvector from BodyFrame to EarthFrame
 * @param[1]       vector in BodyFrame
 * @param[2]       vector in EarthFrame
 * @param[3]       quaternion
 */
void BodyFrameToEarthFrame(const float *vecBF, float *vecEF, float *q)
{
    vecEF[0] = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecBF[0] +
                       (q[1] * q[2] - q[0] * q[3]) * vecBF[1] +
                       (q[1] * q[3] + q[0] * q[2]) * vecBF[2]);

    vecEF[1] = 2.0f * ((q[1] * q[2] + q[0] * q[3]) * vecBF[0] +
                       (0.5f - q[1] * q[1] - q[3] * q[3]) * vecBF[1] +
                       (q[2] * q[3] - q[0] * q[1]) * vecBF[2]);

    vecEF[2] = 2.0f * ((q[1] * q[3] - q[0] * q[2]) * vecBF[0] +
                       (q[2] * q[3] + q[0] * q[1]) * vecBF[1] +
                       (0.5f - q[1] * q[1] - q[2] * q[2]) * vecBF[2]);
}

/**
 * @brief          Transform 3dvector from EarthFrame to BodyFrame
 * @param[1]       vector in EarthFrame
 * @param[2]       vector in BodyFrame
 * @param[3]       quaternion
 */
void EarthFrameToBodyFrame(const float *vecEF, float *vecBF, float *q)
{
    vecBF[0] = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecEF[0] +
                       (q[1] * q[2] + q[0] * q[3]) * vecEF[1] +
                       (q[1] * q[3] - q[0] * q[2]) * vecEF[2]);

    vecBF[1] = 2.0f * ((q[1] * q[2] - q[0] * q[3]) * vecEF[0] +
                       (0.5f - q[1] * q[1] - q[3] * q[3]) * vecEF[1] +
                       (q[2] * q[3] + q[0] * q[1]) * vecEF[2]);

    vecBF[2] = 2.0f * ((q[1] * q[3] + q[0] * q[2]) * vecEF[0] +
                       (q[2] * q[3] - q[0] * q[1]) * vecEF[1] +
                       (0.5f - q[1] * q[1] - q[2] * q[2]) * vecEF[2]);
}

/**
 * @brief reserved.用于修正IMU安装误差与标度因数误差,即陀螺仪轴和云台轴的安装偏移
 *
 *
 * @param param IMU参数
 * @param gyro  角速度
 * @param accel 加速度
 */
static void IMU_Param_Correction(IMU_Param_t *param, float gyro[3], float accel[3])
{
    static float lastYawOffset, lastPitchOffset, lastRollOffset;
    static float c_11, c_12, c_13, c_21, c_22, c_23, c_31, c_32, c_33;
    float cosPitch, cosYaw, cosRoll, sinPitch, sinYaw, sinRoll;

    if (fabsf(param->Yaw - lastYawOffset) > 0.001f ||
        fabsf(param->Pitch - lastPitchOffset) > 0.001f ||
        fabsf(param->Roll - lastRollOffset) > 0.001f || param->flag)
    {
        cosYaw = arm_cos_f32(param->Yaw / RAD_TO_ANGLE_COEF);
        cosPitch = arm_cos_f32(param->Pitch / RAD_TO_ANGLE_COEF);
        cosRoll = arm_cos_f32(param->Roll / RAD_TO_ANGLE_COEF);
        sinYaw = arm_sin_f32(param->Yaw / RAD_TO_ANGLE_COEF);
        sinPitch = arm_sin_f32(param->Pitch / RAD_TO_ANGLE_COEF);
        sinRoll = arm_sin_f32(param->Roll / RAD_TO_ANGLE_COEF);

        // 1.yaw(alpha) 2.pitch(beta) 3.roll(gamma)
        c_11 = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
        c_12 = cosPitch * sinYaw;
        c_13 = cosYaw * sinRoll - cosRoll * sinYaw * sinPitch;
        c_21 = cosYaw * sinPitch * sinRoll - cosRoll * sinYaw;
        c_22 = cosYaw * cosPitch;
        c_23 = -sinYaw * sinRoll - cosYaw * cosRoll * sinPitch;
        c_31 = -cosPitch * sinRoll;
        c_32 = sinPitch;
        c_33 = cosPitch * cosRoll;
        param->flag = 0;
    }
    float gyro_temp[3];
    for (uint8_t i = 0; i < 3; i++)
        gyro_temp[i] = gyro[i] * param->scale[i];

    gyro[X] = c_11 * gyro_temp[X] +
              c_12 * gyro_temp[Y] +
              c_13 * gyro_temp[Z];
    gyro[Y] = c_21 * gyro_temp[X] +
              c_22 * gyro_temp[Y] +
              c_23 * gyro_temp[Z];
    gyro[Z] = c_31 * gyro_temp[X] +
              c_32 * gyro_temp[Y] +
              c_33 * gyro_temp[Z];

    float accel_temp[3];
    for (uint8_t i = 0; i < 3; i++)
        accel_temp[i] = accel[i];

    accel[X] = c_11 * accel_temp[X] +
               c_12 * accel_temp[Y] +
               c_13 * accel_temp[Z];
    accel[Y] = c_21 * accel_temp[X] +
               c_22 * accel_temp[Y] +
               c_23 * accel_temp[Z];
    accel[Z] = c_31 * accel_temp[X] +
               c_32 * accel_temp[Y] +
               c_33 * accel_temp[Z];

    lastYawOffset = param->Yaw;
    lastPitchOffset = param->Pitch;
    lastRollOffset = param->Roll;
}

//------------------------------------functions below are not used in this demo-------------------------------------------------
//----------------------------------you can read them for learning or programming-----------------------------------------------
//----------------------------------they could also be helpful for further design-----------------------------------------------

/**
 * @brief        Update quaternion
 */
void QuaternionUpdate(float *q, float gx, float gy, float gz, float dt)
{
    float qa, qb, qc;

    gx *= 0.5f * dt;
    gy *= 0.5f * dt;
    gz *= 0.5f * dt;
    qa = q[0];
    qb = q[1];
    qc = q[2];
    q[0] += (-qb * gx - qc * gy - q[3] * gz);
    q[1] += (qa * gx + qc * gz - q[3] * gy);
    q[2] += (qa * gy - qb * gz + q[3] * gx);
    q[3] += (qa * gz + qb * gy - qc * gx);
}

/**
 * @brief        Convert quaternion to eular angle
 */
void QuaternionToEularAngle(float *q, float *Yaw, float *Pitch, float *Roll)
{
    *Yaw = atan2f(2.0f * (q[0] * q[3] + q[1] * q[2]), 2.0f * (q[0] * q[0] + q[1] * q[1]) - 1.0f) * RAD_TO_ANGLE_COEF;
    *Pitch = atan2f(2.0f * (q[0] * q[1] + q[2] * q[3]), 2.0f * (q[0] * q[0] + q[3] * q[3]) - 1.0f) * RAD_TO_ANGLE_COEF;
    *Roll = asinf(2.0f * (q[0] * q[2] - q[1] * q[3])) * RAD_TO_ANGLE_COEF;
}

/**
 * @brief        Convert eular angle to quaternion
 */
void EularAngleToQuaternion(float Yaw, float Pitch, float Roll, float *q)
{
    float cosPitch, cosYaw, cosRoll, sinPitch, sinYaw, sinRoll;
    Yaw /= RAD_TO_ANGLE_COEF;
    Pitch /= RAD_TO_ANGLE_COEF;
    Roll /= RAD_TO_ANGLE_COEF;
    cosPitch = arm_cos_f32(Pitch / 2);
    cosYaw = arm_cos_f32(Yaw / 2);
    cosRoll = arm_cos_f32(Roll / 2);
    sinPitch = arm_sin_f32(Pitch / 2);
    sinYaw = arm_sin_f32(Yaw / 2);
    sinRoll = arm_sin_f32(Roll / 2);
    q[0] = cosPitch * cosRoll * cosYaw + sinPitch * sinRoll * sinYaw;
    q[1] = sinPitch * cosRoll * cosYaw - cosPitch * sinRoll * sinYaw;
    q[2] = sinPitch * cosRoll * sinYaw + cosPitch * sinRoll * cosYaw;
    q[3] = cosPitch * cosRoll * sinYaw - sinPitch * sinRoll * cosYaw;
}
