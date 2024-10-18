#ifndef _IMU_H_
#define _IMU_H_

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "sys.h"

//下面的宏定义不要动
/* Adjust the Kp and Ki of IMU module
   This can change the convergence speed of the IMU output */
#define BOARD_DOWN                  1
#define sampleFreq                  1000.0f                  // sample frequency in Hz
#define twoKpDef                    (40.0f * 0.5f)          // 2 * proportional gain
#define twoKiDef                    (2.0f * 0.005f)         // 2 * integral gain
#define AXIS_6                      1                       // 0--9axis   1--6axis

#define ACC_X_OFFSET    -33   //加速度计 x
#define ACC_Y_OFFSET    77		//
#define ACC_Z_OFFSET    -43		//

#define GYRO_X_OFFSET    23   //陀螺仪 x
#define GYRO_Y_OFFSET    77		//陀螺仪 y
#define GYRO_Z_OFFSET    26		//陀螺仪 z

#define MAG_SEN         0.1f    //转换成 uT
#define MAG_X_OFFSET    -10     //磁场偏移，采样空间磁场后matlab计算得到
#define MAG_Y_OFFSET    1
#define MAG_Z_OFFSET    12
//上面这几个参数不要动

#define IMU_SETTEMCONTROL_LOW()  GPIO_WriteBit(GPIOF, GPIO_Pin_6, Bit_RESET)
#define IMU_SETTEMCONTROL_HIGH() GPIO_WriteBit(GPIOF, GPIO_Pin_6, Bit_SET)

//C板陀螺仪温度控制PID初始化使用
#define IMU_TEMPERATURE_CONTROL     0                       //imu温控，可能导致漂移，需测试后使用
#define IMU_CONTROL_TEMPERATURE     30                      //imu控制温度
#define	IMUTEMP_MAX_PWM							5000										//这个就是TIM10的PWM的最大计数值

#define IMU_TEMPERATURE_KP								1600.0f
#define IMU_TEMPERATURE_KI								0.20f
#define IMU_TEMPERATURE_KD								0.00f
#define IMU_TEMPERATURE_RAMPSTEP					5
#define IMU_TEMPERATURE_ERRALLMAX 				10000
#define IMU_TEMPERATURE_OUTMAX						4500
#define IMU_TEMPERATURE_OUTSTEP						2000
#define IMU_TEMPERATURE_PRECISION 				0.0f
//C板陀螺仪初始化宏定义,用于创建温控的时候进行赋值初始化
#define IMU_TEMPERATURE_DEFAULT \
{\
	IMU_TEMPERATURE_KP,\
	IMU_TEMPERATURE_KI,\
	IMU_TEMPERATURE_KD,\
	0,0,0,0,0,0,0,0,\
	Normal_e,\
	0,0,0,\
	IMU_TEMPERATURE_RAMPSTEP,\
	IMU_TEMPERATURE_ERRALLMAX,\
	IMU_TEMPERATURE_OUTMAX,\
	IMU_TEMPERATURE_OUTSTEP,\
	0,\
	IMU_TEMPERATURE_PRECISION,\
}


//结构体定义
typedef struct{
    float ax; //unit: m/s2
    float ay;
    float az;
    
    float gx; //rad/s
    float gy;
    float gz;
    
    float mx; //unit: uT
    float my;
    float mz;
		
    float temp;
}imu_ripdata_t;  //处理过的数据


typedef struct{
    float rol;
    float pit;
    float yaw;
}attitude_angle_t;  //姿态角

typedef struct{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t gx;
    int16_t gy;
    int16_t gz;
    
    int16_t mx;
    int16_t my;
    int16_t mz;
    
}imu_offset_t;      //偏移

typedef struct{
    int16_t ax;
    int16_t ay;
    int16_t az;
 
    int16_t temp;

    int16_t gx;
    int16_t gy;
    int16_t gz;
    
    int16_t mx;     //unit: mG
    int16_t my;
    int16_t mz;
}imu_rawdata_t;     //原始数据


typedef struct
{
	float q0;
	float q1;
	float q2;
	float q3;
}imu_quaternion_t;



typedef struct{
    attitude_angle_t   		atti; 			//姿态角
    imu_ripdata_t 			rip;			//处理后的数据
    imu_rawdata_t 			raw;			//原始数据
    imu_offset_t  			offset;			//偏移数据
	imu_quaternion_t 		IMUquaternion; 	//陀螺仪四元数
}imu_t;			//imu_t 的全部数据
//end
typedef struct
{
		
	float T;
	float A[2][2];//系统系数A
	float B[2];	  //系统系数B,H
	float H[2];	  
	float Last_result;//上一次的角度值
	float Bias_angle;             //陀螺仪积分出的角度相对于加速度计的偏移量
	float Now_Groy_palstance;            //当前包含固定偏差的陀螺仪输出角速度
	float Gyro_measure_noise;                      //为陀螺仪测量噪声 w
	float Acceleration_measure_noise;              //为加速度计测量噪声 v
	float Measure;                               //当前的测量值Zk
	float Q[2][2];
	float R;
	float Now_measure_result[2];        //当前预测结果
	float Now_best_result[2];          //当前最优化估算值
	float Now_covariance[2][2];       //对应于Now_measure_result的协方差
	float last_covariance[2][2];
	float Kalman_Gain[2];              //卡尔曼增益
	float I[2][2];         //I为单位阵     
	float Real_angle;   
	float rate;

	//P-先验协方差矩阵，K-卡尔曼增益
	float P[2][2];
	float K[2];
	float D[2][2];
	float S[2][2];  
	
	float Kal_Angle;
	
}KalMan_t;

extern void imu_main(void);              //imu获取姿态角主函数
extern float imu_yaw_angle;              //yaw角度 °
extern float imu_yaw_angular_speed;      //yaw角速度 °/s
extern float imu_pitch_angular_speed;    //pitch角速度 °/s 

float get_yaw_angle(void) ;
float get_pit_angle(void) ;
float get_imu_wx(void)    ;
float get_imu_wy_pitch_rpm(void)    ;
float get_imu_wz_yaw_rpm(void)    ;
imu_quaternion_t getIMUQuar(void);
void Print_IMU(void);
float Get_KRpit();
float Kalman_LvBo(float angle,float Gyro_X,float gALL);
void IMU_KalInit(KalMan_t *Kimu_t);
void Kalman_Calc(KalMan_t *Kimu_t,float angle,float Gyro_X,float gALL);
float GetKalpit(void);

float GetKalyaw(void);
void IMUOffset_Init(void);
//#ifdef __cplusplus
//		}
//#endif
#endif
