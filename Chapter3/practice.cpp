#include <list>
#include <mutex>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find)
{
    std::lock_guard<std::mutex> guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), vlaue_to_find) != some_list.end();
}

class some_data
{
    int a;
    std::string b;
public:
    void do_something();
};

class data_wrapper
{
private:
    some_data data;
    std::mutex m;
public:
    template <typename Function>
    void process_data(Function func)
    {
        std::lock_guard<std::mutex> l(m);
        func(data);
    }
};

some_data *unprotected;

void malicious_function(some_data &protected_data)
{
    unprotected = &protected_data;
}

data_wrapper x;
void foo()
{
    x.process_data(malicious_function);
    unprotected->do_something();
}

template <typename T, typename Container = std::deque<T> >
class stack
{
public:
    explicit stack(const Container&);
    explicit stack(Container&& = container());
    template <class Alloc> explicit statck(const Alloc&);
    template <class Alloc> stack(const Container&, const Alloc&);
    template <class Alloc> stack(Container&&, const Alloc&);
    template <class Alloc> stack(stack&&, const Alloc&);

    bool empty() const;
    size_t size() const;
    T& top();
    T const& top() const;
    void push(T const&);
    void push(T&&);
    void pop();
    void swap(stack&&);
};

#include <exception>
#include <memory>

struct empty: std::exception
{
    const char* what() const throw();
};

template <typename T>
class threadsafe_stack
{
public:
    threadsafe_stack();
    threadsafe_stack(const threadsafe_stack&);
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_value);
    std::shared_ptr<T> pop();
    void pop(T& value);
    bool empty() const;
};

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack: std::exception
{
    const char *what() const throw() {
        return "empty stack!";
    }
};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;

public:
    threadsafe_stack() : data(std::stack<T>()) {}

    threadsafe_stack(const threadsafe_stack &other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) throw empty_stack();

        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }

    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) throw empty_stack();

        value = data.top();
        data.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

// 这里的 std::lock() 需要包含 <mutex> 头文件
class some_big_object;
void swap(some_big_object &lhs, some_big_object &rhs);
class X 
{
private:
    some_big_object some_detail;
    std::mutext m;
public:
    X(some_big_object const &sd): some_detail(sd) {} 

    friend void swap(X& lhs, X& rhs)
    {
        if (&lhs == &rhs) return;
        std::lock(lhs.m, rhs.m);
        std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
        std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
        swap(lhs.some_detail, rhs.some_detail);
    }
};

hierarichical_mutex high_level_mutex(10000);
hierarichical_mutex low_level_mutex(5000);

int do_low_level_stuff();

int low_level_stuff()
{
    std::lock_guard<hierarichical_mutex> lk(low_level_mutex);
    return do_low_level_stuff();
}

void high_level_stuff(int some_param);

void high_level_func()
{
    std::lock_guard<hierarichical_mutex> lk(high_level_mutex);
    high_level_stuff(low_level_func());
}

void thread_a()
{
    high_level_func();
}

hierarichical_mutex other_mutex(100);
void do_other_stuff();

void other_stuff()
{
    high_level_func();
    do_other_stuff();
}

void thread_b()
{
    std::lock_guard<hierarichical_mutex> lk(other_mutex);
    other_stuff;
}

// 简单的层级互斥量实现
class hierarichical_mutex
{
    std::mutex internal_mutex;

    unsigned long const hierarichy_value;
    unsigned long previous_hierarchiy_value;

    static thread_local unsigned long this_thread_hierarchy_value;

    void check_for_hierarchy_violation()
    {
        if (this_thread_hierarchy_value <= hierarchy_value) {
            throw std::logic_error("mutex hierarch violated");
        }
    }

    void update_hierarchy_value()
    {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchy_mutex(unsigned long value):
        hierarchy_value(value),
        previous_hierarchy_value(0) {}

    void lock() 
    {
        check_for_hierarchy_violation();
        internal_mutex.lock();
        update_hierarchy_value();
    }

    void unlock()
    {
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock()
    {
        check_for_hierarchy_violation();
        if (!internal_mutex.try_lock()) return false;
        update_hierarchy_value();
        return true;
    }
};

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);

// 交换操作中 std::lock() 和 std::unique_lock 的使用
class some_big_object;
void swap(some_big_object& lhs, some_big_object& rhs);
class x
{
private:
    some_big_object some_detail;
    std::mutex m;
public:
    X(some_big_object const& sd): some_detail(sd) {} 
    friend void swap(X& lhs, X& rhs) 
    {
        if (&lhs == &rhs) return;
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        std::lock(lock_a, lock_b);
        swap(lhs.some_detail, rhs.some_detail);
    }
};

// 函数 get_lock() 锁住了互斥量，然后准备数据，返回锁的调用函数
std::unique_lock<std::mutex> get_lock()
{
    extern std::mutex some_mutex;
    std::unique_lock<std::mutex> lk(some_mutex);
    prepare_data();
    return lk;
}
void process_data()
{
    std::unique_lock<std::mutex> lk(get_lock());
    do_something();
}

// 锁的粒度
void get_and_process_data()
{
    std::unique_lock<std::mutex> my_lock(the_mutex);
    some_class data_to_process = get_next_data_chunk();
    my_lock.unlock();       // 不要让锁住的互斥量越过 process() 函数的调用
    result_type result = process(data_to_process);
    my_lock.lock();         // 为了写入数据，对互斥量再次上锁
    write_result(data_to_process, result);
}

// 列表3.10 比较操作符中一次锁住一个互斥量
class Y
{
private:
    int some_detail;
    mutable std::mutex m;
    int get_detail() const
    {
        std::lock_guard<std::mutex> lock_a(m);
        return some_detail;
    }
    
public:
    Y(int sd) : some_detail(sd) {}

    friend bool operator==(Y const &lhs, Y const &rhs)
    {
        if (&lhs == &rhs) return true;
        int const lhs_value = lhs.get_detail();
        int const rhs_value = rhs.get_detail();
        return lhs_value == rhs_value;
    }
};

std::shared_ptr<some_resource> resource_ptr;
void foo()
{
    if (!resource_ptr) {
        resource_ptr.reset(new some_resource);
    }
    resource_ptr->do_something();
}

// Listing 3.11 Thread-safe lazy initialization using a mutex
std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;
void foo()
{
    std::unique_lock<std::mutex> lk(resource_mutex);
    if (!resource_ptr) {
        resource_ptr.reset(new some_resource);
    }
    lk.unlock();
    resource_ptr->do_something();
}

void undefined_behaviour_with_double_checked_locking()
{
    if (!resource_ptr) {
        st::lock_guard<std::mutex> lk(resource_mutex);
        if (!resource_ptr) {
            resource_ptr.reset(new some_resource);
        }
    }
    resource_ptr->do_something();
}

std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag;

void init_resource()
{
    resource_ptr.reset(new some_resource);
}

void foo()
{
    std::call_once(resource_flag, init_resource);
    resource_ptr->do_something();
}

// Listing 3.12 Thread-safe lazy initialization of a class member using std::call_once
class x
{
private:
    connection_info connectioin_details;
    connection_handle connection;
    std::once_flag connection_init_flag;

    void open_connection()
    {
        connection = connection_manager.open(connection_details);
    }
public:
    X(connection_info const& connection_details_):
        connection_details(connectioni_detailes_) {}
    void send_data(data_packet const& data)
    {
        std::call_once(connection_init_flag, &X::open_connection, this);
        connection.send_data(data);
    }
    data_packet receive_data()
    {
        std::call_once(connection_ini_flag, &X::open_connection, this);
        return connectioin.receive_data();
    }
};

class my_class;
my_class& get_my_class_instance()
{
    static my_class instance;   // 线程安全的初始化过程
    return instance;
}

// Listing 3.13 Protecting a data structure with a boost::shared_mutex
#include <map>
#include <string>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

class dns_entry;

class dns_cache
{
    std::map<std::string, dns_entry> entries;
    mutable boost::shared_mutex entry_mutex;
public:
    dns_entry find_entry(std::string const& domain) const 
    {
        boost::shared_lock<boost::shared_mutex> lk(entry_mutex);
        std::map<std::string, dns_entry>::const_iterator const it = entries.find(domain);
        return (it == entries.end()) ? dns_entry() : it->second;
    }
    void update_or_add_entry(std::string const& domain,
                             dns_entry const& dns_details)
    {
        std::lock_guard<boost::shared_mutex> lk(entry_mutex);
        entries[domain] = dns_details;
    }
};