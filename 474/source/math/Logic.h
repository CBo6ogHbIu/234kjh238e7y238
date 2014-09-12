#ifndef LOGIC_H
#define LOGIC_H

#include "matem.h"
#include "ErrHelper.h"
#include "MeasAlgs.h"

//Структура для хранения информации о состоянии таймера
typedef struct _TimerData
{
	//Шаг инкрементирования таймера
	unsigned int step;
	//Счетчик времени
	unsigned int counter;
	//Уставка по выдержке времени
	unsigned int trip_set;
	//Флаг разрешения раоты таймера (задействован/выключен)
	bool enabled;
	//Выходное значение таймера
	bool out;
	//Флаг запуска таймера
	bool in_start;
	//Тип времятокойвой харктеристики таймера
	//0 - независимая; 1 - длительно инверсная; 2 - нормально инверсная; 3 - сильно иснвесная; 4 - чрезвычаянно инверсная;
	//5 - RXIDG типа; 6 - RI типа.
	char type;
	//Коэффициент времени для времятокойвой характеристики
	float koefT;
} TimerData;

typedef struct _OCPData
{
	//Сигнал срабатывания (на отключение или сигнализацию)
	bool out_trip;
	//Сигнал пуска защиты
	bool out_start;
	//Таймер (выдержка времени)
	TimerData t1;
	//Уставка срабатывания по току
	int set_I;
	//Коэффициент возврата
	float k_reset;
	//Флаг ввода защиты в работы
	bool enbled;
	//Сигнал РПО с выдержкой времени для ускорения и удвоения уставки
	bool in_rpcTime;
	//Разрешение удвоения уставки по сигналу РПВ
	bool x2setEnable;
 } OCPData;

typedef struct _OCP3tData
{
	//Сигнал срабатывания от выдержки времени 1 (на отключение или сигнализацию)
	bool trip1;
	//Сигнал срабатывания от выдержки времени 2 (на отключение или сигнализацию)
	bool trip2;
	//Сигнал срабатывания от выдержки времени 3 (на отключение или сигнализацию)
	bool trip3;
	//Сигнал пуска защиты
	bool start;
	//Таймер (выдержка времени)
	TimerData t1;
	//Таймер (выдержка времени)
	TimerData t2;
	//Таймер (выдержка времени)
	TimerData t3;
	//Уставка срабатывания по току
	int set_I;
	//Коэффициент возврата
	float k_reset;
	//Флаг ввода защиты в работы
	bool enbled;
	//Разрешение удвоения уставки по сигналу РПВ
	bool x2setEnable;
 } OCP3tData;

//Инициализация стркутуры TimerData
TimerData TimerDataInit();

//Инициализация стркутуры OCPData
OCPData OCPDataInit();

//Инициализация стркутуры OCP3tData
OCP3tData OCP3tDataInit();

//Обработка таймера с независимой выдержкой времени
//TimerData * data - структура описывающая текущее состояние таймера (изменяется внутри ф-и);
//errCode 0x4201
void ttimer(TimerData * data);

//Обработка максимальной защиты (по повышению параметра) с одной выдержкой времени
//Вход:
//int current - значение максимального параметра по фазам (тока или напряжения);
//OCPData * data - структура с информацией о состоянии защиты (изменяется внутри функции);
//errCode 0x4201, 0x3304
void ocp_work(int current, OCPData * data);

//Обработка максимальной защиты (по повышению параметра) с тремя выдержками времени
//Вход:
//int current - значение максимального параметра по фазам (тока или напряжения);
//OCP3tData * data - структура с информацией о состоянии защиты (изменяется внутри функции);
//errCode 0x4201, 0x3304
void ocp3t_work(int current, OCP3tData * data);

//Возвраящает натуральный логарифм заданный в табличном виде в диапазое x от 1.0 до 800.0
//errCode 0x3304
float myLn(float x);

//Обработка таймера возможностью выбора типа выдержки времени
//TimerData * data - структура описывающая текущее состояние таймера (изменяется внутри ф-и);
//float curDim - кратность тока к уставке (I/Iуст);
//errCode 0x4201, 0x3304
void inverseTimer(TimerData * data, float curDim);

//Расчет времени таймера с токозависимой характеристикой.
//Вычисляет время уставки таймера в выборках АЦП.
//TimerData * data - структура описывающая текущее состояние таймера (изменяется внутри ф-и);
//float dim - кратность тока к уставке (I/Iуст);
//Выход:
//bool res - был ли произведен расчет
//errCode 0x3304
bool calcTripTimerTime(TimerData * data, float dim);

//Находит x^0.02 по таблице в диапазоне от 1 до 21.5
//errCode 0x3304
float pow002(float x);

//Находит максимальный элемент массива, возвращает новый порядок элементов массива при сортировке по возрастанию.
//Вход:
//int val[3] - массив значений
//int * num - ссылка на массив индексов исходного массива после сортировки по возратания (int[3]) 
int max3(int val[3], int * num);

//Находит максимальный элемент массива, возвращает новый порядок элементов массива при сортировке по возрастанию.
//Вход:
//float val[3] - массив значений
//int * num - ссылка на массив индексов исходного массива после сортировки по возратания (int[3]) 
//Выход: максимальное значение в массиве
int max3f(float val[3], int * num);

#endif //LOGIC_H
