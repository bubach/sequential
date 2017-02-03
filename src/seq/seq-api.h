#ifndef SEQUENTIAL_SEQ_API_H
#define SEQUENTIAL_SEQ_API_H 1

#include "../sequential.h"

#include <stdlib.h>
#include <stdio.h>

#define seq_malloc(type) (type)(calloc(1, sizeof(struct _##type)))
#define seq_opt(opt, mask) (opt <= mask##_MAX && ((opt & mask) == mask))
#define seq_args_start va_start
#define seq_args_end va_end
#define seq_args_wrap(func, start, ret) \
	seq_args_t args; \
	seq_args_start(args, start); \
	ret = seq_##func(start, args); \
	seq_args_end(args)

typedef void (*seq_impl_create_t)(seq_t seq);
typedef void (*seq_impl_destroy_t)(seq_t seq);
typedef seq_bool_t (*seq_impl_add_t)(seq_t seq, seq_args_t args);
typedef seq_bool_t (*seq_impl_remove_t)(seq_t seq, seq_args_t args);
typedef seq_data_t (*seq_impl_get_t)(seq_t seq, seq_args_t args);
typedef seq_bool_t (*seq_impl_set_t)(seq_t seq, seq_args_t args);

typedef seq_iter_t (*seq_impl_iter_create_t)(seq_t seq, seq_args_t args);
typedef void (*seq_impl_iter_destroy_t)(seq_iter_t iter);
typedef seq_data_t (*seq_impl_iter_get_t)(seq_iter_t iter, seq_args_t args);
typedef seq_bool_t (*seq_impl_iter_set_t)(seq_iter_t iter, seq_args_t args);
typedef seq_bool_t (*seq_impl_iter_iterate_t)(seq_iter_t iter);

typedef struct _seq_impl_t* seq_impl_t;

struct _seq_impl_t {
	seq_impl_create_t create;
	seq_impl_destroy_t destroy;
	seq_impl_add_t add;
	seq_impl_remove_t remove;
	seq_impl_get_t get;
	seq_impl_set_t set;

	struct {
		seq_impl_iter_create_t create;
		seq_impl_iter_destroy_t destroy;
		seq_impl_iter_get_t get;
		seq_impl_iter_set_t set;
		seq_impl_iter_iterate_t iterate;
	} iter;
};

#define SEQ_TYPE(type) \
	static void seq_##type##_create(seq_t seq); \
	static void seq_##type##_destroy(seq_t seq); \
	static seq_bool_t seq_##type##_add(seq_t seq, seq_args_t args); \
	static seq_bool_t seq_##type##_remove(seq_t seq, seq_args_t args); \
	static seq_data_t seq_##type##_get(seq_t seq, seq_args_t args); \
	static seq_bool_t seq_##type##_set(seq_t seq, seq_args_t args); \
	static seq_iter_t seq_##type##_iter_create(seq_t seq, seq_args_t args); \
	static void seq_##type##_iter_destroy(seq_iter_t iter); \
	static seq_data_t seq_##type##_iter_get(seq_iter_t iter, seq_args_t args); \
	static seq_bool_t seq_##type##_iter_set(seq_iter_t iter, seq_args_t args); \
	static seq_bool_t seq_##type##_iter_iterate(seq_iter_t iter); \
	static struct _seq_impl_t SEQ_IMPL_##type = { \
		seq_##type##_create, \
		seq_##type##_destroy, \
		seq_##type##_add, \
		seq_##type##_remove, \
		seq_##type##_get, \
		seq_##type##_set, \
		{ \
			seq_##type##_iter_create, \
			seq_##type##_iter_destroy, \
			seq_##type##_iter_get, \
			seq_##type##_iter_set, \
			seq_##type##_iter_iterate \
		} \
	}; \
	seq_impl_t seq_impl_##type() { \
		return &SEQ_IMPL_##type; \
	}

struct _seq_t {
	seq_impl_t impl;
	seq_data_t data;

	struct {
		seq_on_add_t add;
		seq_on_remove_t remove;
	} on;

	seq_opt_t type;
	seq_size_t size;
};

struct _seq_iter_t {
	seq_t seq;
	seq_opt_t state;
};

seq_impl_t seq_impl_list();

#endif

