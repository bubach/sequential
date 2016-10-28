#include "test.h"

#include <stdio.h>
#include <stdarg.h>

#if 0
static const char* TEST_ERR_STR[] = {
	"SEQ_ERR_NONE",
	"SEQ_ERR_MALLOC",
	"SEQ_ERR_NULL",
	"SEQ_ERR_ENUM",
	"SEQ_ERR_ARG",
	"SEQ_ERR_TODO",
	"SEQ_ERR_USER"
};

#define SEQ_QUIT_FORMAT_DEFAULT "Sequential Error (%s:%d) [%s]: %s\n", \
	__FILE__, \
	__LINE__, \
	TEST_ERR_STR[err.type], \
	(log ? log : "...")
#endif

seq_bool_t test_die(seq_t* seq, ...) {
	test_t arg = TEST_DONE;
	seq_bool_t destroy = SEQ_FALSE;
	int r = -1;

	va_list args;

	va_start(args, seq);

	while((arg = va_arg(args, test_t))) {
		if(arg == TEST_DESTROY) destroy = SEQ_TRUE;

		else if(arg == TEST_EXIT) r = va_arg(args, int);
	}

	va_end(args);

	fprintf(stderr, "ERROR!");

	if(destroy) seq_destroy(seq);

	if(r >= 0) exit(r);

	return SEQ_FALSE;
}

int main(int argc, char** argv) {
	test_copy_list();

	return 0;
}

