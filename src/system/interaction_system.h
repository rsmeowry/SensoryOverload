#ifndef SENSORYOVERLOAD_INTERACTION_SYSTEM_H
#define SENSORYOVERLOAD_INTERACTION_SYSTEM_H

#include "../data/loader.h"
#include "../world/world.h"

class InteractionSystem {
public:
  static void OnInteract(GlobalState &state, MapData &map,
                         DataRegistry &registry);
};

#endif // SENSORYOVERLOAD_INTERACTION_SYSTEM_H
