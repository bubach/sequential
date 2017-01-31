#ifndef TEST_H
#define TEST_H

#include "../sequential.h"

#include <stdio.h>

#define TERM_ESC "\033["
#define TERM_RESET "0"
#define TERM_RESET_UNDERLINE "24"
#define TERM_RESET_REVERSE "27"
#define TERM_DEFAULT "39"
#define TERM_DEFAULTB "49"
#define TERM_BOLD "1"
#define TERM_BRIGHT "2"
#define TERM_UNDERSCORE "4"
#define TERM_REVERSE "7"
#define TERM_BLACK "30"
#define TERM_RED "31"
#define TERM_GREEN "32"
#define TERM_BROWN "33"
#define TERM_BLUE "34"
#define TERM_MAGENTA "35"
#define TERM_CYAN "36"
#define TERM_WHITE "37"
#define TERM_BLACKB "40"
#define TERM_REDB "41"
#define TERM_GREENB "42"
#define TERM_BROWNB "43"
#define TERM_BLUEB "44"
#define TERM_MAGENTAB "45"
#define TERM_CYANB "46"
#define TERM_WHITEB "47"

#define SEQ_TEST_BEGIN(name) \
seq_bool_t test_##name(const char* descr) { \
	seq_t seq = seq_create(); \
	seq_bool_t r = SEQ_TRUE; \
	printf("============================================================\n"); \
	printf("test_%s: %s\n", #name, descr); \
	printf("============================================================\n"); {

#define SEQ_TEST_END } \
done: \
	printf("\n"); \
	if(seq) seq_destroy(seq); \
	return r; \
}

#define SEQ_ASSERT(expr) \
	if(!(expr)) { \
		printf(" >> [" TERM_ESC TERM_RED "mFAIL" TERM_ESC TERM_RESET "m] " #expr "\n"); \
		r = SEQ_FALSE; \
		goto done; \
	} \
	else printf(" >> [" TERM_ESC TERM_GREEN "mPASS" TERM_ESC TERM_RESET "m] " #expr "\n");

void test_info(const char* fmt, ...) {
	va_list args;
	char buffer[1024];

	va_start(args, fmt);

	sprintf(buffer, " >> [" TERM_ESC TERM_CYAN "mINFO" TERM_ESC TERM_RESET "m] %s\n", fmt);
	vprintf(buffer, args);

	va_end(args);
}

#endif

