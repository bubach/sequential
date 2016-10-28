#include "seq-api.h"

static seq_t* seq_test_create(va_list args) {
	return NULL;
}

static void seq_test_destroy(seq_t* seq) {
}

static seq_bool_t seq_test_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_test_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_test_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_TEST = {
	seq_test_create,
	seq_test_destroy,
	seq_test_add,
	seq_test_remove,
	seq_test_get,
};

