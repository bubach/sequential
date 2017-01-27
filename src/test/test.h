#ifndef TEST_H
#define TEST_H

#include "../sequential.h"

#include <stdio.h>

#define SEQ_TEST_BEGIN(name) seq_bool_t test_##name() { \
	seq_t seq = NULL; \
	seq_bool_t r = SEQ_TRUE; \
	printf("%s\n", #name); {

#define SEQ_TEST_END } \
done: \
	if(seq) seq_destroy(seq); \
	return r; \
}

#define SEQ_ASSERT(expr) \
	if(!(expr)) { \
		printf(" >> [FAIL] " #expr "\n"); \
		r = SEQ_FALSE; \
		goto done; \
	} \
	else printf(" >> [PASS] " #expr "\n");

#endif

