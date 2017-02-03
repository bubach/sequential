#include "seq-api.h"

typedef struct _seq_list_node_t* seq_list_node_t;
typedef struct _seq_list_data_t* seq_list_data_t;
typedef struct _seq_list_iter_t* seq_list_iter_t;

struct _seq_list_node_t {
	seq_data_t data;
	seq_list_node_t next;
	seq_list_node_t prev;
};

struct _seq_list_data_t {
	seq_list_node_t front;
	seq_list_node_t back;
};

struct _seq_list_iter_t {
	struct _seq_iter_t iter;

	seq_list_node_t node;
	seq_size_t index;
	seq_size_t inc;

	struct {
		seq_size_t begin;
		seq_size_t end;
	} range;
};

#define seq_list_data(seq) (seq_list_data_t)(seq->data)
#define seq_list_iter(iter) (seq_list_iter_t)(iter)

SEQ_TYPE(list)

/* ============================================================================================= */
static seq_size_t seq_list_index(seq_t seq, seq_size_t index) {
	index = index < 0 ? seq->size - abs(index) : index;

	if(index >= seq->size || index < 0) return -1;

	return index;
}

static void seq_list_node_destroy(seq_t seq, seq_list_node_t node) {
	if(seq->on.remove) seq->on.remove(node->data);

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

static seq_list_node_t seq_list_node_get(seq_t seq, seq_args_t args) {
	seq_opt_t get = seq_arg_opt(args);
	seq_list_node_t node = NULL;

	if(seq_opt(get, SEQ_GET) && get == SEQ_INDEX) node = seq_list_node_get_index(
		seq,
		seq_arg_size(args)
	);

	return node;
}

static seq_data_t seq_list_node_data(seq_t seq, seq_args_t args) {
	if(!seq->on.add) return seq_arg_data(args);

	else return seq->on.add(args);
}

/* ============================================================================================= */
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

	if(
		!seq_opt(add, SEQ_ADD) ||
		!(node = seq_malloc(seq_list_node_t))
	) goto err;

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
		seq_list_node_t pnode = seq_list_node_get(seq, args);

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
	seq_list_node_t node = seq_list_node_get(seq, args);

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

static seq_data_t seq_list_get(seq_t seq, seq_args_t args) {
	seq_list_node_t node = seq_list_node_get(seq, args);

	if(node) return node->data;

	return NULL;
}

static seq_bool_t seq_list_set(seq_t seq, seq_args_t args) {
	return SEQ_TRUE;
}

/* ============================================================================================= */
static seq_iter_t seq_list_iter_create(seq_t seq, seq_args_t args) {
	seq_list_iter_t iter = seq_malloc(seq_list_iter_t);
	seq_opt_t opt = SEQ_NONE;

	iter->node = NULL;
	iter->index = -1;
	iter->range.begin = 0;
	iter->range.end = seq->size - 1;
	iter->inc = 1;

	while((opt = seq_arg_opt(args))) {
		if(opt == SEQ_RANGE) {
			iter->range.begin = seq_list_index(seq, seq_arg_size(args));
			iter->range.end = seq_list_index(seq, seq_arg_size(args));
		}

		else if(opt == SEQ_INC) iter->inc = seq_arg_size(args);
	}

	iter->node = seq_list_node_get_index(seq, iter->range.begin);

	return (seq_iter_t)(iter);
}

static void seq_list_iter_destroy(seq_iter_t iter) {
	free(seq_list_iter(iter));
}

static seq_data_t seq_list_iter_get(seq_iter_t iter, seq_args_t args) {
	seq_list_iter_t liter = seq_list_iter(iter);
	seq_opt_t get = seq_arg_opt(args);

	if(seq_opt(get, SEQ_GET)) {
		if(get == SEQ_INDEX) return (seq_data_t)(liter->index);

		else if(get == SEQ_DATA) return liter->node->data;
	}

	return NULL;
}

static seq_bool_t seq_list_iter_set(seq_iter_t iter, seq_args_t args) {
	/* seq_list_iter_t liter = seq_list_iter(iter); */

	return SEQ_FALSE;
}

static seq_bool_t seq_list_iter_iterate(seq_iter_t iter) {
	seq_list_iter_t liter = seq_list_iter(iter);

	if(liter->iter.state == SEQ_STOP) return SEQ_FALSE;

	else if(liter->iter.state == SEQ_READY) {
		liter->iter.state = SEQ_ACTIVE;
		liter->index = liter->range.begin;
	}

	else {
		seq_size_t i;

		for(i = 0; i < liter->inc; i++) {
			liter->node = liter->node->next;

			if(!liter->node) return SEQ_FALSE;

			liter->index++;
		}
	}

	return SEQ_TRUE;
}

