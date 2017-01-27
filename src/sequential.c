#include "sequential.h"

#include <string.h>
#include <stdio.h>

/* TODO:
 * - seq_flatten(seq_t seq): Convert data into a contiguous array_t.
 * - Compare performance against C++ STL.
 * - Create an explicit SEQ (or type) that takes advantage of Intel i5+ caching.
 * - SEQ_ITER_FILTER (callback to skip, etc; borrow from Ruby...)
 * - SEQ_ITER_TIME (gather start, stop and average per-iteration times)
 * - sort
 * - performance
 * - allocate entire node to include "data" at end */

#define seq_args_copy(args_to, args_from) __va_copy(args_to, args_from)
#define seq_opt(opt, mask) ((opt & mask) == mask)

typedef struct _seq_node_t seq_node_t;

struct _seq_node_t {
	seq_data_t data;
	seq_node_t* next;
	seq_node_t* prev;
};

struct _seq_t {
	seq_node_t* front;
	seq_node_t* back;
	seq_size_t size;
	seq_cb_t cb;
};

/* struct _seq_iter_t {
	seq_node_t* node;
	seq_size_t index;
	struct {
		seq_size_t begin;
		seq_size_t end;
	} range;
	seq_size_t stride;
	unsigned int bits;
}; */

/* ==================================================================== Callbacks Implementation */
static seq_data_t seq_cb_add(seq_args_t args) {
	return seq_arg_data(args);
}

static seq_data_t seq_cb_add_copy(seq_args_t args) {
	seq_data_t dest = NULL;
	seq_data_t src = seq_arg_data(args);
	seq_size_t size = seq_arg_size(args);

	if(!src || size <= 0) return NULL;

	dest = (seq_data_t)(malloc(size));

	if(!dest) return NULL;

	memcpy(dest, src, size);

	return dest;
}

static seq_bool_t seq_cb_remove(seq_data_t data) {
	return SEQ_TRUE;
}

static seq_bool_t seq_cb_remove_free(seq_data_t data) {
	free(data);
}

static seq_bool_t seq_cb_get() {
	return SEQ_FALSE;
}

static seq_bool_t seq_cb_set() {
	return SEQ_FALSE;
}

static seq_cb_t seq_cb = {
	seq_cb_add,
	seq_cb_remove,
	seq_cb_get,
	seq_cb_set
};

/* ========================================================================= Node Implementation */
static void seq_node_destroy(seq_t seq, seq_node_t* node) {
	seq->cb.remove(node->data);

	free(node);
}

#if 0
/* TODO: Some way to determine if we should work backwards. */
static seq_node_t* seq_node_get_data(seq_t seq, seq_args_t args) {
	seq_node_t* node = seq_list(seq)->front;

	while(node) {
		seq_args_t args_copy;
		seq_bool_t cmp;

		seq_args_copy(args_copy, args);

		cmp = seq->callbacks->compare(seq, node->data, args_copy);

		va_end(args_copy);

		if(!cmp) return node;

		node = node->next;
	}

	return NULL;
}
#endif

static seq_node_t* seq_node_get(seq_t seq, seq_args_t args) {
	seq_opt_t get = seq_arg_opt(args);
	seq_node_t* node = NULL;

	if(!seq_opt(get, SEQ_GET)) return NULL;

	if(get & SEQ_INDEX) {
		seq_size_t index = seq_arg_size(args);
		seq_size_t sindex = index < 0 ? seq->size - abs(index) : index;
		seq_size_t i;

		if(index >= seq->size) return NULL;

		/* If the index is PAST the middle of the list, work backwards. */
		else if(seq->size / 2 < sindex) {
			node = seq->back;

			for(i = 0; i < seq->size - sindex - 1; i--) node = node->prev;
		}

		/* Otherwise, start from the front. */
		else {
			node = seq->front;

			for(i = 0; i < sindex; i++) node = node->next;
		}
	}

	return node;
}

/* ========================================================================= Core Implementation */
#define seq_vfunc(func, start, ret) \
	seq_args_t args; \
	va_start(args, start); \
	ret = seq_v##func(start, args); \
	va_end(args)

seq_t seq_create(seq_opt_t opt, ...) {
	seq_t seq = NULL;

	seq_vfunc(create, opt, seq);

	return seq;
}

seq_t seq_vcreate(seq_opt_t opt, seq_args_t args) {
	seq_t seq = (seq_t)(malloc(sizeof(struct _seq_t)));
	seq_opt_t o = opt;

	if(!seq) return NULL;

	memcpy(&seq->cb, &seq_cb, sizeof(seq_cb_t));

	while(o) {
		if(!seq_opt(o, SEQ_OPT) || !seq_vset(seq, args)) break;

		o = seq_arg_opt(args);
	}

	return seq;
}

void seq_destroy(seq_t seq) {
	seq_node_t* node = seq->front;

	while(node) {
		seq_node_t* tmp = node->next;

		seq_node_destroy(seq, node);

		node = tmp;
	}

	free(seq);
}

seq_bool_t seq_add(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_vfunc(add, seq, r);

	return r;
}

seq_bool_t seq_vadd(seq_t seq, seq_args_t args) {
	seq_node_t* node = NULL;
	seq_opt_t add = seq_arg_opt(args);

	if(!seq_opt(add, SEQ_ADD) || !(node = (seq_node_t*)(malloc(sizeof(seq_node_t))))) goto err;

	if(add & (SEQ_APPEND | SEQ_PREPEND)) {
		node->data = seq->cb.add(args);

		/* This is the FIRST node added; set the @front and @back of $list and quickly return. */
		if(!seq->front && !seq->back) {
			seq->front = node;
			seq->back = node;
		}

		else {
			if(add & SEQ_APPEND) {
				node->prev = seq->back;

				seq->back->next = node;
				seq->back = node;
			}

			else {
				node->prev = NULL;
				node->next = seq->front;

				seq->front->prev = node;
				seq->front = node;
			}
		}
	}

	else {
		seq_node_t* pnode = seq_node_get(seq, args);

		if(!pnode) goto err;

		if(add & SEQ_BEFORE) {
		}

		else if(add & SEQ_AFTER) {
		}

		else {
		}
	}

	seq->size++;

	return SEQ_TRUE;

err:
	if(node) seq_node_destroy(seq, node);

	return SEQ_FALSE;
}

seq_bool_t seq_remove(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_vfunc(remove, seq, r);

	return r;
}

seq_bool_t seq_vremove(seq_t seq, seq_args_t args) {
	seq_node_t* node = seq_node_get(seq, args);

	if(!node) return SEQ_FALSE;

	/* Somewhere in the middle. */
	if(node->prev && node->next) {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	/* The very last node. */
	else if(node->prev) {
		node->prev->next = NULL;

		seq->back = node->prev;
	}

	/* The very first node. */
	else {
		node->next->prev = NULL;

		seq->front = node->next;
	}

	seq_node_destroy(seq, node);

	seq->size--;

	return SEQ_TRUE;
}

seq_data_t seq_get(seq_t seq, ...) {
	seq_data_t data = NULL;

	seq_vfunc(get, seq, data);

	return data;
}

seq_data_t seq_vget(seq_t seq, seq_args_t args) {
	seq_node_t* node = seq_node_get(seq, args);

	if(node) return node->data;

	return NULL;
}

seq_bool_t seq_set(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_vfunc(set, seq, r);

	return r;
}

seq_bool_t seq_vset(seq_t seq, seq_args_t args) {
	seq_opt_t opt = seq_arg_opt(args);

	if(opt == SEQ_CALLBACKS) {
		seq_cb_t* cb = seq_arg(args, seq_cb_t*);

		if(cb) {
			if(cb->add) seq->cb.add = cb->add;
			if(cb->remove) seq->cb.remove = cb->remove;
			if(cb->get) seq->cb.get = cb->get;
			if(cb->set) seq->cb.set = cb->set;
		}
	}

	else if(opt == SEQ_MAP) return SEQ_FALSE;

	else if(opt == SEQ_RING) return SEQ_FALSE;

	else if(opt == SEQ_QUEUE) return SEQ_FALSE;

	else if(opt == SEQ_STACK) return SEQ_FALSE;

	else if(opt == SEQ_ADD_COPY) seq->cb.add = seq_cb_add_copy;

	else if(opt == SEQ_REMOVE_FREE) seq->cb.remove = seq_cb_remove_free;

	else return SEQ_FALSE;

	return SEQ_TRUE;
}

seq_size_t seq_size(seq_t seq) {
	return seq->size;
}

