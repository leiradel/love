/**
 * Copyright (c) 2006-2020 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// C
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

// LOVE
#include "Decoder.h"

// speex
#define OUTSIDE_SPEEX
#define RANDOM_PREFIX speex
#define EXPORT
#define _USE_SSE2
#define FIXED_POINT
#include "speex/resample.c"

namespace love
{
namespace libretro
{

Fifo::Fifo(uint8_t *buffer, size_t size)
    : buffer(buffer)
    , bufsize(size)
    , avail(size)
    , first(0)
    , last(0)
{
    mutex = love::thread::newMutex();
}

Fifo::~Fifo()
{
    delete mutex;
}

void Fifo::reset()
{
    avail = bufsize;
    first = last = 0;
}

void Fifo::read(void *data, size_t size)
{
    mutex->lock();

    size_t first = size;
    size_t second = 0;

    if (first > bufsize - first)
    {
        first = bufsize - first;
        second = size - first;
    }

    uint8_t *src = buffer + first;
    memcpy(data, src, first);
    memcpy((uint8_t*) data + first, buffer, second);

    first = (first + size) % bufsize;
    avail += size;

    mutex->unlock();
}

void Fifo::write(const void *data, size_t size)
{
    mutex->lock();

    size_t first = size;
    size_t second = 0;

    if (first > bufsize - last)
    {
        first = bufsize - last;
        second = size - first;
    }

    uint8_t *dest = buffer + last;
    memcpy(dest, data, first);
    memcpy(buffer, (uint8_t*) data + first, second);

    last = (last + size) % bufsize;
    avail -= size;

    mutex->unlock();
}

size_t Fifo::occupied()
{
    size_t count;

    mutex->lock();
    count = bufsize - avail;
    mutex->unlock();

    return count;
}

size_t Fifo::free()
{
    size_t count;

    mutex->lock();
    count = avail;
    mutex->unlock();

    return count;
}

Decoder::Decoder(double sampleRate)
    : love::sound::Decoder(nullptr, 0)
    , sampleRate(sampleRate)
    , coreRate(0.0)
    , rateControlDelta(0.005)
    , currentRatio(0.0)
    , originalRatio(0.0)
    , resampler(nullptr)
{}

Decoder::~Decoder()
{
    if (resampler != nullptr)
        speex_resampler_destroy(resampler);
}

love::sound::Decoder *Decoder::clone()
{
    return nullptr;
}

int Decoder::decode()
{

}

bool Decoder::seek(double s)
{
    return false;
}

bool Decoder::rewind()
{
    return false;
}

bool Decoder::isSeekable()
{
    return false;
}

int Decoder::getChannelCount() const
{
    return 2;
}

int Decoder::getBitDepth() const
{
    return 16;
}

int Decoder::getSampleRate() const
{
    return currentRatio;
}

double Decoder::getDuration()
{
    return 0.0;
}

} // libretro
} // love
