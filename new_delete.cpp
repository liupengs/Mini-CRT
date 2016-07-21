extern "C" void* malloc(unsigned int);
extern "C" void free(void*);

void* operator new(unsigned int size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void* operator new[](unsigned int size)
{
    return malloc(size);
}

void operator delete[](void* p)
{
    free(p);
}


