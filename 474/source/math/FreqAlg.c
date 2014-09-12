// FreqAlg.cpp: определяет точку входа для консольного приложения.
//
#include "FreqAlg.h"

static float dTdtbuf[BUFSIZE_LO];
static float freqBuf[BUFSIZE_LO];
static float periodBuf[BUFSIZE_LO];
static float sampleBufer[BUFSIZE_HI];
static float timeBuf[BUFSIZE_LO];
static int filtSamplesBuffer[BUFSIZE_HI];

#define BLOCKSIZE 1
#define NUMTAPS DEV_PeriodSamples
#define BLOCKCOUNT 1

static arm_fir_instance_f32 S;

static float32_t firState[BLOCKSIZE + DEV_PeriodSamples - 1];
static float32_t coefs[NUMTAPS];

static float32_t output[BLOCKSIZE*BLOCKCOUNT];

static float32_t input[(BLOCKSIZE+NUMTAPS-1)*BLOCKCOUNT];

//Инциализирует структуру для расчета частоты
//errCode 0x3301
FreqF_intFlt FreqFIntInit()
{
	FreqF_intFlt fr;

	fr.dTdtbuf.buf = dTdtbuf;
	fr.dTdtbuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.dTdtbuf);

	fr.freqBuf.buf = freqBuf;
	fr.freqBuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.freqBuf);

	fr.periodBuf.buf = periodBuf;
	fr.periodBuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.periodBuf);

	fr.sampleBufer.buf = sampleBufer;
	fr.sampleBufer.size = BUFSIZE_HI;
	fifo_initialize_float(&fr.sampleBufer);

	fr.timeBuf.buf = timeBuf;
	fr.timeBuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.timeBuf);

	fr.filter.samples.buf = filtSamplesBuffer;
	fr.filter.samples.size = BUFSIZE_HI;
	fifo_initialize_int(&fr.filter.samples);

	fr.dfdt=0;
	fr.frequency=DEV_WorkFreq;
	fr.campleCounter=0;
	fr.dTdt=0;
	fr.T=(DEV_SampleFreq)/(DEV_WorkFreq);
	fr.sampleFreq = DEV_SampleFreq;
	fr.nomFreq = DEV_WorkFreq;

	FiltIntInit(&fr.filter);

	fr.isCalc = false;

	if (getErr() != 0)
		setErr_str("FreqFInit");
	return fr;
}

//Инциализирует структуру для расчета частоты
//errCode 0x3301
FreqF FreqFInit()
{
	FreqF fr;

	fr.dTdtbuf.buf = dTdtbuf;
	fr.dTdtbuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.dTdtbuf);

	fr.freqBuf.buf = freqBuf;
	fr.freqBuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.freqBuf);

	fr.periodBuf.buf = periodBuf;
	fr.periodBuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.periodBuf);

	fr.sampleBufer.buf = sampleBufer;
	fr.sampleBufer.size = BUFSIZE_HI;
	fifo_initialize_float(&fr.sampleBufer);

	fr.timeBuf.buf = timeBuf;
	fr.timeBuf.size = BUFSIZE_LO;
	fifo_initialize_float(&fr.timeBuf);

	fr.filter.samples.buf = filtSamplesBuffer;
	fr.filter.samples.size = BUFSIZE_HI;
	fifo_initialize_int(&fr.filter.samples);

	fr.dfdt=0;
	fr.frequency=DEV_WorkFreq;
	fr.campleCounter=0;
	fr.dTdt=0;
	fr.T=(DEV_SampleFreq)/(DEV_WorkFreq);
	fr.sampleFreq = DEV_SampleFreq;
	fr.nomFreq = DEV_WorkFreq;


	FiltInitDSP(&fr.filter);
	//FiltInit(&fr.filter);

	fr.isCalc = false;

	if (getErr() != 0)
		setErr_str("FreqFInit");
	return fr;
}

//Рассчитывает период сигнала по переходам через ноль.
//Расчет периода ведется по среднему расстоянию между переходами через ноль из "-" в "+".
//Для расчета используется среднее время за 3 предыдущих периода. T = (t0-t3)/3.
//Функция вычисляет период сигнала в выборках, с дискретностью до 1/(2^16) доли выборки.
//Вход:
//int sample - значение новой выборки;
//Freq *fr - структура для хранения информации о частоте;
//Выход:
//int res - занчение периода основной гармоники сигнала в выбоках: (Tmc * 2^16)/dt, где dt - период дискретизации сигнала.
//
//Усреднение по трем периодам дает запаздывание результата измерения на 1.5 периода текущей частоты. Например, при наличии df/dt != 0 результатом
//будет частота не в данный момент времени (переход чрез 0), а в момент времени отстающий от текущего на 1.5 периода назад. 
//При f = 50 Гц и df/dt = 5 Гц/с ~ 0.3/0.06, замер частоты будет запаздывать на 0.3/2 = 0.15 Гц или , где 0.3 - изменение частоты за 3 периода.
//int periodCalc(int sample, _Freq *fr) 
//{
//	int period=0;
//	int dt=0;
//	unsigned __int16 passDt;
//	fr->isCalc = 0;
//	signalShift(fr->sampleBufer, sample);
//	if (fr->sampleBufer[18] < 0 && sample >=0 )
//	{
//		 passDt= nulpass(sample, fr->sampleBufer[18]);//вычисляем доли выборок при переходе черз 0 и кладем в буфер
//		
//		signalShiftUINT(fr->timeBuf, fr->campleCounter + passDt);//кладем в буфер время до текущего перехода через ноль с точностью до 1/(2^16) доли выборки
//
//		if(fr->timeBuf[19] > fr->timeBuf[16])
//		{
//			dt = fr->timeBuf[19] - fr->timeBuf[16];
//		}
//		else//если буфер выборок переполнился между этими двумя переходами
//		{
//			dt= 0xFFFFFFFF - fr->timeBuf[16] + fr->timeBuf[19];
//		}
//		period = dt / 3;
//	
//		fr->T = period;
//		signalShiftUINT( fr->periodBuf, period);
//		calcFreq(fr);
//		calcDTDt(fr);
//		calcDfDt(fr);
//		//int tmp = fr->frequency;
//		fr->isCalc = 1;
//	}
//	fr->campleCounter+=0xFFFF;//счетчик инкрементируется на 2^16. байт Младшие 16 бит числа используется для хранения дробной части выборки.
//
//	return fr->T;
//}


//Рассчитывает период и частоту сигнала по переходам через ноль. Используется FLOAT. С применением фильтра.
//Расчет периода ведется по среднему расстоянию между переходами через ноль из "-" в "+".
//Для расчета используется среднее время за 3 предыдущих периода. T = (t0-t3)/3.
//Функция вычисляет период сигнала в выборках.
//Вход:
//int sample - значение новой выборки;
//FreqF *fr - структура для хранения информации о частоте;
//Выход:
//float res - занчение частоы основной гармоники сигнала в Гц.
//
//Усреднение по трем периодам дает запаздывание результата измерения на 1.5 периода текущей частоты + запаздывание фильтра на 0.5. Например, при наличии df/dt != 0 результатом
//будет частота не в данный момент времени (переход чрез 0), а в момент времени отстающий от текущего на 2 периода назад. 
//При f = 50 Гц и df/dt = 5 Гц/с ~ 0.3/0.06, замер частоты будет запаздывать на 0.3/2 + 0.05 = 0.2 Гц, где 0.3 - изменение частоты за 3 периода, 0.06 - примерная длительность 3 периодов.
//errCode 0x3302, 0x3303, 0x3101
float periodCalcF_flt(int sample, FreqF *fr) 
{
	float time, time3;
	float sampleF=0;
	float period=0;
	float passDt=0;
	float dt=0.0;
	float frTmp=0;

	static int freqBlockCounter;

	float f1=0, f2=0, df=0, t1, dt_;


	fr->isCalc = false;
	//filter(sample, &fr->filter);
	filterDSP(sample, &fr->filter);
	if (getErr() != 0)
	{
		setErr_str("periodCalcF_flt");
		return 0.0f;
	}
	sampleF = fr->filter.result;

	fifo_float_push(&fr->sampleBufer, sampleF);

	if (fifo_float_oldnumber(&fr->sampleBufer, 1) < 0 && sampleF >=0 )
	{
		passDt = nulpassF(sampleF, fifo_float_oldnumber(&fr->sampleBufer, 1));//вычисляем доли выборок при переходе черз 0 и кладем в буфер

		if (getErr()!=0)
		{
			setErr_str("periodCalcF_flt");
			return 0.0f;
		}
		//signalShiftF(fr->timeBuf, fr->campleCounter + passDt);//кладем в буфер время до текущего перехода через ноль
		fifo_float_push(&fr->timeBuf, fr->campleCounter + passDt);

		time = fifo_float_pull(&fr->timeBuf);
		time3 = fifo_float_oldnumber(&fr->timeBuf, 3);
		if(time > time3)
		{
			dt = time - time3;
		}
		else//если буфер выборок переполнился между этими двумя переходами
		{
			dt= (float)0xFFFF - time3 + time;
		}

		if ((float)0.0f == dt)
		{
			setErr_code_str(0x3101, "periodCalcF_flt");
			return 0.0f;
		}

		period = dt / 3.0f;
		//signalShiftF( fr->periodBuf, period);
		fifo_float_push(&fr->periodBuf, period);

		frTmp = 3.0f * DEV_SampleFreq / dt;

		//signalShiftF( fr->freqBuf, fr->frequency);
		fifo_float_push(&fr->freqBuf, frTmp);
	
		fr->isCalc = true;//поднимаем флаг появления нового значения частоты

		//Оцениваем текущее значение df/dt
		f1= fifo_float_pull(&fr->freqBuf);
		f2 = fifo_float_oldnumber(&fr->freqBuf,1);
		df = (f1 - f2 );
		
		t1= fifo_float_pull(&fr->periodBuf);
		//t2 = fifo_float_oldnumber(&fr->periodBuf,1);
		dt_ = t1/DEV_SampleFreq;

		//если df/dt > 15 Гц/с - велика вероятность, что это переходной процесс (скачек фазы)
		#ifndef MSVC
			if (fabs(df/dt_) > 15)
		#else
			if (abs(df/dt_) > 15)
		#endif
		{
			freqBlockCounter=1;
		}
		
		if (freqBlockCounter>0)
		{
			if (freqBlockCounter<4)//При переходном процессе пропускаем 4 периода с моемента его начала. 3 периода расчет частоты + 1 период фильтр
				freqBlockCounter++;
			else
				freqBlockCounter=0;
		}
		
		if (0 == freqBlockCounter)
		{
			if (time3 == 0)//первые 3 периода частоту не считаем.
			{
				fr->T = DEV_PeriodSamples;
				fr->frequency = fr->nomFreq;
				freqBlockCounter = 0;
			}
			else
			{
				fr->frequency = frTmp;
				fr->T = period;
				calcDfDtF(fr);
			}
		}
	}

	fr->campleCounter++;
	if (fr->campleCounter == 0)
		fr->campleCounter = 1;

	if (getErr() !=0)
	{
		setErr_str("periodCalcF_flt");
	}

	if (fr->T == 0)
	{
		fr->T = DEV_PeriodSamples;
		fr->frequency = fr->nomFreq;
	}

	return fr->T;
}

//Рассчитывает период и частоту сигнала по переходам через ноль. Используется FLOAT. С применением фильтра.
//Расчет периода ведется по среднему расстоянию между переходами через ноль из "-" в "+".
//Для расчета используется среднее время за 3 предыдущих периода. T = (t0-t3)/3.
//Функция вычисляет период сигнала в выборках.
//Вход:
//int sample - значение новой выборки;
//FreqF_intFlt *fr - структура для хранения информации о частоте;
//Выход:
//float res - занчение частоы основной гармоники сигнала в Гц.
//
//Усреднение по трем периодам дает запаздывание результата измерения на 1.5 периода текущей частоты + запаздывание фильтра на 0.5. Например, при наличии df/dt != 0 результатом
//будет частота не в данный момент времени (переход чрез 0), а в момент времени отстающий от текущего на 2 периода назад. 
//При f = 50 Гц и df/dt = 5 Гц/с ~ 0.3/0.06, замер частоты будет запаздывать на 0.3/2 + 0.05 = 0.2 Гц, где 0.3 - изменение частоты за 3 периода, 0.06 - примерная длительность 3 периодов.
//errCode 0x3302, 0x3303, 0x3101
float periodCalcF_fltInt(int sample, FreqF_intFlt *fr)
{
	float time, time3;
	volatile int64_t sampleFlt=0;
	float period=0;
	float passDt=0;
	float dt=0.0;
	float frTmp=0;

	static int freqBlockCounter;

	float f1=0, f2=0, df=0, t1, dt_;


	fr->isCalc = false;
	filterInt(sample, &fr->filter);
	if (getErr() != 0)
	{
		setErr_str("periodCalcF_flt");
		return 0.0f;
	}
	sampleFlt = fr->filter.result;

	fifo_float_push(&fr->sampleBufer, (float)sampleFlt);

	if (fifo_float_oldnumber(&fr->sampleBufer, 1) < 0 && (float)sampleFlt >=0 )
	{
		passDt = nulpassF((float)sample, fifo_float_oldnumber(&fr->sampleBufer, 1));//вычисляем доли выборок при переходе черз 0 и кладем в буфер

		if (getErr()!=0)
		{
			setErr_str("periodCalcF_flt");
			return 0.0f;
		}
		//signalShiftF(fr->timeBuf, fr->campleCounter + passDt);//кладем в буфер время до текущего перехода через ноль
		fifo_float_push(&fr->timeBuf, fr->campleCounter + passDt);

		time = fifo_float_pull(&fr->timeBuf);
		time3 = fifo_float_oldnumber(&fr->timeBuf, 3);
		if(time > time3)
		{
			dt = time - time3;
		}
		else//если буфер выборок переполнился между этими двумя переходами
		{
			dt= (float)0xFFFF - time3 + time;
		}

		if ((float)0.0f == dt)
		{
			setErr_code_str(0x3101, "periodCalcF_flt");
			return 0.0f;
		}

		period = dt / 3.0f;
		//signalShiftF( fr->periodBuf, period);
		fifo_float_push(&fr->periodBuf, period);

		frTmp = 3.0f * DEV_SampleFreq / dt;

		//signalShiftF( fr->freqBuf, fr->frequency);
		fifo_float_push(&fr->freqBuf, frTmp);
	
		fr->isCalc = true;//поднимаем флаг появления нового значения частоты

		//Оцениваем текущее значение df/dt
		f1= fifo_float_pull(&fr->freqBuf);
		f2 = fifo_float_oldnumber(&fr->freqBuf,1);
		df = (f1 - f2 );
		
		t1= fifo_float_pull(&fr->periodBuf);
		//t2 = fifo_float_oldnumber(&fr->periodBuf,1);
		dt_ = t1/DEV_SampleFreq;

		//если df/dt > 15 Гц/с - велика вероятность, что это переходной процесс (скачек фазы)
		#ifndef MSVC
			if (fabs(df/dt_) > 15)
		#else
			if (abs(df/dt_) > 15)
		#endif
		{
			freqBlockCounter=1;
		}
		
		if (freqBlockCounter>0)
		{
			if (freqBlockCounter<4)//При переходном процессе пропускаем 4 периода с моемента его начала. 3 периода расчет частоты + 1 период фильтр
				freqBlockCounter++;
			else
				freqBlockCounter=0;
		}
		
		if (0 == freqBlockCounter)
		{
			if (time3 == 0)//первые 3 периода частоту не считаем.
			{
				fr->T = DEV_PeriodSamples;
				fr->frequency = fr->nomFreq;
				freqBlockCounter = 0;
			}
			else
			{
				fr->frequency = frTmp;
				fr->T = period;
//				calcDfDtF(fr);
			}
		}
	}

	fr->campleCounter++;
	if (fr->campleCounter == 0)
		fr->campleCounter = 1;

	if (getErr() !=0)
	{
		setErr_str("periodCalcF_flt");
	}

	if (fr->T == 0)
	{
		fr->T = DEV_PeriodSamples;
		fr->frequency = fr->nomFreq;
	}

	return fr->T;
}

//Рассчитывает период сигнала по переходам через ноль. Используется FLOAT.
//Расчет периода ведется по среднему расстоянию между переходами через ноль из "-" в "+".
//Для расчета используется среднее время за 3 предыдущих периода. T = (t0-t3)/3.
//Функция вычисляет период сигнала в выборках.
//Вход:
//int sample - значение новой выборки;
//FreqF *fr - структура для хранения информации о частоте;
//Выход:
//int res - занчение периода основной гармоники сигнала в выбоках: Tmc/dt, где dt - период дискретизации сигнала.
//
//Усреднение по трем периодам дает запаздывание результата измерения на 1.5 периода текущей частоты. Например, при наличии df/dt != 0 результатом
//будет частота не в данный момент времени (переход чрез 0), а в момент времени отстающий от текущего на 1.5 периода назад. 
//При f = 50 Гц и df/dt = 5 Гц/с ~ 0.3/0.06, замер частоты будет запаздывать на 0.3/2 = 0.15 Гц или , где 0.3 - изменение частоты за 3 периода.
//float periodCalcF(int sample, _FreqF *fr) 
//{
//	float passDt=0;
//	float dt=0;
//	float period=0;
//
//	signalShiftF(fr->sampleBufer, sample);
//	fr->isCalc = 0;
//	if (fr->sampleBufer[18] < 0 && sample >=0 )
//	{
//		passDt = nulpassF(sample, fr->sampleBufer[18]);//вычисляем доли выборок при переходе черз 0 и кладем в буфер
//		
//		signalShiftF(fr->timeBuf, fr->campleCounter + passDt);//кладем в буфер время до текущего перехода через ноль
//
//		if(fr->timeBuf[19] > fr->timeBuf[16])
//		{
//			dt = fr->timeBuf[19] - fr->timeBuf[16];
//		}
//		else//если буфер выборок переполнился между этими двумя переходами
//		{
//			dt= (float)0xFFFF - fr->timeBuf[16] + fr->timeBuf[19] +1;
//		}
//		period = dt / 3.0;
//		fr->T = period;
//		signalShiftF( fr->periodBuf, period);
//		//calcFreq(fr);
//		//calcDTDt(fr);
//		//calcDfDt(fr);
//		//int tmp = fr->frequency;
//		fr->frequency = 9600.0 * 3 / dt;
//		signalShiftF( fr->freqBuf, fr->frequency);
//		fr->isCalc=1;
//	}
//	fr->campleCounter++;//счетчик инкрементируется на 2^16. байт Младшие 16 бит числа используется для хранения дробной части выборки.
//	return fr->T;
//}

//int samplescnt=0;
//Расчет частоты с фильтрацией в районе нуля по 6 выборкам
//float periodCalcF6(int sample, _FreqF *fr) 
//{
//	int prev = 0;
//	int cur = 0;
//	float passDt = 0;//вычисляем доли выборок при переходе черз 0 и кладем в буфер
//	float dt=0;
//	float period=0;
//
//	fr->isCalc = 0;
//	signalShiftF(fr->sampleBufer, sample);
//	if (fr->sampleBufer[18] < 0 && sample >=0 )
//	{
//		samplescnt=1;
//	}
//	else
//	{
//		if (samplescnt>=3)
//		{
//			prev = fr->sampleBufer[16] +  fr->sampleBufer[15] +  fr->sampleBufer[14];
//			cur = fr->sampleBufer[19] +  fr->sampleBufer[18] +  fr->sampleBufer[17];
//			passDt = nulpassF(cur, prev);//вычисляем доли выборок при переходе черз 0 и кладем в буфер
//			signalShiftF(fr->timeBuf, fr->campleCounter + passDt);//кладем в буфер время до текущего перехода через ноль
//
//			if(fr->timeBuf[19] > fr->timeBuf[16])
//			{
//				dt = fr->timeBuf[19] - fr->timeBuf[16];
//			}
//			else//если буфер выборок переполнился между этими двумя переходами
//			{
//				dt= (float)0xFFFF - fr->timeBuf[16] + fr->timeBuf[19];
//			}
//			period = dt / 3.0;
//			fr->T = period;
//			signalShiftF( fr->periodBuf, period);
//			//calcFreq(fr);
//			//calcDTDt(fr);
//			//calcDfDt(fr);
//			//int tmp = fr->frequency;
//			samplescnt=0;
//			fr->isCalc = 1;
//		}
//		if (samplescnt>0)
//			samplescnt++;
//	}
//	fr->campleCounter++;//инкремент счетчика выборок
//
//	return fr->T;
//}

//Вычисление частоты по периоду сигнала.
//Процедура используется после periodCalc.
//f = sampleFreq / T. sampleFreq - частота дискретизации сигнала [Гц], T - период в выборках * 2^16.
//Результат в Гц * 2^16.
//int calcFreq(_Freq *fr )
//{
//	if (fr->T > 0)
//	{
//		__int64 sampleFr = fr->sampleFreq;
//		fr->frequency = (sampleFr<<32) / fr->T;
//	}
//	signalShift(fr->freqBuf, fr->frequency);
//	return fr->frequency;
//}
//
//int calcDTDt(_Freq* fr)
//{
//	__int64 dT = fr->periodBuf[19] - fr->periodBuf[15] + fr->periodBuf[18] - fr->periodBuf[14];
//	int dtime;
//	if (0 == dT)
//	{
//		fr->dTdt = 0;
//	}
//	else
//	{
//		dtime = fr->timeBuf[19] - fr->timeBuf[15] + fr->timeBuf[18] - fr->timeBuf[14];
//		if (0 != dtime)
//		{
//			fr->dTdt = (dT << 24) / dtime;
//		}
//		else
//		{
//			fr->dTdt = 0;
//		}
//	}
//	signalShift(fr->dTdtbuf, fr->dTdt);
//	return fr->dTdt;
//}


//На выходе = (df * 2^16 * 2^16)/(dt * 2^16 / 9600). Для пересчета в Гц/c = dfdt * 9600.0/2^16.
//int calcDfDt(_Freq* fr)
//{
//	__int64 df = fr->freqBuf[19] - fr->freqBuf[15] + fr->freqBuf[18] - fr->freqBuf[14];
//	int dtime;
//	if (0 == df)
//	{
//		fr->dfdt = 0;
//	}
//	else
//	{
//		dtime = fr->timeBuf[19] - fr->timeBuf[15] + fr->timeBuf[18] - fr->timeBuf[14];
//		if (0 != dtime)
//		{
//			fr->dfdt = (df << 16) / dtime;
//		}
//		else
//		{
//			fr->dfdt = 0;
//		}
//	}
//	//signalShift(fr->dTdtbuf, fr->dTdt);
//	return fr->dfdt;
//}

//Расчет скорости изменения чатоты. Используется FLOAT.	
//df/dt расчитывается оп формуле df/dt = (f(n) - f(n-4) + f(n-1) - f(n-5)) / (t(n) - t(n-4) + t(n-1) - t(n-5)).
//На выходе = df/dt в Гц/c .
//errCode 0x3302, 0x3303
float calcDfDtF(FreqF* fr)
{
	float  dfdt, df, dtime=0;
	float f0 = 0,f1=0,f4=0,f5=0;
	float t0, t1, t4, t5;
	f0 = fifo_float_pull(&fr->freqBuf);
	f1 = fifo_float_oldnumber(&fr->freqBuf, 1);
	f4 = fifo_float_oldnumber(&fr->freqBuf, 4);
	f5 = fifo_float_oldnumber(&fr->freqBuf, 5);

	//df = fr->freqBuf[19] - fr->freqBuf[15] + fr->freqBuf[18] - fr->freqBuf[14];
	df = f0 - f4 + f1 - f5;// fifo_float_pull(&fr->freqBuf) - fifo_float_oldnumber(&fr->freqBuf, 4) + fifo_float_oldnumber(&fr->freqBuf, 1) - fifo_float_oldnumber(&fr->freqBuf, 5);

	if (getErr() != 0)
	{
		setErr_str("calcDfDtF");
		return 0.0;
	}

	if (0 == df)
	{
		fr->dfdt = 0;
	}
	else
	{
		//dtime = fr->timeBuf[19] - fr->timeBuf[15] + fr->timeBuf[18] - fr->timeBuf[14];

		t0 = fifo_float_pull(&fr->timeBuf);
		t1 = fifo_float_oldnumber(&fr->timeBuf, 1);
		t4 = fifo_float_oldnumber(&fr->timeBuf, 4);
		t5 = fifo_float_oldnumber(&fr->timeBuf, 5);

		//if(fifo_float_pull(&fr->timeBuf) > fifo_float_oldnumber(&fr->timeBuf, 4))
		//{
		//	dtime += fifo_float_pull(&fr->timeBuf) - fifo_float_oldnumber(&fr->timeBuf, 4);
		//}
		//else//если буфер выборок переполнился между этими двумя переходами
		//{
		//	dtime += (float)0xFFFF - fifo_float_oldnumber(&fr->timeBuf, 4) + fifo_float_pull(&fr->timeBuf);
		//}

		//if(fifo_float_oldnumber(&fr->timeBuf, 1) > fifo_float_oldnumber(&fr->timeBuf, 5))
		//{
		//	dtime += fifo_float_oldnumber(&fr->timeBuf, 1) - fifo_float_oldnumber(&fr->timeBuf, 5);
		//}
		//else//если буфер выборок переполнился между этими двумя переходами
		//{
		//	dtime += (float)0xFFFF - fifo_float_oldnumber(&fr->timeBuf, 5) + fifo_float_oldnumber(&fr->timeBuf, 1);
		//}

		if(t0 > t4)
		{
			dtime += t0 - t4;
		}
		else//если буфер выборок переполнился между этими двумя переходами
		{
			dtime += (float)0xFFFF - t4 + t0;
		}

		if(t1 > t5)
		{
			dtime += t1 - t5;
		}
		else//если буфер выборок переполнился между этими двумя переходами
		{
			dtime += (float)0xFFFF - t5 + t1;
		}

		if (getErr() != 0)
		{
			setErr_str("calcDfDtF");
			return 0.0;
		}

		if (0 != dtime)
		{
			dfdt = df *DEV_SampleFreq / dtime;
			#ifndef MSVC
				if (fabs(dfdt)<=15.0f)//если df/dt > 15 Гц/с - велика вероятность, что это периходной процесс (скачек фазы)
			#else
				if (abs(dfdt)<=15.0f)//если df/dt > 15 Гц/с - велика вероятность, что это периходной процесс (скачек фазы)
			#endif
				fr->dfdt = dfdt;
		}
		else
		{
			setErr_code_str(0x4101, "calcDfDtF");
			fr->dfdt = 0.0f;
		}
	}
	return fr->dfdt;
}

//Вычисляет дробную часть выборки момента перехода через ноль из "-" в "+".
//Исспользуется int. Возврящает дробную часть умноженную 256^2
//errCode 0x3101
//unsigned int nulpass(int sample, int prev)
//{
//	int dt, tmp = -prev + sample;
//	if (0 == tmp)
//	{
//		setErr_code_str(0x3101, "nulpass");
//		return 1;
//	}
//	dt = -(prev << 16) / tmp;
//	return dt;
//}

//Вычисляет дробную часть выборки момента перехода через ноль из "-" в "+".
//Исспользуется float
//errCode 0x3101
float nulpassF(float sample, float prev)
{
	int tmp = -prev + sample;
	if (0.0 == tmp)
	{
		setErr_code_str(0x3101, "nulpassF");
		return 1.0;
	}
	return -prev  / tmp;
}


void FiltInitDSP(Filt * flt )
{
	int index;
	FiltInit(flt);
	for (index=0;index<DEV_PeriodSamples;index++)
	{
		coefs[index] = flt->filtCoef[index];
	}
	arm_fir_init_f32(&S, NUMTAPS, &(coefs[0]), &(firState[0]), BLOCKSIZE);
}

void FiltInit(Filt * flt )
{
	int index =0;
	double cosres;
	//Расчет коэффициентов косинусной части алгортма Фурье для первой гармоники
	for (index = 0; index < DEV_PeriodSamples; index++)
	{
#ifndef MSVC
		cosres = arm_cos_f32(2.0f * MATH_Pi / (float)DEV_PeriodSamples*(float)index);
#else
		cosres = cos(2.0f * MATH_Pi / (float)DEV_PeriodSamples*(float)index);		
#endif
		flt->filtCoef[index] = (float)cosres;
	}
}

void FiltIntInit(FiltInt * flt )
{
	int index =0;
	double cosres;
	//Расчет коэффициентов косинусной части алгортма Фурье для первой гармоники
	for (index = 0; index < DEV_PeriodSamples; index++)
	{
#ifndef MSVC
		cosres =  arm_cos_f32(2.0f * MATH_Pi / (float)DEV_PeriodSamples*(float)index)*((int64_t)1<<32);
#else
		cosres = cos(2.0f * MATH_Pi / (float)DEV_PeriodSamples*(float)index)*((int64_t)1<<32);		
#endif
		flt->filtCoef[index] = (int64_t)cosres;
	}
}

//КИХ фильтр работающий по DEV_PeriodSamples количеству выборок. Коэффициенты предварительно рассчитываются в calcFiltCoef
//errCode 0x3302, 0x3303
float filter(int sample, Filt *filt)
{
	int index=0;
	fifo_int_push(&(filt->samples), sample);	//signalShiftInt(filt->samples , sample, DEV_PeriodSamples);
	if (getErr()!=0)
	{
		setErr_str("filter");
		return 0.0;
	}
	filt->result =0;
	for (index = 0; index < DEV_PeriodSamples; index++) //TODO: переписать под CMSIS в Keil
	{
		filt->result += filt->filtCoef[index] * fifo_int_oldnumber(&(filt->samples), DEV_PeriodSamples - 1 - index); //filt->samples[DEV_PeriodSamples - 1 - index];
		if (getErr()!=0)
		{
			setErr_str("filter");
			return 0.0;
		}
	}
	filt->result *= 2.0 / DEV_PeriodSamples;//Множитель для алгоритма Фурье
	return filt->result;
}

//КИХ фильтр работающий по DEV_PeriodSamples количеству выборок. Коэффициенты предварительно рассчитываются в calcFiltCoef
//errCode 0x3302, 0x3303
float filterDSP(int sample, Filt *filt)
{
	float inp;
	//float in;
	//int index=0;
	fifo_int_push(&(filt->samples), sample);	//signalShiftInt(filt->samples , sample, DEV_PeriodSamples);
//	if (getErr()!=0)
//	{
//		setErr_str("filter");
//		return 0.0;
//	}
//	filt->result =0;
//	for (index = 0; index < DEV_PeriodSamples; index++) //TODO: переписать под CMSIS в Keil
//	{
//		filt->result += filt->filtCoef[index] * fifo_int_oldnumber(&(filt->samples), DEV_PeriodSamples - 1 - index); //filt->samples[DEV_PeriodSamples - 1 - index];
//		if (getErr()!=0)
//		{
//			setErr_str("filter");
//			return 0.0;
//		}
//	}
	inp = (float)sample;
	arm_fir_f32(&S, &inp, &(filt->result), 1);
	filt->result *= 2.0 / DEV_PeriodSamples;//Множитель для алгоритма Фурье
	return filt->result;
}

//КИХ фильтр работающий по DEV_PeriodSamples количеству выборок. Коэффициенты предварительно рассчитываются в calcFiltCoef
//errCode 0x3302, 0x3303
int filterInt(int sample, FiltInt *filt)
{
	volatile int index=0;
	fifo_int_push(&(filt->samples), sample);	//signalShiftInt(filt->samples , sample, DEV_PeriodSamples);
	if (getErr()!=0)
	{
		setErr_str("filterInt");
		return 0;
	}
	filt->result =0;
	//sample <<=16;
	//index = DEV_PeriodSamples;
	for (index = 0; index < DEV_PeriodSamples; index++) //TODO: переписать под CMSIS в Keil
	//while(index)
	{	
	//filt->result += filt->filtCoef[index] * (int)sample; //filt->samples[DEV_PeriodSamples - 1 - index];
		filt->result += filt->filtCoef[index] * fifo_int_oldnumber(&(filt->samples), DEV_PeriodSamples - 1 - index); //filt->samples[DEV_PeriodSamples - 1 - index];
		if (getErr()!=0)
		{
			setErr_str("filterInt");
			return 0;
		}
		//index --;
	}
	filt->result *= 2;
	filt->result /= DEV_PeriodSamples;//Множитель для алгоритма Фурье
	filt->result >>=32;
	return filt->result;
}

//void signalShift ( int *buffer, int signal)
//{
//    buffer[0]  = buffer[1];
//    buffer[1]  = buffer[2];
//    buffer[2]  = buffer[3];
//    buffer[3]  = buffer[4];
//    buffer[4]  = buffer[5];
//    buffer[5]  = buffer[6];
//    buffer[6]  = buffer[7];
//    buffer[7]  = buffer[8];
//    buffer[8]  = buffer[9];
//    buffer[9]  = buffer[10];
//    buffer[10] = buffer[11];
//    buffer[11] = buffer[12];
//    buffer[12] = buffer[13];
//    buffer[13] = buffer[14];
//    buffer[14] = buffer[15];
//    buffer[15] = buffer[16];
//    buffer[16] = buffer[17];
//    buffer[17] = buffer[18];
//    buffer[18] = buffer[19];
//
//    buffer[19] = signal;
//}
//
//void signalShiftInt ( int *buffer, int signal, int length)
//{
//	int index = 0;
//	for (index=0; index<length-1; index++)
//	{
//		buffer[index] = buffer[index+1];
//	}
//	buffer[length-1] = signal;
//
//}
//
//void signalShiftUINT (unsigned int *buffer, unsigned int signal)
//{
//    buffer[0]  = buffer[1];
//    buffer[1]  = buffer[2];
//    buffer[2]  = buffer[3];
//    buffer[3]  = buffer[4];
//    buffer[4]  = buffer[5];
//    buffer[5]  = buffer[6];
//    buffer[6]  = buffer[7];
//    buffer[7]  = buffer[8];
//    buffer[8]  = buffer[9];
//    buffer[9]  = buffer[10];
//    buffer[10] = buffer[11];
//    buffer[11] = buffer[12];
//    buffer[12] = buffer[13];
//    buffer[13] = buffer[14];
//    buffer[14] = buffer[15];
//    buffer[15] = buffer[16];
//    buffer[16] = buffer[17];
//    buffer[17] = buffer[18];
//    buffer[18] = buffer[19];
//
//    buffer[19] = signal;
//}
//


void signalShiftF (float *buffer, float signal, int length)
{
	int index = 0;
	for (index=0; index<length-1; index++)
	{
		buffer[index] = buffer[index+1];
	}
	buffer[length-1] = signal;
}
