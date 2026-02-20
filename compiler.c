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

enum token_type {
	token_type_error = 0,
	token_type_semi = 1,
	token_type_comma = 2,
	token_type_asterisk = 3,
	token_type_left_paren = 4,
	token_type_right_paren = 5,
	token_type_left_brace = 6,
	token_type_right_brace = 7,
	token_type_int = 8,
	token_type_char = 9,
	token_type_return = 10,
	token_type_ident = 11,
	token_type_num_literal = 12,
	token_type_term = 13
};

struct token {
	enum token_type type;
	long off;
	long len;
	long line_num;
	long col_num;
};

struct lexer {
	char *in;
	long off;
	long len;
	long line_num;
	long col_start;
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
#define READ_IDENT(t, l)                      \
	do {                                  \
		t->off = l->off;              \
		l->off++;                     \
		while (l->off < l->len) {     \
			int res__;            \
			IS_ALPHA(res__, *in); \
			if (!res__) break;    \
			in++;                 \
			l->off++;             \
		}                             \
		t->type = token_type_ident;   \
		t->len = l->off - t->off;     \
	} while (0);

void *memset(void *dest, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);
void *memmove(void *dest, const void *src, unsigned long n);
unsigned long strlen(const char *x);
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
	if ((res = sync_init(&global_sync)) < 0) return res;
	return (int)sync_execute(global_sync, sqe);
}

static void *fmap_ro(int fd, unsigned long size, unsigned long offset) {
	void *v = mmap(0, size, 1, 1, fd, offset);
	if (v == (void *)-1) return 0;
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

static void lexer_skip_whitespace(struct lexer *l) {
	while (l->off < l->len) {
		char ch = l->in[l->off];
		if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') return;
		l->off++;
	}
}

static void lexer_next_token(struct token *t, struct lexer *l) {
	const char *in;
	lexer_skip_whitespace(l);
	in = l->in + l->off;
	if (*in == ';') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_semi;
		l->off++;
	} else if (*in == ',') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_comma;
		l->off++;
	} else if (*in == '*') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_asterisk;
		l->off++;
	} else if (*in == '(') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_left_paren;
		l->off++;
	} else if (*in == ')') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_right_paren;
		l->off++;
	} else if (*in == '{') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_left_brace;
		l->off++;
	} else if (*in == '}') {
		t->off = l->off;
		t->len = 1;
		t->type = token_type_right_brace;
		l->off++;
	} else if (*in == 'r') {
		++in;
		if (l->off + 5 < l->len) {
			int cmp = strncmp(in, "eturn", 5);
			if (!cmp) {
				int is_match = 0;
				if (l->off + 6 < l->len) {
					NOT_ALPHA(is_match, l->in[l->off + 6]);
				} else
					is_match = 1;

				if (is_match) {
					t->off = l->off;
					t->len = 6;
					t->type = token_type_return;
					l->off += 6;
					return;
				}
			}
		}
		READ_IDENT(t, l);
	} else if (*in == 'c') {
		++in;
		if (l->off + 3 < l->len) {
			int cmp = strncmp(in, "har", 3);
			if (!cmp) {
				int is_match = 0;
				if (l->off + 4 < l->len) {
					NOT_ALPHA(is_match, l->in[l->off + 4]);
				} else
					is_match = 1;

				if (is_match) {
					t->off = l->off;
					t->len = 4;
					t->type = token_type_char;
					l->off += 4;
					return;
				}
			}
		}
		READ_IDENT(t, l);
	} else if (*in == 'i') {
		++in;
		if (l->off + 2 < l->len) {
			int cmp = strncmp(in, "nt", 2);
			if (!cmp) {
				int is_match = 0;
				if (l->off + 3 < l->len) {
					NOT_ALPHA(is_match, l->in[l->off + 3]);
				} else
					is_match = 1;

				if (is_match) {
					t->off = l->off;
					t->len = 3;
					t->type = token_type_int;
					l->off += 3;
					return;
				}
			}
		}
		READ_IDENT(t, l);
	} else if ((*in >= 'a' && *in <= 'z') || (*in >= 'A' && *in <= 'Z') ||
		   *in == '_') {
		in++;
		READ_IDENT(t, l);
	} else if (*in >= '0' && *in <= '9') {
		in++;
		t->off = l->off;
		l->off++;
		while (l->off < l->len) {
			if (*in < '0' || *in > '9') break;
			in++;
			l->off++;
		}
		t->type = token_type_num_literal;
		t->len = l->off - t->off;

	} else {
		t->type = token_type_term;
		t->line_num = l->line_num;
	}
}

int main(int argc, char **argv, char **envp) {
	struct token t = {0};
	struct lexer l;
	struct statx st;
	int fd;

	if (argc < 2) {
		write_str(2, "Usage: famc <file>\n");
		exit_group(-1);
	}

	fd = open(argv[1], 0, 0);

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
	close(fd);

	if (!l.in) {
		write_str(2, "Couldn't mmap file.\n");
		exit_group(-1);
	}

	while (1) {
		lexer_next_token(&t, &l);
		write_str(2, "token=");
		write_num(2, t.type);
		write_str(2, ",offset=");
		write_num(2, t.off);
		write_str(2, "\n");
		if (t.type == token_type_term) break;
	}

	munmap(l.in, l.len);

	return 0;

	(void)fallocate;
	(void)unlink;
	(void)write_num;
	(void)envp;
	(void)close;
}
