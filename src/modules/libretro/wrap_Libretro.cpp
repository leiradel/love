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
#include "Libretro.h"
#include "wrap_Libretro.h"
#include "wrap_Core.h"

namespace love
{
namespace libretro
{

#define instance() (Module::getInstance<Libretro>(Module::M_LIBRETRO))

int w_newCore(lua_State *L)
{
    const char *corePath = luaL_checkstring(L, 1);
    const char *gamePath = luaL_optstring(L, 2, "");

    Core *core = nullptr;
    luax_catchexcept(L, [&]() {
        core = instance()->newCore(corePath, gamePath);
    });

    luax_pushtype(L, core);
    core->release();
    return 1;
}

int w_setPaths(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, -1, "systemPath");
    const char *systemPath = luaL_checkstring(L, -1);
    Core::setSystemPath(systemPath);

    lua_getfield(L, -2, "coreAssetsDirectory");
    const char *coreAssetsDirectory = luaL_checkstring(L, -1);
    Core::setCoreAssetsDirectory(coreAssetsDirectory);

    lua_getfield(L, -3, "saveDirectory");
    const char *saveDirectory = luaL_checkstring(L, -1);
    Core::setSaveDirectory(saveDirectory);

    return 0;
}

static const lua_CFunction types[] =
{
    luaopen_core,
    0
};

static const luaL_Reg functions[] =
{
    { "newCore", w_newCore },
    { "setPaths", w_setPaths },
    { 0, 0 }
};

extern "C" int luaopen_love_libretro(lua_State *L)
{
    Libretro *instance = instance();
    if (instance == nullptr)
    {
        luax_catchexcept(L, [&](){ instance = new love::libretro::Libretro(); });
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
