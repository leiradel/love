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

struct Enum
{
	const char *name;
	int value;
};

static int findEnum(const Enum* values, size_t count, const char *name)
{
	for (size_t i = 0; i < count; i++)
		if (values[i].name[0] == name[0] && !strcmp(values[i].name, name))
			return values[i].value;

	return -1;
}

static int getDevice(const char *device)
{
	static const Enum devices[] =
	{
		{ "none", RETRO_DEVICE_NONE },
		{ "joypad", RETRO_DEVICE_JOYPAD },
		{ "mouse", RETRO_DEVICE_MOUSE },
		{ "keyboard", RETRO_DEVICE_KEYBOARD },
		{ "lightgun", RETRO_DEVICE_LIGHTGUN },
		{ "analog", RETRO_DEVICE_ANALOG },
		{ "pointer", RETRO_DEVICE_POINTER }
	};

	return findEnum(devices, sizeof(devices) / sizeof(devices[0]), device);
}

static int getIndex(int device, const char *index)
{
	static const Enum indexes[] =
	{
		{ "left", RETRO_DEVICE_INDEX_ANALOG_LEFT },
		{ "right", RETRO_DEVICE_INDEX_ANALOG_RIGHT },
		{ "button", RETRO_DEVICE_INDEX_ANALOG_BUTTON }
	};

	if (index[0] == 0)
		return 3; // No index

	if (device != RETRO_DEVICE_ANALOG)
		return -1; // Invalid device
	
	return findEnum(indexes, sizeof(indexes) / sizeof(indexes[0]), index);
}

static int getId(int device, int index, const char *id)
{
	static const Enum joypad[] =
	{
		{ "b", RETRO_DEVICE_ID_JOYPAD_B },
		{ "y", RETRO_DEVICE_ID_JOYPAD_Y },
		{ "select", RETRO_DEVICE_ID_JOYPAD_SELECT },
		{ "start", RETRO_DEVICE_ID_JOYPAD_START },
		{ "up", RETRO_DEVICE_ID_JOYPAD_UP },
		{ "down", RETRO_DEVICE_ID_JOYPAD_DOWN },
		{ "left", RETRO_DEVICE_ID_JOYPAD_LEFT },
		{ "l", RETRO_DEVICE_ID_JOYPAD_L },
		{ "l2", RETRO_DEVICE_ID_JOYPAD_L2 },
		{ "l3", RETRO_DEVICE_ID_JOYPAD_L3 },
		{ "right", RETRO_DEVICE_ID_JOYPAD_RIGHT },
		{ "r", RETRO_DEVICE_ID_JOYPAD_R },
		{ "r2", RETRO_DEVICE_ID_JOYPAD_R2 },
		{ "r3", RETRO_DEVICE_ID_JOYPAD_R3 },
		{ "a", RETRO_DEVICE_ID_JOYPAD_A },
		{ "x", RETRO_DEVICE_ID_JOYPAD_X }
	};

	static const Enum analog[] =
	{
		{ "x", RETRO_DEVICE_ID_ANALOG_X },
		{ "y", RETRO_DEVICE_ID_ANALOG_Y }
	};

	static const Enum mouse[] =
	{
		{ "x", RETRO_DEVICE_ID_MOUSE_X },
		{ "y", RETRO_DEVICE_ID_MOUSE_Y },
		{ "left", RETRO_DEVICE_ID_MOUSE_LEFT },
		{ "right", RETRO_DEVICE_ID_MOUSE_RIGHT },
		{ "wheelup", RETRO_DEVICE_ID_MOUSE_WHEELUP },
		{ "wheeldown", RETRO_DEVICE_ID_MOUSE_WHEELDOWN },
		{ "middle", RETRO_DEVICE_ID_MOUSE_MIDDLE },
		{ "hwheelup", RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELUP },
		{ "hwheeldown", RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN },
		{ "button4", RETRO_DEVICE_ID_MOUSE_BUTTON_4 },
		{ "button5", RETRO_DEVICE_ID_MOUSE_BUTTON_5 }
	};

	static const Enum lightgun[] =
	{
		{ "screenx", RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X },
		{ "screeny", RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y },
		{ "start", RETRO_DEVICE_ID_LIGHTGUN_START },
		{ "select", RETRO_DEVICE_ID_LIGHTGUN_SELECT },
		{ "isoffscreen", RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN },
		{ "trigger", RETRO_DEVICE_ID_LIGHTGUN_TRIGGER },
		{ "reload", RETRO_DEVICE_ID_LIGHTGUN_RELOAD },
		{ "right", RETRO_DEVICE_ID_LIGHTGUN_DPAD_RIGHT },
		{ "auxa", RETRO_DEVICE_ID_LIGHTGUN_AUX_A },
		{ "auxb", RETRO_DEVICE_ID_LIGHTGUN_AUX_B },
		{ "auxc", RETRO_DEVICE_ID_LIGHTGUN_AUX_C },
		{ "up", RETRO_DEVICE_ID_LIGHTGUN_DPAD_UP },
		{ "down", RETRO_DEVICE_ID_LIGHTGUN_DPAD_DOWN },
		{ "left", RETRO_DEVICE_ID_LIGHTGUN_DPAD_LEFT }
	};

	static const Enum pointer[] =
	{
		{ "x", RETRO_DEVICE_ID_POINTER_X },
		{ "y", RETRO_DEVICE_ID_POINTER_Y },
		{ "pressed", RETRO_DEVICE_ID_POINTER_PRESSED }
	};

	if (device == RETRO_DEVICE_JOYPAD)
	{
		if (index != 3)
			return -1;
		
		return findEnum(joypad, sizeof(joypad) / sizeof(joypad[0]), id);
	}
	else if (device == RETRO_DEVICE_ANALOG)
	{
		if (index != RETRO_DEVICE_INDEX_ANALOG_LEFT &&
		    index != RETRO_DEVICE_INDEX_ANALOG_RIGHT &&
			index != RETRO_DEVICE_INDEX_ANALOG_BUTTON)
			return -1;
		
		return findEnum(analog, sizeof(analog) / sizeof(analog[0]), id);
	}
	else if (device == RETRO_DEVICE_MOUSE)
	{
		if (index != 3)
			return -1;
		
		return findEnum(mouse, sizeof(mouse) / sizeof(mouse[0]), id);
	}
	else if (device == RETRO_DEVICE_LIGHTGUN)
	{
		if (index != 3)
			return -1;
		
		return findEnum(lightgun, sizeof(lightgun) / sizeof(lightgun[0]), id);
	}
	else if (device == RETRO_DEVICE_POINTER)
	{
		if (index != 3)
			return -1;
		
		return findEnum(pointer, sizeof(pointer) / sizeof(pointer[0]), id);
	}

	return -1;
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

int w_Core_setControllerPortDevice(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
	lua_Integer port = luaL_checkinteger(L, 2);
	lua_Integer idevice = luaL_optinteger(L, 3, -1);

	if (port < 0 || port > 7)
		return luaL_error(L, "port outside of valid range [0,7]");
	
	if (idevice == -1)
	{
		const char *device = luaL_checkstring(L, 3);
		idevice = getDevice(device);

		if (idevice == -1)
			return luaL_error(L, "invalid device %s", device);
	}

	core->setControllerPortDevice(port, idevice);
	return 0;
}

int w_Core_setInput(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
	lua_Integer port = luaL_checkinteger(L, 2);
	const char *device = luaL_checkstring(L, 3);
	const char *index = luaL_optstring(L, 4, "");
	const char *id = luaL_checkstring(L, 5);
	lua_Integer value = luaL_checkinteger(L, 6);

	if (port < 0 || port > 7)
		return luaL_error(L, "port outside of valid range [0,7]");
	
	int idevice = getDevice(device);

	if (idevice == -1)
		return luaL_error(L, "invalid device %s", device);
	
	int iindex = getIndex(idevice, index);

	if (iindex == -1)
		return luaL_error(L, "invalid index %s for device %s", index, device);

	int iid = getId(idevice, iindex, id);

	if (iid == -1)
		return luaL_error(L, "invalid id %s for index %s and device %s", id, index[0] ? index : "nil", device);

	if (iindex == 3)
		iindex = 0;

	core->setInput(port, idevice, iindex, iid, value);
	return 0;
}

static const luaL_Reg core_functions[] =
{
    { "step", w_Core_step },
	{ "getImage", w_Core_getImage },
	{ "getAspectRatio", w_Core_getAspectRatio },
	{ "setControllerPortDevice", w_Core_setControllerPortDevice },
	{ "setInput", w_Core_setInput },
	{ 0, 0 }
};

int luaopen_core(lua_State *L)
{
	return luax_register_type(L, &Core::type, core_functions, nullptr);
}

} // libretro
} // love
