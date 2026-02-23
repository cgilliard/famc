__asm__(
    ".section .text\n"
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
    "    call main\n");
__asm__(
    ".section .text\n"
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

__asm__(
    ".section .text\n"
    "div:\n"
    "    mov    %rdx, %rcx\n"
    "    mov    %rsi, %rax\n"
    "    cqto\n"
    "    idiv   %rcx\n"
    "    mov    %rax, (%rdi)\n"
    "    ret\n");

__asm__(
    ".section .text\n"
    "mod:\n"
    "    mov    %rdx, %rcx\n"
    "    mov    %rsi, %rax\n"
    "    cqto\n"
    "    idiv   %rcx\n"
    "    mov    %rdx, (%rdi)\n"
    "    ret\n");

__asm__(
    ".section .text\n"
    "cycle_counter:\n"
    "    rdtsc\n"
    "    shlq    $32, %rdx\n"
    "    orq     %rdx, %rax\n"
    "    movq    %rax, (%rdi)\n"
    "    ret\n");

struct arena {
	char *start;
	char *current;
	char *end;
	long align;
};

struct lexer {
	char *in;
	char *end;
	long off;
	long line;
	long col_start;
};

enum node_kind {
	nk_error,
	nk_semi,
	nk_comma,
	nk_colon,
	nk_asterisk,
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
	nk_break,
	nk_char,
	nk_else,
	nk_enum,
	nk_goto,
	nk_if,
	nk_long,
	nk_sizeof,
	nk_struct,
	nk_void,
	nk_while,
	nk_num_lit,
	nk_str_lit,
	nk_comment,
	nk_ident,
	nk_program,
	nk_type,
	nk_function,
	nk_term
};

struct source_location {
	long off;
	long len;
	long line;
	long col;
};

struct slice {
	void *ptr;
	long len;
};

enum type_kind {
	type_kind_struct,
	type_kind_enum,
	type_kind_long,
	type_kind_char,
	type_kind_void
};

struct type_data {
	enum type_kind kind;
	struct slice type_name;
	struct slice var_name;
	long levels;
};

struct node {
	enum node_kind kind;
	struct source_location loc;
	struct node *parent;
	struct node *first_child;
	struct node *last_child;
	struct node *next_sibling;
	struct node *prev_sibling;
	void *node_data;
};

struct parser {
	char *in;
	struct arena *a;
	struct node *root;
	struct node *current;
	long scope;
	long sp;
	struct node *stack;
	long capacity;
};

void memmove(void *dest, void *src, long n) {
	char *d = (void *)((char *)dest + n);
	char *s = (void *)((char *)src + n);
	while (n--) d--, s--, *d = *s;
}

void memset(void *dest, char c, long n) {
	char *tmp = dest;
	while (n--) *tmp++ = (char)c;
}

void memcpy(void *dest, void *src, long n) {
	char *d = (void *)dest;
	char *s = (void *)src;
	while (n--) *d++ = *s++;
}

void strlen(long *ret, char *x) {
	char *y = x;
	while (*x) x++;
	*ret = x - y;
}

void strcmp(long *ret, char *x, char *y) {
	while (*x == *y && *x) x++, y++;
	if (*x > *y)
		*ret = 1;
	else if (*y > *x)
		*ret = -1;
	else
		*ret = 0;
}

void exit_group(long status) {
	long ign;
	syscall(&ign, 231, status, 0, 0, 0, 0, 0);
}

void write(long *ret, long fd, void *buf, long len) {
	syscall(ret, 1, fd, (long)buf, len, 0, 0, 0);
}

void open(long *ret, char *path, long flags, long mode) {
	syscall(ret, 2, (long)path, flags, mode, 0, 0, 0);
}

void close(long *ret, long fd) { syscall(ret, 3, fd, 0, 0, 0, 0, 0); }

void lseek(long *ret, long fd, long offset, long whence) {
	syscall(ret, 8, fd, offset, whence, 0, 0, 0);
}

void mmap(void **ret, void *addr, long length, long prot, long flags, long fd,
	  long offset) {
	syscall((void *)ret, 9, (long)addr, length, prot, flags, fd, offset);
}

void map(void **ret, long size) {
	long tmp;
	mmap(ret, 0, size, 3, 34, -1, 0);
	tmp = (long)*ret;
	if (tmp < 0 && tmp >= -4095) *ret = 0;
}

void fmap_ro(void **ret, long fd, long size, long offset) {
	long tmp;
	mmap(ret, 0, size, 1, 1, fd, offset);
	tmp = (long)*ret;
	if (tmp < 0 && tmp >= -4095) *ret = 0;
}

void panic(char *msg) {
	long r;
	strlen(&r, msg);
	write(&r, 2, msg, r);
	write(&r, 2, "\n", 1);
	exit_group(-1);
}

void write_str(long fd, char *msg) {
	long r;
	strlen(&r, msg);
	write(&r, fd, msg, r);
}

void write_num(long fd, long num) {
	char buf[21];
	char *p;
	long len;
	long written;

	p = buf + sizeof(buf) - 1;
	*p = *"\0";

	if (num < 0) {
		num = -num;
		*--p = *"-";
	} else if (num == 0)
		*--p = *"0";

	while (num > 0) {
		long tmp;
		mod(&tmp, num, 10);
		*--p = *"0" + tmp;
		div(&num, num, 10);
	}

	len = (buf + sizeof(buf) - 1) - p;
	write(&written, fd, p, len);
}

void arena_init(struct arena **a, long size, long align) {
	struct arena *ret;
	map((void *)&ret, size + sizeof(struct arena));
	if (!ret) panic("Could not mmap sufficient memory! Halting.\n");
	ret->start = ((char *)(ret)) + sizeof(struct arena);
	ret->end = ret->start + size;
	if (((long)ret->start % align) == 0)
		ret->current = ret->start;
	else
		ret->current =
		    (char *)((long)(ret->start + (align - 1)) & ~(align - 1));
	ret->align = align;
	*a = ret;
}

void arena_alloc(void **result, struct arena *a, long size) {
	void *ret;
	long to_alloc;
	to_alloc = (size + (a->align - 1)) & ~(a->align - 1);
	if (a->current > a->end - to_alloc || to_alloc > (long)a->end)
		panic("Could not allocate memory! Halting.\n");

	ret = a->current;
	a->current += to_alloc;
	*result = ret;
}

void lexer_next_token(struct node *next, struct lexer *l) {
	char *in = l->in + l->off;

	while (in != l->end) {
		if ((*in - 9) >= 5 && *in != *" ") break;
		if (*in == *"\n") {
			l->col_start = (in - l->in) + 1;
			l->line++;
		}
		in++;
	}
	l->off = in - l->in;
	next->loc.off = l->off;
	next->loc.line = l->line;
	next->loc.col = l->off - l->col_start;
	if (in == l->end) {
		next->loc.len = 0;
		next->kind = nk_term;
	} else if (*in == *";") {
		next->loc.len = 1;
		next->kind = nk_semi;
		l->off++;
	} else if (*in == *",") {
		next->loc.len = 1;
		next->kind = nk_comma;
		l->off++;
	} else if (*in == *":") {
		next->loc.len = 1;
		next->kind = nk_colon;
		l->off++;
	} else if (*in == *"*") {
		next->loc.len = 1;
		next->kind = nk_asterisk;
		l->off++;
	} else if (*in == *"|") {
		if (++in != l->end && *in == *"|") {
			next->loc.len = 2;
			next->kind = nk_double_pipe;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->kind = nk_pipe;
			l->off++;
		}
	} else if (*in == *".") {
		next->loc.len = 1;
		next->kind = nk_dot;
		l->off++;
	} else if (*in == *"&") {
		if (++in != l->end && *in == *"&") {
			next->loc.len = 2;
			next->kind = nk_double_ampersand;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->kind = nk_ampersand;
			l->off++;
		}
	} else if (*in == *"=") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->kind = nk_double_equal;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->kind = nk_equal;
			l->off++;
		}
	} else if (*in == *"(") {
		next->loc.len = 1;
		next->kind = nk_left_paren;
		l->off++;
	} else if (*in == *")") {
		next->loc.len = 1;
		next->kind = nk_right_paren;
		l->off++;
	} else if (*in == *"{") {
		next->loc.len = 1;
		next->kind = nk_left_brace;
		l->off++;
	} else if (*in == *"}") {
		next->loc.len = 1;
		next->kind = nk_right_brace;
		l->off++;
	} else if (*in == *"[") {
		next->loc.len = 1;
		next->kind = nk_left_bracket;
		l->off++;
	} else if (*in == *"]") {
		next->loc.len = 1;
		next->kind = nk_right_bracket;
		l->off++;
	} else if (*in == *"-") {
		if (++in != l->end) {
			if (*in == *"=") {
				next->loc.len = 2;
				next->kind = nk_hyphen_equal;
				l->off += 2;
				goto end;
			} else if (*in == *"-") {
				next->loc.len = 2;
				next->kind = nk_double_hyphen;
				l->off += 2;
				goto end;
			} else if (*in == *">") {
				next->loc.len = 2;
				next->kind = nk_arrow;
				l->off += 2;
				goto end;
			}
		}
		next->loc.len = 1;
		next->kind = nk_hyphen;
		l->off++;
	} else if (*in == *"+") {
		if (++in != l->end) {
			if (*in == *"=") {
				next->loc.len = 2;
				next->kind = nk_plus_equal;
				l->off += 2;
				goto end;
			} else if (*in == *"+") {
				next->loc.len = 2;
				next->kind = nk_double_plus;
				l->off += 2;
				goto end;
			}
		}
		next->loc.len = 1;
		next->kind = nk_plus;
		l->off++;
	} else if (*in == *"<") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->kind = nk_lte;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->kind = nk_lt;
			l->off++;
		}
	} else if (*in == *">") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->kind = nk_gte;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->kind = nk_gt;
			l->off++;
		}
	} else if (*in == *"!") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->kind = nk_ne;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->kind = nk_bang;
			l->off++;
		}
	} else if (*in == *"\"") {
		while (++in != l->end) {
			if (*in == *"\"") {
				if (*(in - 1) != *"\\") break;
				if (*(in - 2) == *"\\") break;
			} else if (*in == *"\n") {
				l->col_start = (in - l->in) + 1;
				l->line++;
			}
		}
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
		if (++in == l->end || *in != *"*") goto error;
		while (++in != l->end) {
			if (*in == *"/" && *(in - 1) == *"*") break;
			if (*in == *"\n") {
				l->col_start = (in - l->in) + 1;
				l->line++;
			}
		}
		if (in == l->end) {
			next->loc.len = in - (l->in + l->off);
			next->kind = nk_error;
			l->off += next->loc.len;
		} else {
			next->loc.len = in - (l->in + l->off) + 1;
			next->kind = nk_comment;
			l->off += next->loc.len;
		}
	} else if (*in == *"_") {
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"_") goto ident;
		if (++in == l->end || *in != *"a") goto ident;
		if (++in == l->end || *in != *"s") goto ident;
		if (++in == l->end || *in != *"m") goto ident;
		if (++in == l->end || *in != *"_") goto ident;
		if (++in == l->end || *in != *"_") goto ident;

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
	} else if (*in == *"b") {
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"r") goto ident;
		if (++in == l->end || *in != *"e") goto ident;
		if (++in == l->end || *in != *"a") goto ident;
		if (++in == l->end || *in != *"k") goto ident;

		if (++in != l->end) {
			ch1 = (*in - *"a") & 0xFF;
			ch2 = (*in - *"A") & 0xFF;
			ch3 = (*in - *"0") & 0xFF;
			if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
				goto ident;
		}

		next->loc.len = (long)(in - (l->in + l->off));
		next->kind = nk_break;
		l->off += next->loc.len;
	} else if (*in == *"c") {
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"h") goto ident;
		if (++in == l->end || *in != *"a") goto ident;
		if (++in == l->end || *in != *"r") goto ident;

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
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"l") {
			if (in != l->end && *in == *"n") {
				if (++in == l->end || *in != *"u") goto ident;
				if (++in == l->end || *in != *"m") goto ident;

				if (++in != l->end) {
					ch1 = (*in - *"a") & 0xFF;
					ch2 = (*in - *"A") & 0xFF;
					ch3 = (*in - *"0") & 0xFF;
					if (ch1 < 26 || ch2 < 26 || ch3 < 10 ||
					    *in == *"_")
						goto ident;
				}

				next->loc.len = (long)(in - (l->in + l->off));
				next->kind = nk_enum;
				l->off += next->loc.len;
				goto end;
			}

			goto ident;
		}
		if (++in == l->end || *in != *"s") goto ident;
		if (++in == l->end || *in != *"e") goto ident;

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
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"o") goto ident;
		if (++in == l->end || *in != *"t") goto ident;
		if (++in == l->end || *in != *"o") goto ident;

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
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"f") goto ident;

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
	} else if (*in == *"l") {
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"o") goto ident;
		if (++in == l->end || *in != *"n") goto ident;
		if (++in == l->end || *in != *"g") goto ident;

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
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"i") {
			if (in != l->end && *in == *"t") {
				if (++in == l->end || *in != *"r") goto ident;
				if (++in == l->end || *in != *"u") goto ident;
				if (++in == l->end || *in != *"c") goto ident;
				if (++in == l->end || *in != *"t") goto ident;

				if (++in != l->end) {
					ch1 = (*in - *"a") & 0xFF;
					ch2 = (*in - *"A") & 0xFF;
					ch3 = (*in - *"0") & 0xFF;
					if (ch1 < 26 || ch2 < 26 || ch3 < 10 ||
					    *in == *"_")
						goto ident;
				}

				next->loc.len = (long)(in - (l->in + l->off));
				next->kind = nk_struct;
				l->off += next->loc.len;
				goto end;
			}
			goto ident;
		}
		if (++in == l->end || *in != *"z") goto ident;
		if (++in == l->end || *in != *"e") goto ident;
		if (++in == l->end || *in != *"o") goto ident;
		if (++in == l->end || *in != *"f") goto ident;

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
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"o") goto ident;
		if (++in == l->end || *in != *"i") goto ident;
		if (++in == l->end || *in != *"d") goto ident;

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
	} else if (*in == *"w") {
		long ch1, ch2, ch3;
		if (++in == l->end || *in != *"h") goto ident;
		if (++in == l->end || *in != *"i") goto ident;
		if (++in == l->end || *in != *"l") goto ident;
		if (++in == l->end || *in != *"e") goto ident;

		if (++in != l->end) {
			ch1 = (*in - *"a") & 0xFF;
			ch2 = (*in - *"A") & 0xFF;
			ch3 = (*in - *"0") & 0xFF;
			if (ch1 < 26 || ch2 < 26 || ch3 < 10 || *in == *"_")
				goto ident;
		}

		next->loc.len = (long)(in - (l->in + l->off));
		next->kind = nk_while;
		l->off += next->loc.len;
	} else {
		long ch1, ch2, ch3;

		ch1 = (*in - *"a") & 0xFF;
		ch2 = (*in - *"A") & 0xFF;

		if (ch1 < 26 || ch2 < 26 || *in == *"_") {
		ident:
			in = l->in + l->off;
			while (++in != l->end) {
				ch1 = (*in - *"a") & 0xFF;
				ch2 = (*in - *"A") & 0xFF;
				ch3 = (*in - *"0") & 0xFF;
				if (!(ch1 < 26 || ch2 < 26 || ch3 < 10 ||
				      *in == *"_"))
					break;
			}
			next->loc.len = (long)(in - (l->in + l->off));
			next->kind = nk_ident;
			l->off += next->loc.len;
			goto end;
		} else {
			ch1 = (*in - *"0") & 0xFF;

			if (ch1 < 10) {
				long is_hex = 0, is_bin = 0;
				if (in + 1 != l->end && *(in + 1) == *"x" &&
				    *in == *"0") {
					is_hex = 1;
					in++;
				} else if (in + 1 != l->end &&
					   *(in + 1) == *"b" && *in == *"0") {
					is_bin = 1;
					in++;
				}
				while (++in != l->end) {
					ch1 = (*in - *"0") & 0xFF;
					if (is_bin && ch1 > 1)
						break;
					else if (ch1 >= 10) {
						if (is_hex) {
							ch1 =
							    (*in - *"a") & 0xFF;
							ch2 =
							    (*in - *"A") & 0xFF;
							if (ch1 > 5 && ch2 > 5)
								break;
						} else
							break;
					}
				}
				next->loc.len = (long)(in - (l->in + l->off));
				next->kind = nk_num_lit;
				l->off += next->loc.len;
				goto end;
			}
		}
	error:
		next->loc.len = 1;
		next->kind = nk_error;
		l->off++;
	}
end:;
}

void print_error(struct node *n, char *msg) {
	write_str(2, "Error: ");
	if (n) {
		write_num(2, 1 + n->loc.line);
		write_str(2, ":");
		write_num(2, 1 + n->loc.col);
		write_str(2, ": ");
	}
	write_str(2, msg);
	write_str(2, "\n");
	exit_group(-1);
}

void node_print_impl(struct node *n, long depth) {
	long i = 0;
	struct node *child = n->first_child;

	while (i++ < depth) write_str(1, "   ");
	write_str(1, "kind=");
	write_num(1, n->kind);
	if (n->kind == nk_asm) {
		write_str(1, " (asm)");
	} else if (n->kind == nk_str_lit) {
		long r;
		struct slice *s = n->node_data;
		write_str(1, " (string literal) [");
		write(&r, 1, s->ptr, s->len);
		write_str(1, "]");
	} else if (n->kind == nk_enum) {
		long r;
		struct slice *s = n->node_data;
		write_str(1, " (enum) [");
		write(&r, 1, s->ptr, s->len);
		write_str(1, "]");
	} else if (n->kind == nk_struct) {
		long r;
		struct slice *s = n->node_data;
		write_str(1, " (struct) [");
		write(&r, 1, s->ptr, s->len);
		write_str(1, "]");
	} else if (n->kind == nk_type) {
		long r;
		struct type_data *td = n->node_data;
		write_str(1, " (type) [");
		if (td->kind == type_kind_char)
			write_str(1, "char ");
		else if (td->kind == type_kind_long)
			write_str(1, "long ");
		else if (td->kind == type_kind_void)
			write_str(1, "void ");
		else if (td->kind == type_kind_struct)
			write_str(1, "struct ");
		else if (td->kind == type_kind_enum)
			write_str(1, "enum ");
		write(&r, 1, td->type_name.ptr, td->type_name.len);
		if (td->type_name.len) write_str(1, " ");
		i = 0;
		while (i < td->levels) {
			write(&r, 1, "*", 1);
			i++;
		}
		write(&r, 1, td->var_name.ptr, td->var_name.len);
		write_str(1, "]");
	} else if (n->kind == nk_ident) {
		long r;
		struct slice *s = n->node_data;
		write_str(1, " (ident) [");
		write(&r, 1, s->ptr, s->len);
		write_str(1, "]");
	} else if (n->kind == nk_function) {
		long r;
		struct slice *s = n->node_data;
		write_str(1, " (function) [");
		write(&r, 1, s->ptr, s->len);
		write_str(1, "]");
	}
	write_str(1, "\n");

	while (child) {
		node_print_impl(child, depth + 1);
		child = child->next_sibling;
	}
}

void node_print(struct node *n) { node_print_impl(n, 0); }

void node_init(struct node *n, enum node_kind kind) {
	memset(n, 0, sizeof(struct node));
	n->kind = kind;
}

void node_append(struct node *parent, struct node *child, long prepend) {
	child->parent = parent;
	if (parent->last_child) {
		if (prepend)
			parent->first_child->prev_sibling = child;
		else
			parent->last_child->next_sibling = child;
	} else {
		if (prepend)
			parent->last_child = child;
		else
			parent->first_child = child;
	}
	if (prepend) {
		child->next_sibling = parent->first_child;
		parent->first_child = child;
	} else {
		child->prev_sibling = parent->last_child;
		parent->last_child = child;
	}
}

void proc_nk_left_paren_root(struct parser *p) {
	struct node *asm_node;
	if (p->sp < 2) {
		if (p->sp)
			print_error(&p->stack[p->sp - 1],
				    "unexpected token '('");
		else
			print_error(0, "unexpected token '('");
	}
	if (p->stack[p->sp - 2].kind == nk_asm) {
		arena_alloc((void *)&asm_node, p->a, sizeof(struct node));
		node_init(asm_node, nk_asm);
		node_append(p->root, asm_node, 0);
		p->current = asm_node;
	}
}

void proc_nk_asm_complete(struct parser *p) {
	p->sp -= 2;
	while (p->sp > 0 && p->stack[p->sp].kind != nk_left_paren) {
		struct node *nnode;
		struct slice *s;
		if (p->stack[p->sp].kind != nk_str_lit)
			print_error(&p->stack[p->sp],
				    "expected string literal");

		arena_alloc((void *)&nnode, p->a, sizeof(struct node));
		arena_alloc((void *)&s, p->a, sizeof(struct slice));
		node_init(nnode, nk_str_lit);
		s->ptr = p->in + p->stack[p->sp].loc.off;
		s->len = p->stack[p->sp].loc.len;
		nnode->node_data = s;
		node_append(p->current, nnode, 1);

		p->sp--;
	}

	if (p->sp > 1) print_error(&p->stack[p->sp - 2], "unexpected token");
	p->current = p->current->parent;
}

void proc_build_type(struct node **node, struct parser *p) {
	struct node *type_node;
	struct type_data *td;

	arena_alloc((void *)&type_node, p->a, sizeof(struct node));
	node_init(type_node, nk_type);
	arena_alloc((void *)&td, p->a, sizeof(struct type_data));
	type_node->node_data = td;

	if (p->sp < 0) print_error(0, "unexpected token");
	if (p->stack[p->sp].kind != nk_ident)
		print_error(&p->stack[p->sp], "expected identifier");

	td->var_name.ptr = p->in + p->stack[p->sp].loc.off;
	td->var_name.len = p->stack[p->sp].loc.len;
	td->levels = 0;

	while (--p->sp >= 0) {
		if (p->stack[p->sp].kind != nk_asterisk) break;
		td->levels++;
	}
	if (p->sp < 0) print_error(&p->stack[0], "unexpected token");
	if (p->stack[p->sp].kind == nk_ident) {
		td->type_name.ptr = p->in + p->stack[p->sp].loc.off;
		td->type_name.len = p->stack[p->sp].loc.len;

		p->sp--;
		if (p->sp < 0) print_error(&p->stack[0], "unexpected token");
		if (p->stack[p->sp].kind != nk_struct &&
		    p->stack[p->sp].kind != nk_enum)
			print_error(&p->stack[p->sp], "unexpected token");

		if (p->stack[p->sp].kind == nk_struct)
			td->kind = type_kind_struct;
		else
			td->kind = type_kind_enum;
	} else {
		if (p->stack[p->sp].kind != nk_char &&
		    p->stack[p->sp].kind != nk_long &&
		    p->stack[p->sp].kind != nk_void)
			print_error(&p->stack[p->sp], "unexpected token");

		if (p->stack[p->sp].kind == nk_void && !td->levels)
			print_error(&p->stack[p->sp],
				    "void can only be a pointer type");

		if (p->stack[p->sp].kind == nk_char)
			td->kind = type_kind_char;
		else if (p->stack[p->sp].kind == nk_long)
			td->kind = type_kind_long;
		else if (p->stack[p->sp].kind == nk_void)
			td->kind = type_kind_void;
	}
	p->sp--;
	*node = type_node;
}

void proc_struct_elem(struct parser *p) {
	struct node *type_node;
	p->sp--;

	proc_build_type(&type_node, p);
	node_append(p->current, type_node, 1);
}

void proc_function(struct parser *p) {
	struct slice *s;
	struct node *function_node;

	arena_alloc((void *)&function_node, p->a, sizeof(struct node));
	node_init(function_node, nk_function);

	p->scope = 0;
	p->sp -= 2;

	if (p->stack[p->sp].kind != nk_right_paren)
		print_error(&p->stack[p->sp], "Unexpected token. Expected ')'");

	p->sp--;

	while (p->sp > 0 && p->stack[p->sp].kind != nk_left_paren) {
		struct node *nnode;

		proc_build_type(&nnode, p);
		node_append(function_node, nnode, 1);
		if (p->stack[p->sp].kind == nk_left_paren) break;
		p->sp--;
	}
	p->sp--;
	if (p->sp <= 0) print_error(&p->stack[0], "unexpected token");
	if (p->stack[p->sp].kind != nk_ident)
		print_error(&p->stack[p->sp], "expected identifier");
	arena_alloc((void *)&s, p->a, sizeof(struct slice));
	s->ptr = p->in + p->stack[p->sp].loc.off;
	s->len = p->stack[p->sp].loc.len;

	p->sp--;
	if (p->sp != 0 || p->stack[0].kind != nk_void)
		print_error(&p->stack[0], "unexpected token");

	function_node->node_data = s;
	node_append(p->root, function_node, 0);
	p->current = function_node;
}

void proc_struct(struct parser *p) {
	struct node *str_node;

	arena_alloc((void *)&str_node, p->a, sizeof(struct node));
	node_init(str_node, nk_struct);
	node_append(p->root, str_node, 0);
	p->current = str_node;
}

void proc_enum(struct parser *p) {
	struct node *str_node;

	arena_alloc((void *)&str_node, p->a, sizeof(struct node));
	node_init(str_node, nk_enum);
	node_append(p->root, str_node, 0);
	p->current = str_node;
}

void proc_struct_complete(struct parser *p) {
	struct slice *s;

	p->sp -= 2;
	while (p->sp >= 0 && p->stack[p->sp].kind != nk_left_brace)
		proc_struct_elem(p);

	p->sp--;
	if (p->sp < 1) print_error(&p->stack[0], "unexpected token");
	if (p->stack[p->sp].kind != nk_ident)
		print_error(&p->stack[p->sp], "expected identifier");

	arena_alloc((void *)&s, p->a, sizeof(struct slice));
	s->ptr = p->in + p->stack[p->sp].loc.off;
	s->len = p->stack[p->sp].loc.len;
	p->current->node_data = s;

	p->current = p->current->parent;
}

void proc_enum_complete(struct parser *p) {
	struct slice *name;

	p->sp -= 2;
	while (p->sp >= 0) {
		struct slice *s;
		struct node *nnode;

		if (p->stack[p->sp].kind != nk_ident)
			print_error(&p->stack[p->sp], "expected identifier");

		arena_alloc((void *)&s, p->a, sizeof(struct slice));
		s->ptr = p->in + p->stack[p->sp].loc.off;
		s->len = p->stack[p->sp].loc.len;
		arena_alloc((void *)&nnode, p->a, sizeof(struct node));
		node_init(nnode, nk_ident);
		nnode->node_data = s;
		node_append(p->current, nnode, 1);

		p->sp--;
		if (p->sp < 0) print_error(&p->stack[0], "unexpected token");
		if (p->stack[p->sp].kind == nk_left_brace) break;
		if (p->stack[p->sp].kind != nk_comma)
			print_error(&p->stack[p->sp],
				    "expected comma or brace");
		p->sp--;
	}
	p->sp--;
	if (p->sp < 1) print_error(&p->stack[0], "unexpected token");
	if (p->stack[p->sp].kind != nk_ident)
		print_error(&p->stack[p->sp], "expected identifier");

	arena_alloc((void *)&name, p->a, sizeof(struct slice));
	name->ptr = p->in + p->stack[p->sp].loc.off;
	name->len = p->stack[p->sp].loc.len;
	p->current->node_data = name;
	p->current = p->current->parent;
}

void proc_nk_left_brace_root(struct parser *p) {
	if (p->sp >= 3) {
		if (p->stack[p->sp - 3].kind == nk_struct) {
			if (p->sp > 3)
				print_error(&p->stack[p->sp - 4],
					    "unexpected token");
			proc_struct(p);
		} else if (p->stack[p->sp - 3].kind == nk_enum) {
			if (p->sp > 3)
				print_error(&p->stack[p->sp - 4],
					    "unexpected token");
			proc_enum(p);
		} else
			proc_function(p);
	} else {
		if (p->sp > 0)
			print_error(&p->stack[p->sp - 1],
				    "unexpected token '{'");
		else
			print_error(&p->stack[0], "unexpected token '{'");
	}
}
void proc_nk_left_brace_function(struct parser *p) { p->scope++; }

void proc_nk_left_paren(struct parser *p) {
	if (p->current == p->root) proc_nk_left_paren_root(p);
}

void proc_nk_right_paren(struct parser *p) {
	if (p->current->kind == nk_asm) proc_nk_asm_complete(p);
}

void proc_nk_left_brace(struct parser *p) {
	if (p->current == p->root)
		proc_nk_left_brace_root(p);
	else if (p->current->kind == nk_function)
		proc_nk_left_brace_function(p);
}

void proc_function_complete(struct parser *p) {
	if (!p->scope) {
		write_str(2, "function complete\n");
		p->current = p->current->parent;
	} else
		p->scope--;
}

void proc_nk_right_brace(struct parser *p) {
	if (p->current->kind == nk_struct)
		proc_struct_complete(p);
	else if (p->current->kind == nk_enum)
		proc_enum_complete(p);
	else if (p->current->kind == nk_function)
		proc_function_complete(p);

	p->sp = 0;
}

void proc_nk_semi(struct parser *p) {
	if (p->current == p->root) p->sp = 0;
}

void parse(struct parser *p, struct lexer *l, long debug) {
	long cc, r;

	if (debug) cycle_counter(&cc);
	while (1) {
		if (p->sp >= p->capacity) panic("Stack overflow!");
		lexer_next_token(&p->stack[p->sp], l);
		if (debug == 1 && p->stack[p->sp].kind <= nk_term) {
			write_str(1, "token=");
			write_num(1, p->stack[p->sp].kind);
			write_str(1, ",offset=");
			write_num(1, p->stack[p->sp].loc.off);
			write_str(1, ",value='");
			write(&r, 1, l->in + p->stack[p->sp].loc.off,
			      p->stack[p->sp].loc.len);
			write_str(1, "',line=");
			write_num(1, p->stack[p->sp].loc.line + 1);
			write_str(1, ",col=");
			write_num(1, p->stack[p->sp].loc.col + 1);
			write_str(1, "\n");
		}

		if (p->stack[p->sp].kind == nk_term)
			break;
		else {
			enum node_kind kind = p->stack[p->sp].kind;
			p->sp++;
			if (kind == nk_comment) continue;
			if (kind == nk_right_paren)
				proc_nk_right_paren(p);
			else if (kind == nk_left_paren)
				proc_nk_left_paren(p);
			else if (kind == nk_left_brace)
				proc_nk_left_brace(p);
			else if (kind == nk_right_brace)
				proc_nk_right_brace(p);
			else if (kind == nk_semi)
				proc_nk_semi(p);
		}
	}

	if (debug) {
		long cc1;
		cycle_counter(&cc1);
		cc = cc1 - cc;
		write_str(1, "parsing_cycles=");
		write_num(1, cc);
		write_str(1, "\n");
	}
}
void main(long argc, char **argv) {
	long fd, size, debug, r;
	struct lexer l;
	struct parser p;

	debug = 0;

	if (argc < 2) {
		write_str(2, "Usage: famc <file>\n");
		exit_group(-1);
	}

	if (argc > 2) {
		strcmp(&r, argv[1], "--debug");
		if (r == 0)
			debug = 1;
		else {
			strcmp(&r, argv[1], "--perf");
			if (r == 0) debug = 2;
		}
	}

	open(&fd, argv[argc - 1], 0, 0);
	if (fd < 0) {
		write_str(2, "Could not open file!\n");
		exit_group(-1);
	}

	lseek(&size, fd, 0, 2);
	if (size < 0) {
		write_str(2, "Could not determine file size!\n");
		exit_group(-1);
	}

	fmap_ro((void *)&(&l)->in, fd, size, 0);
	if (l.in == 0) {
		write_str(2, "Could not mmap file!\n");
		exit_group(-1);
	}
	close(&r, fd);

	arena_init(&p.a, 16 * 1024 * 1024, 8);
	arena_alloc((void *)&p.root, p.a, sizeof(struct node));
	p.in = l.in;
	node_init(p.root, nk_program);
	p.current = p.root;
	p.sp = 0;
	p.capacity = 4096;
	map((void *)&p.stack, sizeof(struct node) * 4096);
	if (!p.stack) panic("could not allocate stack");

	l.end = l.in + size;
	l.off = 0;
	l.col_start = 0;
	l.line = 0;

	parse(&p, &l, debug);
	if (debug == 1) node_print(p.root);

	if (debug) write_str(1, "success!\n");

	exit_group(0);
}

