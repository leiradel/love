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

// LOVE
#include "LibretroMod.h"
#include "wrap_LibretroMod.h"
#include "wrap_LibretroCore.h"

namespace love
{
namespace libretro
{

#define instance() (Module::getInstance<LibretroMod>(Module::M_LIBRETRO))

int w_newCore(lua_State *L)
{
	const char *corePath = luaL_checkstring(L, 1);
	const char *gamePath = luaL_checkstring(L, 2);

	LibretroCore *core = nullptr;
	luax_catchexcept(L, [&]() {
		core = instance()->newCore(corePath, gamePath);
	});

	luax_pushtype(L, core);
	core->release();
	return 1;
}

static const lua_CFunction types[] =
{
	luaopen_libretrocore,
	0
};

static const luaL_Reg functions[] =
{
	{ "newCore", w_newCore },
	{ 0, 0 }
};

extern "C" int luaopen_love_libretro(lua_State *L)
{
	LibretroMod *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new love::libretro::LibretroMod(); });
	}
	else
		instance->retain();

	WrappedModule w;
	w.module = instance;
	w.name = "libretro";
	w.type = &Module::type;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

} // libretro
} // love

#include "wrap_LibretroCore.cpp"
