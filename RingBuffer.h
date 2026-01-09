//
// Created by sam-o-reilly on 1/8/26.
//

#ifndef RINGBUFFER_RINGBUFFER_H
#define RINGBUFFER_RINGBUFFER_H
#include <oneapi/tbb/task_arena.h>

//Overwriting RingBuffer

template<size_t capacity>
class RingBuffer {
public:

    static constexpr size_t CAPACITY = capacity;

    alignas(8) std::atomic<uint64_t> writeIndex{0};

    alignas(8) std::atomic<uint64_t> readIndex{0};

    int buffer[CAPACITY];

    // false if not read, true if read
    std::atomic<bool> sequence[CAPACITY]{false};

    bool write(int& val);
    int read();

};

template<size_t Capacity>
bool RingBuffer<Capacity>::write(int& val) {
    uint64_t currWrite = writeIndex.load(std::memory_order_relaxed);

    while (true) {
        // Use acquire to ensure we see the latest readIndex
        uint64_t currentRead = readIndex.load(std::memory_order_acquire);

        if (currWrite - currentRead >= Capacity) {
            return false;
        }

        //keep spinning until we have the value
        if (writeIndex.compare_exchange_strong(currWrite, currWrite + 1, std::memory_order_relaxed)) {
            break;
        }

    }

    uint64_t index = currWrite & (CAPACITY - 1);

    while (sequence[index].load(std::memory_order_acquire)) {
        // spin until the value at that index has been read
    }

    buffer[index] = val;
    sequence[index].store(true, std::memory_order_release);
    return true;
}
template<size_t Capacity>
int RingBuffer<Capacity>::read() {

    uint64_t currRead = readIndex.load(std::memory_order_relaxed);

    while (true) {
        uint64_t currWrite = writeIndex.load(std::memory_order_relaxed);

        if (currRead >= currWrite) {
            return -1;
        }

        if (readIndex.compare_exchange_strong(currRead, currRead + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {

            break;
        }
    }

    uint64_t index = currRead & (CAPACITY - 1);
    while (!sequence[index].load(std::memory_order_acquire)) {
        //spin
     }

    int result = buffer[index];
    sequence[index].store(false, std::memory_order_release);
    return result;


}


#endif //RINGBUFFER_RINGBUFFER_H