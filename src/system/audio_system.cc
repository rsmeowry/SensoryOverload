#include "audio_system.h"
#include "../consts.h"
#include "../data/loader.h"
#include <AL/alext.h>
#include <AL/efx.h>
#include <fstream>
#include <iostream>
#include <cstring>

ALCdevice* AudioSystem::device_ = nullptr;
ALCcontext* AudioSystem::context_ = nullptr;
std::unordered_map<std::string, ALuint> AudioSystem::buffer_cache_;
std::vector<ALuint> AudioSystem::active_sources_;

// efx functions
static LPALGENFILTERS al_gen_filters = nullptr;
static LPALDELETEFILTERS al_delete_filters = nullptr;
static LPALFILTERI al_filter_i = nullptr;
static LPALFILTERF al_filter_f = nullptr;
static bool exf_available = false;

bool AudioSystem::Init() {
  device_ = alcOpenDevice(nullptr);
  if (!device_) {
    std::cerr << "NO AUDIO DEVICE!!" << std::endl;
    return false;
  }

  context_ = alcCreateContext(device_, nullptr);
  if (!context_ || !alcMakeContextCurrent(context_)) {
    std::cerr << "device error :(" << std::endl;
    if (context_) alcDestroyContext(context_);
    alcCloseDevice(device_);
    device_ = nullptr;
    context_ = nullptr;
    return false;
  }

  // loading efx extensions for filters
  if (alcIsExtensionPresent(device_, "ALC_EXT_EFX")) {
    al_gen_filters = reinterpret_cast<LPALGENFILTERS>(alGetProcAddress("alGenFilters"));
    al_delete_filters = reinterpret_cast<LPALDELETEFILTERS>(alGetProcAddress("alDeleteFilters"));
    al_filter_i = reinterpret_cast<LPALFILTERI>(alGetProcAddress("alFilteri"));
    al_filter_f = reinterpret_cast<LPALFILTERF>(alGetProcAddress("alFilterf"));
    exf_available = (al_gen_filters && al_delete_filters && al_filter_i && al_filter_f);
  }

  return true;
}

void AudioSystem::Cleanup() {
  for (auto src : active_sources_) {
    alSourceStop(src);
    alDeleteSources(1, &src);
  }
  active_sources_.clear();

  for (auto& [id, buf] : buffer_cache_) {
    alDeleteBuffers(1, &buf);
  }
  buffer_cache_.clear();

  if (context_) {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context_);
    context_ = nullptr;
  }
  if (device_) {
    alcCloseDevice(device_);
    device_ = nullptr;
  }
}

// simple wav loader
ALuint AudioSystem::LoadWav(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) return 0;

  // check magic values + header
  char riff[4];
  file.read(riff, 4);
  if (std::strncmp(riff, "RIFF", 4) != 0) return 0;

  uint32_t file_size;
  file.read(reinterpret_cast<char*>(&file_size), 4);

  char wave[4];
  file.read(wave, 4);
  if (std::strncmp(wave, "WAVE", 4) != 0) return 0;

  // parse format
  uint16_t audio_format = 0, num_channels = 0, bits_per_sample = 0;
  uint32_t sample_rate = 0;

  while (file.good()) {
    char chunk_id[4];
    uint32_t chunk_size;
    file.read(chunk_id, 4);
    file.read(reinterpret_cast<char*>(&chunk_size), 4);

    if (std::strncmp(chunk_id, "fmt ", 4) == 0) {
      file.read(reinterpret_cast<char*>(&audio_format), 2);
      file.read(reinterpret_cast<char*>(&num_channels), 2);
      file.read(reinterpret_cast<char*>(&sample_rate), 4);
      uint32_t byte_rate;
      file.read(reinterpret_cast<char*>(&byte_rate), 4);
      uint16_t block_align;
      file.read(reinterpret_cast<char*>(&block_align), 2);
      file.read(reinterpret_cast<char*>(&bits_per_sample), 2);

      // skipping extra format stuff
      if (chunk_size > 16) {
        file.seekg(chunk_size - 16, std::ios::cur);
      }
    } else if (std::strncmp(chunk_id, "data", 4) == 0) {
      // audio buffer
      if (audio_format != 1) return 0; // if not pcm we skip

      ALenum format;
      if (num_channels == 1 && bits_per_sample == 8) format = AL_FORMAT_MONO8;
      else if (num_channels == 1 && bits_per_sample == 16) format = AL_FORMAT_MONO16;
      else if (num_channels == 2 && bits_per_sample == 8) format = AL_FORMAT_STEREO8;
      else if (num_channels == 2 && bits_per_sample == 16) format = AL_FORMAT_STEREO16;
      else return 0;

      std::vector<char> data(chunk_size);
      file.read(data.data(), chunk_size);

      ALuint buffer;
      alGenBuffers(1, &buffer);
      alBufferData(buffer, format, data.data(), chunk_size, sample_rate);

      if (alGetError() != AL_NO_ERROR) {
        alDeleteBuffers(1, &buffer);
        return 0;
      }

      return buffer;
    } else {
      // if chunk is wrong/unknown
      file.seekg(chunk_size, std::ios::cur);
    }
  }

  return 0;
}

void AudioSystem::CleanupFinishedSources() {
  auto it = active_sources_.begin();
  while (it != active_sources_.end()) {
    ALint state;
    alGetSourcei(*it, AL_SOURCE_STATE, &state);
    if (state == AL_STOPPED) {
      alDeleteSources(1, &*it);
      it = active_sources_.erase(it);
    } else {
      ++it;
    }
  }
}

void AudioSystem::Play(const std::string& soundId, const SensorState& sensors, const DataRegistry& registry) {
  if (soundId.empty() || !device_) return;

  CleanupFinishedSources();

  // loading buffer (or getting cached buffer)
  ALuint buffer;
  auto cache_it = buffer_cache_.find(soundId);
  if (cache_it != buffer_cache_.end()) {
    buffer = cache_it->second;
  } else {
    std::string path = kRootDir + "assets/" + soundId + ".wav";
    buffer = LoadWav(path);
    if (buffer == 0) { // couldnt load
      return;
    }
    buffer_cache_[soundId] = buffer;
  }

  // creating audio sourc
  ALuint source;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);

  // effect modulation
  if (!sensors.current_effect_.empty()) {
    auto effect_it = registry.effects_.find(sensors.current_effect_);
    if (effect_it != registry.effects_.end()) {
      const Effect* effect = effect_it->second;

      // pitch
      if (effect->sound_pitch_ > 0) {
        alSourcef(source, AL_PITCH, effect->sound_pitch_);
      }

      // lowpass
      if (exf_available && effect->lowpass_param_ > 0) {
        ALuint filter;
        al_gen_filters(1, &filter);
        al_filter_i(filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        al_filter_f(filter, AL_LOWPASS_GAIN, effect->lowpass_param_);
        al_filter_f(filter, AL_LOWPASS_GAINHF, effect->lowpass_param_);
        alSourcei(source, AL_DIRECT_FILTER, filter);
        al_delete_filters(1, &filter);
      }
    }
  }

  alSourcePlay(source);
  active_sources_.push_back(source);
}
