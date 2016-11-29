#pragma once

#include <glm.hpp>

#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef DEGTORAD
#define DEGTORAD(degree) ((degree * PI) / 180.0f)
#endif

#ifndef RADTODEG
#define RADTODEG(rad) ((rad * 180.0f) / PI)
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif

#ifndef CLAMP
#define CLAMP(value, max, min) (MIN(max, MAX(value, min)))
#endif


inline void ClampVector(glm::vec3 & vector, float degreeMax, float degreeMin) {
	vector.x = CLAMP(vector.x, degreeMax, degreeMin);
	vector.y = CLAMP(vector.y, degreeMax, degreeMin);
	vector.z = CLAMP(vector.z, degreeMax, degreeMin);
}

