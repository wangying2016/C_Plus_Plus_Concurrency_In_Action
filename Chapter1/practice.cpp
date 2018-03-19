std::thread std::mutex std::async
native_handle()

#include <iostream>
int main()
{
    std::cout << "Hello World\n";
}

#include <iostream>
#include <thread>
void hello()
{
    std::cout << "Hello Concurrent World\n";
}
int main()
{
    std::thread t(hello);
    t.join();
}