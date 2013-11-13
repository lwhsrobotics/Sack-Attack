#include "PID/pid.c"

TSemaphore lift_semaphore;

PidState liftStateLeft;
PidState liftStateRight;

int LIFT_DT = 15;

const int LIFT_MIN = 850;
const int LIFT_MAX = 2500;

const int LIFT_BOTTOM = LIFT_MAX;
const int LIFT_MID = (LIFT_MAX + LIFT_MIN) / 2;
const int LIFT_TOP = LIFT_MIN;

const int LIFT_DOWN_THRESHOLD = 100;
const int LIFT_DOWN_SPEED = 15;

const int LIFT_SCORING = LIFT_TOP;
const int LIFT_HOLDING = LIFT_MID;
const int LIFT_PICKING = LIFT_BOTTOM;

float LIFT_KP = 0.18;
float LIFT_KI = 0.06;
float LIFT_KD = 12;

const int LIFT_INTEGRAL_THRESHOLD = 150;

#define LIFT_ACQUIRE bool __lift_had_semaphore = lift_acquire_semaphore();
#define LIFT_RELEASE if(!__lift_had_semaphore) lift_release_semaphore();

bool _lift_static_enabled;
int _lift_static_speed;

bool lift_acquire_semaphore(int timeout) {
	if (!bDoesTaskOwnSemaphore(lift_semaphore))
		SemaphoreLock(lift_semaphore, timeout);
  return bDoesTaskOwnSemaphore(lift_semaphore);
}

bool lift_acquire_semaphore() {
	return lift_acquire_semaphore(36000);
}

bool lift_release_semaphore() {
	if (bDoesTaskOwnSemaphore(lift_semaphore))
		SemaphoreUnlock(lift_semaphore);
  return !bDoesTaskOwnSemaphore(lift_semaphore);
}

bool lift_static_enabled() {
	return _lift_static_enabled;
}

int lift_static_speed() {
	return _lift_static_speed;
}

void lift_static_enable(int speed) {
	LIFT_ACQUIRE

	_lift_static_enabled = true;
	_lift_static_speed = speed;

	LIFT_RELEASE
}

void lift_static_disable() {
	LIFT_ACQUIRE

	_lift_static_enabled = false;

	LIFT_RELEASE
}

void lift_init() {
  pidInitState(liftStateLeft, LIFT_KP, LIFT_KI, LIFT_KD, LIFT_INTEGRAL_THRESHOLD);
	pidInitState(liftStateRight, LIFT_KP, LIFT_KI, LIFT_KD, LIFT_INTEGRAL_THRESHOLD);
	pidSetTarget(liftStateLeft, SensorValue[liftPotLeft]);
	pidSetTarget(liftStateRight, SensorValue[liftPotRight]);
	lift_static_disable();
}

void lift_set_target(int target) {
	LIFT_ACQUIRE

	if (target <= LIFT_MAX && target >= LIFT_MIN) {
		pidSetTarget(liftStateLeft, target);
		pidSetTarget(liftStateRight, target);
	}

	LIFT_RELEASE
}

void lift_manual(int y) {
  // reset static
  if(lift_static_enabled()) lift_static_disable();

	int oldLeft  = liftStateLeft.target;
	int oldRight = liftStateRight.target;

	int offset = y;

	if (!vexRT[Btn7U]) {
  	if (offset < 0) {
			int minValue = min(oldLeft, oldRight);
			if (minValue + offset < LIFT_MIN) {
				offset = LIFT_MIN - minValue;
			}
		} else if (offset > 0) {
	 	  int maxValue = max(oldLeft, oldRight);
	  	if (maxValue + offset > LIFT_MAX) {
	  		offset = LIFT_MAX - maxValue;
	  		// hard code speeds if at bottom
	  		if(abs(liftStateLeft.last_error + liftStateRight.last_error) < LIFT_DOWN_THRESHOLD * 2) {
	  			lift_static_enable(LIFT_DOWN_SPEED);
	  		}
	  	}
		}
	}

	pidIncrementTarget(liftStateLeft, offset);
	pidIncrementTarget(liftStateRight, offset);
}

void lift_increment_target(int amount) {
	LIFT_ACQUIRE

	if (amount + pidGetTarget(liftStateLeft) < LIFT_MAX && amount + pidGetTarget(liftStateRight) > LIFT_MIN) {
		pidIncrementTarget(liftStateLeft, amount);
		pidIncrementTarget(liftStateRight, amount);
	}

	LIFT_RELEASE
}

bool lift_at_target(int threshold) {
	LIFT_ACQUIRE

	int dLeft = abs(SensorValue[liftPotLeft] - pidGetTarget(liftStateLeft));
	int dRight = abs(SensorValue[liftPotRight] - pidGetTarget(liftStateRight));
	bool atTarget = dLeft < threshold || dRight < threshold;

	LIFT_RELEASE

	return atTarget;
}

void lift_set_motors(int speed) {
	motor[liftLeft] = speed;
	motor[liftRight] = speed;
}

int liftOutputLeft = 0, liftOutputRight = 0;

task lift() {
	SemaphoreInitialize(lift_semaphore);

	while (true) {
		if(lift_static_enabled()) {
			lift_set_motors(lift_static_speed());
		} else {
			motor[liftLeft] = pid(liftStateLeft, SensorValue[liftPotLeft], LIFT_DT);
			motor[liftRight] = pid(liftStateRight, SensorValue[liftPotRight], LIFT_DT);
		}

		liftOutputLeft = motor[liftLeft];
		liftOutputRight = motor[liftRight];

		wait1Msec(LIFT_DT);
  }
}
