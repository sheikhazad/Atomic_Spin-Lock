
#include <atomic>
#include <thread>
#include <iostream>

/*
Explanation:
test_and_set() atomically sets the flag and returns the previous value.
If the flag is already set (true), the thread keeps spinning.
Once unlocked (clear()), another thread can enter the critical section.
*/
class Spinlock {

public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // spin (busy-wait)
        }
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

Spinlock spinlock;
int shared_data = 0;

void thread_func(int id) {
    spinlock.lock();
    shared_data++;
    std::cout << "Thread " << id << " incremented shared_data to " << shared_data << "\n";
    spinlock.unlock();
}

int main() {
    std::thread t1(thread_func, 1);
    std::thread t2(thread_func, 2);
    t1.join();
    t2.join();
}

