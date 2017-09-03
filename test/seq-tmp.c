#include <sequential.h>

#include <stdio.h>

void tmp_printf(seq_t seq) {
	seq_size_t size = seq_size(seq);
	seq_size_t i;

	printf("seq.size = %d\n", size);

	for(i = 0; i < size; i++) printf(
		"seq[%d] = %s\n", i, (const char*)(seq_get(seq, SEQ_INDEX, i).data)
	);
}

int main(int argc, char** argv) {
	seq_t seq = seq_create(SEQ_LIST);

	seq_add(seq, SEQ_APPEND, "foo");
	seq_add(seq, SEQ_APPEND, "bar");
	seq_add(seq, SEQ_APPEND, "baz");

	printf("seq.type = %s\n", seq_string(seq_type(seq)));

	tmp_printf(seq);

	seq_remove(seq, SEQ_INDEX, 2);

	tmp_printf(seq);

	seq_destroy(seq);

	return 0;
}

