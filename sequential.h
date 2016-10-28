#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

/* TODO: The va_list usage here will only work with MSVC 2013 and higher. */

#define SEQ_API_BEGIN
#define SEQ_API_END
#define SEQ_API

#if defined(SEQ_API_PUBLIC)
	#ifdef  __cplusplus
		#define SEQ_API_BEGIN extern "C" {
		#define SEQ_API_END }
	#endif

	#if defined(_MSC_VER)
		#define SEQ_API __declspec(dllimport)
	#endif
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

SEQ_API_BEGIN

/* ======================================================================== Types, Constants, Enums
 * seq_t
 * seq_cb_t
 * seq_bool_t
 * seq_size_t
 * seq_type_t
 * seq_enum_t
 * seq_group_t
 * seq_cmp_t
 * seq_cb_add_t
 * seq_cb_remove_t
 * seq_cb_compare_t
 * ============================================================================================= */

typedef struct _seq_t seq_t;
typedef struct _seq_cb_t seq_cb_t;

typedef enum _seq_bool_t {
	SEQ_FALSE,
	SEQ_TRUE
} seq_bool_t;

/* Sequential uses #int32_t (rather than #size_t) for its sizing and indexing API. This allows the
 * use to specify negative indices, indicating that the collection should be accessed/traversed in
 * reverse. */
typedef int32_t seq_size_t;

/* Many of the collections/objects supported by Sequential share a large amount of common code and
 * can be easily treated as a differing type without any harmful side effects. To this end,
 * Sequential wraps each instance in an opaque #seq_t "context", abstracting the gritty details of
 * interacting with the object using a clean--and expressive--public API. The #seq_type_t enum
 * represents how the #seq_t instance is implemented internally. It is always the first argument
 * passed to seq_create(), and can be queried using seq_type(). */
typedef enum _seq_type_t {
	/* A basic, doubly-linked list, where each node is indexed numerically. */
	SEQ_LIST,

	/* Each node is indexed by a key, as well as numerically. */
	SEQ_MAP,

	/* The @back node always points to the @front, allowing for endless, repeating iteration. */
	SEQ_RING,

	/* A "first in, first out" collection; uses SEQ_SEND and SEQ_RECV. */
	SEQ_QUEUE,

	/* A "last in, first out" collection; uses SEQ_PUSH and SEQ_POP. */
	SEQ_STACK,

	/* Requires that the sequence's size is specified explicity. This is also the type a #seq_t
	 * instance is converted to when it seq_flatten() is called. */
	SEQ_ARRAY,

	/* Special type used only for testing. */
	SEQ_TEST,

	/* The static, const, built-in seq_t instance used when all else fails. */
	SEQ_ERROR
} seq_type_t;

#define SEQ_ENUM(v) (1 << v)

typedef enum _seq_enum_t {
	SEQ_NULL = 0,

	SEQ_COPY = SEQ_ENUM(0),
	SEQ_REF = SEQ_ENUM(1),
	SEQ_USER = SEQ_ENUM(2),

	SEQ_APPEND = SEQ_ENUM(3),
	SEQ_PREPEND = SEQ_ENUM(4),
	SEQ_SEND = SEQ_ENUM(5),
	SEQ_PUSH = SEQ_ENUM(6),

	SEQ_BEFORE = SEQ_ENUM(7),
	SEQ_AFTER = SEQ_ENUM(8),
	SEQ_REPLACE = SEQ_ENUM(9),

	SEQ_INDEX = SEQ_ENUM(10),
	SEQ_DATA = SEQ_ENUM(11),
	SEQ_RECV = SEQ_ENUM(12),
	SEQ_POP = SEQ_ENUM(13),

	SEQ_RANGE = SEQ_ENUM(14),
	SEQ_STRIDE = SEQ_ENUM(15),

	SEQ_MAX = SEQ_ENUM(16)
} seq_enum_t;

typedef enum _seq_group_t {
	SEQ_CREATE = SEQ_COPY | SEQ_REF | SEQ_USER,
	SEQ_ADD = SEQ_APPEND | SEQ_PREPEND | SEQ_SEND | SEQ_PUSH,
	SEQ_GET = SEQ_INDEX | SEQ_DATA | SEQ_RECV | SEQ_POP,
	SEQ_ADD_GET = SEQ_BEFORE | SEQ_AFTER | SEQ_REPLACE,
	SEQ_ITER = SEQ_RANGE | SEQ_STRIDE,
	SEQ_ANY = SEQ_MAX - 1
} seq_group_t;

typedef enum _seq_cmp_t {
	SEQ_LESS = -1,
	SEQ_EQUAL = 0,
	SEQ_GREATER = 1,
	SEQ_PASS = 2
} seq_cmp_t;

typedef void* (*seq_cb_add_t)(seq_t* seq, va_list args);
typedef void (*seq_cb_remove_t)(seq_t* seq, void* data);
typedef seq_cmp_t (*seq_cb_compare_t)(seq_t* seq, void* data, va_list args);

/* ====================================================================================== Error API
 * seq_err_code_t
 * seq_err_type_t
 * seq_err_t
 * seq_err
 * seq_err_malloc
 * seq_err_null
 * seq_err_enum
 * seq_err_arg
 * seq_err_todo
 * seq_err_user
 * ============================================================================================= */

typedef char seq_err_code_t[7];

typedef enum _seq_err_type_t {
	SEQ_ERR_NONE,

	/* A call to malloc failed internally. */
	SEQ_ERR_MALLOC,

	/* A NULL pointer was passed into the API. */
	SEQ_ERR_NULL,

	/* An enum value within Sequential was specified incorrectly or not at all. */
	SEQ_ERR_ENUM,

	/* An expected argument within the API was specified incorrectly or not at all. */
	SEQ_ERR_ARG,

	/* For features that aren't implemented yet, this error is returned. */
	SEQ_ERR_TODO,

	/* A user-defined error. */
	SEQ_ERR_USER
} seq_err_type_t;

typedef struct _seq_err_t {
	seq_err_code_t code;
	seq_err_type_t type;
} seq_err_t;

SEQ_API seq_err_t seq_err(seq_err_type_t type, const char* code);

#define SEQ_ERR(type) { "\0\0\0\0\0\0", SEQ_ERR_##type }

#define seq_err_malloc(code) seq_err(SEQ_ERR_MALLOC, code)
#define seq_err_null(code) seq_err(SEQ_ERR_NULL, code)
#define seq_err_enum(code) seq_err(SEQ_ERR_ENUM, code)
#define seq_err_arg(code) seq_err(SEQ_ERR_ARG, code)
#define seq_err_todo(code) seq_err(SEQ_ERR_TODO, code)
#define seq_err_user(code) seq_err(SEQ_ERR_USER, code)

/* ======================================================================================= Core API
 * seq_create
 * seq_destroy
 * seq_add
 * seq_remove
 * seq_get
 * seq_type
 * seq_size
 * seq_valid
 * seq_error
 * ============================================================================================= */

/* Creates a new #seq_t instance corresponding to the passed-in $type parameter. Depending on the
 * type of Sequential collection being created, additional arguments may be required (see the
 * documentation for #seq_type_t).
 *
 * A valid #seq_t is ALWAYS returned (even when malloc() fails a static, internal instance is
 * returned). However, any errors that may have occurred should be queried using seq_valid() and/or
 * seq_error(). */
SEQ_API seq_t* seq_create(seq_type_t type, ...);
SEQ_API seq_t* seq_vcreate(seq_type_t type, va_list args);

/* Properly deconstructs the passed-in $seq instance, first recursively destroying all attached
 * children (by calling the corresponding #seq_cb_remove_t callback) and finally destroying the
 * #seq_t instance itself.
 *
 * This routine will *IGNORE* the error state of $seq, and will always attempt to destroy the
 * passed-in instance (if non-NULL). */
SEQ_API void seq_destroy(seq_t* seq);

SEQ_API seq_bool_t seq_add(seq_t* seq, ...);
SEQ_API seq_bool_t seq_vadd(seq_t* seq, va_list args);

SEQ_API seq_bool_t seq_remove(seq_t* seq, ...);
SEQ_API seq_bool_t seq_vremove(seq_t* seq, va_list args);

SEQ_API void* seq_get(seq_t* seq, ...);
SEQ_API void* seq_vget(seq_t* seq, va_list args);

SEQ_API seq_type_t seq_type(seq_t* seq);

SEQ_API seq_size_t seq_size(seq_t* seq);

SEQ_API seq_bool_t seq_valid(seq_t* seq);

SEQ_API seq_err_t seq_error(seq_t* seq);

/* =================================================================================== Callback API
 * seq_arg_enum
 * seq_arg_size
 * seq_arg_data
 * seq_arg_cb
 * ============================================================================================= */

SEQ_API seq_bool_t seq_arg_enum(seq_t* seq, seq_enum_t* e, va_list args);

SEQ_API seq_bool_t seq_arg_size(seq_t* seq, seq_size_t* size, va_list args);

SEQ_API seq_bool_t seq_arg_data(seq_t* seq, void** data, va_list args);

SEQ_API seq_bool_t seq_arg_cb(seq_t* seq, seq_cb_t** cb, va_list args);

#if 0
/* =================================================================================== Iterator API
 * ============================================================================================= */
typedef struct _seq_iter_t {
	uint8_t _private[32];
} seq_iter_t;

SEQ_API seq_iter_t* seq_iter_create(seq_enum_t opt, ...);

SEQ_API void* seq_iter_data(seq_iter_t* iter);

SEQ_API size_t seq_iter_index(seq_iter_t* iter);

SEQ_API void seq_iter_stop(seq_iter_t* iter);

SEQ_API seq_bool_t seq_iterate(seq_t* seq, seq_iter_t* iter);

/* ... */
typedef void (*seq_cb_apply_t)(void* data, size_t index, va_list args);

SEQ_API void seq_apply(seq_t* seq, seq_cb_apply_t apply, seq_enum_t opt, ...);
#endif

SEQ_API_END

#endif

