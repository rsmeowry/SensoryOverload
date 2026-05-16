#ifndef SENSORYOVERLOAD_WORLD_STATE_H
#define SENSORYOVERLOAD_WORLD_STATE_H

#include "../component/component.h"
#include "mob.h"
#include "player.h"

#include "../data/content.h"
#include <vector>

struct MapData {
private:
  std::vector<std::vector<Interactable *>> map_;
public:
  std::vector<LivingMob> mobs_;

  void load(DataRegistry *loader, std::string map);
  Interactable * obj_at(int8_t x, int8_t y) const;
};

struct GlobalState {
  PlayerState player_;
};

#endif // SENSORYOVERLOAD_WORLD_STATE_H
