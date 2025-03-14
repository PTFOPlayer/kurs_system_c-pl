#pragma once
#include <stdint.h>
template <typename T, uint32_t ArraySize>
class CircularBuffer {
   private:
    uint32_t size = 128;
    T buffer[];
    uint32_t start = 0;
    uint32_t end = 0;

   public:
    CircularBuffer();
    ~CircularBuffer();

    void push(T);
    T* pop();
    void flush(T*);
};

template <typename T, uint32_t ArraySize>
CircularBuffer<T, ArraySize>::CircularBuffer() {}

template <typename T, uint32_t ArraySize>
CircularBuffer<T, ArraySize>::~CircularBuffer() {}

template <typename T, uint32_t ArraySize>
void CircularBuffer<T, ArraySize>::push(T item) {
    buffer[end] = item;
    end = (end + 1) % size;
    if (end == start) {
        start = (start + 1) % size;
    }
}

template <typename T, uint32_t ArraySize>
T* CircularBuffer<T, ArraySize>::pop() {
    if (start == end) {
        return nullptr;
    }
    T* item = &buffer[start];
    start = (start + 1) % size;
    return item;
}

template <typename T, uint32_t ArraySize>
void CircularBuffer<T, ArraySize>::flush(T* dst) {
    if (dst) {
        for (size_t i = 0; i < size; i++) {
            dst[i] = buffer[(start + i) % size];
        }
    }
}