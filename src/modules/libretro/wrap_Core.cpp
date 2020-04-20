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

#include "wrap_Core.h"

namespace love
{
namespace libretro
{

Core *luax_checkcore(lua_State *L, int idx)
{
	return luax_checktype<Core>(L, idx);
}

int w_Core_step(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
    core->step();
    return 0;
}

int w_Core_getImage(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
    luax_pushtype<love::graphics::Image>(L, core->getImage());
    return 1;
}

int w_Core_getAspectRatio(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
	lua_pushnumber(L, core->getAspectRatio());
	return 1;
}

static const luaL_Reg core_functions[] =
{
    { "step", w_Core_step },
	{ "getImage", w_Core_getImage },
	{ "getAspectRatio", w_Core_getAspectRatio },
	{ 0, 0 }
};

int luaopen_core(lua_State *L)
{
	return luax_register_type(L, &Core::type, core_functions, nullptr);
}

} // libretro
} // love
