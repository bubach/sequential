#include "sequential.h"

#include <string.h>
#include <stdio.h>

/* TODO:
 * - Compare performance against C++ STL.
 * - SEQ_ITER_FILTER (callback to skip, etc; borrow from Ruby...)
 * - SEQ_ITER_TIME (gather start, stop and average per-iteration times)
 * - sort */

#define seq_args_copy(args_to, args_from) __va_copy(args_to, args_from)
#define seq_opt(opt, mask) (opt <= mask##_MAX && ((opt & mask) == mask))

typedef struct _seq_node_t* seq_node_t;

#define SEQ_NODE_SIZE sizeof(struct _seq_node_t)

struct _seq_node_t {
	seq_data_t data;
	seq_node_t next;
	seq_node_t prev;
};

#define SEQ_SIZE sizeof(struct _seq_t)

struct _seq_t {
	seq_opt_t opts;
	seq_size_t size;
	seq_node_t front;
	seq_node_t back;
	seq_cb_add_t add;
	seq_cb_remove_t remove;
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

/* ============================================================================================= */
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

static void seq_cb_remove_free(seq_data_t data) {
	free(data);
}

/* ============================================================================================= */
static void seq_node_destroy(seq_t seq, seq_node_t node) {
	if(seq->remove) seq->remove(node->data);

	free(node);
}

static seq_node_t seq_node_get(seq_t seq, seq_args_t args) {
	seq_opt_t get = seq_arg_opt(args);
	seq_node_t node = NULL;

	if(!seq_opt(get, SEQ_GET)) return NULL;

	if(get == SEQ_INDEX) {
		seq_size_t index = seq_arg_size(args);
		seq_size_t i;

		index = index < 0 ? seq->size - abs(index) : index;

		if(index >= seq->size || index < 0) return NULL;

		/* If the index is PAST the middle of the list, work backwards. */
		else if(index > seq->size / 2) {
			node = seq->back;

			for(i = 1; i < seq->size - index; i++) node = node->prev;
		}

		/* Otherwise, start from the front. */
		else {
			node = seq->front;

			for(i = 0; i < index; i++) node = node->next;
		}
	}

	return node;
}

static seq_data_t seq_data(seq_t seq, seq_args_t args) {
	if(!seq->add) return seq_arg_data(args);

	else return seq->add(args);
}

/* ============================================================================================= */
#define seq_vfunc(func, start, ret) \
	seq_args_t args; \
	va_start(args, start); \
	ret = seq_v##func(start, args); \
	va_end(args)

seq_t seq_create() {
	seq_t seq = (seq_t)(malloc(SEQ_SIZE));

	if(!seq) return NULL;

	memset(seq, 0, SEQ_SIZE);

	return seq;
}

void seq_destroy(seq_t seq) {
	seq_node_t node = seq->front;

	while(node) {
		seq_node_t tmp = node->next;

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
	seq_node_t node = NULL;
	seq_opt_t add = seq_arg_opt(args);

	if(
		!seq_opt(add, SEQ_ADD) ||
		!(node = (seq_node_t)(malloc(SEQ_NODE_SIZE)))
	) goto err;

	memset(node, 0, SEQ_NODE_SIZE);

	if(add == SEQ_APPEND || add == SEQ_PREPEND) {
		if(!(node->data = seq_data(seq, args))) goto err;

		/* This is the first node. */
		if(!seq->front && !seq->back) {
			seq->front = node;
			seq->back = node;
		}

		else {
			if(add == SEQ_APPEND) {
				node->prev = seq->back;

				seq->back->next = node;
				seq->back = node;
			}

			else {
				node->next = seq->front;

				seq->front->prev = node;
				seq->front = node;
			}
		}
	}

	else if(add == SEQ_BEFORE || add == SEQ_AFTER || add == SEQ_REPLACE) {
		seq_node_t pnode = seq_node_get(seq, args);

		if(!pnode || !(node->data = seq_data(seq, args))) goto err;

		if(add == SEQ_BEFORE) {
			node->next = pnode;
			node->prev = pnode->prev;

			if(pnode == seq->front) seq->front = node;

			else pnode->prev->next = node;

			pnode->prev = node;
		}

		else if(add == SEQ_AFTER) {
			node->next = pnode->next;
			node->prev = pnode;

			if(pnode == seq->back) seq->back = node;

			else pnode->next->prev = node;

			pnode->next = node;
		}

		/* TODO: Got to fix this. */
		else {
			pnode->data = node->data;

			free(node);

			return SEQ_TRUE;
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
	seq_node_t node = seq_node_get(seq, args);

	if(!node) return SEQ_FALSE;

	/* Somewhere in the middle. */
	if(node->prev && node->next) {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	/* The very last node. */
	else if(node->prev) {
		if(node->prev) node->prev->next = NULL;

		seq->back = node->prev;
	}

	/* The very first node. */
	else {
		if(node->next) node->next->prev = NULL;

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
	seq_node_t node = seq_node_get(seq, args);

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

	if(opt == SEQ_ARRAY) return SEQ_FALSE;

	else if(opt == SEQ_MAP) return SEQ_FALSE;

	else if(opt == SEQ_RING) return SEQ_FALSE;

	else if(opt == SEQ_QUEUE) return SEQ_FALSE;

	else if(opt == SEQ_STACK) return SEQ_FALSE;

	else if(opt == SEQ_ON_ADD) {
		seq_cb_add_t add = seq_arg(args, seq_cb_add_t);

		if(!add) return SEQ_FALSE;

		seq->add = add;
	}

	else if(opt == SEQ_ON_REMOVE) {
		seq_cb_remove_t remove = seq_arg(args, seq_cb_remove_t);

		if(!remove) return SEQ_FALSE;

		seq->remove = remove;
	}

	else if(opt == SEQ_ADD_COPY) seq->add = seq_cb_add_copy;

	else if(opt == SEQ_REMOVE_FREE) seq->remove = seq_cb_remove_free;

	else return SEQ_FALSE;

	return SEQ_TRUE;
}

seq_size_t seq_size(seq_t seq) {
	return seq->size;
}

/* ============================================================================================= */
void seq_printall_str(seq_t seq) {
	seq_node_t node = seq->front;
	int i = 0;

	printf("seq_t (%p) [ size=%d ] {\n", (void*)(seq), seq->size);
	printf("   @f: (%p)\n", (void*)(seq->front));

	while(node) {
		printf(
			"   %02d: (%p) [ data='%s' next='%s' prev='%s' ]\n",
			i,
			(void*)(node),
			(char*)(node->data),
			node->next ? (char*)(node->next->data) : "NUL",
			node->prev ? (char*)(node->prev->data) : "NUL"
		);

		node = node->next;

		i++;
	}

	printf("   @b: (%p)\n", (void*)(seq->back));
	printf("}\n");
}

void seq_printall_ptr(seq_t seq) {
	seq_node_t node = seq->front;
	int i;

	while(node) {
		printf("%02d = \"%p\"\n", i, node->data);

		node = node->next;
		i++;
	}
}

