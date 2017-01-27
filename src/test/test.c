#include "test.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

SEQ_TEST_BEGIN(simple)
	SEQ_ASSERT( seq = seq_create(SEQ_NONE) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "foo\0", 4) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "bar\0", 4) )
	SEQ_ASSERT( seq_add(seq, SEQ_APPEND, "baz\0", 4) )
	SEQ_ASSERT( seq_size(seq) == 3 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 0), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 2), "baz") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, -1), "baz") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, -2), "bar") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, -3), "foo") == 0 )
	SEQ_ASSERT( seq_get(seq, SEQ_INDEX, 4) == NULL )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 1) )
	SEQ_ASSERT( seq_remove(seq, SEQ_INDEX, 2) == SEQ_FALSE )
	SEQ_ASSERT( seq_size(seq) == 2 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 0), "foo") == 0 )
	SEQ_ASSERT( strcmp(seq_get(seq, SEQ_INDEX, 1), "baz") == 0 )
SEQ_TEST_END

int main(int argc, char** argv) {
	test_simple("simple");

	return 0;
}

