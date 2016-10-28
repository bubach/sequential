#ifndef TEST_H
#define TEST_H

#include "../sequential.h"

typedef enum _test_t {
	TEST_DONE,
	TEST_EXIT,
	TEST_DESTROY
} test_t;

seq_bool_t test_die(seq_t* seq, ...);

seq_bool_t test_copy_list();

#endif

