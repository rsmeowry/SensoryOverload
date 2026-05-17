#ifndef SENSORYOVERLOAD_MOB_SYSTEM_H
#define SENSORYOVERLOAD_MOB_SYSTEM_H

#include "../data/loader.h"
#include "../world/world.h"

class MobSystem {
public:
  static void Update(MapData &map, GlobalState &state, DataRegistry &registry);
};

#endif // SENSORYOVERLOAD_MOB_SYSTEM_H
