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

#ifndef LOVE_LIBRETRO_CORE_H
#define LOVE_LIBRETRO_CORE_H

// STL
#include <vector>

// LOVE
#include "common/Module.h"
#include "graphics/Image.h"
#include "audio/Audio.h"

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

#define MKINPUT1(dev) (static_cast<uint32_t>(lrcpp::Device::dev) << 16)
#define MKINPUT2(dev, id) (MKINPUT1(dev) | static_cast<uint32_t>(lrcpp::DeviceId::id))
#define MKINPUT3(dev, idx, id) (MKINPUT2(dev, id) | static_cast<uint32_t>(lrcpp::DeviceIndex::idx) << 8)

    enum class Input
    {
        NONE                  = MKINPUT1(None),
        JOYPAD_B              = MKINPUT2(Joypad, JoypadB),
        JOYPAD_Y              = MKINPUT2(Joypad, JoypadY),
        JOYPAD_SELECT         = MKINPUT2(Joypad, JoypadSelect),
        JOYPAD_START          = MKINPUT2(Joypad, JoypadStart),
        JOYPAD_UP             = MKINPUT2(Joypad, JoypadUp),
        JOYPAD_DOWN           = MKINPUT2(Joypad, JoypadDown),
        JOYPAD_LEFT           = MKINPUT2(Joypad, JoypadLeft),
        JOYPAD_RIGHT          = MKINPUT2(Joypad, JoypadRight),
        JOYPAD_A              = MKINPUT2(Joypad, JoypadA),
        JOYPAD_X              = MKINPUT2(Joypad, JoypadX),
        JOYPAD_L              = MKINPUT2(Joypad, JoypadL),
        JOYPAD_R              = MKINPUT2(Joypad, JoypadR),
        JOYPAD_L2             = MKINPUT2(Joypad, JoypadL2),
        JOYPAD_R2             = MKINPUT2(Joypad, JoypadR2),
        JOYPAD_L3             = MKINPUT2(Joypad, JoypadL3),
        JOYPAD_R3             = MKINPUT2(Joypad, JoypadR3),
        ANALOG_LEFT_X         = MKINPUT3(Analog, AnalogLeft, AnalogX),
        ANALOG_LEFT_Y         = MKINPUT3(Analog, AnalogLeft, AnalogY),
        ANALOG_RIGHT_X        = MKINPUT3(Analog, AnalogRight, AnalogX),
        ANALOG_RIGHT_Y        = MKINPUT3(Analog, AnalogRight, AnalogY),
        ANALOG_B              = MKINPUT3(Analog, AnalogButton, JoypadB),
        ANALOG_Y              = MKINPUT3(Analog, AnalogButton, JoypadY),
        ANALOG_SELECT         = MKINPUT3(Analog, AnalogButton, JoypadSelect),
        ANALOG_START          = MKINPUT3(Analog, AnalogButton, JoypadStart),
        ANALOG_UP             = MKINPUT3(Analog, AnalogButton, JoypadUp),
        ANALOG_DOWN           = MKINPUT3(Analog, AnalogButton, JoypadDown),
        ANALOG_LEFT           = MKINPUT3(Analog, AnalogButton, JoypadLeft),
        ANALOG_RIGHT          = MKINPUT3(Analog, AnalogButton, JoypadRight),
        ANALOG_A              = MKINPUT3(Analog, AnalogButton, JoypadA),
        ANALOG_X              = MKINPUT3(Analog, AnalogButton, JoypadX),
        ANALOG_L              = MKINPUT3(Analog, AnalogButton, JoypadL),
        ANALOG_R              = MKINPUT3(Analog, AnalogButton, JoypadR),
        ANALOG_L2             = MKINPUT3(Analog, AnalogButton, JoypadL2),
        ANALOG_R2             = MKINPUT3(Analog, AnalogButton, JoypadR2),
        ANALOG_L3             = MKINPUT3(Analog, AnalogButton, JoypadL3),
        ANALOG_R3             = MKINPUT3(Analog, AnalogButton, JoypadR3),
        MOUSE_X               = MKINPUT2(Mouse, MouseX),
        MOUSE_Y               = MKINPUT2(Mouse, MouseY),
        MOUSE_LEFT            = MKINPUT2(Mouse, MouseLeft),
        MOUSE_RIGHT           = MKINPUT2(Mouse, MouseRight),
        MOUSE_WHEELUP         = MKINPUT2(Mouse, MouseWheelup),
        MOUSE_WHEELDOWN       = MKINPUT2(Mouse, MouseWheeldown),
        MOUSE_MIDDLE          = MKINPUT2(Mouse, MouseMiddle),
        MOUSE_HORIZ_WHEELUP   = MKINPUT2(Mouse, MouseHorizWheelup),
        MOUSE_HORIZ_WHEELDOWN = MKINPUT2(Mouse, MouseHorizWheeldown),
        MOUSE_BUTTON_4        = MKINPUT2(Mouse, MouseButton4),
        MOUSE_BUTTON_5        = MKINPUT2(Mouse, MouseButton5),
        KEYBOARD              = MKINPUT1(Keyboard),
        LIGHTGUN_SCREEN_X     = MKINPUT2(Lightgun, LightgunScreenX),
        LIGHTGUN_SCREEN_Y     = MKINPUT2(Lightgun, LightgunScreenY),
        LIGHTGUN_IS_OFFSCREEN = MKINPUT2(Lightgun, LightgunIsOffscreen),
        LIGHTGUN_TRIGGER      = MKINPUT2(Lightgun, LightgunTrigger),
        LIGHTGUN_RELOAD       = MKINPUT2(Lightgun, LightgunReload),
        LIGHTGUN_AUX_A        = MKINPUT2(Lightgun, LightgunAuxA),
        LIGHTGUN_AUX_B        = MKINPUT2(Lightgun, LightgunAuxB),
        LIGHTGUN_START        = MKINPUT2(Lightgun, LightgunStart),
        LIGHTGUN_SELECT       = MKINPUT2(Lightgun, LightgunSelect),
        LIGHTGUN_AUX_C        = MKINPUT2(Lightgun, LightgunAuxC),
        LIGHTGUN_DPAD_UP      = MKINPUT2(Lightgun, LightgunDpadUp),
        LIGHTGUN_DPAD_DOWN    = MKINPUT2(Lightgun, LightgunDpadDown),
        LIGHTGUN_DPAD_LEFT    = MKINPUT2(Lightgun, LightgunDpadLeft),
        LIGHTGUN_DPAD_RIGHT   = MKINPUT2(Lightgun, LightgunDpadRight),
        POINTER_X             = MKINPUT2(Pointer, PointerX),
        POINTER_Y             = MKINPUT2(Pointer, PointerY),
        POINTER_PRESSED       = MKINPUT2(Pointer, PointerPressed),
    };

#undef MKINPUT1
#undef MKINPUT2
#undef MKINPUT3

    Core(const std::string& corePath, const std::string &gamePath);
	virtual ~Core();

    love::StrongRef<love::graphics::Image> &getImage();
    float getAspectRatio() const;

    void setControllerPortDevice(unsigned port, unsigned device);
    bool setInput(unsigned port, Input input, int16_t value);
    bool setInput(unsigned port, Input input, unsigned index, int16_t value);
    bool setKey(unsigned port, Input input, unsigned key, bool pressed);

    void step();
    
    static lrcpp::Device getDevice(Input input)
    {
        return static_cast<lrcpp::Device>((static_cast<uint32_t>(input) >> 16) & 0xff);
    }

    static lrcpp::DeviceIndex getDeviceIndex(Input input)
    {
        return static_cast<lrcpp::DeviceIndex>((static_cast<uint32_t>(input) >> 8) & 0xff);
    }

    static lrcpp::DeviceId getDeviceId(Input input)
    {
        return static_cast<lrcpp::DeviceId>(static_cast<uint32_t>(input) & 0xff);
    }

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

    int16_t samples[16384];
    size_t samplesCount;
    love::audio::Source* source;

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

#endif // LOVE_LIBRETRO_CORE_H
