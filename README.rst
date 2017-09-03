TODO
====

* Implement support for set_get(seq, SEQ_DATA, "*") that will search for a node
  matching the passed-in data. This will also mean adding a new seq_cb_compare_t
  callback.

* Compare performance against C++ STL.

* SEQ_ITER_FILTER (callback to skip, etc; borrow from Ruby...)

* sort

* SEQ_ADD, SEQ_GET perform the default actions

* Implement SEQ_MULTI_APPEND; e.g., seq_add(seq, SEQ_MULTI_APPEND, 3, "foo",
  "bar", "baz");

* Introduce SEQ_LOCK and SEQ_RO/SEQ_RW to the seq_iter_t interface (to make
  multithreaded access easier).

* Be able to set default return values on error

* Add const where appropriate, especially for the SEQ_GET functions.

