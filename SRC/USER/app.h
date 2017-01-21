
#ifndef __APP_H
#define __APP_H

#include "includes.h"
#include "led.h"
#include "delay.h"
#include "PC_Link.h"
#include "spi.h"
#include "sys.h"
#include "tim.h"
#include "usart.h"
#include "motor.h"
#include "manipulator.h"
#include "stm32f10x_it.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "Kalman_filter.h"
#include "ANO-Tech.h"
#include "includes.h"
#include "hmc5883.h"
#include "infrared.h"
#include "pid.h"

extern float pitch,roll,yaw; 		//ŷ����
extern short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
extern short gyrox,gyroy,gyroz;	//������ԭʼ����

#define LED PCout(13)
#define LIG PAout(15)

/*��ʼ����*/
//�������ȼ�
#define START_TASK_PRIO	3
//�����ջ��С
#define START_STK_SIZE	128
//������ƿ�
extern OS_TCB	StartTackTCB;
//�����ջ
extern CPU_STK	START_TASK_STK[START_STK_SIZE];
//��ʼ������ƺ���
void start_task(volatile void *p_arg);

/*���1*/
//����MOTOR���ȼ�
#define MOTOR1_TASK_PRIO	4
//����MOTOR��ջ��С
#define MOTOR1_STK_SIZE	128
//����MOTOR���ƿ�
extern OS_TCB MOTOR1TCB;
//����MOTOR��ջ
extern CPU_STK	MOTOR1_STK[MOTOR1_STK_SIZE];
//������ƺ���
void Motor1_task(volatile void *p_arg);

/*���2*/
//����MOTOR���ȼ�
#define MOTOR2_TASK_PRIO	5
//����MOTOR��ջ��С
#define MOTOR2_STK_SIZE	128
//����MOTOR���ƿ�
extern OS_TCB MOTOR2TCB;
//����MOTOR��ջ
extern CPU_STK	MOTOR2_STK[MOTOR2_STK_SIZE];
//������ƺ���
void Motor2_task(volatile void *p_arg);

/*�ɼ�����*/
//����COLLECTION���ȼ�
#define COLLECTION_TASK_PRIO	8
//����COLLECTION��ջ��С
#define COLLECTION_STK_SIZE	128
//����COLLECTION���ƿ�
extern OS_TCB COLLECTIONTCB;
//����COLLECTION��ջ
extern CPU_STK	COLLECTION_STK[COLLECTION_STK_SIZE];
//������ƺ���
void Collection_task(volatile void *p_arg);

/*�˲�����*/
//����FILTER���ȼ�
#define FILTER_TASK_PRIO	7
//����FILTER��ջ��С
#define FILTER_STK_SIZE	128
//����FILTER���ƿ�
extern OS_TCB FILTERTCB;
//����FILTER��ջ
extern CPU_STK	FILTER_STK[FILTER_STK_SIZE];
//������ƺ���
void filter_task(volatile void* p_arg);

/*������ʾ����*/
//����DISPLAY���ȼ�
#define DISPLAY_TASK_PRIO	6
//����DISPLAY��ջ��С
#define DISPLAY_STK_SIZE	128
//����DISPLAY���ƿ�
extern OS_TCB DISPLAYTCB;
//����DISPLAY��ջ
extern CPU_STK	DISPLAY_STK[DISPLAY_STK_SIZE];
//����DISPLAY���ƺ���
void Display_task(volatile void *p_arg);

/*��ʱ��1�ص�����*/
extern OS_TMR tmr1;
void tmr1_callback_led(void *p_tmr, void *p_arg);

#endif
