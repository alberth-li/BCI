#ifndef POSITIONPID_C_INCLUDED
#define POSITIONPID_C_INCLUDED

#include "positionPID.h"

void pos_PID_InitController(pos_PID *pid, const tSensors sensor, const float kP, const float kI, const float kD, const float kBias, const int errorThreshold, const int integralLimit, const bool isEnabled)
{
	pid->kP = kP;
	pid->kI = kI;
	pid->kD = kD;
	pid->kBias = kBias;

	pid->error = 0;
	pid->prevError = 0;
	pid->integral = 0;
	pid->derivative = 0;

	pid->errorThreshold = errorThreshold;
	pid->integralLimit = integralLimit;

	pid->dt = 0;
	pid->prevTime = 0;

	pid->sensor = sensor;
	pid->targetPos = SensorValue[sensor];

	pid->outVal = 0;
}

void pos_PID_SetTargetPosition(pos_PID *pid, const int targetPos)
{
	pid->targetPos = targetPos;
}

int pos_PID_StepController(pos_PID *pid)
{
	//Calculate timestep
	pid->dt = (nSysTime - pid->prevTime) / 1000;
	pid->prevTime = nSysTime;

	//Calculate error
	pid->error = pid->targetPos - SensorValue[pid->sensor];

	//If error is large enough, calculate integral and limit to avoid windup
	if (abs(pid->error) > pid->errorThreshold && abs(pid->integral) < pid->integralLimit)
	{
		pid->integral = pid->integral + pid->error * pid->dt;
	}

	//Calculate derivative
	pid->derivative = (pid->error - pid->prevError) / pid->dt;
	pid->prevError = pid->error;

	//Calculate output
	pid->outVal = (pid->error * pid->kP) + (pid->integral * pid->kI) + (pid->derivative * pid->kD) + pid->kBias;

	return pid->outVal;
}


#endif //POSITIONPID_C_INCLUDED