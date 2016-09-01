//z 2015-03-30 17:35
//z C++ 11 Threads Make your (multitasking) life easier
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <future>


// Simple
void foo1()
{
}

void func1()
{
    std::thread t(foo1); // Starts. Equal to CreateThread.
    t.join();  // Equal to WaitForSingleObject to the thread handle.
}

void foo2(int x,int y)
{
    // x = 4, y = 5.
    int z = x + y;
}

void func2()
{
    std::thread t(foo2,4,5); // Acceptable.
    t.join();
}

// MUTEX
std::mutex m3;
int j3 = 0;
void foo3()
{
    m3.lock();
    j3++;
    m3.unlock();
}
void func3()
{
    std::thread t1(foo3);
    std::thread t2(foo3);
    t1.join();
    t2.join();
    // j3 = 2;
}

std::recursive_mutex m4;
int j4 = 0;
void func4()
{
    m4.lock();
    m4.lock(); // now valid
    j4++;
    m4.unlock();
    m4.unlock(); // don't forget!
}


// Variables
std::condition_variable c;
std::mutex mu; // We use a mutex rather than a recursive_mutex because the lock has to be acquired only and exactly once.
void foo5()
{
    std::unique_lock<std::mutex> lock(mu); // Lock the mutex
    c.notify_one(); // WakeConditionVariable. It also releases the unique lock
}

void func5()
{
    std::unique_lock<std::mutex> lock(mu); // Lock the mutex
    std::thread t1(foo5);
    c.wait(lock); // Equal to SleepConditionVariableCS. This unlocks the mutex mu
    t1.join();
}


// Future and Promises

int GetMyAnswer()
{
    return 10;
}

void func6()
{
    std::future<int> GetAnAnswer = std::async(GetMyAnswer);  // GetMyAnswer starts background execution
    int answer = GetAnAnswer.get(); // answer = 10;
    // If GetMyAnswer has finished, this call returns immediately.
    // If not, it waits for the thread to finish.
}


std::promise<int> sex;
void foo7()
{
    // do stuff

    sex.set_value(1); // After this call, future::get() will return this value.
//	sex.set_exception(std::make_exception_ptr(std::runtime_error("TEST"))); // After this call, future::get() will throw this exception
}
void func7()
{
    std::future<int> makesex = sex.get_future();
    std::thread t(foo7);

    // do stuff
    try
    {
        makesex.get();
        // hurray();
    }
    catch(...)
    {
        // She dumped us :(
    }
    t.join();
}

int main()
{
    func1();
    func2();
    func3();
    func4();
    func5();
    func6();
    func7();
    return 0;
}


