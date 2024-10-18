#ifndef _IMU_H_
#define _IMU_H_

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "sys.h"

//����ĺ궨�岻Ҫ��
/* Adjust the Kp and Ki of IMU module
   This can change the convergence speed of the IMU output */
#define BOARD_DOWN                  1
#define sampleFreq                  1000.0f                  // sample frequency in Hz
#define twoKpDef                    (40.0f * 0.5f)          // 2 * proportional gain
#define twoKiDef                    (2.0f * 0.005f)         // 2 * integral gain
#define AXIS_6                      1                       // 0--9axis   1--6axis

#define ACC_X_OFFSET    -33   //���ٶȼ� x
#define ACC_Y_OFFSET    77		//
#define ACC_Z_OFFSET    -43		//

#define GYRO_X_OFFSET    23   //������ x
#define GYRO_Y_OFFSET    77		//������ y
#define GYRO_Z_OFFSET    26		//������ z

#define MAG_SEN         0.1f    //ת���� uT
#define MAG_X_OFFSET    -10     //�ų�ƫ�ƣ������ռ�ų���matlab����õ�
#define MAG_Y_OFFSET    1
#define MAG_Z_OFFSET    12
//�����⼸��������Ҫ��

#define IMU_SETTEMCONTROL_LOW()  GPIO_WriteBit(GPIOF, GPIO_Pin_6, Bit_RESET)
#define IMU_SETTEMCONTROL_HIGH() GPIO_WriteBit(GPIOF, GPIO_Pin_6, Bit_SET)

//C���������¶ȿ���PID��ʼ��ʹ��
#define IMU_TEMPERATURE_CONTROL     0                       //imu�¿أ����ܵ���Ư�ƣ�����Ժ�ʹ��
#define IMU_CONTROL_TEMPERATURE     30                      //imu�����¶�
#define	IMUTEMP_MAX_PWM							5000										//�������TIM10��PWM��������ֵ

#define IMU_TEMPERATURE_KP								1600.0f
#define IMU_TEMPERATURE_KI								0.20f
#define IMU_TEMPERATURE_KD								0.00f
#define IMU_TEMPERATURE_RAMPSTEP					5
#define IMU_TEMPERATURE_ERRALLMAX 				10000
#define IMU_TEMPERATURE_OUTMAX						4500
#define IMU_TEMPERATURE_OUTSTEP						2000
#define IMU_TEMPERATURE_PRECISION 				0.0f
//C�������ǳ�ʼ���궨��,���ڴ����¿ص�ʱ����и�ֵ��ʼ��
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


//�ṹ�嶨��
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
}imu_ripdata_t;  //�����������


typedef struct{
    float rol;
    float pit;
    float yaw;
}attitude_angle_t;  //��̬��

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
    
}imu_offset_t;      //ƫ��

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
}imu_rawdata_t;     //ԭʼ����


typedef struct
{
	float q0;
	float q1;
	float q2;
	float q3;
}imu_quaternion_t;



typedef struct{
    attitude_angle_t   		atti; 			//��̬��
    imu_ripdata_t 			rip;			//����������
    imu_rawdata_t 			raw;			//ԭʼ����
    imu_offset_t  			offset;			//ƫ������
	imu_quaternion_t 		IMUquaternion; 	//��������Ԫ��
}imu_t;			//imu_t ��ȫ������
//end
typedef struct
{
		
	float T;
	float A[2][2];//ϵͳϵ��A
	float B[2];	  //ϵͳϵ��B,H
	float H[2];	  
	float Last_result;//��һ�εĽǶ�ֵ
	float Bias_angle;             //�����ǻ��ֳ��ĽǶ�����ڼ��ٶȼƵ�ƫ����
	float Now_Groy_palstance;            //��ǰ�����̶�ƫ���������������ٶ�
	float Gyro_measure_noise;                      //Ϊ�����ǲ������� w
	float Acceleration_measure_noise;              //Ϊ���ٶȼƲ������� v
	float Measure;                               //��ǰ�Ĳ���ֵZk
	float Q[2][2];
	float R;
	float Now_measure_result[2];        //��ǰԤ����
	float Now_best_result[2];          //��ǰ���Ż�����ֵ
	float Now_covariance[2][2];       //��Ӧ��Now_measure_result��Э����
	float last_covariance[2][2];
	float Kalman_Gain[2];              //����������
	float I[2][2];         //IΪ��λ��     
	float Real_angle;   
	float rate;

	//P-����Э�������K-����������
	float P[2][2];
	float K[2];
	float D[2][2];
	float S[2][2];  
	
	float Kal_Angle;
	
}KalMan_t;

extern void imu_main(void);              //imu��ȡ��̬��������
extern float imu_yaw_angle;              //yaw�Ƕ� ��
extern float imu_yaw_angular_speed;      //yaw���ٶ� ��/s
extern float imu_pitch_angular_speed;    //pitch���ٶ� ��/s 

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
