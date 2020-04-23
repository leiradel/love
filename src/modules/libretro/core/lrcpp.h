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
        addrSpace = value.addrspace;
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
