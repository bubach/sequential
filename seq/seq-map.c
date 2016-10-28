#include "seq-api.h"

typedef struct _seq_map_node_t seq_map_node_t;

typedef struct _seq_map_data_t {
	void* key;
	void* value;
} seq_map_data_t;

struct _seq_map_node_t {
	seq_map_data_t data;
	seq_map_node_t* left;
	seq_map_node_t* right;
	seq_map_node_t* parent;
	seq_bool_t red;
};

typedef struct _seq_map_t {
	seq_t seq;
	seq_map_node_t* root;
	seq_map_node_t* nil;
} seq_map_t;

#define seq_map(seq) ((seq_map_t*)(seq))

void seq_map_rotate_left(seq_t* seq, seq_map_node_t* x) {
  seq_map_t* tree = seq_map(seq);
  seq_map_node_t* y;
  seq_map_node_t* nil=tree->nil;

  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls LeftRotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when RBDeleteFixUP */
  /*  calls LeftRotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  y=x->right;
  x->right=y->left;

  if (y->left != nil) y->left->parent=x; /* used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */

  y->parent=x->parent;

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  if( x == x->parent->left) {
    x->parent->left=y;
  } else {
    x->parent->right=y;
  }
  y->left=x;
  x->parent=y;
}

static seq_t* seq_map_create(va_list args) {
	return NULL;
}

static void seq_map_destroy(seq_t* seq) {
}

static seq_bool_t seq_map_add(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_map_remove(seq_t* seq, va_list args) {
	return SEQ_FALSE;
}

static void* seq_map_get(seq_t* seq, va_list args) {
	return NULL;
}

const seq_impl_t SEQ_IMPL_MAP = {
	seq_map_create,
	seq_map_destroy,
	seq_map_add,
	seq_map_remove,
	seq_map_get,
};

