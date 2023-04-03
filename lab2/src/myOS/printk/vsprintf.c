/*
 * 识别格式化字符串的核心代码写在本文件中
 * 可以从网上移植代码
 */ 
 
#include <stdarg.h>  

void intToStr(int num, char str[]);
int vsprintf(char *buf, const char *fmt, va_list argptr){
	//只实现%d的格式化字符串的处理
	//填写正确的内容 
	
	int j = 0;
	for(int i=0; fmt[i]!=0;i++)
	{
		if(fmt[i]=='%' && fmt[i+1] == 'd'){
			int input = va_arg(argptr, int);
			char string[50];
			intToStr(input, string); 
			for(int k=0; string[k]!=0;k++)
			{
				buf[j] = string[k];
				j++;
			}
			i++;
		}
		else{
			buf[j] = fmt[i];
			j++;
		}
	}
	buf[j] = 0;
	
	return j;
	
}

void intToStr(int num, char str[])
{
    int i = 0;
    int isNegative = 0;

    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0)
    {
        isNegative = 1;
        num = -num;
    }

    while (num != 0)
    {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end)
    {
        char temp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = temp;
        start++;
        end--;
    }
}
