#ifndef _COMMON_
#define _COMMON_

// Motor Speeds
#define MAX_SPEED 127
#define NO_SPEED 0
#define MIN_SPEED -127

#define ON 1
#define OFF 0

// Button States
#define PRESSED 1
#define NOT_PRESSED 0

// Complete Revolution (360 deg) of a Quadrature Encoder
#define ENCODER_REVOLUTION 360
// Circumference of a Wheel (diameter * pi)
#define WHEEL_CIRCUMFERENCE 12.57
// Diameter of a Wheel
#define WHEEL_DIAMETER 4

typedef enum {
  UP = 1,
  DOWN = -1,
  STABLE = 0
} OperationState;

typedef struct {
  int left;
  int right;
} LeftRightPair;

const int JOYSTICK_TOLERANCE = 20;

#endif // _COMMON_
