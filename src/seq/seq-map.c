#include "seq-api.h"

/* ======================================================================== Types, Constants, Enums
 * struct _seq_map_node_t
 * seq_map_data
 * SEQ_TYPE_API(map)
 * --------------------------------------------------------------------------------------------- */

typedef struct _seq_map_node_t* seq_map_node_t;

#define SEQ_MAP_LEFT 0
#define SEQ_MAP_RIGHT 1

struct _seq_map_node_t {
	seq_data_t data;
	seq_map_node_t link[2];
	seq_bool_t red;
};

/* seq->data ends up being the "root" node. */
#define seq_map_data(seq) (seq_map_node_t)(seq->data)

SEQ_TYPE_API(map)

/* ============================================================================ Private Map Helpers
 * seq_map_node_rotate
 * seq_map_node_rotate2
 * ============================================================================================= */

static int seq_map_node_is_red(const seq_map_node_t node) {
	return node ? node->red : 0;
}

static seq_map_node_t seq_map_node_rotate(seq_map_node_t node, seq_opt_t dir) {
	seq_map_node_t result = node->link[!dir];

	node->link[!dir] = result->link[dir];
	node->red = SEQ_TRUE;

	result->link[dir] = node;
	result->red = SEQ_FALSE;

	return result;
}

static seq_map_node_t seq_map_node_rotate2(seq_map_node_t node, seq_opt_t dir) {
	seq_map_node_t result = NULL;

	node->link[!dir] = seq_map_node_rotate(node->link[!dir], !dir);

	result = seq_map_node_rotate(node, dir);

	return result;
}

/* ======================================================================== SEQ_LIST Implementation
 * seq_map_create
 * seq_map_destroy
 * seq_map_add
 * seq_map_remove
 * seq_map_get
 * seq_map_set
 * ============================================================================================= */

static void seq_map_create(seq_t seq) {
	seq->type = SEQ_MAP;
	seq->impl = seq_impl_map();
}

static void seq_map_destroy(seq_t seq) {
	/* seq_map_node_t node = seq_map_data(seq);

	seq_map_node_destroy(seq, node);

	free(data); */
}

static seq_bool_t seq_map_add(seq_t seq, seq_args_t args) {
	seq_map_node_t node = NULL;
	seq_map_node_t root = seq_map_data(seq);
	seq_opt_t add = seq_arg_opt(args);

	if(add != SEQ_KEYVAL) goto err;

	if(!(node = seq_malloc(seq_map_node_t))) goto err;

	node->red = SEQ_TRUE;

	/* if(!(node->data = seq_map_node_data(seq, args))) goto err; */

	if(!root) root = node;

	else {
		struct _seq_map_node_t head; /* False tree root. */

		seq_map_node_t g;
		seq_map_node_t t; /* Grandparent & parent. */
		seq_map_node_t p;
		seq_map_node_t q; /* Iterator & parent. */

		int dir = 0;
		int last = 0;

		head.red = SEQ_FALSE;

		t = &head;
		g = p = NULL;
		q = t->link[1] = root;

		/* Search down the tree for a place to insert. */
		while(SEQ_TRUE) {
			/* Insert node at the first null link. */
			if(!q) p->link[dir] = q = node;

			/* Simple red violation: color flip. */
			else if(seq_map_node_is_red(q->link[0]) && seq_map_node_is_red(q->link[1])) {
				q->red = 1;
				q->link[0]->red = 0;
				q->link[1]->red = 0;
			}

			/* Hard red violation: rotations necessary. */
			if(seq_map_node_is_red(q) && seq_map_node_is_red(p)) {
				int dir2 = t->link[1] == g;

				if(q == p->link[last]) t->link[dir2] = seq_map_node_rotate(g, !last);

				else t->link[dir2] = seq_map_node_rotate2(g, !last);
			}

#if 0
			/* Stop working if we inserted a node. This check also disallows duplicates in the
			 * tree. */
			/* TODO: THIS !!!!!!!!!!!!!!!!!!!!!!!!! */
			/* if(!self->cmp(self, q, node)) break; */

			last = dir;
			dir = self->cmp(self, q, node) < 0;

			/* Move the helpers down. */
			if(g) t = g;
#endif

			g = p, p = q;
			q = q->link[dir];
		}

		/* Update the root (it may be different). */
		/* self->root = head.link[1]; */
	}

	/* Make the root black for simplified logic. */
	self->root->red = 0;

	/* ++self->size; */

	return SEQ_TRUE;

err:
	/* if(node) seq_map_node_destroy(seq, node); */

	return SEQ_FALSE;
}

static seq_bool_t seq_map_remove(seq_t seq, seq_args_t args) {
	return SEQ_FALSE;
}

static seq_get_t seq_map_get(seq_t seq, seq_args_t args) {
	struct _seq_map_node_t node;
	seq_map_node_t n = seq_map_data(seq);
	seq_opt_t cmp = SEQ_EQUAL;

	/* node.value = value; */

	while(n) {
		if((cmp = seq->cb.compare(seq, n, &node))) n = n->link[cmp == SEQ_LESS];

		else break;
	}

	return seq_got_key(n->data, NULL);

	/* return n ? n->data: NULL; */
}

static seq_bool_t seq_map_set(seq_t seq, seq_opt_t set, seq_args_t args) {
	return SEQ_FALSE;
}

/* ============================================================== SEQ_LIST Iteration Implementation
 * seq_map_iter_create
 * seq_map_iter_destroy
 * seq_map_iter_get
 * seq_map_iter_set
 * seq_map_iter_iterate
 * ============================================================================================= */

static void seq_map_iter_create(seq_iter_t iter, seq_args_t args) {
}

static void seq_map_iter_destroy(seq_iter_t iter) {
}

static seq_get_t seq_map_iter_get(seq_iter_t iter, seq_args_t args) {
	return seq_got_null();
}

static seq_bool_t seq_map_iter_set(seq_iter_t iter, seq_args_t args) {
	return SEQ_FALSE;
}

static seq_bool_t seq_map_iter_iterate(seq_iter_t iter) {
	return SEQ_TRUE;
}

