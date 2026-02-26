__asm__(".section .text\n"
        ".global _start\n"
        "_start:\n"
        "    movq (%rsp), %rdi\n"
        "    lea 8(%rsp), %rsi\n"
        "    mov %rdi, %rcx\n"
        "    add $1, %rcx\n"
        "    shl $3, %rcx\n"
        "    lea (%rsi, %rcx), %rdx\n"
        "    mov %rsp, %rcx\n"
        "    and $-16, %rsp\n"
        "    call cmain\n");
__asm__(".section .text\n"
        "syscall:\n"
        "push    %rbp\n"
        "mov     %rdi, %rbp\n"
        "mov     %rsi, %rax\n"
        "mov     %rdx, %rdi\n"
        "mov     %rcx, %rsi\n"
        "mov     %r8,  %rdx\n"
        "mov     %r9,  %r10\n"
        "mov     16(%rsp), %r8\n"
        "mov     24(%rsp), %r9\n"
        "syscall\n"
        "mov     %rax, 0(%rbp)\n"
        "pop     %rbp\n"
        "ret\n");

void
syscall(long* result,
        long num,
        long r1,
        long r2,
        long r3,
        long r4,
        long r5,
        long r6);

void
cstrlen(long* ret, char* x)
{
  char* y;
  y = x;
loop:
  *x ? ({}) : ({ goto end; });
  x++;
  goto loop;
end:

  *ret = x - y;
}

void
cstrcmp(long* ret, char* x, char* y)
{
loop:
  *x != *y || *x == 0 ? ({ goto end; }) : ({});
  x++;
  y++;
  goto loop;
end:
  *x > *y ? ({ *ret = 1; }) : *x < *y ? ({ *ret = -1; }) : ({ *ret = 0; });
}

void
cmemset(void* dest, char c, long n)
{
  char* tmp;
  tmp = dest;
loop:
  n-- ? ({}) : ({ goto end; });
  *tmp++ = (char)c;
  goto loop;
end:;
}

void
exit_group(long status)
{
  syscall(&status, 231, status, 0, 0, 0, 0, 0);
}

void
write(long* ret, long fd, void* buf, long len)
{
  syscall(ret, 1, fd, (long)buf, len, 0, 0, 0);
}

void
open(long* ret, char* path, long flags, long mode)
{
  syscall(ret, 2, (long)path, flags, mode, 0, 0, 0);
}

void
close(long* ret, long fd)
{
  syscall(ret, 3, fd, 0, 0, 0, 0, 0);
}

void
lseek(long* ret, long fd, long offset, long whence)
{
  syscall(ret, 8, fd, offset, whence, 0, 0, 0);
}

void
mmap(void** ret,
     void* addr,
     long length,
     long prot,
     long flags,
     long fd,
     long offset)
{
  syscall((void*)ret, 9, (long)addr, length, prot, flags, fd, offset);
}

void
munmap(void* addr, long length)
{
  long ign;
  syscall(&ign, 11, (long)addr, length, 0, 0, 0, 0);
}

void
map(void** ret, long size)
{
  long tmp;
  mmap(ret, 0, size, 3, 34, -1, 0);
  tmp = (long)*ret;
  tmp < 0 && tmp >= -4095 ? * ret = 0 : 0;
}

void
fmap_ro(void** ret, long fd, long size, long offset)
{
  long tmp;
  mmap(ret, 0, size, 1, 1, fd, offset);
  tmp = (long)*ret;
  tmp < 0 && tmp >= -4095 ? * ret = 0 : 0;
}

void
panic(char* msg)
{
  long r;
  cstrlen(&r, msg);
  write(&r, 2, msg, r);
  write(&r, 2, "\n", 1);
  exit_group(-1);
}

void
write_str(long fd, char* msg)
{
  long r;
  cstrlen(&r, msg);
  write(&r, fd, msg, r);
}

void
write_num(long fd, long num)
{
  char* buf;
  char* p;
  long len;
  long written;
  long neg;
  long tmp;

  neg = 0;
  map((void*)&buf, 21);
  buf ? ({}) : panic("Could not allocate memory!");

  p = buf + 20;
  *p = *"\0";

  num < 0 ? (
              {
                num = -num;
                neg = 1;
              })
  : num == 0 ? (
                 { *--p = *"0"; })
             : 0;

loop:
  num > 0 ? ({}) : ({ goto end; });
  tmp = num % 10;
  *--p = *"0" + tmp;
  num = num / 10;
  goto loop;
end:
  neg ? ({ *--p = *"-"; }) : ({ 0; });
  len = (buf + 20) - p;
  write(&written, fd, p, len);
  munmap(buf, 21);
}

void
cmain(long argc, char** argv)
{
  long r;
  write_num(2, argc);
  write(&r, 2, "\nhi\n", 4);
  argc > 2 ? ({
    cstrcmp(&r, argv[1], argv[2]);
    0;
  })
           : ({ r = 0; });
  exit_group(r);
  (void)argc;
  (void)argv;
}
