void intake_in() {
	motor[intakeLeft] = -100;
	motor[intakeRight] = -100;
}

void intake_out() {
	motor[intakeLeft] = 100;
	motor[intakeRight] = 100;
}

void intake_stop() {
	motor[intakeLeft] = 0;
	motor[intakeRight] = 0;
}
