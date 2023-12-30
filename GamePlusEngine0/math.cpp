
#include "math.h"


START_NAMESPACE


float lerp(float a, float b, float t) {
	return (1.0f - t) * a + b * t;
}

float clamp(float p) {
	if (p < 0) p = 0.0f;
	if (p > 1) p = 1.0f;
	return p;
}

END_NAMESPACE