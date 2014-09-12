#include "MeasAlgs.h"

static Fifo_int signalsFifo[DEV_numberAnalogChannels];
static int signalsBufs[DEV_numberAnalogChannels][BUFSIZE_HI];

static unsigned int measNsignalsRunCounter;

//Обявлено для внутреннего использования
ComplexF fourierX32s(float * buf, const float * sinCoef, const float *cosCoef);

// region Foureier coefficients
//Синусные коэффициенты для измерительного органа Фурье для 1-й гармоники для 32 выборок за период
static const float fft32sin_coef[32]=
{
0.0000002,	-0.1950902,	-0.3826833,	-0.5555701,	-0.7071067,	-0.8314695,	-0.9238795,	-0.9807853,	
-1.0000000,	-0.9807853,	-0.9238796,	-0.8314697,	-0.7071069,	-0.5555703,	-0.3826835,	-0.1950904,	
-0.0000001,	0.1950902,	0.3826834,	0.5555702,	0.7071067,	0.8314696,	0.9238795,	0.9807853,	
1.0000000,	0.9807853,	0.9238795,	0.8314696,	0.7071068,	0.5555702,	0.3826834,	0.1950903	
};
//Косиинусные коэффициенты для измерительного органа Фурье для 1-й гармоники для 32 выборок за период
static const float fft32cos_coef[32]=
{
1.0000000,	0.9807853,	0.9238796,	0.8314697,	0.7071069,	0.5555704,	0.3826836,	0.1950905,	
0.0000001,	-0.1950902,	-0.3826833,	-0.5555701,	-0.7071067,	-0.8314696,	-0.9238795,	-0.9807853,	
-1.0000000,	-0.9807853,	-0.9238796,	-0.8314697,	-0.7071068,	-0.5555703,	-0.3826835,	-0.1950904,	
-0.0000000,	0.1950903,	0.3826834,	0.5555702,	0.7071068,	0.8314696,	0.9238795,	0.9807853		
};

//Синусные коэффициенты для измерительного органа Фурье для 2-й гармоники для 32 выборок за период
static const float fft32_2_sin_coef[32]=
{
0.0000003,	-0.3826831,	-0.7071065,	-0.9238794,	-1.0000000,	-0.9238796,	-0.7071070,	-0.3826837,	
-0.0000003,	0.3826832,	0.7071066,	0.9238794,	1.0000000,	0.9238796,	0.7071069,	0.3826836,	
0.0000002,	-0.3826833,	-0.7071067,	-0.9238795,	-1.0000000,	-0.9238796,	-0.7071069,	-0.3826835,	
-0.0000001,	0.3826834,	0.7071067,	0.9238795,	1.0000000,	0.9238795,	0.7071068,	0.3826834	
};
//Косиинусные коэффициенты для измерительного органа Фурье для 2-й гармоники для 32 выборок за период
static const float fft32_2_cos_coef[32]=
{
1.0000000,	0.9238797,	0.7071070,	0.3826837,	0.0000003,	-0.3826832,	-0.7071066,	-0.9238794,	
-1.0000000,	-0.9238796,	-0.7071070,	-0.3826836,	-0.0000002,	0.3826832,	0.7071066,	0.9238795,	
1.0000000,	0.9238796,	0.7071069,	0.3826836,	0.0000001,	-0.3826833,	-0.7071067,	-0.9238795,	
-1.0000000,	-0.9238796,	-0.7071068,	-0.3826835,	-0.0000000,	0.3826834,	0.7071068,	0.9238795		
};

//Синусные коэффициенты для измерительного органа Фурье для 3-й гармоники для 32 выборок за период
static const float fft32_3_sin_coef[32]=
{
0.0000005,	-0.5555698,	-0.9238793,	-0.9807854,	-0.7071071,	-0.1950908,	0.3826830,	0.8314694,	
1.0000000,	0.8314698,	0.3826838,	-0.1950900,	-0.7071065,	-0.9807852,	-0.9238796,	-0.5555705,	
-0.0000003,	0.5555700,	0.9238794,	0.9807853,	0.7071069,	0.1950905,	-0.3826833,	-0.8314695,	
-1.0000000,	-0.8314697,	-0.3826835,	0.1950902,	0.7071067,	0.9807853,	0.9238795,	0.5555702	
};
//Косиинусные коэффициенты для измерительного органа Фурье для 3-й гармоники для 32 выборок за период
static const float fft32_3_cos_coef[32]=
{
1.0000000,	0.8314699,	0.3826839,	-0.1950899,	-0.7071065,	-0.9807852,	-0.9238797,	-0.5555706,	
-0.0000004,	0.5555699,	0.9238794,	0.9807853,	0.7071070,	0.1950906,	-0.3826832,	-0.8314695,	
-1.0000000,	-0.8314697,	-0.3826836,	0.1950901,	0.7071066,	0.9807852,	0.9238796,	0.5555704,	
0.0000001,	-0.5555701,	-0.9238795,	-0.9807853,	-0.7071068,	-0.1950904,	0.3826834,	0.8314696		
};

//Синусные коэффициенты для измерительного органа Фурье для 5-й гармоники для 32 выборок за период
static const float fft32_5_sin_coef[32]=
{
0.0000009,	-0.8314691,	-0.9238798,	-0.1950911,	0.7071062,	0.9807854,	0.3826841,	-0.5555697,	
-1.0000000,	-0.5555708,	0.3826829,	0.9807852,	0.7071072,	-0.1950898,	-0.9238793,	-0.8314699,	
-0.0000004,	0.8314694,	0.9238797,	0.1950907,	-0.7071065,	-0.9807853,	-0.3826837,	0.5555700,	
1.0000000,	0.5555704,	-0.3826833,	-0.9807853,	-0.7071069,	0.1950902,	0.9238795,	0.8314696	
};
//Косиинусные коэффициенты для измерительного органа Фурье для 5-й гармоники для 32 выборок за период
static const float fft32_5_cos_coef[32]=
{
1.0000000,	0.5555709,	-0.3826827,	-0.9807851,	-0.7071073,	0.1950896,	0.9238793,	0.8314700,	
0.0000007,	-0.8314693,	-0.9238798,	-0.1950909,	0.7071064,	0.9807854,	0.3826839,	-0.5555698,	
-1.0000000,	-0.5555706,	0.3826831,	0.9807852,	0.7071070,	-0.1950900,	-0.9238794,	-0.8314697,	
-0.0000002,	0.8314695,	0.9238796,	0.1950905,	-0.7071067,	-0.9807853,	-0.3826835,	0.5555702		
};

//Синусные коэффициенты для измерительного органа Фурье для 7-й гармоники для 32 выборок за период
static const float fft32_7_sin_coef[32]=
{
0.0000012,	-0.9807850,	-0.3826845,	0.8314690,	0.7071075,	-0.5555694,	-0.9238799,	0.1950894,	
1.0000000,	0.1950912,	-0.9238792,	-0.5555709,	0.7071062,	0.8314700,	-0.3826828,	-0.9807854,	
-0.0000006,	0.9807852,	0.3826839,	-0.8314693,	-0.7071071,	0.5555699,	0.9238797,	-0.1950900,	
-1.0000000,	-0.1950906,	0.9238794,	0.5555704,	-0.7071067,	-0.8314697,	0.3826834,	0.9807853	
};
//Косиинусные коэффициенты для измерительного органа Фурье для 7-й гармоники для 32 выборок за период
static const float fft32_7_cos_coef[32]=
{
1.0000000,	0.1950915,	-0.9238791,	-0.5555712,	0.7071060,	0.8314702,	-0.3826825,	-0.9807855,	
-0.0000009,	0.9807851,	0.3826842,	-0.8314692,	-0.7071073,	0.5555696,	0.9238798,	-0.1950897,	
-1.0000000,	-0.1950909,	0.9238793,	0.5555706,	-0.7071065,	-0.8314698,	0.3826831,	0.9807853,	
0.0000003,	-0.9807852,	-0.3826836,	0.8314695,	0.7071069,	-0.5555701,	-0.9238796,	0.1950903
};

//Синусные коэффициенты для измерительного органа Фурье для 9-й гармоники для 32 выборок за период
static const float fft32_9_sin_coef[32]=
{
0.0000016,	-0.9807856,	0.3826821,	0.8314704,	-0.7071058,	-0.5555713,	0.9238790,	0.1950915,	
-1.0000000,	0.1950892,	0.9238799,	-0.5555694,	-0.7071075,	0.8314691,	0.3826843,	-0.9807851,	
-0.0000008,	0.9807854,	-0.3826828,	-0.8314700,	0.7071064,	0.5555707,	-0.9238793,	-0.1950908,	
1.0000000,	-0.1950900,	-0.9238796,	0.5555700,	0.7071069,	-0.8314695,	-0.3826835,	0.9807853	
};
//Косиинусные коэффициенты для измерительного органа Фурье для 9-й гармоники для 32 выборок за период
static const float fft32_9_cos_coef[32]=
{
1.0000000,	-0.1950888,	-0.9238801,	0.5555690,	0.7071078,	-0.8314689,	-0.3826846,	0.9807850,	
0.0000012,	-0.9807855,	0.3826824,	0.8314702,	-0.7071061,	-0.5555710,	0.9238792,	0.1950911,	
-1.0000000,	0.1950896,	0.9238798,	-0.5555697,	-0.7071072,	0.8314693,	0.3826839,	-0.9807852,	
-0.0000004,	0.9807853,	-0.3826832,	-0.8314697,	0.7071066,	0.5555704,	-0.9238795,	-0.1950904
};
// endregion Foureier coefficients

//Инициализация структуры Vectors
//Вход:
//int count - количество исспорльзуемых векторов
Vectors VectorsInit(int count)
{
	Vectors vect;
	ComplexF arr[DEV_MaxNumberAnalogChannels];
	int index;
	vect.size = count;
	for (index = 0; index < count; index++)
	{
		complex_Init(&arr[index]);
	}
	measNsignalsRunCounter = 0;
	initSignalBufers();
	return vect;
}


// region interpolation

//Функция линейной интерполяции сигнала
//Расчет времени ведется от конца массива
//Вход:
//int *buffer - указатель на массив выборок для инерполяции;
//int bufCoun - длинна буфера
//float dt - время между отсчетами (шаг по времени постоянный)
//float t - время, от конца массива, искомой точки
//Выход:
//координата точки по оси ординат
//errCode = 0x3304, 0x3302
float lineInterpolation(int *buffer, int bufCount, float dt, float t)
{
	int x1;
	if (dt<=0)
	{
		setErr_code_str(0x3304, "lineInterpolation");
		return 0;
	}
	x1 = (int)(t/dt);//номер выборки с конца ближайшей к данной
	if (x1 == bufCount-1)
		return (float)buffer[x1];

	if (x1+1 > bufCount)
	{
		setErr_code_str(0x3302, "lineInterpolation");
		return 0;
	}

	return (float)buffer[x1] + (buffer[x1+1] - buffer[x1])/dt * (t - x1 * dt);//y=y1+(y2-y1)/(x2-x1)*(x-x1)
}

//Функция линейной интерполяции сигнала при применении циклического буфера.
//Логика fifo реализуется в самой функции (не опртимизировано).
//Расчет времени ведется от конца массива.
//Вход:
//Fifo_int * fifo - циклический буфер с исходными данными;
//float dt - время между отсчетами (шаг по времени постоянный)
//float t - время, от конца массива, искомой точки
//Выход:
//координата точки по оси ординат
//errCode = 0x3304, 0x3302, 0x3303
float lineInterpolation_fifo(Fifo_int * fifo, float dt, float t)
{
	int x1;
	if (dt<=0)
	{
		setErr_code_str(0x3304, "lineInterpolation_fifo");
		return 0;
	}
	x1 = (int)(t/dt);//номер выборки с конца ближайшей к данной
	if (x1 == fifo->size-1)
		return (float)fifo_int_pull(fifo);

	if (x1+1 > fifo->size-1)
	{
		setErr_code_str(0x3302, "lineInterpolation_fifo");
		return 0;
	}

	return (float) fifo_int_oldnumber(fifo, fifo->size-x1) + (fifo_int_oldnumber(fifo, fifo->size-1-x1) - fifo_int_oldnumber(fifo, fifo->size-x1))/dt * (t - x1 * dt);//y=y1+(y2-y1)/(x2-x1)*(x-x1)
}

//Функция линейной интерполяции сигнала при применении циклического буфера.
//Логика fifo реализуется в самой функции (не опртимизировано).
//Расчет времени ведется от конца массива.
//Вход:
//int *buffer - указатель на массив выборок для инерполяции;
//int bufCoun - длинна буфера
//int bufHead - положение последней выборки в циклическом буффере
//float dt - время между отсчетами (шаг по времени постоянный)
//float t - время, от конца массива, искомой точки
//Выход:
//координата точки по оси ординат
//errCode = 0x3302, 0x3304
float lineInterpolation_fifo1(int *buffer, int bufCount, int bufHead, float dt, float t)
{
	int x1;
	int x1head, x2head;
	if (bufHead > bufCount)
	{
		setErr_code_str(0x3302, "lineInterpolation_fifo1");
		return 0;
	}
	if (dt<=0)
	{
		setErr_code_str(0x3304, "lineInterpolation_fifo1");
		return 0;
	}
	x1 = (int)(t/dt);//номер выборки с конца ближайшей к данной
	if (x1 == bufHead+1)
		return (float)buffer[bufHead];

	//вычисляем положение нужных выборок в циклическом буфере
	if (x1+1 > bufHead+1)
	{
		setErr_code_str(0x3304, "lineInterpolation_fifo1");
		return 0;
	}
	x1head = bufHead - (bufCount - x1);
	if (x1head < 0)
		x1head+=bufCount;
	if (x1head != bufCount-1)
		x2head = x1head+1;
	else
		x2head = 0;

	return (float)buffer[x1head] + (buffer[x2head] - buffer[x1head])/dt * (t - x1 * dt);//y=y1+(y2-y1)/(x2-x1)*(x-x1)
}

// endregion interpolation

// region resampling
//Функция ресенмплинга. Использует линейную интерполяцию. Работет с входными значениями int. На выходе float[].
//Вход:
//int * buffer - указатель на буфер вхоных данных (выборок) (циклический буфер)
//int bufcount - длинна буфера
//int bufhead - текущее положение конца циклического буфера. Если использовать как простой буфер - то bufhead = bufcount.
//float dt - шаг по времени данных исходного буфера
//float resTime - время выходного буфера
//int resSampleCount - длинна выходного буфера в выборках
//float * res - указатель на выходной буфер длинной resSampleCount
//errCode = 0x3305, 0x3302, 0x3304
void resampling(int * buffer, int bufcount, int bufhead, float dt, float resTime, int resSampleCount, float * res)
{
	int index;
	float resTimeStep, baseTime;

	if (resTime > bufcount * dt)
	{
		setErr_code_str(0x3305, "resampling");
			return;
	}
	resTimeStep = resTime/resSampleCount;//шаг по времени результирующего массива
	baseTime = bufcount*dt;//суммарное время базового массива

	res[resSampleCount -1] = (float)buffer[bufhead-1];
	for(index=1; index < resSampleCount; index ++)
	{
		res[resSampleCount - 1 - index] = lineInterpolation_fifo1(buffer, bufcount, bufhead, dt, baseTime - index*resTimeStep);
	}
	return;
}

//Функция ресенмплинга. Использует линейную интерполяцию. Работет с входными значениями int. Применяется при использовании реализованного алгоритма fifo.
//На выходе float[].
//Вход:
//_Fifo_int * buffer - указатель на буфер вхоных данных (выборок) (циклический буфер)
//float base_dt - шаг по времени исходного буфера
//float res_dt - шаг по времени выходного буфера
//int resSampleCount - длинна выходного буфера в выборках
//float * res - указатель на выходной буфер длинной resSampleCount
//errCode = 0x3305,0x3304, 0x3302, 0x3303
void resampling_fifo(Fifo_int * fifo, float base_dt, float res_dt, int resSampleCount, float * res)
{
	int index;
	float baseTime;

	if (res_dt * resSampleCount > fifo->size * base_dt)
	{
		setErr_code_str(0x3305, "resampling_fifo");
		return;
	}
	baseTime = fifo->size*base_dt;//суммарное время базового массива

	res[resSampleCount -1] = (float)fifo_int_pull(fifo);
	for(index=1; index < resSampleCount; index ++)
	{
		res[resSampleCount - 1 - index] = lineInterpolation_fifo(fifo, base_dt, baseTime - index*res_dt);
	}
	return;
}

// endregion resampling

// region Fourier

//Расчет комплексной амплитуды сигнала первой гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//Для получения точного результата массив должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourier1st32s(float * buf)
{
	int index;
	ComplexF res;
	complex_Init(&res);
	for (index = 0; index < 32; index ++)//TODO: переделать под CMSIS
	{
		res.Real += buf[index] * fft32cos_coef[index];
		res.Imag += buf[index] * fft32sin_coef[index];
	}
	res.Real /= (float)16.0;
	res.Imag /= (float)16.0;
	return res;
}

//Расчет комплексной амплитуды сигнала X гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Для получения точного результата массив buf должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//float * sinCoef - коэффициенты синусной составляющей ряда фурье для данной гармоники, 32 коэффициента. 
//float * cosCoef - коэффициенты косинусной составляющей ряда фурье для данной гармоники, 32 коэффициента.
//
//cosCoef[i] + j*sinCoef[i] = exp(-j*f*n*2*pi/N *(N-i-1), где N - кличество выборок за период,
//n - номер гармоники, f - частота первой гармоники, i - номер коэффициента.
//
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourierX32s(float * buf, const float * sinCoef, const float *cosCoef)
{
	int index;
	ComplexF res;
	complex_Init(&res);
	for (index = 0; index < 32; index ++)//TODO: переделать под CMSIS
	{
		res.Real += buf[index] * cosCoef[index];
		res.Imag += buf[index] * sinCoef[index];
	}
	res.Real /=(float)16.0;
	res.Imag /=(float)16.0;
	return res;
}

//Расчет комплексной амплитуды сигнала 2-й гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//Для получения точного результата массив должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourier2nd32s(float * buf)
{
	return fourierX32s(buf, fft32_2_sin_coef, fft32_2_cos_coef);
}

//Расчет комплексной амплитуды сигнала 3-й гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//Для получения точного результата массив должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourier3rd32s(float * buf)
{
	return fourierX32s(buf, fft32_3_sin_coef, fft32_3_cos_coef);
}

//Расчет комплексной амплитуды сигнала 5-й гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//Для получения точного результата массив должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourier5th32s(float * buf)
{
	return fourierX32s(buf, fft32_5_sin_coef, fft32_5_cos_coef);
}

//Расчет комплексной амплитуды сигнала 7-й гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//Для получения точного результата массив должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourier7th32s(float * buf)
{
	return fourierX32s(buf, fft32_7_sin_coef, fft32_7_cos_coef);
}

//Расчет комплексной амплитуды сигнала 9-й гармоники по 32 точкам методом Фурье.
//На выходе амплитудное значение.
//Вход:
//float * buf - указатель на массив выборок сигнала для вычисления амплитуды.
//Для получения точного результата массив должен содержать данные ровно на 1 париод основной частоты.
//Для этого предварительно определяется частота синала и делается ресемплинг.
//Выход:
//ComplexF res - комплексная амплитуда перевой гармоники сигнала.
ComplexF fourier9th32s(float * buf)
{
	return fourierX32s(buf, fft32_9_sin_coef, fft32_9_cos_coef);
}

//Адаптивный к частоте орган определения вектора первой гармоники на основе алгоритма Фурье.
//Определяет вкличину первой гармоники рассчетной частоты.
//Вход:
//Fifo_int * signal - циклически буфер выборок сигнала.
//float period - период основной частоты в выборках (20 мс (50 гц) = DEV_PeriodSamples выборок за период).
//Выход:
//ComplexF res - вектор первой гармоники рассчетной частоты.
//errCode = 0x3305,0x3304, 0x3302, 0x3303
ComplexF fourier1st32sFreq(Fifo_int * signal, float period)
{
	ComplexF res;
	float signalResamp[32];
	//Здесь задем время в выборках для простоты расчетов
	resampling_fifo(signal, 1, period/32.0f, 32, signalResamp);
	if (getErr() != 0)
	{
		setErr_str("fourier1st32sFreq");
		complex_Init(&res);
	}
	else
		res = fourier1st32s(signalResamp);
	return res;
}

// endregion Fourier

//Симметричные составляющие
// region Symmetrical Components

//a = e^(j2pi/3)
static ComplexF coefSCa = {-0.5f, 0.8660254f};
//a^2
static ComplexF coefSCa2 = {-0.5f, -0.8660254f};

//Вычисление вектора прямой последовательности по векторам трех фаз
//X1 = Xa + Xb * a + Xc * a^2
//Вход:
//ComplexF *phA - вектор фазы A
//ComplexF *phB - вектор фазы B
//ComplexF *phC - вектор фазы C
//Выход:
//ComplexF res - вектор прямой последовательности
ComplexF calcPhase2Sym1(ComplexF *phA, ComplexF *phB, ComplexF *phC)
{
	ComplexF res, Btmp, Ctmp;
	Btmp = complex_mul(phB, &coefSCa);
	Ctmp = complex_mul(phC, &coefSCa2);
	res = complex_add3(phA, &Btmp, &Ctmp);
	res = complex_div_float(&res, 3.0f);
	return res;
}

//Вычисление вектора обратной последовательности по векторам трех фаз
//X1 = Xa + Xb * a^2 + Xc * a
//Вход:
//ComplexF *phA - вектор фазы A
//ComplexF *phB - вектор фазы B
//ComplexF *phC - вектор фазы C
//Выход:
//ComplexF res - вектор обратной последовательности
ComplexF calcPhase2Sym2(ComplexF *phA, ComplexF *phB, ComplexF *phC)
{
	ComplexF res, Btmp, Ctmp;
	Btmp = complex_mul(phB, &coefSCa2);
	Ctmp = complex_mul(phC, &coefSCa);
	res = complex_add3(phA, &Btmp, &Ctmp);
	res = complex_div_float(&res, 3.0f);
	return res;
}

//Вычисление вектора нулевой последовательности по векторам трех фаз
//X1 = Xa + Xb * a^2 + Xc * a
//Вход:
//ComplexF *phA - вектор фазы A
//ComplexF *phB - вектор фазы B
//ComplexF *phC - вектор фазы C
//Выход:
//ComplexF res - вектор нулевой последовательности
ComplexF calcPhase2Sym0(ComplexF *phA, ComplexF *phB, ComplexF *phC)
{
	ComplexF res = complex_add3(phA, phB, phC);
	res = complex_div_float(&res, 3.0f);
	return res;
}

//Вычисление вектора фазы A по симметричным составляющим фазы A
//XB = X1 + X2 + X0
//Вход:
//ComplexF *x1 - вектор прямой последовательности
//ComplexF *x2 - вектор обратной последовательности
//ComplexF *x0 - вектор нулевой последовательности
//Выход:
//ComplexF res - вектор фазы A
ComplexF calcSym2PhaseA(ComplexF *x1, ComplexF *x2, ComplexF *x0)
{
	ComplexF res;
	res = complex_add3(x1, x2, x0);
	return res;
}

//Вычисление вектора фазы B по симметричным составляющим фазы A
//XB = X1 * a^2 + X2 * a + X0
//Вход:
//ComplexF *x1 - вектор прямой последовательности
//ComplexF *x2 - вектор обратной последовательности
//ComplexF *x0 - вектор нулевой последовательности
//Выход:
//ComplexF res - вектор фазы B
ComplexF calcSym2PhaseB(ComplexF *x1, ComplexF *x2, ComplexF *x0)
{
	ComplexF res, x1tmp, x2tmp;
	x1tmp = complex_mul(x1, &coefSCa2);
	x2tmp = complex_mul(x2, &coefSCa);
	res = complex_add3(&x1tmp, &x2tmp, x0);
	return res;
}

//Вычисление вектора фазы C по симметричным составляющим фазы A
//XB = X1 * a^2 + X2 * a + X0
//Вход:
//ComplexF *x1 - вектор прямой последовательности
//ComplexF *x2 - вектор обратной последовательности
//ComplexF *x0 - вектор нулевой последовательности
//Выход:
//ComplexF res - вектор фазы C
ComplexF calcSym2PhaseC(ComplexF *x1, ComplexF *x2, ComplexF *x0)
{
	ComplexF res, x1tmp, x2tmp;
	x1tmp = complex_mul(x1, &coefSCa);
	x2tmp = complex_mul(x2, &coefSCa2);
	res = complex_add3(&x1tmp, &x2tmp, x0);
	return res;
}

//Вычисление утроенного значения отсчета нулевой последовательности по отсчетм 3-х фаз.
//3xo = xa + xb + xc
//Вход:
//int xa - отсчет фазы A
//int xb - отсчет фазы B
//int xc - отсчет фазы C
//Выход
//int res - отсчет утроенного значения сигнала нулевой последовательности
int calc3XoSample(int xa, int xb, int xc)
{
	return xa+xb+xc;
}

//Вычисление вектора фазы С по векторам фазы A, B и нулевого тока (1*Io).
//Вход:
//ComplexF *ia - вектор тока фазы A
//ComplexF *ib - вектор тока фазы B
//ComplexF *io - вектор тока нулевой последовательноти.
//Выход:
//ComplexF res - Вектор фазы C.
ComplexF calcIabo2Ic(ComplexF *ia, ComplexF *ib, ComplexF *io)
{
	ComplexF tmp;
	tmp = complex_sub(io, ia);
	return complex_sub(&tmp, ib);
}

// endregion Symmetrical Components

//Реализции методов измернения параметров сигнала (действющего значения и фазы) отличные от Фурье.
// region Other measurmrnts method

//Расчет амплитуды сигнала "амплитудным" методом (peak-to-peak) по 32 выборкам.
//Возвращает среднне арифметическое между максимумами положительной и отрицательной полуволн.
//Данная реализация не чувствительна к постоянной составляющей в сигнале.
//Вход:
//float * buf - буффер входных выборок.
//Выход: 
//float res - амплитуда сигнала.
float peakToPeak(float * buf)
{
	int index;
	float max = 0;
	float min = 0;
	float res = 0;
	for(index = 0; index < 32; index ++)//TODO: использовать CMSIS Min/Max
	{
		if (buf[index] > max)
			max = buf[index];
		if (buf[index] < min)
			min = buf[index];
	}
	if ((max != 0 || min != 0) && max != min )
	{
		res = (max - min)/2.0f;
	}
	return res;
}

//Расчет амплитуды сигнала среднеквадратичным методом (RMS) по 32 выборкам.
//Вход:
//float * buf - буффер входных выборок.
//Выход: 
//float res - амплитуда сигнала.
float RMS32(float *buf)
{
	float res=0;
	int index;
	for (index = 0; index<32; index++)//TODO: использовать CMSIS arm_rms_f32
	{
		res+=buf[index]*buf[index];
	}
	res/=16.0f;
	res = sqrtf(res);
	return res;
}

// endregion Other measurmrnts methods

//Измерение мощности и направления мошности
// region Power mesurments

//Рачет комплексной мошности по току и напряжению.
//Вход:
//ComplexF *current - комплексный вектор тока
//ComplexF *voltage - комплексный вектор напряжения
//Выход:
//ComplexF res - комплексный вектор мощности
ComplexF calcPower(ComplexF *current, ComplexF *voltage)
{
	return complex_mul(current, voltage);
}
// endregion Power mesurments

//полный цикл измерений
// region general measurments

//Инициализация буфферов выборок каналов
void initSignalBufers()
{
	int index, index1;
	//Инициализируем циклические буферы
	for (index =0; index <DEV_numberAnalogChannels; index++)
	{
		signalsFifo[index].buf = signalsBufs[index];
		signalsFifo[index].head =0;
		signalsFifo[index].size = BUFSIZE_HI;
	}
	//Инициализируем массивы циклических буферов нулями
	for (index1 = 0; index1 < BUFSIZE_HI; index1++)
	{
		for (index =0; index <DEV_numberAnalogChannels; index++)
		{
			signalsFifo[index].buf[index1]=0;
		}
	}
}

//Процедура расчета комплексной амплитуды сигналов для текущего среза (момента времени) и частоты сети по первому каналу.
//Расчет векторов ведется с использованием информации о частоте.
//Использутся статически обьявленые буферы.
//Вход:
//int *signals - указатель на массив выборок каналов.
//FreqF *fr - указатель на структуру исп. для расчета частоты.
//Vectors *res - Указатель на структуру с искомыми векторами - выходной параметр (необходимо проинициализировать Vectors.size заранее).
//
//errCode 0x3305, 0x3302, 0x3303, 0x3304
void measNsignals(short *signals, FreqF *fr, Vectors *res)
{
	int index;

	if(res->size < DEV_numberAnalogChannels)
	{
		setErr_code_str(0x3305, "measNsignals");
		return;
	}
	
	//Для первого канала запускаем расчет частоты
	periodCalcF_flt(signals[0], fr);

	for (index=0; index<DEV_numberAnalogChannels; index++)
	{
		fifo_int_push(&signalsFifo[index], signals[index]);
	}
	if (measNsignalsRunCounter >= DEV_LogicWorkTime)//Если набралось нужное количество новых выборок - зппускаем расчет действующих значений
	{
//		for (index=0; index<DEV_numberAnalogChannels; index++)
//		{
//			res->vect[index] = fourier1st32sFreq(&signalsFifo[index], fr->T);
//		}
		measNsignalsRunCounter=0;
	}
	measNsignalsRunCounter++;
}
// endregion general measurments
