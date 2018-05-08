#include <sequential.h>

#include <stdio.h>

void tmp_printf(seq_t seq) {
	seq_size_t size = seq_size(seq);
	seq_size_t i;

	printf("seq.size = %lu\n", size);

	for(i = 0; i < size; i++) printf(
		"seq[%lu] = %s\n", i, (const char*)(seq_get(seq, SEQ_INDEX, i))
	);
}

int main(int argc, char** argv) {
	seq_t seq = seq_create(SEQ_LIST);
	seq_opt_t err = SEQ_ERR_NONE;

	/* Append 3 values. */
	seq_add(seq, SEQ_APPEND, "foo");
	seq_add(seq, SEQ_APPEND, "bar");
	seq_add(seq, SEQ_APPEND, "baz");

	printf("seq.type = %s\n", seq_string(seq_type(seq)));

	tmp_printf(seq);

	/* Remove last value. */
	seq_remove(seq, SEQ_INDEX, 2);

	tmp_printf(seq);

	/* Replace an existing value. */
	if((err = seq_set(seq, SEQ_INDEX, 0, "FOO"))) {
		printf("Error: %s (%08X)\n", seq_string(err), err);
	}

	tmp_printf(seq);

	seq_destroy(seq);

	return 0;
}

#if 0
/* --------------------------------------------------------------------------------------------- */
seq_add(seq, SEQ_APPEND, "foo");
seq_add(seq, SEQ_APPEND, "bar");
seq_add(seq, SEQ_APPEND, "baz");

seq_get_t data = seq_get(seq, SEQ_INDEX, 1);

set_set(seq, SEQ_INDEX, 1, "FOO");

/* --------------------------------------------------------------------------------------------- */
seq_iter_t iter = seq_iter_create(...);

while(seq_iterate(iter)) {
	seq_get_t g = seq_iter_get(iter);

	printf("g->handle.index = %d\n", g->handle.index);
	printf("g->data = %p\n", g->data);

	seq_iter_set(g, make_object());
}

seq_iter_destroy(iter);

/* --------------------------------------------------------------------------------------------- */
seq_t seq = seq_create(SEQ_MAP);
seq_iter_t iter0 = seq_iter_create(...);
seq_iter_t iter1 = seq_iter_create(...);

while(seq_iterate_n(2, iter0, iter1)) {
	seq_get_t g0 = seq_iter_get(iter0);
	seq_get_t g1 = seq_iter_get(iter1);

	seq_add(seq, SEQ_KEYVAL, g0->data, g1->data);
}

seq_iter_destroy(iter0);
seq_iter_destroy(iter1);

#endif

