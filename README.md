## light coroutine c++ api & coroutine Chan like as goroutine chan <- 

examples: test1 test2 test3 test4

```{cpp}
NoodleManager nm;  

/*
 * create a new coroutine with specific name.
 * this function can also be invoked from any coroutine.
 *
 * name: the routine name, can be same with other coroutine
 * f: the routine entry, like as void entry(Noodle* noodle) {} 
 * data: user data, default nullptr
 * stack_size: routine stack size, default 8K
 *
 * the coroutine won't be running until NoodleManager::start
*/
nm.new_noodle(const std::string& name, Noodle::Entry f, void *data, int stack_size);

/*
 * start the coroutines 
 * the main coroutine will block until all routines end or anyone called NoodleManager::exit
 *
*/
nm.start();

/* 
 * ask NoodleManager exit
 * can only call from any coroutine, this can make NoodleManager::start exit.
*/
nm.exit();

/*
 * remove routine with specific name
 *
*/
nm.remove(const std::string& name);
nm.remove_all(const std::string& name);

/*
 * make specific routine as the immediate next one after yield
 *
*/
nm.set_next(const std::string& name);


/*
 * create and return a channel
*/
Chan<T,Size> *chan = nm.new_chan<T,Size>();

/* 
 * channel operation
 * write T from chan, will automatic yield when blocked
*/
chan->put(noodle*, const T&);

/*
 * channel operation
 * read T from chan, will automatic yield when blocked
*/
const T& chan->get(noodle*);
```

