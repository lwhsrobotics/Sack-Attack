#ifndef _PID_
#define _PID_

#include "../math.c"

/***************************************************************************
 * The length of the error log for the calculation of the derivative term. *
 * Set to 1 to record only the previous error.                             *
 ***************************************************************************/
const int ERROR_LOG_LEN = 1;

/***************************************************************************
 * Structure to contain important information for debugging PID.           *
 *                                                                         *
 * Tracks the values of the P, I, and D terms, both before (raw) and after *
 * multiplication by the K constants, as well as the value of the output   *
 * of the PID function.                                                    *
 ***************************************************************************/
typedef struct {
	int output;
	float p;
	float p_raw;
	float d;
	float d_raw;
	float i;
	float i_raw;
} pid_debug_t;

/**************************************************************************
 * Structure to contain information for PID.                              *
 *                                                                        *
 * By default this will create an invalid structure.  Run pidInitState on *
 * it before using the structure.                                         *
 **************************************************************************/
typedef struct {
  float Kp;
  float Ki;
  float Kd;

  int last_error;

  float integral;

  int target;

  int previous_errors[ERROR_LOG_LEN];

  int integral_threshold;

  pid_debug_t debug;
} PidState;

/**************************************
 * Appends an error to the error log. *
 **************************************/
void pidAppendError(PidState* state, int error) {
  int i = 0;
  for(; i < ERROR_LOG_LEN - 1; ++i) {
    state->previous_errors[i] = state->previous_errors[i+1];
  }

  state->previous_errors[i] = error;
}

/***************************************
 * Sums the errors from the error log. *
 ***************************************/
int pidSumErrors(PidState* state) {
  int sum = 0;

  for (int i = 0; i < ERROR_LOG_LEN; ++i) {
    sum += state->previous_errors[i];
  }

  return sum;
}

/**************************************************************************************************
 * Initialize the PID state using the three K constants and a minimum/maximum integral threshold. *
 **************************************************************************************************/
void pidInitState(PidState* state, float Kp, float Ki, float Kd, int integral_threshold) {
  state->Kp = Kp;
  state->Ki = Ki;
  state->Kd = Kd;

  state->last_error = 0;
  state->integral = 0;
  state->target = 0;

  state->integral_threshold = integral_threshold;

  for (int i = 0; i < ERROR_LOG_LEN; ++i) {
    state->previous_errors = 0;
  }
}

/*********************************************
 * Returns the target value of the PidState. *
 *********************************************/
int pidGetTarget(PidState* state) {
  return state->target;
}

/*****************************
 * Set the PID target value. *
 *****************************/
void pidSetTarget(PidState* state, int target) {
  if (state->target != target) {
    state->target = target;
    state->integral = 0;
  }
}

/*******************************************
 * Increment PID target by a given amount. *
 *******************************************/
void pidIncrementTarget(PidState* state, int amount) {
  pidSetTarget(state, state->target + amount);
}

/***************************************************
 * Calculate PID.                                  *
 *                                                 *
 * pos: Real position to calculate error from.     *
 * dt: Time in milliseconds since the last update. *
 ***************************************************/
int pid(PidState* state, int pos, int dt) {
  // calculate error between target and actual position
  int error = state->target - pos;

  // append new error and calculate average
  int previous_errors_avg = pidSumErrors(state) / ERROR_LOG_LEN;
  pidAppendError(state, error);

  // calculate integral
  state->integral += (float) (error * dt) / 1000; // dt is in ms

  // reset integral if error has changed signs from previous error
  if (signum(state->last_error) != signum(error)) {
    state->integral = 0;
  }

  // reset integral if integral is outside of threshold
  if (abs(state->integral) > state->integral_threshold) {
    state->integral = 0;
  }

  // calculate derivative from average of previous errors
  float derivative = (float) (error - previous_errors_avg) / dt;

  // for debugging
  state->debug.p = state->Kp * error;
  state->debug.p_raw = error;
  state->debug.i = state->Ki * state->integral;
  state->debug.i_raw = state->integral;
  state->debug.d = state->Kd * derivative;
  state->debug.d_raw = error - previous_errors_avg;

  // calculate the output
  int output = state->Kp * error + state->Ki * state->integral + state->Kd * derivative;

  // constrict output to [-127, 127]
  if(output > 127) output = 127;
  if(output < -127) output = -127;

  // for debugging
  state->debug.output = output;

  // set new previous error
  state->last_error = error;

  return output;
}

#endif  // _PID_
