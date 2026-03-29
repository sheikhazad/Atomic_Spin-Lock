#include <immintrin.h> // for _mm_pause()
#include <thread>        // for yield(), sleep_for()
#include <chrono>        
#include <iostream>

class Spinlock 
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
  
void lock() {
    //1.  First phase: very fast spinning with CPU-friendly pause
    for (int i = 0; i < 100; ++i) 
    {
       if(!flag.test_and_set(std::memory_order_acquire))
          return;

       _mm_pause();  
       // ^ Hint to CPU: "I'm in a tight spin loop."
       // Reduces power usage and improves   performance on hyper‑threaded CPUs.
    }

    // 2. Second phase: give other threads a chance to run
    for (int i = 0; i < 50; ++i)
    {
          if(!flag.test_and_set(std::memory_order_acquire))
              return;

        std::this_thread::yield();
        // ^ Ask OS scheduler to let another thread run.
        //   Avoids burning CPU if the lock is held a bit longer.
    }

    // 3. Third phase: back off more aggressively
    while (flag.test_and_set(std::memory_order_acquire)
    {
       std::this_thread::sleep_for(std::chrono::microseconds(50));
       // ^ Sleep briefly to avoid wasting CPU entirely.
       //   Useful when the lock may be held for a long time.
    } 
}

    void unlock() {
        flag.clear(std::memory_order_release);
    }
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

Explanation:  

• test_and_set() sets the flag to true and returns the old value

• If the old value was true, someone else holds the lock → keep spinning

• If the old value was false, you acquired the lock

• clear() releases the lock

• It cannot be copied or assigned

• Default constructed atomic_flag starts in an unspecified state, so you must initialize it with ATOMIC_FLAG_INIT