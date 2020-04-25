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

#ifndef LOVE_LIBRETRO_CORE_LRCPP_H
#define LOVE_LIBRETRO_CORE_LRCPP_H

// STL
#include <string>
#include <vector>

#include "libretro.h"

namespace lrcpp
{

enum class PixelFormat
{
    XRGB1555 = RETRO_PIXEL_FORMAT_0RGB1555,
    XRGB8888 = RETRO_PIXEL_FORMAT_XRGB8888,
    RGB565,
    Unknown = RETRO_PIXEL_FORMAT_UNKNOWN
};

enum class Device
{
    None = RETRO_DEVICE_NONE,
    Joypad = RETRO_DEVICE_JOYPAD,
    Mouse = RETRO_DEVICE_MOUSE,
    Keyboard = RETRO_DEVICE_KEYBOARD,
    Lightgun = RETRO_DEVICE_LIGHTGUN,
    Analog = RETRO_DEVICE_ANALOG,
    Pointer = RETRO_DEVICE_POINTER
};

enum class DeviceId
{
    JoypadB = RETRO_DEVICE_ID_JOYPAD_B,
    JoypadY = RETRO_DEVICE_ID_JOYPAD_Y,
    JoypadSelect = RETRO_DEVICE_ID_JOYPAD_SELECT,
    JoypadStart = RETRO_DEVICE_ID_JOYPAD_START,
    JoypadUp = RETRO_DEVICE_ID_JOYPAD_UP,
    JoypadDown = RETRO_DEVICE_ID_JOYPAD_DOWN,
    JoypadLeft = RETRO_DEVICE_ID_JOYPAD_LEFT,
    JoypadRight = RETRO_DEVICE_ID_JOYPAD_RIGHT,
    JoypadA = RETRO_DEVICE_ID_JOYPAD_A,
    JoypadX = RETRO_DEVICE_ID_JOYPAD_X,
    JoypadL = RETRO_DEVICE_ID_JOYPAD_L,
    JoypadR = RETRO_DEVICE_ID_JOYPAD_R,
    JoypadL2 = RETRO_DEVICE_ID_JOYPAD_L2,
    JoypadR2 = RETRO_DEVICE_ID_JOYPAD_R2,
    JoypadL3 = RETRO_DEVICE_ID_JOYPAD_L3,
    JoypadR3 = RETRO_DEVICE_ID_JOYPAD_R3,
    AnalogX = RETRO_DEVICE_ID_ANALOG_X,
    AnalogY = RETRO_DEVICE_ID_ANALOG_Y,
    MouseX = RETRO_DEVICE_ID_MOUSE_X,
    MouseY = RETRO_DEVICE_ID_MOUSE_Y,
    MouseLeft = RETRO_DEVICE_ID_MOUSE_LEFT,
    MouseRight = RETRO_DEVICE_ID_MOUSE_RIGHT,
    MouseWheelup = RETRO_DEVICE_ID_MOUSE_WHEELUP,
    MouseWheeldown = RETRO_DEVICE_ID_MOUSE_WHEELDOWN,
    MouseMiddle = RETRO_DEVICE_ID_MOUSE_MIDDLE,
    MouseHorizWheelup = RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELUP,
    MouseHorizWheeldown = RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN,
    MouseButton4 = RETRO_DEVICE_ID_MOUSE_BUTTON_4,
    MouseButton5 = RETRO_DEVICE_ID_MOUSE_BUTTON_5,
    LightgunScreenX = RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X,
    LightgunScreenY = RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y,
    LightgunIsOffscreen = RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN,
    LightgunTrigger = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER,
    LightgunReload = RETRO_DEVICE_ID_LIGHTGUN_RELOAD,
    LightgunAuxA = RETRO_DEVICE_ID_LIGHTGUN_AUX_A,
    LightgunAuxB = RETRO_DEVICE_ID_LIGHTGUN_AUX_B,
    LightgunStart = RETRO_DEVICE_ID_LIGHTGUN_START,
    LightgunSelect = RETRO_DEVICE_ID_LIGHTGUN_SELECT,
    LightgunAuxC = RETRO_DEVICE_ID_LIGHTGUN_AUX_C,
    LightgunDpadUp = RETRO_DEVICE_ID_LIGHTGUN_DPAD_UP,
    LightgunDpadDown = RETRO_DEVICE_ID_LIGHTGUN_DPAD_DOWN,
    LightgunDpadLeft = RETRO_DEVICE_ID_LIGHTGUN_DPAD_LEFT,
    LightgunDpadRight = RETRO_DEVICE_ID_LIGHTGUN_DPAD_RIGHT,
    PointerX = RETRO_DEVICE_ID_POINTER_X,
    PointerY = RETRO_DEVICE_ID_POINTER_Y,
    PointerPressed = RETRO_DEVICE_ID_POINTER_PRESSED
};

enum class Key
{
    Backspace = RETROK_BACKSPACE,
    Tab = RETROK_TAB,
    Clear = RETROK_CLEAR,
    Return = RETROK_RETURN,
    Pause = RETROK_PAUSE,
    Escape = RETROK_ESCAPE,
    Space = RETROK_SPACE,
    Exclaim = RETROK_EXCLAIM,
    Quotedbl = RETROK_QUOTEDBL,
    Hash = RETROK_HASH,
    Dollar = RETROK_DOLLAR,
    Ampersand = RETROK_AMPERSAND,
    Quote = RETROK_QUOTE,
    Leftparen = RETROK_LEFTPAREN,
    Rightparen = RETROK_RIGHTPAREN,
    Asterisk = RETROK_ASTERISK,
    Plus = RETROK_PLUS,
    Comma = RETROK_COMMA,
    Minus = RETROK_MINUS,
    Period = RETROK_PERIOD,
    Slash = RETROK_SLASH,
    _0 = RETROK_0,
    _1 = RETROK_1,
    _2 = RETROK_2,
    _3 = RETROK_3,
    _4 = RETROK_4,
    _5 = RETROK_5,
    _6 = RETROK_6,
    _7 = RETROK_7,
    _8 = RETROK_8,
    _9 = RETROK_9,
    Colon = RETROK_COLON,
    Semicolon = RETROK_SEMICOLON,
    Less = RETROK_LESS,
    Equals = RETROK_EQUALS,
    Greater = RETROK_GREATER,
    Question = RETROK_QUESTION,
    At = RETROK_AT,
    Leftbracket = RETROK_LEFTBRACKET,
    Backslash = RETROK_BACKSLASH,
    Rightbracket = RETROK_RIGHTBRACKET,
    Caret = RETROK_CARET,
    Underscore = RETROK_UNDERSCORE,
    Backquote = RETROK_BACKQUOTE,
    A = RETROK_a,
    B = RETROK_b,
    C = RETROK_c,
    D = RETROK_d,
    E = RETROK_e,
    F = RETROK_f,
    G = RETROK_g,
    H = RETROK_h,
    I = RETROK_i,
    J = RETROK_j,
    K = RETROK_k,
    L = RETROK_l,
    M = RETROK_m,
    N = RETROK_n,
    O = RETROK_o,
    P = RETROK_p,
    Q = RETROK_q,
    R = RETROK_r,
    S = RETROK_s,
    T = RETROK_t,
    U = RETROK_u,
    V = RETROK_v,
    W = RETROK_w,
    X = RETROK_x,
    Y = RETROK_y,
    Z = RETROK_z,
    Leftbrace = RETROK_LEFTBRACE,
    Bar = RETROK_BAR,
    Rightbrace = RETROK_RIGHTBRACE,
    Tilde = RETROK_TILDE,
    Delete = RETROK_DELETE,
    Kp0 = RETROK_KP0,
    Kp1 = RETROK_KP1,
    Kp2 = RETROK_KP2,
    Kp3 = RETROK_KP3,
    Kp4 = RETROK_KP4,
    Kp5 = RETROK_KP5,
    Kp6 = RETROK_KP6,
    Kp7 = RETROK_KP7,
    Kp8 = RETROK_KP8,
    Kp9 = RETROK_KP9,
    Kpperiod = RETROK_KP_PERIOD,
    Kpdivide = RETROK_KP_DIVIDE,
    Kpmultiply = RETROK_KP_MULTIPLY,
    Kpminus = RETROK_KP_MINUS,
    Kpplus = RETROK_KP_PLUS,
    Kpenter = RETROK_KP_ENTER,
    Kpequals = RETROK_KP_EQUALS,
    Up = RETROK_UP,
    Down = RETROK_DOWN,
    Right = RETROK_RIGHT,
    Left = RETROK_LEFT,
    Insert = RETROK_INSERT,
    Home = RETROK_HOME,
    End = RETROK_END,
    Pageup = RETROK_PAGEUP,
    Pagedown = RETROK_PAGEDOWN,
    F1 = RETROK_F1,
    F2 = RETROK_F2,
    F3 = RETROK_F3,
    F4 = RETROK_F4,
    F5 = RETROK_F5,
    F6 = RETROK_F6,
    F7 = RETROK_F7,
    F8 = RETROK_F8,
    F9 = RETROK_F9,
    F10 = RETROK_F10,
    F11 = RETROK_F11,
    F12 = RETROK_F12,
    F13 = RETROK_F13,
    F14 = RETROK_F14,
    F15 = RETROK_F15,
    Numlock = RETROK_NUMLOCK,
    Capslock = RETROK_CAPSLOCK,
    Scrollock = RETROK_SCROLLOCK,
    Rshift = RETROK_RSHIFT,
    Lshift = RETROK_LSHIFT,
    Rctrl = RETROK_RCTRL,
    Lctrl = RETROK_LCTRL,
    Ralt = RETROK_RALT,
    Lalt = RETROK_LALT,
    Rmeta = RETROK_RMETA,
    Lmeta = RETROK_LMETA,
    Lsuper = RETROK_LSUPER,
    Rsuper = RETROK_RSUPER,
    Mode = RETROK_MODE,
    Compose = RETROK_COMPOSE,
    Help = RETROK_HELP,
    Print = RETROK_PRINT,
    Sysreq = RETROK_SYSREQ,
    Break = RETROK_BREAK,
    Menu = RETROK_MENU,
    Power = RETROK_POWER,
    Euro = RETROK_EURO,
    Undo = RETROK_UNDO
};

enum class DeviceIndex
{
    AnalogLeft = RETRO_DEVICE_INDEX_ANALOG_LEFT,
    AnalogRight = RETRO_DEVICE_INDEX_ANALOG_RIGHT,
    AnalogButton = RETRO_DEVICE_INDEX_ANALOG_BUTTON
};

struct InputDescriptor
{
    InputDescriptor() = default;

    InputDescriptor(const retro_input_descriptor& value)
    {
        port = value.port;
        device = value.device;
        index = value.index;
        id = value.id;
        description = value.description;
    }

    unsigned port;
    unsigned device;
    unsigned index;
    unsigned id;
    std::string description;
};

struct Variable
{
    Variable() = default;

    Variable(const retro_variable& value)
    {
        key = value.key;
        this->value = value.value;
    }

    std::string key;
    std::string value;
};

enum class HWContextType
{
    None = RETRO_HW_CONTEXT_NONE,
    OpenGL = RETRO_HW_CONTEXT_OPENGL,
    OpenGLES2 = RETRO_HW_CONTEXT_OPENGLES2,
    OpenGLCore = RETRO_HW_CONTEXT_OPENGL_CORE,
    OpenGLES3 = RETRO_HW_CONTEXT_OPENGLES3,
    OpenGLESVersion = RETRO_HW_CONTEXT_OPENGLES_VERSION,
    Vulkan = RETRO_HW_CONTEXT_VULKAN,
    Direct3D = RETRO_HW_CONTEXT_DIRECT3D,
    Dummy = RETRO_HW_CONTEXT_DUMMY
};

struct HWRenderCallback
{
    HWRenderCallback() = default;

    HWRenderCallback(const retro_hw_render_callback& value)
    {
        contextType = static_cast<HWContextType>(value.context_type);
        contextReset = value.context_reset;
        getCurrentFramebuffer = value.get_current_framebuffer;
        depth = value.depth;
        stencil = value.stencil;
        bottomLeftOrigin = value.bottom_left_origin;
        versionMajor = value.version_major;
        versionMinor = value.version_minor;
        cacheContext = value.cache_context;
        contextDestroy = value.context_destroy;
        debugContext = value.debug_context;
    }

    HWContextType contextType;
    retro_hw_context_reset_t contextReset;
    retro_hw_get_current_framebuffer_t getCurrentFramebuffer;
    retro_hw_get_proc_address_t getProcAddress;
    bool depth;
    bool stencil;
    bool bottomLeftOrigin;
    unsigned versionMajor;
    unsigned versionMinor;
    bool cacheContext;
    retro_hw_context_reset_t contextDestroy;
    bool debugContext;
};

struct SystemInfo
{
    SystemInfo() = default;

    SystemInfo(const retro_system_info& value)
    {
        libraryName = value.library_name;
        libraryVersion = value.library_version;
        validExtensions = value.valid_extensions;
        needFullPath = value.need_fullpath;
        blockExtract = value.block_extract;
    }

    std::string libraryName;
    std::string libraryVersion;
    std::string validExtensions;
    bool needFullPath;
    bool blockExtract;
};

struct GameGeometry
{
    GameGeometry() = default;

    GameGeometry(const retro_game_geometry& value)
    {
        baseWidth = value.base_width;
        baseHeight = value.base_height;
        maxWidth = value.max_width;
        maxHeight = value.max_height;
        aspectRatio = value.aspect_ratio;
    }

    unsigned baseWidth;
    unsigned baseHeight;
    unsigned maxWidth;
    unsigned maxHeight;
    float aspectRatio;
};

struct SystemTiming
{
    SystemTiming() = default;

    SystemTiming(const retro_system_timing& value)
    {
        fps = value.fps;
        sampleRate = value.sample_rate;
    }

    double fps;
    double sampleRate;
};

struct SystemAVInfo
{
    SystemAVInfo() = default;

    SystemAVInfo(const retro_system_av_info& value)
        : geometry(value.geometry)
        , timing(value.timing)
    {}

    GameGeometry geometry;
    SystemTiming timing;
};

struct SubsystemMemoryInfo
{
    SubsystemMemoryInfo() = default;

    SubsystemMemoryInfo(const retro_subsystem_memory_info& value)
    {
        extension = value.extension;
        type = value.type;
    }

    std::string extension;
    unsigned type;
};

struct SubsystemRomInfo
{
    SubsystemRomInfo() = default;

    SubsystemRomInfo(const retro_subsystem_rom_info& value)
    {
        desc = value.desc;
        validExtensions = value.valid_extensions;
        needFullpath = value.need_fullpath;
        blockExtract = value.block_extract;
        required = value.required;

        memory.resize(value.num_memory);

        for (unsigned i = 0; i < value.num_memory; i++)
            memory[i] = SubsystemMemoryInfo(value.memory[i]);
    }

    std::string desc;
    std::string validExtensions;
    bool needFullpath;
    bool blockExtract;
    bool required;
    std::vector<SubsystemMemoryInfo> memory;
};

struct SubsystemInfo
{
    SubsystemInfo() = default;

    SubsystemInfo(const retro_subsystem_info& value)
    {
        desc = value.desc;
        ident = value.ident;
        id = value.id;

        roms.resize(value.num_roms);

        for (unsigned i = 0; i < value.num_roms; i++)
            roms[i] = SubsystemRomInfo(value.roms[i]);
    }

    std::string desc;
    std::string ident;
    std::vector<SubsystemRomInfo> roms;
    unsigned id;
};

struct ControllerDescription
{
    ControllerDescription() = default;

    ControllerDescription(const retro_controller_description& value)
    {
        desc = value.desc;
        id = value.id;
    }

    std::string desc;
    unsigned id;
};

struct ControllerInfo
{
    ControllerInfo() = default;

    ControllerInfo(const retro_controller_info& value)
    {
        types.resize(value.num_types);

        for (unsigned i = 0; i < value.num_types; i++)
            types[i] = ControllerDescription(value.types[i]);
    }

    std::vector<ControllerDescription> types;
};

struct DiskControlCallback
{
    DiskControlCallback() = default;

    DiskControlCallback(const retro_disk_control_callback& value)
    {
        setEjectState = value.set_eject_state;
        getEjectState = value.get_eject_state;
        getImageIndex = value.get_image_index;
        setImageIndex = value.set_image_index;
        getNumImages = value.get_num_images;
        replaceImageIndex = value.replace_image_index;
        addImageIndex = value.add_image_index;
    }

    retro_set_eject_state_t setEjectState;
    retro_get_eject_state_t getEjectState;
    retro_get_image_index_t getImageIndex;
    retro_set_image_index_t setImageIndex;
    retro_get_num_images_t  getNumImages;
    retro_replace_image_index_t replaceImageIndex;
    retro_add_image_index_t addImageIndex;
};

struct MemoryDescriptor
{
    MemoryDescriptor() = default;

    MemoryDescriptor(const retro_memory_descriptor& value)
    {
        flags = value.flags;
        ptr = value.ptr;
        offset = value.offset;
        start = value.start;
        select = value.select;
        disconnect = value.disconnect;
        len = value.len;
        addrSpace = value.addrspace ? value.addrspace : "";
    }

    uint64_t flags;
    void *ptr;
    size_t offset;
    size_t start;
    size_t select;
    size_t disconnect;
    size_t len;
    std::string addrSpace;
};

struct MemoryMap
{
    MemoryMap() = default;

    MemoryMap(const retro_memory_map& value)
    {
        descriptors.resize(value.num_descriptors);

        for (unsigned i = 0; i < value.num_descriptors; i++)
            descriptors[i] = MemoryDescriptor(value.descriptors[i]);
    }

    std::vector<MemoryDescriptor> descriptors;
};

} // libretro

#endif // LOVE_LIBRETRO_CORE_LRCPP_H
