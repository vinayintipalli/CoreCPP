/**
 Build a Thread-Safe Bounded Queue — implement a bounded blocking queue using condition variables and mutexes. Support multiple producers and consumers. Ensure there are no deadlocks, spurious wakeup bugs, or incorrect wake-ups.
 **/

/**
Assumption:
1. Using 2 condition variables 
    i. one for queue not full --> producer
    ii. one for queue not empty --> consumer
2. mutex for queue contention
3. notify producer once consumed, notify consumer once produced
4. After this will think about lock free (CAS)
**/ 

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BoundedQueue {
    private:
        std::queue<T> qe;
        size_t capacity;
        std::condition_variable queue_not_full;
        std::condition_variable queue_not_empty;
        mutable std::mutex mtx; // mutable: const function can update this variable (for reading purpose, here no such significant presence)
    public:
        explicit BoundedQueue(size_t cap):capacity(cap) {};
        void push(T item) {
            std::unique_lock<std::mutex> lock(mtx);
            queue_not_full.wait(lock, [&](){
                return (qe.size() < capacity);
            });
            /*
            while(qe.size() == capacity){
                cv.wait();
            }
            */
            qe.push(std::move(item)); // to avoid copy
            lock.unlock();
            queue_not_empty.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock(mtx);
            queue_not_empty.wait(lock, [&]() {
                return (!qe.empty());
            });
            T k = std::move(qe.front());
            qe.pop();
            lock.unlock();
            queue_not_full.notify_one();
            return k;
        }
};

int main() {
    BoundedQueue<int> bq(10);
}