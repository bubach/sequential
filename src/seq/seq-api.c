#include "seq-api.h"

#include <string.h>

/* =================================================================================== Callbacks */

static void seq_cb_remove_free(seq_data_t data) {
	free(data);
}

static void seq_cb_error_stdout(const char* message, seq_data_t data) {
	fprintf(stdout, message);
}

static void seq_cb_error_stderr(const char* message, seq_data_t data) {
	fprintf(stderr, message);
}

static void seq_cb_error_fwrite(const char* message, seq_data_t data) {
	fprintf((FILE*)(data), message);
}

/* ==================================================================================== Core API */

#define seq_args_wrap(func, start, ret) \
	va_list args; \
	va_start(args, start); \
	ret = seq_##func(start, &args); \
	va_end(args)

seq_t seq_create(seq_opt_t type) {
	seq_t seq = NULL;

	if(seq_opt(type, SEQ_TYPE)) {
		seq = seq_malloc(seq_t);

		if(seq) {
			seq->error.prefix = "";
			seq->error.postfix = "";

			if(type == SEQ_LIST) seq_impl_list()->create(seq);
		}
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

	return seq_false(seq, "seq_add: failed");
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

	return seq_false(seq, "seq_remove: failed");
}

seq_get_t seq_get(seq_t seq, ...) {
	seq_get_t get;

	seq_args_wrap(vget, seq, get);

	return get;
}

seq_get_t seq_vget(seq_t seq, seq_args_t args) {
	return seq->impl->get(seq, args);
}

seq_bool_t seq_set(seq_t seq, ...) {
	seq_bool_t r = SEQ_FALSE;

	seq_args_wrap(vset, seq, r);

	return r;
}

seq_bool_t seq_vset(seq_t seq, seq_args_t args) {
	seq_opt_t set = seq_arg_opt(args);

	if(!seq_opt(set, SEQ_SET)) return seq_false(seq, "seq_set: invalid SEQ_SET [%X]", set);

	if(set == SEQ_CB_ADD) {
		seq_cb_add_t add = seq_arg(args, seq_cb_add_t);

		if(!add) return seq_false(seq, "seq_set: invalid SEQ_CB_ADD");

		seq->cb.add = add;
	}

	else if(set == SEQ_CB_REMOVE) {
		seq_cb_remove_t remove = seq_arg(args, seq_cb_remove_t);

		if(!remove) return seq_false(seq, "seq_set: invalid SEQ_CB_REMOVE");

		seq->cb.remove = remove;
	}

	else if(set == SEQ_CB_REMOVE_FREE) seq->cb.remove = seq_cb_remove_free;

	else if(set == SEQ_CB_ERROR) seq->cb.error = seq_arg(args, seq_cb_error_t);

	else if(set == SEQ_ERROR_STDOUT) seq->cb.error = seq_cb_error_stdout;

	else if(set == SEQ_ERROR_STDERR) seq->cb.error = seq_cb_error_stderr;

	else if(set == SEQ_ERROR_FWRITE) {
		seq->cb.error = seq_cb_error_fwrite;
		seq->error.data = seq_arg(args, FILE*);
	}

	else if(set == SEQ_ERROR_PREFIX) seq->error.prefix = seq_arg(args, const char*);

	else if(set == SEQ_ERROR_POSTFIX) seq->error.postfix = seq_arg(args, const char*);

	else return seq->impl->set(seq, set, args);

	return SEQ_TRUE;
}

seq_opt_t seq_type(seq_t seq) {
	return seq->type;
}

seq_size_t seq_size(seq_t seq) {
	return seq->size;
}

/* =============================================================================== Iteration API */

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

seq_get_t seq_iter_get(seq_iter_t iter, ...) {
	seq_get_t get;

	seq_args_wrap(iter_vget, iter, get);

	return get;
}

seq_get_t seq_iter_vget(seq_iter_t iter, seq_args_t args) {
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

/* =========================================================================== Miscellaneous API */

static const char* seq_string_type[] = {
	"TYPE",
	"LIST",
	"MAP",
	"RING",
	"QUEUE",
	"STACK",
	"ARRAY"
};

static const char* seq_string_add[] = {
	"ADD",
	"APPEND",
	"PREPEND",
	"BEFORE",
	"AFTER",
	"REPLACE",
	"KEYVAL",
	"SEND",
	"PUSH"
};

static const char* seq_string_get[] = {
	"GET",
	"INDEX",
	"KEY",
	"RECV",
	"POP",
	"DATA"
};

static const char* seq_string_set[] = {
	"SET",
	"CB_ADD",
	"CB_REMOVE",
	"CB_REMOVE_FREE",
	"CB_ERROR",
	"ERROR_STDOUT",
	"ERROR_STDERR",
	"ERROR_FWRITE",
	"ERROR_PREFIX",
	"ERROR_POSTFIX"
};

static const char* seq_string_iter[] = {
	"ITER",
	"READY",
	"ACTIVE",
	"STOP",
	"RANGE",
	"INC"
};

static const char* seq_string_compare[] = {
	"LESS",
	"EQUAL",
	"GREATER"
};

static const char** seq_string_data[] = {
	seq_string_type,
	seq_string_add,
	seq_string_get,
	seq_string_set,
	seq_string_iter,
	seq_string_compare
};

const char* seq_string(seq_opt_t opt) {
	if(opt == SEQ_FALSE) return "FALSE";

	else if(opt == SEQ_TRUE) return "TRUE";

	else if(
		seq_opt(opt, SEQ_TYPE) ||
		seq_opt(opt, SEQ_ADD) ||
		seq_opt(opt, SEQ_GET) ||
		seq_opt(opt, SEQ_SET) ||
		seq_opt(opt, SEQ_ITER) ||
		seq_opt(opt, SEQ_COMPARE)
	) return seq_string_data[((opt & 0x000F0000) >> 16) - 1][seq_opt_val(opt)];

	else return "NULL";
}

/* =================================================================================== Debugging */

#define seq_error_args_wrap(start) \
	va_list args; \
	va_start(args, start); \
	seq_verror(seq, start, &args); \
	va_end(args)

static void seq_verror(seq_t seq, const char* fmt, seq_args_t args) {
	if(seq->cb.error) {
		char format[512];
		char result[1024];

		snprintf(format, 512, "%s%s%s\n", seq->error.prefix, fmt, seq->error.postfix);
		vsnprintf(result, 1024, format, *args);

		seq->cb.error(result, seq->error.data);
	}
}

void seq_error(seq_t seq, const char* fmt, ...) {
	seq_error_args_wrap(fmt);
}

seq_bool_t seq_false(seq_t seq, const char* fmt, ...) {
	seq_error_args_wrap(fmt);

	return SEQ_FALSE;
}

seq_data_t seq_null(seq_t seq, const char* fmt, ...) {
	seq_error_args_wrap(fmt);

	return NULL;
}

/* ============================================================================= SEQ_GET Helpers */

static seq_get_t seq_got(seq_data_t data, seq_data_t key, seq_size_t index) {
	seq_get_t get;

	get.data = data;

	if(key) get.handle.key = key;

	else get.handle.index = index;

	return get;
}

seq_get_t seq_got_index(seq_data_t data, seq_size_t index) {
	return seq_got(data, NULL, index);
}

seq_get_t seq_got_key(seq_data_t data, seq_data_t key) {
	return seq_got(data, key, 0);
}

seq_get_t seq_got_null() {
	return seq_got(NULL, NULL, 0);
}
