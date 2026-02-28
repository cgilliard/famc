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
cstrcmp(long* ret, char* x, char* y)
{
loop:
  *x != *y || *x == 0 ? ({ goto end; }) : 0;
  x++;
  y++;
  goto loop;
end:
  *ret = *x > *y ? 1 : *x < *y ? -1 : 0;
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
string_to_long(long* result, char* buf, long len)
{
  long i;
  char c;

  i = 0;
  *result = 0;

begin:
  i < len ? 0 : ({ goto end; });
  c = buf[i];
  c<*"0" || c> * "9" ? ({
    *result = -1;
    goto end;
  })
                     : 0;
  *result > 0x7FFFFFFFFFFFFFFF / 10 ? ({
    *result = -1;
    goto end;
  })
                                    : 0;
  *result = *result * 10 + (c - *"0");
  i++;
  goto begin;
end:;
}

void
cstrlen(long* ret, char* x)
{
  char* y;
  y = x;
loop:
  *x ? 0 : ({ goto end; });
  x++;
  goto loop;
end:

  *ret = x - y;
}

void
cmemset(void* dest, char c, long n)
{
  char* tmp;
  tmp = dest;
loop:
  n-- ? 0 : ({ goto end; });
  *tmp++ = (char)c;
  goto loop;
end:;
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
  char buf[21];
  char* p;
  long len;
  long written;
  long neg;
  long tmp;

  neg = 0;
  p = buf + 20;
  *p = *"\0";

  neg = num < 0;
  num = neg ? -num : num;
  num == 0 ? (*--p = *"0") : 0;

loop:
  num > 0 ? 0 : ({ goto end; });
  tmp = num % 10;
  *--p = *"0" + tmp;
  num = num / 10;
  goto loop;
end:
  neg ? (*--p = *"-") : 0;
  len = (buf + 20) - p;
  write(&written, fd, p, len);
}

void
arena_init(struct arena** a, long size)
{
  struct arena* ret;
  map((void*)&ret, size + sizeof(struct arena));
  !ret ? panic("Could not mmap sufficient memory! Halting.\n") : 0;
  ret->start = ((char*)(ret)) + sizeof(struct arena);
  ret->end = ret->start + size;
  ret->current = ret->start;
  *a = ret;
}

void
arena_alloc(void** result, struct arena* a, long size)
{
  void* ret;
  long to_alloc;
  to_alloc = (size + 7) & 0xFFFFFFFFFFFFFFF8;
  a->current > a->end - to_alloc || to_alloc > (long)a->end
    ? panic("Could not allocate memory! Halting.\n")
    : 0;

  ret = a->current;
  a->current += to_alloc;
  *result = ret;
}

void
lexer_register(struct lexer* l,
               struct arena* a,
               char* s,
               long len,
               enum node_kind kind,
               long is_reserved_word)
{
  long i;
  struct token_trie* cur;
  long r;
  long ch;
  cur = &l->root;
  i = 0;

begin:
  i < len ? 0 : ({ goto end; });
  ch = s[i] & 0xFF;
  cur->children[ch]
    ? ({
        cur = cur->children[ch];
        cur->kind ? ({
          write_str(2, "reg=");
          write(&r, 2, s, len);
          write_str(2, "\n");
          panic("Compiler internal error: longer first!");
        })
                  : 0;
      })
    : ({
        arena_alloc((void*)&cur->children[ch], a, sizeof(struct token_trie));
        cur = cur->children[ch];
        cur->kind = 0;
        cmemset(cur->children, 0, sizeof(struct token_trie*) * 256);
      });
  i == len - 1 ? ({
    cur->kind = kind;
    cur->len = len;
    cur->is_reserved_word = is_reserved_word;
  })
               : 0;
  i++;
  goto begin;
end:;
}

void
lexer_init(struct lexer* l, struct arena* a, long size, long debug)
{
  l->end = l->in + size;
  l->off = 0;
  l->col_start = 0;
  l->line = 0;
  l->debug = debug;
  cmemset(&l->root, 0, sizeof(struct token_trie));

  lexer_register(l, a, ";", 1, nk_semi, 0);
  lexer_register(l, a, "*", 1, nk_asterisk, 0);
  lexer_register(l, a, ",", 1, nk_comma, 0);
  lexer_register(l, a, ":", 1, nk_colon, 0);
  lexer_register(l, a, "?", 1, nk_questionmark, 0);
  lexer_register(l, a, "(", 1, nk_left_paren, 0);
  lexer_register(l, a, ")", 1, nk_right_paren, 0);
  lexer_register(l, a, "{", 1, nk_left_brace, 0);
  lexer_register(l, a, "}", 1, nk_right_brace, 0);
  lexer_register(l, a, "[", 1, nk_left_bracket, 0);
  lexer_register(l, a, "]", 1, nk_right_bracket, 0);
  lexer_register(l, a, ".", 1, nk_dot, 0);
  lexer_register(l, a, "++", 2, nk_double_plus, 0);
  lexer_register(l, a, "+", 1, nk_plus, 0);
  lexer_register(l, a, "!=", 2, nk_ne, 0);
  lexer_register(l, a, "!", 1, nk_bang, 0);
  lexer_register(l, a, "||", 2, nk_double_pipe, 0);
  lexer_register(l, a, "|", 1, nk_pipe, 0);
  lexer_register(l, a, "&&", 2, nk_double_ampersand, 0);
  lexer_register(l, a, "&", 1, nk_ampersand, 0);
  lexer_register(l, a, "--", 2, nk_double_minus, 0);
  lexer_register(l, a, "->", 2, nk_arrow, 0);
  lexer_register(l, a, "-", 1, nk_minus, 0);
  lexer_register(l, a, "==", 2, nk_double_equal, 0);
  lexer_register(l, a, ">=", 2, nk_gte, 0);
  lexer_register(l, a, "<=", 2, nk_lte, 0);
  lexer_register(l, a, "<", 1, nk_lt, 0);
  lexer_register(l, a, ">", 1, nk_gt, 0);
  lexer_register(l, a, "=", 1, nk_equal, 0);
  lexer_register(l, a, "/*", 2, nk_comment, 0);
  lexer_register(l, a, "/", 1, nk_div, 0);
  lexer_register(l, a, "%", 1, nk_mod, 0);
  lexer_register(l, a, "__asm__", 7, nk_asm, 1);
  lexer_register(l, a, "char", 4, nk_char, 1);
  lexer_register(l, a, "enum", 4, nk_enum, 1);
  lexer_register(l, a, "goto", 4, nk_goto, 1);
  lexer_register(l, a, "long", 4, nk_long, 1);
  lexer_register(l, a, "sizeof", 6, nk_sizeof, 1);
  lexer_register(l, a, "struct", 6, nk_struct, 1);
  lexer_register(l, a, "void", 4, nk_void, 1);
}

void
lexer_next_token(struct node* next, struct lexer* l, long peek)
{
  char* in;
  struct token_trie* node;
  long ch;
  long reserve_match;

  in = l->in + l->off;

begin1:
  in == l->end || (((*in & 0xFF) - 9) >= 5 && *in != *" ") ? ({ goto end1; })
                                                           : 0;
  l->line += *in == *"\n";
  l->col_start = *in == *"\n" ? (in - l->in) + 1 : l->col_start;
  in++;
  goto begin1;
end1:
  l->off = in - l->in;
  next->loc.off = l->off;
  next->loc.line = l->line;
  next->loc.col = l->off - l->col_start;

  in == l->end ? ({
    next->loc.len = 0;
    next->kind = nk_term;
    goto end;
  })
               : 0;

  *in == *"\"" ? ({
    begin2:
      ++in != l->end ? 0 : ({ goto end2; });
      *in == *"\""   ? ({
        *(in - 1) != *"\\" ? ({ goto end2; }) : 0;
        *(in - 2) == *"\\" ? ({ goto end2; }) : 0;
      })
      : *in == *"\n" ? ({
          l->col_start = (in - l->in) + 1;
          l->line++;
        })
                     : 0;

      goto begin2;
    end2:
      in != l->end ? ({
        in++;
        next->loc.len = in - (l->in + l->off);
        next->kind = nk_str_lit;
      })
                   : ({
                       next->loc.len = in - (l->in + l->off);
                       next->kind = nk_error;
                     });
      goto end;
  })
               : 0;

  node = &l->root;
  next->loc.len = 1;
  next->kind = nk_error;
  reserve_match = 0;

begin3:
  in == l->end ? ({ goto end3; }) : 0;
  ch = *in++ & 0xFF;
  (node = node->children[ch])
    ? ({
        node->len ? ({
          next->loc.len = node->len;
          next->kind = node->kind;
          reserve_match = node->is_reserved_word;
        })
                  : 0;
      })
    : ({
        next->kind ? ({
          long ch1;
          long ch2;
          long ch3;
          ch1 = (ch - *"a") & 0xFF;
          ch2 = (ch - *"A") & 0xFF;
          ch3 = (ch - *"0") & 0xFF;

          ch1 < 26 || ch2 < 26 || ch3 < 10 || ch == *"_"
            ? (
                {
                  reserve_match ? ({
                    next->kind = nk_error;
                    next->loc.len = 1;
                  })
                                : 0;
                })
            : 0;
        })
                   : 0;
        goto end3;
      });
  goto begin3;
end3:

  next->kind == nk_error ? ({
    long ch1;
    long ch2;
    long ch3;

    in = l->in + l->off;
    ch1 = (*in - *"a") & 0xFF;
    ch2 = (*in - *"A") & 0xFF;

    ch1 < 26 || ch2 < 26 || *in == *"_"
      ? (
          {
            begin4:
              ++in == l->end ? ({ goto end4; }) : 0;
              ch1 = (*in - *"a") & 0xFF;
              ch2 = (*in - *"A") & 0xFF;
              ch3 = (*in - *"0") & 0xFF;
              !(ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
                ? ({ goto end4; })
                : 0;
              goto begin4;
            end4:
              next->loc.len = in - (l->in + l->off);
              next->kind = nk_ident;
          })
      : (
          {
            ch3 = (*in - *"0") & 0xFF;
            ch3 < 10
              ? (
                  {
                    long is_hex;
                    long is_bin;
                    is_hex = 0;
                    is_bin = 0;
                    in + 1 != l->end && *(in + 1) == *"x" && *in == *"0"
                      ? ({
                          is_hex = 1;
                          in++;
                        })
                      : ({
                          in + 1 != l->end && *(in + 1) == *"b" && *in == *"0"
                            ? ({
                                is_bin = 1;
                                in++;
                              })
                            : 0;
                        });
                  begin5:
                    ++in == l->end ? ({ goto end5; }) : 0;
                    ch1 = (*in - *"0") & 0xFF;
                    is_bin&& ch1 > 1 ? ({ goto end5; }) : 0;
                    ch1 >= 10 ? ({
                      is_hex ? ({
                        ch1 = (*in - *"a") & 0xFF;
                        ch2 = (*in - *"A") & 0xFF;
                        ch1 > 5 && ch2 > 5 ? ({ goto end5; }) : 0;
                      })
                             : ({ goto end5; });
                    })
                              : 0;
                    goto begin5;
                  end5:
                    next->loc.len = (long)(in - (l->in + l->off));
                    next->kind = nk_num_lit;
                  })
              : 0;
          });
    goto end;
  })
                         : 0;

  next->kind == nk_comment ? ({
    begin6:
      ++in != l->end ? 0 : ({ goto end6; });
      *in == *"/" && *(in - 1) == *"*" ? ({ goto end6; }) : 0;
      *in == *"\n" ? ({
        l->col_start = (in - l->in) + 1;
        l->line++;
      })
                   : 0;
      goto begin6;
    end6:
      in == l->end ? ({
        next->loc.len = in - (l->in + l->off);
        next->kind = nk_error;
      })
                   : ({
                       next->loc.len = in - (l->in + l->off) + 1;
                       next->kind = nk_comment;
                     });
  })
                           : 0;
end:
  !peek ? l->off += next->loc.len : 0;
  l->debug ? ({
    long r;
    write_str(1, "token=");
    write_num(1, next->kind);
    write_str(1, ",offset=");
    write_num(1, next->loc.off);
    write_str(1, ",value='");
    write(&r, 1, l->in + next->loc.off, next->loc.len);
    write_str(1, "',line=");
    write_num(1, next->loc.line + 1);
    write_str(1, ",col=");
    write_num(1, next->loc.col + 1);
    write_str(1, "\n");
  })
           : 0;
}

void
alloc_slice(struct slice** s, struct arena* a, void* ptr, long len)
{
  arena_alloc((void*)s, a, sizeof(struct slice));
  (*s)->ptr = ptr;
  (*s)->len = len;
}

void
copy_location(struct source_location* dst, struct source_location* src)
{
  dst->off = src->off;
  dst->len = src->len;
  dst->line = src->line;
  dst->col = src->col;
}

void
node_init(struct parser* p, struct node** n, enum node_kind kind)
{
  arena_alloc((void*)n, p->a, sizeof(struct node));
  cmemset(*n, 0, sizeof(struct node));
  (*n)->kind = kind;
}

void
node_copy(struct parser* p, struct node** dst, struct node* src)
{
  node_init(p, dst, src->kind);
  copy_location(&(*dst)->loc, &src->loc);
}

void
node_append(struct node* parent, struct node* child, long prepend)
{
  child->parent = parent;
  parent->last_child ? ({
    prepend ? ({ parent->first_child->prev_sibling = child; })
            : ({ parent->last_child->next_sibling = child; });
  })
                     : ({
                         prepend ? ({ parent->last_child = child; })
                                 : ({ parent->first_child = child; });
                       });
  prepend ? ({
    child->next_sibling = parent->first_child;
    parent->first_child = child;
  })
          : ({
              child->prev_sibling = parent->last_child;
              parent->last_child = child;
            });
}

void
node_display(char** result, enum node_kind kind)
{
  *result = kind == nk_asm             ? "assembly"
            : kind == nk_str_lit       ? "string literal"
            : kind == nk_enum          ? "enum"
            : kind == nk_program       ? "program"
            : kind == nk_ident         ? "ident"
            : kind == nk_struct        ? "struct"
            : kind == nk_type          ? "type"
            : kind == nk_func_decl     ? "function"
            : kind == nk_compound_stmt ? "compound statement"
                                       : "unknown";
}

void
node_print_impl(struct parser* p, struct node* n, long depth)
{
  long i;
  long r;
  struct node* child;
  char* display;

  child = n->first_child;
  i = 0;

begin_depth:
  i++ < depth ? 0 : ({ goto end_depth; });
  write_str(1, "   ");
  goto begin_depth;
end_depth:
  write_str(1, "kind=");
  write_num(1, n->kind);
  node_display(&display, n->kind);
  write_str(1, " [");
  write_str(1, display);
  write_str(1, "]");
  n->loc.len ? ({
    write_str(1, " (");
    write(&r, 1, p->in + n->loc.off, n->loc.len);
    write_str(1, ") ");
  })
             : 0;
  n->kind == nk_type ? ({
    struct type_data* td = n->node_data;
    write_str(1, " [");
    td->kind == tk_long     ? write_str(1, "long ")
    : td->kind == tk_char   ? write_str(1, "char ")
    : td->kind == tk_void   ? write_str(1, "void ")
    : td->kind == tk_struct ? write_str(1, "struct ")
    : td->kind == tk_enum   ? write_str(1, "enum ")
                            : write_str(1, "unknown");
    td->type_name.len ? ({
      write(&r, 1, td->type_name.ptr, td->type_name.len);
      write_str(1, " ");
    })
                      : 0;

    i = 0;
  begin_levels:
    i++ >= td->levels ? ({ goto end_levels; }) : 0;
    write_str(1, "*");
    goto begin_levels;
  end_levels:

    write(&r, 1, td->var_name.ptr, td->var_name.len);
    write_str(1, "]");
  })
                     : 0;
  write_str(1, "\n");

begin_child:
  child ? 0 : ({ goto end_child; });
  node_print_impl(p, child, depth + 1);
  child = child->next_sibling;
  goto begin_child;
end_child:;
}

void
node_print(struct parser* p, struct node* n)
{
  node_print_impl(p, n, 0);
}

void
print_error(struct node* n, char* msg)
{
  write_str(2, "Error: ");
  n ? ({
    write_num(2, 1 + n->loc.line);
    write_str(2, ":");
    write_num(2, 1 + n->loc.col);
    write_str(2, ": ");
  })
    : 0;
  write_str(2, msg);
  write_str(2, "\n");
  exit_group(-1);
}

void
parse_type(struct node** result, struct parser* p, struct lexer* l)
{
  struct node* ret;
  struct type_data* td;
  struct node token;

  ret = 0;

  lexer_next_token(&token, l, 0);
  token.kind == nk_right_brace || token.kind == nk_right_paren ? ({ goto end; })
                                                               : 0;

  node_init(p, &ret, nk_type);
  arena_alloc((void*)&td, p->a, sizeof(struct type_data));
  td->levels = 0;
  td->type_name.ptr = 0;
  td->type_name.len = 0;
  ret->node_data = td;

  token.kind == nk_long     ? td->kind = tk_long
  : token.kind == nk_char   ? td->kind = tk_char
  : token.kind == nk_enum   ? td->kind = tk_enum
  : token.kind == nk_struct ? td->kind = tk_struct
  : token.kind == nk_void
    ? td->kind = tk_void
    : print_error(&token, "expected one of (long, char, void, enum, struct)");

  token.kind == nk_enum || token.kind == nk_struct ? ({
    lexer_next_token(&token, l, 0);
    token.kind != nk_ident ? print_error(&token, "expected identifier") : 0;
    td->type_name.ptr = p->in + token.loc.off;
    td->type_name.len = token.loc.len;
  })
                                                   : 0;

begin_loop:
  lexer_next_token(&token, l, 0);
  token.kind != nk_asterisk ? ({ goto end_loop; }) : td->levels++;
  goto begin_loop;
end_loop:

  token.kind != nk_ident ? print_error(&token, "expected identifier") : 0;

  td->var_name.ptr = p->in + token.loc.off;
  td->var_name.len = token.loc.len;

  lexer_next_token(&token, l, 1);
  token.kind == nk_left_bracket ? ({
    lexer_next_token(&token, l, 0);
    lexer_next_token(&token, l, 0);
    token.kind != nk_num_lit ? print_error(&token, "expected numeric value")
                             : 0;
    string_to_long(&td->array_elems, p->in + token.loc.off, token.loc.len);
    lexer_next_token(&token, l, 0);
    token.kind != nk_right_bracket ? print_error(&token, "expected ']'") : 0;
  })
                                : ({ td->array_elems = 0; });

end:
  *result = ret;
}

void
parse_struct(struct parser* p, struct lexer* l)
{
  struct node* struct_node;
  struct node token;
  struct node* result;

  arena_alloc((void*)&struct_node, p->a, sizeof(struct node));
  cmemset(struct_node, 0, sizeof(struct node));
  lexer_next_token(struct_node, l, 0);
  struct_node->kind != nk_ident
    ? print_error(struct_node, "expected identifier")
    : 0;
  struct_node->kind = nk_struct;
  lexer_next_token(&token, l, 0);
  token.kind != nk_left_brace ? print_error(&token, "expected '{'") : 0;
begin:
  parse_type(&result, p, l);
  result == 0 ? ({ goto end; }) : 0;
  node_append(struct_node, result, 0);
  lexer_next_token(&token, l, 0);
  token.kind != nk_semi ? print_error(&token, "expected '}' or ';'") : 0;
  goto begin;
end:
  lexer_next_token(&token, l, 0);
  token.kind != nk_semi ? print_error(&token, "expected ';'") : 0;
  node_append(p->current, struct_node, 0);
}

void
parse_enum(struct parser* p, struct lexer* l)
{
  struct node* enum_node;
  struct node* variant;
  struct node token;

  arena_alloc((void*)&enum_node, p->a, sizeof(struct node));
  cmemset(enum_node, 0, sizeof(struct node));
  lexer_next_token(enum_node, l, 0);
  enum_node->kind != nk_ident ? print_error(enum_node, "expected identifier")
                              : 0;
  enum_node->kind = nk_enum;
  lexer_next_token(&token, l, 0);
  token.kind != nk_left_brace ? print_error(&token, "expected '{'") : 0;

begin:
  arena_alloc((void*)&variant, p->a, sizeof(struct node));
  cmemset(variant, 0, sizeof(struct node));
  lexer_next_token(variant, l, 0);
  variant->kind != nk_ident ? print_error(variant, "expected identifier") : 0;
  node_append(enum_node, variant, 0);
  lexer_next_token(&token, l, 0);
  token.kind == nk_right_brace ? ({ goto end; }) : 0;
  token.kind != nk_comma ? print_error(&token, "expected ',' or '}'") : 0;
  goto begin;
end:

  lexer_next_token(&token, l, 0);
  token.kind != nk_semi ? print_error(&token, "expected ';'") : 0;

  node_append(p->current, enum_node, 0);
}

void
parse_asm(struct parser* p, struct lexer* l)
{
  struct node* asm_node;
  struct node* str;
  struct node token;

  node_init(p, &asm_node, nk_asm);
  lexer_next_token(&token, l, 0);
  token.kind != nk_left_paren ? print_error(&token, "expected '('") : 0;

begin:
  lexer_next_token(&token, l, 0);
  token.kind == nk_right_paren ? ({ goto end; }) : 0;
  token.kind != nk_str_lit ? print_error(&token, "expected a string literal")
                           : 0;
  node_copy(p, &str, &token);
  node_append(asm_node, str, 0);
  goto begin;
end:

  lexer_next_token(&token, l, 0);
  token.kind != nk_semi ? print_error(&token, "expected ';'") : 0;
  node_append(p->current, asm_node, 0);
}

void
parse_compound_stmt(struct node** result, struct parser* p, struct lexer* l)
{
  struct node token;
  long level;

  lexer_next_token(&token, l, 0);
  token.kind != nk_left_brace ? print_error(&token, "unexpected token") : 0;

  level = 1;
begin:
  lexer_next_token(&token, l, 0);
  token.kind == nk_left_brace    ? level++
  : token.kind == nk_right_brace ? ({ --level ? 0 : ({ goto end; }); })
                                 : 0;
  goto begin;
end:
  node_init(p, result, nk_compound_stmt);
}

void
parse_void(struct parser* p, struct lexer* l)
{
  struct node* result;
  struct node* func_decl;
  struct node* stmts;
  struct node token;

  arena_alloc((void*)&func_decl, p->a, sizeof(struct node));
  cmemset(func_decl, 0, sizeof(struct node));
  lexer_next_token(func_decl, l, 0);
  func_decl->kind != nk_ident ? print_error(func_decl, "expected identifier")
                              : 0;
  func_decl->kind = nk_func_decl;
  lexer_next_token(&token, l, 0);
  token.kind != nk_left_paren ? print_error(&token, "expected '('") : 0;

begin:
  parse_type(&result, p, l);
  result == 0 ? ({ goto end; }) : 0;
  node_append(func_decl, result, 0);
  lexer_next_token(&token, l, 0);
  token.kind == nk_right_paren ? ({ goto end; })
  : token.kind != nk_comma     ? print_error(&token, "expected ')' or ','")
                               : 0;
  goto begin;
end:

  lexer_next_token(&token, l, 1);
  token.kind != nk_semi ? ({
    parse_compound_stmt(&stmts, p, l);
    node_append(func_decl, stmts, 0);
  })
                        : lexer_next_token(&token, l, 0);
  node_append(p->current, func_decl, 0);
}

void
parse(struct parser* p, struct lexer* l)
{
  struct node token;
begin:
  lexer_next_token(&token, l, 0);
  token.kind == nk_error     ? print_error(&token, "unrecognized token")
  : token.kind == nk_term    ? ({ goto end; })
  : token.kind == nk_comment ? ({ goto begin; })
  : token.kind == nk_enum    ? parse_enum(p, l)
  : token.kind == nk_asm     ? parse_asm(p, l)
  : token.kind == nk_struct  ? parse_struct(p, l)
  : token.kind == nk_void    ? parse_void(p, l)
                             : print_error(&token, "unexpected token");
  goto begin;
end:;
}

void
cmain(long argc, char** argv)
{
  long fd;
  long size;
  long debug;
  long r;
  struct lexer l;
  struct parser p;

  debug = 0;
  argc < 2 ? panic("Usage: famc <file>") : 0;

  argc > 2 ? ({
    cstrcmp(&r, argv[1], "--debug");
    r == 0 ? debug = 1 : 0;
  })
           : 0;
  open(&fd, argv[argc - 1], 0, 0);
  fd < 0 ? panic("Could not open file!") : 0;
  lseek(&size, fd, 0, 2);
  size < 0 ? panic("Could not determine "
                   "file size!")
           : 0;
  fmap_ro((void*)&(&l)->in, fd, size, 0);
  l.in == 0 ? panic("Could not mmap file!") : 0;
  close(&r, fd);
  arena_init(&p.a, 256 * 1024 * 1024);
  node_init(&p, &p.root, nk_program);
  p.in = l.in;
  p.current = p.root;
  lexer_init(&l, p.a, size, debug);
  parse(&p, &l);
  debug ? node_print(&p, p.root) : 0;
  debug ? write_str(1, "success!\n") : 0;
  exit_group(0);
}

