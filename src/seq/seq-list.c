#include "seq-api.h"

/* ======================================================================== Types, Constants, Enums
 * struct _seq_list_node_t
 * struct _seq_list_data_t
 * struct _seq_list_iter_data_t
 * seq_list_data
 * seq_list_iter_data
 * SEQ_TYPE_API(list)
 * --------------------------------------------------------------------------------------------- */

typedef struct _seq_list_node_t* seq_list_node_t;
typedef struct _seq_list_data_t* seq_list_data_t;
typedef struct _seq_list_iter_data_t* seq_list_iter_data_t;

struct _seq_list_node_t {
	seq_data_t data;
	seq_list_node_t next;
	seq_list_node_t prev;
};

struct _seq_list_data_t {
	seq_list_node_t front;
	seq_list_node_t back;
};

struct _seq_list_iter_data_t {
	seq_list_node_t node;
	seq_size_t index;
	seq_size_t inc;

	struct {
		seq_size_t begin;
		seq_size_t end;
	} range;
};

typedef struct _seq_list_node_get_t {
	seq_list_node_t node;
	seq_size_t index;
} seq_list_node_get_t;

#define seq_list_data(seq) (seq_list_data_t)(seq->data)
#define seq_list_iter_data(iter) (seq_list_iter_data_t)(iter->data)

SEQ_TYPE_API(list)

/* =========================================================================== Private List Helpers
 * seq_list_index
 * seq_list_node_destroy
 * seq_list_node_get_index
 * seq_list_node_get
 * seq_list_node_data
 * ============================================================================================= */

static seq_size_t seq_list_index(seq_t seq, seq_size_t index) {
	index = index < 0 ? seq->size - abs(index) : index;

	if(index >= seq->size || index < 0) return -1;

	return index;
}

static void seq_list_node_destroy(seq_t seq, seq_list_node_t node) {
	if(seq->cb.remove) seq->cb.remove(node->data);

	free(node);
}

static seq_list_node_t seq_list_node_get_index(seq_t seq, seq_size_t index) {
	seq_list_data_t data = seq_list_data(seq);
	seq_list_node_t node = NULL;
	seq_size_t i;

	index = seq_list_index(seq, index);

	if(index >= 0) {
		/* If the index is PAST the middle of the list, work backwards. */
		if(index > seq->size / 2) {
			node = data->back;

			for(i = 1; i < seq->size - index; i++) node = node->prev;
		}

		/* Otherwise, start from the front. */
		else {
			node = data->front;

			for(i = 0; i < index; i++) node = node->next;
		}
	}

	return node;
}

static seq_list_node_get_t seq_list_node_get(seq_t seq, seq_args_t args) {
	seq_list_node_get_t get;
	seq_opt_t opt = seq_arg_opt(args);
	seq_size_t index = seq_arg_size(args);

	if(seq_opt(opt, SEQ_GET) && opt == SEQ_INDEX) {
		get.node = seq_list_node_get_index(seq, index);
		get.index = index;
	}

	return get;
}

static seq_data_t seq_list_node_data(seq_t seq, seq_args_t args) {
	if(!seq->cb.add) return seq_arg_data(args);

	else return seq->cb.add(args);
}

/* ======================================================================== SEQ_LIST Implementation
 * seq_list_create
 * seq_list_destroy
 * seq_list_add
 * seq_list_remove
 * seq_list_get
 * seq_list_set
 * ============================================================================================= */

static void seq_list_create(seq_t seq) {
	seq->type = SEQ_LIST;
	seq->impl = seq_impl_list();
	seq->data = seq_malloc(seq_list_data_t);
}

static void seq_list_destroy(seq_t seq) {
	seq_list_data_t data = seq_list_data(seq);
	seq_list_node_t node = data->front;

	while(node) {
		seq_list_node_t tmp = node->next;

		seq_list_node_destroy(seq, node);

		node = tmp;
	}

	free(data);
}

static seq_bool_t seq_list_add(seq_t seq, seq_args_t args) {
	seq_list_data_t data = seq_list_data(seq);
	seq_list_node_t node = NULL;
	seq_opt_t add = seq_arg_opt(args);

	if(!seq_opt(add, SEQ_ADD) || !(node = seq_malloc(seq_list_node_t))) goto err;

	if(add == SEQ_APPEND || add == SEQ_PREPEND) {
		if(!(node->data = seq_list_node_data(seq, args))) goto err;

		/* This is the first node. */
		if(!data->front && !data->back) {
			data->front = node;
			data->back = node;
		}

		else {
			if(add == SEQ_APPEND) {
				node->prev = data->back;

				data->back->next = node;
				data->back = node;
			}

			else {
				node->next = data->front;

				data->front->prev = node;
				data->front = node;
			}
		}
	}

	else if(add == SEQ_BEFORE || add == SEQ_AFTER || add == SEQ_REPLACE) {
		seq_list_node_t pnode = seq_list_node_get(seq, args).node;

		if(!pnode || !(node->data = seq_list_node_data(seq, args))) goto err;

		if(add == SEQ_BEFORE) {
			node->next = pnode;
			node->prev = pnode->prev;

			if(pnode == data->front) data->front = node;

			else pnode->prev->next = node;

			pnode->prev = node;
		}

		else if(add == SEQ_AFTER) {
			node->next = pnode->next;
			node->prev = pnode;

			if(pnode == data->back) data->back = node;

			else pnode->next->prev = node;

			pnode->next = node;
		}

		else {
			node->next = pnode->next;
			node->prev = pnode->prev;

			if(node->next) node->next->prev = node;

			if(node->prev) node->prev->next = node;

			if(pnode == data->front) data->front = node;

			else if(pnode == data->back) data->back = node;

			seq_list_node_destroy(seq, pnode);

			/* TODO: This is a silly hack... */
			seq->size--;
		}
	}

	return SEQ_TRUE;

err:
	if(node) seq_list_node_destroy(seq, node);

	return SEQ_FALSE;
}

static seq_bool_t seq_list_remove(seq_t seq, seq_args_t args) {
	seq_list_data_t data = seq_list_data(seq);
	seq_list_node_t node = seq_list_node_get(seq, args).node;

	if(!node) return SEQ_FALSE;

	/* Somewhere in the middle. */
	if(node->prev && node->next) {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	/* The very last node. */
	else if(node->prev) {
		if(node->prev) node->prev->next = NULL;

		data->back = node->prev;
	}

	/* The very first node. */
	else {
		if(node->next) node->next->prev = NULL;

		data->front = node->next;
	}

	seq_list_node_destroy(seq, node);

	return SEQ_TRUE;
}

static seq_get_t seq_list_get(seq_t seq, seq_args_t args) {
	seq_list_node_get_t get = seq_list_node_get(seq, args);

	if(get.node) return seq_got_index(get.node->data, get.index);

	return seq_got_null();
}

static seq_bool_t seq_list_set(seq_t seq, seq_opt_t set, seq_args_t args) {
	return SEQ_FALSE;
}

/* ============================================================== SEQ_LIST Iteration Implementation
 * seq_list_iter_create
 * seq_list_iter_destroy
 * seq_list_iter_get
 * seq_list_iter_set
 * seq_list_iter_iterate
 * ============================================================================================= */

static void seq_list_iter_create(seq_iter_t iter, seq_args_t args) {
	seq_opt_t opt = SEQ_FALSE;
	seq_list_iter_data_t data = NULL;

	iter->data = seq_malloc(seq_list_iter_data_t);

	if(!iter->data) return;

	data = iter->data;

	data->node = NULL;
	data->index = -1;
	data->range.begin = 0;
	data->range.end = iter->seq->size - 1;
	data->inc = 1;

	while((opt = seq_arg_opt(args))) {
		if(opt == SEQ_RANGE) {
			data->range.begin = seq_list_index(iter->seq, seq_arg_size(args));
			data->range.end = seq_list_index(iter->seq, seq_arg_size(args));
		}

		else if(opt == SEQ_INC) data->inc = seq_arg_size(args);
	}

	data->node = seq_list_node_get_index(iter->seq, data->range.begin);
}

static void seq_list_iter_destroy(seq_iter_t iter) {
}

static seq_get_t seq_list_iter_get(seq_iter_t iter, seq_args_t args) {
	seq_list_iter_data_t data = seq_list_iter_data(iter);
	seq_opt_t get = seq_arg_opt(args);

	if(seq_opt(get, SEQ_GET) && get == SEQ_DATA) return seq_got_index(
		data->node->data,
		data->index
	);

	return seq_got_null();
}

static seq_bool_t seq_list_iter_set(seq_iter_t iter, seq_args_t args) {
	/* seq_list_iter_data_t data = seq_list_iter_data(iter); */

	return SEQ_FALSE;
}

static seq_bool_t seq_list_iter_iterate(seq_iter_t iter) {
	seq_list_iter_data_t data = seq_list_iter_data(iter);

	if(iter->state == SEQ_STOP) return SEQ_FALSE;

	else if(iter->state == SEQ_READY) {
		iter->state = SEQ_ACTIVE;
		data->index = data->range.begin;
	}

	else {
		seq_size_t i;

		for(i = 0; i < data->inc; i++) {
			data->node = data->node->next;

			if(!data->node) return SEQ_FALSE;

			data->index++;
		}
	}

	return SEQ_TRUE;
}

