#ifndef _MATH_
#define _MATH_

/* Returns the maximum of the two integers. */
int max(int a, int b) {
  return (a > b ? a : b);
}

/* Returns the minimum of the two integers. */
int min(int a, int b) {
  return (a < b ? a : b);
}

/* Clamps the integer n to range [a, b]. */
int clamp(int n, int a, int b) {
  if(n < a) return a;
  if(n > b) return b;
  return n;
}

/* Clamps the integer n to a valid motor speed range. */
int clampMotor(int n) {
  return clamp(n, -127, 127);
}

/* Clamps the integer n to a range b +- c. */
int clampChange(int n, int b, int c) {
  return clamp(n, b - c, b + c);
}

/* Returns the sign (-1, 0, 1) of the integer n. */
int signum(int n) {
	return (n > 0) - (n < 0);
}

int scaleQuad(int maxSpeed, float coeff, float completion) {
	return maxSpeed * (-coeff * (completion * completion) + 1);
}

int scaleQuad(int maxSpeed, float completion) {
	return scaleQuad(maxSpeed, 0.6, completion);
}

#endif // _MATH_
