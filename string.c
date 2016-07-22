#include "minicrt.h"
char* itoa(f_size n, char* str, f_size radix)
{
    char digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* p = str;
    char* head = str;

    if(!p || radix<2 || radix>36)
        return p;
    if(radix != 10 && n<0)
        return p;
    if(n == 0){
        *p++ = '0';
        *p = 0;
        return p;
    }
    if(radix == 10 && n < 0)
    {
        *p++ = '-';
        head += 1;
        n = -n;
    }
    while(n){
        *p++ = digit[n%radix];
        n /= radix;
    }
    *p =0;

    for(--p; head<p; ++head,--p)
    {
        char temp = *head;
        *head = *p;
        *p = temp;
    }
    return str;

}

int strcmp(const char* src, const char* dst)
{
    int ret = 0;
    unsigned char* p1 = (unsigned char *)src;
    unsigned char* p2 = (unsigned char *)dst;
    while(*p2 && *p1 && !(ret = *p1 - *p2))
        ++p1,++p2;
    if(ret < 0)
        ret = -1;
    else if(ret >0)
        ret = 1;
    return (ret);
}

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;
    while(*src)
        *dest++ = *src++;
    *dest = '\0';
    return ret;
}

unsigned strlen(const char *str)
{
    int cnt = 0;
    if(!str)
        return 0;
    for(;*str != '\0'; ++str)
        ++cnt;
    return cnt;
}

int fputc(char c, FILE *stream)
{
    if(fwrite(&c, 1, 1, stream)!=1)
        return EOF;
    else
        return c;
}

int fputs(const char *str, FILE *stream)
{
    int len = strlen(str);
    if(fwrite(str, 1, len, stream) != len)
        return EOF;
    else
        return len;
}

#ifndef WIN32
typedef char* va_list;
#define va_start(ap,arg) (ap=(va_list)&arg+sizeof(arg))
#define va_arg(ap,t) (*(t*)((ap+=sizeof(t))-sizeof(t)))
#define va_end(ap) (ap=(va_list)0)
#else
#include <Windows.h>
#endif // WIN32

int buf_printf(FILE *stream, char *pbuf, int idx_size)
{
    int ret;
    pbuf[idx_size] = '\0';
    ret = fputs(pbuf, stream);
    return ret;
}

int strcpy_buf(FILE* stream, char *dest, const char *src, int lens, int *idx_size, int idx_all)
{
    int ret = 0, index, t;
    index = *idx_size;
    while(lens--){
        if(index == idx_all){
            t = buf_printf(stream, dest, index);
            if(t<0){
                return t;
            }else{
                ret += t;
            }
            index = 0;
        }
        dest[index++] = *src++;
    }
    *idx_size = index;
    return ret;
}

#define COMPARE_RESULT {if(t<0) return EOF; else ret+=t;}

int vfprintf(FILE *stream, const char *format, va_list arglist)
{
    int translating = 0;
    int ret = 0, t;
    const char* p =0;
    /**
    *这有一个问题，如果我将buf设置为局部变量，这个值不能正确地传递。所以不能打印出任何值
    **/
    static char buf[WRITE_BUF_SIZE];
    char pbuf[16];
    int now_idx = 0;
    for(p=format; *p != '\0'; ++p)
    {
        switch(*p)
        {
        case '%':
            if(!translating)
                translating = 1;
            else{
                t = strcpy_buf(stream, buf, p, 1, &now_idx, WRITE_BUF_SIZE-1);
                COMPARE_RESULT
                translating = 0;
            }
            break;
        case 'd':
            if(translating){
                translating = 0;
                itoa(va_arg(arglist, f_size), pbuf, 10);
                t=strcpy_buf(stream, buf, pbuf, strlen(pbuf), &now_idx, WRITE_BUF_SIZE-1);
            } else{
                t = strcpy_buf(stream, buf, p, 1, &now_idx, WRITE_BUF_SIZE-1);
            }
            COMPARE_RESULT
            break;
        case 's':
            if(translating)
            {
                const char *str = va_arg(arglist, const char *);
                translating = 0;
                t=strcpy_buf(stream, buf, str, strlen(str), &now_idx, WRITE_BUF_SIZE-1);
            }
            else{
                t = strcpy_buf(stream, buf, p, 1, &now_idx, WRITE_BUF_SIZE-1);
            }
            COMPARE_RESULT
            break;
        default:
            t = strcpy_buf(stream, buf, p, 1, &now_idx, WRITE_BUF_SIZE-1);
            if(translating)
                translating=0;
            COMPARE_RESULT
            break;
        }
    }
    if(now_idx!=0){
        t = buf_printf(stream, buf, now_idx);
        if(t<0)
            return EOF;
        else
            ret+=t;
    }
    return ret;
}

int printf(const char *format, ...)
{
    // rdi, rsi, rdx, rcx, r8, r9
    va_list arglist;
    #ifdef X64
    asm( "movq %%rbp,%0":"=r"(arglist) );
    arglist -= 0xa0;
    #else
    va_start(arglist, format);
    #endif // X64
    return vfprintf(stdout, format, arglist);
}

int fprintf(FILE *stream, const char *format, ...)
{
    va_list arglist;
    #ifdef X64
    asm( "movq %%rbp,%0":"=r"(arglist) );
    arglist -= 0xa0;
    #else
    va_start(arglist, format);
    #endif // X64
    return vfprintf(stream, format, arglist);
}


