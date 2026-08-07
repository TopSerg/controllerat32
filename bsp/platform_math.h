/*
 * artery_patform.h
 *
 *  Created on: 2024 Jan 20
 *      Author: Andrey
 */

#ifndef BSP_ARTERY_PATFORM_H_
#define BSP_ARTERY_PATFORM_H_

#include  <math.h>
#include "arm_math.h"

#define	platform_abs(x)	( ((x)>0)? (x): (-(x)) )
#define	sign(x)	( ((x)>=0)? (1): (-(1)) )
#define platform_min(a,b) ((a) < (b) ? (a) : (b))
#define platform_max(a,b) ((a) > (b) ? (a) : (b))

__STATIC_INLINE float  platform_sin(float angle)
{
	return arm_sin_f32(angle);
}
__STATIC_INLINE float platform_cos(float32_t angle)
{
	return arm_cos_f32(angle);
}
__STATIC_INLINE float platform_sqrt(float val)
{
	float out;
	arm_sqrt_f32(val, &out);
	return out;
}
__STATIC_INLINE float platform_exp(float val)
{
	//return arm_exponent_f32(val, 1);
	return expf(val);
}

#define PI_FLOAT 3.14159265f
#define PIBY2_FLOAT 1.5707963f
// |error| < 0.005
__STATIC_INLINE float platform_atan2(float y, float x)
{
	if (x == 0.0f)
	{
		if (y > 0.0f) return PIBY2_FLOAT;
		if (y == 0.0f) return 0.0f;
		return -PIBY2_FLOAT;
	}
	float atan;
	float z = y / x;
	if (fabs(z) < 1.0f)
	{
		atan = z / (1.0f + 0.28f*z*z);
		if (x < 0.0f)
		{
			if (y < 0.0f) return atan - PI_FLOAT;
			return atan + PI_FLOAT;
		}
	}
	else
	{
		atan = PIBY2_FLOAT - z / (z*z + 0.28f);
		if (y < 0.0f) return atan - PI_FLOAT;
	}
	return atan;
}
#endif /* BSP_ARTERY_PATFORM_H_ */
