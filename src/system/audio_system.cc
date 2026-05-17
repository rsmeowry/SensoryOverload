#include "audio_system.h"
#include <iostream>

void AudioSystem::play(const std::string& soundId, const SensorState& sensors) {
  if (soundId.empty()) return;
  
  std::cout << "SOUNDDD " << soundId;
  if (!sensors.current_effect_.empty()) {
    std::cout << " WITH " << sensors.current_effect_ << std::endl;
  }
}
