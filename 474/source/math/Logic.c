#include "Logic.h"

//Таблица значений степени x^0,02 c шагом 0,5 от 1 до 21,5---
static const float pow002table[]=
{
1.000000, 1.008142, 1.013960, 1.018495, 1.022215, 
1.025372, 1.028114, 1.030539, 1.032712, 1.034683, 
1.036485, 1.038146, 1.039685, 1.041121, 1.042466, 
1.043730, 1.044924, 1.046055, 1.047129, 1.048151, 
1.049127, 1.050060, 1.050954, 1.051812, 1.052638, 
1.053432, 1.054199, 1.054939, 1.055655, 1.056347, 
1.057018, 1.057669, 1.058300, 1.058914, 1.059511, 
1.060092, 1.060657, 1.061208, 1.061746, 1.062270, 
1.062783, 1.063283
};

//Таблица значений натурального логарифма от 1 до 800.
//при 1<=x<3 шаг 0.1, 3<=x<=40 шаг 1.0, при 40<x<=800 шаг 10.0 
static const float lnTabley[]=
{
0.000000, 0.095310, 0.182322, 0.262364, 0.336472, 
0.405465, 0.470004, 0.530628, 0.587787, 0.641854, 
0.693147, 0.741937, 0.788457, 0.832909, 0.875469, 
0.916291, 0.955511, 0.993252, 1.029619, 1.064711, 
1.098612, 1.386294, 1.609438, 1.791759, 1.945910, 
2.079442, 2.197225, 2.302585, 2.397895, 2.484907, 
2.564949, 2.639057, 2.708050, 2.772589, 2.833213, 
2.890372, 2.944439, 2.995732, 3.044523, 3.091043, 
3.135494, 3.178054, 3.218876, 3.258096, 3.295837, 
3.332205, 3.367296, 3.401197, 3.433987, 3.465736, 
3.496508, 3.526361, 3.555348, 3.583519, 3.610918, 
3.637586, 3.663562, 3.688879, 3.912023, 4.094345, 
4.248495, 4.382027, 4.499810, 4.605170, 4.700480, 
4.787492, 4.867535, 4.941642, 5.010635, 5.075174, 
5.135798, 5.192957, 5.247024, 5.298317, 5.347107, 
5.393628, 5.438079, 5.480639, 5.521461, 5.560682, 
5.598422, 5.634789, 5.669881, 5.703783, 5.736572, 
5.768321, 5.799093, 5.828946, 5.857933, 5.886104, 
5.913503, 5.940171, 5.966147, 5.991465, 6.016157, 
6.040255, 6.063785, 6.086775, 6.109248, 6.131227, 
6.152733, 6.173786, 6.194406, 6.214608, 6.234411, 
6.253829, 6.272877, 6.291569, 6.309918, 6.327937, 
6.345636, 6.363028, 6.380123, 6.396930, 6.413459, 
6.429719, 6.445720, 6.461468, 6.476973, 6.492240, 
6.507277, 6.522093, 6.536692, 6.551080, 6.565265, 
6.579251, 6.593045, 6.606650, 6.620073, 6.633318, 
6.646390, 6.659294, 6.672033, 6.684612
};

//Таблица аргументов для таблици натурального логарифма lnTabley.
//при 1<=x<3 шаг 0.1, 3<=x<=40 шаг 1.0, при 40<x<=800 шаг 10.0 
static const float lnTablex[]=
{
1.000000, 1.100000, 1.200000, 1.300000, 1.400000, 
1.500000, 1.600000, 1.700000, 1.800000, 1.900000, 
2.000000, 2.100000, 2.200000, 2.300000, 2.400000, 
2.500000, 2.600000, 2.700000, 2.800000, 2.900000, 
3.000000, 4.000000, 5.000000, 6.000000, 7.000000, 
8.000000, 9.000000, 10.000000, 11.000000, 12.000000, 
13.000000, 14.000000, 15.000000, 16.000000, 17.000000, 
18.000000, 19.000000, 20.000000, 21.000000, 22.000000, 
23.000000, 24.000000, 25.000000, 26.000000, 27.000000, 
28.000000, 29.000000, 30.000000, 31.000000, 32.000000, 
33.000000, 34.000000, 35.000000, 36.000000, 37.000000, 
38.000000, 39.000000, 40.000000, 50.000000, 60.000000, 
70.000000, 80.000000, 90.000000, 100.000000, 110.000000, 
120.000000, 130.000000, 140.000000, 150.000000, 160.000000, 
170.000000, 180.000000, 190.000000, 200.000000, 210.000000, 
220.000000, 230.000000, 240.000000, 250.000000, 260.000000, 
270.000000, 280.000000, 290.000000, 300.000000, 310.000000, 
320.000000, 330.000000, 340.000000, 350.000000, 360.000000, 
370.000000, 380.000000, 390.000000, 400.000000, 410.000000, 
420.000000, 430.000000, 440.000000, 450.000000, 460.000000, 
470.000000, 480.000000, 490.000000, 500.000000, 510.000000, 
520.000000, 530.000000, 540.000000, 550.000000, 560.000000, 
570.000000, 580.000000, 590.000000, 600.000000, 610.000000, 
620.000000, 630.000000, 640.000000, 650.000000, 660.000000, 
670.000000, 680.000000, 690.000000, 700.000000, 710.000000, 
720.000000, 730.000000, 740.000000, 750.000000, 760.000000, 
770.000000, 780.000000, 790.000000, 800.000000
};

TimerData TimerDataInit()
{
	TimerData td;
	td.counter = 0;
	td.enabled = false;
	td.in_start = false;
	td.koefT = 0.0f;
	td.out = false;
	td.step = 0;
	td.trip_set = 0;
	td.type = 0;
	return td;
}

OCPData OCPDataInit()
{
	OCPData ocp;
	ocp.enbled = false;
	ocp.in_rpcTime = false;
	ocp.k_reset = 0.9;
	ocp.out_start = false;
	ocp.out_trip = false;
	ocp.set_I = 0;
	ocp.t1 = TimerDataInit();
	return ocp;
}

OCP3tData OCP3tDataInit()
{
	OCP3tData ocp;
	ocp.enbled = false;
	ocp.k_reset = 0.9;
	ocp.set_I = 0;
	ocp.start = false;
	ocp.t1 = TimerDataInit();
	ocp.t2 = TimerDataInit();
	ocp.t3 = TimerDataInit();
	ocp.trip1 = false;
	ocp.trip2 = false;
	ocp.trip3 = false;
	ocp.x2setEnable = false;
}

// region OCP

//Обработка максимальной защиты (по повышению параметра) с одной выдержкой времени
//Вход:
//int current - значение максимального параметра по фазам (тока или напряжения);
//OCPData * data - структура с информацией о состоянии защиты (изменяется внутри функции);
//errCode 0x4201, 0x3304
void ocp_work(int current, OCPData * data)
{
	int set;
	//Если защита не активна - все выхода в 0
	if (!data->enbled)
	{
		data->out_start=false;
		data->out_trip=false;
		return;
	}
	//Если есть превышение уставки - пуск защиты
	if ((data->in_rpcTime) && (data->x2setEnable))//проверка удвоения уствки
		set = data->set_I * 2.0;
	else
		set = data->set_I;
	if (current >= set)
	{
		data->out_start = true;
	}
	else
	{
		//Возврат пуска с коэффициентом возврата
		if (current < data->k_reset * set)
		{
			data->out_start = false;
			data->out_trip = false;
			data->t1.counter=0;
			data->t1.out=false;
			data->t1.in_start=false;
			return;
		}
	}
	//Обработка таймера
	data->t1.in_start = data->out_start;
	if (data->t1.type != 0)
		inverseTimer(&data->t1, current/(float)(data->set_I));
	else
		ttimer(&data->t1);

	setErr_str("ocp_work");

	//Выход таймера на выход защиты
	data->out_trip = data->t1.out;
}

//Обработка максимальной защиты (по повышению параметра) с тремя выдержками времени
//Вход:
//int current - значение максимального параметра по фазам (тока или напряжения);
//OCP3tData * data - структура с информацией о состоянии защиты (изменяется внутри функции);
//errCode 0x4201, 0x3304
void ocp3t_work(int current, OCP3tData * data)
{
	int set;
	//Если защита не активна - все выхода в 0
	if (!data->enbled)
	{
		data->start=false;
		data->trip1=false;
		data->trip2=false;
		data->trip3=false;
		return;
	}
	//Если есть превышение уставки - пуск защиты	if (data->setX2)//проверка удвоения уствки
	if (data->x2setEnable)//проверка удвоения уствки
		set = data->set_I * 2.0f;
	else
		set = data->set_I;
	if (current >= set)
	{
		data->start = true;
	}
	else
	{
		//Возврат пуска с коэффициентом возврата
		if (current < data->k_reset * set)
		{
			data->start = false;
			data->trip1 = false;
			data->trip2 = false;
			data->trip3 = false;
			data->t1.counter=0;
			data->t1.out=false;
			data->t1.in_start=false;
			data->t2.counter=0;
			data->t2.out=false;
			data->t2.in_start=false;
			data->t3.counter=0;
			data->t3.out=false;
			data->t3.in_start=false;
			return;
		}
	}

	//Обработка тайметров
	data->t1.in_start = data->start;
	if (data->t1.type != 0)
		inverseTimer(&data->t1, current/(float)(data->set_I));
	else
		ttimer(&data->t1);

	data->t2.in_start = data->start;
	if (data->t2.type != 0)
		inverseTimer(&data->t2, current/(float)(data->set_I));
	else
		ttimer(&data->t2);

	data->t3.in_start = data->start;
	if (data->t3.type != 0)
		inverseTimer(&data->t3, current/(float)(data->set_I));
	else
		ttimer(&data->t3);

	if (getErr() != 0)
		setErr_str("ocp3t_work");

	//Выходы тайметров на выходы защиты.
	data->trip1 = data->t1.out;
	data->trip2 = data->t2.out;
	data->trip3 = data->t3.out;
}

// endregion OCP

// region timers

//Обработка таймера с независимой выдержкой времени
//TimerData * data - структура описывающая текущее состояние таймера (изменяется внутри ф-и);
//errCode 0x4201
void ttimer(TimerData * data)
{	
	if (!data->enabled)
	{
		data->out = false;
		return;
	}
	if (0xffffffff - data->step-1 > data->step)//проверка на переполнение
		setErr_code_str(0x4201, "timer");
	
	if (data->in_start)
		if (data->counter < data->trip_set)
			data->counter+=data->step;
		else
			data->out = true;
}

//Обработка таймера возможностью выбора типа выдержки времени
//TimerData * data - структура описывающая текущее состояние таймера (изменяется внутри ф-и);
//float curDim - кратность тока к уставке (I/Iуст);
//errCode 0x4201, 0x3304
void inverseTimer(TimerData * data, float curDim)
{
	if (!data->enabled)
	{
		data->out = false;
		return;
	}
	if (0xffffffff - data->step-1 < data->counter)//проверка на переполнение
		setErr_code_str(0x4201, "inverseTimer");
	
	if (data->in_start)
	{
		if (calcTripTimerTime(data, curDim))
			if (data->counter < data->trip_set)
				data->counter+=data->step;
			else
				data->out = true;
		else
			data->out = false;
	}
}

//Расчет времени таймера с токозависимой характеристикой.
//Вычисляет время уставки таймера в выборках АЦП.
//TimerData * data - структура описывающая текущее состояние таймера (изменяется внутри ф-и);
//float dim - кратность тока к уставке (I/Iуст);
//Выход:
//bool res - был ли произведен расчет
//errCode 0x3304
bool calcTripTimerTime(TimerData * data, float dim)
{
	float set=0;
	float tmp;
	if (dim < 1.1f)
		return false;

	if (data->koefT<0.05f || data->koefT>10)
	{
		setErr_code_str(0x3304, "calcTripTimerTime");
		return false;
	}

	switch (data->type)
	{
	case 1://Длительно инверсная
		tmp = (dim - 1);
		set = data->koefT * 120.0f / tmp;
		break;
	case 2://Инверсная
		tmp = (pow002(dim) - 1);
		set = data->koefT * 0.14f / tmp;
		break;
	case 3://Сильно инверсная
		tmp = (dim - 1);
		set = data->koefT * 13.5f / tmp;
		break;
	case 4://Чрезвычаянно инверсная
		tmp = (dim * dim - 1);
		set = data->koefT * 80.0f / tmp;
		if (set < 0.01f)
			set = 0.01f;
		break;
	case 5://RXIDG типа.
		set = 5.8f - 1.35f * myLn(dim/data->koefT);
		if (set < 0.03f)
			set = 0.03f;
		break;
	case 6://RI типа
		set = data->koefT / (0.39f - 0.236f / dim);
		if (set < 0.03f)
			set = 0.03f;
	default:
		break;
	}
	data->trip_set = set * DEV_SampleFreq;
	return true;
}

// endregion timers

// region matem

//Находит x^0.02 по таблице в диапазоне от 1 до 21.5
//errCode 0x3304
float pow002(float x)
{
	int px1;
	if (x < 1)
	{
		setErr_code_str(0x3304,"pow002");
		return 0;
	}
	if (x >= 21.5f)//экстраполяция
		return	pow002table[41]+(pow002table[41]-pow002table[40])/0.5f * (x - 21.5f);
	else
	{
		px1 = (int)(x*2.0f)-2;
		return pow002table[px1]+(pow002table[px1+1]-pow002table[px1])/0.5f * (x - (px1+2)/2.0f);
	}
}

//Возвраящает натуральный логарифм заданный в табличном виде в диапазое x от 1.0 до 800.0
//errCode 0x3304
float myLn(float x)
{
	int index;

	if (x < 1)
	{
		setErr_code_str(0x3304,"myLn");
		return 0;
	}
	if (x >= 800)//экстраполяция
		return	lnTabley[133]+(lnTabley[133]-lnTabley[132])/10.0f * (x - 800);
	else
	{
		if (x < 3)
            for (index = 0; index < 20; index ++)
            {
                if (x>= lnTablex[index] && x<lnTablex[index+1])
                    return lnTabley[index]+(lnTabley[index+1]-lnTabley[index])/(lnTablex[index+1] - lnTablex[index]) * (x - lnTablex[index]);
            }
		if (x >= 3 && x < 40)
			for (index = 20; index < 57; index ++)
            {
                if (x>= lnTablex[index] && x<lnTablex[index+1])
                    return lnTabley[index]+(lnTabley[index+1]-lnTabley[index])/(lnTablex[index+1] - lnTablex[index]) * (x - lnTablex[index]);
            }
		if (x >= 40)
			for (index = 57; index < 133; index ++)
            {
                if (x>= lnTablex[index] && x<lnTablex[index+1])
                    return lnTabley[index]+(lnTabley[index+1]-lnTabley[index])/(lnTablex[index+1] - lnTablex[index]) * (x - lnTablex[index]);
            }
		//for (index = 0; index < 133; index ++)
		//{
		//	if (x>= lnTablex[index] && x<lnTablex[index+1])
		//		return lnTabley[index]+(lnTabley[index+1]-lnTabley[index])/(lnTablex[index+1] - lnTablex[index]) * (x - lnTablex[index]);
		//}
	}
	return 0.0;
}

// endregion matem

// region statistic

//Находит максимальный элемент массива, возвращает новый порядок элементов массива при сортировке по возрастанию.
//Вход:
//int val[3] - массив значений
//int * num - ссылка на массив индексов исходного массива после сортировки по возратания (int[3]) 
//Выход: максимальное значение в массиве
int max3(int val[3], int * num)
{
	int tmpNum;
	num[0] = 0; num[1] = 1; num[2] = 2;
	if (val[num[0]] < val[num[1]])
	{	
		tmpNum = num[0];
		num[0] = num[1];
		num[1] = tmpNum;
	}
	if (val[num[1]] < val[num[2]])
	{	
		tmpNum = num[1];
		num[1] = num[2];
		num[2] = tmpNum;
	}
	if (val[num[0]] < val[num[1]])
	{	
		tmpNum = num[0];
		num[0] = num[1];
		num[1] = tmpNum;
	}
	return val[num[0]];
}

//Находит максимальный элемент массива, возвращает новый порядок элементов массива при сортировке по возрастанию.
//Вход:
//float val[3] - массив значений
//int * num - ссылка на массив индексов исходного массива после сортировки по возратания (int[3]) 
//Выход: максимальное значение в массиве
int max3f(float val[3], int * num)
{
	int tmpNum;
	num[0] = 0; num[1] = 1; num[2] = 2;
	if (val[num[0]] < val[num[1]])
	{	
		tmpNum = num[0];
		num[0] = num[1];
		num[1] = tmpNum;
	}
	if (val[num[1]] < val[num[2]])
	{	
		tmpNum = num[1];
		num[1] = num[2];
		num[2] = tmpNum;
	}
	if (val[num[0]] < val[num[1]])
	{	
		tmpNum = num[0];
		num[0] = num[1];
		num[1] = tmpNum;
	}
	return val[num[0]];
}

// endregion statistic

