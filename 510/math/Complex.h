#ifndef COMPLEX_H
#define COMPLEX_H

#include "ErrHelper.h"
#include "matem.h"

typedef struct 
{
	float Real;
	float Imag;
} ComplexF;

//�������������� ��������� ����� (0+j0)
void complex_Init(ComplexF * val);

//���������� ���������� ����������� ����� (a -j*b)
ComplexF complex_conj(ComplexF * val);

//���������� ������ ������������ ����� ( sqrt(Re^2 + Im^2))
float complex_abs(ComplexF * val);

//���������� ���� ������������ �����, ���
float complex_angle(ComplexF *val);

//�������� ���� ����������� �����
ComplexF complex_add(ComplexF *val1, ComplexF *val2);

//�������� ���� ����������� �����
ComplexF complex_add3(ComplexF *val1, ComplexF *val2, ComplexF *val3);

//��������� ���� ����������� �����
//����:
//ComplexF *val1 - �����������
//ComplexF *val2 - �����������
//�����:
//ComplexF res - ���������
ComplexF complex_sub(ComplexF *val1, ComplexF *val2);

//��������� ���� ����������� ����� (z=z1*z2 =(x1*x2- �1*�2)+j(x1*y2+y1*x2 ) )
ComplexF complex_mul(ComplexF *val1, ComplexF *val2);

//������� ���� ����������� �����
//����:
//ComplexF *val1 - �������
//ComplexF *val2 - ��������
//�����:
//ComplexF res - ���������
//errcode = 0x3101
ComplexF complex_mul(ComplexF *val1, ComplexF *val2);

//������� ������������ ����� �� float
//����:
//ComplexF *val1 - �������
//flaot val2 - ��������
//�����:
//ComplexF res - ���������
//errcode = 0x3101
ComplexF complex_div_float(ComplexF *val1, float val2);

#endif	//	COMPLEX_H
