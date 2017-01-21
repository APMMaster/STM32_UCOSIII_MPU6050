
#include "PID.h"
#include "math.h"

#define RANGE_CONTRL(x)	((x>900)?(900):((x<200)?200:x))

//����PID�ṹ��
struct PID pid;
struct D_PID d_pid;
float Speed;
float Displacement;

/**
  ************************************
  * @brief  PID������ʼ��
  * @param  None
  * @retval None
	************************************
*/
void PID_init(void)
{
	pid.SetSpeed = 0.0;
	pid.ActualSpeed = 0.0;
	pid.err = 0.0;
	pid.err_last = 0.0;
	pid.Kp = 10;
	pid.Ki = 0.0;
	pid.Kd = 0.0;
	pid.Kpp = 0.0;
}

/**
  ************************************
  * @brief  PID������ʼ��
  * @param  None
  * @retval None
	************************************
*/
void D_PID_Init(void)
{
	d_pid.Errory_In = 0.0;
	d_pid.Errory_Out = 0.0;
	d_pid.Gro_y = 0.0;
	d_pid.In_PID_Y = 0.0;
	d_pid.In_Yd = 0;
	d_pid.In_Yi = 0;
	d_pid.In_Yp = 10;
	d_pid.Last_Gro_y = 0.0;
	d_pid.Out_Pi_Y = 0.0;
	d_pid.Out_Yi = 0.0;
	d_pid.Out_Yp = 20;
}

/**
  ************************************
  * @brief  ����PID���㹫ʽ
  * @param  None
  * @retval None
	************************************
*/
float PID_realize(float speed)
{
//	pid.SetSpeed = speed;
////	pid.ActualSpeed = Angle_X_Final;
//	pid.err = pid.SetSpeed-pid.ActualSpeed;
//	float incrementSpeed = pid.Kp*(pid.err - pid.err_next) + pid.Ki*pid.err + pid.Kd*(pid.err - 2*pid.err_next + pid.err_last);
//	pid.ActualSpeed += incrementSpeed;
//	pid.err_last = pid.err_next;
//	pid.err_next = pid.err;
//	return pid.ActualSpeed;
	
	pid.SetSpeed = speed;
	pid.err = pid.SetSpeed - pid.ActualSpeed;
	pid.integral += pid.err;
	pid.pwm = pid.Kp*pid.err + pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_last);
	pid.err_last = pid.err;
	pid.ActualSpeed = pid.pwm*1.0;
	return pid.ActualSpeed;
}

/**
  ************************************
  * @brief  PID���Ƶ��
  * @param  None
  * @retval None
	************************************
*/
void PWM_Control(int16_t speed)
{
	if(speed < 0)
	{
		MOTOR2_FORWARD((-speed) + DEAD_VOLTAGE);
		return;
	}
	MOTOR2_REVERSE(speed + DEAD_VOLTAGE);
}

//void PID_Colu(void)
//{
//	int16_t pwm = 0;
////	Displacement += Speed*0.5;
////	pwm = (int16_t)(pid.Kp*Angle_Y_Final + pid.Kd*Gyro_y + pid.Kpp*Displacement);
//	pwm = 20*Angle_Y_Final;
//	PWM_Control(pwm);
//}

/**
  ************************************
  * @brief  ����PID
  * @param  None
  * @retval None
	************************************
*/
void double_PID(void)
{
//	/*������һ������*/
//	d_pid.Last_Gro_y = d_pid.Gro_y;
//	d_pid.Gro_y = gyroy;
//	/*�⻷PI*/
//	d_pid.Errory_Out += Angle_Y_Final;					//�⻷������
//	Integral_Limiting(d_pid.Errory_Out, 500);		//�����޷�
//	d_pid.Out_Pi_Y = Angle_Y_Final*d_pid.Out_Yp + d_pid.Errory_Out*d_pid.Out_Yi;//�⻷PI
//	Integral_Limiting(d_pid.Out_Pi_Y, 900);			//�����޷�
//	/*�ڻ�PID*/
//	d_pid.Errory_In += (d_pid.Gro_y - d_pid.Out_Pi_Y);	//�ڻ�������
//	Integral_Limiting(d_pid.Errory_In, 500);		//�����޷�
//	d_pid.In_PID_Y = (d_pid.Gro_y + d_pid.Out_Pi_Y)*d_pid.In_Yp + d_pid.Errory_In*d_pid.In_Yi + (d_pid.Gro_y - d_pid.Last_Gro_y)*d_pid.In_Yd;//�ڻ�PID
//	Integral_Limiting(d_pid.In_PID_Y, 900);			//�����޷�
//	PWM_Control(d_pid.In_PID_Y);
	
	d_pid.Last_Gro_y = d_pid.Gro_y;
	d_pid.Gro_y = Gyro_y;
	d_pid.Errory_In += Angle_Y_Final;		//������
	Integral_Limiting(d_pid.Errory_In, 500);		//�����޷�
	d_pid.In_PID_Y = d_pid.In_Yp*Gyro_y + d_pid.In_Yi*d_pid.Errory_In + d_pid.In_Yd*(Gyro_y-d_pid.Last_Gro_y);//PID���Ĺ�ʽ
	Integral_Limiting(d_pid.In_PID_Y, 500);		//����޷�
	PWM_Control(d_pid.In_PID_Y);
}
