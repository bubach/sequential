#include "seq-api.h"

static seq_t* seq_ring_create(va_list args) {
	return NULL;
}

static void seq_ring_destroy(seq_t* seq) {
}

static seq_bool_t seq_ring_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_ring_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_ring_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_RING = {
	seq_ring_create,
	seq_ring_destroy,
	seq_ring_add,
	seq_ring_remove,
	seq_ring_get,
};

