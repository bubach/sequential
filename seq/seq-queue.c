#include "seq-api.h"

static seq_t* seq_queue_create(va_list args) {
	return NULL;
}

static void seq_queue_destroy(seq_t* seq) {
}

static seq_bool_t seq_queue_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_queue_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_queue_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_QUEUE = {
	seq_queue_create,
	seq_queue_destroy,
	seq_queue_add,
	seq_queue_remove,
	seq_queue_get,
};

