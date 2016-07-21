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

f_size fread(void* buffer, int size, int count, FILE *stream)
{
    f_size read = 0;
    if(!ReadFile((HANDLE)stream, buffer, size*count, &read, 0))
        return 0;
    return read;
}

f_size fwrite(const void* buffer, int size, int count, FILE *stream)
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

f_size fseek(FILE *fp, int offset, int set)
{
    return SetFilePointer((HANDLE)fp, offset, 0, set);

}
#else

static f_size open(const char *pathname, int flags, int mode)
{
    f_size fd=0;
    asm(
        "movl $5, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t":
        "=m"(fd):"m"(pathname),"m"(flags),"m"(mode)
    );
}

static f_size read( int fd, void* buffer, unsigned size)
{
    f_size ret = 0;
    asm("movl $3, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size)
        );
    return ret;
}

static f_size write( int fd, const void *buffer, unsigned size)
{
    f_size ret = 0;
    asm(
        "movl $4, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
        return ret;
}

static f_size close(int fd)
{
    f_size ret = 0;
    asm("movl $6, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        :"=m"(ret):"m"(fd)
        );
    return ret;
}

static f_size seek(int fd, int offset, int mode)
{
    f_size ret = 0;
    asm("movl $19, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "movl %2, %%ecx \n\t"
        "movl %3, %%edx \n\t"
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        :"=m"(ret):"m"(fd),"m"(offset),"m"(mode)
        );
    return ret;
}

FILE *fopen(const char *filename, const char *mode)
{
    long fd = -1;
    int flags = 0;
    int access = 00700;

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

f_size fread(void* buffer, int size, int count, FILE* stream)
{
    return read((f_size)stream, buffer, size*count);
}

f_size fwrite(const void *buffer, int size, int count, FILE* stream)
{
    return write((f_size)stream, buffer, size*count);
}

f_size fclose(FILE *fp)
{
    return close((f_size) fp);
}

f_size fseek(FILE *fp, int offset, int set)
{
    return seek((f_size) fp, offset, set);
}
#endif // WIN32

