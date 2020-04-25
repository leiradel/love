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
#include <sys/stat.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

// LOVE
#include "CoreDll.h"

namespace love
{
namespace libretro
{

#define CORE_DLSYM(prop, name) \
  do { \
    void *sym = dynlib_symbol(handle, name); \
    if (!sym) goto error; \
    memcpy(&prop, &sym, sizeof(prop)); \
  } while (0)

CoreDll::CoreDll(const std::string &path)
{
    handle = dynlib_open(path.c_str());

    if (handle == NULL)
    {
    error:
        char message[128];
        strncpy(message, dynlib_error(), sizeof(message));
        message[sizeof(message) - 1] = 0;

        if (handle != NULL)
            dynlib_close(handle);
      
        throw love::Exception("Error loading libretro core: %s", message);
    }

    CORE_DLSYM(_init, "retro_init");
    CORE_DLSYM(_deinit, "retro_deinit");
    CORE_DLSYM(_apiVersion, "retro_api_version");
    CORE_DLSYM(_getSystemInfo, "retro_get_system_info");
    CORE_DLSYM(_getSystemAVInfo, "retro_get_system_av_info");
    CORE_DLSYM(_setEnvironment, "retro_set_environment");
    CORE_DLSYM(_setVideoRefresh, "retro_set_video_refresh");
    CORE_DLSYM(_setAudioSample, "retro_set_audio_sample");
    CORE_DLSYM(_setAudioSampleBatch, "retro_set_audio_sample_batch");
    CORE_DLSYM(_setInputPoll, "retro_set_input_poll");
    CORE_DLSYM(_setInputState, "retro_set_input_state");
    CORE_DLSYM(_setControllerPortDevice, "retro_set_controller_port_device");
    CORE_DLSYM(_reset, "retro_reset");
    CORE_DLSYM(_run, "retro_run");
    CORE_DLSYM(_serializeSize, "retro_serialize_size");
    CORE_DLSYM(_serialize, "retro_serialize");
    CORE_DLSYM(_unserialize, "retro_unserialize");
    CORE_DLSYM(_cheatReset, "retro_cheat_reset");
    CORE_DLSYM(_cheatSet, "retro_cheat_set");
    CORE_DLSYM(_loadGame, "retro_load_game");
    CORE_DLSYM(_loadGameSpecial, "retro_load_game_special");
    CORE_DLSYM(_unloadGame, "retro_unload_game");
    CORE_DLSYM(_getRegion, "retro_get_region");
    CORE_DLSYM(_getMemoryData, "retro_get_memory_data");
    CORE_DLSYM(_getMemorySize, "retro_get_memory_size");
}

#undef CORE_DLSYM

CoreDll::~CoreDll()
{
    dynlib_close(handle);
}

void CoreDll::init() const
{
  _init();
}

void CoreDll::deinit() const
{
    _deinit();
}

unsigned CoreDll::apiVersion() const
{
    return _apiVersion();
}

void CoreDll::getSystemInfo(struct retro_system_info* info) const
{
    _getSystemInfo(info);
}

void CoreDll::getSystemAVInfo(struct retro_system_av_info* info) const
{
    _getSystemAVInfo(info);
}

void CoreDll::setEnvironment(retro_environment_t cb) const
{
    _setEnvironment(cb);
}

void CoreDll::setVideoRefresh(retro_video_refresh_t cb) const
{
    _setVideoRefresh(cb);
}

void CoreDll::setAudioSample(retro_audio_sample_t cb) const
{
    _setAudioSample(cb);
}

void CoreDll::setAudioSampleBatch(retro_audio_sample_batch_t cb) const
{
    _setAudioSampleBatch(cb);
}

void CoreDll::setInputPoll(retro_input_poll_t cb) const
{
    _setInputPoll(cb);
}

void CoreDll::setInputState(retro_input_state_t cb) const
{
    _setInputState(cb);
}

void CoreDll::setControllerPortDevice(unsigned port, unsigned device) const
{
    _setControllerPortDevice(port, device);
}

void CoreDll::reset() const
{
    _reset();
}

void CoreDll::run() const
{
    _run();
}

size_t CoreDll::serializeSize() const
{
    return _serializeSize();
}

bool CoreDll::serialize(void* data, size_t size) const
{
    return _serialize(data, size);
}

bool CoreDll::unserialize(const void* data, size_t size) const
{
    return _unserialize(data, size);
}

void CoreDll::cheatReset() const
{
    _cheatReset();
}

void CoreDll::cheatSet(unsigned index, bool enabled, const char* code) const
{
    _cheatSet(index, enabled, code);
}

bool CoreDll::loadGame(const struct retro_game_info* game) const
{
    return _loadGame(game);
}

bool CoreDll::loadGameSpecial(unsigned game_type, const struct retro_game_info* info, size_t num_info) const
{
    return _loadGameSpecial(game_type, info, num_info);
}

void CoreDll::unloadGame() const
{
    _unloadGame();
}

unsigned CoreDll::getRegion() const
{
    return _getRegion();
}

void* CoreDll::getMemoryData(unsigned id) const
{
    return _getMemoryData(id);
}

size_t CoreDll::getMemorySize(unsigned id) const
{
    return _getMemorySize(id);
}

} // libretro
} // love
