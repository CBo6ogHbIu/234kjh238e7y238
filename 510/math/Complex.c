#include "Complex.h"

//Инициализирует комлесное число (0+j0)
void complex_Init(ComplexF * val)
{
	val->Imag = 0;
	val->Real = 0 ;
}

//Возвращает комплексно сопряженное число (a -j*b)
ComplexF complex_conj(ComplexF * val)
{
	ComplexF res;
	res.Real = val->Real;
	res.Imag = -val->Imag;
	return res;
}

//Возвращает модуль комплексного числа ( sqrt(Re^2 + Im^2))
float complex_abs(ComplexF * val)
{
	float res;
#ifndef MSVC
	arm_sqrt_f32(val->Real*val->Real + val->Imag*val->Imag, &res);
#else
	res = sqrtf(val->Real*val->Real + val->Imag*val->Imag);
#endif
	return res;
}

//Возвращает угол комплексного числа, рад
float complex_angle(ComplexF *val)
{
	float at;

	if ( (0== val->Real) && (0 == val->Imag) )
	{
		return 0.0f;
	}

	//Если точка лежит на оси
	if (0 == val->Real)
	{
		if (val->Imag > 0)
		{
			return MATH_Pi / 2.0f;
		}
		else
		{
			return 3.0f * MATH_Pi / 2.0f;
		}
	}
	if ( 0 == val->Imag)
	{
		if (val->Real > 0)
		{
			return 0.0f;
		}
		else
		{
			return MATH_Pi;
		}
	}

	at = atan(val->Imag/val->Real);

	//Если точка находится в квадрантах
	if (val->Real > 0)//I и IV квадранты
	{
		if (val->Imag >0)//I
		{
			return at;
		}
		else//IV
		{
			return at + 2.0f * MATH_Pi;
		}
	}
	else//II и III квадратнты
	{
		if (val->Imag > 0)//II
		{
			return at + MATH_Pi;
		}
		else//III
		{
			return at + MATH_Pi;
		}
	}
}

//Сложение двух комплексных чисел
ComplexF complex_add(ComplexF *val1, ComplexF *val2)
{
	ComplexF res;
	res.Real = val1->Real + val2->Real;
	res.Imag = val1->Imag + val2->Imag;
	return res;
}

//Сложение трех комплексных чисел
ComplexF complex_add3(ComplexF *val1, ComplexF *val2, ComplexF *val3)
{
	ComplexF res;
	res.Real = val1->Real + val2->Real + val3->Real;
	res.Imag = val1->Imag + val2->Imag + val3->Imag;
	return res;
}

//Вычитание двух комплексных чисел
//Вход:
//ComplexF *val1 - уменьшаемое
//ComplexF *val2 - вычистаемое
//Выход:
//ComplexF res - результат
ComplexF complex_sub(ComplexF *val1, ComplexF *val2)
{
	ComplexF res;
	res.Real = val1->Real + val2->Real;
	res.Imag = val1->Imag + val2->Imag;
	return res;
}

//Умножение двух комплексных чисел (z=z1*z2 =(x1*x2- у1*у2)+j(x1*y2+y1*x2 ) )
ComplexF complex_mul(ComplexF *val1, ComplexF *val2)
{
	ComplexF res;
	res.Real = val1->Real * val2->Real - val1->Imag * val2->Imag;
	res.Imag = val1->Real * val2->Imag + val1->Imag * val2->Real;
	return res;
}

//Деление двух комплексных чисел
//Вход:
//ComplexF *val1 - делимое
//ComplexF *val2 - делитель
//Выход:
//ComplexF res - результат
//errcode = 0x3101
ComplexF complex_div(ComplexF *val1, ComplexF *val2)
{
	ComplexF res;
	float tmp;
	if ((val2->Real == 0.0f) && (val2->Imag == 0.0f))//TODO:Вернуть код ошибки
	{
		res.Imag=0.0f;
		res.Real=0.0f;
		setErr_code_str(0x3101, "complex_div");//деление ни ноль
	}
	else
	{
		tmp = val2->Real*val2->Real + val2->Imag * val2->Imag;
		res.Real = (val1->Real * val2->Real + val1->Imag * val2->Imag)/tmp;
		res.Imag = (val1->Imag * val2->Real - val1->Real * val2->Imag)/tmp;
	}
	return res;
}

//Деление комплексного числа на float
//Вход:
//ComplexF *val1 - делимое
//flaot val2 - делитель
//Выход:
//ComplexF res - результат
//errcode = 0x3101
ComplexF complex_div_float(ComplexF *val1, float val2)
{
	ComplexF res = {0.0f, 0.0f};
	if (0.0f == val2)
	{
		setErr_code_str(0x3101, "complex_div_float");
		return res;
	}
	res.Real = val1->Real/val2;
	res.Imag = val1->Imag/val2;
	return res;
}
