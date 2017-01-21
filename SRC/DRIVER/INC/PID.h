
#ifndef __PID_H_
#define __PID_H_

#include "Kalman_filter.h"
#include "motor.h"

#define DEAD_VOLTAGE	80u

struct PID
{
	float SetSpeed;			//�趨ֵ
	float ActualSpeed;	//ʵ��ֵ
	float err;					//ƫ��ֵ
	float err_last;			//���ϸ�ƫ��ֵ
	float pwm;					//����ֵ
	float integral;			//�������ֵ
	float Kp,Ki,Kd,Kpp;	//PID����
};

struct D_PID
{
	float Last_Gro_y;			//��һ��������ֵ
	float Gro_y;					//����������ֵ
	float Errory_Out;			//�⻷���
	float Out_Pi_Y;				//�⻷PI
	float Out_Yp;					//�⻷P
	float Out_Yi;					//�⻷I
	float Errory_In;			//�ڻ����
	float In_Yp;					//�ڻ�P
	float In_Yi;					//�ڻ�I
	float In_Yd;					//�ڻ�D
	float In_PID_Y;				//�ڻ�PID
};

extern struct PID pid;
extern struct D_PID d_pid;

#define Integral_Limiting(variable,amplitude)	do{\
																	if(variable > amplitude)\
																		variable = amplitude;\
																	else if(variable < -amplitude)\
																		variable = -amplitude;\
																	}while(0);

void PID_init(void);
void D_PID_Init(void);
void PWM_Control(int16_t Speed);
void PID_Colu(void);
void double_PID(void);

#endif
