#include <iostream>
#include <chrono>
#include "RingBuffer.h"

// arrow ( -> ) is used to access members in a pointer
void producer(RingBuffer<4096>* ring_buffer) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 2500000; i++) {
        while (!ring_buffer->write(i)) {
            //keep retrying
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " micro-seconds | producer" << "\n";
}

void consumer(RingBuffer<4096>* ring_buffer) {
    auto start = std::chrono::high_resolution_clock::now();

    long long checksum = 0;
    for (int i = 0; i < 1000000; i++) {
        int val;
        while ((val = ring_buffer->read()) == -1) { }
        checksum += val;
    }

    auto end = std::chrono::high_resolution_clock::now();

    sleep(.5);//to avoid print statements overlapping

    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " micro-seconds | consumer (sum: " << checksum << ")" << "\n";
}

int main() {

    RingBuffer<4096> ring_buffer;

    std::thread t(producer, &ring_buffer);
    std::thread t1(producer, &ring_buffer);
    std::thread t2(consumer, &ring_buffer);
    std::thread t3(consumer, &ring_buffer);
    std::thread t4(consumer, &ring_buffer);
    std::thread t5(consumer, &ring_buffer);
    std::thread t6(consumer, &ring_buffer);

    t.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
}

