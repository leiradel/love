/*
Copyright (c) 2020 Andre Leiradella
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// LOVE
#include "BDFFontRasterizer.h"
#include "filesystem/Filesystem.h"
#include "image/Image.h"

// C++
#include <sstream>
#include <vector>
#include <algorithm>

// C
#include <stdlib.h>
#include <ctype.h>

#define BBX             UINT32_C(0x0b87d481)
#define SIZE            UINT32_C(0x7c8bd560)
#define SWIDTH          UINT32_C(0xcfcab8d8)
#define ENDFONT         UINT32_C(0x7e1a9db3)
#define ENCODING        UINT32_C(0x3fb5ea0c)
#define STARTFONT       UINT32_C(0x8cf73faa)
#define METRICSSET      UINT32_C(0xaf5b3888)
#define DWIDTH          UINT32_C(0xaccd5769)
#define ENDPROPERTIES   UINT32_C(0x58fbd469)
#define STARTPROPERTIES UINT32_C(0xc27dd3a0)
#define BITMAP          UINT32_C(0xa72bdb22)
#define DWIDTH1         UINT32_C(0x467844ba)
#define FONT            UINT32_C(0x7c84cc7c)
#define ENDCHAR         UINT32_C(0x7e18d91a)
#define STARTCHAR       UINT32_C(0x8cf57b11)
#define CHARS           UINT32_C(0x0ce40496)
#define COMMENT         UINT32_C(0xe721d578)
#define FONTBOUNDINGBOX UINT32_C(0x3344af7b)
#define CONTENTVERSION  UINT32_C(0x56774aa6)
#define VVECTOR         UINT32_C(0xad72744e)

#define XVAL(x) (isdigit(x) ? (x) - '0' : toupper(x) - 'A' + 10)

namespace
{
    class Reader
    {
    public:
        Reader(const char *source, size_t length)
        {
            this->source = reinterpret_cast<const uint8_t*>(source);
            this->length = length;
        }

        uint8_t current()
        {
            return length != 0 ? *source : 0;
        }

        void skip()
        {
            if (length != 0)
            {
                length--;
                source++;
            }
        }

        void skipSpaces()
        {
            while (isspace(current()))
                skip();
        }

        void skipLine()
        {
            while (current() != '\n' && current() != 0)
                skip();

            if (current() == '\n')
                skip();
        }

        uint32_t getKeyword()
        {
            uint32_t hash = 5381;

            if (!isalpha(current()) && current() != '_')
                throw love::Exception("Keyword expected");

            do
            {
                hash = hash * 33 + current();
                skip();
            }
            while (isalpha(current()) || current() == '_');

            while (isalnum(current()) || current() == '_')
            {
                hash = hash * 33 + current();
                skip();
            }

            return hash;
        }

        int readInt()
        {
            int val = 0, sig = 1;
            skipSpaces();

            // Adobe's spec doesn't say numbers can be preceeded by '+' but we never know.
            switch (current())
            {
                case '-':
                    sig = -1;
                case '+':
                    skip();
                    break;
            }

            // Skip spaces between signal and first digit.
            skipSpaces();

            if (!isdigit(current()))
                throw love::Exception("Digit expected");

            // Now we start reading digits.
            do
            {
                val = val * 10 + current() - '0';
                skip();
            }
            while (isdigit(current()));

            return sig * val;
        }

        int readHex()
        {
            skipSpaces();

            if (!isxdigit(current()))
                throw love::Exception("Hexadecimal digit expected");
            
            int val = XVAL(current()) * 16;
            skip();

            if (!isxdigit(current()))
                throw love::Exception("Hexadecimal digit expected");
            
            val |= XVAL(current());
            skip();
            return val;
        }

    protected:
        const uint8_t* source;
        size_t length;
    };
}

namespace love
{
namespace font
{

BDFFontRasterizer::BDFFontRasterizer(love::filesystem::FileData *fontdef)
    : lineHeight(0)
    , baseLine(0)
{
    parseFont((const char *) fontdef->getData(), fontdef->getSize());
}

BDFFontRasterizer::~BDFFontRasterizer()
{
}

void BDFFontRasterizer::parseFont(const char *source, size_t length)
{
    Reader reader(source, length);
    bool inChar = false;
    Character chr, defaults;
    int code, glyph;

    for (;;)
    {
        const uint32_t hash = reader.getKeyword();

        switch (hash)
        {
            // Starts a font.
            case STARTFONT:
            {
                int major = reader.readInt();

                if (reader.current() != '.')
                    throw love::Exception("Error parsing version");

                reader.skip();
                int minor = reader.readInt();

                // Issue an error on versions higher than 2.2.
                if (major > 2 || (major == 2 && minor > 2))
                    throw love::Exception("Unsupported version %d.%d", major, minor);

                reader.skipLine();
                break;
            }

            // The FONTBOUNDINGBOX values seems to be defaults for BBX values.
            case FONTBOUNDINGBOX:
            {
                defaults.width = reader.readInt();
                defaults.height = reader.readInt();
                defaults.bearingX = reader.readInt();
                defaults.bearingY = reader.readInt();
                defaults.advance = 0;
                defaults.bits = 0;

                lineHeight = defaults.height;
                baseLine = defaults.height + defaults.bearingY;

                reader.skipLine();
                break;
            }

            case METRICSSET:
            {
                const int metricsSet = reader.readInt();

                // We only handle horizontal writing by now.
                if (metricsSet != 0)
                    throw love::Exception("Unsupported writing direction");

                reader.skipLine();
                break;
            }

            // This is the character's width in pixels.
            case DWIDTH:
            {
                chr.advance = reader.readInt();
                reader.readInt();
                reader.skipLine();
                break;
            }

            case CHARS:
            {
                // Read the number of chars in this font and malloc the required memory.
                numChars = reader.readInt();
                reader.skipLine();
                break;
            }

            case STARTCHAR:
            {
                // Copy default values.
                chr = defaults;
                reader.skipLine();
                break;
            }

            case ENCODING:
            {
                // Read character's code and glyph index, they can be -1.
                code = reader.readInt();
                glyph = code == -1 ? reader.readInt() : -1;
                reader.skipLine();
                break;
            }

            // The bounding box around the character's black pixels.
            case BBX:
            {
                chr.width = reader.readInt();
                chr.height = reader.readInt();
                chr.bearingX = reader.readInt();
                chr.bearingY = reader.readInt();

                reader.skipLine();
                break;
            }

            // BITMAP signals the start of the hex data.
            case BITMAP:
            {
                reader.skipLine();

                // wbytes is the width of the char in bytes.
                const int wbytes = (chr.width + 7) / 8;

                // Allocate memory for the pixels.
                chr.bits = bitmaps.size();
                bitmaps.resize(chr.bits + wbytes * chr.height);

                // Read all pixels from file.
                size_t ndx = chr.bits;

                for (int y = 0; y < chr.height; y++)
                {
                    for (int x = 0; x < wbytes; x++)
                    {
                        bitmaps[ndx++] = reader.readHex();
                    }

                    reader.skipLine();
                }

                break;
            }

            case ENDCHAR:
            {
                characters.insert({code != -1 ? code : glyph, chr});
                reader.skipLine();
                break;
            }

            case ENDFONT:
                return;

            default:
            {
                // Unknown section, skip.
                reader.skipLine();
                break;
            }
        }
    }
}

int BDFFontRasterizer::getLineHeight() const
{
    return lineHeight;
}

GlyphData *BDFFontRasterizer::getGlyphData(uint32 glyph) const
{
    auto it = characters.find(glyph);

    // Return an empty GlyphData if we don't have the glyph character.
    if (it == characters.end())
        return new GlyphData(glyph, GlyphMetrics(), PIXELFORMAT_RGBA8);

    const Character &c = it->second;

    GlyphData *g = new GlyphData(glyph, c, PIXELFORMAT_LA8);
    uint8 *pixels = (uint8 *) g->getData();
    size_t bits = c.bits;

    // Copy the subsection of the texture from the ImageData to the GlyphData.
    int wbytes = (c.width + 7) / 8;

    for (int y = 0; y < c.height; y++)
    {
        for (int x = 0; x < c.width; x++)
        {
            uint8_t byte = bitmaps[bits + x / 8];
            bool bit = (byte & (1 << (x & 7))) != 0;

            *pixels++ = bit ? 0xc0 : 0x40;
            *pixels++ = 0x80;
        }

        bits += wbytes;
    }

    return g;
}

int BDFFontRasterizer::getGlyphCount() const
{
    return (int) characters.size();
}

bool BDFFontRasterizer::hasGlyph(uint32 glyph) const
{
    return characters.find(glyph) != characters.end();
}

float BDFFontRasterizer::getKerning(uint32 leftglyph, uint32 rightglyph) const
{
    return 0.0f;
}

Rasterizer::DataType BDFFontRasterizer::getDataType() const
{
    return DATA_IMAGE;
}

bool BDFFontRasterizer::accepts(love::filesystem::FileData *fontdef)
{
    const char *data = (const char *) fontdef->getData();

    // Check if the "info" tag is at the start of the file. This is a truly
    // crappy test. Is the tag even guaranteed to be at the start?
    return fontdef->getSize() > 9 && memcmp(data, "STARTFONT", 9) == 0;
}

} // font
} // love
