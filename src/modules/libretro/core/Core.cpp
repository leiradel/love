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

// C
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

// LOVE
#include "Core.h"

#include "graphics/Graphics.h"

namespace love
{
namespace libretro
{

/**
 * Unavoidable because the libretro API don't have an userdata pointer to
 * allow us pass and receive back the core instance :/
 */
static thread_local Core *s_instance;

// Helper class to set and unset the s_instance thread local.
class InstanceSetter
{
public:
    InstanceSetter(Core *instance)
    {
        previous = s_instance;
        s_instance = instance;
    }
    
    ~InstanceSetter()
    {
        s_instance = previous;
    }

protected:
    Core *previous;
};

love::Type Core::type("Core", &Object::type);

Core::Core(const std::string& corePath, const std::string &gamePath)
    : core(corePath)
{
    InstanceSetter instance_setter(this);

    memset(ctrlState, 0, sizeof(ctrlState));
    memset(keyState, 0, sizeof(keyState));

    memset(samples, 0, sizeof(samples));
    samplesCount = 0;
    source = nullptr;

    pixelFormat = lrcpp::PixelFormat::Unknown;
    scratchBuffer = nullptr;
    bpp = 0;
    image = nullptr;

    variablesUpdated = false;

    libretroPath = {};
    performanceLevel = 0;
    supportsNoGame = false;
    supportAchievements = false;
    inputDescriptors = {};
    variables = {};
    hardwareRenderCallback = {};
    needsHardwareRender = false;
    systemInfo = {};
    systemAVInfo = {};
    subsystemInfo = {};
    controllerInfo = {};
    diskControlInterfaceSet = false;
    diskControlInterface = {};
    memoryMap = {};

    struct retro_system_info system_info;
    core.getSystemInfo(&system_info);
    systemInfo = system_info;

    core.setEnvironment(staticEnvironmentCallback);
    core.init();

    try
    {
        libretroPath = corePath;

        if (!supportsNoGame && gamePath.empty())
            throw love::Exception("Can't load this core without game data");

        retro_game_info game;
        game.path = gamePath.c_str();
        game.data = nullptr;
        game.size = 0;
        game.meta = nullptr;

        if (!gamePath.empty() && !systemInfo.needFullPath)
        {
            struct stat stbuf;

            if (stat(gamePath.c_str(), &stbuf) != 0)
                throw love::Exception("Error stat'ing \"%s\": %s", gamePath.c_str(), strerror(errno));

            void* data = malloc(stbuf.st_size);
            game.data = data;
            game.size = stbuf.st_size;

            if (data == nullptr)
                throw std::bad_alloc();

            FILE *file = fopen(gamePath.c_str(), "rb");

            if (file == nullptr)
            {
                free(data);
                throw love::Exception("Error opening \"%s\" for reading: %s", gamePath.c_str(), strerror(errno));
            }

            const size_t numRead = fread(data, 1, stbuf.st_size, file);

            if (numRead < stbuf.st_size)
            {
                free(data);

                if (ferror(file))
                    throw love::Exception("Error reading from \"%s\": %s", gamePath.c_str(), strerror(errno));
                
                throw love::Exception("Short read count, %zu expected, %zu read", stbuf.st_size, numRead);
            }

            fclose(file);
        }

        if (!core.loadGame(&game))
        {
            throw love::Exception("Error loading game \"%s\"", gamePath.c_str());
        }

        try
        {
            if (needsHardwareRender)
            {
                hardwareRenderCallback.contextReset();
            }

            core.setVideoRefresh(staticVideoRefreshCallback);
            core.setAudioSampleBatch(staticAudioSampleBatchCallback);
            core.setAudioSample(staticAudioSampleCallback);
            core.setInputState(staticInputStateCallback);
            core.setInputPoll(staticInputPollCallback);

            retro_system_av_info system_av_info;
            core.getSystemAVInfo(&system_av_info);
            systemAVInfo = system_av_info;

            if (systemAVInfo.geometry.aspectRatio <= 0.0f)
                systemAVInfo.geometry.aspectRatio = (float)systemAVInfo.geometry.baseWidth /
                                                    (float)systemAVInfo.geometry.baseHeight;

            const lrcpp::HWRenderCallback *cb = needsHardwareRender ? &hardwareRenderCallback : NULL;

            videoSetGeometry(systemAVInfo.geometry.baseWidth,
                             systemAVInfo.geometry.baseHeight,
                             systemAVInfo.geometry.aspectRatio,
                             pixelFormat,
                             cb);

            audioSetRate(systemAVInfo.timing.sampleRate);

            for (size_t i = 0; i < controllerInfo.size(); i++)
                core.setControllerPortDevice(i, RETRO_DEVICE_NONE);
        }
        catch (std::exception &e)
        {
            core.unloadGame();
            throw e;
        }
    }
    catch (std::exception &e)
    {
        core.deinit();
        throw e;
    }
}

Core::~Core()
{
    InstanceSetter instance_setter(this);
    core.unloadGame();
    core.deinit();
    delete[] scratchBuffer;
    delete source;
}

love::StrongRef<love::graphics::Image> &Core::getImage()
{
    return image;
}

void Core::setControllerPortDevice(unsigned port, lrcpp::Device device)
{
    core.setControllerPortDevice(port, static_cast<unsigned>(device));
}

void Core::setControllerPortDevice(unsigned port, unsigned device)
{
    core.setControllerPortDevice(port, device);
}

bool Core::setInput(unsigned port, Input input, int16_t value)
{
    if (port < MaxPorts)
    {
        lrcpp::Device device = getDevice(input);
        unsigned udev = static_cast<unsigned>(device);

        if (udev < MaxDevices && device != lrcpp::Device::Pointer)
        {
            lrcpp::DeviceIndex index = getDeviceIndex(input);
            unsigned uidx = static_cast<unsigned>(index);

            if (uidx < MaxIndices)
            {
                lrcpp::DeviceId id = getDeviceId(input);
                unsigned uid = static_cast<unsigned>(id);

                if (uid < MaxIds)
                {
                    ctrlState[port][udev][uidx][uid] = value;
                    return true;
                }
            }
        }
    }

    return false;
}

bool Core::setInput(unsigned port, Input input, unsigned index, int16_t value)
{
    if (port < MaxPorts && index < MaxIndices)
    {
        lrcpp::Device device = getDevice(input);
        unsigned udev = static_cast<unsigned>(device);

        if (udev < MaxDevices && device == lrcpp::Device::Pointer)
        {
            lrcpp::DeviceId id = getDeviceId(input);
            unsigned uid = static_cast<unsigned>(id);

            if (uid < MaxIds)
            {
                ctrlState[port][udev][index][uid] = value;
                return true;
            }
        }
    }

    return false;
}

bool Core::setKey(unsigned port, Input input, lrcpp::Key key, bool pressed)
{
    unsigned ukey = static_cast<unsigned>(key);

    if (port < MaxPorts && ukey < RETROK_LAST)
    {
        lrcpp::Device device = getDevice(input);
        unsigned udev = static_cast<unsigned>(device);

        if (udev < MaxDevices && device == lrcpp::Device::Keyboard)
        {
            unsigned index = ukey >> 3;
            unsigned bit = 1 << (ukey & 7);

            if (pressed)
                keyState[port][index] |= bit;
            else
                keyState[port][index] &= ~bit;
            
            return true;
        }
    }

    return false;
}

bool Core::setVariable(const lrcpp::Variable &var)
{
    const size_t count = variables.size();

    for (size_t i = 0; i < count; i++)
    {
        if (variables[i].key == var.key)
        {
            const size_t option = variables[i].value.find(var.value);

            if (option != std::string::npos)
            {
                const size_t end = option + var.value.length();

                if (end >= variables[i].value.length() || variables[i].value[end] != '|')
                    return false;
                else
                {
                    values[i] = var.value;
                    variablesUpdated = true;
                    return true;
                }
            }
        }
    }

    return false;
}

void Core::step()
{
    InstanceSetter instance_setter(this);

    samplesCount = 0;
    core.run();

    if (samplesCount > 0)
        audioMix(samples, samplesCount / 2);
}

bool Core::setRotation(unsigned data)
{
    // TODO implement me!
}

bool Core::getOverscan(bool *data) const
{
    *data = false;
    return true;
}

bool Core::getCanDupe(bool *data) const
{
    *data = true;
    return true;
}

bool Core::setMessage(const struct retro_message *data)
{
    videoShowMessage(data->msg, data->frames);
    return true;
}

bool Core::shutdown()
{
    return false;
}

bool Core::setPerformanceLevel(unsigned data)
{
    performanceLevel = data;
    return true;
}

bool Core::getSystemDirectory(const char **data) const
{
    *data = configGetSystemPath().c_str();
    return true;
}

bool Core::setPixelFormat(enum retro_pixel_format data)
{
    switch (data)
    {
        case RETRO_PIXEL_FORMAT_0RGB1555:
            pixelFormat = lrcpp::PixelFormat::XRGB1555;
            return true;

        case RETRO_PIXEL_FORMAT_XRGB8888:
            pixelFormat = lrcpp::PixelFormat::XRGB8888;
            return true;

        case RETRO_PIXEL_FORMAT_RGB565:
            pixelFormat = lrcpp::PixelFormat::RGB565;
            return true;

        case RETRO_PIXEL_FORMAT_UNKNOWN:
        default:
            pixelFormat = lrcpp::PixelFormat::RGB565;
            return false;
    }
}

bool Core::setInputDescriptors(const struct retro_input_descriptor *data)
{
    const struct retro_input_descriptor *desc;

    for (desc = data; desc->description != NULL; desc++) /* Just count */;

    const size_t count = desc - data;
    inputDescriptors.resize(count);

    for (size_t i = 0; i < count; i++, data++)
    {
        inputDescriptors[i] = *data;
    }
    
    return true;
}

bool Core::setKeyboardCallback(const struct retro_keyboard_callback *data)
{
    (void)data;
    return true;
}

bool Core::setDiskControlInterface(const struct retro_disk_control_callback *data)
{
  diskControlInterface = *data;
  return true;
}

bool Core::setHWRender(struct retro_hw_render_callback *data)
{
    if (!videoSupportsContext(data->context_type))
        return false;

    data->get_current_framebuffer = staticGetCurrentFramebuffer;
    data->get_proc_address = staticGetProcAddress;

    hardwareRenderCallback = *data;
    needsHardwareRender = true;
    return true;
}

bool Core::getVariable(struct retro_variable *data)
{
    const size_t count = variables.size();

    for (size_t i = 0; i < count; i++)
    {
        if (variables[i].key == data->key)
        {
            data->value = values[i].c_str();
            return true;
        }
    }

    return false;
}

bool Core::setVariables(const struct retro_variable *data)
{
    const struct retro_variable *var;

    for (var = data; var->key != NULL; var++) /* Just count */;

    const size_t count = var - data;
    variables.resize(count);
    values.resize(count);

    for (size_t i = 0; i < count; i++, data++)
    {
        variables[i] = *data;

        const char *option = strchr(data->value, ';');

        if (option != nullptr)
        {
            while (isspace(*++option)) /* Just skip */;
            const char *pipe = strchr(option, '|');

            if (pipe != nullptr)
                values[i] = std::string(option, pipe - option);
            else
                values[i] = option;
        }
    }

    return true;
}

bool Core::getVariableUpdate(bool *data)
{
    *data = variablesUpdated;
    variablesUpdated = false;
    return true;
}

bool Core::setSupportNoGame(bool data)
{
    supportsNoGame = data;
    return true;
}

bool Core::getLibretroPath(const char **data) const
{
    *data = libretroPath.c_str();
    return true;
}

bool Core::setFrameTimeCallback(const struct retro_frame_time_callback *data)
{
    // TODO implement me!
    return false;
}

bool Core::setAudioCallback(const struct retro_audio_callback *data)
{
    // TODO implement me!
    return false;
}

bool Core::getRumbleInterface(struct retro_rumble_interface *data) const
{
    // TODO implement me!
    return false;
}

bool Core::getInputDeviceCapabilities(uint64_t *data) const
{
    *data = 1 << RETRO_DEVICE_JOYPAD; // TODO support more devices
    return false;
}

bool Core::getSensorInterface(struct retro_sensor_interface *data) const
{
    // TODO implement me!
    return false;
}

bool Core::getCameraInterface(struct retro_camera_callback *data) const
{
    // TODO implement me!
    return false;
}

bool Core::getLogInterface(struct retro_log_callback *data) const
{
  data->log = staticLogCallback;
  return true;
}

bool Core::getPerfInterface(struct retro_perf_callback *data) const
{
    // TODO implement me!
    return false;
}

bool Core::getLocationInterface(struct retro_location_callback *data) const
{
    // TODO implement me!
    return false;
}

bool Core::getCoreAssetsDirectory(const char **data) const
{
    *data = configGetCoreAssetsDirectory().c_str();
    return true;
}

bool Core::getSaveDirectory(const char** data) const
{
    *data = configGetSaveDirectory().c_str();
    return true;
}

bool Core::setSystemAVInfo(const struct retro_system_av_info *data)
{
    systemAVInfo = *data;

    if (systemAVInfo.geometry.aspectRatio <= 0.0f)
        systemAVInfo.geometry.aspectRatio = (float)systemAVInfo.geometry.baseWidth /
                                            (float)systemAVInfo.geometry.baseHeight;

    const lrcpp::HWRenderCallback *cb = needsHardwareRender ? &hardwareRenderCallback : NULL;

    try
    {
        videoSetGeometry(systemAVInfo.geometry.baseWidth,
                         systemAVInfo.geometry.baseHeight,
                         systemAVInfo.geometry.aspectRatio,
                         pixelFormat,
                         cb);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool Core::setProcAddressCallback(const struct retro_get_proc_address_interface *data)
{
    // TODO implement me!
    return false;
}

bool Core::setSubsystemInfo(const struct retro_subsystem_info *data)
{
    const struct retro_subsystem_info *info;

    for (info = data; info->desc != NULL; info++) /* Just count */;

    const size_t count = info - data;
    subsystemInfo.resize(count);

    for (size_t i = 0; i < count; i++, data++)
        subsystemInfo[i] = *data;

    return true;
}

bool Core::setControllerInfo(const struct retro_controller_info *data)
{
    const struct retro_controller_info *info;

    for (info = data; info->types != NULL; info++) /* Just count */;

    const size_t count = info - data;
    controllerInfo.resize(count);

    for (size_t i = 0; i < count; i++, data++)
        controllerInfo[i] = *data;

    return true;
}

bool Core::setMemoryMaps(const struct retro_memory_map *data)
{
    memoryMap = *data;
    preprocessMemoryDescriptors();

    return true;
}

bool Core::setGeometry(const struct retro_game_geometry *data)
{
    systemAVInfo.geometry = *data;

    if (systemAVInfo.geometry.aspectRatio <= 0.0f)
        systemAVInfo.geometry.aspectRatio = (float)systemAVInfo.geometry.baseWidth /
                                            (float)systemAVInfo.geometry.baseHeight;

    const lrcpp::HWRenderCallback *cb = needsHardwareRender ? &hardwareRenderCallback : NULL;

    try
    {
        videoSetGeometry(systemAVInfo.geometry.baseWidth,
                         systemAVInfo.geometry.baseHeight,
                         systemAVInfo.geometry.aspectRatio,
                         pixelFormat,
                         cb);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool Core::getUsername(const char **data) const
{
    // TODO implement me!
    return false;
}

bool Core::getLanguage(unsigned *data) const
{
    // TODO implement me!
    *data = RETRO_LANGUAGE_ENGLISH;
    return true;
}

bool Core::getCurrentSoftwareFramebuffer(struct retro_framebuffer *data) const
{
    // TODO implement me!
    return false;
}

bool Core::getHWRenderInterface(const struct retro_hw_render_interface **data) const
{
    // TODO implement me!
    return false;
}

bool Core::setSupportAchievements(bool data)
{
    supportAchievements = data;
    return true;
}

bool Core::environmentCallback(unsigned cmd, void *data)
{
    bool ret;

    switch (cmd)
    {
        case RETRO_ENVIRONMENT_SET_ROTATION:
            return setRotation(*(const unsigned*)data);

        case RETRO_ENVIRONMENT_GET_OVERSCAN:
            return getOverscan((bool*)data);

        case RETRO_ENVIRONMENT_GET_CAN_DUPE:
            return getCanDupe((bool*)data);

        case RETRO_ENVIRONMENT_SET_MESSAGE:
            return setMessage((const struct retro_message*)data);

        case RETRO_ENVIRONMENT_SHUTDOWN:
            return shutdown();

        case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
            return setPerformanceLevel(*(unsigned*)data);

        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
            return getSystemDirectory((const char**)data);

        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
            return setPixelFormat(*(enum retro_pixel_format*)data);

        case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
            return setInputDescriptors((const struct retro_input_descriptor*)data);

        case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
            return setKeyboardCallback((const struct retro_keyboard_callback*)data);

        case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
            return setDiskControlInterface((const struct retro_disk_control_callback*)data);

        case RETRO_ENVIRONMENT_SET_HW_RENDER:
            return setHWRender((struct retro_hw_render_callback*)data);

        case RETRO_ENVIRONMENT_GET_VARIABLE:
            return getVariable((struct retro_variable*)data);

        case RETRO_ENVIRONMENT_SET_VARIABLES:
            return setVariables((const struct retro_variable*)data);

        case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
            return getVariableUpdate((bool*)data);

        case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
            return setSupportNoGame(*(bool*)data);

        case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
            return getLibretroPath((const char**)data);

        case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
            return setFrameTimeCallback((const struct retro_frame_time_callback*)data);

        case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
            return setAudioCallback((const struct retro_audio_callback*)data);

        case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
            return getRumbleInterface((struct retro_rumble_interface*)data);

        case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
            return getInputDeviceCapabilities((uint64_t*)data);

        case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
            return getSensorInterface((struct retro_sensor_interface*)data);

        case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE:
            return getCameraInterface((struct retro_camera_callback*)data);

        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
            return getLogInterface((struct retro_log_callback*)data);

        case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
            return getPerfInterface((struct retro_perf_callback*)data);

        case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
            return getLocationInterface((struct retro_location_callback*)data);

        case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
            return getCoreAssetsDirectory((const char**)data);

        case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
            return getSaveDirectory((const char**)data);

        case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
            return setSystemAVInfo((const struct retro_system_av_info*)data);

        case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK:
            return setProcAddressCallback((const struct retro_get_proc_address_interface*)data);

        case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO:
            return setSubsystemInfo((const struct retro_subsystem_info*)data);

        case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
            return setControllerInfo((const struct retro_controller_info*)data);

        case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
            return setMemoryMaps((const struct retro_memory_map*)data);

        case RETRO_ENVIRONMENT_SET_GEOMETRY:
            return setGeometry((const struct retro_game_geometry*)data);

        case RETRO_ENVIRONMENT_GET_USERNAME:
            return getUsername((const char**)data);

        case RETRO_ENVIRONMENT_GET_LANGUAGE:
            return getLanguage((unsigned*)data);

        case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
            return getCurrentSoftwareFramebuffer((struct retro_framebuffer*)data);

        case RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE:
            return getHWRenderInterface((const struct retro_hw_render_interface**)data);

        case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
            return setSupportAchievements(*(bool*)data);

        default:
            return false;
    }
}

void Core::videoRefreshCallback(const void* data, unsigned width, unsigned height, size_t pitch)
{
    videoRefresh(data, width, height, pitch);
}

size_t Core::audioSampleBatchCallback(const int16_t *data, size_t frames)
{
    if (samplesCount + frames * 2 < sizeof(samples) / sizeof(samples[0]))
    {
        memcpy(samples + samplesCount, data, frames * 4);
        samplesCount += frames * 2;
    }

    return frames;
}

void Core::audioSampleCallback(int16_t left, int16_t right)
{
    if (samplesCount + 2 < sizeof(samples) / sizeof(samples[0]))
    {
        samples[samplesCount++] = left;
        samples[samplesCount++] = right;
    }
}

int16_t Core::inputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    device &= RETRO_DEVICE_MASK;

    if (port < MaxPorts && device < MaxDevices && index < MaxIndices)
        if (static_cast<lrcpp::Device>(device) != lrcpp::Device::Keyboard)
        {
            if (id < MaxIndices)
                return ctrlState[port][device][index][id];
        }
        else
        {
            if (id < RETROK_LAST)
                return (keyState[port][id >> 3] & (1 << (id & 7))) != 0 ? 32767 : 0;
        }

    return 0;
}

void Core::inputPollCallback()
{
    // Nothing to do
}

uintptr_t Core::getCurrentFramebuffer()
{
    return videoGetCurrentFramebuffer();
}

retro_proc_address_t Core::getProcAddress(const char* symbol)
{
    return videoGetProcAddress(symbol);
}

void Core::logCallback(enum retro_log_level level, const char *fmt, va_list args)
{
    logVprintf(level, fmt, args);
}

bool Core::staticEnvironmentCallback(unsigned cmd, void* data)
{
    return s_instance->environmentCallback(cmd, data);
}

void Core::staticVideoRefreshCallback(const void* data, unsigned width, unsigned height, size_t pitch)
{
    s_instance->videoRefreshCallback(data, width, height, pitch);
}

size_t Core::staticAudioSampleBatchCallback(const int16_t* data, size_t frames)
{
    return s_instance->audioSampleBatchCallback(data, frames);
}

void Core::staticAudioSampleCallback(int16_t left, int16_t right)
{
    s_instance->audioSampleCallback(left, right);
}

int16_t Core::staticInputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    return s_instance->inputStateCallback(port, device, index, id);
}

void Core::staticInputPollCallback()
{
    s_instance->inputPollCallback();
}

uintptr_t Core::staticGetCurrentFramebuffer()
{
    return s_instance->getCurrentFramebuffer();
}

retro_proc_address_t Core::staticGetProcAddress(const char* symbol)
{
    return s_instance->getProcAddress(symbol);
}

void Core::staticLogCallback(enum retro_log_level level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    s_instance->logVprintf(level, fmt, args);
    va_end(args);
}

void Core::audioSetRate(double rate)
{
    delete source;

    auto audio = Module::getInstance<love::audio::Audio>(Module::M_AUDIO);
    source = audio->newSource(rate, 16, 2, 8);
}

void Core::audioMix(const int16_t *samples, size_t frames)
{
    if (source->getFreeBufferCount() != 0)
    {
        source->queue((void*)samples, frames * 4, systemAVInfo.timing.sampleRate, 16, 2);

        if (!source->isPlaying())
            source->play();
    }
}

void Core::videoSetGeometry(unsigned width, unsigned height, float aspect, lrcpp::PixelFormat pixelFormat,
                            const lrcpp::HWRenderCallback* hwRenderCallback)
{
    (void)width;
    (void)height;
    (void)aspect;
    (void)hwRenderCallback;

    if (this->pixelFormat != pixelFormat)
    {
        // Force the reconstruction of the image in the next refresh
        image = nullptr;
        free(scratchBuffer);
        this->pixelFormat = pixelFormat;
    }
}

void Core::videoShowMessage(const char* msg, unsigned frames)
{
    // TODO implement me!
}

bool Core::videoSupportsContext(enum retro_hw_context_type type)
{
    // TODO implement me!
    return false;
}

void Core::videoRefresh(const void* data, unsigned width, unsigned height, size_t pitch)
{
    // Check for frame duplication.
    if (data == nullptr)
        return;

    if (!image || image->getWidth() != width || image->getHeight() != height)
    {
        love::PixelFormat format;

        switch (pixelFormat)
        {
            case lrcpp::PixelFormat::XRGB8888:
                format = PIXELFORMAT_RGBA8;
                bpp = 4;
                break;

            case lrcpp::PixelFormat::RGB565:
                format = PIXELFORMAT_RGB565;
                bpp = 2;
                break;

            default: // fallthrough
            case lrcpp::PixelFormat::XRGB1555:
                format = PIXELFORMAT_RGB5A1;
                bpp = 2;
                break;
        }

        auto graphics = Module::getInstance<love::graphics::Graphics>(Module::M_GRAPHICS);

        love::graphics::Image::Settings settings = {};
        auto newImage = graphics->newImage(love::graphics::TEXTURE_2D, format, width, height, 1, settings);

        image.set(newImage, love::Acquire::RETAIN);

        delete[] scratchBuffer;
        scratchBuffer = new uint8_t[width * height * bpp];
    }

    const uint8_t *source = static_cast<const uint8_t*>(data);
    uint8_t *dest = scratchBuffer;

    if (pixelFormat != lrcpp::PixelFormat::XRGB8888)
    {
        for (unsigned y = 0; y < height; y++)
        {
            memcpy(dest, source, width * bpp);

            source += pitch;
            dest += width * bpp;
        }
    }
    else
    {
        for (unsigned y = 0; y < height; y++)
        {
            for (unsigned x = 0; x < width; x++)
            {
                // xRGB
                uint32_t pixel = ((uint32_t*)source)[x];

                // ABGR
                ((uint32_t*)dest)[x] = UINT32_C(0xff000000)
                                     | (pixel & 0x00ff0000) >> 16
                                     | (pixel & 0x0000ff00)
                                     | (pixel & 0x000000ff) << 16;
            }

            source += pitch;
            dest += width * bpp;
        }
    }

    love::Rect rect = {0, 0, (int)width, (int)height};
    image->replacePixels(scratchBuffer, width * height * bpp, 0, 0, rect, true);
}

uintptr_t Core::videoGetCurrentFramebuffer()
{
    // TODO implement me!
    return 0;
}

retro_proc_address_t Core::videoGetProcAddress(const char *symbol)
{
    // TODO implement me!
    return nullptr;
}

const std::string &Core::configGetSystemPath() const
{
    // TODO implement me!
    static const std::string path = "~/home/.love/libretro/system";
    return path;
}

const std::string &Core::configGetCoreAssetsDirectory() const
{
    // TODO implement me!
    static const std::string path = "~/home/.love/libretro/assets";
    return path;
}

const std::string &Core::configGetSaveDirectory() const
{
    // TODO implement me!
    static const std::string path = "~/home/.love/libretro/saves";
    return path;
}

void Core::logVprintf(enum retro_log_level level, const char* fmt, va_list args)
{
    const char *tag = "???";

    switch (level)
    {
        case RETRO_LOG_DEBUG: tag = "DBG"; break;
        case RETRO_LOG_INFO:  tag = "NFO"; break;
        case RETRO_LOG_WARN:  tag = "WRN"; break;
        case RETRO_LOG_ERROR: tag = "ERR"; break;
    }

    fprintf(stderr, "[%s] ", tag);
    vfprintf(stderr, fmt, args);
}

static size_t addBitsDown(size_t n)
{
    n |= n >>  1;
    n |= n >>  2;
    n |= n >>  4;
    n |= n >>  8;
    n |= n >> 16;

    /* double shift to avoid warnings on 32bit (it's dead code, but compilers suck) */
    if (sizeof(size_t) > 4)
    {
        n |= n >> 16 >> 16;
    }

    return n;
}

static size_t inflate(size_t addr, size_t mask)
{
    while (mask)
    {
        size_t tmp = (mask - 1) & ~mask;
        /* to put in an 1 bit instead, OR in tmp+1 */
        addr = ((addr & ~tmp) << 1) | (addr & tmp);
        mask = mask & (mask - 1);
    }

    return addr;
}

static size_t reduce(size_t addr, size_t mask)
{
    while (mask)
    {
        size_t tmp = (mask - 1) & ~mask;
        addr = (addr & tmp) | ((addr >> 1) & ~tmp);
        mask = (mask & (mask - 1)) >> 1;
    }

    return addr;
}

static size_t highestBit(size_t n)
{
    n = addBitsDown(n);
    return n ^ (n >> 1);
}

bool Core::preprocessMemoryDescriptors()
{
    size_t disconnect_mask;
    size_t top_addr = 1;

    const size_t count = memoryMap.descriptors.size();

    for (size_t i = 0; i < count; i++)
    {
        lrcpp::MemoryDescriptor &desc = memoryMap.descriptors[i];

        if (desc.select != 0)
            top_addr |= desc.select;
        else
            top_addr |= desc.start + desc.len - 1;
    }

    top_addr = addBitsDown(top_addr);

    for (size_t i = 0; i < count; i++)
    {
        lrcpp::MemoryDescriptor &desc = memoryMap.descriptors[i];

        if (desc.select == 0)
        {
            if (desc.len == 0)
                return false;

            if ((desc.len & (desc.len - 1)) != 0)
                return false;

            desc.select = top_addr & ~inflate(addBitsDown(desc.len - 1), desc.disconnect);
        }

        if (desc.len == 0)
            desc.len = addBitsDown(reduce(top_addr & ~desc.select, desc.disconnect)) + 1;

        if (desc.start & ~desc.select)
            return false;

        while (reduce(top_addr & ~desc.select, desc.disconnect) >> 1 > desc.len - 1)
            desc.disconnect |= highestBit(top_addr & ~desc.select & ~desc.disconnect);

        disconnect_mask = addBitsDown(desc.len - 1);
        desc.disconnect &= disconnect_mask;

        while ((~disconnect_mask) >> 1 & desc.disconnect)
        {
            disconnect_mask >>= 1;
            desc.disconnect &= disconnect_mask;
        }
    }

    return true;
}

} // libretro
} // love
