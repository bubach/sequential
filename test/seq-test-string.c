#include <sequential.h>

#include <stdio.h>

void test_seq_string(seq_opt_t opt, const char* val) {
	printf("%08X: %s = %s\n", opt, val, seq_string(opt));
}

int main(int argc, char** argv) {
	test_seq_string(SEQ_TYPE, "SEQ_TYPE");
	test_seq_string(SEQ_LIST, "SEQ_LIST");
	test_seq_string(SEQ_MAP, "SEQ_MAP");
	test_seq_string(SEQ_RING, "SEQ_RING");
	test_seq_string(SEQ_QUEUE, "SEQ_QUEUE");
	test_seq_string(SEQ_STACK, "SEQ_STACK");
	test_seq_string(SEQ_ARRAY, "SEQ_ARRAY");

	test_seq_string(SEQ_CONFIG, "SEQ_CONFIG");
	test_seq_string(SEQ_CB_ADD, "SEQ_CB_ADD");
	test_seq_string(SEQ_CB_REMOVE, "SEQ_CB_REMOVE");
	test_seq_string(SEQ_SORTED, "SEQ_SORTED");
	test_seq_string(SEQ_BLOCKING, "SEQ_BLOCKING");

	test_seq_string(SEQ_ADD, "SEQ_ADD");
	test_seq_string(SEQ_APPEND, "SEQ_APPEND");
	test_seq_string(SEQ_PREPEND, "SEQ_PREPEND");
	test_seq_string(SEQ_BEFORE, "SEQ_BEFORE");
	test_seq_string(SEQ_AFTER, "SEQ_AFTER");
	test_seq_string(SEQ_REPLACE, "SEQ_REPLACE");
	test_seq_string(SEQ_KEYVAL, "SEQ_KEYVAL");
	test_seq_string(SEQ_SEND, "SEQ_SEND");
	test_seq_string(SEQ_PUSH, "SEQ_PUSH");

	test_seq_string(SEQ_GET, "SEQ_GET");
	test_seq_string(SEQ_INDEX, "SEQ_INDEX");
	test_seq_string(SEQ_KEY, "SEQ_KEY");
	test_seq_string(SEQ_RECV, "SEQ_RECV");
	test_seq_string(SEQ_POP, "SEQ_POP");
	test_seq_string(SEQ_DATA, "SEQ_DATA");

	test_seq_string(SEQ_ITER, "SEQ_ITER");
	test_seq_string(SEQ_READY, "SEQ_READY");
	test_seq_string(SEQ_ACTIVE, "SEQ_ACTIVE");
	test_seq_string(SEQ_STOP, "SEQ_STOP");
	test_seq_string(SEQ_RANGE, "SEQ_RANGE");
	test_seq_string(SEQ_INC, "SEQ_INC");

	test_seq_string(SEQ_CMP, "SEQ_CMP");
	test_seq_string(SEQ_LESS, "SEQ_LESS");
	test_seq_string(SEQ_EQUAL, "SEQ_EQUAL");
	test_seq_string(SEQ_GREATER, "SEQ_GREATER");

	test_seq_string(SEQ_ERR, "SEQ_ERR");
	test_seq_string(SEQ_ERR_NONE, "SEQ_ERR_NONE");
	test_seq_string(SEQ_ERR_OPT, "SEQ_ERR_OPT");
	test_seq_string(SEQ_ERR_MEM, "SEQ_ERR_MEM");
	test_seq_string(SEQ_ERR_DATA, "SEQ_ERR_DATA");
	test_seq_string(SEQ_ERR_NODE, "SEQ_ERR_NODE");
	test_seq_string(SEQ_ERR_CB, "SEQ_ERR_CB");
	test_seq_string(SEQ_ERR_TODO, "SEQ_ERR_TODO");

	test_seq_string(0x10101010, "(null)");

	return 0;
}
