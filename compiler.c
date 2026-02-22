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

struct lexer {
	char *in;
	char *end;
	long off;
	long line;
	long col_start;
};

struct arena {
	char *start;
	char *current;
	char *end;
	long align;
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
	nk_term
};

struct source_location {
	long off;
	long len;
	long line;
	long col;
};

struct node {
	enum node_kind type;
	struct source_location loc;
	struct parse_node *parent;
	struct parse_node *first_child;
	struct parse_node *last_child;
	struct parse_node *next_sibling;
	struct parse_node *prev_sibling;
	void *node_data;
};

struct parser {
	struct arena *a;
	struct node *root;
	struct node *current;
	long sp;
	struct node stack[4096];
};

void strlen(long *ret, const char *x) {
	const char *y = x;
	while (*x) x++;
	*ret = x - y;
}

void strcmp(long *ret, const char *x, const char *y) {
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

void write_str(long *res, long fd, char *msg) {
	long r;
	strlen(&r, msg);
	write(res, fd, msg, r);
}

void write_num(long *ret, long fd, long num) {
	char buf[21];
	char *p;
	long len;
	long written;
	char negative = 0;

	if (fd < 0) {
		*ret = -1;
		goto end;
	}
	p = buf + sizeof(buf) - 1;
	*p = *"\0";

	if (num < 0) {
		negative = 1;
		if (num == ((long)(-0x7FFFFFFFFFFFFFFF - 1))) {
			char min_str[] = "-9223372036854775808";
			len = sizeof(min_str) - 1;
			write(&written, fd, min_str, len);
			if (written < 0) {
				*ret = written;
				goto end;
			}
			if ((long)written != len) {
				*ret = -1;
				goto end;
			}
			*ret = 0;
			goto end;
		}
		num = -num;
	}

	if (num == 0)
		*--p = *"0";
	else
		while (num > 0) {
			long tmp;
			mod(&tmp, num, 10);
			*--p = *"0" + tmp;
			div(&num, num, 10);
		}

	if (negative) *--p = *"-";
	len = (buf + sizeof(buf) - 1) - p;
	write(&written, fd, p, len);
	if (written < 0) {
		*ret = written;
		goto end;
	}
	if ((long)written != len)
		*ret = -1;
	else
		*ret = 0;
end:;
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
		next->type = nk_term;
	} else if (*in == *";") {
		next->loc.len = 1;
		next->type = nk_semi;
		l->off++;
	} else if (*in == *",") {
		next->loc.len = 1;
		next->type = nk_comma;
		l->off++;
	} else if (*in == *":") {
		next->loc.len = 1;
		next->type = nk_colon;
		l->off++;
	} else if (*in == *"*") {
		next->loc.len = 1;
		next->type = nk_asterisk;
		l->off++;
	} else if (*in == *"|") {
		if (++in != l->end && *in == *"|") {
			next->loc.len = 2;
			next->type = nk_double_pipe;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->type = nk_pipe;
			l->off++;
		}
	} else if (*in == *".") {
		next->loc.len = 1;
		next->type = nk_dot;
		l->off++;
	} else if (*in == *"&") {
		if (++in != l->end && *in == *"&") {
			next->loc.len = 2;
			next->type = nk_double_ampersand;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->type = nk_ampersand;
			l->off++;
		}
	} else if (*in == *"=") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->type = nk_double_equal;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->type = nk_equal;
			l->off++;
		}
	} else if (*in == *"(") {
		next->loc.len = 1;
		next->type = nk_left_paren;
		l->off++;
	} else if (*in == *")") {
		next->loc.len = 1;
		next->type = nk_right_paren;
		l->off++;
	} else if (*in == *"{") {
		next->loc.len = 1;
		next->type = nk_left_brace;
		l->off++;
	} else if (*in == *"}") {
		next->loc.len = 1;
		next->type = nk_right_brace;
		l->off++;
	} else if (*in == *"[") {
		next->loc.len = 1;
		next->type = nk_left_bracket;
		l->off++;
	} else if (*in == *"]") {
		next->loc.len = 1;
		next->type = nk_right_bracket;
		l->off++;
	} else if (*in == *"-") {
		if (++in != l->end) {
			if (*in == *"=") {
				next->loc.len = 2;
				next->type = nk_hyphen_equal;
				l->off += 2;
				goto end;
			} else if (*in == *"-") {
				next->loc.len = 2;
				next->type = nk_double_hyphen;
				l->off += 2;
				goto end;
			} else if (*in == *">") {
				next->loc.len = 2;
				next->type = nk_arrow;
				l->off += 2;
				goto end;
			}
		}
		next->loc.len = 1;
		next->type = nk_hyphen;
		l->off++;
	} else if (*in == *"+") {
		if (++in != l->end) {
			if (*in == *"=") {
				next->loc.len = 2;
				next->type = nk_plus_equal;
				l->off += 2;
				goto end;
			} else if (*in == *"+") {
				next->loc.len = 2;
				next->type = nk_double_plus;
				l->off += 2;
				goto end;
			}
		}
		next->loc.len = 1;
		next->type = nk_plus;
		l->off++;
	} else if (*in == *"<") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->type = nk_lte;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->type = nk_lt;
			l->off++;
		}
	} else if (*in == *">") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->type = nk_gte;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->type = nk_gt;
			l->off++;
		}
	} else if (*in == *"!") {
		if (++in != l->end && *in == *"=") {
			next->loc.len = 2;
			next->type = nk_ne;
			l->off += 2;
		} else {
			next->loc.len = 1;
			next->type = nk_bang;
			l->off++;
		}
	} else if (*in == *"\"") {
		while (++in != l->end) {
			if (*in == *"\" ") {
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
			next->type = nk_str_lit;
			l->off += next->loc.len;
		} else {
			next->loc.len = in - (l->in + l->off);
			next->type = nk_error;
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
			next->type = nk_error;
			l->off += next->loc.len;
		} else {
			next->loc.len = in - (l->in + l->off) + 1;
			next->type = nk_comment;
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
		next->type = nk_asm;
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
		next->type = nk_break;
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
		next->type = nk_char;
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
				next->type = nk_enum;
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
		next->type = nk_else;
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
		next->type = nk_goto;
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
		next->type = nk_char;
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
		next->type = nk_long;
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
				next->type = nk_struct;
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
		next->type = nk_sizeof;
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
		next->type = nk_void;
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
		next->type = nk_while;
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
			next->type = nk_ident;
			l->off += next->loc.len;
			goto end;
		} else {
			ch1 = (*in - *"0") & 0xFF;

			if (ch1 < 10) {
				if (in + 1 != l->end && *(in + 1) == *"x" &&
				    *in == *"0") {
					in++;
				}
				while (++in != l->end) {
					ch1 = (*in - *"0") & 0xFF;
					if (ch1 >= 10) break;
				}
				next->loc.len = (long)(in - (l->in + l->off));
				next->type = nk_num_lit;
				l->off += next->loc.len;
				goto end;
			}
		}
	error:
		next->loc.len = 1;
		next->type = nk_error;
		l->off++;
	}
end:;
}

void parse(struct parser *p, struct lexer *l, long debug) {
	long r;
	long cc;
	struct node next;
	(void)p;

	next.type = -1;
	if (debug) cycle_counter(&cc);
	while (1) {
		if (debug == 1 && next.type <= nk_term) {
			write_str(&r, 1, "token=");
			write_num(&r, 1, next.type);
			write_str(&r, 1, ",offset=");
			write_num(&r, 1, next.loc.off);
			write_str(&r, 1, ",value='");
			write(&r, 1, l->in + next.loc.off, next.loc.len);
			write_str(&r, 1, "',line=");
			write_num(&r, 1, next.loc.line + 1);
			write_str(&r, 1, ",col=");
			write_num(&r, 1, next.loc.col + 1);
			write_str(&r, 1, "\n");
		}

		if (next.type == nk_term) break;
		lexer_next_token(&next, l);
	}
	if (debug) {
		long cc1;
		cycle_counter(&cc1);
		cc = cc1 - cc;
		write_str(&r, 1, "lexing_cycles=");
		write_num(&r, 1, cc);
		write_str(&r, 1, "\n");
	}
}

void main(long argc, char **argv) {
	long fd, size, debug, r;
	struct lexer l;
	struct parser p;

	debug = 0;

	if (argc < 2) {
		write_str(&r, 2, "Usage: famc <file>\n");
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
		write_str(&r, 2, "Could not open file!\n");
		exit_group(-1);
	}

	lseek(&size, fd, 0, 2);
	if (size < 0) {
		write_str(&r, 2, "Could not determine file size!\n");
		exit_group(-1);
	}

	fmap_ro((void *)&(&l)->in, fd, size, 0);
	if (l.in == 0) {
		write_str(&r, 2, "Could not mmap file!\n");
		exit_group(-1);
	}
	close(&r, fd);

	l.end = l.in + size;
	l.off = 0;
	l.col_start = 0;
	l.line = 0;

	parse(&p, &l, debug);
	exit_group(0);
}
