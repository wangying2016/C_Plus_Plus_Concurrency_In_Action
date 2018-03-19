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
            do_something;           // 1.Ǳ�ڷ�����������������
        }
    }
};

void oops()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();         // 2. ���ȴ��߳̽���
                                // 3. ���߳̿��ܻ�������
}