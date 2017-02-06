#include "seq-api.h"

/* ============================================================================================= */
static void seq_on_remove_free(seq_data_t data) {
	free(data);
}

/* ============================================================================================= */
seq_t seq_create(seq_opt_t type) {
	seq_t seq = NULL;

	if(seq_opt(type, SEQ_TYPE) && type == SEQ_LIST) {
		seq = seq_malloc(seq_t);

		if(!seq) return NULL;

		seq_impl_list()->create(seq);
	}

	return seq;
}

void seq_destroy(seq_t seq) {
	seq->impl->destroy(seq);

	free(seq);
}

seq_bool_t seq_add(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_args_wrap(vadd, seq, r);

	return r;
}

seq_bool_t seq_vadd(seq_t seq, seq_args_t args) {
	if(seq->impl->add(seq, args)) {
		seq->size++;

		return SEQ_TRUE;
	}

	return SEQ_FALSE;
}

seq_bool_t seq_remove(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_args_wrap(vremove, seq, r);

	return r;
}

seq_bool_t seq_vremove(seq_t seq, seq_args_t args) {
	if(seq->impl->remove(seq, args)) {
		seq->size--;

		return SEQ_TRUE;
	}

	return SEQ_FALSE;
}

seq_data_t seq_get(seq_t seq, ...) {
	seq_data_t data = NULL;

	seq_args_wrap(vget, seq, data);

	return data;
}

seq_data_t seq_vget(seq_t seq, seq_args_t args) {
	return seq->impl->get(seq, args);
}

seq_bool_t seq_set(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_args_wrap(vset, seq, r);

	return r;
}

seq_bool_t seq_vset(seq_t seq, seq_args_t args) {
	seq_opt_t opt = seq_arg_opt(args);

	if(opt == SEQ_LIST) return SEQ_FALSE;

	else if(opt == SEQ_MAP) return SEQ_FALSE;

	else if(opt == SEQ_RING) return SEQ_FALSE;

	else if(opt == SEQ_QUEUE) return SEQ_FALSE;

	else if(opt == SEQ_STACK) return SEQ_FALSE;

	else if(opt == SEQ_ARRAY) return SEQ_FALSE;

	else if(opt == SEQ_ON_ADD) {
		seq_on_add_t on_add = seq_arg(args, seq_on_add_t);

		if(!on_add) return SEQ_FALSE;

		seq->on.add = on_add;
	}

	else if(opt == SEQ_ON_REMOVE) {
		seq_on_remove_t on_remove = seq_arg(args, seq_on_remove_t);

		if(!on_remove) return SEQ_FALSE;

		seq->on.remove = on_remove;
	}

	else if(opt == SEQ_ON_REMOVE_FREE) seq->on.remove = seq_on_remove_free;

	else return SEQ_FALSE;

	return SEQ_TRUE;
}

seq_size_t seq_size(seq_t seq) {
	return seq->size;
}

/* ============================================================================================= */
seq_iter_t seq_iter_create(seq_t seq, ...) {
	seq_iter_t iter = NULL;

	seq_args_wrap(iter_vcreate, seq, iter);

	return iter;
}

seq_iter_t seq_iter_vcreate(seq_t seq, seq_args_t args) {
	seq_iter_t iter = seq_malloc(seq_iter_t);

	if(!iter) return NULL;

	iter->seq = seq;
	iter->state = SEQ_READY;

	seq->impl->iter.create(iter, args);

	return iter;
}

void seq_iter_destroy(seq_iter_t iter) {
	iter->seq->impl->iter.destroy(iter);

	free(iter->data);
	free(iter);
}

seq_data_t seq_iter_get(seq_iter_t iter, ...) {
	seq_data_t data = NULL;

	seq_args_wrap(iter_vget, iter, data);

	return data;
}

seq_data_t seq_iter_vget(seq_iter_t iter, seq_args_t args) {
	return iter->seq->impl->iter.get(iter, args);
}

seq_bool_t seq_iter_set(seq_iter_t iter, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_args_wrap(iter_vset, iter, r);

	return r;
}

seq_bool_t seq_iter_vset(seq_iter_t iter, seq_args_t args) {
	return iter->seq->impl->iter.set(iter, args);
}

seq_bool_t seq_iterate(seq_iter_t iter) {
	return iter->seq->impl->iter.iterate(iter);
}

