#ifndef MATEM_H
#define MATEM_H

#ifndef MSVC
	#define ARM_MATH_CM4
	#include "MK70F15.h"                    // Device header
	#include "arm_math.h"                   // ARM::CMSIS:DSP	
#else
//	#include <math.h>
#endif

#include <math.h>

#define MATH_Pi 3.14159265359F //число Пи

#endif //MATEM

