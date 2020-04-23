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
#include "common/delay.h"

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

Decoder::Decoder()
    : love::sound::Decoder(nullptr, 0)
    , sampleRate(DEFAULT_SAMPLE_RATE)
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

void Decoder::setRate(double rate)
{
    if (resampler != NULL)
        speex_resampler_destroy(resampler);

    coreRate = rate;
    currentRatio = originalRatio = sampleRate / coreRate;

    int error;
    resampler = speex_resampler_init(2, coreRate, sampleRate, SPEEX_RESAMPLER_QUALITY_DEFAULT, &error);

    if (resampler == nullptr)
        throw love::Exception("Error creating resampler: %s", speex_resampler_strerror(error));
}

void Decoder::mix(const int16_t *data, size_t frames)
{
    size_t avail = samples.free();

    /* Readjust the audio input rate. */
    const int halfSize = (int) samples.size() / 2;
    const int deltaMid = (int) avail - halfSize;
    const double direction = (double) deltaMid / (double) halfSize;
    const double adjust = 1.0 + rateControlDelta * direction;

    currentRatio = originalRatio * adjust;

    spx_uint32_t inLen = frames * 2;
    spx_uint32_t outLen = (spx_uint32_t) (inLen * currentRatio);
    outLen += outLen & 1; // request an even number of samples (stereo)
    int16_t *output = (int16_t*)alloca(outLen * 2);

    if (output == nullptr)
        throw std::bad_alloc();

    int error = speex_resampler_process_int(resampler, 0, data, &inLen, output, &outLen);

    if (error != RESAMPLER_ERR_SUCCESS)
        throw love::Exception("speex_resampler_process_int: %s", speex_resampler_strerror(error));

    outLen &= ~1; // don't send incomplete audio frames
    const size_t size = outLen * 2;

    /*while (size > avail)
    {
        love::sleep(1);
        avail = samples.free();
    }*/

    samples.write(output, std::min(size, avail));
}

love::sound::Decoder *Decoder::clone()
{
    return nullptr;
}

int Decoder::decode()
{
    const size_t toRead = std::min(samples.occupied(), static_cast<size_t>(OutBufferSize));
    samples.read(outBuffer, toRead);
    return static_cast<int>(toRead);
}

void *Decoder::getBuffer() const
{
    // Source::streamAtomic() says this must return a pointer to the samples
    // decoded in decode().
    return const_cast<void*>(reinterpret_cast<const void*>(outBuffer));
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
