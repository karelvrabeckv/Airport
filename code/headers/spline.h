#pragma once

#include "pgr.h"

using namespace glm;

// ========================================

bool isVectorNull(const vec3 &vect);
mat4 alignObject(const vec3 &position, const vec3 &front, const vec3 &up);
vec3 evaluateCurveSegment(const vec3 &P0, const vec3 &P1, const vec3 &P2, const vec3 &P3, const float t);
vec3 evaluateCurveSegment_1stDerivative(const vec3 &P0, const vec3 &P1, const vec3 &P2, const vec3 &P3, const float t);
vec3 evaluateClosedCurve(const vec3 points[], const size_t count, const float t);
vec3 evaluateClosedCurve_1stDerivative(const vec3 points[], const size_t count, const float t);
