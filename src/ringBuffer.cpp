//30-7-2026: 

#include "ringbuffer.h"
#include <iostream>

//tbv van de min functie (minimaal)
#include <algorithm>

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

void RingBuffer::write(const float* data, size_t frameCount)
{
    for(size_t i = 0; i< frameCount;i++)
    {
        
        //30-7-2026:DEbug regels
        // cout<<"Capacity: "<<capacity<<endl;
        // cout<<"Buffer size: "<<buffer.size()<<endl;
        // cout<<"Write index: "<<writeIndex<<endl;
        // cout<<"Frame Count: "<<frameCount<<endl;


        // Schrijf het linker sample
        buffer[writeIndex*2] = data[i*2];

        // Schrijf het rechter sample
        buffer[writeIndex*2 +1] = data[i*2+1];

        // Ga naar het volgende frame
        //Tbv Ringbuffer modulus gebruiken (zoals circular queue zie DSA)
        //BELANGRIJK dus.
        writeIndex = (writeIndex + 1) % capacity;

        // Als buffer nog niet vol was, komt er 1 frame bij
        if(size<capacity)
        {
            size++;
        }
        else
        {
            // De buffer was al vol: we hebben het oudste frame overschreven
            // ReadIndex wordt meegeschoven
            readIndex = (readIndex+1)%capacity;
        }
    }
}

size_t RingBuffer::read(float* destination, size_t framecount)
{
    //neem het minimale van de framecount en size en stop deze in framesToRead met datatype size_t
    size_t framesToRead=min(framecount, size);

    //Loop door frames heen
    for(size_t i=0; i< framesToRead;i++)
    {
        //Lees het linker sample
        destination[i*2]=buffer[readIndex*2+1];
        //Lees het rechter sample
        destination[i*2+1]=buffer[readIndex*2+1];
        //Ga naar het volgende frame
        readIndex = (readIndex+1)%capacity;
        // Er is nu 1 frame minder beschikbaar
        size--;
    }
    return framesToRead;
}