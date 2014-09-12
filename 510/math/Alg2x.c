// test
//
//  Alg2x.c: определяет точку входа для консольного приложения.
//


#ifdef MSVC
	#include "stdafx.h"
#endif

#include "matem.h"
#include "FreqAlg.h"
#include "MeasAlgs.h"
#include "Logic.h"
#include "../std_types.h"
#include "ErrHelper.h"
#include <stdlib.h>
#include <stdio.h>

void testFrequensy(void);
void testFrequensyIntFlt(void);
short signal(double t);
void testInterp(void);
void testResampling(void);
void testFourier(void);
void calCoefsFFT(void);
void testSymComp(void);
void test_p2p(void);
void testmeasNsignals(void);
void signal3ph(double t, short * res);
void testmeasNsignals(void);
void testOCP(void);
void calcPow002Table(void);
void calcLnTable(void);
void testmyLn(void);
void tespow002(void);
void testtimedelay(void);
void testOCPCurTime(void);

//MAIN
#ifndef MSVC
	int main(void)
#else
	int _tmain(int argc, _TCHAR* argv[])
#endif
{
	testFrequensyIntFlt();
	//testFrequensy();
	//testInterp();
	//testResampling();
	//calCoefsFFT();
	//testFourier();
	//testSymComp();
	//test_p2p();
	//testmeasNsignals();
	//testOCP();
	 //calcPow002Table();
	//calcLnTable();
	//testmyLn();
	//tespow002();
	//testtimedelay();

	//int res;
	//int vl[] = {3,2,1};
	//int nm[3];
	//res = max3(vl, nm);

	while (1)
	{}
	return 0;
}

//void testOCPCurTime()
//{

//}

//void testtimedelay()
//{
//	int index;
//	int index1=0;
//	float time;
//	FILE *fl;
//	TimerData td;
//	td.type = 6;
//	td.koefT = 0.5;

//	#pragma warning(disable : 4996)
//	fl = fopen("inverse6.txt", "w");

//	for (index = 11; index<400;index++)
//	{
//		for (index1 = 0;index1<=10; index1++)
//		{
//			if (index1 == 0)
//			td.koefT = 0.05;
//			else
//			{
//				td.koefT = index1/10.0;
//			}
//			calcTripTimerTime(&td, index/10.0);
//			fprintf(fl ,"%f; ", td.trip_set/(float)DEV_SampleFreq);
//			//printf("%i, %f\r\n", index, td.trip_set/(float)DEV_SampleFreq);
//		}
//		fprintf(fl, "\r");
//	}
//	fclose(fl);
//}

//void testOCP()
//{
//	int index = 0;
//	int cur =0;

//	OCPData ocpd;
//	ocpd.enbled = true;
//	ocpd.k_reset=0.9;
//	ocpd.set_I = 20;
//	ocpd.out_start=false;
//	ocpd.out_trip =false;
//	ocpd.t1.counter=0;
//	ocpd.t1.enabled=true;
//	ocpd.t1.out=false;
//	ocpd.t1.in_start=false;
//	ocpd.t1.step=96;
//	ocpd.t1.trip_set=10;
//	ocpd.t1.type=5;
//	ocpd.t1.koefT=0.05;
//	ocpd.out_trip=false;

//	for (index=0; index<100; index++)
//	{
//		//if (index > 3)
//		//	cur = 20;
//		//if (index > 5)
//		//	cur = 30;
//		cur = index;

//		if (index>25)
//			cur = 17;
//		if (index>27)
//			cur = 66;
//		ocp_work(cur, &ocpd);
//		printf("%i; %i; %i; %i\r\n", index, cur, ocpd.out_start, ocpd.out_trip);
//	}
//	getchar();
//}

//void test_p2p()
//{
//	float buf1[32];
//	int sample, index =0, res, resRMS;

//	for (index=0;index<32;index++)
//	{
//		buf1[index]=0;
//	}

//	for ( index = 0; index < 1600; index++)
//	{
//		sample = 6000*sin(50*2*MATH_Pi* (index * 1.0/1600.0));
//		signalShiftF(buf1, sample, 32);
//		res = peakToPeak(buf1);
//		resRMS = RMS32(buf1);
//	}
//}

//void testSymComp()
//{
//	ComplexF Ia = {1, 2};
//	ComplexF Ib = {2, 1};
//	ComplexF Ic = {2, 2};
//	ComplexF I1, I2, I0, Ia1, Ib1, Ic1;

//	I1 = calcPhase2Sym1(&Ia, &Ib, &Ic);
//	I2 = calcPhase2Sym2(&Ia, &Ib, &Ic);
//	I0 = calcPhase2Sym0(&Ia, &Ib, &Ic);

//	Ia1 = calcSym2PhaseA(&I1, &I2, &I0); 
//	Ib1 = calcSym2PhaseB(&I1, &I2, &I0); 
//	Ic1 = calcSym2PhaseC(&I1, &I2, &I0); 
//}

//void testInterp()
//{
//	int buf[] = {1, 2, 3}, buf1[] = {2, 3, 1};
//	float res1, res = lineInterpolation(buf, 3, 0.5f, 0.9f);
//	res1 = lineInterpolation(buf1, 3,  0.5f, 1.0f);
//}

//void testResampling()
//{
//	int buf[256];
//	Fifo_int fifo;
//	float buf1[32];
//	int sample, index =0;

//	fifo.size = 256;
//	fifo.buf = buf;
//	fifo_initialize_int(&fifo);
//	
//	for ( index = 0; index <= 250; index++)
//	{
//		sample = index;//signal(index * 1.0/DEV_SampleFreq);
//		//buf[index] = sample;
//		fifo_int_push( &fifo,sample);
//	}
//	//resampling(fifo.buf, fifo.size, fifo.head, 1.0/DEV_SampleFreq, 1.0/DEV_SampleFreq * 192, 32, buf1);
//	resampling_fifo(&fifo,1.0, 2, 32, buf1);
//	//resampling(buf, 250, 250, 1.0/DEV_SampleFreq, 1.0/DEV_SampleFreq * 192, 32, buf1);
//	sample = buf1[31];
//}

//void testFourier()
//{
//	float buf1[32];
//	float Z=0, Z1;
//	float argZ = 0, argZ1, at, at2;
//	int sample, index =0;
//	ComplexF ampl;
//	
//	for (index=0;index<32;index++)
//	{
//		buf1[index]=0;
//	}

//	for ( index = 0; index < 1600; index++)
//	{
//		sample = signal(index * 1.0/1600.0);
//		signalShiftF(buf1, sample, 32);
//		ampl = fourier9th32s(buf1);
//		Z = complex_abs(&ampl);
//		argZ = complex_angle(&ampl)/MATH_Pi*180.0;
//		Z1 = complex_abs(&ampl);
//		argZ1 = complex_angle(&ampl)/MATH_Pi*180.0;
//	}
//}

//double currentF = 0;

//float curFreqBuf[20];

////FILE * file=0;

////FILE *sinFile=0;

//FILE *forierFreqFile=0;

//FILE *forierFreqFile3ph=0;

void testFrequensyIntFlt()
{
	FreqF frf_flt;
	int i = 0;
	__int64 index=0;
	int sample=0;
	ComplexF ampl;
	int fifobuf[BUFSIZE_HI];
	Fifo_int signalBuf;
	float amplABS, amplAngle;

	signalBuf.size = BUFSIZE_HI;
	signalBuf.buf = fifobuf;
	fifo_initialize_int(&signalBuf);

	frf_flt = FreqFInit();

	//#pragma warning(disable : 4996)
	//file = fopen("freq.txt", "w");
	//sinFile = fopen("sin.txt", "w");
	//forierFreqFile = fopen("forierFreqFile.txt", "w");

	for ( index = 0; index < DEV_SampleFreq; index++)
	{
		if(index>95000)
			sample=0;
		//fprintf(sinFile, "%f; ", index * 1.0/DEV_SampleFreq);
		sample = signal(index * 1.0/DEV_SampleFreq);
		fifo_int_push(&signalBuf, sample);
		// sample = signal(index * 1/1600.0);
		periodCalcF_flt(sample, &frf_flt);

		
		ampl = fourier1st32sFreq(&signalBuf, frf_flt.T);
		amplABS = complex_abs(&ampl);
		amplAngle = complex_angle(&ampl);

		if (frf_flt.isCalc)
		{
			//signalShiftF( curFreqBuf, currentF, 20);
			frf_flt.isCalc = false;
			//fprintf(file, "%f; %f; %f; %f; %f", index * 1.0/DEV_SampleFreq, currentF, frf_flt.frequency, frf_flt.dfdt, curFreqBuf[17] - frf_flt.frequency);
				//	fprintf(file, "\r");
		}

		//fprintf(sinFile, "\r", index * 1.0/DEV_SampleFreq);
		//fprintf(forierFreqFile, "%i; %f; %f \r", sample, amplABS, amplAngle);
	}

	//fclose(file);
	//fclose(sinFile);
	//fclose(forierFreqFile);
}

void testFrequensy()
{
	FreqF frf_flt;
	int i = 0;
	__int64 index=0;
	int sample=0;
	ComplexF ampl;
	int fifobuf[BUFSIZE_HI];
	Fifo_int signalBuf;
	float amplABS, amplAngle;

	signalBuf.size = BUFSIZE_HI;
	signalBuf.buf = fifobuf;
	fifo_initialize_int(&signalBuf);

	frf_flt = FreqFInit();

	//#pragma warning(disable : 4996)
	//file = fopen("freq.txt", "w");
	//sinFile = fopen("sin.txt", "w");
	//forierFreqFile = fopen("forierFreqFile.txt", "w");

	for ( index = 0; index < DEV_SampleFreq; index++)
	{
		if(index>95000)
			sample=0;
		//fprintf(sinFile, "%f; ", index * 1.0/DEV_SampleFreq);
		sample = signal(index * 1.0/DEV_SampleFreq);
		fifo_int_push(&signalBuf, sample);
		// sample = signal(index * 1/1600.0);
		periodCalcF_flt(sample, &frf_flt);

		if (frf_flt.isCalc)
		{
			//signalShiftF( curFreqBuf, currentF, 20);
			frf_flt.isCalc = false;
			//fprintf(file, "%f; %f; %f; %f; %f", index * 1.0/DEV_SampleFreq, currentF, frf_flt.frequency, frf_flt.dfdt, curFreqBuf[17] - frf_flt.frequency);
				//	fprintf(file, "\r");
		}

		ampl = fourier1st32sFreq(&signalBuf, frf_flt.T);
		amplABS = complex_abs(&ampl);
		amplAngle = complex_angle(&ampl);

		//fprintf(sinFile, "\r", index * 1.0/DEV_SampleFreq);
		//fprintf(forierFreqFile, "%i; %f; %f \r", sample, amplABS, amplAngle);
	}

	//fclose(file);
	//fclose(sinFile);
	//fclose(forierFreqFile);
}

//void testmeasNsignals()
//{
//	FreqF frf_flt;
//	ComplexF vect[DEV_numberAnalogChannels];
//	Vectors res;
//	short sg[3];
//	int index=0;
//	float a,b,c;
//	res.size=DEV_numberAnalogChannels;

//	//инициализирует структуру для расчета частоты
//	frf_flt = FreqFInit();

//	initSignalBufers();

//	res.size = DEV_numberAnalogChannels;
//	res.vect = vect;

//	//#pragma warning(disable : 4996)
//	//file = fopen("freq.txt", "w");
//	//sinFile = fopen("sin.txt", "w");
//	//forierFreqFile3ph = fopen("forierFreqFile3ph.txt", "w");

//	for (index=0; index<DEV_SampleFreq*10; index++)
//	{
//		signal3ph(index * 1.0/DEV_SampleFreq, &sg[0]);

//		measNsignals(&sg[0], &frf_flt, &res);
//		a = complex_abs(&res.vect[0]);
//		b = complex_abs(&res.vect[1]);
//		c = complex_abs(&res.vect[2]);
//		//fprintf(forierFreqFile3ph, "%f; %f; %f; %f; %f; %f; %f; %f\r", a,b,c, frf_flt.frequency, frf_flt.dfdt, complex_angle(&res.vect[0]), complex_angle(&res.vect[1]), complex_angle(&res.vect[2]));
//	}
//	
//	fclose(forierFreqFile3ph);
//}

//Значение сигнала
//t - с
//sin(fi0 - 2pi(fo*t + kt^2/2))
short signal(double t)
{
	int rnd=0;
	double pi=0;
	short signal=0;
	short signalInst=0;
	double f0=DEV_WorkFreq;
	double dfdt = 0;
	//currentF = f0  + dfdt * t;
	//rnd = rand() % 650;// * (((int)(t * DEV_SampleFreq) )% 2);
	pi = MATH_Pi;
	//short signal = 6550*sin( 2*pi*(f0 * t + dfdt * t*t / 2)) + 3000 * sin( 2*pi*(3*f0 * t + dfdt * t*t / 2)-0.6*pi) +	2000 * sin( 2*pi*(4*f0 * t + dfdt * t*t / 2)-0.2*pi) + 2000 * sin( 2*pi*(2*f0 * t + dfdt * t*t / 2)-0.2*pi)+ rnd;
	//short signal = 6550*sin( 2*pi*(f0 * t + dfdt * t*t / 2)) + 500 * sin( 2*pi*(3*f0 * t + dfdt * t*t / 2)-0.6*pi) +	500 * sin( 2*pi*(4*f0 * t + dfdt * t*t / 2)-0.2*pi) + 500 * sin( 2*pi*(2*f0 * t + dfdt * t*t / 2)-0.2*pi);//+ rnd;
#ifdef MSVC
	signal = 6550*sin(2*pi*(f0 * t + dfdt * t*t / 2))+3500*sin(2*pi*2*(f0 * t + dfdt * t*t / 2))+1000*sin(2*pi*3*(f0 * t + dfdt * t*t / 2));
		//+4000*sin(2*5*pi*(f0 * t + dfdt * t*t / 2))+4500*sin(2*7*pi*(f0 * t + dfdt * t*t / 2))+5000*sin(2*9*pi*(f0 * t + dfdt * t*t / 2));//+rnd;
	signalInst = 6550*sin(2*pi*(f0 * t + dfdt * t*t / 2));
#else
	signal = 6550*arm_sin_f32( -2*pi*(f0 * t + dfdt * t*t / 2));//+rnd;
	signalInst = 6550*arm_sin_f32( -2*pi*(f0 * t + dfdt * t*t / 2));
#endif

	//fprintf(sinFile, "%i;", signalInst);
	return signal;
}

////Значение сигнала
////t - с
////short * res - выходной массив длинной 3
////sin(fi0 - 2pi(fo*t + kt^2/2))
//void signal3ph(double t, short * res)
//{
//	int rnd=0, ampl = 6550;
//	double pi=0;
//	short signal1, signal2, signal3;
//	short signalInst=0;
//	double f0=DEV_WorkFreq;
//	double dfdt = 5;
//	double fi = 0;
//	//currentF = f0  + dfdt * t;
//	rnd = rand() % 650;// * (((int)(t * DEV_SampleFreq) )% 2);
//	pi = MATH_Pi;
//	
//	if (t>2)
//		ampl =  10000;
//	if (t>2)
//		fi = 180*pi/180.0;

//#ifdef MSVC
//	res[0] = ampl*sin(2*pi*(f0 * t + dfdt * t*t / 2) + fi);//+3500*sin(2*pi*2*(f0 * t + dfdt * t*t / 2))+1000*sin(2*pi*3*(f0 * t + dfdt * t*t / 2));
//		//+4000*sin(2*5*pi*(f0 * t + dfdt * t*t / 2))+4500*sin(2*7*pi*(f0 * t + dfdt * t*t / 2))+5000*sin(2*9*pi*(f0 * t + dfdt * t*t / 2));//+rnd;
//	res[1] = ampl*sin(2*pi*(f0 * t + dfdt * t*t / 2) + 240.0*pi/180.0 + fi);
//	res[2] = ampl*sin(2*pi*(f0 * t + dfdt * t*t / 2) + 120.0*pi/180.0 + fi);
//	//signalInst = 6550*sin(2*pi*(f0 * t + dfdt * t*t / 2));
//#else
//	res[0] = ampl*arm_sin_f32(2*pi*(f0 * t + dfdt * t*t / 2) + fi);//+3500*sin(2*pi*2*(f0 * t + dfdt * t*t / 2))+1000*sin(2*pi*3*(f0 * t + dfdt * t*t / 2));
//		//+4000*sin(2*5*pi*(f0 * t + dfdt * t*t / 2))+4500*sin(2*7*pi*(f0 * t + dfdt * t*t / 2))+5000*sin(2*9*pi*(f0 * t + dfdt * t*t / 2));//+rnd;
//	res[1] = ampl*arm_sin_f32(2*pi*(f0 * t + dfdt * t*t / 2) + 240.0*pi/180.0 + fi);
//	res[2] = ampl*arm_sin_f32(2*pi*(f0 * t + dfdt * t*t / 2) + 120.0*pi/180.0 + fi);
//#endif

//	//fprintf(sinFile, "%i;", signalInst);
//}

//void calCoefsFFT()
//{
//	FILE *fl;
//	double csin[32], ccos[32];
//	int index, index1;
//	double coef;

//#pragma warning(disable : 4996)
//	fl = fopen("coefs", "w");

//	for (index1=1; index1<=10;index1++)
//	{
//		fprintf(fl, "\rsin%i=\r", index1);
//		for (index=0; index<32; index++)
//		{
//			if (index%8 == 0)
//			{
//				fprintf(fl, "\r");
//			}
//			coef = sin(2.0 * index1* MATH_Pi /32.0 * (32-index));
//			csin[index] = coef;
//			fprintf(fl, "%0.7f,	", coef);
//		}
//		fprintf(fl, "\rcos%i=\r", index1);
//		for (index=0; index<32; index++)
//		{
//			if (index%8 == 0)
//			{
//				fprintf(fl, "\r");
//			}
//			coef = cos(2.0 * index1 * MATH_Pi /32.0 * (32-index));
//			csin[index] = coef;
//			fprintf(fl, "%0.7f,	", coef);
//		}
//	}
//}

//void testmyLn()
//{
//	int index;
//	float tmp, res;

//	for (index=10; index<100; index++)
//	{
//		tmp = index/1.0;
//		res=myLn(tmp);
//		printf("%f %f %f %f\r\n", tmp, logf(tmp), res, (logf(tmp) - res)/logf(tmp)*100);
//	}
//}

//void tespow002()
//{
//	int index;
//	float tmp, res;

//	for (index=0; index<100; index++)
//	{
//		tmp = index/10.0;
//		res=pow002(tmp);
//		printf("%f %f %f %f\r\n", tmp, powf(tmp, 0.02), res, (powf(tmp, 0.02) - res)/powf(tmp, 0.02)*100);
//	}
//}

//void  calcPow002Table()
//{
//	int index;
//	float res, kr;
//	FILE *tbl;
//	#pragma warning(disable : 4996)

//	tbl = fopen("002table.txt", "w");

//	for (index =0; index<42; index++)
//	{
//		kr = 1 + index * 21.0 / 42.0;
//		res = powf(kr, 0.02);
//		if (index%5 == 0)
//		{
//			fprintf(tbl, "\r");
//		}
//		fprintf(tbl, "%f, ", res);
//		printf("%f; %f \r\n", kr, res);
//	}
//	fclose(tbl);
//}

//void  calcLnTable()
//{
//	int index;
//	int counter;
//	float res, kr;
//	FILE *tbly, *tblx, *tbl2;
//	#pragma warning(disable : 4996)

//	tbly = fopen("Ln002tabley.txt", "w");
//	tblx = fopen("Ln002tablex.txt", "w");
//	tbl2 = fopen("Ln002table_.txt", "w");

//	counter = 0;

//	for (index = 0; index<20; index++)
//	{
//		kr = 1 + index/10.0;
//		res = logf(kr);
//		if (counter%5 == 0)
//		{
//			fprintf(tbly, "\r");
//			fprintf(tblx, "\r");
//		}
//		fprintf(tbly, "%f, ", res);
//		fprintf(tblx, "%f, ", kr);
//		printf("%f; %f \r\n", kr, res);
//		fprintf(tbl2, "%i; %f; %f\r\n", index, kr, res);
//		counter++;
//	}
//	for (index = 3; index <= 40; index ++)
//	{
//		kr = index;
//		res = logf(kr);
//		if (counter%5 == 0)
//		{
//			fprintf(tbly, "\r");
//			fprintf(tblx, "\r");
//		}
//		fprintf(tbly, "%f, ", res);
//		fprintf(tblx, "%f, ", kr);
//		printf("%f; %f \r\n", kr, res);
//		fprintf(tbl2, "%i; %f; %f\r\n", index, kr, res);
//		counter++;
//	}
//	for (index = 50; index <=800; index+=10)
//	{
//		kr = index;
//		res = logf(kr);
//		if (counter%5 == 0)
//		{
//			fprintf(tbly, "\r");
//			fprintf(tblx, "\r");
//		}
//		fprintf(tbly, "%f, ", res);
//		fprintf(tblx, "%f, ", kr);
//		printf("%f; %f \r\n", kr, res);
//		fprintf(tbl2, "%i; %f; %f\r\n", index, kr, res);
//		counter++;
//	}
//	fclose(tbly);
//	fclose(tblx);
//	fclose(tbl2);
//}
