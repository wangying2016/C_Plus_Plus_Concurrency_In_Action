void do_some_work();
std::thread my_thread(do_some_work);

class background_task
{
public:
    void operator()() const
    {
        do_something();
        do_something_else();
    }
};

background_task f;
std::thread my_thread(f);

std::thread my_thread((background_task()));
std::thread my_thread{background_task()};

std::thread my_thread([]{
    do_something();
    do_something_else();
});

struct func 
{
    int& i;
    func(int& i_) : i(i_) {} 
    void operator()()
    {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_something;           // 1.潜在访问隐患：悬空引用
        }
    }
};

void oops()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();         // 2. 不等待线程结束
                                // 3. 新线程可能还在运行
}

void do_some_work();
std::thread my_thread(do_some_work);

class background_task {
public:
    void operator()() const {
        do_something();
        do_something_else();
    }
};

background_task f;
std::thread my_thead(f);

std::thread my_thrad([]{
    do_something();
    do_something_else();
});

struct func; 
void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_funct);
    try {
        do_something_in_current_thread();
    } catch (...) {
        t.join();
        throw;
    }
    t.join();
}

class thread_guard 
{
    std::thread &t;
public:
    explicit thread_guard(std::thread &t_) : t(t_) {}
    ~thread_guard() {
        if (t.joinable()) {
            t.join()
        }
    }
    thread_guard(thread_guard const &) = delete;
    thread_guard &operator=(thread_guard const &) = delete;
};
struct func;

void f()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard g(t);
    do_something_in_current_thread();
}

std::thread t(do_background_work);
t.detach();
assert(!t.joinable());

void edit_document(std::string const &filename) 
{
    open_document_and_display_gui(filename);
    while (!done_editing()) {
        user_command cmd = get_user_input();
        if (cmd.type == open_new_document) {
            std::string const new_name = get_filename_from_user();
            std::thread t(edit_document, new_name);
            t.detach();
        } else {
            process_user_input(cmd);
        }
    }
}

void f(int i, const std::string &s);
std::thread t(f, 3, "hello");

void f(int i, std::string const &s);
void oops(int some_param) 
{
    char buffer[1024];
    sprintf(buffer, "%i", some_param);
    std::thread t(f, 3, buffer);
    t.detach();
}

void f(int i, std::string const &s);
void not_oops(int some_param)
{
    char buffer[1024];
    sprintf(buffer, "%i", some_param);
    std::thread t(f, 3, std::string(buffer));
    t.detach();
}

void update_data_for_widget(widget_id w, widget_data &data);
void oops_again(widget_id w) 
{
    widget_data data;
    std::threaad t(update_data_for_widget, w, data);
    display_status();
    t.join();
    process_widget_data(data);
}

std::thread t(update_data_for_widet, w, std::ref(data));

class X
{
public:
    void do_lengthy_work();
};

X my_x;
std::thread t(&X::do_lengthy_work, &my_x);

class X 
{
public:
    void do_lengthy_work(int);
};
X my_x;
int num(0);
std::thread t(&x::do_lengthy_work, &my_x, num);

void process_big_object(std::unique_ptr<big_object>);

std::unique_ptr<big_object> p(new big_object);
p->prepare_data(42);
std::thread t(process_big_object, std::move(p));

void some_function();
void some_other_function();
std::thread t1(some_function);
std::thread t2 = std::move(t1);
t1 = std::thread(some_other_function);
std::thread t3;
t3 = std::move(t2);
t1 = std::move(t3);

std::thread f()
{
    void some_function();
    return std::thread(some_function);
}

std::thread g()
{
    void some_other_function(int);
    std::thread t(some_other_function, 42);
    return t;
}

void f(std::thread t);
void g()
{
    void some_function();
    f(std::thread(some_function));
    std::thread t(some_function);
    f(std::move(t));
}

class scoped_thread 
{
    std::thread t;
public:
    explicit scoped_thread(std::thread t_):
        t(std::move(t_)) 
    {
        if (!t.joinable())
            throw std::logic_error("No thread");
    }
    ~scoped_thread()
    {
        t.join();
    }
    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;
};

struct func;

void f()
{
    int some_local_state;
    scoped_thread t(std::thread(func(some_local_state)));
    do_something_in_current_thread();
}

void do_work(unsigned id);

void f()
{
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < 20; ++i) 
        threads.push_back(std::thread(do_work, i));
    // 对每个线程调用 join()
    std::for_each(threads.begin(), threads.end(),
                    std::mem_fn(&std::thread::join));   
}

template<typename Iterator, typename T>
struct accumulate_block
{
    void operator()(Iterator first, Iterator last, T &result)
    {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsinged long const length = std::distance(first, last);

    if (!length)
        return init;

    unsigend long const min_per_thread = 25;
    unsigned long const max_threads = 
        (length + min_per_thread - 1) / min_per_thread;
    
    unsigned long const hardware_threads = 
        std::thread::hardware_concurrency();

    unsigned long const num_threads = 
        std::min(hardware_threads != 0 ? hardware_threads : 2,
        max_threads);

    unsigned long const block_size = length / num_threads;
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(
        block_start, last, results[num_threads - 1]);
    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);
}

template <typename Iterator, typename T>
struct accumulate_block
{
    void operator()(Iterator first, Iterator last, T &result)
    {
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);

    if (!length) return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = 
        (length + min_per_thread - 1) / min_per_thread;
    
    unsigned long const hardware_threads = 
        std::thread::hardware_concurrency();
    
    unsigned long const num_threads = 
        std::min(hardware_threads != 0 ? hardware_threads : 2,
        max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(
        block_start, last, results[num_threads - 1]);
    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);
}

std::thread::id master_thread;
void some_core_part_of_algorithm()
{
    if (std::this_thread::get_id() == master_thread) {
        do_master_thread_work();
    }
    do_common_work();
}

std::cout << std::this_thread::get_id();