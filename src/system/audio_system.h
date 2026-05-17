#ifndef SENSORYOVERLOAD_AUDIO_SYSTEM_H
#define SENSORYOVERLOAD_AUDIO_SYSTEM_H

#include <string>
#include <unordered_map>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>
#include "../world/player.h"

class DataRegistry;

class AudioSystem {
public:
  static bool Init();
  static void Cleanup();
  static void Play(const std::string& soundId, const SensorState& sensors, const DataRegistry& registry);

private:
  static ALCdevice* device_;
  static ALCcontext* context_;
  static std::unordered_map<std::string, ALuint> buffer_cache_;
  static std::vector<ALuint> active_sources_;

  static ALuint LoadWav(const std::string& path);
  static void CleanupFinishedSources();
};

#endif // SENSORYOVERLOAD_AUDIO_SYSTEM_H
