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

StringMap<Core::Input, (unsigned) Core::Input::COUNT>::Entry inputEntries[] =
{
    { "none",                Core::Input::NONE                  },
    { "joypadb",             Core::Input::JOYPAD_B              },
    { "joypady",             Core::Input::JOYPAD_Y              },
    { "joypadselect",        Core::Input::JOYPAD_SELECT         },
    { "joypadstart",         Core::Input::JOYPAD_START          },
    { "joypadup",            Core::Input::JOYPAD_UP             },
    { "joypaddown",          Core::Input::JOYPAD_DOWN           },
    { "joypadleft",          Core::Input::JOYPAD_LEFT           },
    { "joypadright",         Core::Input::JOYPAD_RIGHT          },
    { "joypada",             Core::Input::JOYPAD_A              },
    { "joypadx",             Core::Input::JOYPAD_X              },
    { "joypadl",             Core::Input::JOYPAD_L              },
    { "joypadr",             Core::Input::JOYPAD_R              },
    { "joypadl2",            Core::Input::JOYPAD_L2             },
    { "joypadr2",            Core::Input::JOYPAD_R2             },
    { "joypadl3",            Core::Input::JOYPAD_L3             },
    { "joypadr3",            Core::Input::JOYPAD_R3             },
    { "analogleftx",         Core::Input::ANALOG_LEFT_X         },
    { "analoglefty",         Core::Input::ANALOG_LEFT_Y         },
    { "analogrightx",        Core::Input::ANALOG_RIGHT_X        },
    { "analogrighty",        Core::Input::ANALOG_RIGHT_Y        },
    { "analogb",             Core::Input::ANALOG_B              },
    { "analogy",             Core::Input::ANALOG_Y              },
    { "analogselect",        Core::Input::ANALOG_SELECT         },
    { "analogstart",         Core::Input::ANALOG_START          },
    { "analogup",            Core::Input::ANALOG_UP             },
    { "analogdown",          Core::Input::ANALOG_DOWN           },
    { "analogleft",          Core::Input::ANALOG_LEFT           },
    { "analogright",         Core::Input::ANALOG_RIGHT          },
    { "analoga",             Core::Input::ANALOG_A              },
    { "analogx",             Core::Input::ANALOG_X              },
    { "analogl",             Core::Input::ANALOG_L              },
    { "analogr",             Core::Input::ANALOG_R              },
    { "analogl2",            Core::Input::ANALOG_L2             },
    { "analogr2",            Core::Input::ANALOG_R2             },
    { "analogl3",            Core::Input::ANALOG_L3             },
    { "analogr3",            Core::Input::ANALOG_R3             },
    { "mousex",              Core::Input::MOUSE_X               },
    { "mousey",              Core::Input::MOUSE_Y               },
    { "mouseleft",           Core::Input::MOUSE_LEFT            },
    { "mouseright",          Core::Input::MOUSE_RIGHT           },
    { "mousewheelup",        Core::Input::MOUSE_WHEELUP         },
    { "mousewheeldown",      Core::Input::MOUSE_WHEELDOWN       },
    { "mousemiddle",         Core::Input::MOUSE_MIDDLE          },
    { "mousehorizwheelup",   Core::Input::MOUSE_HORIZ_WHEELUP   },
    { "mousehorizwheeldown", Core::Input::MOUSE_HORIZ_WHEELDOWN },
    { "mousebutton4",        Core::Input::MOUSE_BUTTON_4        },
    { "mousebutton5",        Core::Input::MOUSE_BUTTON_5        },
    { "keyboard",            Core::Input::KEYBOARD              },
    { "lightgunscreenx",     Core::Input::LIGHTGUN_SCREEN_X     },
    { "lightgunscreeny",     Core::Input::LIGHTGUN_SCREEN_y     },
    { "lightgunisoffscreen", Core::Input::LIGHTGUN_IS_OFFSCREEN },
    { "lightguntrigger",     Core::Input::LIGHTGUN_TRIGGER      },
    { "lightgunreload",      Core::Input::LIGHTGUN_RELOAD       },
    { "lightgunauxa",        Core::Input::LIGHTGUN_AUX_A        },
    { "lightgunauxb",        Core::Input::LIGHTGUN_AUX_B        },
    { "lightgunstart",       Core::Input::LIGHTGUN_START        },
    { "lightgunselect",      Core::Input::LIGHTGUN_SELECT       },
    { "lightgunauxc",        Core::Input::LIGHTGUN_AUX_C        },
    { "lightgundpadup",      Core::Input::LIGHTGUN_DPAD_UP      },
    { "lightgundpaddown",    Core::Input::LIGHTGUN_DPAD_DOWN    },
    { "lightgundpadleft",    Core::Input::LIGHTGUN_DPAD_LEFT    },
    { "lightgundpadright",   Core::Input::LIGHTGUN_DPAD_RIGHT   },
    { "pointerx",            Core::Input::POINTER_X             },
    { "pointery",            Core::Input::POINTER_Y             },
    { "pointerpressed",      Core::Input::POINTER_PRESSED       }
};

StringMap<Core::Input, (unsigned) Core::Input::COUNT> inputs(inputEntries, sizeof(inputEntries));

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
	int port = (int) luaL_checkinteger(L, 2);
	int device = (int) luaL_checkinteger(L, 3);

	if (port < 0 || port >= Core::MaxPorts)
		return luaL_error(L, "port outside of valid range [0,%d]", Core::MaxPorts - 1);
	
	if (device < 0 || device >= Core::MaxDevices)
		return luaL_error(L, "device outside of valid range [0,%d]", Core::MaxDevices - 1);

	core->setControllerPortDevice(port, device);
	return 0;
}

int w_Core_setInput(lua_State *L)
{
	auto core = luax_checkcore(L, 1);
	int port = (int) luaL_checkinteger(L, 2);

	Core::Input input;
	const char *inputStr = luaL_checkstring(L, 3);

	if (inputs.find(inputStr, input))
		return luax_enumerror(L, "core input", inputs.getNames(), inputStr);
	
	// TODO deal with keyboard

	if (lua_isnoneornil(L, 5))
	{
		int value = (int) luaL_checkinteger(L, 4);
			
		if (!core->setInput(port, input, value))
			return luaL_error(L, "invalid input");
	}
	else
	{
		int index = (int) luaL_checkinteger(L, 4);
		int value = (int) luaL_checkinteger(L, 5);
			
		if (!core->setInput(port, input, index, value))
			return luaL_error(L, "invalid input");
	}

	return 0;
}

static const luaL_Reg core_functions[] =
{
    {"step", w_Core_step},
	{"getImage", w_Core_getImage},
	{"getAspectRatio", w_Core_getAspectRatio},
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
