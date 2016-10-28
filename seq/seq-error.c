#include "seq-api.h"

static seq_t* seq_error_create(va_list args) {
	return NULL;
}

static void seq_error_destroy(seq_t* seq) {
}

static seq_bool_t seq_error_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_error_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_error_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_ERROR = {
	seq_error_create,
	seq_error_destroy,
	seq_error_add,
	seq_error_remove,
	seq_error_get,
};

