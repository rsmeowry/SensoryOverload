#ifndef SENSORYOVERLOAD_RENDER_SYSTEM_H
#define SENSORYOVERLOAD_RENDER_SYSTEM_H

#include "../data/loader.h"
#include "../world/world.h"
#include <string>

class RenderSystem {
public:
  static void PrintMessage(const std::string &msg);
  static void PrintInteraction(const std::string &default_text,
                               GlobalState &state, DataRegistry &registry);
};

#endif // SENSORYOVERLOAD_RENDER_SYSTEM_H
