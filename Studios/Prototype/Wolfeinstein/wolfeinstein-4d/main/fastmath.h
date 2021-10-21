#ifndef FASTMATH_H
#define FASTMATH_H

float pow(float operand, int degree)
{
    if (degree == 1) return operand;
    return operand * pow(operand, degree-1);
}

#define SQRT_MAGIC_F 0x5f3759df
float sqrt(const float x)
{
  const float xhalf = 0.5f*x;

  union // get bits for floating value
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
  return x*u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy
}
#endif // FASTMATH_H
