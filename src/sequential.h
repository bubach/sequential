#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H 1

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
	#define SEQ_API __declspec(dllimport)

#else
	#define SEQ_API
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

/* ======================================================================== Types, Constants, Enums
 * seq_t
 * seq_bool_t
 * seq_size_t
 * seq_enum_t
 * seq_cb_add_t
 * seq_cb_remove_t
 * ============================================================================================= */

typedef struct _seq_t* seq_t;

typedef enum _seq_bool_t {
	SEQ_FALSE,
	SEQ_TRUE
} seq_bool_t;

typedef uint32_t seq_opt_t;

/* Sequential uses #int32_t (rather than #size_t) for its sizing and indexing API. This allows the
 * use of negative indices, indicating that the collection should be accessed/traversed in
 * reverse. */
typedef int32_t seq_size_t;

typedef va_list seq_args_t;
typedef void* seq_data_t;

#define seq_arg va_arg
#define seq_arg_size(args) va_arg(args, seq_size_t)
#define seq_arg_data(args) va_arg(args, seq_data_t)
#define seq_arg_opt(args) va_arg(args, seq_opt_t)

#define SEQ_NONE 0x0000

/* TODO: SEQ_MULTI_{APPEND,PREPEND,SEND,PUSH} */
#define SEQ_ADD 0x0100
#define SEQ_APPEND (SEQ_ADD | 0x0001)
#define SEQ_PREPEND (SEQ_ADD | 0x0002)
#define SEQ_BEFORE (SEQ_ADD | 0x0003)
#define SEQ_AFTER (SEQ_ADD | 0x0004)
#define SEQ_REPLACE (SEQ_ADD | 0x0005)
#define SEQ_SEND (SEQ_ADD | 0x0006)
#define SEQ_PUSH (SEQ_ADD | 0x0007)
#define SEQ_ADD_MAX SEQ_PUSH

#define SEQ_GET 0x0200
#define SEQ_INDEX (SEQ_GET | 0x0001)
#define SEQ_KEY (SEQ_GET | 0x0002)
#define SEQ_RECV (SEQ_GET | 0x0003)
#define SEQ_POP (SEQ_GET | 0x0004)
#define SEQ_GET_MAX SEQ_POP

#define SEQ_ITER 0x0300
#define SEQ_RANGE (SEQ_ITER | 0x0001)
#define SEQ_STRIDE (SEQ_ITER | 0x0002)
#define SEQ_ITER_MAX SEQ_STRIDE

#define SEQ_OPT 0x1000
#define SEQ_ARRAY (SEQ_OPT | 0x0001)
#define SEQ_MAP (SEQ_OPT | 0x0002)
#define SEQ_RING (SEQ_OPT | 0x0003)
#define SEQ_QUEUE (SEQ_OPT | 0x0004)
#define SEQ_STACK (SEQ_OPT | 0x0005)
#define SEQ_ON_ADD (SEQ_OPT | 0x0006)
#define SEQ_ON_REMOVE (SEQ_OPT | 0x0007)
#define SEQ_ADD_COPY (SEQ_OPT | 0x0008)
#define SEQ_REMOVE_FREE (SEQ_OPT | 0x0009)
#define SEQ_OPT_MAX SEQ_REMOVE_FREE

typedef seq_data_t (*seq_cb_add_t)(seq_args_t args);
typedef void (*seq_cb_remove_t)(seq_data_t data);

/* ======================================================================================= Core API
 * seq_create
 * seq_destroy
 * seq_add
 * seq_remove
 * seq_get
 * seq_set
 * seq_size
 * ============================================================================================= */

SEQ_API seq_t seq_create();

SEQ_API void seq_destroy(seq_t seq);

SEQ_API seq_bool_t seq_add(seq_t seq, ...);
SEQ_API seq_bool_t seq_vadd(seq_t seq, seq_args_t args);

SEQ_API seq_bool_t seq_remove(seq_t seq, ...);
SEQ_API seq_bool_t seq_vremove(seq_t seq, seq_args_t args);

SEQ_API seq_data_t seq_get(seq_t seq, ...);
SEQ_API seq_data_t seq_vget(seq_t seq, seq_args_t args);

SEQ_API seq_bool_t seq_set(seq_t seq, ...);
SEQ_API seq_bool_t seq_vset(seq_t seq, seq_args_t args);

SEQ_API seq_size_t seq_size(seq_t seq);

/* ================================================================================== Iteration API
 * seq_iter_create
 * seq_iter_data
 * seq_iter_index
 * seq_iter_stop
 * seq_iterate
 * ============================================================================================= */

typedef struct _seq_iter_t* seq_iter_t;

SEQ_API seq_iter_t seq_iter_create(seq_t seq, ...);

SEQ_API void seq_iter_destroy(seq_iter_t iter);

SEQ_API seq_data_t seq_iter_data(seq_iter_t iter);

SEQ_API seq_size_t seq_iter_index(seq_iter_t iter);

SEQ_API void seq_iter_stop(seq_iter_t iter);

SEQ_API seq_bool_t seq_iterate(seq_iter_t iter);

#ifdef __cplusplus
}
#endif

#endif

