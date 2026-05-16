#ifndef SENSORYOVERLOAD_WORLD_STATE_H
#define SENSORYOVERLOAD_WORLD_STATE_H

#include <cstdint>
#include <vector>
#include "../data/content.h"
#include "../data/loader.h"

class LivingMob {
public:
  int8_t x_;
  int8_t y_;
  Mob mob_;

  void act();
};

struct MapData {
private:
  std::vector<std::vector<Interactable *>> map_;
public:
  std::vector<LivingMob> mobs_;

  void load(DataRegistry *loader, std::string map);
  Interactable * obj_at(int8_t x, int8_t y) const;
};

#endif // SENSORYOVERLOAD_WORLD_STATE_H
