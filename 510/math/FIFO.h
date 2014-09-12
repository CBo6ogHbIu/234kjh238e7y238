#ifndef FIFO__H
#define FIFO__H

#include "ErrHelper.h"

//Циклический буфер с элементами типа int
//размер должен быть степенью двойки: 4,8,16,32...128
typedef struct 
{
	//Указатель на массив данных
    int *buf;
	//Размер циклического буфера. Размер должен быть степенью двойки: 4,8,16,32...128
	int size;
	//Текущее положение полседнго элемента в буфере
    unsigned int head;
} Fifo_int;

//Циклический буфер с элементами типа uint
//размер должен быть степенью двойки: 4,8,16,32...128
typedef struct 
{
	//Указатель на массив данных
    unsigned int *buf;
	//Размер циклического буфера. Размер должен быть степенью двойки: 4,8,16,32...128
	int size;
	//Текущее положение полседнго элемента в буфере
    unsigned int head;
} Fifo_uint;

//Циклический буфер с элементами типа float
//размер должен быть степенью двойки: 4,8,16,32...128
typedef struct 
{
	//Указатель на массив данных
    float *buf;
	//Размер циклического буфера. Размер должен быть степенью двойки: 4,8,16,32...128
	int size;
	//Текущее положение полседнго элемента в буфере
    unsigned int head;
} Fifo_float;

//поместить элемент в fifo
void fifo_int_push(Fifo_int * fifo, int value);

//взять последний элемент из fifo
int fifo_int_pull(Fifo_int *fifo);

//взять элемент старше последнего на number
int fifo_int_oldnumber(Fifo_int *fifo, int num);

//Заполняет fifo нулями и устанавливает его начальное положение 
//errCode 0x3301
void fifo_initialize_int(Fifo_int *fifo);

//Заполняет fifo нулями и устанавливает его начальное положение
//errCode 0x3301
void fifo_initialize_uint(Fifo_uint *fifo);

//Заполняет fifo нулями и устанавливает его начальное положение
//errCode 0x3301
void fifo_initialize_float(Fifo_float *fifo);

//поместить элемент в fifo
//errCode 0x3302, 0x3303
void fifo_int_push(Fifo_int * fifo, int value);

//поместить элемент в fifo
//errCode 0x3302, 0x3303
void fifo_uint_push(Fifo_uint * fifo, unsigned int value);

//поместить элемент в fifo
//errCode 0x3302, 0x3303
void fifo_float_push(Fifo_float * fifo, float value);

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
int fifo_int_pull(Fifo_int *fifo);

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
unsigned int fifo_uint_pull(Fifo_uint *fifo);

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
float fifo_float_pull(Fifo_float *fifo);

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
void fifo_float_pull_(Fifo_float *fifo, float * res);

//взять элемент старше последнего на number
//errCode 0x3302, 0x3303
int fifo_int_oldnumber(Fifo_int *fifo, int num);

//взять элемент старше последнего на number
//errCode 0x3302, 0x3303
unsigned int fifo_uint_oldnumber(Fifo_uint *fifo, int num);

//взять элемент старше последнего на number
//errCode 0x3302, 0x3303
float fifo_float_oldnumber(Fifo_float *fifo, int num);

//Проверка величины fifo->head по отношению к fifo->size
//errCode 0x3302, 0x3303
void checkFifoPos(int size, int head);

//Проверка fifo->size на корректность ((size = 2^n) > 1)
//errCode = 0x3301
void checkFifoSize(int size);

#endif //FIFO__H
