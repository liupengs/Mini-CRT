//entry.c
#include "minicrt.h"

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

extern int main(f_size argc, char *argv[]);
void exit(f_size);

static void crt_fatal_error(const char *msg)
{
    //printf("fatal error: %s", msg);
    exit(1);
}

void mini_crt_entry(void)
{
    int ret;

#ifdef WIN32
        int flag = 0;
        int argc =0;
        char* argv[16];
        char* c1 = GetCommandLineA();

        argv[0] = c1;
        argc++;
        while(*c1){
            if(*c1 == '\"'){
                if(flag == 0) flag =1;
                else flag = 0;
            }else if(*c1 == ' ' && flag == 0){
                if(*(c1+1)){
                    argv[argc] = c1 +1;
                    argc++;
                }
                *c1 = '\0';
            }
            c1++;
        }
#else
    f_size argc;
    char **argv;

    char *ebp_reg = 0;
    #ifdef X64
    asm("movq %%rbp, %0 \n":"=r"(ebp_reg)); //__asm__¡¡__volatile__("Instruction List" : Output : Input : Clobber/Modify);
    #else // WIN32
    asm("movl %%ebp, %0 \n":"=r"(ebp_reg));
    #endif
    argc = *(f_size*)(ebp_reg+4);
    argv = (char **) (ebp_reg + 8);
#endif
    if(!mini_crt_heap_init())
        crt_fatal_error("heap initialize failed");

    if(!mini_crt_io_init())
        crt_fatal_error("IO initialize failed");
    do_global_ctors();
    ret = main(argc, argv);
    exit(ret);
}

void exit(f_size exitCode)
{
    mini_crt_call_exit_routine();
    #ifdef WIN32
    ExitProcess(exitCode);
    #elif X64
    asm("movq %0, %%rbx \n\t"
        "movq $1, %%rax \n\t"
        "int $0x80 \n\t"
        "hlt \n\t"::"m"(exitCode)
    );
    #else
    asm("movl %0, %%ebx \n\t"
        "movl $1, %%eax \n\t"
        "int $0x80 \n\t"
        "hlt \n\t"::"m"(exitCode)
    );
    #endif // WIN32
}
