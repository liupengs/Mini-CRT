//stdio.h
#include "minicrt.h"

int mini_crt_io_init()
{
    return 1;
}

#ifdef WIN32
#include <Windows.h>
FILE *fopen(const char *filename, const char*mode)
{
    HANDLE hFile = 0;
    int access = 0;
    int creation =0;

    if(strcmp(mode, "w") == 0){
        access |= GENERIC_WRITE;
        creation |= CREATE_ALWAYS;
    }
    if(strcmp(mode, "w+") == 0){
        access |= GENERIC_WRITE | GENERIC_READ;
        creation |= CREATE_ALWAYS;
    }

    if(strcmp(mode, "r") == 0){
        access |= GENERIC_READ;
        creation |= OPEN_EXISTING;
    }

    if(strcmp(mode, "r+") == 0){
        access |= GENERIC_READ | GENERIC_READ;
        creation |= TRUNCATE_EXISTING;
    }

    hFile = CreateFileA(filename, access, 0, 0, creation, 0, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return 0;
    return (FILE*) hFile;
}

f_size fread(void* buffer, f_size size, f_size count, FILE *stream)
{
    f_size read = 0;
    if(!ReadFile((HANDLE)stream, buffer, size*count, &read, 0))
        return 0;
    return read;
}

f_size fwrite(const void* buffer, f_size size, f_size count, FILE *stream)
{
    f_size wirtten =0;
    if(!WriteFile((HANDLE)stream, buffer, size*count, &wirtten, 0))
        return 0;
    return wirtten;
}

f_size fclose(FILE *fp)
{
    return CloseHandle((HANDLE) fp);
}

f_size fseek(FILE *fp, f_size offset, f_size set)
{
    return SetFilePointer((HANDLE)fp, offset, 0, set);

}
#else

static f_size open(const char *pathname, f_size flags, f_size mode)
{
    f_size fd=0;
    #ifdef X64
    asm(
        "movq $5, %%rax \n\t"
        "movq %1, %%rbx \n\t"
        "movq %2, %%rcx \n\t"
        "movq %3, %%rdx \n\t"
        "int $0x80 \n\t"
        "movq %%rax, %0 \n\t":
        "=m"(fd):"m"(pathname),"m"(flags),"m"(mode)
    );
    #else
    asm(
        "movl $5, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t":
        "=m"(fd):"m"(pathname),"m"(flags),"m"(mode)
    );
    #endif // X64
}

static f_size read( f_size fd, void* buffer, u_f_size size)
{
    f_size ret = 0;
    #ifdef X64
    asm("movq $3, %%rax \n\t"
        "movq %1, %%rbx \n\t"
        "movq %2, %%rcx \n\t"
        "movq %3, %%rdx \n\t"
        "int $0x80 \n\t"
        "movq %%rax, %0 \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size)
        );
    #else

    asm("movl $3, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size)
        );
    #endif
    return ret;
}

static f_size write( f_size fd, const void *buffer, u_f_size size)
{
    f_size ret = 0;
    #ifdef X64
    asm(
        "movq $4, %%rax \n\t"
        "movq %1, %%rbx \n\t"
        "movq %2, %%rcx \n\t"
        "movq %3, %%rdx \n\t"
        "int $0x80 \n\t"
        "movq %%rax, %0 \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
    #else
    asm(
        "movl $4, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
    #endif
    return ret;

}

static f_size close(f_size fd)
{
    f_size ret = 0;
    #ifdef X64
    asm("movq $6, %%rax \n\t"
        "movq %1, %%rbx \n\t"
        "int $0x80 \n\t"
        "movq %%rax, %0 \n\t"
        :"=m"(ret):"m"(fd)
        );
    #else
    asm("movl $6, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        :"=m"(ret):"m"(fd)
        );
    #endif
    return ret;
}

static f_size seek(f_size fd, f_size offset, f_size mode)
{
    f_size ret = 0;
    #ifdef X64
    asm("movq $19, %%rax \n\t"
        "movq %1, %%rbx \n\t"
        "movq %2, %%rcx \n\t"
        "movq %3, %%rdx \n\t"
        "int $0x80 \n\t"
        "movq %%rax, %0 \n\t"
        :"=m"(ret):"m"(fd),"m"(offset),"m"(mode)
        );
    #else
    asm("movl $19, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        :"=m"(ret):"m"(fd),"m"(offset),"m"(mode)
        );
    #endif // X64
    return ret;
}

FILE *fopen(const char *filename, const char *mode)
{
    f_size fd = -1;
    f_size flags = 0;
    f_size access = 00700;

#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR   02
#define O_CREAT  0100
#define O_TRUNC  01000
#define O_APPEND 02000

    if(strcmp(mode, "w") == 0)
        flags |= O_WRONLY | O_CREAT |O_TRUNC;

    if(strcmp(mode, "w+") == 0)
        flags |= O_CREAT | O_TRUNC;

    if(strcmp(mode, "r") == 0)
        flags |= O_RDONLY;

    if(strcmp(mode, "r+") == 0)
        flags |= O_CREAT | O_RDWR;

    fd = open(filename, flags, access);
    return (FILE*) fd;
}

f_size fread(void* buffer, f_size size, f_size count, FILE* stream)
{
    return read((f_size)stream, buffer, size*count);
}

f_size fwrite(const void *buffer, f_size size, f_size count, FILE* stream)
{
    return write((f_size)stream, buffer, size*count);
}

f_size fclose(FILE *fp)
{
    return close((f_size) fp);
}

f_size fseek(FILE *fp, f_size offset, f_size set)
{
    return seek((f_size) fp, offset, set);
}
#endif // WIN32

