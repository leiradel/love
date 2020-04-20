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

#include "wrap_LibretroCore.h"

namespace love
{
namespace libretro
{

LibretroCore *luax_checklibretrocore(lua_State *L, int idx)
{
	return luax_checktype<LibretroCore>(L, idx);
}

int w_LibretroCore_step(lua_State *L)
{
	auto core = luax_checklibretrocore(L, 1);
    core->step();
    return 0;
}

int w_LibretroCore_getImage(lua_State *L)
{
	auto core = luax_checklibretrocore(L, 1);
    luax_pushtype<love::graphics::Image>(L, core->getImage());
    return 1;
}

static const luaL_Reg libretrocore_functions[] =
{
    { "step", w_LibretroCore_step },
	{ "getImage", w_LibretroCore_getImage },
	{ 0, 0 }
};

int luaopen_libretrocore(lua_State *L)
{
	return luax_register_type(L, &LibretroCore::type, libretrocore_functions, nullptr);
}

} // libretro
} // love
