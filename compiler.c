#ifndef __x86_64__
#error "Unsupported platform!"
#endif

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
    "    call main\n"
    "    mov %rax, %rdi\n"
    "    jmp exit_group\n");

__asm__(
    ".section .text\n"
    ".global syscall\n"
    "syscall:\n"
    "    movq %rdi, %rax\n"
    "    movq %rsi, %rdi\n"
    "    movq %rdx, %rsi\n"
    "    movq %rcx, %rdx\n"
    "    movq %r8,  %r10\n"
    "    movq %r9,  %r8\n"
    "    movq 8(%rsp), %r9\n"
    "    syscall\n"
    "    ret\n");
__asm(
    ".section .text\n"
    "atomic_add_unsigned :\n"
    "lock add %esi,(%rdi)\n"
    "ret\n");

__asm(
    ".section .text\n"
    "atomic_sub_unsigned :\n"
    "lock sub %esi,(%rdi)\n"
    "ret\n");

void atomic_add_unsigned(unsigned *ptr, unsigned value);
void atomic_sub_unsigned(unsigned *ptr, unsigned value);
long syscall(long sysno, long a0, long a1, long a2, long a3, long a4, long a5);

struct statx_timestamp {
	long tv_sec;
	unsigned tv_nsec;
	int __reserved;
};

struct statx {
	unsigned stx_mask;
	unsigned stx_blksize;
	unsigned long stx_attributes;
	unsigned stx_nlink;
	unsigned stx_uid;
	unsigned stx_gid;
	unsigned short stx_mode;
	unsigned short __spare0[1];
	unsigned long stx_ino;
	unsigned long stx_size;
	unsigned long stx_blocks;
	unsigned long stx_attributes_mask;
	struct statx_timestamp stx_atime;
	struct statx_timestamp stx_btime;
	struct statx_timestamp stx_ctime;
	struct statx_timestamp stx_mtime;
	unsigned stx_rdev_major;
	unsigned stx_rdev_minor;
	unsigned stx_dev_major;
	unsigned stx_dev_minor;
	unsigned long stx_mnt_id;
	unsigned stx_dio_mem_align;
	unsigned stx_dio_offset_align;
	unsigned long __spare3[12];
};

struct io_cqring_offsets {
	unsigned head;
	unsigned tail;
	unsigned ring_mask;
	unsigned ring_entries;
	unsigned overflow;
	unsigned cqes;
	unsigned flags;
	unsigned resv1;
	unsigned long user_addr;
};

struct io_sqring_offsets {
	unsigned head;
	unsigned tail;
	unsigned ring_mask;
	unsigned ring_entries;
	unsigned flags;
	unsigned dropped;
	unsigned array;
	unsigned resv1;
	unsigned long user_addr;
};

struct io_uring_params {
	unsigned sq_entries;
	unsigned cq_entries;
	unsigned flags;
	unsigned sq_thread_cpu;
	unsigned sq_thread_idle;
	unsigned features;
	unsigned wq_fd;
	unsigned resv[3];
	struct io_sqring_offsets sq_off;
	struct io_cqring_offsets cq_off;
};

struct io_uring_cqe {
	unsigned long user_data;
	int res;
	unsigned flags;
};

struct io_uring_sqe {
	unsigned char opcode;
	unsigned char flags;
	unsigned short ioprio;
	int fd;
	unsigned long off;
	unsigned long addr;
	unsigned len;
	unsigned rw_flags;
	unsigned long user_data;
	unsigned short buf_index;
	unsigned short personality;
	int splice_fd_in;
	unsigned long _reserved[2];
};

struct open_how {
	unsigned long flags;
	unsigned long mode;
	unsigned long resolve;
};

struct Sync {
	struct io_uring_params params;
	int ring_fd;
	unsigned char *sq_ring;
	unsigned char *cq_ring;
	struct io_uring_sqe *sqes;
	struct io_uring_cqe *cqes;
	unsigned long sq_ring_size;
	unsigned long cq_ring_size;
	unsigned long sqes_size;
	unsigned *sq_tail;
	unsigned *sq_array;
	unsigned *cq_head;
	unsigned *cq_tail;
	unsigned *sq_mask;
	unsigned *cq_mask;
};

struct lexer {
	char *in;
	char *end;
	unsigned long off;
	unsigned long len;
	unsigned long line_num;
	unsigned long col_start;
};

struct arena {
	unsigned char *start;
	unsigned char *current;
	unsigned char *end;
	unsigned long align;
};

enum node_kind {
	token_type_error,
	token_type_semi,
	token_type_comma,
	token_type_asterisk,
	token_type_equal,
	token_type_left_paren,
	token_type_right_paren,
	token_type_left_brace,
	token_type_right_brace,
	token_type_left_bracket,
	token_type_right_bracket,
	token_type_minus,
	token_type_bang,
	token_type_percent,
	token_type_ampersand,
	token_type_plus,
	token_type_dot,
	token_type_div,
	token_type_colon,
	token_type_less_than,
	token_type_greater_than,
	token_type_question_mark,
	token_type_caret,
	token_type_pipe,
	token_type_tilde,
	token_type_pound,
	token_type_backslash,
	token_type_arrow,
	token_type_plus_plus,
	token_type_minus_minus,
	token_type_shift_left,
	token_type_shift_right,
	token_type_less_than_equal,
	token_type_greater_than_equal,
	token_type_double_equal,
	token_type_not_equal,
	token_type_double_ampersand,
	token_type_double_pipe,
	token_type_plus_equal,
	token_type_minus_equal,
	token_type_mul_equal,
	token_type_div_equal,
	token_type_percent_equal,
	token_type_shift_left_assign,
	token_type_shift_right_assign,
	token_type_ampersand_equal,
	token_type_caret_equal,
	token_type_pipe_equal,
	token_type_ellipsis,
	token_type_double_pound,
	token_type_auto,
	token_type_break,
	token_type_case,
	token_type_char,
	token_type_const,
	token_type_continue,
	token_type_default,
	token_type_do,
	token_type_double,
	token_type_else,
	token_type_enum,
	token_type_extern,
	token_type_float,
	token_type_for,
	token_type_goto,
	token_type_if,
	token_type_int,
	token_type_long,
	token_type_return,
	token_type_short,
	token_type_signed,
	token_type_sizeof,
	token_type_static,
	token_type_struct,
	token_type_switch,
	token_type_typedef,
	token_type_union,
	token_type_unsigned,
	token_type_void,
	token_type_volatile,
	token_type_while,
	token_type_ident,
	token_type_num_literal,
	token_type_char_literal,
	token_type_string_literal,
	token_type_comment,
	node_kind_program,
	node_kind_func_def,
	node_kind_decl_var,
	node_kind_type_basic,
	node_kind_decl_param_list,
	node_kind_decl_param,
	node_kind_type_pointer,
	node_kind_stmt_compound,
	node_kind_stmt_return,
	token_type_term
};

struct source_location {
	unsigned long off;
	unsigned long len;
	unsigned long line;
	unsigned long col;
};

struct func_def_data {
	struct parse_node *return_type;
	struct parse_node *name;
	struct parse_node *params;
	struct parse_node *body;
};

struct decl_var_data {
	struct parse_node *type;
	struct parse_node *name;
	struct parse_node *init;
	int storage_class;
	int qualifiers;
};

struct type_basic_data {
	int basic_type;
	int qualifiers;
	int signedness;
};

struct decl_param_data {
	struct parse_node *type;
	struct parse_node *name;
};

struct type_pointer_data {
	struct parse_node *base_type;
	int qualifiers;
	int levels;
};

struct parse_node {
	enum node_kind type;
	struct source_location loc;
	struct parse_node *parent;
	struct parse_node *first_child;
	struct parse_node *next_sibling;
	void *node_data;
};

#define PARSER_STACK_MAX 128

struct parser {
	struct parser_node *root;
	struct parser_node *current;
	int sp;
	struct parse_node stack[PARSER_STACK_MAX];
};

#define EPERM 1	   /* Operation not permitted */
#define ENOENT 2   /* No such file or directory */
#define ESRCH 3	   /* No such process */
#define EINTR 4	   /* Interrupted system call */
#define EIO 5	   /* I/O error */
#define ENXIO 6	   /* No such device or address */
#define E2BIG 7	   /* Argument list too long */
#define ENOEXEC 8  /* Exec format error */
#define EBADF 9	   /* Bad file descriptor */
#define ECHILD 10  /* No child processes */
#define EAGAIN 11  /* Resource temporarily unavailable */
#define ENOMEM 12  /* Out of memory */
#define EACCES 13  /* Permission denied */
#define EFAULT 14  /* Bad address */
#define ENOTBLK 15 /* Block device required */
#define EBUSY 16   /* Device or resource busy */
#define EEXIST 17  /* File exists */
#define EXDEV 18   /* Invalid cross-device link */
#define ENODEV 19  /* No such device */
#define ENOTDIR 20 /* Not a directory */
#define EISDIR 21  /* Is a directory */
#define EINVAL 22  /* Invalid argument */
#define ENFILE 23  /* File table overflow */
#define EMFILE 24  /* Too many open files */
#define ENOTTY 25  /* Inappropriate ioctl for device */
#define ETXTBSY 26 /* Text file busy */
#define EFBIG 27   /* File too large */
#define ENOSPC 28  /* No space left on device */

#define IS_ALPHA(ret, ch)                                                 \
	do {                                                              \
		ret = 0;                                                  \
		if (ch == '_' || (ch >= 'a' && ch <= 'z') ||              \
		    (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) \
			ret = 1;                                          \
	} while (0);
#define NOT_ALPHA(ret, ch)         \
	do {                       \
		IS_ALPHA(ret, ch); \
		ret = !ret;        \
	} while (0);
#define IS_NUM(ret, ch, hex, bin)                              \
	do {                                                   \
		ret = 0;                                       \
		if (ch >= '0' && ch <= '9')                    \
			ret = 1;                               \
		else if (hex && ((ch >= 'a' && ch <= 'f') ||   \
				 (ch >= 'A' && ch <= 'F')))    \
			ret = 1;                               \
		if (bin && !(ch == '0' || ch == '1')) ret = 0; \
	} while (0);

#define READ_IDENT(t, l)                              \
	do {                                          \
		const char *in__ = l->in + l->off;    \
		while (++in__ != l->end) {            \
			int res__;                    \
			IS_ALPHA(res__, *in__);       \
			if (!res__) break;            \
		}                                     \
		t->type = token_type_ident;           \
		t->loc.len = in__ - (l->in + l->off); \
		t->loc.off = l->off;                  \
		l->off += t->loc.len;                 \
	} while (0);
#define READ_NUMBER(t, l)                                       \
	do {                                                    \
		int counter__ = 0, hex__ = 0, res__, bin__ = 0; \
		const char *in__ = l->in + l->off;              \
		while (++in__ != l->end) {                      \
			if (counter__ == 0) {                   \
				if (*in__ == 'x') {             \
					hex__ = 1;              \
					counter__++;            \
					continue;               \
				} else if (*in__ == 'b') {      \
					bin__ = 1;              \
					counter__++;            \
					continue;               \
				}                               \
			}                                       \
			IS_NUM(res__, *in__, hex__, bin__);     \
			if (!res__) {                           \
				break;                          \
			}                                       \
			counter__++;                            \
		}                                               \
		t->loc.len = in__ - (l->in + l->off);           \
		if ((hex__ || bin__) && t->loc.len <= 2)        \
			t->type = token_type_error;             \
		else                                            \
			t->type = token_type_num_literal;       \
		t->loc.off = l->off;                            \
		l->off += t->loc.len;                           \
	} while (0);
#define SET_MATCH(t, l, mlen, mtype) \
	do {                         \
		t->loc.off = l->off; \
		t->loc.len = mlen;   \
		t->type = mtype;     \
		l->off += mlen;      \
		return;              \
	} while (0);

#define LEXER_SKIP_WHITESPACE(l)                                              \
	do {                                                                  \
		const char *in__ = l->in + l->off;                            \
		while (in__ != l->end) {                                      \
			if (*in__ != ' ' && *in__ != '\t' && *in__ != '\r' && \
			    *in__ != '\n' && *in__ != '\v' && *in__ != '\f')  \
				break;                                        \
			if (*in__ == '\n') {                                  \
				l->col_start = (in__ - l->in) + 1;            \
				l->line_num++;                                \
			}                                                     \
			in__++;                                               \
		}                                                             \
		l->off += in__ - (l->in + l->off);                            \
	} while (0);

void *memset(void *dest, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);
void *memmove(void *dest, const void *src, unsigned long n);
unsigned long strlen(const char *x);
int strcmp(const char *x, const char *y);
int strncmp(const char *x, const char *y, unsigned long n);
void exit_group(int status);

struct Sync *global_sync = 0;

void *memset(void *dest, int c, unsigned long n) {
	unsigned char *tmp = dest;
	while (n--) *tmp++ = (unsigned char)c;
	return dest;
}

void *memcpy(void *dest, const void *src, unsigned long n) {
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const void *)src;
	while (n--) *d++ = *s++;
	return dest;
}

void *memmove(void *dest, const void *src, unsigned long n) {
	unsigned char *d = (void *)((unsigned char *)dest + n);
	const unsigned char *s = (const void *)((const unsigned char *)src + n);
	while (n--) d--, s--, *d = *s;
	return dest;
}

unsigned long strlen(const char *x) {
	const char *y = x;
	while (*x) x++;
	return x - y;
}

int strcmp(const char *x, const char *y) {
	while (*x == *y && *x) x++, y++;
	return *x > *y ? 1 : *y > *x ? -1 : 0;
}

int strncmp(const char *x, const char *y, unsigned long n) {
	while (n > 0 && *x == *y && *x) x++, y++, n--;
	if (n == 0) return 0;
	return (char)*x - (char)*y;
}

void exit_group(int status) { syscall(231, status, 0, 0, 0, 0, 0); }

static void *mmap(void *addr, unsigned long length, int prot, int flags, int fd,
		  long offset) {
	return (void *)syscall(9, (long)addr, (long)length, prot, flags, fd,
			       offset);
}

static int munmap(void *addr, unsigned long len) {
	return (int)syscall(11, (long)addr, (long)len, 0, 0, 0, 0);
}

static int io_uring_setup(unsigned entries, struct io_uring_params *params) {
	return (int)syscall(425, entries, (long)params, 0, 0, 0, 0);
}

static int io_uring_enter2(unsigned fd, unsigned to_submit,
			   unsigned min_complete, unsigned flags, void *arg,
			   unsigned long sz) {
	return (int)syscall(426, fd, to_submit, min_complete, flags, (long)arg,
			    (long)sz);
}

static void sync_destroy(struct Sync *sync) {
	if (sync) {
		if (sync->sq_ring) munmap(sync->sq_ring, sync->sq_ring_size);
		sync->sq_ring = 0;
		if (sync->cq_ring) munmap(sync->cq_ring, sync->cq_ring_size);
		sync->cq_ring = 0;
		if (sync->sqes) munmap(sync->sqes, sync->sqes_size);
		sync->sqes = 0;
		sync->ring_fd = -1;

		munmap(sync, sizeof(struct Sync));
	}
}

static int sync_init(struct Sync **s) {
	struct Sync *sync;

	sync = mmap(0, sizeof(struct Sync), 3, 33, -1, 0);
	if ((long)sync < 0) {
		return -1;
	}

	sync->sq_ring = 0;
	sync->cq_ring = 0;
	sync->sqes = 0;
	sync->ring_fd = io_uring_setup(1, &sync->params);
	if (sync->ring_fd < 0) {
		sync_destroy(sync);
		return sync->ring_fd;
	}

	sync->sq_ring_size = sync->params.sq_off.array +
			     sync->params.sq_entries * sizeof(unsigned);
	sync->cq_ring_size =
	    sync->params.cq_off.cqes +
	    sync->params.cq_entries * sizeof(struct io_uring_cqe);
	sync->sqes_size = sync->params.sq_entries * sizeof(struct io_uring_sqe);

	sync->sq_ring = mmap(0, sync->sq_ring_size, 3, 1, sync->ring_fd, 0);
	if ((long)sync->sq_ring < 0) {
		sync->sq_ring = 0;
		sync_destroy(sync);
		return -ENOMEM;
	}

	sync->cq_ring =
	    mmap(0, sync->cq_ring_size, 3, 1, sync->ring_fd, 134217728);

	if ((long)sync->cq_ring < 0) {
		sync->cq_ring = 0;
		sync_destroy(sync);
		return -ENOMEM;
	}
	sync->sqes = mmap(0, sync->sqes_size, 3, 1, sync->ring_fd, 268435456);
	if ((long)sync->sqes < 0) {
		sync->sqes = 0;
		sync_destroy(sync);
		return -ENOMEM;
	}

	sync->sq_tail = (unsigned *)(sync->sq_ring + sync->params.sq_off.tail);
	sync->sq_array =
	    (unsigned *)(sync->sq_ring + sync->params.sq_off.array);
	sync->cq_head = (unsigned *)(sync->cq_ring + sync->params.cq_off.head);
	sync->cq_tail = (unsigned *)(sync->cq_ring + sync->params.cq_off.tail);
	sync->sq_mask =
	    (unsigned *)(sync->sq_ring + sync->params.sq_off.ring_mask);

	sync->cq_mask =
	    (unsigned *)(sync->cq_ring + sync->params.cq_off.ring_mask);
	sync->cqes =
	    (struct io_uring_cqe *)(sync->cq_ring + sync->params.cq_off.cqes);

	*s = sync;
	return 0;
}

static long sync_execute(struct Sync *sync, struct io_uring_sqe sqe) {
	long ret;
	unsigned cq_mask, sq_mask, sq_tail, index, cq_head, idx;

	cq_mask = *sync->cq_mask;
	sq_mask = *sync->sq_mask;
	sq_tail = *sync->sq_tail;
	index = sq_tail & sq_mask;
	cq_head = *sync->cq_head;
	sync->sq_array[index] = index;
	sync->sqes[index] = sqe;

	atomic_add_unsigned(sync->sq_tail, 1);
	ret = io_uring_enter2((unsigned)sync->ring_fd, 1, 1, 1, 0, 0);

	if (ret < 1) {
		atomic_sub_unsigned(sync->sq_tail, 1);
	} else {
		idx = cq_head & cq_mask;
		ret = sync->cqes[idx].res;
		atomic_add_unsigned(sync->cq_head, 1);
	}

	return ret;
}

static long pwrite(int fd, const void *buf, unsigned len, long offset) {
	int res;
	struct io_uring_sqe sqe = {0};

	if (fd < 0 || !buf || offset < 0) return -EINVAL;

	sqe.opcode = 23;
	sqe.addr = (unsigned long)buf;
	sqe.fd = fd;
	sqe.len = len;
	sqe.off = (unsigned long)offset;
	sqe.user_data = 1;

	if (!global_sync)
		if ((res = sync_init(&global_sync)) < 0) return res;
	return sync_execute(global_sync, sqe);
}

static int open(const char *path, int flags, unsigned mode) {
	int res;
	struct open_how how;
	struct io_uring_sqe sqe = {0};

	how.flags = (unsigned)flags;
	how.mode = mode;
	how.resolve = 0;
	sqe.opcode = 28;
	sqe.addr = (unsigned long)path;
	sqe.fd = -100;
	sqe.len = sizeof(struct open_how);
	sqe.off = (unsigned long)&how;
	sqe.user_data = 1;

	if (!global_sync)
		if ((res = sync_init(&global_sync)) < 0) return res;
	return (int)sync_execute(global_sync, sqe);
}

static int close(int fd) {
	int res;
	struct io_uring_sqe sqe = {0};

	sqe.opcode = 19;
	sqe.fd = fd;
	sqe.user_data = 1;

	if (!global_sync)
		if ((res = sync_init(&global_sync)) < 0) return res;
	return (int)sync_execute(global_sync, sqe);
}

static int fallocate(int fd, unsigned long new_size) {
	int res;
	struct io_uring_sqe sqe = {0};
	sqe.opcode = 17;
	sqe.fd = fd;
	sqe.addr = new_size;
	sqe.user_data = 1;
	if (!global_sync)
		if ((res = sync_init(&global_sync)) < 0) return res;
	return (int)sync_execute(global_sync, sqe);
}

static int fstatx(int fd, struct statx *st) {
	int res;
	struct io_uring_sqe sqe = {0};
	sqe.opcode = 21;
	sqe.fd = fd;
	sqe.addr = (unsigned long)"";
	sqe.len = 0x07ffU;
	sqe.off = (unsigned long)st;
	sqe.rw_flags = 0x1000;
	sqe.user_data = 1;
	if (!global_sync)
		if ((res = sync_init(&global_sync)) < 0) return 0;
	return (int)sync_execute(global_sync, sqe);
}

static int unlink(const char *pathname) {
	int res;
	struct io_uring_sqe sqe = {0};
	sqe.opcode = 36;
	sqe.fd = -100;
	sqe.addr = (unsigned long)pathname;
	sqe.user_data = 1;
	if (!global_sync)
		if ((res = sync_init(&global_sync)) < 0) return res;
	return (int)sync_execute(global_sync, sqe);
}

static void *fmap_ro(int fd, unsigned long size, unsigned long offset) {
	void *v = mmap(0, size, 1, 1, fd, offset);
	if ((long)v < 0) return 0;
	return v;
}

static void *map(unsigned long length) {
	void *v = mmap(0, length, 3, 0x22, -1, 0);
	if ((long)v < 0) return 0;
	return v;
}

static long write_num(int fd, long num) {
	unsigned char buf[21];
	unsigned char *p;
	unsigned long len;
	long written;
	int negative = 0;

	if (fd < 0) return fd;
	p = buf + sizeof(buf) - 1;
	*p = '\0';

	if (num < 0) {
		negative = 1;
		if (num == ((long)(-0x7FFFFFFFFFFFFFFF - 1))) {
			unsigned char min_str[] = "-9223372036854775808";
			len = sizeof(min_str) - 1;
			written = pwrite(fd, min_str, len, 0);
			if (written < 0) return written;
			if ((unsigned long)written != len) return -EIO;
			return 0;
		}
		num = -num;
	}

	if (num == 0)
		*--p = '0';
	else
		while (num > 0) {
			*--p = '0' + (num % 10);
			num /= 10;
		}

	if (negative) *--p = '-';
	len = (buf + sizeof(buf) - 1) - p;
	written = pwrite(fd, p, len, 0);
	if (written < 0) return written;
	if ((unsigned long)written != len) return -EIO;
	return 0;
}

static int write_str(int fd, char *s) {
	long written;
	unsigned long current = 0;
	unsigned long len = strlen(s);
	while (current < len) {
		written = pwrite(fd, s, len - current, 0);
		if (written < 1) return -1;
		current += written;
	}
	return 0;
}

static unsigned long cycle_counter(void) {
	unsigned lo, hi;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((unsigned long)hi << 32) | lo;
}

static void lexer_next_token(struct parse_node *t, struct lexer *l) {
	int is_alpha;
	const char *in;
	LEXER_SKIP_WHITESPACE(l);
	in = l->in + l->off;
	t->loc.line = l->line_num;
	t->loc.col = l->off - l->col_start;

	if (*in == ';') {
		SET_MATCH(t, l, 1, token_type_semi);
	} else if (*in == '!') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_not_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_bang);
	} else if (*in == '%') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_percent_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_percent);
	} else if (*in == '&') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '&') {
				SET_MATCH(t, l, 2, token_type_double_ampersand);
			} else if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_ampersand_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_ampersand);
	} else if (*in == '.') {
		if (l->end - in >= 2) {
			if (*(in + 1) == '.' && *(in + 2) == '.') {
				SET_MATCH(t, l, 3, token_type_ellipsis);
			}
		}
		SET_MATCH(t, l, 1, token_type_dot);
	} else if (*in == '/') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '*') {
				while (++in != l->end) {
					if (*in == '\n') {
						l->col_start = (in - l->in) + 1;
						l->line_num++;
					}
					if (*in == '/' && *(in - 1) == '*')
						break;
				}
				if (in == l->end) {
					SET_MATCH(t, l, in - (l->in + l->off),
						  token_type_error);
				}
				++in;
				SET_MATCH(t, l, in - (l->in + l->off),
					  token_type_comment);
			}
			if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_div_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_div);
	} else if (*in == ':') {
		SET_MATCH(t, l, 1, token_type_colon);
	} else if (*in == '<') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '<') {
				if (in + 2 != l->end && *(in + 2) == '=') {
					SET_MATCH(
					    t, l, 3,
					    token_type_shift_right_assign);
				}
				SET_MATCH(t, l, 2, token_type_shift_right);
			} else if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_less_than_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_less_than);
	} else if (*in == '>') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '>') {
				if (in + 2 != l->end && *(in + 2) == '=') {
					SET_MATCH(t, l, 3,
						  token_type_shift_left_assign);
				}
				SET_MATCH(t, l, 2, token_type_shift_left);
			} else if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2,
					  token_type_greater_than_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_greater_than);
	} else if (*in == '^') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_caret_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_caret);
	} else if (*in == '|') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '|') {
				SET_MATCH(t, l, 2, token_type_double_pipe);
			} else if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_pipe_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_pipe);
	} else if (*in == '~') {
		SET_MATCH(t, l, 1, token_type_tilde);
	} else if (*in == '#') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '#') {
				SET_MATCH(t, l, 2, token_type_double_pound);
			}
		}
		SET_MATCH(t, l, 1, token_type_pound);
	} else if (*in == ',') {
		SET_MATCH(t, l, 1, token_type_comma);
	} else if (*in == '*') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_mul_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_asterisk);
	} else if (*in == '=') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_double_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_equal);
	} else if (*in == '\\') {
		SET_MATCH(t, l, 1, token_type_backslash);
	} else if (*in == '[') {
		SET_MATCH(t, l, 1, token_type_left_bracket);
	} else if (*in == ']') {
		SET_MATCH(t, l, 1, token_type_right_bracket);
	} else if (*in == '(') {
		SET_MATCH(t, l, 1, token_type_left_paren);
	} else if (*in == ')') {
		SET_MATCH(t, l, 1, token_type_right_paren);
	} else if (*in == '{') {
		SET_MATCH(t, l, 1, token_type_left_brace);
	} else if (*in == '}') {
		SET_MATCH(t, l, 1, token_type_right_brace);
	} else if (*in == '+') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '+') {
				SET_MATCH(t, l, 2, token_type_plus_plus);
			} else if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_plus_equal);
			}
		}
		SET_MATCH(t, l, 1, token_type_plus);
	} else if (*in == '-') {
		if (in + 1 != l->end) {
			if (*(in + 1) == '-') {
				SET_MATCH(t, l, 2, token_type_minus_minus);
			} else if (*(in + 1) == '=') {
				SET_MATCH(t, l, 2, token_type_minus_equal);
			} else if (*(in + 1) == '>') {
				SET_MATCH(t, l, 2, token_type_arrow);
			}
		}
		SET_MATCH(t, l, 1, token_type_minus);
	} else if (*in == 'a') {
		if (++in == l->end || *in != 'u') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in == l->end || *in != 'o') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 4, token_type_auto);
	} else if (*in == 'b') {
		if (++in == l->end || *in != 'r') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in == l->end || *in != 'a') goto ident;
		if (++in == l->end || *in != 'k') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 5, token_type_break);
	} else if (*in == 'c') {
		if (++in == l->end || *in != 'h') {
			if (in != l->end && *in == 'a') {
				if (++in == l->end || *in != 's') goto ident;
				if (++in == l->end || *in != 'e') goto ident;
				if (++in != l->end) {
					IS_ALPHA(is_alpha, *in);
					if (is_alpha) goto ident;
				}
				SET_MATCH(t, l, 4, token_type_case);
			} else if (in != l->end && *in == 'o') {
				if (++in == l->end || *in != 'n') goto ident;
				if (++in == l->end || *in != 's') {
					if (*in != 't') goto ident;
					if (++in == l->end || *in != 'i')
						goto ident;
					if (++in == l->end || *in != 'n')
						goto ident;
					if (++in == l->end || *in != 'u')
						goto ident;
					if (++in == l->end || *in != 'e')
						goto ident;
					if (++in != l->end) {
						IS_ALPHA(is_alpha, *in);
						if (is_alpha) goto ident;
					}
					SET_MATCH(t, l, 8, token_type_continue);
				}
				if (++in == l->end || *in != 't') goto ident;
				if (++in != l->end) {
					IS_ALPHA(is_alpha, *in);
					if (is_alpha) goto ident;
				}
				SET_MATCH(t, l, 5, token_type_const);
			}
			goto ident;
		}
		if (++in == l->end || *in != 'a') goto ident;
		if (++in == l->end || *in != 'r') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 4, token_type_char);
	} else if (*in == 'd') {
		if (++in == l->end) goto ident;
		if (*in != 'e') {
			if (*in != 'o') goto ident;
			if (++in == l->end) {
				SET_MATCH(t, l, 2, token_type_do);
			}
			IS_ALPHA(is_alpha, *in);
			if (!is_alpha) {
				SET_MATCH(t, l, 2, token_type_do);
			}
			if (*in != 'u') goto ident;
			if (++in == l->end || *in != 'b') goto ident;
			if (++in == l->end || *in != 'l') goto ident;
			if (++in == l->end || *in != 'e') goto ident;
			if (++in != l->end) {
				IS_ALPHA(is_alpha, *in);
				if (is_alpha) goto ident;
			}
			SET_MATCH(t, l, 6, token_type_double);
		}
		if (++in == l->end || *in != 'f') goto ident;
		if (++in == l->end || *in != 'a') goto ident;
		if (++in == l->end || *in != 'u') goto ident;
		if (++in == l->end || *in != 'l') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 7, token_type_default);
	} else if (*in == 'e') {
		if (++in == l->end || *in != 'l') {
			if (in == l->end)
				goto ident;
			else if (*in == 'n') {
				if (++in == l->end || *in != 'u') goto ident;
				if (++in == l->end || *in != 'm') goto ident;
				if (++in != l->end) {
					IS_ALPHA(is_alpha, *in);
					if (is_alpha) goto ident;
				}
				SET_MATCH(t, l, 4, token_type_enum);
			} else if (*in == 'x') {
				if (++in == l->end || *in != 't') goto ident;
				if (++in == l->end || *in != 'e') goto ident;
				if (++in == l->end || *in != 'r') goto ident;
				if (++in == l->end || *in != 'n') goto ident;
				if (++in != l->end) {
					IS_ALPHA(is_alpha, *in);
					if (is_alpha) goto ident;
				}
				SET_MATCH(t, l, 6, token_type_extern);
			} else
				goto ident;
		}
		if (++in == l->end || *in != 's') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 4, token_type_else);
	} else if (*in == 'f') {
		if (++in == l->end || *in != 'l') {
			if (in == l->end || *in != 'o') goto ident;
			if (++in == l->end || *in != 'r') goto ident;
			if (++in != l->end) {
				IS_ALPHA(is_alpha, *in);
				if (is_alpha) goto ident;
			}
			SET_MATCH(t, l, 3, token_type_for);
		}
		if (++in == l->end || *in != 'o') goto ident;
		if (++in == l->end || *in != 'a') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 5, token_type_float);
	} else if (*in == 'g') {
		if (++in == l->end || *in != 'o') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in == l->end || *in != 'o') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 4, token_type_goto);
	} else if (*in == 'i') {
		if (++in == l->end || *in != 'n') {
			if (in == l->end || *in != 'f') goto ident;
			if (++in != l->end) {
				IS_ALPHA(is_alpha, *in);
				if (is_alpha) goto ident;
			}
			SET_MATCH(t, l, 2, token_type_if);
		}
		if (++in == l->end || *in != 't') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 3, token_type_int);
	} else if (*in == 'l') {
		if (++in == l->end || *in != 'o') goto ident;
		if (++in == l->end || *in != 'n') goto ident;
		if (++in == l->end || *in != 'g') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 4, token_type_long);
	} else if (*in == 'r') {
		if (++in == l->end || *in != 'e') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in == l->end || *in != 'u') goto ident;
		if (++in == l->end || *in != 'r') goto ident;
		if (++in == l->end || *in != 'n') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 6, token_type_return);
	} else if (*in == 's') {
		if (++in == l->end || *in != 'w') {
			if (in == l->end) goto ident;
			if (*in == 'i') {
				if (++in == l->end || *in != 'g') {
					if (in == l->end) goto ident;
					if (*in != 'z') goto ident;
					if (++in == l->end || *in != 'e')
						goto ident;
					if (++in == l->end || *in != 'o')
						goto ident;
					if (++in == l->end || *in != 'f')
						goto ident;
					if (++in != l->end) {
						IS_ALPHA(is_alpha, *in);
						if (is_alpha) goto ident;
					}
					SET_MATCH(t, l, 6, token_type_sizeof);
				}
				if (++in == l->end || *in != 'n') goto ident;
				if (++in == l->end || *in != 'e') goto ident;
				if (++in == l->end || *in != 'd') goto ident;
				if (++in != l->end) {
					IS_ALPHA(is_alpha, *in);
					if (is_alpha) goto ident;
				}
				SET_MATCH(t, l, 6, token_type_signed);
			} else if (*in == 't') {
				if (++in == l->end || *in != 'r') {
					if (in == l->end) goto ident;
					if (*in != 'a') goto ident;
					if (++in == l->end || *in != 't')
						goto ident;
					if (++in == l->end || *in != 'i')
						goto ident;
					if (++in == l->end || *in != 'c')
						goto ident;
					if (++in != l->end) {
						IS_ALPHA(is_alpha, *in);
						if (is_alpha) goto ident;
					}
					SET_MATCH(t, l, 6, token_type_static);
				}
				if (++in == l->end || *in != 'u') goto ident;
				if (++in == l->end || *in != 'c') goto ident;
				if (++in == l->end || *in != 't') goto ident;
				if (++in != l->end) {
					IS_ALPHA(is_alpha, *in);
					if (is_alpha) goto ident;
				}
				SET_MATCH(t, l, 6, token_type_struct);
			}
			if (*in != 'h') goto ident;
			if (++in == l->end || *in != 'o') goto ident;
			if (++in == l->end || *in != 'r') goto ident;
			if (++in == l->end || *in != 't') goto ident;
			if (++in != l->end) {
				IS_ALPHA(is_alpha, *in);
				if (is_alpha) goto ident;
			}
			SET_MATCH(t, l, 5, token_type_short);
		}
		if (++in == l->end || *in != 'i') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in == l->end || *in != 'c') goto ident;
		if (++in == l->end || *in != 'h') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 6, token_type_switch);
	} else if (*in == 't') {
		if (++in == l->end || *in != 'y') goto ident;
		if (++in == l->end || *in != 'p') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in == l->end || *in != 'd') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in == l->end || *in != 'f') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 7, token_type_typedef);
	} else if (*in == 'u') {
		if (++in == l->end || *in != 'n') goto ident;
		if (++in == l->end || *in != 's') {
			if (in == l->end || *in != 'i') goto ident;
			if (++in == l->end || *in != 'o') goto ident;
			if (++in == l->end || *in != 'n') goto ident;
			if (++in != l->end) {
				IS_ALPHA(is_alpha, *in);
				if (is_alpha) goto ident;
			}
			SET_MATCH(t, l, 5, token_type_union);
		}
		if (++in == l->end || *in != 'i') goto ident;
		if (++in == l->end || *in != 'g') goto ident;
		if (++in == l->end || *in != 'n') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in == l->end || *in != 'd') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 8, token_type_unsigned);
	} else if (*in == 'v') {
		if (++in == l->end || *in != 'o') goto ident;
		if (++in == l->end || *in != 'l') {
			if (in == l->end || *in != 'i') goto ident;
			if (++in == l->end || *in != 'd') goto ident;
			if (++in != l->end) {
				IS_ALPHA(is_alpha, *in);
				if (is_alpha) goto ident;
			}
			SET_MATCH(t, l, 4, token_type_void);
		}
		if (++in == l->end || *in != 'a') goto ident;
		if (++in == l->end || *in != 't') goto ident;
		if (++in == l->end || *in != 'i') goto ident;
		if (++in == l->end || *in != 'l') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 8, token_type_volatile);
	} else if (*in == 'w') {
		if (++in == l->end || *in != 'h') goto ident;
		if (++in == l->end || *in != 'i') goto ident;
		if (++in == l->end || *in != 'l') goto ident;
		if (++in == l->end || *in != 'e') goto ident;
		if (++in != l->end) {
			IS_ALPHA(is_alpha, *in);
			if (is_alpha) goto ident;
		}
		SET_MATCH(t, l, 5, token_type_while);
	} else if (*in == '\"') {
		while (++in != l->end)
			if (*in == '\"' && *(in - 1) != '\\')
				break;
			else if (*in == '\n') {
				l->col_start = (in - l->in) + 1;
				l->line_num++;
			}
		if (in != l->end) {
			in++;
			SET_MATCH(t, l, in - (l->in + l->off),
				  token_type_string_literal);
		} else {
			SET_MATCH(t, l, in - (l->in + l->off),
				  token_type_error);
		}
	} else if (*in == '\'') {
		int match_len = 3;
		if (++in == l->end) {
			SET_MATCH(t, l, 1, token_type_error);
		}
		if (*in == '\'') {
			SET_MATCH(t, l, 2, token_type_error);
		} else if (*in == '\\') {
			if (++in == l->end) {
				SET_MATCH(t, l, 2, token_type_error);
			}
			if (*in != '\'' && *in != '\"' && *in != '?' &&
			    *in != '\\' && *in != 'a' && *in != 'b' &&
			    *in != 'f' && *in != 'n' && *in != 'r' &&
			    *in != 't' && *in != 'v' && *in != '0') {
				while (in != l->end) {
					if (*in == '\'') break;
					if (*in == '\n') {
						l->col_start = (in - l->in) + 1;
						l->line_num++;
					}
					++in;
				}
				++in;
				SET_MATCH(t, l, in - (l->in + l->off),
					  token_type_error);
			}
			match_len = 4;
		} else if (*in == '\n') {
			l->col_start = (in - l->in) + 1;
			l->line_num++;
		}
		if (++in == l->end || *in != '\'') {
			while (in != l->end) {
				if (*in == '\'') break;
				if (*in == '\n') {
					l->col_start = (in - l->in) + 1;
					l->line_num++;
				}
				++in;
			}
			++in;
			SET_MATCH(t, l, in - (l->in + l->off),
				  token_type_error);
		}
		SET_MATCH(t, l, match_len, token_type_char_literal);
	} else if ((*in >= 'a' && *in <= 'z') || (*in >= 'A' && *in <= 'Z') ||
		   *in == '_') {
	ident:
		READ_IDENT(t, l);
	} else if (*in >= '0' && *in <= '9') {
		READ_NUMBER(t, l);
	} else if (in >= l->end) {
		t->type = token_type_term;
		t->loc.len = 0;
		t->loc.off = l->end - l->in;
		t->loc.line = l->line_num;
	} else {
		SET_MATCH(t, l, 1, token_type_error);
	}
}

static int arena_init(struct arena **a, unsigned long size,
		      unsigned long align) {
	struct arena *ret;
	if (size == 0 || align == 0 || (align & (align - 1)) != 0)
		return -EINVAL;
	if (align > (~0U)) return -EINVAL;
	if (size + sizeof(struct arena) < size) return -ENOMEM;
	ret = map(size + sizeof(struct arena));
	if (!ret) return -ENOMEM;
	ret->start = ((unsigned char *)(ret)) + sizeof(struct arena);
	ret->end = ret->start + size;
	ret->current =
	    ((unsigned long)ret->start % align) == 0
		? ret->start
		: (unsigned char *)((unsigned long)(ret->start + (align - 1)) &
				    ~(align - 1));
	ret->align = align;
	*a = ret;
	return 0;
}

static void *arena_alloc(struct arena *a, unsigned long size) {
	void *ret;
	unsigned long to_alloc;
	to_alloc = (size + (a->align - 1)) & ~(a->align - 1);
	if (a->current > a->end - to_alloc || to_alloc > (unsigned long)a->end)
		return 0;
	ret = a->current;
	a->current += to_alloc;
	return ret;
}

int main(int argc, char **argv, char **envp) {
	struct arena *a = 0;
	struct parse_node t = {0};
	void *ast;
	struct lexer l;
	struct statx st;
	int debug = 0;
	int fd;
	unsigned long cc;

	if (argc < 2) {
		write_str(2, "Usage: famc <file>\n");
		exit_group(-1);
	}

	if (argc > 2) {
		if (!strcmp(argv[1], "--debug"))
			debug = 1;
		else if (!strcmp(argv[1], "--perf"))
			debug = 2;
	}

	if (arena_init(&a, 1024 * 1024 * 16, 8) < 0) {
		write_str(2, "Could not allocate arena.\n");
		exit_group(-1);
	}

	ast = arena_alloc(a, 8);
	(void)ast;

	fd = open(argv[argc - 1], 0, 0);

	if (fd < 0) {
		write_str(2, "Could not open file.\n");
		exit_group(-1);
	}

	if (fstatx(fd, &st) < 0) {
		write_str(2, "Couldn't obtain size of file.\n");
		exit_group(-1);
	}

	l.len = st.stx_size;
	l.off = l.col_start = l.line_num = 0;
	l.in = fmap_ro(fd, l.len, 0);
	l.end = l.in + l.len;
	close(fd);

	if (!l.in) {
		write_str(2, "Couldn't mmap file.\n");
		exit_group(-1);
	}

	if (debug) cc = cycle_counter();
	while (t.type != token_type_term) {
		lexer_next_token(&t, &l);
		if (debug == 1) {
			write_str(1, "token=");
			write_num(1, t.type);
			write_str(1, ",offset=");
			write_num(1, t.loc.off);
			write_str(1, ",value='");
			pwrite(1, l.in + t.loc.off, t.loc.len, 0);
			write_str(1, "',line=");
			write_num(1, t.loc.line + 1);
			write_str(1, ",col=");
			write_num(1, t.loc.col);
			write_str(1, "\n");
		}
	}
	if (debug) {
		cc = cycle_counter() - cc;
		write_str(1, "lexer cycles=");
		write_num(1, cc);
		write_str(1, "\n");
	}

	return 0;

	(void)fallocate;
	(void)unlink;
	(void)envp;
	(void)close;
}
