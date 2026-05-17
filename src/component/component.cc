#include "component.h"

#include "../world/mob.h"
#include <iostream>

void Moveable::Act(LivingMob *self, MapData *map, DataRegistry *data) {}

void Healable::Act(LivingMob *self, MapData *map, DataRegistry *data) {
  self->alive_ = this->current_ > 0;
}