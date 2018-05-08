API
===

.. code:: cpp
   /* A simple structure--and a corresponding functions for creating
    * and destroying these structures--that we'll use to demonstrate
    * how Sequential manages data requiring more than a simple free()
    * on destruction. */
   typedef struct _str_t {
      const char* data;
      size_t len;
   } str_t;

   /* Creates a new str_t instance using a malloc'd copy of the
    * passed-in @data parameter. */
   str_t* str_create(const char* data) {
      str_t* s = (str_t*)(malloc(sizeof(str_t)));
      size_t len = strlen(data);

      r.data = (const char*)(malloc(len + 1));
      r.len = len;

      strcpy(r.data, data);

      return s;
   }

   /* The callback used by a seq_t instance when data is removed
    * from the sequence OR the container is destroyed. */
   void str_destroy(seq_data_t data) {
      str_t* str = (str_t*)(data);

      free(str.data);
      free(str);
   }

   unsigned int* buf_create(size_t size, unsigned int fill) {
      return NULL;
   }

   /* ... */
   seq_t list = seq_create(SEQ_LIST);
   seq_t map = seq_create(SEQ_MAP);

   /* ... */
   seq_config(list, SEQ_CB_REMOVE, SEQ_FREE);
   seq_config(map, SEQ_CB_REMOVE, SEQ_FREE);

   /* Set blocking support. */
   seq_config(list, SEQ_BLOCK, 1000, 100);

   /* ... */
   seq_add(list, SEQ_APPEND, str("foo"));
   seq_add(list, str("bar"));
   seq_add(list, str("baz"));

   /* ... */
   seq_add(map, "foo", str("FOO"));
   seq_add(map, "bar", str("BAR"));
   seq_add(map, "baz", str("BAZ"));

   /* Simple, direct, const data access... */
   const str_t* s0 = seq_get(list, 1);
   const str_t* s1 = seq_get(map, "baz");

   /* Data access using the locking API... */
   seq_lock_t lock;

   if((lock = seq_lock(list, 0))) {
      str_t* str = seq_data(lock);
      seq_size_t index = seq_index(lock);

      seq_unlock(lock);
   }

   /* Data using iter... */
   seq_iter_t iter = seq_iter_create(map, ...);

   while(seq_iterate(iter)) {
      str_t* str = seq_data(iter);
      const char* key = seq_key(iter);
   }

   seq_iter_destroy(iter);

   /* ... */
   seq_iter_t iter0 = seq_iter_create(list, ...);
   seq_iter_t iter1 = seq_iter_create(map, ...);

   while(seq_enumerate(2, iter0, iter1)) {
      str_t* s0 = seq_data(iter0);
      seq_size_t i = seq_index(iter0);

      str_t* s1 = seq_data(iter1);
      const char* k = seq_key(iter1);
   }

   seq_iter_destroy(iter0);
   seq_iter_destroy(iter1);

