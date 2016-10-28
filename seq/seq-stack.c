#include "seq-api.h"

static seq_t* seq_stack_create(va_list args) {
	return NULL;
}

static void seq_stack_destroy(seq_t* seq) {
}

static seq_bool_t seq_stack_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_stack_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_stack_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_STACK = {
	seq_stack_create,
	seq_stack_destroy,
	seq_stack_add,
	seq_stack_remove,
	seq_stack_get,
};

