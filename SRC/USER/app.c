
#include "app.h"

//��ʼ������ƿ�
OS_TCB	StartTackTCB;
//��ʼ�����ջ
CPU_STK	START_TASK_STK[START_STK_SIZE];

//����MOTOR���ƿ�
OS_TCB MOTOR1TCB;
//����MOTOR��ջ
CPU_STK	MOTOR1_STK[MOTOR1_STK_SIZE];

//����MOTOR���ƿ�
OS_TCB MOTOR2TCB;
//����MOTOR��ջ
CPU_STK	MOTOR2_STK[MOTOR2_STK_SIZE];

//����COLLECTION���ƿ�
OS_TCB COLLECTIONTCB;
//����COLLECTION��ջ
CPU_STK	COLLECTION_STK[COLLECTION_STK_SIZE];

//����FILTER���ƿ�
OS_TCB FILTERTCB;
//����FILTER��ջ
CPU_STK	FILTER_STK[FILTER_STK_SIZE];

//����DISPLAY���ƿ�
OS_TCB DISPLAYTCB;
//����DISPLAY��ջ
CPU_STK	DISPLAY_STK[DISPLAY_STK_SIZE];

/*��ʱ��1�ص�����*/
OS_TMR tmr1;

//����MOTOR1���ƺ���
void Motor1_task(volatile void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
//		OSSchedLock(&err);
//		Remote_Send();
//		OSSchedUnlock(&err);
//		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
		MOTOR1_FORWARD(200);
		OSTimeDlyHMSM(0,0,1,500,OS_OPT_TIME_HMSM_STRICT,&err);
		MOTOR1_REVERSE(200);
		OSTimeDlyHMSM(0,0,1,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

//����MOTOR2���ƺ���
void Motor2_task(volatile void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		MOTOR2_FORWARD(100);
		OSTimeDlyHMSM(0,0,1,500,OS_OPT_TIME_HMSM_STRICT,&err);
		MOTOR2_REVERSE(100);
		OSTimeDlyHMSM(0,0,1,500,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}

//����Collection���ƺ���
void Collection_task(volatile void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSchedLock(&err);
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
		}
		OSSchedUnlock(&err);
		OSTaskSemPost(&FILTERTCB, OS_OPT_POST_NONE, &err);//������Ƕ�ź���
	}
}

//����FILTER���ƺ���
void filter_task(volatile void* p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, NULL, &err);//�ȴ���Ƕ�ź���
		OSSchedLock(&err);
		Angle_Calcu();
		OSSchedUnlock(&err);
		OSTaskSemPost(&DISPLAYTCB, OS_OPT_POST_NONE, &err);//������Ƕ�ź���
	}
}

//����DISPLAY���ƺ���
void Display_task(volatile void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, NULL, &err);//�ȴ���Ƕ�ź���
		OSSchedLock(&err);
		double_PID();
		ANO_DT_Send_Senser(Angle_x_temp,Angle_X_Final,(Angle_y_temp),(Angle_Y_Final),(Gyro_y),(d_pid.In_PID_Y));
		OSSchedUnlock(&err);
//		OSTaskSemPost(&MOTOR2TCB, OS_OPT_POST_NONE, &err);//������Ƕ�ź���
	}
}

//��ʱ��1�ص�����
void tmr1_callback_led(void *p_tmr, void *p_arg)
{
	LED = ~LED;
//	LIG = ~LIG;
}

//��ʼ������ƺ���
void start_task(volatile void *p_arg)
{
  OS_ERR err;
  CPU_SR_ALLOC();
  p_arg = p_arg;
  
  CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif

	OS_CRITICAL_ENTER();//�����ٽ������뱣��
	/*��������FILTER*/
	OSTaskCreate ((OS_TCB        *)&FILTERTCB,
								(CPU_CHAR      *)"FILTER",
								(OS_TASK_PTR    )filter_task,
								(void          *)0,
								(OS_PRIO        )FILTER_TASK_PRIO,
								(CPU_STK       *)FILTER_STK,
								(CPU_STK_SIZE   )FILTER_STK_SIZE/10,
								(CPU_STK_SIZE   )FILTER_STK_SIZE,
								(OS_MSG_QTY     )0,
								(OS_TICK        )0,
								(void          *)0,
								(OS_OPT         )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
								(OS_ERR        *)&err);
	/*��������DISPLAY*/
	OSTaskCreate ((OS_TCB        *)&DISPLAYTCB,
								(CPU_CHAR      *)"DISPLAY",
								(OS_TASK_PTR    )Display_task,
								(void          *)0,
								(OS_PRIO        )DISPLAY_TASK_PRIO,
								(CPU_STK       *)DISPLAY_STK,
								(CPU_STK_SIZE   )DISPLAY_STK_SIZE/10,
								(CPU_STK_SIZE   )DISPLAY_STK_SIZE,
								(OS_MSG_QTY     )0,
								(OS_TICK        )0,
								(void          *)0,
								(OS_OPT         )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
								(OS_ERR        *)&err);
	/*��������COLLECTION*/
	OSTaskCreate ((OS_TCB        *)&COLLECTIONTCB,
								(CPU_CHAR      *)"COLLECTION",
								(OS_TASK_PTR    )Collection_task,
								(void          *)0,
								(OS_PRIO        )COLLECTION_TASK_PRIO,
								(CPU_STK       *)COLLECTION_STK,
								(CPU_STK_SIZE   )COLLECTION_STK_SIZE/10,
								(CPU_STK_SIZE   )COLLECTION_STK_SIZE,
								(OS_MSG_QTY     )0,
								(OS_TICK        )0,
								(void          *)0,
								(OS_OPT         )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
								(OS_ERR        *)&err);
//	/*��������MOTOR1*/
//	OSTaskCreate ((OS_TCB        *)&MOTOR1TCB,
//								(CPU_CHAR      *)"MOTOR1",
//								(OS_TASK_PTR    )Motor1_task,
//								(void          *)0,
//								(OS_PRIO        )MOTOR1_TASK_PRIO,
//								(CPU_STK       *)MOTOR1_STK,
//								(CPU_STK_SIZE   )MOTOR1_STK_SIZE/10,
//								(CPU_STK_SIZE   )MOTOR1_STK_SIZE,
//								(OS_MSG_QTY     )0,
//								(OS_TICK        )0,
//								(void          *)0,
//								(OS_OPT         )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
//								(OS_ERR        *)&err);
//	/*��������MOTOR2*/
//	OSTaskCreate ((OS_TCB        *)&MOTOR2TCB,
//								(CPU_CHAR      *)"MOTOR2",
//								(OS_TASK_PTR    )Motor2_task,
//								(void          *)0,
//								(OS_PRIO        )MOTOR2_TASK_PRIO,
//								(CPU_STK       *)MOTOR2_STK,
//								(CPU_STK_SIZE   )MOTOR2_STK_SIZE/10,
//								(CPU_STK_SIZE   )MOTOR2_STK_SIZE,
//								(OS_MSG_QTY     )0,
//								(OS_TICK        )0,
//								(void          *)0,
//								(OS_OPT         )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
//								(OS_ERR        *)&err);
	/*���������ʱ��1*/
	OSTmrCreate	((OS_TMR         			 *)&tmr1,
							 (CPU_CHAR             *)"tmr1_led",
							 (OS_TICK               )0,//��ʼ�ӳ�Ϊ0*10ms
							 (OS_TICK               )50,//��ʱ����Ϊ50*10ms
							 (OS_OPT                )OS_OPT_TMR_PERIODIC,//����ģʽ
							 (OS_TMR_CALLBACK_PTR   )tmr1_callback_led,
							 (void                 *)0,
							 (OS_ERR               *)&err);
	OS_CRITICAL_EXIT();//�˳��ٽ������뱣��
	OSTmrStart(&tmr1,&err);
}
