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

enum node_kind
{
  nk_error,
  nk_comment,
  nk_semi,
  nk_comma,
  nk_colon,
  nk_asterisk,
  nk_questionmark,
  nk_ampersand,
  nk_equal,
  nk_double_equal,
  nk_gte,
  nk_sizeof,
  nk_struct,
  nk_ident,
  nk_str_lit,
  nk_num_lit,
  nk_program,
  nk_term
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
arena_init(struct arena** a, long size)
{
  struct arena* ret;
  map((void*)&ret, size + sizeof(struct arena));
  ret ? ({}) : panic("Could not mmap sufficient memory! Halting.\n");
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
    : ({});

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
  cur = &l->root;
  i = 0;

  while (i < len) {
    long ch;
    ch = s[i] & 0xFF;
    cur->children[ch] ? ({
      cur = cur->children[ch];
      cur->kind ? ({
        panic(
          "Compiler internal error: Must register lexer entries longer first!");
      })
                : ({});
    })
                      : ({
                          arena_alloc((void*)&cur->children[ch],
                                      a,
                                      sizeof(struct token_trie));
                          cur = cur->children[ch];
                          cur->kind = 0;
                          cmemset(
                            cur->children, 0, sizeof(struct token_trie*) * 256);
                        });
    i == len - 1 ? ({
      cur->kind = kind;
      cur->len = len;
      cur->is_reserved_word = is_reserved_word;
    })
                 : ({});
    i++;
  }
}

void
lexer_init(struct lexer* l, struct arena* a, long size)
{
  l->end = l->in + size;
  l->off = 0;
  l->col_start = 0;
  l->line = 0;
  cmemset(&l->root, 0, sizeof(struct token_trie));

  lexer_register(l, a, ";", 1, nk_semi, 0);
  lexer_register(l, a, "*", 1, nk_asterisk, 0);
  lexer_register(l, a, ",", 1, nk_comma, 0);
  lexer_register(l, a, "==", 2, nk_double_equal, 0);
  lexer_register(l, a, ">=", 2, nk_gte, 0);
  lexer_register(l, a, "=", 1, nk_equal, 0);
  lexer_register(l, a, "/*", 2, nk_comment, 0);
  lexer_register(l, a, "sizeof", 6, nk_sizeof, 1);
  lexer_register(l, a, "struct", 6, nk_struct, 1);
}

void
lexer_next_token(struct node* next, struct lexer* l)
{
  char* in;
  struct token_trie* node;
  long ch;
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

  in == l->end ? ({
    next->loc.len = 0;
    next->kind = nk_term;
    goto end;
  })
               : ({});

  *in == *"\"" ? ({
    begin2:
      ++in != l->end ? ({}) : ({ goto end2; });
      *in == *"\""   ? ({
        *(in - 1) != *"\\" ? ({ goto end2; }) : ({});
        *(in - 2) == *"\\" ? ({ goto end2; }) : ({});
      })
      : *in == *"\n" ? ({
          l->col_start = (in - l->in) + 1;
          l->line++;
        })
                     : ({});

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
               : ({});

  node = &l->root;
  next->loc.len = 1;
  next->kind = nk_error;

begin3:
  in == l->end ? ({ goto end3; }) : ({});
  ch = *in++ & 0xFF;
  (node = node->children[ch])
    ? ({
        node->len ? ({
          next->loc.len = node->len;
          next->kind = node->kind;
        })
                  : ({});
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
                  next->kind = nk_error;
                  next->loc.len = 1;
                })
            : ({});
        })
                   : ({});
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
            while (++in != l->end) {
              ch1 = (*in - *"a") & 0xFF;
              ch2 = (*in - *"A") & 0xFF;
              ch3 = (*in - *"0") & 0xFF;
              !(ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_") ? ({ break; })
                                                                 : ({});
            }
            next->loc.len = in - (l->in + l->off);
            next->kind = nk_ident;
          })
      : ({});
    goto end;
  })
                         : ({});

  next->kind == nk_comment ? ({
    begin4:
      ++in != l->end ? ({}) : ({ goto end4; });
      *in == *"/" && *(in - 1) == *"*" ? ({ goto end4; }) : ({});
      *in == *"\n" ? ({
        l->col_start = (in - l->in) + 1;
        l->line++;
      })
                   : ({});
      goto begin4;
    end4:
      in == l->end ? ({
        next->loc.len = in - (l->in + l->off);
        next->kind = nk_error;
      })
                   : ({
                       next->loc.len = in - (l->in + l->off) + 1;
                       next->kind = nk_comment;
                     });
  })
                           : ({});
end:
  l->off += next->loc.len;
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
parse(struct parser* p, struct lexer* l, long debug)
{
  struct node token;
begin:
  lexer_next_token(&token, l);
  token.kind == nk_error ? print_error(&token, "unrecognized token") : ({});
  debug&& token.kind <= nk_term ? ({
    long r;
    write_str(1, "token=");
    write_num(1, token.kind);
    write_str(1, ",offset=");
    write_num(1, token.loc.off);
    write_str(1, ",value='");
    write(&r, 1, l->in + token.loc.off, token.loc.len);
    write_str(1, "',line=");
    write_num(1, token.loc.line + 1);
    write_str(1, ",col=");
    write_num(1, token.loc.col + 1);
    write_str(1, "\n");
  })
                                : ({});

  token.kind == nk_term ? ({ goto end; }) : ({ goto begin; });
end:;
  (void)p;
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

  arena_init(&p.a, 256 * 1024 * 1024);
  node_init(&p, &p.root, nk_program);
  p.in = l.in;
  p.current = p.root;

  lexer_init(&l, p.a, size);
  parse(&p, &l, debug);
  debug ? node_print(&p, p.root) : ({});
  debug ? write_str(1, "success!\n") : ({});
  exit_group(0);
}

