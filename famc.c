/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 Christopher Gilliard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

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
exit_group(long status)
{
  syscall(&status, 231, status, 0, 0, 0, 0, 0);
}

void
cmain(long argc, char** argv)
{
exit_group(0);
(void)argc;
(void)argv;
}
