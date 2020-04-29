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

#ifndef LOVE_FONT_BDFFONT_RASTERIZER_H
#define LOVE_FONT_BDFFONT_RASTERIZER_H

// LOVE
#include "common/config.h"
#include "Rasterizer.h"
#include "image/ImageData.h"

// C++
#include <unordered_map>
#include <vector>

namespace love
{
namespace font
{

/**
 * Rasterizer for BDF bitmap fonts.
 **/
class BDFFontRasterizer : public Rasterizer
{
public:

    BDFFontRasterizer(love::filesystem::FileData *fontdef);
    virtual ~BDFFontRasterizer();

    // Implements Rasterizer.
    int getLineHeight() const override;
    GlyphData *getGlyphData(uint32 glyph) const override;
    int getGlyphCount() const override;
    bool hasGlyph(uint32 glyph) const override;
    float getKerning(uint32 leftglyph, uint32 rightglyph) const override;
    DataType getDataType() const override;

    static bool accepts(love::filesystem::FileData *fontdef);

private:

	struct Character : public GlyphMetrics
	{
		size_t bits;
	};

    void parseFont(const char *bdf, size_t length);

    // Glyph characters, indexed by their glyph id.
    std::unordered_map<uint32, Character> characters;

    // Glyph bitmaps.
    std::vector<uint8_t> bitmaps;

    int lineHeight;
    int baseLine;
	int numChars;

}; // BDFFontRasterizer

} // font
} // love

#endif // LOVE_FONT_BDFFONT_RASTERIZER_H
