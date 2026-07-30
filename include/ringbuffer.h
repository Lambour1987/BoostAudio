// 30-7-2026: Ringbuffer: Design Pattern: Producer-Consumer
// AudioCapture: Producer, Ringbuffer is tussenliggend en AudioStream is consumer
// Een ring buffer is belangrijk omdat de twee threads niet altijd exact even snel werken
// Omdat we hier met aaneengesloten gegevens werken doen we een vector en geen linked list.

#pragma once

#include <vector>

// Header die size_t mogelijk maakt: unsigned integer-type
#include <cstddef>

class RingBuffer
{
    public:
        RingBuffer(size_t capacity);

        // bool write(const float* data,size_t count);

        // bool read(float* data, size_t count)

        size_t availableToRead() const;

        size_t availableToWrite() const;

    private:

        std::vector<float> buffer;

        size_t capacity;

        size_t readIndex;

        size_t writeIndex;

        size_t size = 0;
};