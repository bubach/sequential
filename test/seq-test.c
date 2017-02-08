#include "seq-test.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct _test_t {
	int i;
	float f;
	double d;
	char s[64];
} test_t;

static test_t* test_create(int i, float f, double d, const char* s) {
	test_t* t = (test_t*)(malloc(sizeof(test_t)));

	t->i = i;
	t->f = f;
	t->d = d;

	strncpy(t->s, s, 64);

	return t;
}

/* static void test_info(test_t* t) {
	test_printf("test_t (%p) {", t);
	test_printf("  i = %d", t->i);
	test_printf("  f = %f", t->f);
	test_printf("  d = %f", t->d);
	test_printf("  s = %s", t->s);
	test_printf("}");
} */

const seq_size_t test_strings_size = 12;
const char* test_strings[] = {
	"foo", "Foo", "FOO",
	"bar", "Bar", "BAR",
	"baz", "Baz", "BAZ",
	"qux", "Qux", "QUX"
};

void test_strings_append(seq_t seq) {
	seq_size_t i;

	for(i = 0; i < test_strings_size; i++) seq_add(seq, SEQ_APPEND, test_strings[i]);
}

SEQ_TEST_BEGIN(add_append_prepend)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "baz") )
	SEQ_ASSERT( seq_add(seq, SEQ_PREPEND, "foo") )
	SEQ_ASSERT( seq_size(seq) == 3 )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 0), "foo" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 1), "bar" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 2), "baz" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, -1), "baz" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, -2), "bar" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, -3), "foo" )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 1) )
	SEQ_ASSERT( seq_size(seq) == 2 )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 0), "foo" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 1), "baz" )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_before)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "FOO") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAR") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAZ") )
	SEQ_ASSERT( seq_add(seq, SEQ_BEFORE, SEQ_INDEX, 1, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_BEFORE, SEQ_INDEX, 0, "foo") )
	SEQ_ASSERT( seq_add(seq, SEQ_BEFORE, SEQ_INDEX, -1, "baz") )
	SEQ_ASSERT( seq_size(seq) == 6 )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 0), "foo" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 1), "FOO" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 2), "bar" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 3), "BAR" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 4), "baz" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 5), "BAZ" )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_after)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "FOO") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAR") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAZ") )
	SEQ_ASSERT( seq_add(seq, SEQ_AFTER, SEQ_INDEX, 1, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_AFTER, SEQ_INDEX, 0, "foo") )
	SEQ_ASSERT( seq_add(seq, SEQ_AFTER, SEQ_INDEX, -1, "baz") )
	SEQ_ASSERT( seq_size(seq) == 6 )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 1), "foo" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 3), "bar" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 4), "baz" )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_replace)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "FOO") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAR") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAZ") )
	SEQ_ASSERT( seq_add(seq, SEQ_REPLACE, SEQ_INDEX, 0, "foo") )
	SEQ_ASSERT( seq_add(seq, SEQ_REPLACE, SEQ_INDEX, 1, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_REPLACE, SEQ_INDEX, 2, "baz") )
	SEQ_ASSERT( seq_size(seq) == 3 )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 0), "foo" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 1), "bar" )
	SEQ_ASSERT_STRCMP( seq_get(seq, SEQ_INDEX, 2), "baz" )
SEQ_TEST_END

SEQ_TEST_BEGIN(remove_free)
	test_t* t = test_create(1, 2.2f, 33.33, "FOUR");

	SEQ_ASSERT( seq_set(seq, SEQ_CB_REMOVE_FREE) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, t) )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 0) )
	SEQ_ASSERT( seq_size(seq) == 0)
SEQ_TEST_END

seq_data_t on_add(seq_args_t args) {
	unsigned long foo = seq_arg(args, unsigned long);
	unsigned long bar = seq_arg(args, unsigned long);
	unsigned long baz = seq_arg(args, unsigned long);

	return (seq_data_t)((foo << 24) | (bar << 16) | baz);
}

void on_remove(seq_data_t data) {
	unsigned long d = (unsigned long)(data);

	test_printf("on_remove(%lu, %lu, %lu)", (d >> 24) & 0xFF, (d >> 16) & 0xFF, d & 0xFF);
}

SEQ_TEST_BEGIN(on_add_remove)
	SEQ_ASSERT( seq_set(seq, SEQ_CB_ADD, on_add) )
	SEQ_ASSERT( seq_set(seq, SEQ_CB_REMOVE, on_remove) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, 10, 20, 30) )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 0) )
SEQ_TEST_END

seq_data_t on_add_addr(seq_args_t args) {
	return seq_arg_data(args);
}

void on_remove_addr(seq_data_t data) {
	seq_data_t* d = (seq_data_t*)(data);

	free(*d);

	*d = NULL;
}

SEQ_TEST_BEGIN(on_add_remove_addr)
	test_t* t = test_create(1, 2.2f, 33.33, "FOUR");

	SEQ_ASSERT( seq_set(seq, SEQ_CB_ADD, on_add_addr) )
	SEQ_ASSERT( seq_set(seq, SEQ_CB_REMOVE, on_remove_addr) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, &t) )
	SEQ_ASSERT( t != NULL )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 0) )
	SEQ_ASSERT( t == NULL )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_errors)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "foo") )
	SEQ_ASSERT( !seq_add(seq, "bar") )
	SEQ_ASSERT( !seq_add(seq, SEQ_APPEND, NULL) )
	/* SEQ_ASSERT( !seq_add(seq, SEQ_SEND, "baz") )
	SEQ_ASSERT( !seq_add(seq, SEQ_ADD, "qux") ) */
	SEQ_ASSERT( !seq_remove(seq, SEQ_INDEX, 1) )
	SEQ_ASSERT( !seq_remove(seq, SEQ_INDEX, -3) )
SEQ_TEST_END

SEQ_TEST_BEGIN(iterate)
	test_strings_append(seq);

	{
		seq_iter_t i = seq_iter_create(seq, SEQ_INC, 3, SEQ_RANGE, 3, 9, SEQ_FALSE);

		while(seq_iterate(i)) {
			seq_get_t get = seq_iter_get(i, SEQ_DATA);

			test_printf(
				"index=%d data=%s test_strings[%d]=%s",
				get.handle.index,
				get.data,
				get.handle.index,
				test_strings[get.handle.index]
			);

			SEQ_ASSERT_STRCMP(get, test_strings[get.handle.index] )
		}

		seq_iter_destroy(i);
	}
SEQ_TEST_END

typedef struct _test_func_t {
	void (*func)(const char*);
	const char* name;
	const char* descr;
} test_func_t;

#define TEST_FUNC(name, descr) { test_##name, #name, #descr }

const seq_size_t test_funcs_size = 9;
const test_func_t test_funcs[] = {
	TEST_FUNC(add_append_prepend, "SEE_APPEND / SEQ_PREPEND"),
	TEST_FUNC(add_before, "SEQ_BEFORE"),
	TEST_FUNC(add_after, "SEQ_AFTER"),
	TEST_FUNC(add_replace, "SEQ_REPLACE"),
	TEST_FUNC(remove_free, "SEQ_REMOVE_FREE"),
	TEST_FUNC(on_add_remove, "SEQ_CB_ADD / SEQ_CB_REMOVE"),
	TEST_FUNC(on_add_remove_addr, "SEQ_CB_ADD / SEQ_CB_REMOVE"),
	TEST_FUNC(add_errors, "SEQ_ADD (ERRORS)"),
	TEST_FUNC(iterate, "SEQ_INC / SEQ_RANGE"),
};

static seq_bool_t run_tests(const char* str, seq_size_t index, seq_bool_t all) {
	seq_bool_t any = SEQ_FALSE;
	seq_size_t i;

	for(i = 0; i < test_funcs_size; i++) {
		const test_func_t* tf = &test_funcs[i];

		if(all || i == index || (str && strstr(tf->name, str))) {
			tf->func(tf->descr);

			any = SEQ_TRUE;
		}
	}

	return any;
}

int main(int argc, char** argv) {
	if(argc == 1) {
		int t;

		printf(
			"Specify one or more test names to run, separated by\n"
			"spaces. Using substrings is permitted, as well as the\n"
			"special keyword 'all' (which runs every test).\n"
			"Alternatively, you can use the numeric index indicated\n"
			"in the list below.\n"
			"\n"
			"Available tests are:\n\n"
		);

		for(t = 0; t < test_funcs_size; t++) {
			const test_func_t* tf = &test_funcs[t];

			printf("   %02d: %s (%s)\n", t + 1, tf->name, tf->descr);
		}

		printf("\nSequential by: Jeremy Moles (cubicool@gmail.com)\n");

		return 1;
	}

	else if(argc == 2 && !strcmp(argv[1], "all")) run_tests(NULL, -1, SEQ_TRUE);

	else {
		int a;

		for(a = 1; a < argc; a++) {
			const char* str = argv[a];
			seq_bool_t any = run_tests(str, atoi(str) - 1, SEQ_FALSE);

			if(!any) printf("No tests matched your request '%s'.\n", str);
		}
	}

	return 0;
}

