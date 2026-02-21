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
	nk_left_paren,
	nk_right_paren,
	nk_goto,
	nk_if,
	nk_else,
	nk_long,
	nk_char,
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
	int sp;
	struct node stack[4096];
};

void strlen(long *ret, char *x) {
	char *y;
	long r;
	set(&y, (long)x);
begin:
	eq(&r, *x, 0);
	if (r) goto end;
	add(&x, (long)x, 1);
	goto begin;
end:
	sub(ret, (long)x, (long)y);
}

void strcmp(long *ret, char *x, char *y) {
	long res, r1, r2;
begin:
	ne(&res, *x, *y);
	if (res) goto end;
	eq(&res, *x, 0);
	if (res) goto end;
	add(&x, (long)x, 1);
	add(&y, (long)y, 1);
	goto begin;
end:
	and(&r1, *x, 255);
	and(&r2, *y, 255);
	sub(ret, r1, r2);
}

void strncmp(long *ret, char *x, char *y, long n) {
	long res, r1, r2;
begin:
	lte(&res, n, 0);
	if (res) goto end;
	ne(&res, *x, *y);
	if (res) goto end;
	eq(&res, *x, 0);
	if (res) goto end;
	add(&x, (long)x, 1);
	add(&y, (long)y, 1);
	sub(&n, n, 1);
	goto begin;
end:
	eq(&res, n, 0);
	if (res)
		set(ret, 0);
	else {
		and(&r1, *x, 255);
		and(&r2, *y, 255);
		sub(ret, r1, r2);
	}
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
	long v;
	negate(&v, 1);
	mmap(ret, 0, size, 3, 34, v, 0);
	lt(&v, (long)*ret, 0);
	if (v) set(ret, 0);
}

void fmap_ro(void **ret, long fd, long size, long offset) {
	long v;
	mmap(ret, 0, size, 1, 1, fd, offset);
	lt(&v, (long)*ret, 0);
	if (v) set(ret, 0);
}

void fmap_exe(void **ret, long fd, long size, long offset) {
	long v;
	mmap(ret, 0, size, 5, 2, fd, offset);
	lt(&v, (long)*ret, 0);
	if (v) set(ret, 0);
}

void write_str(long *ret, long fd, char *msg) {
	strlen(ret, msg);
	write(ret, fd, msg, *ret);
}

void panic(char *msg) {
	long len, ret;
	strlen(&len, msg);
	write(&len, 2, msg, len);
	write(&len, 2, "\n", 1);
	negate(&ret, 1);
	exit_group(ret);
}

void write_num(long *result, long fd, long num) {
	char buf[21];
	char *p;
	long len, written, negative, res;

	set(&negative, 0);
	add(&p, (long)buf, 20);
	set8(p, 0);

	lt(&res, num, 0);
	if (res) {
		set(&negative, 1);
		negate(&num, num);
	}

	eq(&res, num, 0);
	if (res) {
		sub(&p, (long)p, 1);
		set8(p, *"0");
	} else {
	begin:
		lte(&res, num, 0);
		if (res) goto end;
		sub(&p, (long)p, 1);
		mod(&res, num, 10);
		add(&res, res, *"0");
		set8(p, res);
		div(&num, num, 10);
		goto begin;
	}
end:
	if (negative) {
		sub(&p, (long)p, 1);
		set8(p, *"-");
	}

	add(&len, (long)buf, 20);
	sub(&len, len, (long)p);
	write(&written, fd, p, len);

	eq(&res, written, len);
	if (res)
		set(result, 0);
	else
		negate(result, 1);
}

void lexer_next_token_tmp(struct node *next, struct lexer *l) {
	long ch;
	char *in = l->in + l->off;
begin:
	ch = (*in & 0xFF) - 9;
	if (ch > 5) goto end;
	++in;
	goto begin;
end:
	l->off = in - l->in;
	next->type = 0;
}

void lexer_next_token(struct node *next, struct lexer *l) {
	long r1, r2;
	char *in;
	add(&in, l->in, l->off);

begin_whitespace:
	eq(&r1, in, l->end);
	if (r1) goto end_whitespace;
	set(&r2, 0);
	eq(&r1, *in, *" ");
	or (&r2, r2, r1);
	eq(&r1, *in, *"\n");
	or (&r2, r2, r1);
	eq(&r1, *in, *"\r");
	or (&r2, r2, r1);
	eq(&r1, *in, *"\t");
	or (&r2, r2, r1);
	eq(&r1, *in, *"\v");
	or (&r2, r2, r1);
	eq(&r1, *in, *"\f");
	or (&r2, r2, r1);
	eq(&r2, r2, 0);
	if (r2) goto end_whitespace;
	add(&in, in, 1);
	goto begin_whitespace;
end_whitespace:
	sub(&l->off, in, l->in);

	eq(&r1, *in, *";");
	if (r1) {
		set(&(&next->loc)->off, l->off);
		set(&(&next->loc)->len, 1);
		set(&next->type, nk_semi);
		add(&l->off, l->off, 1);
		goto end_lexer;
	}
	eq(&r1, *in, *":");
	if (r1) {
		set(&(&next->loc)->off, l->off);
		set(&(&next->loc)->len, 1);
		set(&next->type, nk_colon);
		add(&l->off, l->off, 1);
		goto end_lexer;
	}
	eq(&r1, *in, *"(");
	if (r1) {
		set(&(&next->loc)->off, l->off);
		set(&(&next->loc)->len, 1);
		set(&next->type, nk_left_paren);
		add(&l->off, l->off, 1);
		goto end_lexer;
	}

	set(&next->type, nk_term);
	set(&(&next->loc)->line, l->line);
end_lexer:;
}

void parse(struct parser *p, struct lexer *l, long debug) {
	long r1;
	struct node next;

	set(&(&next)->type, 0);
begin_lexer:
	write_str(&r1, 2, "next=");
	write_num(&r1, 2, (&next)->type);
	write_str(&r1, 2, "\n");
	if ((&next)->type == nk_term) goto end_lexer;
	lexer_next_token(&next, l);
	goto begin_lexer;
end_lexer:

	(void)p;
	(void)debug;
}

void main(long argc, char **argv) {
	long fd, size, debug, r;
	struct lexer l;
	struct parser p;

	set(&debug, 0);

	if (argc < 2) {
		write_str(&r, 2, "Usage: famc <file>\n");
		exit_group(-1);
	}

	if (argc > 2) {
		strcmp(&r, argv[1], "--debug");
		eq(&r, r, 0);
		if (r)
			set(&debug, 1);
		else {
			strcmp(&r, argv[1], "--perf");
			eq(&r, r, 0);
			if (r) set(&debug, 2);
		}
	}

	open(&fd, argv[argc - 1], 0, 0);
	lt(&r, fd, 0);
	if (r) {
		write_str(&r, 2, "Could not open file!\n");
		exit_group(-1);
	}

	lseek(&size, fd, 0, 2);
	lt(&r, size, 0);
	if (r) {
		write_str(&r, 2, "Could not determine file size!\n");
		exit_group(-1);
	}

	fmap_ro((void *)&(&l)->in, fd, size, 0);
	eq(&r, (&l)->in, 0);
	if (r) {
		write_str(&r, 2, "Could not mmap file!\n");
		exit_group(-1);
	}
	close(&r, fd);

	add(&(&l)->end, (&l)->in, size);
	set(&(&l)->off, 0);
	set(&(&l)->col_start, 0);
	set(&(&l)->line, 0);

	parse(&p, &l, debug);
	exit_group(fd);
}
