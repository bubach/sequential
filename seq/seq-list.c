#include "seq-api.h"

typedef struct _seq_list_node_t seq_list_node_t;

struct _seq_list_node_t {
	void* data;
	seq_list_node_t* next;
	seq_list_node_t* prev;
};

typedef struct _seq_list_t {
	seq_t seq;
	seq_list_node_t* front;
	seq_list_node_t* back;
} seq_list_t;

#define seq_list(seq) ((seq_list_t*)(seq))

/* =========================================================================== Default Callbacks */
static void* seq_list_cb_add_ref(seq_t* seq, va_list args) {
	void* data = NULL;

	if(!seq_arg_data(seq, &data, args)) seq->error = seq_err_null("DATA");

	return data;
}

static void* seq_list_cb_add_copy(seq_t* seq, va_list args) {
	void* data = NULL;
	void* copy = NULL;
	seq_size_t size = 0;

	if(!seq_arg_data(seq, &data, args) || !seq_arg_size(seq, &size, args)) return NULL;

	if(!(copy = malloc(size))) seq->error = seq_err_malloc(NULL);

	else memcpy(copy, data, size);

	return copy;
}

static void seq_list_cb_remove_free(seq_t* seq, void* data) {
	if(data) free(data);
}

static void seq_list_cb_remove_noop(seq_t* seq, void* data) {
}

static seq_cmp_t seq_list_cb_compare(seq_t* seq, void* data, va_list args) {
	void* cmp = NULL;

	if(seq_arg_data(seq, &cmp, args) && cmp == data) return SEQ_EQUAL;

	return SEQ_PASS;
}

static seq_cb_t seq_list_cb_ref = {
	SEQ_HDR_CB,
	seq_list_cb_add_ref,
	seq_list_cb_remove_noop,
	seq_list_cb_compare
};

static seq_cb_t seq_list_cb_copy = {
	SEQ_HDR_CB,
	seq_list_cb_add_copy,
	seq_list_cb_remove_free,
	seq_list_cb_compare
};

/* ========================================================================= LIST Implementation */
static seq_bool_t seq_list_node_add(
	seq_t* seq,
	seq_enum_t action,
	seq_list_node_t* node,
	va_list arg
) {
	seq_list_t* list = seq_list(seq);

	if(action & SEQ_ADD) {
		/* This is the FIRST node added; set the @front and @back of $list and quickly return. */
		if(!list->front && !list->back) {
			list->front = node;
			list->back = node;
		}

		else {
			if(action == SEQ_APPEND) {
				node->prev = list->back;

				list->back->next = node;
				list->back = node;
			}

			else if(action == SEQ_PREPEND) {
				node->prev = NULL;
				node->next = list->front;

				list->front->prev = node;
				list->front = node;
			}

			/* Lists only support APPEND and PREPEND from the ANY group. */
			else seq->error = seq_err_enum(NULL);
		}
	}

	/* This case must be ADD_GET (if we've gotten this far). The calling function makes sure that
	 * action is one of ADD or ADD_GET for us initially. */
	else {
		/* If the list is empty, ADD_GET actions CANNOT be used. */
		if(!list->front && !list->back) seq->error = seq_err_arg(NULL);

		else {
			if(action == SEQ_BEFORE) seq->error = seq_err_todo(NULL);

			else if(action == SEQ_AFTER) seq->error = seq_err_todo(NULL);

			/* This MUST be REPLACE. */
			else seq->error = seq_err_todo(NULL);
		}
	}

	return !seq->error.type;
}

static void seq_list_node_destroy(seq_t* seq, seq_list_node_t* node) {
	seq->callbacks->remove(seq, node->data);

	free(node);
}

/* TODO: Some way to determine if we should work backwards. */
static seq_list_node_t* seq_list_node_get_data(seq_t* seq, va_list args) {
	seq_list_node_t* node = seq_list(seq)->front;

	while(node) {
		va_list args_copy;
		seq_bool_t cmp;

		__va_copy(args_copy, args);

		cmp = seq->callbacks->compare(seq, node->data, args_copy);

		va_end(args_copy);

		if(cmp) break;

		node = node->next;
	}

	return node;
}

static seq_list_node_t* seq_list_node_get_index(seq_t* seq, seq_size_t index) {
	seq_list_node_t* node = NULL;
	seq_size_t sindex = index < 0 ? seq->size - abs(index) : index;
	seq_size_t i;

	if(index > seq->size) seq->error = seq_err_arg(NULL);

	/* If the index is PAST the middle of the list, work backwards. */
	else if(seq->size / 2 < sindex) {
		node = seq_list(seq)->back;

		for(i = 0; i < seq->size - sindex - 1; i--) node = node->prev;
	}

	/* Otherwise, start from the front. */
	else {
		node = seq_list(seq)->front;

		for(i = 0; i < sindex; i++) node = node->next;
	}

	return node;
}

/* ========================================================================= LIST Implementation */
static seq_t* seq_list_create(va_list args) {
	seq_t* seq = (seq_t*)(malloc(sizeof(seq_list_t)));
	seq_enum_t mode = SEQ_NULL;

	if(!seq) return NULL;

	memset(seq, 0, sizeof(seq_list_t));

	if(seq_arg_enum(seq, &mode, args) && (mode & SEQ_CREATE)) {
		if(mode == SEQ_REF) seq->callbacks = &seq_list_cb_ref;

		else if(mode == SEQ_COPY) seq->callbacks = &seq_list_cb_copy;

		/* If the user is adding their own backend, an additional argument is expected that
		 * should point to the user-defined #seq_cb_t structure. */
		else {
			seq_cb_t* cb = NULL;

			if(seq_arg_cb(seq, &cb, args)) seq->callbacks = cb;

			else seq->error = seq_err_arg(NULL);
		}
	}

	return seq;
}

static void seq_list_destroy(seq_t* seq) {
	seq_list_node_t* node = seq_list(seq)->front;

	while(node) {
		seq_list_node_t* tmp = node->next;

		seq_list_node_destroy(seq, node);

		node = tmp;
	}
}

static seq_bool_t seq_list_add(seq_t* seq, va_list args) {
	seq_enum_t action = SEQ_NULL;

	if(seq_arg_enum(seq, &action, args) && (action & (SEQ_ADD | SEQ_ADD_GET))) {
		seq_list_node_t* node = NULL;

		if((node = (seq_list_node_t*)(malloc(sizeof(seq_list_node_t))))) {
			if((node->data = seq->callbacks->add(seq, args))) {
				node->next = NULL;
				node->prev = NULL;

				if(seq_list_node_add(seq, action, node, args)) return SEQ_TRUE;
			}

			else seq->error = seq_err_null(NULL);

			/* If we make it this far, $node has be malloc()'d, but is invalid; we should free() it
			 * to avoid memory leakage. */
			free(node);
		}

		else seq->error = seq_err_malloc(NULL);
	}

	else seq->error = seq_err_enum(NULL);

	return SEQ_FALSE;
}

static seq_bool_t seq_list_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_list_get(seq_t* seq, va_list args) {
	seq_enum_t action = SEQ_NULL;
	seq_list_node_t* node = NULL;

	if(!seq_arg_enum(seq, &action, args) || !(action & SEQ_GET)) seq->error = seq_err_enum(NULL);

	else {
		if(action == SEQ_INDEX) {
			seq_size_t index = -1;

			if(seq_arg_size(seq, &index, args)) node = seq_list_node_get_index(seq, index);
		}

		else if(action == SEQ_DATA) node = seq_list_node_get_data(seq, args);
	}

	if(node) return node->data;

	return NULL;
}

const seq_impl_t SEQ_IMPL_LIST = {
	seq_list_create,
	seq_list_destroy,
	seq_list_add,
	seq_list_remove,
	seq_list_get,
};

