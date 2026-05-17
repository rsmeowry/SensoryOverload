#ifndef SENSORYOVERLOAD_MOB_H
#define SENSORYOVERLOAD_MOB_H

#include "../component/component.h"
#include "../data/content.h"
#include <memory>
#include <vector>

class LivingMob {
public:
  Mob mob_;
  bool alive_ = true;

  std::vector<std::unique_ptr<Component>> components_;

  LivingMob(const Mob &mob) : mob_(mob) {
    auto m = std::make_unique<Moveable>();
    m->x_ = static_cast<uint8_t>(mob.map_x_);
    m->y_ = static_cast<uint8_t>(mob.map_y_);
    m->facing_ = South;
    components_.push_back(std::move(m));

    auto h = std::make_unique<Healable>();
    h->current_ = static_cast<uint8_t>(mob.max_health_);
    h->max_ = static_cast<uint8_t>(mob.max_health_);
    components_.push_back(std::move(h));
  }
  LivingMob(LivingMob &&) = default;
  LivingMob &operator=(LivingMob &&) = default;

  void act(MapData *map, DataRegistry *data) {
    for (auto &&cmp : components_) {
      cmp->Act(this, map, data);
    }
  }
};

#endif // SENSORYOVERLOAD_MOB_H
