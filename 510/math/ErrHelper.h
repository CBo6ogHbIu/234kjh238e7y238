#ifndef ERR_HELPER
#define ERR_HELPER


//Возвращает код ошибки
int getErr(void);

//Сбрасывает ошибку и ее stackTrase
void resetErr(void);

//Установка кода ошибки
void setErr(int errCode);

//Установка кода ошибки и запись имени вызввающей ф-и в строковую переменную stacktrace
void setErr_code_str(int errCode, char *funcName);

//Возвращает строку stacktrace текущей ошибки
char * getErr_str(void);

//Возвращает длинну строки ошибки stacktrace
int getErr_strLength(void);

//Установка  строки ошибки stacktrace
void setErr_str(char * funcName);

#endif // !ERR_HELPER

