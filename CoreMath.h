#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "cinder/CinderMath.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

#define M_TWO_PI 2.0 * M_PI
#define M_HALF_PI M_PI_2 

// Implementation of the following functions was taken from code on canvas: https://github.com/codeoncanvas/coc-core
float min(float x, float y);
float max(float x, float y);
float clamp(float x, float min = 0, float max = 1);
float abs(float x);
float map(float val, float inMin, float inMax, float outMin, float outMax, bool clamp = false);
float lerp(float a, float b, float amount);
vec2 perpendicular(vec2 vec);
float angleClockwise(vec2 direction);
// End of CoC-sourced functions