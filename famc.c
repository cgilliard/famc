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

struct slice
{
  void* ptr;
  long len;
};

enum type_kind
{
  type_kind_struct,
  type_kind_enum,
  type_kind_long,
  type_kind_char,
  type_kind_void
};

struct type_data
{
  enum type_kind kind;
  struct slice type_name;
  struct slice var_name;
  long levels;
};

struct arena
{
  char* start;
  char* current;
  char* end;
  long align;
};

struct lexer
{
  char* in;
  char* end;
  long off;
  long line;
  long col_start;
};

enum unary_op
{
  unary_op_deref,
  unary_op_ref,
  unary_op_negate,
  unary_op_logical_not,
  unary_op_bitwise_not,
  unary_op_pre_inc,
  unary_op_post_inc,
  unary_op_pre_dec,
  unary_op_post_dec,
  unary_op_sizeof
};

enum binary_op
{
  binary_op_add,
  binary_op_sub,
  binary_op_mul,
  binary_op_div,
  binary_op_mod,
  binary_op_bit_and,
  binary_op_bit_or,
  binary_op_eq,
  binary_op_ne,
  binary_op_lte,
  binary_op_lt,
  binary_op_gte,
  binary_op_gt,
  binary_op_logical_and,
  binary_op_logical_or,
  binary_op_add_assign,
  binary_op_sub_assign
};

enum node_kind
{
  nk_error,
  nk_semi,
  nk_comma,
  nk_colon,
  nk_asterisk,
  nk_questionmark,
  nk_ampersand,
  nk_pipe,
  nk_dot,
  nk_left_paren,
  nk_right_paren,
  nk_left_brace,
  nk_right_brace,
  nk_left_bracket,
  nk_right_bracket,
  nk_hyphen,
  nk_plus,
  nk_equal,
  nk_bang,
  nk_mod,
  nk_tilde,
  nk_div,
  nk_gt,
  nk_lt,
  nk_gte,
  nk_lte,
  nk_ne,
  nk_arrow,
  nk_double_plus,
  nk_plus_equal,
  nk_double_hyphen,
  nk_hyphen_equal,
  nk_double_equal,
  nk_double_pipe,
  nk_double_ampersand,
  nk_asm,
  nk_char,
  nk_else,
  nk_enum,
  nk_goto,
  nk_if,
  nk_long,
  nk_sizeof,
  nk_struct,
  nk_void,
  nk_num_lit,
  nk_str_lit,
  nk_comment,
  nk_ident,
  nk_program,
  nk_var,
  nk_type,
  nk_function,
  nk_compound_stmt,
  nk_expr,
  nk_binary_op,
  nk_unary_op,
  nk_func_call,
  nk_cast,
  nk_subscript,
  nk_member_access,
  nk_term
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
  long sp;
  struct node* stack;
  long capacity;
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
  *x == *y && *x ? ({}) : ({ goto end; });
  x++;
  y++;
  goto loop;
end:
  *x > *y ? ({ *ret = 1; }) : 0;
  *x < *y ? ({ *ret = -1; }) : 0;
  *x == *y ? ({ *ret = 0; }) : 0;
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
arena_init(struct arena** a, long size, long align)
{
  struct arena* ret;
  map((void*)&ret, size + sizeof(struct arena));
  ret ? ({}) : panic("Could not mmap sufficient memory! Halting.\n");
  ret->start = ((char*)(ret)) + sizeof(struct arena);
  ret->end = ret->start + size;
  (long)ret->start % align ? ({
    ret->current = (char*)((long)(ret->start + (align - 1)) & ~(align - 1));
  })
                           : ({ ret->current = ret->start; });
  ret->align = align;
  *a = ret;
}

void
arena_alloc(void** result, struct arena* a, long size)
{
  void* ret;
  long to_alloc;
  to_alloc = (size + (a->align - 1)) & ~(a->align - 1);
  a->current > a->end - to_alloc || to_alloc > (long)a->end
    ? panic("Could not allocate memory! Halting.\n")
    : ({});

  ret = a->current;
  a->current += to_alloc;
  *result = ret;
}

void
lexer_next_token(struct node* next, struct lexer* l)
{
  char* in;
  in = l->in + l->off;

begin1:
  in == l->end ? ({ goto end1; }) : ({});
  (*in - 9) >= 5 && *in != *" " ? ({ goto end1; }) : ({});
  (*in == *"\n") ? ({
    l->col_start = (in - l->in) + 1;
    l->line++;
  })
                 : 0;
  in++;
  goto begin1;
end1:
  l->off = in - l->in;
  next->loc.off = l->off;
  next->loc.line = l->line;
  next->loc.col = l->off - l->col_start;
  in == l->end  ? ({
    next->loc.len = 0;
    next->kind = nk_term;
    goto end;
  })
  : *in == *";" ? ({
      next->loc.len = 1;
      next->kind = nk_semi;
      l->off++;
      goto end;
    })
  : *in == *"," ? ({
      next->loc.len = 1;
      next->kind = nk_comma;
      l->off++;
      goto end;
    })
  : *in == *":" ? ({
      next->loc.len = 1;
      next->kind = nk_colon;
      l->off++;
      goto end;
    })
  : *in == *"*" ? ({
      next->loc.len = 1;
      next->kind = nk_asterisk;
      l->off++;
      goto end;
    })
  : *in == *"?" ? ({
      next->loc.len = 1;
      next->kind = nk_questionmark;
      l->off++;
      goto end;
    })
  : *in == *"%" ? ({
      next->loc.len = 1;
      next->kind = nk_mod;
      l->off++;
      goto end;
    })
  : *in == *"~" ? ({
      next->loc.len = 1;
      next->kind = nk_tilde;
      l->off++;
      goto end;
    })
  : *in == *"|" ? ({
      ++in != l->end && *in == *"|" ? ({
        next->loc.len = 2;
        next->kind = nk_double_pipe;
        l->off += 2;
      })
                                    : ({
                                        next->loc.len = 1;
                                        next->kind = nk_pipe;
                                        l->off++;
                                      });
      goto end;
    })
  : *in == *"." ? ({
      next->loc.len = 1;
      next->kind = nk_dot;
      l->off++;
      goto end;
    })
  : *in == *"&" ? ({
      ++in != l->end && *in == *"&" ? ({
        next->loc.len = 2;
        next->kind = nk_double_ampersand;
        l->off += 2;
      })
                                    : ({
                                        next->loc.len = 1;
                                        next->kind = nk_ampersand;
                                        l->off++;
                                      });
      goto end;
    })

  : *in == *"=" ? ({
      ++in != l->end && *in == *"=" ? ({
        next->loc.len = 2;
        next->kind = nk_double_equal;
        l->off += 2;
      })
                                    : ({
                                        next->loc.len = 1;
                                        next->kind = nk_equal;
                                        l->off++;
                                      });
      goto end;
    })
  : *in == *"(" ? ({
      next->loc.len = 1;
      next->kind = nk_left_paren;
      l->off++;
      goto end;
    })
  : *in == *")" ? ({
      next->loc.len = 1;
      next->kind = nk_right_paren;
      l->off++;
      goto end;
    })
  : *in == *"{" ? ({
      next->loc.len = 1;
      next->kind = nk_left_brace;
      l->off++;
      goto end;
    })
  : *in == *"}" ? ({
      next->loc.len = 1;
      next->kind = nk_right_brace;
      l->off++;
      goto end;
    })
  : *in == *"[" ? ({
      next->loc.len = 1;
      next->kind = nk_left_bracket;
      l->off++;
      goto end;
    })
  : *in == *"]" ? ({
      next->loc.len = 1;
      next->kind = nk_right_bracket;
      l->off++;
      goto end;
    })
  : *in == *"-" ? ({
      (++in != l->end) ? ({
        *in == *"="   ? ({
          next->loc.len = 2;
          next->kind = nk_hyphen_equal;
          l->off += 2;
          goto end;
        })
        : *in == *"-" ? ({
            next->loc.len = 2;
            next->kind = nk_double_hyphen;
            l->off += 2;
            goto end;
          })
        : *in == *">" ? ({
            next->loc.len = 2;
            next->kind = nk_arrow;
            l->off += 2;
            goto end;
          })
                      : ({});
      })
                       : ({});
      next->loc.len = 1;
      next->kind = nk_hyphen;
      l->off++;
      goto end;
    })

  : *in == *"+" ? ({
      ++in != l->end ? ({
        *in == *"=" ? ({
          next->loc.len = 2;
          next->kind = nk_plus_equal;
          l->off += 2;
          goto end;
        })
                    : ({
                        *in == *"+" ? ({
                          next->loc.len = 2;
                          next->kind = nk_double_plus;
                          l->off += 2;
                          goto end;
                        })
                                    : ({});
                      });
      })
                     : ({});
      next->loc.len = 1;
      next->kind = nk_plus;
      l->off++;
      goto end;
    })
                : ({});

  if (*in == *"<") {
    ++in != l->end && *in == *"=" ? ({
      next->loc.len = 2;
      next->kind = nk_lte;
      l->off += 2;
    })
                                  : ({
                                      next->loc.len = 1;
                                      next->kind = nk_lt;
                                      l->off++;
                                    });
  } else if (*in == *">") {
    ++in != l->end && *in == *"=" ? ({
      next->loc.len = 2;
      next->kind = nk_gte;
      l->off += 2;
    })
                                  : ({
                                      next->loc.len = 1;
                                      next->kind = nk_gt;
                                      l->off++;
                                    });
  } else if (*in == *"!") {
    ++in != l->end && *in == *"=" ? ({
      next->loc.len = 2;
      next->kind = nk_ne;
      l->off += 2;
    })
                                  : ({
                                      next->loc.len = 1;
                                      next->kind = nk_bang;
                                      l->off++;
                                    });
  } else if (*in == *"\"") {
  begin2:
    ++in != l->end ? ({}) : ({ goto end2; });
    if (*in == *"\"") {
      if (*(in - 1) != *"\\")
        goto end2;
      if (*(in - 2) == *"\\")
        goto end2;
    } else if (*in == *"\n") {
      l->col_start = (in - l->in) + 1;
      l->line++;
    }
    goto begin2;
  end2:
    if (in != l->end) {
      in++;
      next->loc.len = in - (l->in + l->off);
      next->kind = nk_str_lit;
      l->off += next->loc.len;
    } else {
      next->loc.len = in - (l->in + l->off);
      next->kind = nk_error;
      l->off += next->loc.len;
    }
  } else if (*in == *"/") {
    if (++in == l->end || *in != *"*") {
      next->loc.len = 1;
      next->kind = nk_div;
      l->off++;
    } else {
    begin3:
      ++in != l->end ? ({}) : ({ goto end3; });
      if (*in == *"/" && *(in - 1) == *"*")
        goto end3;
      if (*in == *"\n") {
        l->col_start = (in - l->in) + 1;
        l->line++;
      }
      goto begin3;
    end3:
      if (in == l->end) {
        next->loc.len = in - (l->in + l->off);
        next->kind = nk_error;
        l->off += next->loc.len;
      } else {
        next->loc.len = in - (l->in + l->off) + 1;
        next->kind = nk_comment;
        l->off += next->loc.len;
      }
    }
  } else if (*in == *"_") {
    long ch1;
    long ch2;
    long ch3;
    if (++in == l->end || *in != *"_")
      goto ident;
    if (++in == l->end || *in != *"a")
      goto ident;
    if (++in == l->end || *in != *"s")
      goto ident;
    if (++in == l->end || *in != *"m")
      goto ident;
    if (++in == l->end || *in != *"_")
      goto ident;
    if (++in == l->end || *in != *"_")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_asm;
    l->off += next->loc.len;
  } else if (*in == *"c") {
    long ch1;
    long ch2;
    long ch3;
    if (++in == l->end || *in != *"h")
      goto ident;
    if (++in == l->end || *in != *"a")
      goto ident;
    if (++in == l->end || *in != *"r")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_char;
    l->off += next->loc.len;
  } else if (*in == *"e") {
    long ch1;
    long ch2;
    long ch3;
    if (++in == l->end || *in != *"l") {
      if (in != l->end && *in == *"n") {
        if (++in == l->end || *in != *"u")
          goto ident;
        if (++in == l->end || *in != *"m")
          goto ident;

        if (++in != l->end) {
          ch1 = (*in - *"a") & 0xFF;
          ch2 = (*in - *"A") & 0xFF;
          ch3 = (*in - *"0") & 0xFF;
          if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
            goto ident;
        }

        next->loc.len = (long)(in - (l->in + l->off));
        next->kind = nk_enum;
        l->off += next->loc.len;
        goto end;
      }

      goto ident;
    }
    if (++in == l->end || *in != *"s")
      goto ident;
    if (++in == l->end || *in != *"e")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_else;
    l->off += next->loc.len;
  } else if (*in == *"g") {
    long ch1;
    long ch2;
    long ch3;

    if (++in == l->end || *in != *"o")
      goto ident;
    if (++in == l->end || *in != *"t")
      goto ident;
    if (++in == l->end || *in != *"o")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_goto;
    l->off += next->loc.len;
  } else if (*in == *"i") {
    long ch1;
    long ch2;
    long ch3;

    if (++in == l->end || *in != *"f")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_if;
    l->off += next->loc.len;
  } else if (*in == *"l") {
    long ch1;
    long ch2;
    long ch3;

    if (++in == l->end || *in != *"o")
      goto ident;
    if (++in == l->end || *in != *"n")
      goto ident;
    if (++in == l->end || *in != *"g")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_long;
    l->off += next->loc.len;
  } else if (*in == *"s") {
    long ch1;
    long ch2;
    long ch3;

    if (++in == l->end || *in != *"i") {
      if (in != l->end && *in == *"t") {
        if (++in == l->end || *in != *"r")
          goto ident;
        if (++in == l->end || *in != *"u")
          goto ident;
        if (++in == l->end || *in != *"c")
          goto ident;
        if (++in == l->end || *in != *"t")
          goto ident;

        if (++in != l->end) {
          ch1 = (*in - *"a") & 0xFF;
          ch2 = (*in - *"A") & 0xFF;
          ch3 = (*in - *"0") & 0xFF;
          if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
            goto ident;
        }

        next->loc.len = (long)(in - (l->in + l->off));
        next->kind = nk_struct;
        l->off += next->loc.len;
        goto end;
      }
      goto ident;
    }
    if (++in == l->end || *in != *"z")
      goto ident;
    if (++in == l->end || *in != *"e")
      goto ident;
    if (++in == l->end || *in != *"o")
      goto ident;
    if (++in == l->end || *in != *"f")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_sizeof;
    l->off += next->loc.len;
  } else if (*in == *"v") {
    long ch1;
    long ch2;
    long ch3;

    if (++in == l->end || *in != *"o")
      goto ident;
    if (++in == l->end || *in != *"i")
      goto ident;
    if (++in == l->end || *in != *"d")
      goto ident;

    if (++in != l->end) {
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
        goto ident;
    }

    next->loc.len = (long)(in - (l->in + l->off));
    next->kind = nk_void;
    l->off += next->loc.len;
  } else {
    long ch1;
    long ch2;
    long ch3;

    ch1 = (*in - *"a") & 0xFF;
    ch2 = (*in - *"A") & 0xFF;

    if (ch1 < 26 || ch2 < 26 || *in == *"_") {
    ident:
      in = l->in + l->off;
    begin4:
      ++in != l->end ? ({}) : ({ goto end4; });
      ch1 = (*in - *"a") & 0xFF;
      ch2 = (*in - *"A") & 0xFF;
      ch3 = (*in - *"0") & 0xFF;
      !(ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_") ? ({ goto end4; })
                                                         : ({});
      goto begin4;
    end4:
      next->loc.len = (long)(in - (l->in + l->off));
      next->kind = nk_ident;
      l->off += next->loc.len;
      goto end;
    } else {
      ch1 = (*in - *"0") & 0xFF;

      if (ch1 < 10) {
        long is_hex;
        long is_bin;
        is_hex = 0;
        is_bin = 0;
        if (in + 1 != l->end && *(in + 1) == *"x" && *in == *"0") {
          is_hex = 1;
          in++;
        } else if (in + 1 != l->end && *(in + 1) == *"b" && *in == *"0") {
          is_bin = 1;
          in++;
        }
      begin5:
        ++in == l->end ? ({ goto end5; }) : ({});
        ch1 = (*in - *"0") & 0xFF;
        is_bin&& ch1 > 1 ? ({ goto end5; }) : ({});
        ch1 >= 10 ? ({
          is_hex ? ({
            ch1 = (*in - *"a") & 0xFF;
            ch2 = (*in - *"A") & 0xFF;
            ch1 > 5 && ch2 > 5 ? ({ goto end5; }) : ({});
          })
                 : ({ goto end5; });
        })
                  : ({});
        goto begin5;
      end5:
        next->loc.len = (long)(in - (l->in + l->off));
        next->kind = nk_num_lit;
        l->off += next->loc.len;
        goto end;
      }
    }
    next->loc.len = 1;
    next->kind = nk_error;
    l->off++;
  }
end:;
}

void
node_init(struct parser* p, struct node** n, enum node_kind kind)
{
  arena_alloc((void*)n, p->a, sizeof(struct node));
  cmemset(*n, 0, sizeof(struct node));
  (*n)->kind = kind;
}

void
node_print(struct parser* p, struct node* n)
{
  (void)p;
  (void)n;
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
    : ({});
  write_str(2, msg);
  write_str(2, "\n");
  exit_group(-1);
}

void
proc_left_paren(struct parser* p)
{
  p->sp = 0;
}
void
proc_right_paren(struct parser* p)
{
  p->sp = 0;
}
void
proc_left_brace(struct parser* p)
{
  p->sp = 0;
}
void
proc_right_brace(struct parser* p)
{
  p->sp = 0;
}
void
proc_semi(struct parser* p)
{
  p->sp = 0;
}

void
parse(struct parser* p, struct lexer* l, long debug)
{
begin:
  p->sp >= p->capacity ? panic("Stack overflow!") : ({});
  lexer_next_token(&p->stack[p->sp], l);
  p->stack[p->sp].kind == nk_error
    ? print_error(&p->stack[p->sp], "unrecognized token")
    : ({});
  debug && p->stack[p->sp].kind <= nk_term ? ({
    long r;
    write_str(1, "token=");
    write_num(1, p->stack[p->sp].kind);
    write_str(1, ",offset=");
    write_num(1, p->stack[p->sp].loc.off);
    write_str(1, ",value='");
    write(&r, 1, l->in + p->stack[p->sp].loc.off, p->stack[p->sp].loc.len);
    write_str(1, "',line=");
    write_num(1, p->stack[p->sp].loc.line + 1);
    write_str(1, ",col=");
    write_num(1, p->stack[p->sp].loc.col + 1);
    write_str(1, "\n");
  })
                                           : ({});

  p->stack[p->sp].kind == nk_comment ? ({ goto begin; }) : ({});
  p->stack[p->sp].kind == nk_term ? ({ goto end; }) : ({
    enum node_kind kind;
    kind = p->stack[p->sp].kind;
    p->sp++;
    kind == nk_right_paren   ? proc_right_paren(p)
    : kind == nk_left_paren  ? proc_left_paren(p)
    : kind == nk_left_brace  ? proc_left_brace(p)
    : kind == nk_right_brace ? proc_right_brace(p)
    : kind == nk_semi        ? proc_semi(p)
                             : ({});
  });
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

  argc < 2 ? (
               {
                 write_str(2, "Usage: famc <file>\n");
                 exit_group(-1);
               })
           : ({});

  argc > 2 ? ({
    cstrcmp(&r, argv[1], "--debug");
    r == 0 ? debug = 1 : 0;
  })
           : 0;

  open(&fd, argv[argc - 1], 0, 0);
  fd < 0 ? (
             {
               write_str(2, "Could not open file!\n");
               exit_group(-1);
             })
         : ({});

  lseek(&size, fd, 0, 2);
  size < 0 ? (
               {
                 write_str(2,
                           "Could not determine "
                           "file size!\n");
                 exit_group(-1);
               })
           : ({});

  fmap_ro((void*)&(&l)->in, fd, size, 0);
  l.in == 0 ? ({
    write_str(2, "Could not mmap file!\n");
    exit_group(-1);
  })
            : ({});
  close(&r, fd);

  arena_init(&p.a, 256 * 1024 * 1024, 8);
  node_init(&p, &p.root, nk_program);
  p.in = l.in;
  p.current = p.root;
  p.sp = 0;
  p.capacity = 4096;
  map((void*)&p.stack, sizeof(struct node) * 4096);
  p.stack ? ({}) : panic("could not allocate stack");

  l.end = l.in + size;
  l.off = 0;
  l.col_start = 0;
  l.line = 0;

  parse(&p, &l, debug);
  debug ? node_print(&p, p.root) : ({});
  debug ? write_str(1, "success!\n") : ({});
  exit_group(0);
}
