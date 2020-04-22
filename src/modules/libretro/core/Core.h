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

#ifndef LOVE_LIBRETRO_LIBRETROCORE_H
#define LOVE_LIBRETRO_LIBRETROCORE_H

// STL
#include <vector>

// LOVE
#include "common/Module.h"
#include "graphics/Image.h"

#include "CoreDll.h"
#include "lrcpp.h"

namespace love
{
namespace libretro
{

class Core : public Object
{
public:

	static love::Type type;

    enum
    {
        MaxPorts = 16,
        MaxDevices = 7,
        MaxIndices = 16,
        MaxIds = 17,
    };

    enum class Input
    {
        NONE = RETRO_DEVICE_NONE << 16,
        JOYPAD_B = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_B,
        JOYPAD_Y = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_Y,
        JOYPAD_SELECT = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_SELECT,
        JOYPAD_START = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_START,
        JOYPAD_UP = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_UP,
        JOYPAD_DOWN = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_DOWN,
        JOYPAD_LEFT = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_LEFT,
        JOYPAD_RIGHT = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_RIGHT,
        JOYPAD_A = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_A,
        JOYPAD_X = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_X,
        JOYPAD_L = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_L,
        JOYPAD_R = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_R,
        JOYPAD_L2 = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_L2,
        JOYPAD_R2 = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_R2,
        JOYPAD_L3 = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_L3,
        JOYPAD_R3 = RETRO_DEVICE_JOYPAD << 16 | RETRO_DEVICE_ID_JOYPAD_R3,
        ANALOG_LEFT_X = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_LEFT << 8 | RETRO_DEVICE_ID_ANALOG_X,
        ANALOG_LEFT_Y = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_LEFT << 8 | RETRO_DEVICE_ID_ANALOG_Y,
        ANALOG_RIGHT_X = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_RIGHT << 8 | RETRO_DEVICE_ID_ANALOG_X,
        ANALOG_RIGHT_Y = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_RIGHT << 8 | RETRO_DEVICE_ID_ANALOG_Y,
        ANALOG_B = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_B,
        ANALOG_Y = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_Y,
        ANALOG_SELECT = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_SELECT,
        ANALOG_START = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_START,
        ANALOG_UP = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_UP,
        ANALOG_DOWN = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_DOWN,
        ANALOG_LEFT = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_LEFT,
        ANALOG_RIGHT = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_RIGHT,
        ANALOG_A = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_A,
        ANALOG_X = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_X,
        ANALOG_L = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_L,
        ANALOG_R = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_R,
        ANALOG_L2 = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_L2,
        ANALOG_R2 = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_R2,
        ANALOG_L3 = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_L3,
        ANALOG_R3 = RETRO_DEVICE_ANALOG << 16 | RETRO_DEVICE_INDEX_ANALOG_BUTTON << 8 | RETRO_DEVICE_ID_JOYPAD_R3,
        MOUSE_X = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_X,
        MOUSE_Y = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_Y,
        MOUSE_LEFT = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_LEFT,
        MOUSE_RIGHT = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_RIGHT,
        MOUSE_WHEELUP = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_WHEELUP,
        MOUSE_WHEELDOWN = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_WHEELDOWN,
        MOUSE_MIDDLE = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_MIDDLE,
        MOUSE_HORIZ_WHEELUP = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELUP,
        MOUSE_HORIZ_WHEELDOWN = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN,
        MOUSE_BUTTON_4 = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_BUTTON_4,
        MOUSE_BUTTON_5 = RETRO_DEVICE_MOUSE << 16 | RETRO_DEVICE_ID_MOUSE_BUTTON_5,
        KEYBOARD = RETRO_DEVICE_KEYBOARD << 16,
        LIGHTGUN_SCREEN_X = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X,
        LIGHTGUN_SCREEN_y = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y,
        LIGHTGUN_IS_OFFSCREEN = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN,
        LIGHTGUN_TRIGGER = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_TRIGGER,
        LIGHTGUN_RELOAD = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_RELOAD,
        LIGHTGUN_AUX_A = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_AUX_A,
        LIGHTGUN_AUX_B = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_AUX_B,
        LIGHTGUN_START = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_START,
        LIGHTGUN_SELECT = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_SELECT,
        LIGHTGUN_AUX_C = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_AUX_C,
        LIGHTGUN_DPAD_UP = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_DPAD_UP,
        LIGHTGUN_DPAD_DOWN = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_DPAD_DOWN,
        LIGHTGUN_DPAD_LEFT = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_DPAD_LEFT,
        LIGHTGUN_DPAD_RIGHT = RETRO_DEVICE_LIGHTGUN << 16 | RETRO_DEVICE_ID_LIGHTGUN_DPAD_RIGHT,
        POINTER_X = RETRO_DEVICE_POINTER << 16 | RETRO_DEVICE_ID_POINTER_X,
        POINTER_Y = RETRO_DEVICE_POINTER << 16 | RETRO_DEVICE_ID_POINTER_Y,
        POINTER_PRESSED = RETRO_DEVICE_POINTER << 16 | RETRO_DEVICE_ID_POINTER_PRESSED,
        // Count
        COUNT = 66
    };

    Core(const std::string& corePath, const std::string &gamePath);
	virtual ~Core();

    love::StrongRef<love::graphics::Image> &getImage();
    float getAspectRatio() const;
    void setControllerPortDevice(unsigned port, unsigned device);
    bool setInput(unsigned port, Input input, int16_t value);
    bool setInput(unsigned port, Input input, unsigned index, int16_t value);
    bool setKey(unsigned port, unsigned key, bool pressed);
    void step();

protected:
    // Environment functions
    bool setRotation(unsigned data);
    bool getOverscan(bool *data) const;
    bool getCanDupe(bool *data) const;
    bool setMessage(const struct retro_message *data);
    bool shutdown();
    bool setPerformanceLevel(unsigned data);
    bool getSystemDirectory(const char **data) const;
    bool setPixelFormat(enum retro_pixel_format data);
    bool setInputDescriptors(const struct retro_input_descriptor *data);
    bool setKeyboardCallback(const struct retro_keyboard_callback *data);
    bool setDiskControlInterface(const struct retro_disk_control_callback *data);
    bool setHWRender(struct retro_hw_render_callback *data);
    bool getVariable(struct retro_variable *data);
    bool setVariables(const struct retro_variable *data);
    bool getVariableUpdate(bool *data);
    bool setSupportNoGame(bool data);
    bool getLibretroPath(const char **data) const;
    bool setFrameTimeCallback(const struct retro_frame_time_callback *data);
    bool setAudioCallback(const struct retro_audio_callback *data);
    bool getRumbleInterface(struct retro_rumble_interface *data) const;
    bool getInputDeviceCapabilities(uint64_t *data) const;
    bool getSensorInterface(struct retro_sensor_interface *data) const;
    bool getCameraInterface(struct retro_camera_callback *data) const;
    bool getLogInterface(struct retro_log_callback *data) const;
    bool getPerfInterface(struct retro_perf_callback *data) const;
    bool getLocationInterface(struct retro_location_callback *data) const;
    bool getCoreAssetsDirectory(const char **data) const;
    bool getSaveDirectory(const char **data) const;
    bool setSystemAVInfo(const struct retro_system_av_info *data);
    bool setProcAddressCallback(const struct retro_get_proc_address_interface *data);
    bool setSubsystemInfo(const struct retro_subsystem_info *data);
    bool setControllerInfo(const struct retro_controller_info *data);
    bool setMemoryMaps(const struct retro_memory_map *data);
    bool setGeometry(const struct retro_game_geometry *data);
    bool getUsername(const char **data) const;
    bool getLanguage(unsigned *data) const;
    bool getCurrentSoftwareFramebuffer(struct retro_framebuffer *data) const;
    bool getHWRenderInterface(const struct retro_hw_render_interface **data) const;
    bool setSupportAchievements(bool data);

    // Callbacks
    bool environmentCallback(unsigned cmd, void *data);
    void videoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
    size_t audioSampleBatchCallback(const int16_t *data, size_t frames);
    void audioSampleCallback(int16_t left, int16_t right);
    int16_t inputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);
    void inputPollCallback();
    uintptr_t getCurrentFramebuffer();
    retro_proc_address_t getProcAddress(const char *symbol);
    void logCallback(enum retro_log_level level, const char *fmt, va_list args);

    // Static callbacks that use s_instance to call into the core's implementation
    static bool staticEnvironmentCallback(unsigned cmd, void *data);
    static void staticVideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
    static size_t staticAudioSampleBatchCallback(const int16_t *data, size_t frames);
    static void staticAudioSampleCallback(int16_t left, int16_t right);
    static int16_t staticInputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);
    static void staticInputPollCallback();
    static uintptr_t staticGetCurrentFramebuffer();
    static retro_proc_address_t staticGetProcAddress(const char *symbol);
    static void staticLogCallback(enum retro_log_level level, const char *fmt, ...);

    // Input
    int16_t inputRead(unsigned port, unsigned device, unsigned index, unsigned id);

    int16_t ctrlState[MaxPorts][MaxDevices][MaxIndices][MaxIds];
    uint8_t keyState[MaxPorts][(RETROK_LAST + 7) / 8];

    // Audio
    void audioSetRate(double rate);
    void audioMix(const int16_t *samples, size_t frames);

    // Video
    void videoSetGeometry(unsigned width, unsigned height, float aspect, lrcpp::PixelFormat pixelFormat,
                          const lrcpp::HWRenderCallback* hwRenderCallback);
    void videoShowMessage(const char* msg, unsigned frames);
    bool videoSupportsContext(enum retro_hw_context_type type);
    void videoRefresh(const void* data, unsigned width, unsigned height, size_t pitch);
    uintptr_t videoGetCurrentFramebuffer();
    retro_proc_address_t videoGetProcAddress(const char *symbol);

    lrcpp::PixelFormat pixelFormat;
    uint8_t *scratchBuffer;
    unsigned bpp;
    love::StrongRef<love::graphics::Image> image;

    // Config
    const std::string &configGetSystemPath() const;
    const std::string &configGetCoreAssetsDirectory() const;
    const std::string &configGetSaveDirectory() const;
    const std::string &configGetVariable(const std::string &variable);
    void configSetVariables(const std::vector<lrcpp::Variable> &variables);
    bool configVarUpdated();

    // Log
    void logVprintf(enum retro_log_level level, const char* fmt, va_list args);

    bool preprocessMemoryDescriptors();

    CoreDll core;
    
    int16_t samples[8192];
    size_t samplesCount;

    std::string libretroPath;
    unsigned performanceLevel;
    bool supportsNoGame;
    bool supportAchievements;
    
    std::vector<lrcpp::InputDescriptor> inputDescriptors;
    std::vector<lrcpp::Variable> variables;
    
    lrcpp::HWRenderCallback hardwareRenderCallback;
    bool needsHardwareRender;
    
    lrcpp::SystemInfo systemInfo;
    lrcpp::SystemAVInfo systemAVInfo;
    
    std::vector<lrcpp::SubsystemInfo> subsystemInfo;
    
    std::vector<lrcpp::ControllerInfo> controllerInfo;

    bool diskControlInterfaceSet;
    lrcpp::DiskControlCallback diskControlInterface;

    lrcpp::MemoryMap memoryMap;
};

} // libretro
} // love

#endif // LOVE_LIBRETRO_LIBRETROCORE_H
