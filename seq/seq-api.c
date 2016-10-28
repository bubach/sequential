#include "seq-api.h"

static const seq_impl_t* SEQ_IMPL[] = {
	&SEQ_IMPL_LIST,
	&SEQ_IMPL_MAP,
	&SEQ_IMPL_RING,
	&SEQ_IMPL_QUEUE,
	&SEQ_IMPL_STACK,
	&SEQ_IMPL_ARRAY,
	&SEQ_IMPL_TEST,
	&SEQ_IMPL_ERROR
};

static seq_t SEQ_ERROR_INSTANCE = { SEQ_ERROR, -1, SEQ_ERR(NULL), NULL };

#define seq_vfunc(func, start, ret) \
	va_list args; \
	va_start(args, start); \
	ret = seq_v##func(start, args); \
	va_end(args)

/* =================================================================================== Error API */
seq_err_t seq_err(seq_err_type_t type, const char* code) {
	seq_err_t err = SEQ_ERR(NONE);

	if(code) {
		size_t len = strlen(code);

		memcpy(&err, code, len > 6 ? 6 : len);
	}

	err.type = type;

	return err;
}

/* ==================================================================================== Core API */
seq_t* seq_create(seq_type_t type, ...) {
	seq_t* seq = NULL;

	seq_vfunc(create, type, seq);

	return seq;
}

seq_t* seq_vcreate(seq_type_t type, va_list args) {
	seq_t* seq = SEQ_IMPL[type]->create(args);

	if(seq) seq->type = type;

	else seq = &SEQ_ERROR_INSTANCE;

	return seq;
}

void seq_destroy(seq_t* seq) {
	if(!seq) return;

	SEQ_IMPL[seq->type]->destroy(seq);

	free(seq);
}

seq_bool_t seq_add(seq_t* seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_vfunc(add, seq, r);

	return r;
}

seq_bool_t seq_vadd(seq_t* seq, va_list args) {
	if(seq_valid(seq) && SEQ_IMPL[seq->type]->add(seq, args)) {
		seq->size++;

		return SEQ_TRUE;
	}

	return SEQ_FALSE;
}

seq_bool_t seq_remove(seq_t* seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_vfunc(remove, seq, r);

	return r;
}

seq_bool_t seq_vremove(seq_t* seq, va_list args) {
	if(seq_valid(seq) && SEQ_IMPL[seq->type]->remove(seq, args)) {
		seq->size--;

		return SEQ_TRUE;
	}

	return SEQ_FALSE;
}

void* seq_get(seq_t* seq, ...) {
	void* data = NULL;

	seq_vfunc(get, seq, data);

	return data;
}

void* seq_vget(seq_t* seq, va_list args) {
	if(!seq_valid(seq)) return NULL;

	return SEQ_IMPL[seq->type]->get(seq, args);
}

seq_type_t seq_type(seq_t* seq) {
	if(!seq_valid(seq)) return SEQ_ERROR;

	return seq->type;
}

seq_size_t seq_size(seq_t* seq) {
	if(!seq_valid(seq)) return -1;

	return seq->size;
}

seq_bool_t seq_valid(seq_t* seq) {
	if(seq && !seq->error.type) return SEQ_TRUE;

	return SEQ_FALSE;
}

seq_err_t seq_error(seq_t* seq) {
	if(!seq) return seq_err_null("SEQ*");

	return seq->error;
}

/* ================================================================================ Callback API */
seq_bool_t seq_arg_enum(seq_t* seq, seq_enum_t* e, va_list args) {
	/* The last little bit ((*e & (*e - 1))) is some interesting trickery that makes sure the
	 * enum value is a power-of-two. */
	if(!(*e = va_arg(args, int)) || !(*e & SEQ_ANY) || (*e & (*e - 1))) return SEQ_FALSE;

	return SEQ_TRUE;
}

seq_bool_t seq_arg_size(seq_t* seq, seq_size_t* size, va_list args) {
	if((*size = va_arg(args, seq_size_t)) <= 0) return SEQ_FALSE;

	return SEQ_TRUE;
}

seq_bool_t seq_arg_data(seq_t* seq, void** data, va_list args) {
	if(!data || !(*data = va_arg(args, void*))) return SEQ_FALSE;

	return SEQ_TRUE;
}

seq_bool_t seq_arg_cb(seq_t* seq, seq_cb_t** cb, va_list args) {
	if(!cb || !(*cb = va_arg(args, seq_cb_t*)) || ((*cb)->header != SEQ_HDR_CB)) return SEQ_FALSE;

	return SEQ_TRUE;
}

/* ================================================================================= Private API */
#if 0
	if(!seq_err_any(
		!seq, SEQ_ERR_NULL, "SEQ*",
		!seq_func0(seq, foo), SEQ_ERR_ARG, NULL,
		NULL
	)) {
		...
	}
seq_bool_t seq_err_any(seq_t* seq, ...) {
	seq_bool_t result = SEQ_FALSE;
	seq_err_t err = SEQ_ERR(NONE);

	va_list args;

	va_start(args, seq);

	while(

	va_end(args);

	return SEQ_TRUE;
}

void seq_debug_printall(seq_t* seq) {
	seq_list_node_t* node = seq_list(seq)->front;
	seq_size_t i = 0;

	while(node) {
		printf("[%02d] '%s'\n", i, (char*)(node->data));

		node = node->next;

		i++;
	}
}
#endif

