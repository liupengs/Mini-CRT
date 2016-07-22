#ifndef WIN32
typedef void (*ctor_func)(void);

#ifdef X64
typedef long f_size;
#else
typedef int f_size;
#endif

ctor_func ctors_begin[1] __attribute__ ((section(".ctors"))) =
{
    (ctor_func) -1 //由编译器将-1改成全局构造函数的个数
};

void run_hooks()
{
    const ctor_func* list = ctors_begin;
    while((f_size)*++list != -1)
        (**list)();

}
#endif
