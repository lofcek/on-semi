#include <thread>
#include <iostream>
#include <sstream>
#include <random>

using namespace std;
void thread_main(std::string_view name, unsigned seed) {

    std::minstd_rand0 gen(seed);
    gen.discard(20);    // if we don't discard some numbers gen will start with zero. see https://stackoverflow.com/questions/21843172/c-uniform-int-distribution-always-returning-min-on-first-invocation
    std::uniform_int_distribution<int> dist(0, 1000);  // time between 0 and 1000 ms

    for(int i = 0; i < 5; i++) {
        auto duration = std::chrono::milliseconds(dist(gen));

        // the output should not be direclty written into cout,
        // because texts from different threads could interleaved.
        stringstream ss;
        ss << "Thread " << name << " will be sleeping for " << duration.count() << "ms.\n";
        cout << ss.str();
        std::this_thread::sleep_for(duration);
    }
}

int main() {
    // seed is hardcoded, therefore output is relative predictable not random at all.
    std::thread t1(thread_main, "t1", 1);
    std::thread t2(thread_main, "t2", 2);
    std::thread t3(thread_main, "t3", 3);

    t1.join();
    t2.join();
    t3.join();
    return 0;
}