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
#include "wrap_Core.h"

// libc
#include "string.h"

namespace love
{
namespace libretro
{

static bool getInput(const char *name, Core::Input &input)
{
    uint32_t hash = 5381;

    for (const uint8_t *str = (const uint8_t*) name; *str != 0; str++)
        hash = hash * 33 + *str;

    switch (hash)
    {
        case UINT32_C(0x7c9b47f5): input = Core::Input::NONE; return strcmp(name, "none") == 0;
        case UINT32_C(0xef160c6e): input = Core::Input::JOYPAD_B; return strcmp(name, "joypadb") == 0;
        case UINT32_C(0xef160c85): input = Core::Input::JOYPAD_Y; return strcmp(name, "joypady") == 0;
        case UINT32_C(0xfa5d528c): input = Core::Input::JOYPAD_SELECT; return strcmp(name, "joypadselect") == 0;
        case UINT32_C(0x6c777bba): input = Core::Input::JOYPAD_START; return strcmp(name, "joypadstart") == 0;
        case UINT32_C(0xd1d79d11): input = Core::Input::JOYPAD_UP; return strcmp(name, "joypadup") == 0;
        case UINT32_C(0xa629de64): input = Core::Input::JOYPAD_DOWN; return strcmp(name, "joypaddown") == 0;
        case UINT32_C(0xa62e14b7): input = Core::Input::JOYPAD_LEFT; return strcmp(name, "joypadleft") == 0;
        case UINT32_C(0x6c5f734a): input = Core::Input::JOYPAD_RIGHT; return strcmp(name, "joypadright") == 0;
        case UINT32_C(0xef160c6d): input = Core::Input::JOYPAD_A; return strcmp(name, "joypada") == 0;
        case UINT32_C(0xef160c84): input = Core::Input::JOYPAD_X; return strcmp(name, "joypadx") == 0;
        case UINT32_C(0xef160c78): input = Core::Input::JOYPAD_L; return strcmp(name, "joypadl") == 0;
        case UINT32_C(0xef160c7e): input = Core::Input::JOYPAD_R; return strcmp(name, "joypadr") == 0;
        case UINT32_C(0xd1d79baa): input = Core::Input::JOYPAD_L2; return strcmp(name, "joypadl2") == 0;
        case UINT32_C(0xd1d79c70): input = Core::Input::JOYPAD_R2; return strcmp(name, "joypadr2") == 0;
        case UINT32_C(0xd1d79bab): input = Core::Input::JOYPAD_L3; return strcmp(name, "joypadl3") == 0;
        case UINT32_C(0xd1d79c71): input = Core::Input::JOYPAD_R3; return strcmp(name, "joypadr3") == 0;
        case UINT32_C(0x57a09c7a): input = Core::Input::ANALOG_LEFT_X; return strcmp(name, "analogleftx") == 0;
        case UINT32_C(0x57a09c7b): input = Core::Input::ANALOG_LEFT_Y; return strcmp(name, "analoglefty") == 0;
        case UINT32_C(0x59fbdacd): input = Core::Input::ANALOG_RIGHT_X; return strcmp(name, "analogrightx") == 0;
        case UINT32_C(0x59fbdace): input = Core::Input::ANALOG_RIGHT_Y; return strcmp(name, "analogrighty") == 0;
        case UINT32_C(0x36407f59): input = Core::Input::ANALOG_B; return strcmp(name, "analogb") == 0;
        case UINT32_C(0x36407f70): input = Core::Input::ANALOG_Y; return strcmp(name, "analogy") == 0;
        case UINT32_C(0x5c0b5057): input = Core::Input::ANALOG_SELECT; return strcmp(name, "analogselect") == 0;
        case UINT32_C(0x58276c25): input = Core::Input::ANALOG_START; return strcmp(name, "analogstart") == 0;
        case UINT32_C(0xfe506d5c): input = Core::Input::ANALOG_UP; return strcmp(name, "analogup") == 0;
        case UINT32_C(0xd417ed6f): input = Core::Input::ANALOG_DOWN; return strcmp(name, "analogdown") == 0;
        case UINT32_C(0xd41c23c2): input = Core::Input::ANALOG_LEFT; return strcmp(name, "analogleft") == 0;
        case UINT32_C(0x580f63b5): input = Core::Input::ANALOG_RIGHT; return strcmp(name, "analogright") == 0;
        case UINT32_C(0x36407f58): input = Core::Input::ANALOG_A; return strcmp(name, "analoga") == 0;
        case UINT32_C(0x36407f6f): input = Core::Input::ANALOG_X; return strcmp(name, "analogx") == 0;
        case UINT32_C(0x36407f63): input = Core::Input::ANALOG_L; return strcmp(name, "analogl") == 0;
        case UINT32_C(0x36407f69): input = Core::Input::ANALOG_R; return strcmp(name, "analogr") == 0;
        case UINT32_C(0xfe506bf5): input = Core::Input::ANALOG_L2; return strcmp(name, "analogl2") == 0;
        case UINT32_C(0xfe506cbb): input = Core::Input::ANALOG_R2; return strcmp(name, "analogr2") == 0;
        case UINT32_C(0xfe506bf6): input = Core::Input::ANALOG_L3; return strcmp(name, "analogl3") == 0;
        case UINT32_C(0xfe506cbc): input = Core::Input::ANALOG_R3; return strcmp(name, "analogr3") == 0;
        case UINT32_C(0x0e3c1046): input = Core::Input::MOUSE_X; return strcmp(name, "mousex") == 0;
        case UINT32_C(0x0e3c1047): input = Core::Input::MOUSE_Y; return strcmp(name, "mousey") == 0;
        case UINT32_C(0x3da39939): input = Core::Input::MOUSE_LEFT; return strcmp(name, "mouseleft") == 0;
        case UINT32_C(0xf285880c): input = Core::Input::MOUSE_RIGHT; return strcmp(name, "mouseright") == 0;
        case UINT32_C(0x286fd988): input = Core::Input::MOUSE_WHEELUP; return strcmp(name, "mousewheelup") == 0;
        case UINT32_C(0x03c3149b): input = Core::Input::MOUSE_WHEELDOWN; return strcmp(name, "mousewheeldown") == 0;
        case UINT32_C(0x378b079d): input = Core::Input::MOUSE_MIDDLE; return strcmp(name, "mousemiddle") == 0;
        case UINT32_C(0x7a806e34): input = Core::Input::MOUSE_HORIZ_WHEELUP; return strcmp(name, "mousehorizwheelup") == 0;
        case UINT32_C(0x1c4b8447): input = Core::Input::MOUSE_HORIZ_WHEELDOWN; return strcmp(name, "mousehorizwheeldown") == 0;
        case UINT32_C(0xf753a2be): input = Core::Input::MOUSE_BUTTON_4; return strcmp(name, "mousebutton4") == 0;
        case UINT32_C(0xf753a2bf): input = Core::Input::MOUSE_BUTTON_5; return strcmp(name, "mousebutton5") == 0;
        case UINT32_C(0xbb5d5b76): input = Core::Input::KEYBOARD; return strcmp(name, "keyboard") == 0;
        case UINT32_C(0xedc591ff): input = Core::Input::LIGHTGUN_SCREEN_X; return strcmp(name, "lightgunscreenx") == 0;
        case UINT32_C(0xedc59200): input = Core::Input::LIGHTGUN_SCREEN_Y; return strcmp(name, "lightgunscreeny") == 0;
        case UINT32_C(0x8b6b3c7e): input = Core::Input::LIGHTGUN_IS_OFFSCREEN; return strcmp(name, "lightgunisoffscreen") == 0;
        case UINT32_C(0x5d1b73bb): input = Core::Input::LIGHTGUN_TRIGGER; return strcmp(name, "lightguntrigger") == 0;
        case UINT32_C(0x4ad1e4be): input = Core::Input::LIGHTGUN_RELOAD; return strcmp(name, "lightgunreload") == 0;
        case UINT32_C(0xc48f00b6): input = Core::Input::LIGHTGUN_AUX_A; return strcmp(name, "lightgunauxa") == 0;
        case UINT32_C(0xc48f00b7): input = Core::Input::LIGHTGUN_AUX_B; return strcmp(name, "lightgunauxb") == 0;
        case UINT32_C(0x57b3e4f5): input = Core::Input::LIGHTGUN_START; return strcmp(name, "lightgunstart") == 0;
        case UINT32_C(0x4d26e327): input = Core::Input::LIGHTGUN_SELECT; return strcmp(name, "lightgunselect") == 0;
        case UINT32_C(0xc48f00b8): input = Core::Input::LIGHTGUN_AUX_C; return strcmp(name, "lightgunauxc") == 0;
        case UINT32_C(0x2aea8525): input = Core::Input::LIGHTGUN_DPAD_UP; return strcmp(name, "lightgundpadup") == 0;
        case UINT32_C(0x8f971b78): input = Core::Input::LIGHTGUN_DPAD_DOWN; return strcmp(name, "lightgundpaddown") == 0;
        case UINT32_C(0x8f9b51cb): input = Core::Input::LIGHTGUN_DPAD_LEFT; return strcmp(name, "lightgundpadleft") == 0;
        case UINT32_C(0x837452de): input = Core::Input::LIGHTGUN_DPAD_RIGHT; return strcmp(name, "lightgundpadright") == 0;
        case UINT32_C(0x35f64e5e): input = Core::Input::POINTER_X; return strcmp(name, "pointerx") == 0;
        case UINT32_C(0x35f64e5f): input = Core::Input::POINTER_Y; return strcmp(name, "pointery") == 0;
        case UINT32_C(0x8259ff9c): input = Core::Input::POINTER_PRESSED; return strcmp(name, "pointerpressed") == 0;
    }

    return false;
}

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

int w_Core_getDecoder(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
    luax_pushtype<love::sound::Decoder>(L, core->getDecoder());
    return 1;
}

int w_Core_setControllerPortDevice(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
	int port = (int) luaL_checkinteger(L, 2);
	int device = (int) luaL_checkinteger(L, 3);

	if (port < 0 || port >= Core::MaxPorts)
		return luaL_error(L, "port outside of valid range [0,%d]", Core::MaxPorts - 1);
	
	core->setControllerPortDevice(port, device);
	return 0;
}

int w_Core_setInput(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
	int port = (int) luaL_checkinteger(L, 2);

	Core::Input input;
	const char *inputStr = luaL_checkstring(L, 3);

	if (!getInput(inputStr, input))
		return luaL_error(L, "Invalid core input '%s'", inputStr);
	
	// TODO deal with keyboard

	if (lua_isnoneornil(L, 5))
	{
		int value = (int) luaL_checkinteger(L, 4);
			
		if (!core->setInput(port, input, value))
			return luaL_error(L, "Invalid core input '%s'", inputStr);
	}
	else
	{
		int index = (int) luaL_checkinteger(L, 4);
		int value = (int) luaL_checkinteger(L, 5);
			
		if (!core->setInput(port, input, index, value))
			return luaL_error(L, "Invalid core input '%s' with index %d", inputStr, index);
	}

	return 0;
}

static const luaL_Reg core_functions[] =
{
    {"step", w_Core_step},
	{"getImage", w_Core_getImage},
	{"getAspectRatio", w_Core_getAspectRatio},
    {"getDecoder", w_Core_getDecoder},
	{"setControllerPortDevice", w_Core_setControllerPortDevice},
	{"setInput", w_Core_setInput},
	{0, 0}
};

int luaopen_core(lua_State *L)
{
	return luax_register_type(L, &Core::type, core_functions, nullptr);
}

} // libretro
} // love
