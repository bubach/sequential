#ifndef SEQ_API_H
#define SEQ_API_H

#include "../sequential.h"

#include <string.h>
#include <stdio.h>

/* TODO:
 * - seq_flatten(seq_t* seq): Convert data into a contiguous array_t.
 * - Additional SEQ_TYPE_* enumerations using techniques I find online; e.g., rb lists, etc.
 * - Compare performance against C++ STL.
 * - Create an explicit SEQ (or type) that takes advantage of Intel i5+ caching.
 * - Implement seq_insert(), seq_replace(), seq_append(), seq_prepend(), seq_empty(), etc; look
 *   at the C++ std::list manpage for more hints.
 * - Implement seq_cb(); when passed NULL returns the current seq_cb_t*, and when passed a valid
 *   pointer sets the seq_t instance using that seq_cb_t.
 * - Add some method by which lists created using seq_create_cb() can be easily identified by the
 *   user.
 * - SEQ_TYPE_MAP: An implementation using a node that keeps not only some data pointer, but
 *   another pointer used to identify that data.
 * - Potentially have a seq_args_t that is used to handle the va_list stuff consistently.
 * - seq_iterate_cb() (OR seq_apply(), instead)
 * - seq_iter_error()
 * - Mechanism for RW/RO looping modes; SEQ_ITER_RW_BIT, LSIT_ITER_RO_BIT? The seq_t itself will
 *   need a single @write_lock member, too.
 * - SEQ_ITER_FILTER (callback to skip, etc; borrow from Ruby...)
 * - SEQ_ITER_TIME (gather start, stop and average per-iteration times)
 * - SEQ_CB_{ADD,REMOVE,COMPARE,...} macros.
 * - DEBUG STUFF! Both via gdb AND printf,etc.
 * - Some way of creating a seq_iter_t instance locally. */

#if 0
TMP TMP TMP TMP TMP TMP
- hash
- sort
- performance
- gcc debug info text
- flatten
- allocate entire node to include "data" at end
- Add a SEQ_MULTI (or similar) argument that will make an argument like SEQ_APPEND expect multiple
  arguments until a NULL is encountered. e.g.
  seq_add(seq, SEQ_APPEND | SEQ_MULTI, "foo\0", 4, "bar\0", 4, "baz\0", 4, NULL);
#endif

struct _seq_t {
	seq_type_t type;
	seq_size_t size;
	seq_err_t error;
	seq_cb_t* callbacks;
};

typedef unsigned int seq_hdr_t;

#define SEQ_HDR_CB 0xDEADBEEF

struct _seq_cb_t {
	seq_hdr_t header;
	seq_cb_add_t add;
	seq_cb_remove_t remove;
	seq_cb_compare_t compare;
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

typedef struct _seq_impl_t {
	seq_t* (*create)(va_list args);
	void (*destroy)(seq_t* seq);
	seq_bool_t (*add)(seq_t* seq, va_list args);
	seq_bool_t (*remove)(seq_t* seq, va_list args);
	void* (*get)(seq_t* seq, va_list args);
} seq_impl_t;

extern const seq_impl_t SEQ_IMPL_LIST;
extern const seq_impl_t SEQ_IMPL_MAP;
extern const seq_impl_t SEQ_IMPL_RING;
extern const seq_impl_t SEQ_IMPL_QUEUE;
extern const seq_impl_t SEQ_IMPL_STACK;
extern const seq_impl_t SEQ_IMPL_ARRAY;
extern const seq_impl_t SEQ_IMPL_TEST;
extern const seq_impl_t SEQ_IMPL_ERROR;

seq_bool_t seq_err_any(seq_t* seq, ...);

#endif

