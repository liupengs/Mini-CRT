#ifndef __MINI_CRT_H__
#define __MINI_CRT_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#ifdef X64
typedef long f_size;
typedef long FILE;
#else
typedef int f_size;
typedef int FILE;
#endif

//malloc
#ifndef NULL
#define NULL (0)
#endif // NULL

void free(void *ptr);
void* malloc(unsigned size);
#ifndef WIN32
static void* brk(void* end_data_segment);
#endif // WIN32
int mini_crt_heap_init();



//string
char* itoa(int n, char *str, int radix);
int strcmp(const char* src, const char *dst);
char *strcpy(char *dest, const char *src);
unsigned strlen(const char *str);

//io




#define EOF (-1)

#ifdef WIN32
#define stdin ((FILE*) (GetStdHandle(STD_INPUT_HANDLE)))
#define stdout ((FILE*) (GetStdHandle(STD_OUTPUT_HANDLE)))
#define stderr ((FILE*) (GetStdHandle(STD_ERROR_HANDLE)))
#else
#define stdin ((FILE*) 0)
#define stdout ((FILE*) 1)
#define stderr ((FILE*)2)
#endif // WIN32



int mini_crt_io_init();
FILE *fopen(const char *filename, const char*mode);
f_size fread(void* buffer, int size, int count, FILE *stream);
f_size fwrite(const void* buffer, int size, int count, FILE *stream);
f_size fclose(FILE *fp);
f_size fseek(FILE *fp, int offset, int set);

int fputc(int c, FILE *stream);
int fputs(const char *str, FILE *stream);
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);

//internal
void do_global_ctors();
void min_crt_call_exit_routine();

//atexit
typedef void(*atexit_func_t)(void);
int atexit(atexit_func_t func);

#ifdef __cplusplus
}
#endif

#endif // __MINI_CRT_H__
