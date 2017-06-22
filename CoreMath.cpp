#include "CoreMath.h"

// Implementation of the following functions was taken from code on canvas: https://github.com/codeoncanvas/coc-core
/* Copyright (c) 2016 Code on Canvas Pty Ltd, http://CodeOnCanvas.cc

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE. */

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

// End of CoC-sourced functions

// maps [-1, 1] to [0, 1]
float map01(float t) {
	return (t + 1.0) * 0.5;
}

// maps [0, 1] to [1, 0]
float inv(float t) {
	return 1.0 - t;
}

vec3 getPointOnSphere(float vertAng, float horAng) {
	float sinVert = sin(vertAng);
	return vec3(sinVert * cos(horAng), cos(vertAng), sinVert * sin(horAng));
}

double secFromHMS(double hours, double minutes, double seconds) {
	return hours * 60.0 * 60.0 + minutes * 60.0 + seconds;
}