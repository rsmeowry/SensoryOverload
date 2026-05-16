#ifndef SENSORYOVERLOAD_MOB_SYSTEM_H
#define SENSORYOVERLOAD_MOB_SYSTEM_H

#include "../world/world.h"
#include "../data/loader.h"

class MobSystem {
public:
  static void update(MapData& map, GlobalState& state, DataRegistry& registry);
};

#endif // SENSORYOVERLOAD_MOB_SYSTEM_H
