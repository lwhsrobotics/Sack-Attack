void drive_motors(int speed) {
	motor[driveFrontRight] = speed;
	motor[driveBackRight]  = speed;
	motor[driveFrontLeft]  = speed;
	motor[driveBackLeft]   = speed;
}

void drive_manual(int leftX, int leftY, int rightX) {
	if (abs(leftX) < JOYSTICK_TOLERANCE)
		leftX = 0;
	if (abs(leftY) < JOYSTICK_TOLERANCE)
		leftY = 0;
	if (abs(rightX) < JOYSTICK_TOLERANCE)
		rightX = 0;

	motor[driveFrontRight] = leftY - rightX - leftX;
	motor[driveBackRight]  = leftY - rightX + leftX;
	motor[driveFrontLeft]  = leftY + rightX + leftX;
	motor[driveBackLeft]   = leftY + rightX - leftX;

}

void drive_straight(int distance, int speed) {
	SensorValue[driveEncBackRight] = 0;
	wait1Msec(100);

	int distanceTicks = distance * ENCODER_REVOLUTION / WHEEL_CIRCUMFERENCE;
	int direction = signum(distance);
	int absDistanceTicks = abs(distanceTicks);

	while (true) {
		int absEncoder = abs(SensorValue[driveEncBackRight]);
		float completion = (float) absEncoder / absDistanceTicks;

		if (completion > 1) break;

		drive_motors(direction * scaleQuad(speed, completion, 0.4));
	}

	drive_motors(0);
}

void doForTicks(int ticks) {
	while (abs(motor[driveBackRight]) + abs(motor[driveBackLeft]) + abs(motor[driveFrontRight]) + abs(motor[driveFrontLeft])) {
		if (abs(SensorValue[driveEncBackRight]) > ticks)
			motor[driveBackRight] = 0;
		if (abs(SensorValue[driveEncBackLeft]) > ticks)
			motor[driveBackLeft] = 0;
		if (abs(SensorValue[driveEncFrontRight]) > ticks)
			motor[driveFrontRight] = 0;
		if (abs(SensorValue[driveEncFrontLeft]) > ticks)
			motor[driveFrontLeft] = 0;
	}
}

void drive_strafe(int distance, int speed) {
	int distanceTicks = abs(distance * ENCODER_REVOLUTION / WHEEL_CIRCUMFERENCE * 2);

	speed *= signum(distance);

	float numCycles = distanceTicks / 100;

	for (float numCycles = distanceTicks / 100; numCycles >= 1; numCycles--) {
		SensorValue[driveEncBackRight]  = 0;
		SensorValue[driveEncBackLeft]   = 0;
		SensorValue[driveEncFrontRight] = 0;
		SensorValue[driveEncFrontLeft]  = 0;

		motor[driveBackRight]  =  speed;
		motor[driveBackLeft]   = -speed;
		motor[driveFrontRight] = -speed;
		motor[driveFrontLeft]  =  speed;

		doForTicks(100);
	}

	doForTicks(100 * numCycles);
}

void strafe_right(int distance, int speed) {
	int distanceTicks = distance * ENCODER_REVOLUTION / WHEEL_CIRCUMFERENCE;

	motor[driveBackLeft] = speed;
	motor[driveFrontLeft] = speed;
	motor[driveBackRight] = -speed;
	motor[driveFrontRight] = -speed;

	while (SensorValue[driveBackRight] < distanceTicks);

	motor[driveBackLeft] = 0;
	motor[driveFrontLeft] = 0;
	motor[driveBackRight] = 0;
	motor[driveFrontRight] = 0;
}

void strafe_left(int distance, int speed) {
	int distanceTicks = distance * ENCODER_REVOLUTION / WHEEL_CIRCUMFERENCE;

	motor[driveBackLeft] = -speed;
	motor[driveFrontLeft] = -speed;
	motor[driveBackRight] = speed;
	motor[driveFrontRight] = speed;

	while (SensorValue[driveBackRight] < distanceTicks);

	motor[driveBackLeft] = 0;
	motor[driveFrontLeft] = 0;
	motor[driveBackRight] = 0;
	motor[driveFrontRight] = 0;
}

void drive_turn(int degrees, int speed) {
	int encoderTicks = degrees * sqrt(2) * PI * 16 / 135 * 18 * 2;

	SensorValue[driveEncBackRight] = 0;
	SensorValue[driveEncBackLeft] = 0;
	SensorValue[driveEncFrontRight] = 0;
	SensorValue[driveEncFrontLeft] = 0;

	if (degrees > 0) {
		motor[driveBackRight] = -speed;
		motor[driveFrontRight] = -speed;
		motor[driveBackLeft] = speed;
		motor[driveFrontLeft] = speed;

		while (-(SensorValue[driveEncBackRight] + SensorValue[driveEncFrontRight]) / 2 - (SensorValue[driveEncBackLeft] + SensorValue[driveEncFrontLeft]) / 2 < encoderTicks);
	} else if (degrees < 0) {
		motor[driveBackRight] = speed;
		motor[driveFrontRight] = speed;
		motor[driveBackLeft] = -speed;
		motor[driveFrontLeft] = -speed;

		while (-(SensorValue[driveEncBackRight] + SensorValue[driveEncFrontRight]) / 2 - (SensorValue[driveEncBackLeft] + SensorValue[driveEncFrontLeft]) / 2 > encoderTicks);
	}

	motor[driveBackRight] = 0;
	motor[driveFrontRight] = 0;
	motor[driveBackLeft] = 0;
	motor[driveFrontLeft] = 0;
}
