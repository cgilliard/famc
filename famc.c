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

struct arena
{
  char* start;
  char* current;
  char* end;
};

struct slice
{
  void* ptr;
  long len;
};

enum type_kind
{
  tk_struct,
  tk_enum,
  tk_long,
  tk_char,
  tk_void
};

enum expression_kind
{
  ek_add,
  ek_mul,
  ek_assign,
  ek_deref,
  ek_address,
  ek_func_call
};

enum node_kind
{
  nk_error,
  nk_comment,
  nk_semi,
  nk_comma,
  nk_colon,
  nk_dot,
  nk_asterisk,
  nk_questionmark,
  nk_ampersand,
  nk_equal,
  nk_plus,
  nk_minus,
  nk_bang,
  nk_pipe,
  nk_mod,
  nk_div,
  nk_left_paren,
  nk_right_paren,
  nk_left_brace,
  nk_right_brace,
  nk_left_bracket,
  nk_right_bracket,
  nk_arrow,
  nk_double_pipe,
  nk_double_ampersand,
  nk_double_equal,
  nk_double_plus,
  nk_double_minus,
  nk_ne,
  nk_gte,
  nk_gt,
  nk_lte,
  nk_lt,
  nk_asm,
  nk_char,
  nk_enum,
  nk_goto,
  nk_long,
  nk_sizeof,
  nk_struct,
  nk_void,
  nk_ident,
  nk_str_lit,
  nk_num_lit,
  nk_program,
  nk_type,
  nk_func_decl,
  nk_compound_stmt,
  nk_label,
  nk_expr,
  nk_term
};

struct type_data
{
  enum type_kind kind;
  struct slice type_name;
  struct slice var_name;
  long levels;
  long array_elems;
};

struct expression_data
{
  enum expression_kind kind;
};

struct token_trie
{
  enum node_kind kind;
  long len;
  long is_reserved_word;
  struct token_trie* children[256];
};

struct lexer
{
  char* in;
  char* end;
  long off;
  long line;
  long col_start;
  struct token_trie root;
  long debug;
};

struct source_location
{
  long off;
  long len;
  long line;
  long col;
};

struct node
{
  enum node_kind kind;
  struct source_location loc;
  struct node* parent;
  struct node* first_child;
  struct node* last_child;
  struct node* next_sibling;
  struct node* prev_sibling;
  void* node_data;
};

struct parser
{
  char* in;
  struct arena* a;
  struct node* root;
  struct node* current;
};

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
write(long* ret, long fd, void* buf, long len)
{
  syscall(ret, 1, fd, (long)buf, len, 0, 0, 0);
}

void
cmain(long argc, char** argv)
{
  long r;
  write(&r, 2, "ok\n", 3);
  exit_group(0);
  (void)argc;
  (void)argv;
}
