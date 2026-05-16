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
  uint8_t size_;

  void load(DataRegistry *loader, std::string map);
  Interactable * objAt(uint8_t x, uint8_t y) const;
  void setObjAt(uint8_t x, uint8_t y, Interactable* obj);
};

struct GlobalState {
  PlayerState player_;
};

#endif // SENSORYOVERLOAD_WORLD_STATE_H
