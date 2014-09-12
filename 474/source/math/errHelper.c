#include "ErrHelper.h"

#define StringMAXLength 256

short static errCode =0;
static char stacktrace[StringMAXLength];

//Сброс ошибки
void resetErr()
{
	errCode =0;
	stacktrace[0]='\0';//пустая строка
}

//Установка кода ошибки и запись имени вызввающей ф-и в строковую переменную stacktrace
void setErr_code_str(int errCode_, char *funcName)
{
	setErr(errCode_);
	setErr_str(funcName);
}

//Установка  строки ошибки stacktrace
void setErr_str(char * funcName)
{
	int strEnd =0, index, index1=0;
	for(strEnd = 0; strEnd < StringMAXLength; strEnd++)
	{
		if (stacktrace[strEnd] == '\0')
			break;
	}
	stacktrace[strEnd] = '/';//разделитель между именами ф-й
	for (index = strEnd+1; (index < StringMAXLength) && (funcName[index1] != '\0'); index++)//копируем имя функции в stacktrace
	{
		stacktrace[index]=funcName[index1];
		index1++;
	}
	if (stacktrace[index] != '\0' && index< 255)//проверяем наличие конца строки
		stacktrace[index+1] = '\0';
}

//Установка кода ошибки
void setErr(int errCode_)
{
	errCode = errCode_;
}

//Возвращает кода ошибки
int getErr()
{
	return errCode;
}

//Возвращает строку stacktrace текущей ошибки
char * getErr_str()
{
	return stacktrace;
}

//Возвращает длинну строки ошибки stacktrace
int getErr_strLength()
{
	return StringMAXLength;
}

