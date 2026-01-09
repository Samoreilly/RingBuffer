#include <iostream>
#include <chrono>
#include "RingBuffer.h"

// arrow ( -> ) is used to access members in a pointer
void producer(RingBuffer<4096>* ring_buffer) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 500000000; i++) {
        while (!ring_buffer->write(i)) {
            //keep retrying
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "micro-seconds | producer" << "\n";
}

void consumer(RingBuffer<4096>* ring_buffer) {
    auto start = std::chrono::high_resolution_clock::now();

    long long checksum = 0;
    for (int i = 0; i < 500000000; i++) {
        int val;
        while ((val = ring_buffer->read()) == -1) { }
        checksum += val;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " micro-seconds | consumer (sum: " << checksum << ")" << std::endl;
}
int main() {

    RingBuffer<4096> ring_buffer;

    std::thread t(producer, &ring_buffer);
    std::thread t2(consumer, &ring_buffer);

    t.join();
    t2.join();

}

