#ifndef COMPLEX_H
#define COMPLEX_H

#include "ErrHelper.h"
#include "matem.h"

typedef struct 
{
	float Real;
	float Imag;
} ComplexF;

//Инициализирует комлесное число (0+j0)
void complex_Init(ComplexF * val);

//Возвращает комплексно сопряженное число (a -j*b)
ComplexF complex_conj(ComplexF * val);

//Возвращает модуль комплексного числа ( sqrt(Re^2 + Im^2))
float complex_abs(ComplexF * val);

//Возвращает угол комплексного числа, рад
float complex_angle(ComplexF *val);

//Сложение двух комплексных чисел
ComplexF complex_add(ComplexF *val1, ComplexF *val2);

//Сложение трех комплексных чисел
ComplexF complex_add3(ComplexF *val1, ComplexF *val2, ComplexF *val3);

//Вычитание двух комплексных чисел
//Вход:
//ComplexF *val1 - уменьшаемое
//ComplexF *val2 - вычистаемое
//Выход:
//ComplexF res - результат
ComplexF complex_sub(ComplexF *val1, ComplexF *val2);

//Умножение двух комплексных чисел (z=z1*z2 =(x1*x2- у1*у2)+j(x1*y2+y1*x2 ) )
ComplexF complex_mul(ComplexF *val1, ComplexF *val2);

//Деление двух комплексных чисел
//Вход:
//ComplexF *val1 - делимое
//ComplexF *val2 - делитель
//Выход:
//ComplexF res - результат
//errcode = 0x3101
ComplexF complex_mul(ComplexF *val1, ComplexF *val2);

//Деление комплексного числа на float
//Вход:
//ComplexF *val1 - делимое
//flaot val2 - делитель
//Выход:
//ComplexF res - результат
//errcode = 0x3101
ComplexF complex_div_float(ComplexF *val1, float val2);

#endif	//	COMPLEX_H
