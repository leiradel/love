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

namespace love
{
namespace libretro
{

class Core : public Object
{
public:

	static love::Type type;

    Core(const std::string& corePath, const std::string &gamePath);
	virtual ~Core();

    love::StrongRef<love::graphics::Image> &getImage();
    float getAspectRatio() const;

    void step();

    unsigned getApiVersion();
    unsigned getRegion();
    void* getMemoryData(unsigned id);
    size_t getMemorySize(unsigned id);
    void resetGame();
    size_t serializeSize();
    bool serialize(void* data, size_t size);
    bool unserialize(const void* data, size_t size);
    
    unsigned getPerformanceLevel();
    enum retro_pixel_format getPixelFormat();
    bool getNeedsHardwareRender();
    bool getSupportsNoGame();
    bool getSupportAchievements();
    void unloadGame();

    unsigned getNumDiscs();
    unsigned getCurrentDiscIndex();
    void setCurrentDiscIndex(unsigned);
    bool getTrayOpen();
    void setTrayOpen(bool open);

    const struct retro_input_descriptor *getInputDescriptors(unsigned *count) const;
    const struct retro_variable *getVariables(unsigned *count) const;
    const struct retro_subsystem_info *getSubsystemInfo(unsigned *count) const;
    const struct retro_controller_info *getControllerInfo(unsigned *count) const;
    const struct retro_hw_render_callback *getHardwareRenderCallback() const;
    const struct retro_system_info *getSystemInfo() const;
    const struct retro_system_av_info *getSystemAVInfo() const;
    const struct retro_memory_map *getMemoryMap() const;
    
protected:
    struct InputDescriptor
    {
        unsigned port;
        unsigned device;
        unsigned index;
        unsigned id;
        std::string description;
    };

    struct Variable
    {
        std::string key;
        std::string value;
    };

    typedef struct retro_hw_render_callback HWRenderCallback;

    struct SystemInfo
    {
        std::string libraryName;
        std::string libraryVersion;
        std::string validExtensions;
        bool needFullPath;
        bool blockExtract;
    };

    typedef struct retro_system_av_info SystemAVInfo;

    struct SubsystemMemoryInfo
    {
        std::string extension;
        unsigned type;
    };

    struct SubsystemRomInfo
    {
        std::string desc;
        std::string validExtensions;
        bool needFullpath;
        bool blockExtract;
        bool required;
        std::vector<SubsystemMemoryInfo> memory;
    };

    struct SubsystemInfo
    {
        std::string desc;
        std::string ident;
        std::vector<SubsystemRomInfo> roms;
        unsigned id;
    };

    struct ControllerDescription
    {
        std::string desc;
        unsigned id;
    };

    struct ControllerInfo
    {
        std::vector<ControllerDescription> types;
    };

    typedef struct retro_disk_control_callback DiskControlCallback;

    struct MemoryDescriptor
    {
        uint64_t flags;
        void *ptr;
        size_t offset;
        size_t start;
        size_t select;
        size_t disconnect;
        size_t len;
        std::string addrspace;
    };

    struct MemoryMap
    {
        std::vector<MemoryDescriptor> descriptors;
    };

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
    bool inputCtrlUpdated();
    unsigned inputGetController(unsigned port);
    void inputSetInputDescriptors(const std::vector<InputDescriptor> &descs);
    int16_t inputRead(unsigned port, unsigned device, unsigned index, unsigned id);
    void inputPoll();

    // Audio
    void audioSetRate(double rate);
    void audioMix(const int16_t *samples, size_t frames);

    // Video
    void videoSetGeometry(unsigned width, unsigned height, float aspect, enum retro_pixel_format pixelFormat,
                          const HWRenderCallback* hwRenderCallback);
    void videoShowMessage(const char* msg, unsigned frames);
    bool videoSupportsContext(enum retro_hw_context_type type);
    void videoRefresh(const void* data, unsigned width, unsigned height, size_t pitch);
    uintptr_t videoGetCurrentFramebuffer();
    retro_proc_address_t videoGetProcAddress(const char *symbol);

    enum retro_pixel_format pixelFormat;
    uint8_t *scratchBuffer;
    unsigned bpp;
    love::StrongRef<love::graphics::Image> image;

    // Config
    const std::string &configGetSystemPath() const;
    const std::string &configGetCoreAssetsDirectory() const;
    const std::string &configGetSaveDirectory() const;
    const std::string &configGetVariable(const std::string &variable);
    void configSetVariables(const std::vector<Variable> &variables);
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
    
    std::vector<InputDescriptor> inputDescriptors;
    std::vector<Variable> variables;
    
    HWRenderCallback hardwareRenderCallback;
    bool needsHardwareRender;
    
    SystemInfo systemInfo;
    SystemAVInfo systemAVInfo;
    
    std::vector<SubsystemInfo> subsystemInfo;
    
    std::vector<ControllerInfo> controllerInfo;
    std::vector<unsigned> ports;

    bool diskControlInterfaceSet;
    DiskControlCallback diskControlInterface;

    MemoryMap memoryMap;
};

} // libretro
} // love

#endif // LOVE_LIBRETRO_LIBRETROCORE_H
