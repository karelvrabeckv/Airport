#include "headers/spline.h"

// ========================================

/** Checks whether vector is zero-length or not. */
bool isVectorNull(const vec3 &vect)
{
  return !vect.x && !vect.y && !vect.z;
} // IS VECTOR NULL

// ========================================

/** Align (rotate and move) current coordinate system to given parameters. */
mat4 alignObject(const vec3 &position, const vec3 &front, const vec3 &up)
{
  vec3 z = -normalize(front);
  if (isVectorNull(z))
    z = vec3(0.0, 0.0, 1.0);

  vec3 x = normalize(cross(up, z));
  if (isVectorNull(x))
    x =  vec3(1.0, 0.0, 0.0);

  vec3 y = cross(z, x);

  mat4 matrix = mat4
  (
      x.x, x.y, x.z, 0.0,
      y.x, y.y, y.z, 0.0,
      z.x, z.y, z.z, 0.0,
      position.x, position.y, position.z, 1.0
  );

  return matrix;
} // ALIGN OBJECT

// ========================================

/** Evaluates a position on Catmull-Rom curve segment. */
vec3 evaluateCurveSegment(const vec3 &P0, const vec3 &P1, const vec3 &P2, const vec3 &P3, const float t)
{
  vec3 result(0.0, 0.0, 0.0);

  const vec4 params = vec4(pow(t, 3), pow(t, 2), t, 1);
  const mat4 values = transpose(mat4
  (
    -1.0f,  3.0f, -3.0f,  1.0f,
     2.0f, -5.0f,  4.0f, -1.0f,
    -1.0f,  0.0f,  1.0f,  0.0f,
     0.0f,  2.0f,  0.0f,  0.0f
  ));
  const mat3x4 points = transpose(mat4x3(P0, P1, P2, P3));

  return 0.5f * params * values * points;
} // EVALUATE CURVE SEGMENT

// ========================================

/** Evaluates a first derivative of Catmull-Rom curve segment. */
vec3 evaluateCurveSegment_1stDerivative(const vec3 &P0, const vec3 &P1, const vec3 &P2, const vec3 &P3, const float t)
{
  vec3 result(1.0, 0.0, 0.0);
  
  const vec4 params = vec4(3 * pow(t, 2), 2 * t, 1, 0);
  const mat4 values = transpose(mat4
  (
    -1.0f,  3.0f, -3.0f,  1.0f,
     2.0f, -5.0f,  4.0f, -1.0f,
    -1.0f,  0.0f,  1.0f,  0.0f,
     0.0f,  2.0f,  0.0f,  0.0f
  ));
  const mat3x4 points = transpose(mat4x3(P0, P1, P2, P3));

  return 0.5f * params * values * points;
} // EVALUATE CURVE SEGMENT 1ST DERIVATIVE

// ========================================

/** Evaluates a position on a closed curve composed of Catmull-Rom segments. */
vec3 evaluateClosedCurve(const vec3 points[], const size_t count, const float t)
{
  vec3 result(0.0, 0.0, 0.0);

  size_t i = int(t) % count;
  result = evaluateCurveSegment
  (
    points[(i - 1 + count) % count],
    points[(i) % count],
    points[(i + 1) % count],
    points[(i + 2) % count],
    t - floor(t)
  );

  return result;
} // EVALUATE CLOSED CURVE

// ========================================

/** Evaluates a first derivative of a closed curve composed of Catmull-Rom segments. */
vec3 evaluateClosedCurve_1stDerivative(const vec3 points[], const size_t count, const float t)
{
  vec3 result(1.0, 0.0, 0.0);

  size_t i = int(t) % count;
  result = evaluateCurveSegment_1stDerivative
  (
    points[(i - 1 + count) % count],
    points[(i) % count],
    points[(i + 1) % count],
    points[(i + 2) % count],
    t - floor(t)
  );

  return result;
} // EVALUATE CLOSED CURVE 1ST DERIVATIVE
