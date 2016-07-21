#Mini-Crt
Introduction:
This Mini-CRT (C runtime library) is a modified version of the code in the book <程序员的自我修养>.

Environment
Linux x86 or Windows

In Linux:
gcc -c -fno-builtin -nostdlib -fno-stack-protector entry.c malloc.c stdio.c string.c atexit.c
g++ -c -nostdinc++ -fno-rtti -fno-exceptions -fno-builtin -nostdlib -fno-stack-protector crtbegin.cpp crtend.cpp ctors.cpp new_delete.cpp  sysdep.cpp  iostream.cpp sysdep.cpp
ar -rs minicrt.a  malloc.o stdio.o string.o ctors.o atexit.o iostream.o new_delete.o sysdep.o
g++ -c -nostdinc++ -fno-rtti -fno-exceptions -fno-builtin -nostdlib -fno-stack-protector test.cpp
ld -static -e mini_crt_entry entry.o crtbegin.o test.o minicrt.a crtend.o -o test

In Windows:
cl /c /DWIN32 /GS- entry.c malloc.c stdio.c string.c atexit.c
cl /c /DWIN32 /GS- /GR- crtbegin.cpp crtend.cpp ctors.cpp new_delete.cpp iostream.cpp
lib entry.obj malloc.obj stdio.obj string.obj ctors.obj new_delete.obj atexit.obj iostream.obj /OUT:minicrt.lib
cl /c /DWIN32 /GR- test.cpp
link test.obj minicrt.lib kernel32.lib /NODEFAULTLIB /entry:mini_crt_entry
