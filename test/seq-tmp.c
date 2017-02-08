#include <sequential.h>

#include <stdio.h>

int main(int argc, char** argv) {
	seq_t seq = seq_create(SEQ_LIST);

	seq_add(seq, SEQ_APPEND, "foo");
	seq_add(seq, SEQ_APPEND, "bar");
	seq_add(seq, SEQ_APPEND, "baz");

	printf("seq.size = %d\n", seq_size(seq));
	printf("seq.type = %s\n", seq_opt_str(seq_type(seq)));
	printf("seq[0] = %s\n", (const char*)(seq_get(seq, SEQ_INDEX, 0).data));

	seq_destroy(seq);

	return 0;
}

