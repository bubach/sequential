#include "seq-api.h"

static seq_t* seq_array_create(va_list args) {
	return NULL;
}

static void seq_array_destroy(seq_t* seq) {
}

static seq_bool_t seq_array_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_array_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_array_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_ARRAY = {
	seq_array_create,
	seq_array_destroy,
	seq_array_add,
	seq_array_remove,
	seq_array_get,
};

