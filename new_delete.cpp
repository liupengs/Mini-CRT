extern "C" void* malloc(unsigned int);
extern "C" void free(void*);

#ifdef X64
typedef unsigned long u_f_size;
#else
typedef unsigned u_f_size;
#endif


void* operator new(u_f_size size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete(void *p, u_f_size size )
{
    free(p);
}

void* operator new[](u_f_size size)
{
    return malloc(size);
}

void operator delete[](void* p)
{
    free(p);
}


