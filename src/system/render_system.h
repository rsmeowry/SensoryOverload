#ifndef SENSORYOVERLOAD_RENDER_SYSTEM_H
#define SENSORYOVERLOAD_RENDER_SYSTEM_H

#include <string>
#include "../world/world.h"
#include "../data/loader.h"

class RenderSystem {
public:
  static void PrintMessage(const std::string& msg);
  static void PrintInteraction(const std::string& default_text, GlobalState& state, DataRegistry& registry);
};

#endif // SENSORYOVERLOAD_RENDER_SYSTEM_H
