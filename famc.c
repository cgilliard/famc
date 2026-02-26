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
    "    call cmain\n");
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

enum unary_op {
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

enum binary_op {
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

struct source_location {
	long off;
	long len;
	long line;
	long col;
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
	long sp;
	struct node *stack;
	long capacity;
};

void syscall(long *result, long num, long r1, long r2, long r3, long r4,
	     long r5, long r6);

void cstrlen(long *ret, char *x) {
	char *y;
	y = x;
loop:
	*x ? 0 : ({ goto end; });
	x++;
	goto loop;
end:

	*ret = x - y;
}

void cstrcmp(long *ret, char *x, char *y) {
loop:
	*x == *y && *x ? 0 : ({ goto end; });
	x++;
	y++;
	goto loop;
end:
	*x > *y ? ({ *ret = 1; }) : 0;
	*x < *y ? ({ *ret = -1; }) : 0;
	*x == *y ? ({ *ret = 0; }) : 0;
}

void cmemset(void *dest, char c, long n) {
	char *tmp;
	tmp = dest;
loop:
	n-- ? 0 : ({ goto end; });
	*tmp++ = (char)c;
	goto loop;
end:;
}

void exit_group(long status) { syscall(&status, 231, status, 0, 0, 0, 0, 0); }

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

void munmap(void *addr, long length) {
	long ign;
	syscall(&ign, 11, (long)addr, length, 0, 0, 0, 0);
}

void map(void **ret, long size) {
	long tmp;
	mmap(ret, 0, size, 3, 34, -1, 0);
	tmp = (long)*ret;
	tmp < 0 &&tmp >= -4095 ? *ret = 0 : 0;
}

void fmap_ro(void **ret, long fd, long size, long offset) {
	long tmp;
	mmap(ret, 0, size, 1, 1, fd, offset);
	tmp = (long)*ret;
	tmp < 0 &&tmp >= -4095 ? *ret = 0 : 0;
}

void panic(char *msg) {
	long r;
	cstrlen(&r, msg);
	write(&r, 2, msg, r);
	write(&r, 2, "\n", 1);
	exit_group(-1);
}

void write_str(long fd, char *msg) {
	long r;
	cstrlen(&r, msg);
	write(&r, fd, msg, r);
}

void write_num(long fd, long num) {
	char *buf;
	char *p;
	long len;
	long written;
	long neg;

	neg = 0;
	map((void *)&buf, 21);
	buf ? 0 : panic("Could not allocate memory!");

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
	num > 0 ? 0 : ({ goto end; });
	long tmp;
	tmp = num % 10;
	*--p = *"0" + tmp;
	num = num / 10;
	goto loop;
end:
	neg ? ({ *--p = *"-"; }) : 0;
	len = (buf + 20) - p;
	write(&written, fd, p, len);
	munmap(buf, 21);
}

void arena_init(struct arena **a, long size, long align) {
	struct arena *ret;
	map((void *)&ret, size + sizeof(struct arena));
	ret ? 0 : panic("Could not mmap sufficient memory! Halting.\n");
	ret->start = ((char *)(ret)) + sizeof(struct arena);
	ret->end = ret->start + size;
	(long)ret->start % align ? ({
		ret->current =
		    (char *)((long)(ret->start + (align - 1)) & ~(align - 1));
	})
				 : ({ ret->current = ret->start; });
	ret->align = align;
	*a = ret;
}

void arena_alloc(void **result, struct arena *a, long size) {
	void *ret;
	long to_alloc;
	to_alloc = (size + (a->align - 1)) & ~(a->align - 1);
	a->current > a->end - to_alloc || to_alloc > (long)a->end
	    ? panic("Could not allocate memory! Halting.\n")
	    : 0;

	ret = a->current;
	a->current += to_alloc;
	*result = ret;
}

void cmain(long argc, char **argv) {
	char arr[20];
	long i;
	i = 4;

	write_num(2, i);
	write_str(2, "\n");

	exit_group(0);

	(void)argv;
	(void)argc;
}
