#ifndef SENSORYOVERLOAD_AUDIO_SYSTEM_H
#define SENSORYOVERLOAD_AUDIO_SYSTEM_H

#include <string>
#include "../world/player.h"

class AudioSystem {
public:
  static void play(const std::string& soundId, const SensorState& sensors);
};

#endif // SENSORYOVERLOAD_AUDIO_SYSTEM_H
