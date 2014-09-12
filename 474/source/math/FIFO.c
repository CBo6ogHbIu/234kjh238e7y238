#include "FIFO.h"

//Заполняет fifo нулями и устанавливает его начальное положение 
//errCode 0x3301
void fifo_initialize_int(Fifo_int *fifo)
{
	int index;
	checkFifoSize(fifo->size);
	if (getErr() !=0)
	{
		setErr_str("fifo_initialize_int");
		return;
	}
	for (index = 0; index < fifo->size; index++)
	{
		fifo->buf[index] = 0;
	}
	fifo->head = 0;
}

//Заполняет fifo нулями и устанавливает его начальное положение
//errCode 0x3301
void fifo_initialize_uint(Fifo_uint *fifo)
{
	int index;
	checkFifoSize(fifo->size);
	if (getErr() !=0)
	{
		setErr_str("fifo_initialize_uint");
		return;
	}
	for (index = 0; index < fifo->size; index++)
	{
		fifo->buf[index] = 0;
	}
	fifo->head = 0;
}

//Заполняет fifo нулями и устанавливает его начальное положение
//errCode 0x3301
void fifo_initialize_float(Fifo_float *fifo)
{
	int index;
	checkFifoSize(fifo->size);
	if (getErr() !=0)
	{
		setErr_str("fifo_initialize_float");
		return;
	}
	for (index = 0; index < fifo->size; index++)
	{
		fifo->buf[index] = 0.0f;
	}
	fifo->head=0;
}

//поместить элемент в fifo
//errCode 0x3302, 0x3303
void fifo_int_push(Fifo_int * fifo, int value)
{
	checkFifoPos(fifo->size, fifo->head);
	if (getErr() !=0)
	{
		setErr_str("fifo_int_push");
		return;
	}
	fifo->buf[fifo->head] = value;
	fifo->head++;
	fifo->head = fifo->head & (fifo->size - 1);
}

//поместить элемент в fifo
//errCode 0x3302, 0x3303
void fifo_uint_push(Fifo_uint * fifo, unsigned int value)
{
	checkFifoPos(fifo->size, fifo->head);
	if (getErr() !=0)
	{
		setErr_str("fifo_uint_push");
		return;
	}
	fifo->buf[fifo->head] = value;
	fifo->head++;
	fifo->head = fifo->head & (fifo->size - 1);
}

//поместить элемент в fifo
//errCode 0x3302, 0x3303
void fifo_float_push(Fifo_float * fifo, float value)
{
	checkFifoPos(fifo->size, fifo->head);
	if (getErr() !=0)
	{
		setErr_str("fifo_float_push");
		return;
	}
	fifo->buf[fifo->head] = value;
	fifo->head++;
	fifo->head = fifo->head & (fifo->size - 1);
}

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
int fifo_int_pull(Fifo_int *fifo)
{
	checkFifoPos(fifo->size, (fifo->head+fifo->size-1) & (fifo->size-1));
	if (getErr() !=0)
	{		
		setErr_str("fifo_int_pull");
		return 0;
	}
	return fifo->buf[(fifo->head+fifo->size-1) & (fifo->size-1)];
}

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
unsigned int fifo_uint_pull(Fifo_uint *fifo)
{
	checkFifoPos(fifo->size, (fifo->head+fifo->size-1) & (fifo->size-1));
	if (getErr() !=0)
	{
		setErr_str("fifo_uint_pull");
		return 0;
	}
	return fifo->buf[(fifo->head+fifo->size-1) & (fifo->size-1)];
}

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
float fifo_float_pull(Fifo_float *fifo)
{
	//static float res;
	//int pos = (fifo->head+fifo->size-1) & (fifo->size-1);
	//checkFifoPos(fifo->size, pos);
	//if (getErr() !=0)
	//{
	//	setErr_str("fifo_float_pull");
	//	return 0.0;
	//}
	//return fifo->buf[pos];
	//res = fifo->buf[pos];
	return fifo_float_oldnumber(fifo, 0);
}

//взять последний элемент из fifo
//errCode 0x3302, 0x3303
void fifo_float_pull_(Fifo_float *fifo, float * res)
{
	//static float res;
	int pos = (fifo->head+fifo->size-1) & (fifo->size-1);
	checkFifoPos(fifo->size, pos);
	if (getErr() !=0)
	{
		setErr_str("fifo_float_pull");
		//return 0.0;
	}
	*res = fifo->buf[pos];
}

//взять элемент старше последнего на number
//errCode 0x3302, 0x3303
int fifo_int_oldnumber(Fifo_int *fifo, int num)
{
	int pos = (fifo->head-num+fifo->size-1) & (fifo->size-1);
	checkFifoPos(fifo->size, pos);
	if (getErr() !=0)
	{
		setErr_str("fifo_int_oldnumber");
		return 0;
	}
	return fifo->buf[pos];
}

//взять элемент старше последнего на number
//errCode 0x3302, 0x3303
unsigned int fifo_uint_oldnumber(Fifo_uint *fifo, int num)
{
	int pos = (fifo->head-num+fifo->size-1) & (fifo->size-1);
	checkFifoPos(fifo->size, pos);
	if (getErr() != 0)
	{
		setErr_str("fifo_uint_oldnumber");
		return 0.0;
	}
	return fifo->buf[pos];
}

//взять элемент старше последнего на number
//errCode 0x3302, 0x3303
float fifo_float_oldnumber(Fifo_float *fifo, int num)
{
	//(head - num -1)&(size-1) поскольку head может быть меньше чем num
	int pos = (fifo->head-num+fifo->size-1) & (fifo->size-1);
	//int pos = (fifo->head-num-1) & (fifo->size-1);
	checkFifoPos(fifo->size, pos);
	if (getErr() != 0)
	{
		setErr_str("fifo_float_oldnumber");
		return 0.0;
	}
	return fifo->buf[pos];
}

//Проверка величины fifo->head по отношению к fifo->size
//errCode 0x3302, 0x3303
void checkFifoPos(int size, int head)
{
	setErr(0);
	if (head > size)
	{
		setErr_code_str(0x3102, "checkFifoPos");//выход за границу массива
		return;
	}
	if (head<0)
	{
		setErr_code_str( 0x3103, "checkFifoPos");//отрицательный номер элемента массива
		return;
	}
}

//Проверка fifo->size на корректность ((size = 2^n) > 1)
//errCode = 0x3301
void checkFifoSize(int size)
{
	int tmp=size, ost =0;
	while (tmp != 0)
	{
		ost = tmp%2;
		tmp = tmp/2;
		if (ost !=0 && ost != 1)
		{
			setErr_code_str( 0x3301, "checkFifoSize");
			return;
		}
	}
}
