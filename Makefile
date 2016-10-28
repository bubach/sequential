COMPILER = gcc
CFLAGS = $(shell cat .syntastic)
HEADERS = sequential.h seq/seq-api.h
OBJECTS = \
	seq/obj/seq-api.o \
	seq/obj/seq-array.o \
	seq/obj/seq-error.o \
	seq/obj/seq-list.o \
	seq/obj/seq-map.o \
	seq/obj/seq-queue.o \
	seq/obj/seq-ring.o \
	seq/obj/seq-stack.o \
	seq/obj/seq-test.o

seq/obj/%.o: seq/%.c
	$(COMPILER) $(CFLAGS) -c -o $(@) $(<)

default: build_test run_test

build_test: $(OBJECTS) $(HEADERS)
	@$(COMPILER) $(CFLAGS) -o test test.c $(OBJECTS)

run_test:
	@valgrind --leak-check=full ./test

clean:
	@rm -f seq/obj/*.o
	@rm -f test

edit:
	@vim sequential.h seq/seq-api.* seq/seq-list.c test.c
