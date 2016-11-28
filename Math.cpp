#include "Math.h"

float min(float x, float y) {
	return ci::math<float>::min(x, y);
}

float max(float x, float y) {
	return ci::math<float>::max(x, y);
}

float clamp(float x, float min, float max) {
	return ci::math<float>::clamp(x, min, max);
}

float abs(float x) {
	return ci::math<float>::abs(x);
}

float map(float val, float inMin, float inMax, float outMin, float outMax, bool clamp) {
	float outVal = ci::lmap<float>(val, inMin, inMax, outMin, outMax);
	if (clamp) {
		if (outMax < outMin) {
			if (outVal < outMax) {
				outVal = outMax;
			} else if (outVal > outMin) {
				outVal = outMin;
			}
		} else {
			if (outVal > outMax) {
				outVal = outMax;
			} else if (outVal < outMin) {
				outVal = outMin;
			}
		}
	}
	return outVal;
}

float lerp(float a, float b, float amount) {
	return ci::lerp(a, b, amount);
}

vec2 perpendicular(vec2 vec) {
	float length = glm::length(vec);
	if (length > 0) {
		return vec2(-vec.y / length, vec.x / length);
	} else {
		return vec2(0);
	}
}

float angleClockwise(vec2 direction) {
	float angle = atan2(direction.y, direction.x);
	angle += M_PI * 0.5;
	if (angle < 0.0) {
		angle += M_PI * 2;
	} else if (angle > M_PI * 2) {
		angle -= M_PI * 2;
	}
	return angle;
}
