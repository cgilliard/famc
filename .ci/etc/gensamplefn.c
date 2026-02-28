#include <stdio.h>

#define SIZE 10000

int
main(int argc, char** argv)
{
  int i;
  FILE* fp;
  fp = fopen(argv[1], "w");

  fprintf(fp, "__asm__(\n");
  fprintf(fp, "    \".section .text\\n\"\n");
  fprintf(fp, "    \".global _start\\n\"\n");
  fprintf(fp, "    \"_start:\\n\"\n");
  fprintf(fp, "    \"    movq (%%rsp), %%rdi\\n\"\n");
  fprintf(fp, "    \"    lea 8(%%rsp), %%rsi\\n\"\n");
  fprintf(fp, "    \"    mov %%rdi, %%rcx\\n\"\n");
  fprintf(fp, "    \"    add $1, %%rcx\\n\"\n");
  fprintf(fp, "    \"    shl $3, %%rcx\\n\"\n");
  fprintf(fp, "    \"    lea (%%rsi, %%rcx), %%rdx\\n\"\n");
  fprintf(fp, "    \"    mov %%rsp, %%rcx\\n\"\n");
  fprintf(fp, "    \"    and $-16, %%rsp\\n\"\n");
  fprintf(fp, "    \"    call cmain\\n\"\n");
  fprintf(fp, ");\n\n");

  fprintf(fp, "__asm__(\n");
  fprintf(fp, "    \".section .text\\n\"\n");
  fprintf(fp, "    \"syscall:\\n\"\n");
  fprintf(fp, "    \"push    %%rbp\\n\"\n");
  fprintf(fp, "    \"mov     %%rdi, %%rbp\\n\"\n");
  fprintf(fp, "    \"mov     %%rsi, %%rax\\n\"\n");
  fprintf(fp, "    \"mov     %%rdx, %%rdi\\n\"\n");
  fprintf(fp, "    \"mov     %%rcx, %%rsi\\n\"\n");
  fprintf(fp, "    \"mov     %%r8,  %%rdx\\n\"\n");
  fprintf(fp, "    \"mov     %%r9,  %%r10\\n\"\n");
  fprintf(fp, "    \"mov     16(%%rsp), %%r8\\n\"\n");
  fprintf(fp, "    \"mov     24(%%rsp), %%r9\\n\"\n");
  fprintf(fp, "    \"syscall\\n\"\n");
  fprintf(fp, "    \"mov     %%rax, 0(%%rbp)\\n\"\n");
  fprintf(fp, "    \"pop     %%rbp\\n\"\n");
  fprintf(fp, "    \"ret\\n\"\n");
  fprintf(fp, ");\n\n");

  fprintf(fp,
          "void syscall(long *result, long num, long r1, long r2, long "
          "r3, long r4, long r5, long r6);\n\n");

  for (i = 1; i < SIZE; i++)
    fprintf(fp, "void fun%d (long *r, long v) { *r = v * %d; }\n", i, i);
  fprintf(fp,
          "\nvoid cmain(long argc, char **argv) {\n\tlong sum;\n\n\tsum = "
          "argc;\n");
  for (i = 1; i < SIZE; i++)
    fprintf(fp, "\t/*fun%d(&sum, sum); */ sum = sum + argc + %d;\n", i, i);
  fprintf(fp, "\t/*syscall(&sum, 231, sum, 0, 0, 0, 0, 0);*/\n}\n");
  fclose(fp);
  return 0;
}
