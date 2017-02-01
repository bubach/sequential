#include "test.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef struct _test_t {
	int i;
	float f;
	double d;
	char s[64];
} test_t;

test_t* test_create(int i, float f, double d, const char* s) {
	test_t* t = (test_t*)(malloc(sizeof(test_t)));

	t->i = i;
	t->f = f;
	t->d = d;

	strncpy(t->s, s, 64);

	return t;
}

const char* test_strings[] = {
	"foo", "Foo", "FOO",
	"bar", "Bar", "BAR",
	"baz", "Baz", "BAZ",
	"qux", "Qux", "QUX"
};

const seq_size_t test_strings_size = 12;

void test_strings_append(seq_t seq) {
	seq_size_t i;

	for(i = 0; i < test_strings_size; i++) seq_add(seq, SEQ_APPEND, test_strings[i]);
}

/* ============================================================================================= */
SEQ_TEST_BEGIN(add_append_prepend)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "baz") )
	SEQ_ASSERT( seq_add(seq, SEQ_PREPEND, "foo") )
	SEQ_ASSERT( seq_size(seq) == 3 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 0), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 2), "baz") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, -1), "baz") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, -2), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, -3), "foo") == 0 )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 1) )
	SEQ_ASSERT( seq_size(seq) == 2 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 0), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "baz") == 0 )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_before)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "FOO") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAR") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAZ") )
	SEQ_ASSERT( seq_add(seq, SEQ_BEFORE, SEQ_INDEX, 1, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_BEFORE, SEQ_INDEX, 0, "foo") )
	SEQ_ASSERT( seq_add(seq, SEQ_BEFORE, SEQ_INDEX, -1, "baz") )
	SEQ_ASSERT( seq_size(seq) == 6 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 0), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "FOO") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 2), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 3), "BAR") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 4), "baz") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 5), "BAZ") == 0 )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_after)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "FOO") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAR") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAZ") )
	SEQ_ASSERT( seq_add(seq, SEQ_AFTER, SEQ_INDEX, 1, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_AFTER, SEQ_INDEX, 0, "foo") )
	SEQ_ASSERT( seq_add(seq, SEQ_AFTER, SEQ_INDEX, -1, "baz") )
	SEQ_ASSERT( seq_size(seq) == 6 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 3), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 5), "baz") == 0 )
SEQ_TEST_END

SEQ_TEST_BEGIN(add_replace)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "FOO") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAR") )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "BAZ") )
	SEQ_ASSERT( seq_add(seq, SEQ_REPLACE, SEQ_INDEX, 0, "foo") )
	SEQ_ASSERT( seq_add(seq, SEQ_REPLACE, SEQ_INDEX, 1, "bar") )
	SEQ_ASSERT( seq_add(seq, SEQ_REPLACE, SEQ_INDEX, 2, "baz") )
	SEQ_ASSERT( seq_size(seq) == 3 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 0), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 2), "baz") == 0 )
SEQ_TEST_END

/* ============================================================================================= */
SEQ_TEST_BEGIN(add_remove_copy)
	int i;
	int* ints = (int*)(malloc(sizeof(int) * 100));

	SEQ_ASSERT( seq_set(seq, SEQ_ADD_COPY) )
	SEQ_ASSERT( seq_set(seq, SEQ_REMOVE_FREE) )

	for(i = 0; i < 100; i++) ints[i] = i;

	SEQ_ASSERT( ints[10] == 10 )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, ints, sizeof(int) * 100) )

	free(ints);

	SEQ_ASSERT( ((int*)(seq_get(seq, SEQ_INDEX, 0)))[10] == 10 )
	SEQ_ASSERT( ((int*)(seq_get(seq, SEQ_INDEX, 0)))[21] == 21 )
	SEQ_ASSERT( ((int*)(seq_get(seq, SEQ_INDEX, 0)))[99] == 99 )
SEQ_TEST_END

/* ============================================================================================= */
seq_data_t on_add(seq_args_t args) {
	unsigned long foo = seq_arg(args, unsigned long);
	unsigned long bar = seq_arg(args, unsigned long);
	unsigned long baz = seq_arg(args, unsigned long);

	return (seq_data_t)((foo << 24) | (bar << 16) | baz);
}

void on_remove(seq_data_t data) {
	unsigned long d = (unsigned long)(data);

	test_info("on_remove(%lu, %lu, %lu)", (d >> 24) & 0xFF, (d >> 16) & 0xFF, d & 0xFF);
}

SEQ_TEST_BEGIN(on_add_remove)
	SEQ_ASSERT( seq_set(seq, SEQ_ON_ADD, on_add) )
	SEQ_ASSERT( seq_set(seq, SEQ_ON_REMOVE, on_remove) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, 10, 20, 30) )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 0) )
SEQ_TEST_END

/* ============================================================================================= */
SEQ_TEST_BEGIN(add_errors)
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "foo") )
	SEQ_ASSERT( !seq_add(seq, "bar") )
	SEQ_ASSERT( !seq_add(seq, SEQ_APPEND, NULL) )
	SEQ_ASSERT( !seq_add(seq, SEQ_SEND, "baz") )
	SEQ_ASSERT( !seq_add(seq, SEQ_ADD, "qux") )
	SEQ_ASSERT( !seq_remove(seq, SEQ_INDEX, 1) )
	SEQ_ASSERT( !seq_remove(seq, SEQ_INDEX, -3) )
SEQ_TEST_END

/* ============================================================================================= */
SEQ_TEST_BEGIN(iterate)
	test_strings_append(seq);

	{
		seq_iter_t i = seq_iter_create(seq, SEQ_STRIDE, 3, SEQ_RANGE, 3, 9, SEQ_NONE);

		while(seq_iterate(i)) {
			seq_size_t index = seq_iter_index(i);
			seq_data_t data = seq_iter_data(i);

			test_info(
				"index=%d data=%s test_strings[%d]=%s",
				index,
				(char*)(data),
				index,
				test_strings[index]
			);

			SEQ_ASSERT( strcmp(data, test_strings[index]) == 0 )
		}

		seq_iter_destroy(i);
	}
SEQ_TEST_END

/* ============================================================================================= */
int main(int argc, char** argv) {
	test_add_append_prepend("SEQ_APPEND / SEQ_PREPEND");
	test_add_before("SEQ_BEFORE");
	test_add_after("SEQ_AFTER");
	test_add_replace("SEQ_REPLACE");
	test_add_remove_copy("SEQ_ADD_COPY / SEQ_REMOVE_FREE");
	test_on_add_remove("SEQ_ON_ADD / SEQ_ON_REMOVE");
	test_add_errors("SEQ_ADD (ERRORS)");
	test_iterate("SEQ_STRIDE / SEQ_RANGE");

	return 0;
}

