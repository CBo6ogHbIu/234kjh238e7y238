#ifndef ERR_HELPER
#define ERR_HELPER


//���������� ��� ������
int getErr(void);

//���������� ������ � �� stackTrase
void resetErr(void);

//��������� ���� ������
void setErr(int errCode);

//��������� ���� ������ � ������ ����� ���������� �-� � ��������� ���������� stacktrace
void setErr_code_str(int errCode, char *funcName);

//���������� ������ stacktrace ������� ������
char * getErr_str(void);

//���������� ������ ������ ������ stacktrace
int getErr_strLength(void);

//���������  ������ ������ stacktrace
void setErr_str(char * funcName);

#endif // !ERR_HELPER

