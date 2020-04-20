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

#ifndef LOVE_LIBRETRO_LIBRETROMOD_H
#define LOVE_LIBRETRO_LIBRETROMOD_H

// LOVE
#include "common/Module.h"

#include "LibretroCore.h"

namespace love
{
namespace libretro
{

class LibretroMod : public Module
{
public:
	LibretroMod() {}
	virtual ~LibretroMod() {}

	// Implements Module
	virtual ModuleType getModuleType() const { return M_LIBRETRO; }
	virtual const char *getName() const { return "love.libretro"; }

	/**
	 * Create a LibretroCore representing a loaded libretro core
	 **/
	virtual LibretroCore *newCore(const std::string &corePath, const std::string &gamePath);
}; // Video

} // video
} // love

#endif // LOVE_LIBRETRO_LIBRETROMOD_H
