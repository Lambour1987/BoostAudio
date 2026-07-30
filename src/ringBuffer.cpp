//30-7-2026: 

#include "ringbuffer.h"

using namespace std;

//Constructor Ringbuffer

RingBuffer::RingBuffer(size_t capacity):capacity(capacity)
{
    buffer.resize(capacity*2);
}

//functie availableToRead die teruggeeft een size_t en je mag deze niet aanpassen
size_t RingBuffer::availableToRead() const
{
    return size;
}
//functie availableToWrite die teruggeeft een size_t en je mag deze niet aanpassen
size_t RingBuffer::availableToWrite() const
{
    return capacity - size; 
}
