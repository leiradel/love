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

    pixelFormat = RETRO_PIXEL_FORMAT_UNKNOWN;
    scratchBuffer = nullptr;
    bpp = 0;
    image = nullptr;

    samplesCount = 0;
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
    ports = {};
    diskControlInterfaceSet = false;
    diskControlInterface = {};
    memoryMap = {};

    image = nullptr;

    struct retro_system_info system_info;
    core.getSystemInfo(&system_info);

    systemInfo.libraryName = system_info.library_name;
    systemInfo.libraryVersion = system_info.library_version;
    systemInfo.validExtensions = system_info.valid_extensions != NULL ? system_info.valid_extensions : "";
    systemInfo.needFullPath  = system_info.need_fullpath;
    systemInfo.blockExtract = system_info.block_extract;

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
                hardwareRenderCallback.context_reset();
            }

            core.setVideoRefresh(staticVideoRefreshCallback);
            core.setAudioSampleBatch(staticAudioSampleBatchCallback);
            core.setAudioSample(staticAudioSampleCallback);
            core.setInputState(staticInputStateCallback);
            core.setInputPoll(staticInputPollCallback);

            core.getSystemAVInfo(&systemAVInfo);

            if (systemAVInfo.geometry.aspect_ratio <= 0.0f)
                systemAVInfo.geometry.aspect_ratio = (float)systemAVInfo.geometry.base_width /
                                                     (float)systemAVInfo.geometry.base_height;

            const struct retro_hw_render_callback *cb = needsHardwareRender ? &hardwareRenderCallback : NULL;

            videoSetGeometry(systemAVInfo.geometry.base_width,
                             systemAVInfo.geometry.base_height,
                             systemAVInfo.geometry.aspect_ratio,
                             pixelFormat,
                             cb);

            audioSetRate(systemAVInfo.timing.sample_rate);

            for (size_t i = 0; i < controllerInfo.size(); i++)
            {
                core.setControllerPortDevice(i, RETRO_DEVICE_NONE);
            }
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
    core.deinit();
    delete[] scratchBuffer;
}

love::StrongRef<love::graphics::Image> &Core::getImage()
{
    return image;
}

float Core::getAspectRatio() const
{
    return systemAVInfo.geometry.aspect_ratio;
}

void Core::step()
{
    InstanceSetter instance_setter(this);

    if (inputCtrlUpdated())
    {
        for (unsigned i = 0; i < controllerInfo.size(); i++)
        {
            unsigned device = inputGetController(i);

            if (device != ports[i])
            {
                ports[i] = device;
                core.setControllerPortDevice(i, device);
            }
        }
    }

    samplesCount = 0;
    core.run();

    if (samplesCount > 0)
        audioMix(samples, samplesCount / 2);
}

unsigned Core::getApiVersion()
{
    InstanceSetter instance_setter(this);
    return core.apiVersion();
}

unsigned Core::getRegion()
{
    InstanceSetter instance_setter(this);
    return core.getRegion();
}

void* Core::getMemoryData(unsigned id)
{
    InstanceSetter instance_setter(this);
    return core.getMemoryData(id);
}

size_t Core::getMemorySize(unsigned id)
{
    InstanceSetter instance_setter(this);
    return core.getMemorySize(id);
}

void Core::resetGame()
{
    InstanceSetter instance_setter(this);
    core.reset();
}

size_t Core::serializeSize()
{
    InstanceSetter instance_setter(this);
    return core.serializeSize();
}

bool Core::serialize(void *data, size_t size)
{
    InstanceSetter instance_setter(this);
    return core.serialize(data, size);
}

bool Core::unserialize(const void *data, size_t size)
{
    InstanceSetter instance_setter(this);
    return core.unserialize(data, size);
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

void Core::setTrayOpen(bool open)
{
    InstanceSetter instance_setter(this);

    if (diskControlInterfaceSet)
        diskControlInterface.set_eject_state(open);
}

void Core::setCurrentDiscIndex(unsigned index)
{
    InstanceSetter instance_setter(this);

    if (diskControlInterfaceSet)
        diskControlInterface.set_image_index(index);
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
        case RETRO_PIXEL_FORMAT_XRGB8888:
        case RETRO_PIXEL_FORMAT_RGB565:
            pixelFormat = data;
            return true;

        case RETRO_PIXEL_FORMAT_UNKNOWN:
        default:
            pixelFormat = RETRO_PIXEL_FORMAT_RGB565;
            return false;
    }
}

bool Core::setInputDescriptors(const struct retro_input_descriptor *data)
{
    const struct retro_input_descriptor *desc;

    for (desc = data; desc->description != NULL; desc++) /* Just count */;

    const size_t count = desc - data;
    inputDescriptors.resize(count);

    for (size_t i = 0; i < count; i++)
    {
        inputDescriptors[i].port = data[i].port;
        inputDescriptors[i].device = data[i].device;
        inputDescriptors[i].index = data[i].index;
        inputDescriptors[i].id = data[i].id;
        inputDescriptors[i].description = data[i].description;
    }
    
    inputSetInputDescriptors(inputDescriptors);
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
    data->value = configGetVariable(data->key).c_str();
    return true;
}

bool Core::setVariables(const struct retro_variable *data)
{
    const struct retro_variable *var;

    for (var = data; var->key != NULL; var++) /* Just count */;

    const size_t count = var - data;
    variables.resize(count);

    for (size_t i = 0; i < count; i++)
    {
        variables[i].key = data[i].key;
        variables[i].value = data[i].value;
    }

    configSetVariables(variables);
    return true;
}

bool Core::getVariableUpdate(bool *data)
{
    *data = configVarUpdated();
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
    (void)data;
    return false;
}

bool Core::setAudioCallback(const struct retro_audio_callback *data)
{
    (void)data;
    return false;
}

bool Core::getRumbleInterface(struct retro_rumble_interface *data) const
{
    (void)data;
    return false;
}

bool Core::getInputDeviceCapabilities(uint64_t *data) const
{
    *data = (1 << RETRO_DEVICE_JOYPAD) | (1 << RETRO_DEVICE_ANALOG) | (1 << RETRO_DEVICE_MOUSE);
    return false;
}

bool Core::getSensorInterface(struct retro_sensor_interface *data) const
{
    (void)data;
    return false;
}

bool Core::getCameraInterface(struct retro_camera_callback *data) const
{
    (void)data;
    return false;
}

bool Core::getLogInterface(struct retro_log_callback *data) const
{
  data->log = staticLogCallback;
  return true;
}

bool Core::getPerfInterface(struct retro_perf_callback *data) const
{
    (void)data;
    return false;
}

bool Core::getLocationInterface(struct retro_location_callback *data) const
{
    (void)data;
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

    if (systemAVInfo.geometry.aspect_ratio <= 0.0f)
        systemAVInfo.geometry.aspect_ratio = (float)systemAVInfo.geometry.base_width /
                                             (float)systemAVInfo.geometry.base_height;

    const struct retro_hw_render_callback *cb = needsHardwareRender ? &hardwareRenderCallback : NULL;

    try
    {
        videoSetGeometry(systemAVInfo.geometry.base_width,
                         systemAVInfo.geometry.base_height,
                         systemAVInfo.geometry.aspect_ratio,
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
  (void)data;
  return false;
}

bool Core::setSubsystemInfo(const struct retro_subsystem_info *data)
{
    const struct retro_subsystem_info *info;

    for (info = data; info->desc != NULL; info++) /* Just count */;

    const size_t count = info - data;
    subsystemInfo.resize(count);

    for (size_t i = 0; i < count; i++)
    {
        subsystemInfo[i].desc = data[i].desc;
        subsystemInfo[i].ident = data[i].ident;
        subsystemInfo[i].id = data[i].id;
        subsystemInfo[i].roms.resize(data[i].num_roms);

        for (size_t j = 0; j < data[i].num_roms; j++)
        {
            subsystemInfo[i].roms[j].desc = data[i].roms[j].desc;
            subsystemInfo[i].roms[j].validExtensions = data[i].roms[j].valid_extensions;
            subsystemInfo[i].roms[j].needFullpath = data[i].roms[j].need_fullpath;
            subsystemInfo[i].roms[j].blockExtract = data[i].roms[j].block_extract;
            subsystemInfo[i].roms[j].required = data[i].roms[j].required;
            subsystemInfo[i].roms[j].memory.resize(data[i].roms[j].num_memory);

            for (size_t k = 0; k < data[i].roms[j].num_memory; k++)
            {
                subsystemInfo[i].roms[j].memory[k].extension = data[i].roms[j].memory[k].extension;
            }
        }
    }

    return true;
}

bool Core::setControllerInfo(const struct retro_controller_info *data)
{
    const struct retro_controller_info *info;

    for (info = data; info->types != NULL; info++) /* Just count */;

    const size_t count = info - data;
    controllerInfo.resize(count);
    ports.resize(count);

    for (size_t i = 0; i < count; i++)
    {
        controllerInfo[i].types.resize(data[i].num_types);

        for (size_t j = 0; j < data[i].num_types; j++)
        {
            controllerInfo[i].types[j].desc = data[i].types[j].desc;
            controllerInfo[i].types[j].id = data[i].types[j].id;
        }

        ports[i] = RETRO_DEVICE_NONE;
    }

    return true;
}

bool Core::setMemoryMaps(const struct retro_memory_map *data)
{
    memoryMap.descriptors.resize(data->num_descriptors);

    for (size_t i = 0; i < data->num_descriptors; i++)
    {
        memoryMap.descriptors[i].flags = data->descriptors[i].flags;
        memoryMap.descriptors[i].ptr = data->descriptors[i].ptr;
        memoryMap.descriptors[i].offset = data->descriptors[i].offset;
        memoryMap.descriptors[i].start = data->descriptors[i].start;
        memoryMap.descriptors[i].select = data->descriptors[i].select;
        memoryMap.descriptors[i].disconnect = data->descriptors[i].disconnect;
        memoryMap.descriptors[i].len = data->descriptors[i].len;

        if (data->descriptors[i].addrspace)
            memoryMap.descriptors[i].addrspace = data->descriptors[i].addrspace;
        else
            memoryMap.descriptors[i].addrspace.clear();
    }

    preprocessMemoryDescriptors();

  return true;
}

bool Core::setGeometry(const struct retro_game_geometry *data)
{
    systemAVInfo.geometry.base_width = data->base_width;
    systemAVInfo.geometry.base_height = data->base_height;
    systemAVInfo.geometry.aspect_ratio = data->aspect_ratio;

    if (systemAVInfo.geometry.aspect_ratio <= 0.0f)
        systemAVInfo.geometry.aspect_ratio = (float)systemAVInfo.geometry.base_width /
                                             (float)systemAVInfo.geometry.base_height;

    const struct retro_hw_render_callback *cb = needsHardwareRender ? &hardwareRenderCallback : NULL;

    try
    {
        videoSetGeometry(systemAVInfo.geometry.base_width,
                         systemAVInfo.geometry.base_height,
                         systemAVInfo.geometry.aspect_ratio,
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
    (void)data;
    return false;
}

bool Core::getLanguage(unsigned *data) const
{
    *data = RETRO_LANGUAGE_ENGLISH;
    return true;
}

bool Core::getCurrentSoftwareFramebuffer(struct retro_framebuffer *data) const
{
    (void)data;
    return false;
}

bool Core::getHWRenderInterface(const struct retro_hw_render_interface **data) const
{
    (void)data;
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
        memcpy(samples + samplesCount, data, frames * 2 * sizeof(int16_t));
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
    return inputRead(port, device, index, id);
}

void Core::inputPollCallback()
{
    return inputPoll();
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

bool Core::inputCtrlUpdated()
{
    // TODO implement me!
    return false;
}

unsigned Core::inputGetController(unsigned port)
{
    // TODO implement me!
    return RETRO_DEVICE_NONE;
}

void Core::inputSetInputDescriptors(const std::vector<InputDescriptor> &descs)
{
    // TODO implement me!
}

int16_t Core::inputRead(unsigned port, unsigned device, unsigned index, unsigned id)
{
    // TODO implement me!
    return 0;
}

void Core::inputPoll()
{
    // TODO implement me!
}

void Core::audioSetRate(double rate)
{
    // TODO implement me!
}

void Core::audioMix(const int16_t *samples, size_t frames)
{
    // TODO implement me!
}

void Core::videoSetGeometry(unsigned width, unsigned height, float aspect, enum retro_pixel_format pixelFormat,
                                    const HWRenderCallback* hwRenderCallback)
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
    if (!image || image->getWidth() != width || image->getHeight() != height)
    {
        love::PixelFormat format;

        switch (pixelFormat)
        {
            case RETRO_PIXEL_FORMAT_XRGB8888:
                format = PIXELFORMAT_RGBA8;
                bpp = 4;
                break;

            case RETRO_PIXEL_FORMAT_RGB565:
                format = PIXELFORMAT_RGB565;
                bpp = 2;
                break;

            default: // fallthrough
            case RETRO_PIXEL_FORMAT_0RGB1555:
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

    if (pixelFormat != RETRO_PIXEL_FORMAT_XRGB8888)
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

const std::string &Core::configGetVariable(const std::string &variable)
{
    // TODO implement me!
    static const std::string value = "";
    return value;
}

void Core::configSetVariables(const std::vector<Variable> &variables)
{
    // TODO implement me!
}

bool Core::configVarUpdated()
{
    // TODO implement me!
    return false;
}

void Core::logVprintf(enum retro_log_level level, const char* fmt, va_list args)
{
    // TODO implement me!
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
        MemoryDescriptor &desc = memoryMap.descriptors[i];

        if (desc.select != 0)
            top_addr |= desc.select;
        else
            top_addr |= desc.start + desc.len - 1;
    }

    top_addr = addBitsDown(top_addr);

    for (size_t i = 0; i < count; i++)
    {
        MemoryDescriptor &desc = memoryMap.descriptors[i];

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
