#ifndef _TRACE_CMD_H
#define _TRACE_CMD_H

#include "parse-events.h"

extern int input_fd;
extern const char *input_file;

#ifndef PAGE_MASK
#define PAGE_MASK (page_size - 1)
#endif

void parse_cmdlines(char *file, int size);
void parse_proc_kallsyms(char *file, unsigned int size);
void parse_ftrace_printk(char *file, unsigned int size);

int trace_load_plugins(void);

enum {
	RINGBUF_TYPE_PADDING		= 29,
	RINGBUF_TYPE_TIME_EXTEND	= 30,
	RINGBUF_TYPE_TIME_STAMP		= 31,
};

#ifndef TS_SHIFT
#define TS_SHIFT		27
#endif

struct record {
	unsigned long long ts;
	unsigned long long offset;
	int record_size;		/* size of binary record */
	int size;			/* size of data */
	void *data;
};

struct tracecmd_handle;

struct tracecmd_handle *tracecmd_open(int fd);
int tracecmd_read_headers(struct tracecmd_handle *handle);
int tracecmd_long_size(struct tracecmd_handle *handle);
int tracecmd_page_size(struct tracecmd_handle *handle);
int tracecmd_cpus(struct tracecmd_handle *handle);

int tracecmd_init_data(struct tracecmd_handle *handle);

struct record *
tracecmd_peek_data(struct tracecmd_handle *handle, int cpu);

struct record *
tracecmd_read_data(struct tracecmd_handle *handle, int cpu);

struct record *
tracecmd_read_at(struct tracecmd_handle *handle, unsigned long long offset,
		 int *cpu);
struct record *
tracecmd_translate_data(struct tracecmd_handle *handle,
			void *ptr, int size);

int tracecmd_ftrace_overrides(void);

/* hack for function graph work around */
extern __thread struct tracecmd_handle *tracecmd_curr_thread_handle;

#endif /* _TRACE_CMD_H */