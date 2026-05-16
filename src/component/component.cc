#include "component.h"

#include <iostream>
#include "../world/mob.h"

void Moveable::act(LivingMob *self, MapData *map, DataRegistry *data) {
  std::cout << "moved moveable" << std::endl;
}

void Healable::act(LivingMob *self, MapData *map, DataRegistry *data) {
  std::cout << "acted healable" << static_cast<int>(this->current_) << std::endl;
  self->alive_ = this->current_ > 0;
}