#include "seq-api.h"

#include <string.h>

#define seq_args_wrap(func, start, ret) \
	seq_args_t args; \
	seq_args_start(args, start); \
	ret = seq_##func(start, args); \
	seq_args_end(args)

/* =================================================================================== Callbacks */

static void seq_cb_remove_free(seq_data_t data) {
	free(data);
}

static void seq_cb_debug_stdout(seq_opt_t level, const char* message, seq_data_t data) {
	fprintf(stdout, message);
}

static void seq_cb_debug_stderr(seq_opt_t level, const char* message, seq_data_t data) {
	fprintf(stderr, message);
}

static void seq_cb_debug_fwrite(seq_opt_t level, const char* message, seq_data_t data) {
	fprintf((FILE*)(data), message);
}

/* ==================================================================================== Core API */

seq_t seq_create(seq_opt_t type) {
	seq_t seq = NULL;

	if(seq_opt(type, SEQ_TYPE) && type == SEQ_LIST) {
		seq = seq_malloc(seq_t);

		if(!seq) return NULL;

		seq->set.debug.prefix = "";
		seq->set.debug.postfix = "";
		seq->set.debug.level = SEQ_ERROR;

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

	return seq_error(seq, "impl->add() callback failed");
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

	return seq_error(seq, "impl->remove() callback failed");
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

	if(!seq_opt(set, SEQ_SET)) return seq_error(seq, "invalid SEQ_SET value");

	if(set == SEQ_CB_ADD) {
		seq_cb_add_t add = seq_arg(args, seq_cb_add_t);

		if(!add) return seq_error(seq, "invalid SEQ_CB_ADD callback");

		seq->set.add = add;
	}

	else if(set == SEQ_CB_REMOVE) {
		seq_cb_remove_t remove = seq_arg(args, seq_cb_remove_t);

		if(!remove) return seq_error(seq, "invalid SEQ_CB_REMOVE callback");

		seq->set.remove = remove;
	}

	else if(set == SEQ_CB_DEBUG) {
		seq->set.debug.debug = seq_arg(args, seq_cb_debug_t);
		seq->set.debug.data = seq_arg(args, FILE*);
	}

	else if(set == SEQ_CB_REMOVE_FREE) seq->set.remove = seq_cb_remove_free;

	else if(set == SEQ_DEBUG_STDOUT) seq->set.debug.debug = seq_cb_debug_stdout;

	else if(set == SEQ_DEBUG_STDERR) seq->set.debug.debug = seq_cb_debug_stderr;

	else if(set == SEQ_DEBUG_FWRITE) {
		seq->set.debug.debug = seq_cb_debug_fwrite;
		seq->set.debug.data = seq_arg(args, FILE*);
	}

	else if(set == SEQ_DEBUG_PREFIX) seq->set.debug.prefix = seq_arg(args, const char*);

	else if(set == SEQ_DEBUG_POSTFIX) seq->set.debug.postfix = seq_arg(args, const char*);

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

static const char* seq_opt_str_type[] = {
	"TYPE",
	"LIST",
	"MAP",
	"RING",
	"QUEUE",
	"STACK",
	"ARRAY"
};

static const char* seq_opt_str_add[] = {
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

static const char* seq_opt_str_get[] = {
	"GET",
	"INDEX",
	"KEY",
	"RECV",
	"POP",
	"DATA"
};

static const char* seq_opt_str_set[] = {
	"SET",
	"CB_ADD",
	"CB_REMOVE",
	"CB_REMOVE_FREE",
	"CB_DEBUG",
	"DEBUG_STDOUT",
	"DEBUG_STDERR",
	"DEBUG_FWRITE",
	"DEBUG_PREFIX",
	"DEBUG_POSTFIX",
	"DEBUG_LEVEL"
};

static const char* seq_opt_str_iter[] = {
	"ITER",
	"READY",
	"ACTIVE",
	"STOP",
	"RANGE",
	"INC"
};

static const char* seq_opt_str_level[] = {
	"LEVEL",
	"INFO",
	"ERROR"
};

static const char** seq_opt_str_data[] = {
	seq_opt_str_type,
	seq_opt_str_add,
	seq_opt_str_get,
	seq_opt_str_set,
	seq_opt_str_iter,
	seq_opt_str_level
};

const char* seq_opt_str(seq_opt_t opt) {
	if(opt == SEQ_FALSE) return "FALSE";

	else if(opt == SEQ_TRUE) return "TRUE";

	else if(
		seq_opt(opt, SEQ_TYPE) ||
		seq_opt(opt, SEQ_ADD) ||
		seq_opt(opt, SEQ_GET) ||
		seq_opt(opt, SEQ_SET) ||
		seq_opt(opt, SEQ_ITER) ||
		seq_opt(opt, SEQ_LEVEL)
	) return seq_opt_str_data[((opt & 0x000F0000) >> 16) - 1][seq_opt_val(opt)];

	else return "NULL";
}

/* =================================================================================== Debugging */

static void seq_debug(seq_t seq, seq_opt_t level, const char* fmt, seq_args_t args) {
	if(
		seq_opt(seq->set.debug.level, SEQ_LEVEL) &&
		seq->set.debug.debug != NULL &&
		seq_opt_val(level) <= seq_opt_val(seq->set.debug.level)
	) {
		char format[512];
		char result[1024];
		seq_size_t index = 0;

		sprintf(
			format + index,
			"%s[%s] %s%s\n",
			seq->set.debug.prefix,
			seq_opt_str(level),
			fmt,
			seq->set.debug.postfix
		);

		vsprintf(result, format, args);

		seq->set.debug.debug(level, result, seq->set.debug.data);
	}
}

seq_bool_t seq_info(seq_t seq, const char* fmt, ...) {
	seq_args_t args;

	seq_args_start(args, fmt);
	seq_debug(seq, SEQ_INFO, fmt, args);
	seq_args_end(args);

	return SEQ_TRUE;
}

seq_bool_t seq_vinfo(seq_t seq, const char* fmt, seq_args_t args) {
	seq_debug(seq, SEQ_INFO, fmt, args);

	return SEQ_TRUE;
}

seq_bool_t seq_error(seq_t seq, const char* fmt, ...) {
	seq_args_t args;

	seq_args_start(args, fmt);
	seq_debug(seq, SEQ_ERROR, fmt, args);
	seq_args_end(args);

	return SEQ_FALSE;
}

seq_bool_t seq_verror(seq_t seq, const char* fmt, seq_args_t args) {
	seq_debug(seq, SEQ_ERROR, fmt, args);

	return SEQ_FALSE;
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

