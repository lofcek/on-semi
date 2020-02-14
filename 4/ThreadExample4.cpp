#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>
#include "../1/LockGuard.h"     // this is a bit ugly include, but I do not want to copy&paste code


// comments:
// 1. I didn't commit challenge 3, however directly challenge 4. To achieve 3 just replace `condition_variable_any` to `condition_variable` and `challenge::lock_guard` to `std::lock_guard`.
// 2. `std::condition_signal` do not exist. Probably it was typo, correct should be `std::condition_variable`.
// 3. Synchronization primitive condition_variable does not allow to specify which thread will be notified (woken up).
//    Therefore there are two possibilities solve the issue.
//     - All threads will be woken by `notify_all`. Disadvantage of this issue will be for bigger amount of threads.
//     - To have specificy condition_variable and mutex for each thread.

using namespace std;
static std::mutex g_mutex;
static std::condition_variable_any g_cond_var;
static std::string g_current;

static std::random_device g_rd;  //Will be used to obtain a seed for the random number engine
static std::mt19937 g_gen(g_rd());

void thread_main(std::string me, std::string next_thread) {
    cout << me << ": starting, waiting.\n";
    std::uniform_int_distribution<int> timeout(1000, 5000);
    // in our case is of course not imporant whether mutex is locked
    // during "sleeping", because just one thread could be active.
    // However in real world we should lock mutex for a first time
    // during wait, that release it and again
    // during change g_current and notify_all.
    challenge::lock_guard lk(g_mutex);
    for(;;) {
        g_cond_var.wait(g_mutex, [&]{return g_current == me;});
        cout << me << ": signal received, doing work ....\n";
        this_thread::sleep_for(std::chrono::milliseconds(
            timeout(g_gen)
        ));
        cout << me << ": done with work, signal next thread\n";
        g_current = next_thread;
        g_cond_var.notify_all();
    }
}

int main() {
    // names of threads
    const char* n1 = "thread1";
    const char* n2 = "thread2";
    const char* n3 = "thread3";

    cout << "main: starting all threads\n";
    thread t1(thread_main, n1, n2);
    thread t2(thread_main, n2, n3);
    thread t3(thread_main, n3, n1);

    // I should wait until all threads are waiting.
    cout <<  "main: starting thread 1.\n";
    g_current = n1;
    g_cond_var.notify_all();

    t1.join();
    t2.join();
    t3.join();

    return 0;
}