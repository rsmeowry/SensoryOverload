#ifndef SENSORYOVERLOAD_RENDER_SYSTEM_H
#define SENSORYOVERLOAD_RENDER_SYSTEM_H

#include <string>
#include "../world/world.h"
#include "../data/loader.h"

class RenderSystem {
public:
  static void printMessage(const std::string& msg);
  static void printInteraction(const std::string& default_text, GlobalState& state, DataRegistry& registry);
};

#endif // SENSORYOVERLOAD_RENDER_SYSTEM_H
