/**
 * Copyright (c) 2006-2019 LOVE Development Team
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

#ifndef LOVE_LIBRETRO_DECODER_H
#define LOVE_LIBRETRO_DECODER_H

// LOVE
#include "common/Data.h"
#include "sound/Decoder.h"
#include "thread/threads.h"

#include "dr_flac/dr_flac.h"
#include <string.h>

namespace love
{
namespace libretro
{

class Fifo
{
public:
    Fifo(uint8_t *buffer, size_t size);
    virtual ~Fifo();

    void reset();

    void read(void* data, size_t size);
    void write(const void* data, size_t size);

    size_t size() { return bufsize; }

    size_t occupied();
    size_t free();

private:
    love::thread::Mutex* mutex;
    uint8_t* buffer;
    const size_t bufsize;
    size_t avail;
    size_t first;
    size_t last;
};

template<size_t SIZE>
class BufferedFifo : public Fifo
{
public:
    BufferedFifo() : Fifo(buffer, SIZE) {}

private:
    uint8_t buffer[SIZE];
};

class Decoder : public love::sound::Decoder
{
public:
	Decoder();
	virtual ~Decoder();

	love::sound::Decoder *clone();
	int decode();
	bool seek(double s);
	bool rewind();
	bool isSeekable();
	int getChannelCount() const;
	int getBitDepth() const;
	int getSampleRate() const;
	double getDuration();

private:
    BufferedFifo<8192> samples;
};

} // libretro
} // love

#endif // LOVE_LIBRETRO_DECODER_H
