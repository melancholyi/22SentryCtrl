/** 
  * @ brief           IMU文件 2021/10/1 看明白了     
  * @ author         	闫创业 RM22赛季哨兵（增删版本，非原创）
  * @ time						2021/10/1
  * @ version					V1.1       	
  * @ description			
 **/
 
#include "imu.h"
#include "spi.h"
#include "usart6.h"
#include <math.h>
#include <string.h>
#include "stdlib.h"
#include "BMI088driver.h"
/**FreeRTOS*START***************/
#include "FreeRTOS.h"					//FreeRTOS使用
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/

/************** 此处定义的三个变量再imu.c（本文件）的函数 NO.十六用到了 **************/
float imu_yaw_angle=0;			//陀螺仪yaw角度
float imu_yaw_angular_speed=0; //陀螺仪yaw角速度
float imu_pitch_angular_speed=0;//imu Pitch角速度
//---------------------------------------------------------------------------------------------------
// Variable definitions
static volatile float twoKp = twoKpDef;                                           // 2 * proportional gain (Kp)
static volatile float twoKi = twoKiDef;                                           // 2 * integral gain (Ki)
static volatile float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f; // integral error terms scaled by Ki
volatile float        q0 = 1.0f;
volatile float        q1 = 0.0f;
volatile float        q2 = 0.0f;
volatile float        q3 = 0.0f;
static volatile float gx, gy, gz, ax, ay, az, mx, my, mz;  

//初始化imu
static imu_t imu = {
                    {0,0,0},                     //atti
                    {0,0,0, 0,0,0, 0,0,0, 0},       //rip
                    {0,0,0, 0,0,0, 0,0,0, 0},       //raw
                    {ACC_X_OFFSET,  ACC_Y_OFFSET,  ACC_Z_OFFSET,  GYRO_X_OFFSET,  GYRO_Y_OFFSET,  GYRO_Z_OFFSET,  MAG_X_OFFSET,  MAG_Y_OFFSET,  MAG_Z_OFFSET},         //offset
                    };

/** @num 						1 
  * @brief          加速度计低通滤波
  * @author         李运环
  * @param[in]      转化为国际单位的加速度值的结构体指针
  * @retval         返回空
  */
	//用到了
static void accel_low_pass_filter(imu_ripdata_t * mpudata){
    static uint8_t updata_count=0;

    static double accel_fliter_1[3] = {0.0f, 0.0f, 0.0f};
    static double accel_fliter_2[3] = {0.0f, 0.0f, 0.0f};
    static double accel_fliter_3[3] = {0.0f, 0.0f, 0.0f};
    static const double fliter_num[3] = {1.929454039488895f, -0.93178349823448126f, 0.002329458745586203f};
    if(updata_count==0){
        accel_fliter_1[0] = accel_fliter_2[0] = accel_fliter_3[0] = mpudata->ax;
        accel_fliter_1[1] = accel_fliter_2[1] = accel_fliter_3[1] = mpudata->ay;
        accel_fliter_1[2] = accel_fliter_2[2] = accel_fliter_3[2] = mpudata->az;
        updata_count++;
    }
    else{
        accel_fliter_1[0] = accel_fliter_2[0];
        accel_fliter_2[0] = accel_fliter_3[0];
        accel_fliter_3[0] = accel_fliter_2[0] * fliter_num[0] + accel_fliter_1[0] * fliter_num[1] + mpudata->ax * fliter_num[2];

        accel_fliter_1[1] = accel_fliter_2[1];
        accel_fliter_2[1] = accel_fliter_3[1];
        accel_fliter_3[1] = accel_fliter_2[1] * fliter_num[0] + accel_fliter_1[1] * fliter_num[1] + mpudata->ay * fliter_num[2];

        accel_fliter_1[2] = accel_fliter_2[2];
        accel_fliter_2[2] = accel_fliter_3[2];
        accel_fliter_3[2] = accel_fliter_2[2] * fliter_num[0] + accel_fliter_1[2] * fliter_num[1] + mpudata->az * fliter_num[2];
    }
    mpudata->ax = accel_fliter_3[0];
    mpudata->ay = accel_fliter_3[1];
    mpudata->az = accel_fliter_3[2];
}

/** @num						2
  * @brief          更新加速度、角速度、磁力值
  * @author         李运环
  * @retval         返回空
  */
	//用到了
static float Ag=0;
static void imu_get_data(void)
{
		float gyro[3], accel[3], temp;
		BMI088_read(gyro, accel, &temp);
		imu.rip.ax=accel[0];
		imu.rip.ay=accel[1];
		imu.rip.az=accel[2];
		Ag=sqrtf(imu.rip.ax*imu.rip.ax+imu.rip.ay*imu.rip.ay+imu.rip.az*imu.rip.az);
		imu.rip.temp=temp;
		
		imu.rip.gx=gyro[0];
		imu.rip.gy=gyro[1];
		imu.rip.gz=gyro[2];
	
		imu.rip.mx=imu.rip.my=imu.rip.mz=0;
		accel_low_pass_filter(&imu.rip);//低通滤波器
}



/** @num						3
  * @brief          快速计算方根
  * @author         open source
  * @param[in]      需要计算方根的单精度浮点数
  * @retval         返回空
  */
	//用到了
static float invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}
/** @num						4
  * @brief          姿态解算，更新四元数
  * @author         http://x-io.co.uk/open-source-imu-and-ahrs-algorithms/
  * @param[in]      转换成国际单位的加速度、角速度的结构体指针
  * @retval         返回空
  */
	//用到了
static void mahony_ahrs_updateIMU(imu_ripdata_t const *mpudata){
    float recipNorm;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float qa, qb, qc;

    gx = mpudata->gx;
    gy = mpudata->gy;
    gz = mpudata->gz;
    ax = mpudata->ax;
    ay = mpudata->ay;
    az = mpudata->az;

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))){

        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Estimated direction of gravity and vector perpendicular to magnetic flux
        halfvx = q1 * q3 - q0 * q2;
        halfvy = q0 * q1 + q2 * q3;
        halfvz = q0 * q0 - 0.5f + q3 * q3;

        // Error is sum of cross product between estimated and measured direction of gravity
        halfex = (ay * halfvz - az * halfvy);
        halfey = (az * halfvx - ax * halfvz);
        halfez = (ax * halfvy - ay * halfvx);

        // Compute and apply integral feedback if enabled
        if (twoKi > 0.0f){
          integralFBx += twoKi * halfex * (1.0f / sampleFreq); // integral error scaled by Ki
          integralFBy += twoKi * halfey * (1.0f / sampleFreq);
          integralFBz += twoKi * halfez * (1.0f / sampleFreq);
          gx += integralFBx; // apply integral feedback
          gy += integralFBy;
          gz += integralFBz;
        }
        else{
          integralFBx = 0.0f; // prevent integral windup
          integralFBy = 0.0f;
          integralFBz = 0.0f;
        }

        // Apply proportional feedback
        gx += twoKp * halfex;
        gy += twoKp * halfey;
        gz += twoKp * halfez;
    }

    // Integrate rate of change of quaternion
    gx *= (0.5f * (1.0f / sampleFreq)); // pre-multiply common factors
    gy *= (0.5f * (1.0f / sampleFreq));
    gz *= (0.5f * (1.0f / sampleFreq));
    qa = q0;
    qb = q1;
    qc = q2;
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
	
	imu.IMUquaternion.q0 = q0;
	imu.IMUquaternion.q1 = q1;
	imu.IMUquaternion.q2 = q2;
	imu.IMUquaternion.q3 = q3;

}

/** @num						5
  * @brief          姿态解算, 更新四元数, this function takes 56.8us.(168M)
  * @author         http://x-io.co.uk/open-source-imu-and-ahrs-algorithms/
  * @param[in]      转换成国际单位的加速度、角速度、磁力值的结构体指针
  * @retval         返回空
  */
	//用到了
static void mahony_ahrs_update(imu_ripdata_t const *mpudata){
    float recipNorm;
    float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
    float hx, hy, bx, bz;
    float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
    float halfex, halfey, halfez;
    float qa, qb, qc;

    gx = mpudata->gx;
    gy = mpudata->gy;
    gz = mpudata->gz;
    ax = mpudata->ax;
    ay = mpudata->ay;
    az = mpudata->az;
    mx = mpudata->mx;
    my = mpudata->my;
    mz = mpudata->mz;

#if AXIS_6 == 1
    mx = 0;
    my = 0;
    mz = 0;
#endif

    if((mx == 0) && (my == 0) && (mz == 0)){
        mahony_ahrs_updateIMU(mpudata);
        return;
    }
    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
//    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
//	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	if (Ag>=9.7&&Ag<=9.9)
    {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Normalise magnetometer measurement
        recipNorm = invSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        q0q0 = q0 * q0;
        q0q1 = q0 * q1;
        q0q2 = q0 * q2;
        q0q3 = q0 * q3;
        q1q1 = q1 * q1;
        q1q2 = q1 * q2;
        q1q3 = q1 * q3;
        q2q2 = q2 * q2;
        q2q3 = q2 * q3;
        q3q3 = q3 * q3;

        // Reference direction of Earth's magnetic field
        hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
        hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
        bx = sqrt(hx * hx + hy * hy);
        bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));

        // Estimated direction of gravity and magnetic field
        halfvx = q1q3 - q0q2;
        halfvy = q0q1 + q2q3;
        halfvz = q0q0 - 0.5f + q3q3;
        halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
        halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
        halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

        // Error is sum of cross product between estimated direction and measured direction of field vectors
        halfex = (ay * halfvz - az * halfvy) + (my * halfwz - mz * halfwy);
        halfey = (az * halfvx - ax * halfvz) + (mz * halfwx - mx * halfwz);
        halfez = (ax * halfvy - ay * halfvx) + (mx * halfwy - my * halfwx);

        // Compute and apply integral feedback if enabled
        if (twoKi > 0.0f)
        {
            integralFBx += twoKi * halfex * (1.0f / sampleFreq); // integral error scaled by Ki
            integralFBy += twoKi * halfey * (1.0f / sampleFreq);
            integralFBz += twoKi * halfez * (1.0f / sampleFreq);
            gx += integralFBx; // apply integral feedback
            gy += integralFBy;
            gz += integralFBz;
        }
        else
        {
            integralFBx = 0.0f; // prevent integral windup
            integralFBy = 0.0f;
            integralFBz = 0.0f;
        }
        // Apply proportional feedback
        gx += twoKp * halfex;
        gy += twoKp * halfey;
        gz += twoKp * halfez;
    }
    // Integrate rate of change of quaternion
    gx *= (0.5f * (1.0f / sampleFreq)); // pre-multiply common factors
    gy *= (0.5f * (1.0f / sampleFreq));
    gz *= (0.5f * (1.0f / sampleFreq));
    qa = q0;
    qb = q1;
    qc = q2;
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
	
		
}

/** @num						6
  * @brief          根据四元数更新姿态角
  * @author         李运环
  * @param[in]      NULL
  * @retval         返回空
  */
	//用到了
static void IMU_getYawPitchRoll(attitude_angle_t * atti)
{
    static float yaw_temp = 0,last_yaw_temp = 0;
    static int   yaw_count = 0;
    // yaw    -pi----pi
    atti->yaw = atan2(2.0f * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3)* 57.3;
    // pitch  -pi/2--- pi/2
    atti->pit = -asin(2.0f * (q1 * q3 - q0 * q2))* 57.3;
    // roll   -pi-----pi
    atti->rol = atan2(2.0f * (q0 * q1 + q2 * q3), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3)* 57.3;

    //yaw数据连续化
    last_yaw_temp = yaw_temp;
    yaw_temp = atti->yaw; 
    if(yaw_temp - last_yaw_temp>= 300){
        yaw_count--;
    } 
    else if (yaw_temp - last_yaw_temp <= -300){
        yaw_count++;
    }
    atti->yaw = yaw_temp + yaw_count*360;
		
}

void Print_IMU(void)
{
//	printf("YRP:%.4f,%.4f,%.4f,%.4f,%.4f\n",imu.atti.yaw,imu.atti.pit,GetKalyaw(),GetKalpit(),Ag);
	printf("YRP:%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",GetKalyaw(),GetKalpit(),imu.atti.rol,imu.rip.gx,imu.rip.gy,imu.rip.gz);
//	printf("YRP:%.4f,%.4f,%.4f,%.4f,,%.4f,%.4f\n",imu.atti.yaw,imu.atti.pit,imu.atti.rol，GetKalyaw(),GetKalpit(),Ag);
}

///** @num						7
//  * @brief          IMU温度闭环控制
//  * @author         李运环
//   *@param[in]      读取的mpu6500的温度值，单位：摄氏度
//  * @retval         返回空
//  */
//uint16_t tempPWM = 0;								
////创建IMU的温控PID传感器
//GeneralPID_TypeDef IMUTempControlPID = IMU_TEMPERATURE_DEFAULT;
//static void IMU_TempControl(double temp)
//{
//    
//    static uint8_t tempcontrol_flag = 0;
//    static uint8_t temp_constant_time = 0 ;
//	
//    if (tempcontrol_flag) //达到最大温度，才进行控温
//	{
//        IMUTempControlPID.RampTarget = IMU_CONTROL_TEMPERATURE;
//		IMUTempControlPID.State_RampOrNormal = Ramp_e;
//        PID_Update(&IMUTempControlPID,temp);
//        PID_calculate(&IMUTempControlPID);//温度控制，输出PID_OUT给 TIM10的PWM
//        if (IMUTempControlPID.PID_Out < 0.0f)   IMUTempControlPID.PID_Out = 0.0f;
//        tempPWM = (uint16_t)IMUTempControlPID.PID_Out;
//				TIM_SetCompare1(TIM10, tempPWM);
//    }
//    else
//	{
//        //在没有达到设置的温度，一直最大功率加热
//        if (temp > IMU_CONTROL_TEMPERATURE)
//		{
//            temp_constant_time ++;
//            if(temp_constant_time > 50)
//			{
//                tempcontrol_flag = 1;
//            }
//        }
//		TIM_SetCompare1(TIM10, IMUTEMP_MAX_PWM - 1);
//    }
//}





static float offset[3] = {0,0,0};	//中值 ok
static int Init_flag=0;
void offsetInit(void)
{
		
		imu_get_data();       //      得到原始数据  OK					//3334
		offset[0] -= imu.rip.gx * 0.0003f;
		offset[1] -= imu.rip.gy * 0.0003f;
		offset[2] -= imu.rip.gz * 0.0003f;
}
KalMan_t KalPit;
KalMan_t KalYaw;

void IMUOffset_Init(void)
{
	static uint16_t calibrate = 0; 		//校准，临时变量 ok
	if(calibrate <= 3334){	
		calibrate++;
		if(calibrate==2)
		{
			IMU_KalInit(&KalPit);
			IMU_KalInit(&KalYaw);
		}
		imu_get_data();       
		offset[0] -= imu.rip.gx * 0.0003f;
		offset[1] -= imu.rip.gy * 0.0003f;
		offset[2] -= imu.rip.gz * 0.0003f;
	}

}
void imu_main(void){
	imu_get_data();       //      得到原始数据  OK
	imu.rip.gx += offset[0];
	imu.rip.gy += offset[1];
	imu.rip.gz += offset[2];
	
	mahony_ahrs_update(&imu.rip);    //imu.c func brief:姿态解算, 更新四元数, t
	IMU_getYawPitchRoll(&imu.atti);  //imu.c func brief:根据四元数更新姿态角，并对yaw进行连续化处理	
	imu_yaw_angle=imu.atti.yaw;								//全局变量
	//	Kalman_LvBo(imu.atti.pit,imu.rip.gy,Ag);
	Kalman_Calc(&KalYaw,imu.atti.yaw,imu.rip.gz,Ag);	
	Kalman_Calc(&KalPit,imu.atti.pit,imu.rip.gy,Ag);
	
	imu_yaw_angular_speed=imu.rip.gz*57.3f; 	//全局变量
    imu_pitch_angular_speed=imu.rip.gy*57.3f;   //全局变量
    //绝对延时

}

/** 
  * @ brief          	这里就是IMU的参数了，需要哪个就写一个getIMUXXX的函数 不要用Extern 不太好
  * @ author         	闫创业 RM22赛季哨兵
  * @ time						2021/10/2
  * @ version					V1.1       	
  * @ param[in]     	void
  * @ retval         	imu.xxx.xxx	
 **/ 
 //自己实现一个结构体吧
float get_yaw_angle(void){
    return imu.atti.yaw;
}

float get_pit_angle(void){
    return imu.atti.pit;
}

float get_imu_wx(void){
	#if Gimbal_TOP_DEFINE  //TBSYCY5
		return imu.rip.gx * 57.295780f;
	#elif  Gimbal_BOTTOM_DEFINE 
		return -imu.rip.gx * 57.295780f;
	#endif
}

float get_imu_wy_pitch_rpm(void){
    #if Gimbal_TOP_DEFINE  //TBSYCY6
	return imu.rip.gy * 57.295780f/6.0f;
#elif  Gimbal_BOTTOM_DEFINE 
	return -imu.rip.gx * 57.295780f/6.0f;
#endif	
}

float get_imu_wz_yaw_rpm(void){
	#if Gimbal_TOP_DEFINE  //TBSYCY7
		return imu.rip.gz * 57.295780f/6.0f;
	#elif  Gimbal_BOTTOM_DEFINE 
		return -imu.rip.gz * 57.295780f/6.0f;
	#endif
}
imu_quaternion_t getIMUQuar(void)
{
	return imu.IMUquaternion;

}

//KalMan_t KalPit;
#define MPU_GRAVITY 9.77

void IMU_KalInit(KalMan_t *Kimu_t)
{
	Kimu_t->T=0.001;
	
	//系统模型参数
	Kimu_t->A[0][0]=1;
	Kimu_t->A[0][1]=-Kimu_t->T;
	Kimu_t->A[1][0]=0;
	Kimu_t->A[1][1]=1;
	
	Kimu_t->B[0]=Kimu_t->T;
	Kimu_t->B[1]=0;
	
	Kimu_t->H[0]=1;
	Kimu_t->H[1]=0;
	
	//协方差矩阵
	Kimu_t->Q[0][0]=0.0000003;
	Kimu_t->Q[0][1]=0;
	Kimu_t->Q[1][0]=0;
	Kimu_t->Q[1][1]=0.0000001;

	Kimu_t->R=0.005;

	Kimu_t->last_covariance[0][0]=0.005;
	Kimu_t->last_covariance[0][1]=0.0000005;
	Kimu_t->last_covariance[1][0]=0.005;
	Kimu_t->last_covariance[1][1]=0.005;
	
	Kimu_t->I[0][0]=1;
	Kimu_t->I[0][1]=0;
	Kimu_t->I[1][0]=0;
	Kimu_t->I[1][1]=1;
	Kimu_t->rate=0;
}
float NKR_pit; 

void Kalman_Calc(KalMan_t *Kimu_t,float angle,float Gyro_X,float gALL)
{

	Kimu_t->rate=MPU_GRAVITY/gALL;
	Kimu_t->R=fabs(1-Kimu_t->rate* Kimu_t->rate* Kimu_t->rate* Kimu_t->rate* Kimu_t->rate);
	Kimu_t->Now_Groy_palstance=(Gyro_X);       //陀螺仪角速度
	Kimu_t->Last_result=Kimu_t->Now_best_result[0];        //Ψ
	Kimu_t->Bias_angle=Kimu_t->Now_best_result[1];        //b
	//	测量方程         Z(K)
	Kimu_t->Measure=angle;                           //加速度角度
	//公式（1）    当前状态方程       X(K|K-1)
	Kimu_t->Now_measure_result[0]= Kimu_t->A[0][0]*Kimu_t->Last_result+Kimu_t->A[0][1]* Kimu_t->Bias_angle
									+Kimu_t->B[0]*Kimu_t->Now_Groy_palstance+Kimu_t->Gyro_measure_noise*Kimu_t->T;
	Kimu_t->Now_measure_result[1]=Kimu_t->A[1][0]*Kimu_t->Last_result+Kimu_t->A[1][1]* Kimu_t->Bias_angle;
	//公式（2）    当前状态协方差          P(K/K-1)
	Kimu_t->P[0][0]=Kimu_t->A[0][0]*Kimu_t->last_covariance[0][0]+Kimu_t->A[0][1]*Kimu_t->last_covariance[1][0];
	Kimu_t->P[0][1]=Kimu_t->A[0][0]*Kimu_t->last_covariance[0][1]+Kimu_t->A[0][1]*Kimu_t->last_covariance[1][1];
	Kimu_t->P[1][0]=Kimu_t->A[1][0]*Kimu_t->last_covariance[0][0]+Kimu_t->A[1][1]*Kimu_t->last_covariance[1][0];
	Kimu_t->P[1][1]=Kimu_t->A[1][0]*Kimu_t->last_covariance[0][1]+Kimu_t->A[1][1]*Kimu_t->last_covariance[1][1];
    Kimu_t->Now_covariance[0][0]=Kimu_t->P[0][0]*Kimu_t->A[0][0]+Kimu_t->P[0][1]*Kimu_t->A[0][1]+Kimu_t->Q[0][0];
    Kimu_t->Now_covariance[0][1]=Kimu_t->P[0][0]*Kimu_t->A[1][0]+Kimu_t->P[0][1]*Kimu_t->A[1][1]+Kimu_t->Q[0][1];
    Kimu_t->Now_covariance[1][0]=Kimu_t->P[1][0]*Kimu_t->A[0][0]+Kimu_t->P[1][1]*Kimu_t->A[0][1]+Kimu_t->Q[1][0];
    Kimu_t->Now_covariance[1][1]=Kimu_t->P[1][0]*Kimu_t->A[1][0]+Kimu_t->P[1][1]*Kimu_t->A[1][1]+Kimu_t->Q[1][1];
	//公式（4）    卡尔曼增益
    Kimu_t->K[0]=Kimu_t->H[0]*Kimu_t->Now_covariance[0][0]+Kimu_t->H[1]*Kimu_t->Now_covariance[1][0];
    Kimu_t->K[1]=Kimu_t->H[0]*Kimu_t->Now_covariance[0][1]+Kimu_t->H[1]*Kimu_t->Now_covariance[1][1];
	Kimu_t->Kalman_Gain[0]=(Kimu_t->Now_covariance[0][0]*Kimu_t->H[0]+Kimu_t->Now_covariance[0][1]*Kimu_t->H[1])/(Kimu_t->H[0]*Kimu_t->K[0]+Kimu_t->H[1]*Kimu_t->K[1]+Kimu_t->R);
	Kimu_t->Kalman_Gain[1]=(Kimu_t->Now_covariance[1][0]*Kimu_t->H[0]+Kimu_t->Now_covariance[1][1]*Kimu_t->H[1])/(Kimu_t->H[0]*Kimu_t->K[0]+Kimu_t->H[1]*Kimu_t->K[1]+Kimu_t->R);
	//公式（3）   最优化估算值              X(K|K)
	Kimu_t->Now_best_result[0]=Kimu_t->Now_measure_result[0]+Kimu_t->Kalman_Gain[0]*(Kimu_t->Measure-(Kimu_t->H[0]*Kimu_t->Now_measure_result[0]+Kimu_t->H[1]*Kimu_t->Now_measure_result[1]));
	Kimu_t->Now_best_result[1]=Kimu_t->Now_measure_result[1]+Kimu_t->Kalman_Gain[1]*(Kimu_t->Measure-(Kimu_t->H[0]*Kimu_t->Now_measure_result[0]+Kimu_t->H[1]*Kimu_t->Now_measure_result[1]));
	////公式（5）   更新当前状态的协方差      P(K|K)
    Kimu_t->D[0][0]=Kimu_t->I[0][0]-Kimu_t->Kalman_Gain[0]*Kimu_t->H[0];
    Kimu_t->D[0][1]=Kimu_t->I[0][1]-Kimu_t->Kalman_Gain[0]*Kimu_t->H[1];
    Kimu_t->D[1][0]=Kimu_t->I[1][0]-Kimu_t->Kalman_Gain[1]*Kimu_t->H[0];
    Kimu_t->D[1][1]=Kimu_t->I[1][1]-Kimu_t->Kalman_Gain[1]*Kimu_t->H[1];
    Kimu_t->S[0][0]=Kimu_t->D[0][0]*Kimu_t->Now_covariance[0][0]+Kimu_t->D[0][1]*Kimu_t->Now_covariance[1][0];
    Kimu_t->S[0][1]=Kimu_t->D[0][0]*Kimu_t->Now_covariance[0][1]+Kimu_t->D[0][1]*Kimu_t->Now_covariance[1][1];
    Kimu_t->S[1][0]=Kimu_t->D[1][0]*Kimu_t->Now_covariance[0][0]+Kimu_t->D[1][1]*Kimu_t->Now_covariance[1][0];
    Kimu_t->S[1][1]=Kimu_t->D[1][0]*Kimu_t->Now_covariance[0][1]+Kimu_t->D[1][1]*Kimu_t->Now_covariance[1][1];
    Kimu_t->last_covariance[0][0]=Kimu_t->S[0][0]*Kimu_t->D[0][0]+Kimu_t->S[0][1]*Kimu_t->D[0][1]+Kimu_t->Kalman_Gain[0]*Kimu_t->Kalman_Gain[0]*Kimu_t->R;
    Kimu_t->last_covariance[0][1]=Kimu_t->S[0][0]*Kimu_t->D[1][0]+Kimu_t->S[0][1]*Kimu_t->D[1][1]+Kimu_t->Kalman_Gain[0]*Kimu_t->Kalman_Gain[1]*Kimu_t->R;
    Kimu_t->last_covariance[1][0]=Kimu_t->S[1][0]*Kimu_t->D[0][0]+Kimu_t->S[1][1]*Kimu_t->D[0][1]+Kimu_t->Kalman_Gain[1]*Kimu_t->Kalman_Gain[0]*Kimu_t->R;
    Kimu_t->last_covariance[1][1]=Kimu_t->S[1][0]*Kimu_t->D[1][0]+Kimu_t->S[1][1]*Kimu_t->D[1][1]+Kimu_t->Kalman_Gain[1]*Kimu_t->Kalman_Gain[1]*Kimu_t->R;  
   
    Kimu_t->Kal_Angle=Kimu_t->Now_best_result[0];

}

float GetKalpit(void)
{
	return KalPit.Kal_Angle;
}

float GetKalyaw(void)
{
	return KalYaw.Kal_Angle;

}

